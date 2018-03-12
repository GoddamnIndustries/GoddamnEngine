#pragma once

#include "../Miscellaneous/Dictionary.h"
#include "../UserInterface/ShapeBase.h"

#include <d2d1_1.h>
#include <dwrite.h>
#include <wincodec.h>

namespace Engine
{
	namespace Direct2D
	{
		using namespace ::Engine::UI;

		extern ID2D1Factory1* D2DFactory;
		extern IWICImagingFactory* WICFactory;
		extern IDWriteFactory* DWriteFactory;

		void InitializeFactory();
		void ShutdownFactory();

		class D2DRenderDevice : public IRenderingDevice
		{
			ID2D1DeviceContext* Target;
			Array<ID2D1Layer*> Layers;
			UInt32 AnimationTimer;
			UInt32 BlinkPeriod;
			UInt32 HalfBlinkPeriod;
			Dictionary::ObjectCache<Color, IBarRenderingInfo> BrushCache;
			Dictionary::ObjectCache<Float64, IBlurEffectRenderingInfo> BlurCache;
			SafePointer<IInversionEffectRenderingInfo> InversionInfo;

		public:
			D2DRenderDevice(ID2D1DeviceContext* target);
			~D2DRenderDevice() override;

			ID2D1RenderTarget* GetRenderTarget() const;

			virtual IBarRenderingInfo* CreateBarRenderingInfo(const Array<GradientPoint>& gradient, Float64 angle) override;
			virtual IBlurEffectRenderingInfo* CreateBlurEffectRenderingInfo(Float64 power) override;
			virtual IInversionEffectRenderingInfo* CreateInversionEffectRenderingInfo() override;
			virtual ITextureRenderingInfo* CreateTextureRenderingInfo(ITexture* texture, const Box& take_area, bool fill_pattern) override;
			virtual ITextRenderingInfo* CreateTextRenderingInfo(IFont* font, const string& text, Int32 horizontal_align, Int32 vertical_align, const Color& color) override;
			virtual ITextRenderingInfo* CreateTextRenderingInfo(IFont* font, const Array<UInt32>& text, Int32 horizontal_align, Int32 vertical_align, const Color& color) override;
			virtual ILineRenderingInfo* CreateLineRenderingInfo(const Color& color, bool dotted) override;

			virtual ITexture* LoadTexture(Streaming::Stream* Source) override;
			virtual IFont* LoadFont(const string& FaceName, Int32 Height, Int32 Weight, bool IsItalic, bool IsUnderline, bool IsStrikeout) override;

			virtual void RenderBar(IBarRenderingInfo* Info, const Box& At) override;
			virtual void RenderTexture(ITextureRenderingInfo* Info, const Box& At) override;
			virtual void RenderText(ITextRenderingInfo* Info, const Box& At, bool Clip) override;
			virtual void RenderLine(ILineRenderingInfo* Info, const Box& At) override;
			virtual void ApplyBlur(IBlurEffectRenderingInfo* Info, const Box& At) override;
			virtual void ApplyInversion(IInversionEffectRenderingInfo* Info, const Box& At, bool Blink) override;

			virtual void PushClip(const Box& Rect) override;
			virtual void PopClip() override;
			virtual void BeginLayer(const Box& Rect, Float64 Opacity) override;
			virtual void EndLayer() override;

			virtual void SetTimerValue(UInt32 time) override;
			virtual void ClearCache() override;
		};
	}
}
