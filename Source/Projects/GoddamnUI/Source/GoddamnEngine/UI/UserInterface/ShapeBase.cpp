#include "ShapeBase.h"
#include "Templates.h"

namespace Engine
{
	namespace UI
	{
		Float64 Zoom = 1.0;

		Coordinate::Coordinate()
		{
		}
		Coordinate::Coordinate(Int32 shift)
		    : Absolute(shift)
		    , Anchor(0.0)
		    , Zoom(0.0)
		{
		}
		Coordinate::Coordinate(Int32 shift, Float64 zoom, Float64 anchor)
		    : Absolute(shift)
		    , Anchor(anchor)
		    , Zoom(zoom)
		{
		}
		Coordinate& Coordinate::operator+=(const Coordinate& a)
		{
			*this = *this + a;
			return *this;
		}
		Coordinate& Coordinate::operator-=(const Coordinate& a)
		{
			*this = *this - a;
			return *this;
		}
		Coordinate& Coordinate::operator*=(Float64 a)
		{
			*this = *this * a;
			return *this;
		}
		Coordinate& Coordinate::operator/=(Float64 a)
		{
			*this = *this / a;
			return *this;
		}
		Coordinate Coordinate::operator-() const
		{
			return Coordinate(-Absolute, -Zoom, -Anchor);
		}
		Coordinate Coordinate::Right()
		{
			return Coordinate(0, 0.0, 1.0);
		}
		Coordinate Coordinate::Bottom()
		{
			return Coordinate(0, 0.0, 1.0);
		}
		Coordinate operator+(const Coordinate& a, const Coordinate& b)
		{
			return Coordinate(a.Absolute + b.Absolute, a.Zoom + b.Zoom, a.Anchor + b.Anchor);
		}
		Coordinate operator-(const Coordinate& a, const Coordinate& b)
		{
			return Coordinate(a.Absolute - b.Absolute, a.Zoom - b.Zoom, a.Anchor - b.Anchor);
		}
		Coordinate operator*(const Coordinate& a, Float64 b)
		{
			return Coordinate(Int32(a.Absolute * b), a.Zoom* b, a.Anchor* b);
		}
		Coordinate operator*(Float64 b, const Coordinate& a)
		{
			return Coordinate(Int32(a.Absolute * b), a.Zoom* b, a.Anchor* b);
		}
		Coordinate operator/(const Coordinate& a, Float64 b)
		{
			return Coordinate(Int32(a.Absolute / b), a.Zoom / b, a.Anchor / b);
		}
		bool operator==(const Coordinate& a, const Coordinate& b)
		{
			return a.Absolute == b.Absolute && a.Zoom == b.Zoom && a.Anchor == b.Anchor;
		}
		bool operator!=(const Coordinate& a, const Coordinate& b)
		{
			return a.Absolute != b.Absolute || a.Zoom != b.Zoom || a.Anchor != b.Anchor;
		}
		bool operator==(const Rectangle& a, const Rectangle& b)
		{
			return a.Left == b.Left && a.Top == b.Top && a.Right == b.Right && a.Bottom == b.Bottom;
		}
		bool operator!=(const Rectangle& a, const Rectangle& b)
		{
			return a.Left != b.Left || a.Top != b.Top || a.Right != b.Right || a.Bottom != b.Bottom;
		}
		bool operator==(const Point& a, const Point& b)
		{
			return a.x == b.x && a.y == b.y;
		}
		bool operator!=(const Point& a, const Point& b)
		{
			return a.x != b.x || a.y != b.y;
		}
		bool operator==(const Box& a, const Box& b)
		{
			return a.Left == b.Left && a.Top == b.Top && a.Right == b.Right && a.Bottom == b.Bottom;
		}
		bool operator!=(const Box& a, const Box& b)
		{
			return a.Left != b.Left || a.Top != b.Top || a.Right != b.Right || a.Bottom != b.Bottom;
		}
		bool operator==(const Color& a, const Color& b)
		{
			return a.Value == b.Value;
		}
		bool operator!=(const Color& a, const Color& b)
		{
			return a.Value != b.Value;
		}
		bool operator==(const GradientPoint& a, const GradientPoint& b)
		{
			return a.Color == b.Color && a.Position == b.Position;
		}
		bool operator!=(const GradientPoint& a, const GradientPoint& b)
		{
			return a.Color != b.Color || a.Position != b.Position;
		}
		Rectangle::Rectangle()
		{
		}
		Rectangle::Rectangle(const Coordinate& left, const Coordinate& top, const Coordinate& right, const Coordinate& bottom)
		    : Left(left)
		    , Top(top)
		    , Right(right)
		    , Bottom(bottom)
		{
		}
		bool Rectangle::IsValid() const
		{
			return Left.Anchor == Left.Anchor;
		}
		Rectangle Rectangle::Entire()
		{
			return Rectangle(0, 0, Coordinate::Right(), Coordinate::Bottom());
		}
		Rectangle Rectangle::Invalid()
		{
			Float64 z = 0.0, nan = z / z;
			return Rectangle(Coordinate(0, 0.0, nan), Coordinate(0, 0.0, nan), Coordinate(0, 0.0, nan), Coordinate(0, 0.0, nan));
		}
		Point::Point()
		{
		}
		Point::Point(Int32 X, Int32 Y)
		    : x(X)
		    , y(Y)
		{
		}
		Box::Box()
		{
		}
		Box::Box(const Rectangle& source, const Box& outer)
		{
			Float64 Width = outer.Right - outer.Left;
			Float64 Height = outer.Bottom - outer.Top;
			Left = outer.Left + source.Left.Absolute + Int32(source.Left.Anchor * Width) + Int32(source.Left.Zoom * Zoom);
			Right = outer.Left + source.Right.Absolute + Int32(source.Right.Anchor * Width) + Int32(source.Right.Zoom * Zoom);
			Top = outer.Top + source.Top.Absolute + Int32(source.Top.Anchor * Height) + Int32(source.Top.Zoom * Zoom);
			Bottom = outer.Top + source.Bottom.Absolute + Int32(source.Bottom.Anchor * Height) + Int32(source.Bottom.Zoom * Zoom);
		}
		Box::Box(Int32 left, Int32 top, Int32 right, Int32 bottom)
		    : Left(left)
		    , Top(top)
		    , Right(right)
		    , Bottom(bottom)
		{
		}
		bool Box::IsInside(const Point& p) const
		{
			return p.x >= Left && p.x < Right && p.y >= Top && p.y < Bottom;
		}
		Color::Color()
		{
		}
		Color::Color(UInt8 sr, UInt8 sg, UInt8 sb, UInt8 sa)
		    : r(sr)
		    , g(sg)
		    , b(sb)
		    , a(sa)
		{
		}
		Color::Color(Int32 sr, Int32 sg, Int32 sb, Int32 sa)
		    : r((UInt8)sr)
		    , g((UInt8)sg)
		    , b((UInt8)sb)
		    , a((UInt8)sa)
		{
		}
		Color::Color(Float32 sr, Float32 sg, Float32 sb, Float32 sa)
		    : r((UInt8)max(min(Int32(sr * 255.0f), 255), 0))
		    , g((UInt8)max(min(Int32(sg * 255.0f), 255), 0))
		    , b((UInt8)max(min(Int32(sb * 255.0f), 0), 0))
		    , a((UInt8)max(min(Int32(sa * 255.0f), 0), 0))
		{
		}
		Color::Color(Float64 sr, Float64 sg, Float64 sb, Float64 sa)
		    : r((UInt8)max(min(Int32(sr * 255.0), 255), 0))
		    , g((UInt8)max(min(Int32(sg * 255.0), 255), 0))
		    , b((UInt8)max(min(Int32(sb * 255.0), 0), 0))
		    , a((UInt8)max(min(Int32(sa * 255.0), 0), 0))
		{
		}
		Color::Color(UInt32 code)
		    : Value(code)
		{
		}
		Color::operator UInt32() const
		{
			return Value;
		}
		FrameShape::FrameShape(const Rectangle& position)
		    : Children()
		    , RenderMode(FrameRenderMode::Normal)
		    , Opacity(1.0)
		{
			Position = position;
		}
		FrameShape::FrameShape(const Rectangle& position, FrameRenderMode mode, Float64 opacity)
		    : Children()
		    , RenderMode(mode)
		    , Opacity(opacity)
		{
			Position = position;
		}
		FrameShape::~FrameShape()
		{
		}
		void FrameShape::Render(IRenderingDevice* Device, const Box& Outer) const
		{
			Box my(Position, Outer);
			if (my.Right < my.Left || my.Bottom < my.Top)
				return;
			if (RenderMode == FrameRenderMode::Normal)
			{
				for (Int32 i = Children.GetLength() - 1; i >= 0; i--)
					Children[i]->Render(Device, my);
			}
			else if (RenderMode == FrameRenderMode::Clipping)
			{
				Device->PushClip(my);
				for (Int32 i = Children.GetLength() - 1; i >= 0; i--)
					Children[i]->Render(Device, my);
				Device->PopClip();
			}
			else if (RenderMode == FrameRenderMode::Layering)
			{
				Device->BeginLayer(my, Opacity);
				for (Int32 i = Children.GetLength() - 1; i >= 0; i--)
					Children[i]->Render(Device, my);
				Device->EndLayer();
			}
		}
		void FrameShape::ClearCache()
		{
			for (Int32 i = Children.GetLength() - 1; i >= 0; i--)
				Children[i]->ClearCache();
		}
		string FrameShape::ToString() const
		{
			return L"FrameShape";
		}
		GradientPoint::GradientPoint()
		{
		}
		GradientPoint::GradientPoint(const UI::Color& color)
		    : Color(color)
		    , Position(0.0)
		{
		}
		GradientPoint::GradientPoint(const UI::Color& color, Float64 position)
		    : Color(color)
		    , Position(position)
		{
		}
		BarShape::BarShape(const Rectangle& position, const Color& color)
		    : GradientAngle(0.0)
		{
			Position = position;
			Gradient << GradientPoint(color);
		}
		BarShape::BarShape(const Rectangle& position, const Array<GradientPoint>& gradient, Float64 angle)
		    : Gradient(gradient)
		    , GradientAngle(angle)
		{
			Position = position;
		}
		BarShape::~BarShape()
		{
		}
		void BarShape::Render(IRenderingDevice* Device, const Box& Outer) const
		{
			if (!Info)
				Info.operator=(Device->CreateBarRenderingInfo(Gradient, GradientAngle));
			Box my(Position, Outer);
			if (my.Right < my.Left || my.Bottom < my.Top)
				return;
			Device->RenderBar(Info, my);
		}
		void BarShape::ClearCache()
		{
			Info.operator=(0);
		}
		string BarShape::ToString() const
		{
			return L"BarShape";
		}
		IRenderingDevice::~IRenderingDevice()
		{
		}
		IBarRenderingInfo::~IBarRenderingInfo()
		{
		}
		IBlurEffectRenderingInfo::~IBlurEffectRenderingInfo()
		{
		}
		BlurEffectShape::BlurEffectShape(const Rectangle& position, Float64 power)
		    : BlurPower(power)
		{
			Position = position;
		}
		BlurEffectShape::~BlurEffectShape()
		{
		}
		void BlurEffectShape::Render(IRenderingDevice* Device, const Box& Outer) const
		{
			if (!Info)
				Info.operator=(Device->CreateBlurEffectRenderingInfo(BlurPower));
			Box my(Position, Outer);
			if (my.Right < my.Left || my.Bottom < my.Top)
				return;
			Device->ApplyBlur(Info, my);
		}
		void BlurEffectShape::ClearCache()
		{
			Info.operator=(0);
		}
		string BlurEffectShape::ToString() const
		{
			return L"BlurEffectShape";
		}
		ITextureRenderingInfo::~ITextureRenderingInfo()
		{
		}
		TextureShape::TextureShape(const Rectangle& position, ITexture* texture, const Rectangle& take_from, TextureRenderMode mode)
		    : Texture(texture)
		    , From(take_from)
		    , Mode(mode)
		{
			Position = position;
			if (Texture)
				Texture->AddRef();
		}
		TextureShape::~TextureShape()
		{
			if (Texture)
				Texture->Release();
		}
		void TextureShape::Render(IRenderingDevice* Device, const Box& Outer) const
		{
			if (Texture)
			{
				if (!Info)
				{
					FromBox = Box(From, Box(0, 0, Texture->GetWidth(), Texture->GetHeight()));
					Info.operator=(Device->CreateTextureRenderingInfo(Texture, FromBox, Mode == TextureRenderMode::FillPattern));
				}
				Box to(Position, Outer);
				if (to.Right < to.Left || to.Bottom < to.Top)
					return;
				if (Mode == TextureRenderMode::Fit)
				{
					Float64 ta = Float64(to.Right - to.Left) / Float64(to.Bottom - to.Top);
					Float64 fa = Float64(FromBox.Right - FromBox.Left) / Float64(FromBox.Bottom - FromBox.Top);
					if (ta > fa)
					{
						Int32 adjx = Int32(Float64(to.Bottom - to.Top) * fa);
						Int32 xc = (to.Right + to.Left) >> 1;
						to.Left = xc - (adjx >> 1);
						to.Right = xc + (adjx >> 1);
					}
					else if (fa > ta)
					{
						Int32 adjy = Int32(Float64(to.Right - to.Left) / fa);
						Int32 yc = (to.Bottom + to.Top) >> 1;
						to.Top = yc - (adjy >> 1);
						to.Bottom = yc + (adjy >> 1);
					}
				}
				else if (Mode == TextureRenderMode::AsIs)
				{
					Int32 sx = FromBox.Right - FromBox.Left;
					Int32 sy = FromBox.Bottom - FromBox.Top;
					if (to.Right - to.Left < sx || to.Bottom - to.Top < sy)
						return;
					to.Right = to.Left = (to.Right + to.Left) / 2;
					to.Bottom = to.Top = (to.Bottom + to.Top) / 2;
					Int32 rx = sx / 2, ry = sy / 2;
					to.Left -= rx;
					to.Top -= ry;
					to.Right += sx - rx;
					to.Bottom += sy - ry;
				}
				Device->RenderTexture(Info, to);
			}
		}
		void TextureShape::ClearCache()
		{
			Info.operator=(0);
		}
		string TextureShape::ToString() const
		{
			return L"TextureShape";
		}
		ITextRenderingInfo::~ITextRenderingInfo()
		{
		}
		TextShape::TextShape(const Rectangle& position, const string& text, IFont* font, const Color& color, TextHorizontalAlign horizontal_align, TextVerticalAlign vertical_align)
		    : Text(text)
		    , Font(font)
		    , TextColor(color)
		    , halign(horizontal_align)
		    , valign(vertical_align)
		{
			Position = position;
			if (Font)
				Font->AddRef();
		}
		TextShape::~TextShape()
		{
			if (Font)
				Font->Release();
		}
		void TextShape::Render(IRenderingDevice* Device, const Box& Outer) const
		{
			if (!Info)
				Info.operator=(Device->CreateTextRenderingInfo(Font, Text, Int32(halign), Int32(valign), TextColor));
			Box my(Position, Outer);
			if (my.Right < my.Left || my.Bottom < my.Top)
				return;
			Device->RenderText(Info, my, true);
		}
		void TextShape::ClearCache()
		{
			Info.operator=(0);
		}
		string TextShape::ToString() const
		{
			return L"TextShape";
		}
		ILineRenderingInfo::~ILineRenderingInfo()
		{
		}
		LineShape::LineShape(const Rectangle& position, const Color& color, bool dotted)
		    : LineColor(color)
		    , Dotted(dotted)
		{
			Position = position;
		}
		LineShape::~LineShape()
		{
		}
		void LineShape::Render(IRenderingDevice* Device, const Box& Outer) const
		{
			if (!Info)
				Info.operator=(Device->CreateLineRenderingInfo(LineColor, Dotted));
			Box my(Position, Outer);
			Device->RenderLine(Info, my);
		}
		void LineShape::ClearCache()
		{
			Info.operator=(0);
		}
		string LineShape::ToString() const
		{
			return L"LineShape";
		}
		IInversionEffectRenderingInfo::~IInversionEffectRenderingInfo()
		{
		}
		InversionEffectShape::InversionEffectShape(const Rectangle& position)
		{
			Position = position;
		}
		InversionEffectShape::~InversionEffectShape()
		{
		}
		void InversionEffectShape::Render(IRenderingDevice* Device, const Box& Outer) const
		{
			if (!Info)
				Info.operator=(Device->CreateInversionEffectRenderingInfo());
			Box my(Position, Outer);
			if (my.Right < my.Left || my.Bottom < my.Top)
				return;
			Device->ApplyInversion(Info, my, false);
		}
		void InversionEffectShape::ClearCache()
		{
			Info.operator=(0);
		}
		string InversionEffectShape::ToString() const
		{
			return L"InversionEffectShape";
		}
	}
}
