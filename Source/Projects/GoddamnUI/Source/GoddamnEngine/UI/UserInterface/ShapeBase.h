#pragma once

#include "../EngineBase.h"
#include "../Streaming.h"

namespace Engine
{
	namespace UI
	{
		extern Float64 Zoom;

		class Coordinate
		{
		public:
			Float64 Anchor, Zoom;
			Int32 Absolute;

			Coordinate();
			Coordinate(Int32 shift);
			Coordinate(Int32 shift, Float64 zoom, Float64 anchor);

			Coordinate friend operator+(const Coordinate& a, const Coordinate& b);
			Coordinate friend operator-(const Coordinate& a, const Coordinate& b);
			Coordinate friend operator*(const Coordinate& a, Float64 b);
			Coordinate friend operator*(Float64 b, const Coordinate& a);
			Coordinate friend operator/(const Coordinate& a, Float64 b);

			Coordinate& operator+=(const Coordinate& a);
			Coordinate& operator-=(const Coordinate& a);
			Coordinate& operator*=(Float64 a);
			Coordinate& operator/=(Float64 a);
			Coordinate operator-() const;

			bool friend operator==(const Coordinate& a, const Coordinate& b);
			bool friend operator!=(const Coordinate& a, const Coordinate& b);

			static Coordinate Right();
			static Coordinate Bottom();
		};
		class Rectangle
		{
		public:
			Coordinate Left, Top, Right, Bottom;

			Rectangle();
			Rectangle(const Coordinate& left, const Coordinate& top, const Coordinate& right, const Coordinate& bottom);

			bool friend operator==(const Rectangle& a, const Rectangle& b);
			bool friend operator!=(const Rectangle& a, const Rectangle& b);

			bool IsValid() const;

			static Rectangle Entire();
			static Rectangle Invalid();
		};
		class Point
		{
		public:
			Int32 x, y;

			Point();
			Point(Int32 X, Int32 Y);

			bool friend operator==(const Point& a, const Point& b);
			bool friend operator!=(const Point& a, const Point& b);
		};
		class Box
		{
		public:
			Int32 Left, Top, Right, Bottom;

			Box();
			Box(const Rectangle& source, const Box& outer);
			Box(Int32 left, Int32 top, Int32 right, Int32 bottom);

			bool friend operator==(const Box& a, const Box& b);
			bool friend operator!=(const Box& a, const Box& b);

			bool IsInside(const Point& p) const;
		};
		class Color
		{
		public:
			union {
				struct
				{
					UInt8 r, g, b, a;
				};
				UInt32 Value;
			};

			Color();
			Color(UInt8 sr, UInt8 sg, UInt8 sb, UInt8 sa = 0xFF);
			Color(Int32 sr, Int32 sg, Int32 sb, Int32 sa = 0xFF);
			Color(Float32 sr, Float32 sg, Float32 sb, Float32 sa = 1.0);
			Color(Float64 sr, Float64 sg, Float64 sb, Float64 sa = 1.0);
			Color(UInt32 code);

			operator UInt32() const;

			bool friend operator==(const Color& a, const Color& b);
			bool friend operator!=(const Color& a, const Color& b);
		};
		class GradientPoint
		{
		public:
			Color Color;
			Float64 Position;

			GradientPoint();
			GradientPoint(const UI::Color& color);
			GradientPoint(const UI::Color& color, Float64 position);

			bool friend operator==(const GradientPoint& a, const GradientPoint& b);
			bool friend operator!=(const GradientPoint& a, const GradientPoint& b);
		};

		class IRenderingDevice;
		class FrameShape;
		class BarShape;

		class IBarRenderingInfo : public Object
		{
		public:
			virtual ~IBarRenderingInfo();
		};
		class IBlurEffectRenderingInfo : public Object
		{
		public:
			virtual ~IBlurEffectRenderingInfo();
		};
		class IInversionEffectRenderingInfo : public Object
		{
		public:
			virtual ~IInversionEffectRenderingInfo();
		};
		class ITextureRenderingInfo : public Object
		{
		public:
			virtual ~ITextureRenderingInfo();
		};
		class ITextRenderingInfo : public Object
		{
		public:
			virtual void GetExtent(Int32& width, Int32& height) = 0;
			virtual void SetHighlightColor(const Color& color) = 0;
			virtual void HighlightText(Int32 Start, Int32 End) = 0;
			virtual Int32 TestPosition(Int32 point) = 0;
			virtual Int32 EndOfChar(Int32 Index) = 0;
			virtual ~ITextRenderingInfo();
		};
		class ILineRenderingInfo : public Object
		{
		public:
			virtual ~ILineRenderingInfo();
		};

		class ITexture : public Object
		{
		public:
			virtual Int32 GetWidth() const = 0;
			virtual Int32 GetHeight() const = 0;
			virtual bool IsDynamic() const = 0;
			virtual void Reload(IRenderingDevice* Device, Streaming::Stream* Source) = 0;
		};
		class IFont : public Object
		{
		public:
			virtual void Reload(IRenderingDevice* Device) = 0;
		};

		class IRenderingDevice : public Object
		{
		public:
			virtual IBarRenderingInfo* CreateBarRenderingInfo(const Array<GradientPoint>& gradient, Float64 angle) = 0;
			virtual IBlurEffectRenderingInfo* CreateBlurEffectRenderingInfo(Float64 power) = 0;
			virtual IInversionEffectRenderingInfo* CreateInversionEffectRenderingInfo() = 0;
			virtual ITextureRenderingInfo* CreateTextureRenderingInfo(ITexture* texture, const Box& take_area, bool fill_pattern) = 0;
			virtual ITextRenderingInfo* CreateTextRenderingInfo(IFont* font, const string& text, Int32 horizontal_align, Int32 vertical_align, const Color& color) = 0;
			virtual ITextRenderingInfo* CreateTextRenderingInfo(IFont* font, const Array<UInt32>& text, Int32 horizontal_align, Int32 vertical_align, const Color& color) = 0;
			virtual ILineRenderingInfo* CreateLineRenderingInfo(const Color& color, bool dotted) = 0;

			virtual ITexture* LoadTexture(Streaming::Stream* Source) = 0;
			virtual IFont* LoadFont(const string& FaceName, Int32 Height, Int32 Weight, bool IsItalic, bool IsUnderline, bool IsStrikeout) = 0;

			virtual void RenderBar(IBarRenderingInfo* Info, const Box& At) = 0;
			virtual void RenderTexture(ITextureRenderingInfo* Info, const Box& At) = 0;
			virtual void RenderText(ITextRenderingInfo* Info, const Box& At, bool Clip) = 0;
			virtual void RenderLine(ILineRenderingInfo* Info, const Box& At) = 0;
			virtual void ApplyBlur(IBlurEffectRenderingInfo* Info, const Box& At) = 0;
			virtual void ApplyInversion(IInversionEffectRenderingInfo* Info, const Box& At, bool Blink) = 0;

			virtual void PushClip(const Box& Rect) = 0;
			virtual void PopClip() = 0;
			virtual void BeginLayer(const Box& Rect, Float64 Opacity) = 0;
			virtual void EndLayer() = 0;
			virtual void SetTimerValue(UInt32 time) = 0;
			virtual void ClearCache() = 0;
			virtual ~IRenderingDevice();
		};

		class Shape : public Object
		{
		public:
			Rectangle Position;
			virtual void Render(IRenderingDevice* Device, const Box& Outer) const = 0;
			virtual void ClearCache() = 0;
		};
		class FrameShape : public Shape
		{
		public:
			enum class FrameRenderMode
			{
				Normal = 0,
				Clipping = 1,
				Layering = 2
			};
			ObjectArray<Shape> Children;
			FrameRenderMode RenderMode;
			Float64 Opacity;
			FrameShape(const Rectangle& position);
			FrameShape(const Rectangle& position, FrameRenderMode mode, Float64 opacity = 1.0);
			~FrameShape() override;
			void Render(IRenderingDevice* Device, const Box& Outer) const override;
			void ClearCache() override;
			string ToString() const override;
		};
		class BarShape : public Shape
		{
			mutable SafePointer<IBarRenderingInfo> Info;
			Array<GradientPoint> Gradient;
			Float64 GradientAngle;

		public:
			BarShape(const Rectangle& position, const Color& color);
			BarShape(const Rectangle& position, const Array<GradientPoint>& gradient, Float64 angle);
			~BarShape() override;
			void Render(IRenderingDevice* Device, const Box& Outer) const override;
			void ClearCache() override;
			string ToString() const override;
		};
		class BlurEffectShape : public Shape
		{
			mutable SafePointer<IBlurEffectRenderingInfo> Info;
			Float64 BlurPower;

		public:
			BlurEffectShape(const Rectangle& position, Float64 power);
			~BlurEffectShape() override;
			void Render(IRenderingDevice* Device, const Box& Outer) const override;
			void ClearCache() override;
			string ToString() const override;
		};
		class InversionEffectShape : public Shape
		{
			mutable SafePointer<IInversionEffectRenderingInfo> Info;

		public:
			InversionEffectShape(const Rectangle& position);
			~InversionEffectShape() override;
			void Render(IRenderingDevice* Device, const Box& Outer) const override;
			void ClearCache() override;
			string ToString() const override;
		};
		class TextureShape : public Shape
		{
		public:
			enum class TextureRenderMode
			{
				Stretch = 0,
				Fit = 1,
				FillPattern = 2,
				AsIs = 3
			};

		private:
			mutable SafePointer<ITextureRenderingInfo> Info;
			mutable Box FromBox;
			ITexture* Texture;
			TextureRenderMode Mode;
			Rectangle From;

		public:
			TextureShape(const Rectangle& position, ITexture* texture, const Rectangle& take_from, TextureRenderMode mode);
			~TextureShape() override;
			void Render(IRenderingDevice* Device, const Box& Outer) const override;
			void ClearCache() override;
			string ToString() const override;
		};
		class TextShape : public Shape
		{
		public:
			enum class TextHorizontalAlign
			{
				Left = 0,
				Center = 1,
				Right = 2
			};
			enum class TextVerticalAlign
			{
				Top = 0,
				Center = 1,
				Bottom = 2
			};

		private:
			mutable SafePointer<ITextRenderingInfo> Info;
			IFont* Font;
			string Text;
			TextHorizontalAlign halign;
			TextVerticalAlign valign;
			Color TextColor;

		public:
			TextShape(const Rectangle& position, const string& text, IFont* font, const Color& color, TextHorizontalAlign horizontal_align, TextVerticalAlign vertical_align);
			~TextShape() override;
			void Render(IRenderingDevice* Device, const Box& Outer) const override;
			void ClearCache() override;
			string ToString() const override;
		};
		class LineShape : public Shape
		{
		private:
			mutable SafePointer<ILineRenderingInfo> Info;
			Color LineColor;
			bool Dotted;

		public:
			LineShape(const Rectangle& position, const Color& color, bool dotted);
			~LineShape() override;
			void Render(IRenderingDevice* Device, const Box& Outer) const override;
			void ClearCache() override;
			string ToString() const override;
		};
	}
}
