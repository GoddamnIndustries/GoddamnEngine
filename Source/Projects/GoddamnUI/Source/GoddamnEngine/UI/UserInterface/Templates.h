#pragma once

#include "../Miscellaneous/Dictionary.h"
#include "../Miscellaneous/Reflection.h"
#include "ShapeBase.h"

namespace Engine
{
	namespace UI
	{
		class IArgumentProvider
		{
		public:
			virtual void GetArgument(const string& name, Int32* value) = 0;
			virtual void GetArgument(const string& name, Float64* value) = 0;
			virtual void GetArgument(const string& name, Color* value) = 0;
			virtual void GetArgument(const string& name, string* value) = 0;
			virtual void GetArgument(const string& name, ITexture** value) = 0;
			virtual void GetArgument(const string& name, IFont** value) = 0;
		};

		namespace Template
		{
			template <class T>
			class BasicTemplate
			{
				T value;
				string argument;

			public:
				BasicTemplate()
				{
				}
				BasicTemplate(const T& src)
				    : value(src)
				{
				}

				static BasicTemplate Undefined(const string& argument_from)
				{
					BasicTemplate result;
					result.argument = argument_from;
					return result;
				}

				bool IsDefined() const
				{
					return argument.GetLength() == 0;
				}
				T Initialize(IArgumentProvider* provider) const
				{
					if (argument.GetLength())
					{
						T result;
						provider->GetArgument(argument, &result);
						return result;
					}
					else
						return value;
				}
			};
			template <class O>
			class ObjectTemplate
			{
				SafePointer<O> value;
				string argument;

			public:
				ObjectTemplate()
				{
				}
				ObjectTemplate(const ObjectTemplate& src)
				    : argument(src.argument)
				{
					value.operator=(src.value);
				}
				ObjectTemplate(ObjectTemplate&& src)
				    : argument(src.argument)
				{
					value.operator=(src.value);
					src.value.operator=(0);
				}
				ObjectTemplate(O* object)
				{
					value.operator=(object);
				}
				ObjectTemplate& operator=(const ObjectTemplate& src)
				{
					if (this == &src)
						return *this;
					value.operator=(src.value);
					argument = src.argument;
					return *this;
				}

				static ObjectTemplate Undefined(const string& argument_from)
				{
					ObjectTemplate result;
					result.argument = argument_from;
					return result;
				}

				bool IsDefined() const
				{
					return argument.GetLength() == 0;
				}
				O* Initialize(IArgumentProvider* provider) const
				{
					if (argument.GetLength())
					{
						O* result;
						provider->GetArgument(argument, &result);
						return result;
					}
					else
					{
						if (value.Get())
							value->AddRef();
						return value;
					}
				}
			};

			typedef BasicTemplate<Int32> IntegerTemplate;
			typedef BasicTemplate<Float64> DoubleTemplate;
			typedef BasicTemplate<Color> ColorTemplate;
			typedef BasicTemplate<string> StringTemplate;
			typedef ObjectTemplate<ITexture> TextureTemplate;
			typedef ObjectTemplate<IFont> FontTemplate;

			class Coordinate
			{
			public:
				BasicTemplate<Float64> Anchor, Zoom;
				BasicTemplate<Int32> Absolute;

				Coordinate();
				Coordinate(const Engine::UI::Coordinate& src);
				Coordinate(const BasicTemplate<Int32>& shift, const BasicTemplate<Float64>& zoom, const BasicTemplate<Float64>& anchor);

				bool IsDefined() const;
				Engine::UI::Coordinate Initialize(IArgumentProvider* provider) const;
			};
			class Rectangle
			{
			public:
				Coordinate Left, Top, Right, Bottom;

				Rectangle();
				Rectangle(const Engine::UI::Rectangle& src);
				Rectangle(const Coordinate& left, const Coordinate& top, const Coordinate& right, const Coordinate& bottom);

				bool IsDefined() const;
				Engine::UI::Rectangle Initialize(IArgumentProvider* provider) const;
			};
			class GradientPoint
			{
			public:
				BasicTemplate<Color> PointColor;
				BasicTemplate<Float64> Position;

				GradientPoint();
				GradientPoint(const Engine::UI::GradientPoint& gradient);
				GradientPoint(const BasicTemplate<Color>& color, const BasicTemplate<Float64>& position);

				bool IsDefined() const;
				Engine::UI::GradientPoint Initialize(IArgumentProvider* provider) const;
			};

			class Shape : public Object
			{
			public:
				Rectangle Position;

				virtual bool IsDefined() const = 0;
				virtual Engine::UI::Shape* Initialize(IArgumentProvider* provider) const = 0;
			};
			class FrameShape : public Shape
			{
			public:
				ObjectArray<Shape> Children;
				Engine::UI::FrameShape::FrameRenderMode RenderMode;
				BasicTemplate<Float64> Opacity;

				FrameShape();
				virtual ~FrameShape() override;
				virtual string ToString() const override;
				virtual bool IsDefined() const override;
				virtual Engine::UI::Shape* Initialize(IArgumentProvider* provider) const override;
			};
			class BarShape : public Shape
			{
			public:
				Array<GradientPoint> Gradient;
				BasicTemplate<Float64> GradientAngle;

				BarShape();
				virtual ~BarShape() override;
				virtual string ToString() const override;
				virtual bool IsDefined() const override;
				virtual Engine::UI::Shape* Initialize(IArgumentProvider* provider) const override;
			};
			class BlurEffectShape : public Shape
			{
			public:
				BasicTemplate<Float64> BlurPower;

				BlurEffectShape();
				virtual ~BlurEffectShape() override;
				virtual string ToString() const override;
				virtual bool IsDefined() const override;
				virtual Engine::UI::Shape* Initialize(IArgumentProvider* provider) const override;
			};
			class InversionEffectShape : public Shape
			{
			public:
				InversionEffectShape();
				virtual ~InversionEffectShape() override;
				virtual string ToString() const override;
				virtual bool IsDefined() const override;
				virtual Engine::UI::Shape* Initialize(IArgumentProvider* provider) const override;
			};
			class TextureShape : public Shape
			{
			public:
				ObjectTemplate<ITexture> Texture;
				Engine::UI::TextureShape::TextureRenderMode RenderMode;
				Rectangle From;

				TextureShape();
				virtual ~TextureShape() override;
				virtual string ToString() const override;
				virtual bool IsDefined() const override;
				virtual Engine::UI::Shape* Initialize(IArgumentProvider* provider) const override;
			};
			class TextShape : public Shape
			{
			public:
				ObjectTemplate<IFont> Font;
				BasicTemplate<string> Text;
				Engine::UI::TextShape::TextHorizontalAlign HorizontalAlign;
				Engine::UI::TextShape::TextVerticalAlign VerticalAlign;
				BasicTemplate<Color> TextColor;

				TextShape();
				virtual ~TextShape() override;
				virtual string ToString() const override;
				virtual bool IsDefined() const override;
				virtual Engine::UI::Shape* Initialize(IArgumentProvider* provider) const override;
			};
			class LineShape : public Shape
			{
			public:
				BasicTemplate<Color> LineColor;
				bool Dotted;

				LineShape();
				virtual ~LineShape() override;
				virtual string ToString() const override;
				virtual bool IsDefined() const override;
				virtual Engine::UI::Shape* Initialize(IArgumentProvider* provider) const override;
			};

			class ControlReflectedBase : public Reflection::Reflected
			{
			public:
				UI::Rectangle ControlPosition;
				virtual string GetTemplateClass() const = 0;
				virtual ~ControlReflectedBase();
			};
			class ControlTemplate : public Object
			{
			public:
				ObjectArray<ControlTemplate> Children;
				ControlReflectedBase* Properties;

				ControlTemplate(ControlReflectedBase* properties);
				virtual ~ControlTemplate() override;
				virtual string ToString() const override;
			};
		}

		class IResourceLoader
		{
		public:
			virtual ITexture* LoadTexture(Streaming::Stream* Source) = 0;
			virtual ITexture* LoadTexture(const string& Name) = 0;
			virtual IFont* LoadFont(const string& FaceName, Int32 Height, Int32 Weight, bool IsItalic, bool IsUnderline, bool IsStrikeout) = 0;
			virtual void ReloadTexture(ITexture* Texture, Streaming::Stream* Source) = 0;
			virtual void ReloadTexture(ITexture* Texture, const string& Name) = 0;
			virtual void ReloadFont(IFont* Font) = 0;
		};
		class IResourceResolver
		{
		public:
			virtual ITexture* GetTexture(const string& Name) = 0;
			virtual IFont* GetFont(const string& Name) = 0;
			virtual Template::Shape* GetApplication(const string& Name) = 0;
			virtual Template::ControlTemplate* GetDialog(const string& Name) = 0;
		};

		class InterfaceTemplate : public Object
		{
		public:
			Dictionary::Dictionary<string, ITexture> Texture;
			Dictionary::Dictionary<string, IFont> Font;
			Dictionary::Dictionary<string, Template::Shape> Application;
			Dictionary::Dictionary<string, Template::ControlTemplate> Dialog;

			InterfaceTemplate();
			virtual ~InterfaceTemplate() override;
			virtual string ToString() const override;
		};
	}
}
