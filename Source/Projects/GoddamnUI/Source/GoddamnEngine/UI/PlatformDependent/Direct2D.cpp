#include "Direct2D.h"

#include <d2d1_1helper.h>
#include <dwrite.h>

#include <math.h>

#include "ComInterop.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "dxguid.lib")

namespace Engine
{
	namespace Direct2D
	{
		class D2DTexture;
		class D2DFont;
		class D2DRenderDevice;

		ID2D1Factory1* D2DFactory = 0;
		IWICImagingFactory* WICFactory = 0;
		IDWriteFactory* DWriteFactory = 0;

		void InitializeFactory()
		{
			if (!D2DFactory)
			{
				if (D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &D2DFactory) != S_OK)
					throw Exception();
			}
			if (!WICFactory)
			{
				if (CoCreateInstance(CLSID_WICImagingFactory, 0, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&WICFactory)) != S_OK)
					throw Exception();
			}
			if (!DWriteFactory)
			{
				if (DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&DWriteFactory)) != S_OK)
					throw Exception();
			}
		}
		void ShutdownFactory()
		{
			if (D2DFactory)
			{
				D2DFactory->Release();
				D2DFactory = 0;
			}
			if (WICFactory)
			{
				WICFactory->Release();
				WICFactory = 0;
			}
			if (DWriteFactory)
			{
				DWriteFactory->Release();
				DWriteFactory = 0;
			}
		}

		struct BarRenderingInfo : public IBarRenderingInfo
		{
			ID2D1GradientStopCollection* Collection = 0;
			ID2D1SolidColorBrush* Brush = 0;
			Float64 prop_h = 0.0, prop_w = 0.0;
			~BarRenderingInfo() override
			{
				if (Collection)
					Collection->Release();
				if (Brush)
					Brush->Release();
			}
		};
		struct TextureRenderingInfo : public ITextureRenderingInfo
		{
			D2DTexture* Texture;
			Box From;
			Array<ID2D1BitmapBrush*> Brushes;
			bool Fill;
			TextureRenderingInfo()
			    : Brushes()
			{
			}
			~TextureRenderingInfo() override
			{
				for (Int32 i = 0; i < Brushes.GetLength(); i++)
					Brushes[i]->Release();
			}
		};
		struct TextRenderingInfo : public ITextRenderingInfo
		{
			D2DFont* Font;
			Array<UInt32> CharString;
			Array<UInt16> GlyphString;
			Array<Float32> GlyphAdvances;
			Array<UInt8> UseAlternative;
			ID2D1PathGeometry* Geometry;
			ID2D1SolidColorBrush* TextBrush;
			ID2D1SolidColorBrush* HighlightBrush;
			ID2D1RenderTarget* RenderTarget;
			UInt16 NormalSpaceGlyph;
			UInt16 AlternativeSpaceGlyph;
			Int32 BaselineOffset;
			Int32 halign, valign;
			Int32 run_length;
			Float32 UnderlineOffset;
			Float32 UnderlineHalfWidth;
			Float32 StrikeoutOffset;
			Float32 StrikeoutHalfWidth;
			Int32 hls, hle;
			SafePointer<BarRenderingInfo> MainBrushInfo;
			SafePointer<BarRenderingInfo> BackBrushInfo;

			TextRenderingInfo()
			    : CharString()
			    , GlyphString()
			    , GlyphAdvances()
			    , UseAlternative()
			    , hls(-1)
			    , hle(-1)
			{
			}

			void FillAdvances();
			void FillGlyphs();
			void BuildGeometry();
			Float32 FontUnitsToDIP(Int32 units);
			Float32 AltFontUnitsToDIP(Int32 units);

			virtual void GetExtent(Int32& width, Int32& height) override;
			virtual void SetHighlightColor(const Color& color) override
			{
				if (HighlightBrush)
				{
					HighlightBrush->Release();
					HighlightBrush = 0;
				}
				RenderTarget->CreateSolidColorBrush(D2D1::ColorF(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f), &HighlightBrush);
			}
			virtual void HighlightText(Int32 Start, Int32 End) override
			{
				hls = Start;
				hle = End;
			}
			virtual Int32 TestPosition(Int32 point) override
			{
				if (point < 0)
					return 0;
				if (point > run_length)
					return CharString.GetLength();
				Float32 p = Float32(point);
				Float32 s = 0.0f;
				for (Int32 i = 0; i < GlyphAdvances.GetLength(); i++)
				{
					if (p >= s)
					{
						if (p < s + GlyphAdvances[i] / 2.0f)
							return i;
						else
							return i + 1;
						break;
					}
					s += GlyphAdvances[i];
				}
				return CharString.GetLength();
			}
			virtual Int32 EndOfChar(Int32 Index) override
			{
				if (Index < 0)
					return 0;
				Float32 summ = 0;
				for (Int32 i = 0; i <= Index; i++)
					summ += GlyphAdvances[i];
				return Int32(summ);
			}
			virtual ~TextRenderingInfo() override
			{
				if (Geometry)
					Geometry->Release();
				if (TextBrush)
					TextBrush->Release();
				if (HighlightBrush)
					HighlightBrush->Release();
			}
		};
		struct LineRenderingInfo : public ILineRenderingInfo
		{
			ID2D1SolidColorBrush* Brush;
			ID2D1StrokeStyle* Stroke;
			SafePointer<BarRenderingInfo> BrushInfo;
			virtual ~LineRenderingInfo() override
			{
				if (Brush)
					Brush->Release();
				if (Stroke)
					Stroke->Release();
			}
		};
		struct BlurEffectRenderingInfo : public IBlurEffectRenderingInfo
		{
			SafePointer<ID2D1Effect> Effect;
			virtual ~BlurEffectRenderingInfo() override
			{
			}
		};
		struct InversionEffectRenderingInfo : public IInversionEffectRenderingInfo
		{
			SafePointer<ID2D1Effect> Effect;
			virtual ~InversionEffectRenderingInfo() override
			{
			}
		};
		class D2DTexture : public ITexture
		{
		public:
			Array<ID2D1Bitmap*> Frames;
			Array<UInt32> FrameDuration;
			UInt32 TotalDuration;
			Int32 w, h;
			D2DTexture()
			    : Frames()
			{
			}
			virtual ~D2DTexture()
			{
				for (Int32 i = 0; i < Frames.GetLength(); i++)
					Frames[i]->Release();
			}
			virtual Int32 GetWidth() const override
			{
				return w;
			}
			virtual Int32 GetHeight() const override
			{
				return h;
			}
			virtual bool IsDynamic() const override
			{
				return false;
			}
			virtual void Reload(IRenderingDevice* Device, Streaming::Stream* Source) override
			{
				for (Int32 i = 0; i < Frames.GetLength(); i++)
					Frames[i]->Release();
				auto New = static_cast<D2DTexture*>(Device->LoadTexture(Source));
				Frames = New->Frames;
				FrameDuration = New->FrameDuration;
				TotalDuration = New->TotalDuration;
				w = New->w;
				h = New->h;
				New->Frames.Clear();
				New->Release();
			}
			virtual string ToString() const override
			{
				return L"D2DTexture";
			}
		};
		class D2DFont : public IFont
		{
		public:
			IDWriteFontFace* FontFace;
			IDWriteFontFace* AlternativeFace;
			string FontName;
			Int32 Height, Weight;
			Int32 ActualHeight;
			bool Italic, Underline, Strikeout;
			DWRITE_FONT_METRICS FontMetrics;
			DWRITE_FONT_METRICS AltMetrics;
			virtual ~D2DFont() override
			{
				if (FontFace)
					FontFace->Release();
				if (AlternativeFace)
					AlternativeFace->Release();
			}
			virtual void Reload(IRenderingDevice* Device) override
			{
				if (FontFace)
					FontFace->Release();
				if (AlternativeFace)
					AlternativeFace->Release();
				auto New = static_cast<D2DFont*>(Device->LoadFont(FontName, ActualHeight, Weight, Italic, Underline, Strikeout));
				FontFace = New->FontFace;
				FontMetrics = New->FontMetrics;
				AlternativeFace = New->AlternativeFace;
				New->FontFace = 0;
				New->AlternativeFace = 0;
				New->Release();
			}
			virtual string ToString() const override
			{
				return L"D2DFont";
			}
		};

		D2DRenderDevice::D2DRenderDevice(ID2D1DeviceContext* target)
		    : Target(target)
		    , Layers(0x10)
		    , BrushCache(0x100, Dictionary::ExcludePolicy::ExcludeLeastRefrenced)
		    , BlurCache(0x10, Dictionary::ExcludePolicy::ExcludeLeastRefrenced)
		{
			Target->AddRef();
			HalfBlinkPeriod = GetCaretBlinkTime();
			BlinkPeriod = HalfBlinkPeriod * 2;
		}
		D2DRenderDevice::~D2DRenderDevice()
		{
			Target->Release();
		}
		ID2D1RenderTarget* D2DRenderDevice::GetRenderTarget() const
		{
			return Target;
		}
		IBarRenderingInfo* D2DRenderDevice::CreateBarRenderingInfo(const Array<GradientPoint>& gradient, Float64 angle)
		{
			if (gradient.GetLength() > 1)
			{
				D2D1_GRADIENT_STOP* stop = gd_new D2D1_GRADIENT_STOP[gradient.GetLength()];
				if (!stop)
				{
					throw OutOfMemoryException();
				}
				for (Int32 i = 0; i < gradient.GetLength(); i++)
				{
					stop[i].color = D2D1::ColorF(gradient[i].Color.r / 255.0f, gradient[i].Color.g / 255.0f, gradient[i].Color.b / 255.0f, gradient[i].Color.a / 255.0f);
					stop[i].position = Float32(gradient[i].Position);
				}
				ID2D1GradientStopCollection* Collection;
				if (Target->CreateGradientStopCollection(stop, gradient.GetLength(), &Collection) != S_OK)
				{
					gd_delete[] stop;
					throw Exception();
				}
				BarRenderingInfo* Info = gd_new BarRenderingInfo;
				if (!Info)
				{
					gd_delete[] stop;
					Collection->Release();
					throw OutOfMemoryException();
				}
				Info->Collection = Collection;
				Info->prop_w = cos(angle), Info->prop_h = sin(angle);
				gd_delete[] stop;
				return Info;
			}
			else
			{
				auto CachedInfo = BrushCache.ElementByKey(gradient[0].Color);
				if (CachedInfo)
				{
					CachedInfo->AddRef();
					return CachedInfo;
				}
				BarRenderingInfo* Info = gd_new BarRenderingInfo;
				if (!Info)
				{
					throw OutOfMemoryException();
				}
				ID2D1SolidColorBrush* Brush;
				if (Target->CreateSolidColorBrush(
				        D2D1::ColorF(gradient[0].Color.r / 255.0f, gradient[0].Color.g / 255.0f, gradient[0].Color.b / 255.0f, gradient[0].Color.a / 255.0f), &Brush)
				    != S_OK)
				{
					Info->Release();
					throw Exception();
				}
				Info->Brush = Brush;
				BrushCache.InsertLast(gradient[0].Color, Info);
				return Info;
			}
		}
		IBlurEffectRenderingInfo* D2DRenderDevice::CreateBlurEffectRenderingInfo(Float64 power)
		{
			auto CachedInfo = BlurCache.ElementByKey(power);
			if (CachedInfo)
			{
				CachedInfo->AddRef();
				return CachedInfo;
			}
			auto Info = gd_new BlurEffectRenderingInfo;
			if (!Info)
				throw OutOfMemoryException();
			if (Target->CreateEffect(CLSID_D2D1GaussianBlur, Info->Effect.GetPtr()) != S_OK)
			{
				Info->Release();
				throw Exception();
			}
			Info->Effect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, Float32(power));
			Info->Effect->SetValue(D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION, D2D1_GAUSSIANBLUR_OPTIMIZATION_SPEED);
			Info->Effect->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);
			BlurCache.InsertLast(power, Info);
			return Info;
		}
		IInversionEffectRenderingInfo* D2DRenderDevice::CreateInversionEffectRenderingInfo()
		{
			if (!InversionInfo.Get())
			{
				auto Info = gd_new InversionEffectRenderingInfo;
				if (!Info)
					throw OutOfMemoryException();
				if (Target->CreateEffect(CLSID_D2D1ColorMatrix, Info->Effect.GetPtr()) != S_OK)
				{
					Info->Release();
					throw Exception();
				}
				Info->Effect->SetValue(D2D1_COLORMATRIX_PROP_COLOR_MATRIX,
				    D2D1::Matrix5x4F(-1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f));
				InversionInfo.operator=(Info);
			}
			InversionInfo->AddRef();
			return InversionInfo;
		}
		ITextureRenderingInfo* D2DRenderDevice::CreateTextureRenderingInfo(ITexture* texture, const Box& take_area, bool fill_pattern)
		{
			auto* Info = gd_new TextureRenderingInfo;
			Info->Texture = static_cast<D2DTexture*>(texture);
			Info->From = take_area;
			Info->Fill = fill_pattern;
			if (fill_pattern && !texture->IsDynamic())
			{
				for (Int32 i = 0; i < Info->Texture->Frames.GetLength(); i++)
				{
					ID2D1Bitmap* Fragment;
					if (take_area.Left == 0 && take_area.Top == 0 && take_area.Right == Info->Texture->w && take_area.Bottom == Info->Texture->h)
					{
						Fragment = Info->Texture->Frames[i];
						Fragment->AddRef();
					}
					else
					{
						if (Target->CreateBitmap(D2D1::SizeU(UInt32(take_area.Right - take_area.Left), UInt32(take_area.Bottom - take_area.Top)),
						        D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 0.0f, 0.0f), &Fragment)
						    != S_OK)
						{
							Info->Release();
							throw Exception();
						}
						auto destPoint = D2D1::Point2U(0, 0);
						auto srcRect = D2D1::RectU(take_area.Left, take_area.Top, take_area.Right, take_area.Bottom);
						if (Fragment->CopyFromBitmap(&destPoint, Info->Texture->Frames[i], &srcRect) != S_OK)
						{
							Fragment->Release();
							Info->Release();
							throw Exception();
						}
					}
					ID2D1BitmapBrush* Brush;
					if (Target->CreateBitmapBrush(
					        Fragment, D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_WRAP, D2D1_EXTEND_MODE_WRAP, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR), &Brush)
					    != S_OK)
					{
						Fragment->Release();
						Info->Release();
						throw Exception();
					}
					Fragment->Release();
					try
					{
						Info->Brushes << Brush;
					}
					catch (...)
					{
						Brush->Release();
						Info->Release();
						throw;
					}
				}
			}
			return Info;
		}
		ITextRenderingInfo* D2DRenderDevice::CreateTextRenderingInfo(IFont* font, const string& text, Int32 horizontal_align, Int32 vertical_align, const Color& color)
		{
			auto Info = gd_new TextRenderingInfo;
			try
			{
				Array<GradientPoint> array;
				array << GradientPoint(color, 0.0);
				Info->halign = horizontal_align;
				Info->valign = vertical_align;
				Info->Font = static_cast<D2DFont*>(font);
				Info->RenderTarget = Target;
				Info->HighlightBrush = 0;
				Info->TextBrush = 0;
				Info->Geometry = 0;
				Info->MainBrushInfo.operator=(static_cast<BarRenderingInfo*>(CreateBarRenderingInfo(array, 0.0)));
				Info->MainBrushInfo->AddRef();
				Info->TextBrush = Info->MainBrushInfo->Brush;
				Info->TextBrush->AddRef();
				Info->CharString.InsertLast('H');
				Info->CharString.InsertLast('U');
				Info->CharString.InsertLast('I');
				GD_STUBBED(UTF32);
				// text.Encode(Info->CharString, Encoding::UTF32, false);
				Info->FillGlyphs();
				Info->FillAdvances();
				Info->BuildGeometry();
			}
			catch (...)
			{
				Info->Release();
			}
			return Info;
		}
		ITextRenderingInfo* D2DRenderDevice::CreateTextRenderingInfo(IFont* font, const Array<UInt32>& text, Int32 horizontal_align, Int32 vertical_align, const Color& color)
		{
			auto Info = gd_new TextRenderingInfo;
			try
			{
				Array<GradientPoint> array(1);
				array << GradientPoint(color, 0.0);
				Info->halign = horizontal_align;
				Info->valign = vertical_align;
				Info->Font = static_cast<D2DFont*>(font);
				Info->RenderTarget = Target;
				Info->HighlightBrush = 0;
				Info->TextBrush = 0;
				Info->Geometry = 0;
				Info->MainBrushInfo.operator=(static_cast<BarRenderingInfo*>(CreateBarRenderingInfo(array, 0.0)));
				Info->MainBrushInfo->AddRef();
				Info->TextBrush = Info->MainBrushInfo->Brush;
				Info->TextBrush->AddRef();
				Info->CharString = text;
				Info->FillGlyphs();
				Info->FillAdvances();
				Info->BuildGeometry();
			}
			catch (...)
			{
				Info->Release();
			}
			return Info;
		}
		ILineRenderingInfo* D2DRenderDevice::CreateLineRenderingInfo(const Color& color, bool dotted)
		{
			Array<GradientPoint> array(1);
			array << GradientPoint(color, 0.0f);
			SafePointer<BarRenderingInfo> BarInfo = static_cast<BarRenderingInfo*>(CreateBarRenderingInfo(array, 0.0));
			LineRenderingInfo* Info = gd_new LineRenderingInfo;
			if (!Info)
				throw OutOfMemoryException();
			Info->Stroke = 0;
			Info->BrushInfo.operator=(BarInfo);
			Info->BrushInfo->AddRef();
			Info->Brush = Info->BrushInfo->Brush;
			Info->Brush->AddRef();
			if (dotted)
			{
				Float32 len[] = { 1.0f, 1.0f };
				D2DFactory->CreateStrokeStyle(
				    D2D1::StrokeStyleProperties(D2D1_CAP_STYLE_FLAT, D2D1_CAP_STYLE_FLAT, D2D1_CAP_STYLE_FLAT, D2D1_LINE_JOIN_MITER, 10.0f, D2D1_DASH_STYLE_CUSTOM, 0.5f), len, 2,
				    &Info->Stroke);
			}
			return Info;
		}
		ITexture* D2DRenderDevice::LoadTexture(Streaming::Stream* Source)
		{
			Streaming::ComStream* Stream = gd_new Streaming::ComStream(Source);
			IWICBitmapDecoder* Decoder = 0;
			IWICBitmapFrameDecode* FrameDecoder = 0;
			IWICMetadataQueryReader* Metadata = 0;
			IWICFormatConverter* Converter = 0;
			D2DTexture* Texture = gd_new D2DTexture;
			if (!Texture)
			{
				Stream->Release();
				throw OutOfMemoryException();
			}
			try
			{
				UInt32 FrameCount;
				HRESULT r;
				(r = WICFactory->CreateDecoderFromStream(Stream, 0, WICDecodeMetadataCacheOnDemand, &Decoder) != S_OK);
				if (r != S_OK)
					throw IO::FileFormatException();
				if (Decoder->GetFrameCount(&FrameCount) != S_OK)
					throw IO::FileFormatException();
				Texture->w = Texture->h = -1;
				for (UInt32 i = 0; i < FrameCount; i++)
				{
					if (Decoder->GetFrame(i, &FrameDecoder) != S_OK)
						throw IO::FileFormatException();
					UInt32 fw, fh;
					if (FrameDecoder->GetSize(&fw, &fh) != S_OK)
						throw IO::FileFormatException();
					if (Texture->w != -1 && Texture->w != (Int32)fw)
						throw IO::FileFormatException();
					if (Texture->h != -1 && Texture->h != (Int32)fh)
						throw IO::FileFormatException();
					Texture->w = Int32(fw);
					Texture->h = Int32(fh);
					if (FrameDecoder->GetMetadataQueryReader(&Metadata) == S_OK)
					{
						PROPVARIANT Value;
						PropVariantInit(&Value);
						if (Metadata->GetMetadataByName(L"/grctlext/Delay", &Value) == S_OK)
						{
							if (Value.vt == VT_UI2)
							{
								Texture->FrameDuration << UInt32(Value.uiVal) * 10;
							}
							else
								Texture->FrameDuration << 0;
						}
						else
							Texture->FrameDuration << 0;
						PropVariantClear(&Value);
						Metadata->Release();
						Metadata = 0;
					}
					else
						Texture->FrameDuration << 0;
					if (WICFactory->CreateFormatConverter(&Converter) != S_OK)
						throw Exception();
					if (Converter->Initialize(FrameDecoder, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, 0, 0.0f, WICBitmapPaletteTypeCustom) != S_OK)
						throw IO::FileFormatException();
					ID2D1Bitmap* Frame;
					if (Target->CreateBitmapFromWicBitmap(Converter, &Frame) != S_OK)
						throw IO::FileFormatException();
					try
					{
						Texture->Frames << Frame;
					}
					catch (...)
					{
						Frame->Release();
						throw;
					}
					Converter->Release();
					Converter = 0;
					FrameDecoder->Release();
					FrameDecoder = 0;
				}
				Decoder->Release();
				Decoder = 0;
			}
			catch (...)
			{
				Stream->Release();
				if (Decoder)
					Decoder->Release();
				if (FrameDecoder)
					FrameDecoder->Release();
				if (Metadata)
					Metadata->Release();
				if (Converter)
					Converter->Release();
				if (Texture)
					Texture->Release();
				throw;
			}
			Stream->Release();
			Texture->TotalDuration = Texture->FrameDuration[0];
			for (Int32 i = 1; i < Texture->FrameDuration.GetLength(); i++)
			{
				auto v = Texture->FrameDuration[i];
				Texture->FrameDuration[i] = Texture->TotalDuration;
				Texture->TotalDuration += v;
			}
			return Texture;
		}
		IFont* D2DRenderDevice::LoadFont(const string& FaceName, Int32 Height, Int32 Weight, bool IsItalic, bool IsUnderline, bool IsStrikeout)
		{
			D2DFont* Font = gd_new D2DFont;
			Font->FontFace = 0;
			Font->FontName = FaceName;
			Font->ActualHeight = Height;
			Font->Height = Int32(Float32(Height) * 72.0f / 96.0f);
			Font->Weight = Weight;
			Font->Italic = IsItalic;
			Font->Underline = IsUnderline;
			Font->Strikeout = IsStrikeout;
			IDWriteFontCollection* Collection = 0;
			IDWriteFontFamily* FontFamily = 0;
			IDWriteFont* FontSource = 0;
			try
			{
				if (DWriteFactory->GetSystemFontCollection(&Collection) != S_OK)
					throw Exception();
				UInt32 Index;
				BOOL Exists;
				if (Collection->FindFamilyName(FaceName.CStr(), &Index, &Exists) != S_OK)
					throw Exception();
				if (!Exists)
					throw Exception();
				if (Collection->GetFontFamily(Index, &FontFamily) != S_OK)
					throw Exception();
				if (FontFamily->GetFirstMatchingFont(
				        DWRITE_FONT_WEIGHT(Weight), DWRITE_FONT_STRETCH_NORMAL, IsItalic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL, &FontSource)
				    != S_OK)
					throw Exception();
				if (FontSource->CreateFontFace(&Font->FontFace) != S_OK)
					throw Exception();
				if (Font->FontFace->GetGdiCompatibleMetrics(Float32(Height), 1.0f, 0, &Font->FontMetrics) != S_OK)
					throw Exception();
				FontSource->Release();
				FontSource = 0;
				FontFamily->Release();
				FontFamily = 0;
				if (Collection->FindFamilyName(L"Segoe UI Emoji", &Index, &Exists) == S_OK)
				{
					if (Exists)
					{
						if (Collection->GetFontFamily(Index, &FontFamily) == S_OK)
						{
							if (FontFamily->GetFirstMatchingFont(
							        DWRITE_FONT_WEIGHT(Weight), DWRITE_FONT_STRETCH_NORMAL, IsItalic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL, &FontSource)
							    == S_OK)
							{
								FontSource->CreateFontFace(&Font->AlternativeFace);
								if (Font->AlternativeFace)
									Font->AlternativeFace->GetGdiCompatibleMetrics(Float32(Height), 1.0f, 0, &Font->AltMetrics);
								FontSource->Release();
								FontSource = 0;
							}
							FontFamily->Release();
							FontFamily = 0;
						}
					}
				}
				Collection->Release();
				Collection = 0;
			}
			catch (...)
			{
				Font->Release();
				if (Collection)
					Collection->Release();
				if (FontFamily)
					FontFamily->Release();
				if (FontSource)
					FontSource->Release();
				throw;
			}
			return Font;
		}
		void D2DRenderDevice::RenderBar(IBarRenderingInfo* Info, const Box& At)
		{
			auto info = reinterpret_cast<BarRenderingInfo*>(Info);
			if (info->Brush)
			{
				Target->FillRectangle(D2D1::RectF(Float32(At.Left), Float32(At.Top), Float32(At.Right), Float32(At.Bottom)), info->Brush);
			}
			else
			{
				Int32 w = At.Right - At.Left, h = At.Bottom - At.Top;
				if (!w || !h)
					return;
				D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES bp;
				if (fabs(info->prop_w) > fabs(info->prop_h))
				{
					Float64 aspect = info->prop_h / info->prop_w;
					if (fabs(aspect) > Float64(h) / Float64(w))
					{
						Int32 dx = Int32(h / fabs(aspect) * sgn(info->prop_w));
						bp.startPoint.x = Float32(At.Left + (w - dx) / 2);
						bp.startPoint.y = Float32((info->prop_h > 0.0) ? At.Bottom : At.Top);
						bp.endPoint.x = Float32(At.Left + (w + dx) / 2);
						bp.endPoint.y = Float32((info->prop_h > 0.0) ? At.Top : At.Bottom);
					}
					else
					{
						Int32 dy = Int32(w * fabs(aspect) * sgn(info->prop_h));
						bp.startPoint.x = Float32((info->prop_w > 0.0) ? At.Left : At.Right);
						bp.startPoint.y = Float32(At.Top + (h + dy) / 2);
						bp.endPoint.x = Float32((info->prop_w > 0.0) ? At.Right : At.Left);
						bp.endPoint.y = Float32(At.Top + (h - dy) / 2);
					}
				}
				else
				{
					Float64 aspect = info->prop_w / info->prop_h;
					if (fabs(aspect) > Float64(w) / Float64(h))
					{
						Int32 dy = Int32(w / fabs(aspect) * sgn(info->prop_h));
						bp.startPoint.x = Float32((info->prop_w > 0.0) ? At.Left : At.Right);
						bp.startPoint.y = Float32(At.Top + (h + dy) / 2);
						bp.endPoint.x = Float32((info->prop_w > 0.0) ? At.Right : At.Left);
						bp.endPoint.y = Float32(At.Top + (h - dy) / 2);
					}
					else
					{
						Int32 dx = Int32(h * fabs(aspect) * sgn(info->prop_w));
						bp.startPoint.x = Float32(At.Left + (w - dx) / 2);
						bp.startPoint.y = Float32((info->prop_h > 0.0) ? At.Bottom : At.Top);
						bp.endPoint.x = Float32(At.Left + (w + dx) / 2);
						bp.endPoint.y = Float32((info->prop_h > 0.0) ? At.Top : At.Bottom);
					}
				}
				ID2D1LinearGradientBrush* Brush;
				if (Target->CreateLinearGradientBrush(bp, info->Collection, &Brush) == S_OK)
				{
					Target->FillRectangle(D2D1::RectF(Float32(At.Left), Float32(At.Top), Float32(At.Right), Float32(At.Bottom)), Brush);
					Brush->Release();
				}
			}
		}
		void D2DRenderDevice::RenderTexture(ITextureRenderingInfo* Info, const Box& At)
		{
			auto info = static_cast<TextureRenderingInfo*>(Info);
			Int32 frame = 0;
			GD_STUBBED(Unhandled exception);
			//	if (info->Texture->Frames.GetLength() > 1) frame = max(BinarySearchLE(info->Texture->FrameDuration, AnimationTimer % info->Texture->TotalDuration), 0);
			D2D1_RECT_F To = D2D1::RectF(Float32(At.Left), Float32(At.Top), Float32(At.Right), Float32(At.Bottom));
			if (info->Fill)
			{
				Target->FillRectangle(To, info->Brushes[frame]);
			}
			else
			{
				D2D1_RECT_F From = D2D1::RectF(Float32(info->From.Left), Float32(info->From.Top), Float32(info->From.Right), Float32(info->From.Bottom));
				Target->DrawBitmap(info->Texture->Frames[frame], To, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, From);
			}
		}
		void D2DRenderDevice::RenderText(ITextRenderingInfo* Info, const Box& At, bool Clip)
		{
			auto info = static_cast<TextRenderingInfo*>(Info);
			if (Clip)
				PushClip(At);
			Int32 width, height;
			Info->GetExtent(width, height);
			D2D1_MATRIX_3X2_F Transform;
			Target->GetTransform(&Transform);
			Int32 shift_x = At.Left;
			Int32 shift_y = At.Top + info->BaselineOffset;
			if (info->halign == 1)
				shift_x += (At.Right - At.Left - width) / 2;
			else if (info->halign == 2)
				shift_x += (At.Right - At.Left - width);
			if (info->valign == 1)
				shift_y += (At.Bottom - At.Top - height) / 2;
			else if (info->valign == 2)
				shift_y += (At.Bottom - At.Top - height);
			if (info->hls != -1 && info->HighlightBrush)
			{
				Int32 start = info->EndOfChar(info->hls - 1);
				Int32 end = info->EndOfChar(info->hle - 1);
				D2D1_RECT_F Rect = D2D1::RectF(Float32(shift_x + start), Float32(At.Top), Float32(shift_x + end), Float32(At.Bottom));
				Target->FillRectangle(Rect, info->HighlightBrush);
			}
			Target->SetTransform(D2D1::Matrix3x2F::Translation(D2D1::SizeF(Float32(shift_x), Float32(shift_y))));
			Target->FillGeometry(info->Geometry, info->TextBrush, 0);
			if (info->Font->Underline)
			{
				Target->FillRectangle(
				    D2D1::RectF(0.0f, info->UnderlineOffset - info->UnderlineHalfWidth, Float32(info->run_length), info->UnderlineOffset + info->UnderlineHalfWidth),
				    info->TextBrush);
			}
			if (info->Font->Strikeout)
			{
				Target->FillRectangle(
				    D2D1::RectF(0.0f, info->StrikeoutOffset - info->StrikeoutHalfWidth, Float32(info->run_length), info->StrikeoutOffset + info->StrikeoutHalfWidth),
				    info->TextBrush);
			}
			Target->SetTransform(Transform);
			if (Clip)
				PopClip();
		}
		void D2DRenderDevice::RenderLine(ILineRenderingInfo* Info, const Box& At)
		{
			auto info = reinterpret_cast<LineRenderingInfo*>(Info);
			if (info->Brush)
			{
				D2D1_POINT_2F Start = D2D1::Point2F(Float32(At.Left) + 0.5f, Float32(At.Top) + 0.5f);
				D2D1_POINT_2F End = D2D1::Point2F(Float32(At.Right) + 0.5f, Float32(At.Bottom) + 0.5f);
				Target->DrawLine(Start, End, info->Brush, 1.0f, info->Stroke);
			}
		}
		void D2DRenderDevice::ApplyBlur(IBlurEffectRenderingInfo* Info, const Box& At)
		{
			auto info = static_cast<BlurEffectRenderingInfo*>(Info);
			SafePointer<ID2D1Bitmap> Fragment;
			Box Corrected = At;
			if (Corrected.Left < 0)
				Corrected.Left = 0;
			if (Corrected.Top < 0)
				Corrected.Top = 0;
			if (Target->CreateBitmap(D2D1::SizeU(Corrected.Right - Corrected.Left, Corrected.Bottom - Corrected.Top),
			        D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE), 0.0f, 0.0f), Fragment.GetPtr())
			    == S_OK)
			{
				auto a = D2D1::Point2U(0, 0);
				auto b = D2D1::RectU(Corrected.Left, Corrected.Top, Corrected.Right, Corrected.Bottom);
				Fragment->CopyFromRenderTarget(&a, Target, &b);
				info->Effect->SetInput(0, Fragment);
				Target->DrawImage(
				    info->Effect, D2D1::Point2F(Float32(Corrected.Left), Float32(Corrected.Top)), D2D1_INTERPOLATION_MODE_NEAREST_NEIGHBOR, D2D1_COMPOSITE_MODE_SOURCE_OVER);
			}
		}
		void D2DRenderDevice::ApplyInversion(IInversionEffectRenderingInfo* Info, const Box& At, bool Blink)
		{
			if (!Blink || (AnimationTimer % BlinkPeriod) < HalfBlinkPeriod)
			{
				auto info = static_cast<InversionEffectRenderingInfo*>(Info);
				SafePointer<ID2D1Bitmap> Fragment;
				Box Corrected = At;
				if (Corrected.Left < 0)
					Corrected.Left = 0;
				if (Corrected.Top < 0)
					Corrected.Top = 0;
				if (Target->CreateBitmap(D2D1::SizeU(Corrected.Right - Corrected.Left, Corrected.Bottom - Corrected.Top),
				        D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE), 0.0f, 0.0f), Fragment.GetPtr())
				    == S_OK)
				{
					auto a = D2D1::Point2U(0, 0);
					auto b = D2D1::RectU(Corrected.Left, Corrected.Top, Corrected.Right, Corrected.Bottom);
					Fragment->CopyFromRenderTarget(&a, Target, &b);
					info->Effect->SetInput(0, Fragment);
					Target->DrawImage(
					    info->Effect, D2D1::Point2F(Float32(Corrected.Left), Float32(Corrected.Top)), D2D1_INTERPOLATION_MODE_NEAREST_NEIGHBOR, D2D1_COMPOSITE_MODE_SOURCE_OVER);
				}
			}
		}
		void D2DRenderDevice::PushClip(const Box& Rect)
		{
			Target->PushAxisAlignedClip(D2D1::RectF(Float32(Rect.Left), Float32(Rect.Top), Float32(Rect.Right), Float32(Rect.Bottom)), D2D1_ANTIALIAS_MODE_ALIASED);
		}
		void D2DRenderDevice::PopClip()
		{
			Target->PopAxisAlignedClip();
		}
		void D2DRenderDevice::BeginLayer(const Box& Rect, Float64 Opacity)
		{
			ID2D1Layer* Layer;
			if (Target->CreateLayer(0, &Layer) != S_OK)
				throw Exception();
			Layers << Layer;
			D2D1_LAYER_PARAMETERS lp;
			lp.contentBounds = D2D1::RectF(Float32(Rect.Left), Float32(Rect.Top), Float32(Rect.Right), Float32(Rect.Bottom));
			lp.geometricMask = 0;
			lp.maskAntialiasMode = D2D1_ANTIALIAS_MODE_ALIASED;
			lp.maskTransform = D2D1::IdentityMatrix();
			lp.opacity = Float32(Opacity);
			lp.opacityBrush = 0;
			lp.layerOptions = D2D1_LAYER_OPTIONS_NONE;
			Target->PushLayer(&lp, Layer);
		}
		void D2DRenderDevice::EndLayer()
		{
			Target->PopLayer();
			Layers.GetLast()->Release();
			Layers.EraseLast();
		}
		void D2DRenderDevice::SetTimerValue(UInt32 time)
		{
			AnimationTimer = time;
		}
		void D2DRenderDevice::ClearCache()
		{
			InversionInfo.operator=(0);
			BrushCache.Clear();
			BlurCache.Clear();
		}

		void TextRenderingInfo::FillAdvances()
		{
			GlyphAdvances.Resize(GlyphString.GetLength());
			Array<DWRITE_GLYPH_METRICS> Metrics(0x40);
			Metrics.Resize(GlyphString.GetLength());
			if (Font->FontFace->GetGdiCompatibleGlyphMetrics(Float32(Font->Height), 1.0f, 0, TRUE, GlyphString.GetData(), GlyphString.GetLength(), Metrics.GetData()) != S_OK)
				throw Exception();
			if (Font->AlternativeFace)
			{
				for (Int32 i = 0; i < GlyphString.GetLength(); i++)
					if (UseAlternative[i])
					{
						Font->AlternativeFace->GetGdiCompatibleGlyphMetrics(Float32(Font->Height), 1.0f, 0, TRUE, GlyphString.GetData() + i, 1, Metrics.GetData() + i);
					}
			}
			for (Int32 i = 0; i < GlyphString.GetLength(); i++)
				GlyphAdvances[i] = (UseAlternative[i]) ? AltFontUnitsToDIP(Metrics[i].advanceWidth) : FontUnitsToDIP(Metrics[i].advanceWidth);
			BaselineOffset = Int32(FontUnitsToDIP(Font->FontMetrics.ascent));
			Float32 rl = 0.0f;
			for (Int32 i = 0; i < GlyphString.GetLength(); i++)
				rl += GlyphAdvances[i];
			run_length = Int32(rl);
		}
		void TextRenderingInfo::FillGlyphs()
		{
			UInt32 Space = 0x20;
			Font->FontFace->GetGlyphIndicesW(&Space, 1, &NormalSpaceGlyph);
			Font->AlternativeFace->GetGlyphIndicesW(&Space, 1, &AlternativeSpaceGlyph);
			GlyphString.Resize(CharString.GetLength());
			if (Font->FontFace->GetGlyphIndicesW(CharString.GetData(), CharString.GetLength(), GlyphString.GetData()) != S_OK)
				throw Exception();
			UseAlternative.Resize(CharString.GetLength());
			for (Int32 i = 0; i < UseAlternative.GetLength(); i++)
			{
				if (!GlyphString[i])
				{
					if (Font->AlternativeFace && Font->AlternativeFace->GetGlyphIndicesW(CharString.GetData() + i, 1, GlyphString.GetData() + i) == S_OK && GlyphString[i])
						UseAlternative[i] = true;
					else
						UseAlternative[i] = false;
				}
				else
					UseAlternative[i] = false;
			}
		}
		void TextRenderingInfo::BuildGeometry()
		{
			if (Geometry)
			{
				Geometry->Release();
				Geometry = 0;
			}
			ID2D1GeometrySink* Sink;
			if (D2DFactory->CreatePathGeometry(&Geometry) != S_OK)
				throw Exception();
			if (Geometry->Open(&Sink) != S_OK)
			{
				Geometry->Release();
				Geometry = 0;
				throw Exception();
			}
			Array<UInt16> Glyph = GlyphString;
			for (Int32 i = 0; i < Glyph.GetLength(); i++)
				if (UseAlternative[i])
					Glyph[i] = NormalSpaceGlyph;
			if (Font->FontFace->GetGlyphRunOutline(
			        Float32(Font->Height), Glyph.GetData(), GlyphAdvances.GetData(), 0, GlyphString.GetLength(), FALSE, FALSE, static_cast<ID2D1SimplifiedGeometrySink*>(Sink))
			    != S_OK)
			{
				Sink->Close();
				Sink->Release();
				Geometry->Release();
				Geometry = 0;
				throw Exception();
			}
			if (Font->AlternativeFace)
			{
				for (Int32 i = 0; i < Glyph.GetLength(); i++)
					Glyph[i] = (UseAlternative[i]) ? GlyphString[i] : AlternativeSpaceGlyph;
				Font->AlternativeFace->GetGlyphRunOutline(
				    Float32(Font->Height), Glyph.GetData(), GlyphAdvances.GetData(), 0, GlyphString.GetLength(), FALSE, FALSE, static_cast<ID2D1SimplifiedGeometrySink*>(Sink));
			}
			Sink->Close();
			Sink->Release();
			UnderlineOffset = -FontUnitsToDIP(Int16(Font->FontMetrics.underlinePosition));
			UnderlineHalfWidth = FontUnitsToDIP(Font->FontMetrics.underlineThickness) / 2.0f;
			StrikeoutOffset = -FontUnitsToDIP(Int16(Font->FontMetrics.strikethroughPosition));
			StrikeoutHalfWidth = FontUnitsToDIP(Font->FontMetrics.strikethroughThickness) / 2.0f;
		}
		Float32 TextRenderingInfo::FontUnitsToDIP(Int32 units)
		{
			return Float32(units) * Float32(Font->Height) / Float32(Font->FontMetrics.designUnitsPerEm);
		}
		Float32 TextRenderingInfo::AltFontUnitsToDIP(Int32 units)
		{
			return Float32(units) * Float32(Font->Height) / Float32(Font->AltMetrics.designUnitsPerEm);
		}
		void TextRenderingInfo::GetExtent(Int32& width, Int32& height)
		{
			width = run_length;
			height = Font->ActualHeight;
		}
	}
}
