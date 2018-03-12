#include "StaticControls.h"

namespace Engine
{
	namespace UI
	{
		namespace Controls
		{
			namespace ArgumentService
			{
				class StaticArgumentProvider : public IArgumentProvider
				{
				public:
					Static* Owner;
					StaticArgumentProvider(Static* owner)
					    : Owner(owner)
					{
					}
					virtual void GetArgument(const string& name, Int32* value) override
					{
						*value = 0;
					}
					virtual void GetArgument(const string& name, Float64* value) override
					{
						*value = 0.0;
					}
					virtual void GetArgument(const string& name, Color* value) override
					{
						*value = 0;
					}
					virtual void GetArgument(const string& name, string* value) override
					{
						if (name == L"Text")
							*value = Owner->Text;
						else
							*value = L"";
					}
					virtual void GetArgument(const string& name, ITexture** value) override
					{
						if (name == L"Texture" && Owner->Image)
						{
							*value = Owner->Image;
							(*value)->AddRef();
						}
						else
							*value = 0;
					}
					virtual void GetArgument(const string& name, IFont** value) override
					{
						if (name == L"Font" && Owner->Font)
						{
							*value = Owner->Font;
							(*value)->AddRef();
						}
						else
							*value = 0;
					}
				};
				class ProgressBarArgumentProvider : public IArgumentProvider
				{
				public:
					ProgressBar* Owner;
					ProgressBarArgumentProvider(ProgressBar* owner)
					    : Owner(owner)
					{
					}
					virtual void GetArgument(const string& name, Int32* value) override
					{
						if (name == L"Progress")
							*value = Int32(Owner->Progress);
						else
							*value = 0;
					}
					virtual void GetArgument(const string& name, Float64* value) override
					{
						if (name == L"Progress")
							*value = Owner->Progress;
						else
							*value = 0.0;
					}
					virtual void GetArgument(const string& name, Color* value) override
					{
						*value = 0;
					}
					virtual void GetArgument(const string& name, string* value) override
					{
						*value = L"";
					}
					virtual void GetArgument(const string& name, ITexture** value) override
					{
						*value = 0;
					}
					virtual void GetArgument(const string& name, IFont** value) override
					{
						*value = 0;
					}
				};
				class ColorViewArgumentProvider : public IArgumentProvider
				{
				public:
					ColorView* Owner;
					ColorViewArgumentProvider(ColorView* owner)
					    : Owner(owner)
					{
					}
					virtual void GetArgument(const string& name, Int32* value) override
					{
						*value = 0;
					}
					virtual void GetArgument(const string& name, Float64* value) override
					{
						*value = 0.0;
					}
					virtual void GetArgument(const string& name, Color* value) override
					{
						if (name == L"Color")
							*value = Owner->Color;
						else
							*value = 0;
					}
					virtual void GetArgument(const string& name, string* value) override
					{
						*value = L"";
					}
					virtual void GetArgument(const string& name, ITexture** value) override
					{
						*value = 0;
					}
					virtual void GetArgument(const string& name, IFont** value) override
					{
						*value = 0;
					}
				};
			}

			Static::Static(Window* Parent, WindowStation* Station)
			    : Window(Parent, Station)
			{
				ControlPosition = Rectangle::Invalid();
				Reflection::PropertyZeroInitializer Initializer;
				EnumerateProperties(Initializer);
			}
			Static::Static(Window* Parent, WindowStation* Station, Template::ControlTemplate* Template)
			    : Window(Parent, Station)
			{
				if (Template->Properties->GetTemplateClass() != L"Static")
					throw InvalidArgumentException();
				static_cast<Template::Controls::Static&>(*this) = static_cast<Template::Controls::Static&>(*Template->Properties);
			}
			Static::~Static()
			{
			}
			void Static::Render(const Box& at)
			{
				if (!shape)
				{
					if (View)
					{
						ArgumentService::StaticArgumentProvider staticArgumentProvider(this);
						shape.operator=(View->Initialize(&staticArgumentProvider));
						shape->Render(GetStation()->GetRenderingDevice(), at);
					}
				}
				else
					shape->Render(GetStation()->GetRenderingDevice(), at);
			}
			void Static::ResetCache()
			{
				shape.operator=(0);
			}
			void Static::Enable(bool enable)
			{
			}
			bool Static::IsEnabled()
			{
				return false;
			}
			void Static::Show(bool visible)
			{
				Invisible = !visible;
			}
			bool Static::IsVisible()
			{
				return !Invisible;
			}
			void Static::SetID(Int32 _ID)
			{
				ID = _ID;
			}
			Int32 Static::GetID()
			{
				return ID;
			}
			Window* Static::FindChild(Int32 _ID)
			{
				if (ID == _ID && ID != 0)
					return this;
				else
					return 0;
			}
			void Static::SetRectangle(const Rectangle& rect)
			{
				ControlPosition = rect;
				GetParent()->ArrangeChildren();
			}
			Rectangle Static::GetRectangle()
			{
				return ControlPosition;
			}
			void Static::SetText(const string& text)
			{
				Text = text;
				ResetCache();
			}
			string Static::GetText()
			{
				return Text;
			}
			void Static::SetImage(ITexture* image)
			{
				Image.operator=(image);
				ResetCache();
			}
			ITexture* Static::GetImage()
			{
				return Image;
			}

			ProgressBar::ProgressBar(Window* Parent, WindowStation* Station)
			    : Window(Parent, Station)
			{
				ControlPosition = Rectangle::Invalid();
				Reflection::PropertyZeroInitializer Initializer;
				EnumerateProperties(Initializer);
			}
			ProgressBar::ProgressBar(Window* Parent, WindowStation* Station, Template::ControlTemplate* Template)
			    : Window(Parent, Station)
			{
				if (Template->Properties->GetTemplateClass() != L"ProgressBar")
					throw InvalidArgumentException();
				static_cast<Template::Controls::ProgressBar&>(*this) = static_cast<Template::Controls::ProgressBar&>(*Template->Properties);
			}
			ProgressBar::~ProgressBar()
			{
			}
			void ProgressBar::Render(const Box& at)
			{
				if (!shape)
				{
					if (View)
					{
						ArgumentService::ProgressBarArgumentProvider progressBarArgumentProvider(this);
						shape.operator=(View->Initialize(&progressBarArgumentProvider));
						shape->Render(GetStation()->GetRenderingDevice(), at);
					}
				}
				else
					shape->Render(GetStation()->GetRenderingDevice(), at);
			}
			void ProgressBar::ResetCache()
			{
				shape.operator=(0);
			}
			void ProgressBar::Enable(bool enable)
			{
			}
			bool ProgressBar::IsEnabled()
			{
				return false;
			}
			void ProgressBar::Show(bool visible)
			{
				Invisible = !visible;
			}
			bool ProgressBar::IsVisible()
			{
				return !Invisible;
			}
			void ProgressBar::SetID(Int32 _ID)
			{
				ID = _ID;
			}
			Int32 ProgressBar::GetID()
			{
				return ID;
			}
			Window* ProgressBar::FindChild(Int32 _ID)
			{
				if (ID == _ID && ID != 0)
					return this;
				else
					return 0;
			}
			void ProgressBar::SetRectangle(const Rectangle& rect)
			{
				ControlPosition = rect;
				GetParent()->ArrangeChildren();
			}
			Rectangle ProgressBar::GetRectangle()
			{
				return ControlPosition;
			}
			void ProgressBar::SetValue(Float64 progress)
			{
				Progress = progress;
				ResetCache();
			}
			Float64 ProgressBar::GetValue()
			{
				return Progress;
			}

			ColorView::ColorView(Window* Parent, WindowStation* Station)
			    : Window(Parent, Station)
			{
				ControlPosition = Rectangle::Invalid();
				Reflection::PropertyZeroInitializer Initializer;
				EnumerateProperties(Initializer);
			}
			ColorView::ColorView(Window* Parent, WindowStation* Station, Template::ControlTemplate* Template)
			    : Window(Parent, Station)
			{
				if (Template->Properties->GetTemplateClass() != L"ColorView")
					throw InvalidArgumentException();
				static_cast<Template::Controls::ColorView&>(*this) = static_cast<Template::Controls::ColorView&>(*Template->Properties);
			}
			ColorView::~ColorView()
			{
			}
			void ColorView::Render(const Box& at)
			{
				if (!shape)
				{
					if (View)
					{
						ArgumentService::ColorViewArgumentProvider colorViewArgumentProvider(this);
						shape.operator=(View->Initialize(&colorViewArgumentProvider));
						shape->Render(GetStation()->GetRenderingDevice(), at);
					}
				}
				else
					shape->Render(GetStation()->GetRenderingDevice(), at);
			}
			void ColorView::ResetCache()
			{
				shape.operator=(0);
			}
			void ColorView::Enable(bool enable)
			{
			}
			bool ColorView::IsEnabled()
			{
				return false;
			}
			void ColorView::Show(bool visible)
			{
				Invisible = !visible;
			}
			bool ColorView::IsVisible()
			{
				return !Invisible;
			}
			void ColorView::SetID(Int32 _ID)
			{
				ID = _ID;
			}
			Int32 ColorView::GetID()
			{
				return ID;
			}
			Window* ColorView::FindChild(Int32 _ID)
			{
				if (ID == _ID && ID != 0)
					return this;
				else
					return 0;
			}
			void ColorView::SetRectangle(const Rectangle& rect)
			{
				ControlPosition = rect;
				GetParent()->ArrangeChildren();
			}
			Rectangle ColorView::GetRectangle()
			{
				return ControlPosition;
			}
			void ColorView::SetColor(UI::Color color)
			{
				Color = color;
				ResetCache();
			}
			UI::Color ColorView::GetColor()
			{
				return Color;
			}
		}
	}
}
