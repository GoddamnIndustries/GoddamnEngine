#pragma once

#include "ControlBase.h"
#include "ControlClasses.h"

namespace Engine
{
	namespace UI
	{
		namespace Controls
		{
			class Button : public Window, public Template::Controls::Button
			{
				SafePointer<Shape> _normal;
				SafePointer<Shape> _disabled;
				SafePointer<Shape> _focused;
				SafePointer<Shape> _hot;
				SafePointer<Shape> _pressed;
				Int32 _state;

			public:
				Button(Window* Parent, WindowStation* Station);
				Button(Window* Parent, WindowStation* Station, Template::ControlTemplate* Template);
				~Button() override;

				virtual void Render(const Box& at) override;
				virtual void ResetCache() override;
				virtual void Enable(bool enable) override;
				virtual bool IsEnabled() override;
				virtual void Show(bool visible) override;
				virtual bool IsVisible() override;
				virtual bool IsTabStop() override;
				virtual void SetID(Int32 ID) override;
				virtual Int32 GetID() override;
				virtual Window* FindChild(Int32 ID) override;
				virtual void SetRectangle(const Rectangle& rect) override;
				virtual Rectangle GetRectangle() override;
				virtual void SetText(const string& text) override;
				virtual string GetText() override;
				virtual void FocusChanged(bool got_focus) override;
				virtual void CaptureChanged(bool got_capture) override;
				virtual void LeftButtonDown(Point at) override;
				virtual void LeftButtonUp(Point at) override;
				virtual void MouseMove(Point at) override;
				virtual void KeyDown(Int32 key_code) override;
				virtual void KeyUp(Int32 key_code) override;

				virtual void SetNormalImage(ITexture* Image);
				virtual ITexture* GetNormalImage();
				virtual void SetGrayedImage(ITexture* Image);
				virtual ITexture* GetGrayedImage();
			};
		}
	}
}
