#pragma once

#include "ControlBase.h"
#include "ControlClasses.h"

namespace Engine
{
	namespace UI
	{
		namespace Controls
		{
			class Static : public Window, public Template::Controls::Static
			{
			private:
				SafePointer<Shape> shape;

			public:
				Static(Window* Parent, WindowStation* Station);
				Static(Window* Parent, WindowStation* Station, Template::ControlTemplate* Template);
				~Static() override;

				virtual void Render(const Box& at) override;
				virtual void ResetCache() override;
				virtual void Enable(bool enable) override;
				virtual bool IsEnabled() override;
				virtual void Show(bool visible) override;
				virtual bool IsVisible() override;
				virtual void SetID(Int32 ID) override;
				virtual Int32 GetID() override;
				virtual Window* FindChild(Int32 ID) override;
				virtual void SetRectangle(const Rectangle& rect) override;
				virtual Rectangle GetRectangle() override;
				virtual void SetText(const string& text) override;
				virtual string GetText() override;

				virtual void SetImage(ITexture* Image);
				virtual ITexture* GetImage();
			};
			class ProgressBar : public Window, public Template::Controls::ProgressBar
			{
			private:
				SafePointer<Shape> shape;

			public:
				ProgressBar(Window* Parent, WindowStation* Station);
				ProgressBar(Window* Parent, WindowStation* Station, Template::ControlTemplate* Template);
				~ProgressBar() override;

				virtual void Render(const Box& at) override;
				virtual void ResetCache() override;
				virtual void Enable(bool enable) override;
				virtual bool IsEnabled() override;
				virtual void Show(bool visible) override;
				virtual bool IsVisible() override;
				virtual void SetID(Int32 ID) override;
				virtual Int32 GetID() override;
				virtual Window* FindChild(Int32 ID) override;
				virtual void SetRectangle(const Rectangle& rect) override;
				virtual Rectangle GetRectangle() override;

				virtual void SetValue(Float64 progress);
				virtual Float64 GetValue();
			};
			class ColorView : public Window, public Template::Controls::ColorView
			{
			private:
				SafePointer<Shape> shape;

			public:
				ColorView(Window* Parent, WindowStation* Station);
				ColorView(Window* Parent, WindowStation* Station, Template::ControlTemplate* Template);
				~ColorView() override;

				virtual void Render(const Box& at) override;
				virtual void ResetCache() override;
				virtual void Enable(bool enable) override;
				virtual bool IsEnabled() override;
				virtual void Show(bool visible) override;
				virtual bool IsVisible() override;
				virtual void SetID(Int32 ID) override;
				virtual Int32 GetID() override;
				virtual Window* FindChild(Int32 ID) override;
				virtual void SetRectangle(const Rectangle& rect) override;
				virtual Rectangle GetRectangle() override;

				virtual void SetColor(UI::Color color);
				virtual UI::Color GetColor();
			};
		}
	}
}
