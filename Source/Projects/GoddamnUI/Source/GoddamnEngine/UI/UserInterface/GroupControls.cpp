#include "GroupControls.h"

namespace Engine
{
	namespace UI
	{
		namespace Controls
		{
			ControlGroup::ControlGroup(Window* Parent, WindowStation* Station)
			    : ParentWindow(Parent, Station)
			{
				ControlPosition = Rectangle::Invalid();
				Reflection::PropertyZeroInitializer Initializer;
				EnumerateProperties(Initializer);
			}
			ControlGroup::ControlGroup(Window* Parent, WindowStation* Station, Template::ControlTemplate* Template)
			    : ParentWindow(Parent, Station)
			{
				if (Template->Properties->GetTemplateClass() != L"ControlGroup")
					throw InvalidArgumentException();
				static_cast<Template::Controls::ControlGroup&>(*this) = static_cast<Template::Controls::ControlGroup&>(*Template->Properties);
			}
			ControlGroup::~ControlGroup()
			{
			}
			void ControlGroup::Render(const Box& at)
			{
				ZeroArgumentProvider zeroArgumentProvider;
				if (!_background && Background)
					_background.operator=(Background->Initialize(&zeroArgumentProvider));
				if (_background)
					_background->Render(GetStation()->GetRenderingDevice(), at);
				ParentWindow::Render(at);
			}
			void ControlGroup::ResetCache()
			{
				_background.operator=(0);
				ParentWindow::ResetCache();
			}
			void ControlGroup::Enable(bool enable)
			{
			}
			bool ControlGroup::IsEnabled()
			{
				return true;
			}
			void ControlGroup::Show(bool visible)
			{
				Invisible = !visible;
			}
			bool ControlGroup::IsVisible()
			{
				return !Invisible;
			}
			void ControlGroup::SetID(Int32 _ID)
			{
				ID = _ID;
			}
			Int32 ControlGroup::GetID()
			{
				return ID;
			}
			void ControlGroup::SetRectangle(const Rectangle& rect)
			{
				ControlPosition = rect;
				GetParent()->ArrangeChildren();
			}
			Rectangle ControlGroup::GetRectangle()
			{
				return ControlPosition;
			}
		}
	}
}
