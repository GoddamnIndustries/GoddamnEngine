#include "Templates.h"

namespace Engine
{
	namespace UI
	{
		namespace Template
		{
			Coordinate::Coordinate()
			{
			}
			Coordinate::Coordinate(const Engine::UI::Coordinate& src)
			    : Absolute(src.Absolute)
			    , Zoom(src.Zoom)
			    , Anchor(src.Anchor)
			{
			}
			Coordinate::Coordinate(const BasicTemplate<Int32>& shift, const BasicTemplate<Float64>& zoom, const BasicTemplate<Float64>& anchor)
			    : Absolute(shift)
			    , Zoom(zoom)
			    , Anchor(anchor)
			{
			}
			bool Coordinate::IsDefined() const
			{
				return Absolute.IsDefined() && Zoom.IsDefined() && Anchor.IsDefined();
			}
			Engine::UI::Coordinate Coordinate::Initialize(IArgumentProvider* provider) const
			{
				return Engine::UI::Coordinate(Absolute.Initialize(provider), Zoom.Initialize(provider), Anchor.Initialize(provider));
			}
			Rectangle::Rectangle()
			{
			}
			Rectangle::Rectangle(const Engine::UI::Rectangle& src)
			    : Left(src.Left)
			    , Top(src.Top)
			    , Right(src.Right)
			    , Bottom(src.Bottom)
			{
			}
			Rectangle::Rectangle(const Coordinate& left, const Coordinate& top, const Coordinate& right, const Coordinate& bottom)
			    : Left(left)
			    , Top(top)
			    , Right(right)
			    , Bottom(bottom)
			{
			}
			bool Rectangle::IsDefined() const
			{
				return Left.IsDefined() && Top.IsDefined() && Right.IsDefined() && Bottom.IsDefined();
			}
			Engine::UI::Rectangle Rectangle::Initialize(IArgumentProvider* provider) const
			{
				return Engine::UI::Rectangle(Left.Initialize(provider), Top.Initialize(provider), Right.Initialize(provider), Bottom.Initialize(provider));
			}
			GradientPoint::GradientPoint()
			{
			}
			GradientPoint::GradientPoint(const Engine::UI::GradientPoint& gradient)
			    : PointColor(gradient.Color)
			    , Position(gradient.Position)
			{
			}
			GradientPoint::GradientPoint(const BasicTemplate<Color>& color, const BasicTemplate<Float64>& position)
			    : PointColor(color)
			    , Position(position)
			{
			}
			bool GradientPoint::IsDefined() const
			{
				return PointColor.IsDefined() && Position.IsDefined();
			}
			Engine::UI::GradientPoint GradientPoint::Initialize(IArgumentProvider* provider) const
			{
				return Engine::UI::GradientPoint(PointColor.Initialize(provider), Position.Initialize(provider));
			}

			BarShape::BarShape()
			    : GradientAngle(0.0)
			{
				Position = Engine::UI::Rectangle::Entire();
			}
			BarShape::~BarShape()
			{
			}
			string BarShape::ToString() const
			{
				return L"Templates::BarShape";
			}
			bool BarShape::IsDefined() const
			{
				for (Int32 i = 0; i < Gradient.GetLength(); i++)
					if (!Gradient[i].IsDefined())
						return false;
				return GradientAngle.IsDefined() && Position.IsDefined();
			}
			Engine::UI::Shape* BarShape::Initialize(IArgumentProvider* provider) const
			{
				Array<Engine::UI::GradientPoint> init(1);
				init.Resize(Gradient.GetLength());
				for (Int32 i = 0; i < Gradient.GetLength(); i++)
					init[i] = Gradient[i].Initialize(provider);
				return gd_new Engine::UI::BarShape(Position.Initialize(provider), init, GradientAngle.Initialize(provider));
			}
			BlurEffectShape::BlurEffectShape()
			    : BlurPower(5.0)
			{
				Position = Engine::UI::Rectangle::Entire();
			}
			BlurEffectShape::~BlurEffectShape()
			{
			}
			string BlurEffectShape::ToString() const
			{
				return L"Templates::BlurEffectShape";
			}
			bool BlurEffectShape::IsDefined() const
			{
				return BlurPower.IsDefined() && Position.IsDefined();
			}
			Engine::UI::Shape* BlurEffectShape::Initialize(IArgumentProvider* provider) const
			{
				return gd_new Engine::UI::BlurEffectShape(Position.Initialize(provider), BlurPower.Initialize(provider));
			}
			InversionEffectShape::InversionEffectShape()
			{
				Position = Engine::UI::Rectangle::Entire();
			}
			InversionEffectShape::~InversionEffectShape()
			{
			}
			string InversionEffectShape::ToString() const
			{
				return L"Templates::InversionEffectShape";
			}
			bool InversionEffectShape::IsDefined() const
			{
				return Position.IsDefined();
			}
			Engine::UI::Shape* InversionEffectShape::Initialize(IArgumentProvider* provider) const
			{
				return gd_new Engine::UI::InversionEffectShape(Position.Initialize(provider));
			}
			TextureShape::TextureShape()
			    : RenderMode(Engine::UI::TextureShape::TextureRenderMode::Stretch)
			{
				Position = Engine::UI::Rectangle::Entire();
				From = Engine::UI::Rectangle::Entire();
			}
			TextureShape::~TextureShape()
			{
			}
			string TextureShape::ToString() const
			{
				return L"Templates::TextureShape";
			}
			bool TextureShape::IsDefined() const
			{
				return Texture.IsDefined() && Position.IsDefined() && From.IsDefined();
			}
			Engine::UI::Shape* TextureShape::Initialize(IArgumentProvider* provider) const
			{
				SafePointer<ITexture> Query = Texture.Initialize(provider);
				return gd_new Engine::UI::TextureShape(Position.Initialize(provider), Query, From.Initialize(provider), RenderMode);
			}
			TextShape::TextShape()
			    : HorizontalAlign(Engine::UI::TextShape::TextHorizontalAlign::Left)
			    , VerticalAlign(Engine::UI::TextShape::TextVerticalAlign::Top)
			    , TextColor(Engine::UI::Color(0))
			{
				Position = Engine::UI::Rectangle::Entire();
			}
			TextShape::~TextShape()
			{
			}
			string TextShape::ToString() const
			{
				return L"Templates::TextShape";
			}
			bool TextShape::IsDefined() const
			{
				return Position.IsDefined() && TextColor.IsDefined() && Text.IsDefined() && Font.IsDefined();
			}
			Engine::UI::Shape* TextShape::Initialize(IArgumentProvider* provider) const
			{
				SafePointer<IFont> Query = Font.Initialize(provider);
				return gd_new Engine::UI::TextShape(
				    Position.Initialize(provider), Text.Initialize(provider), Query, TextColor.Initialize(provider), HorizontalAlign, VerticalAlign);
			}
			LineShape::LineShape()
			    : Dotted(false)
			    , LineColor(Engine::UI::Color(0))
			{
				Position = Engine::UI::Rectangle::Entire();
			}
			LineShape::~LineShape()
			{
			}
			string LineShape::ToString() const
			{
				return L"Templates::LineShape";
			}
			bool LineShape::IsDefined() const
			{
				return Position.IsDefined() && LineColor.IsDefined();
			}
			Engine::UI::Shape* LineShape::Initialize(IArgumentProvider* provider) const
			{
				return gd_new Engine::UI::LineShape(Position.Initialize(provider), LineColor.Initialize(provider), Dotted);
			}
			FrameShape::FrameShape()
			    : RenderMode(Engine::UI::FrameShape::FrameRenderMode::Normal)
			    , Opacity(1.0)
			{
				Position = Engine::UI::Rectangle::Entire();
			}
			FrameShape::~FrameShape()
			{
			}
			string FrameShape::ToString() const
			{
				return L"Templates::FrameShape";
			}
			bool FrameShape::IsDefined() const
			{
				for (Int32 i = 0; i < Children.GetLength(); i++)
					if (!Children[i]->IsDefined())
						return false;
				return Position.IsDefined() && Opacity.IsDefined();
			}
			Engine::UI::Shape* FrameShape::Initialize(IArgumentProvider* provider) const
			{
				SafePointer<Engine::UI::FrameShape> New = gd_new Engine::UI::FrameShape(Position.Initialize(provider), RenderMode, Opacity.Initialize(provider));
				for (Int32 i = 0; i < Children.GetLength(); i++)
					New->Children << Children[i]->Initialize(provider);
				New->AddRef();
				return New;
			}
			ControlTemplate::ControlTemplate(ControlReflectedBase* properties)
			    : Properties(properties)
			{
			}
			ControlTemplate::~ControlTemplate()
			{
				gd_delete Properties;
			}
			string ControlTemplate::ToString() const
			{
				return Properties->GetTemplateClass();
			}
			ControlReflectedBase::~ControlReflectedBase()
			{
			}
		}
		InterfaceTemplate::InterfaceTemplate()
		    : Texture(0x20)
		    , Font(0x10)
		    , Application(0x20)
		    , Dialog(0x10)
		{
		}
		InterfaceTemplate::~InterfaceTemplate()
		{
		}
		string InterfaceTemplate::ToString() const
		{
			return L"InterfaceTemplate";
		}
	}
}
