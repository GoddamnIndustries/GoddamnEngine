#pragma once

#include "ControlBase.h"
#include "ControlClasses.h"

namespace Engine
{
	namespace UI
	{
		namespace Controls
		{
			class ControlGroup : public ParentWindow, public Template::Controls::ControlGroup
			{
			private:
				SafePointer<Shape> _background;

			public:
				ControlGroup(Window* Parent, WindowStation* Station);
				ControlGroup(Window* Parent, WindowStation* Station, Template::ControlTemplate* Template);
				~ControlGroup() override;

				virtual void Render(const Box& at) override;
				virtual void ResetCache() override;
				virtual void Enable(bool enable) override;
				virtual bool IsEnabled() override;
				virtual void Show(bool visible) override;
				virtual bool IsVisible() override;
				virtual void SetID(Int32 ID) override;
				virtual Int32 GetID() override;
				virtual void SetRectangle(const Rectangle& rect) override;
				virtual Rectangle GetRectangle() override;
			};
		}
	}
}
