#pragma once

#include "../EngineBase.h"
#include "ShapeBase.h"
#include "Templates.h"

namespace Engine
{
	namespace UI
	{
		class WindowStation;
		class Window;
		class TopLevelWindow;

		class Window : public Object
		{
			friend class WindowStation;
			friend class ParentWindow;

		public:
			enum class DepthOrder
			{
				SetFirst = 0,
				SetLast = 1,
				MoveUp = 2,
				MoveDown = 3
			};
			enum class Event
			{
				Command = 0,
				DoubleClick = 1,
				ContextClick = 2,
				ValueChange = 3
			};

		private:
			ObjectArray<Window> Children;
			SafePointer<Window> Parent;
			SafePointer<WindowStation> Station;

		protected:
			Box WindowPosition;

		public:
			Window(Window* parent, WindowStation* station);
			virtual ~Window() override;
			virtual void Render(const Box& at);
			virtual void ResetCache();
			virtual void ArrangeChildren();
			virtual void Enable(bool enable);
			virtual bool IsEnabled();
			virtual void Show(bool visible);
			virtual bool IsVisible();
			virtual bool IsTabStop();
			virtual bool IsOverlapped();
			virtual void SetID(Int32 ID);
			virtual Int32 GetID();
			virtual Window* FindChild(Int32 ID);
			virtual void SetRectangle(const Rectangle& rect);
			virtual Rectangle GetRectangle();
			virtual void SetPosition(const Box& box);
			virtual Box GetPosition();
			virtual void SetText(const string& text);
			virtual string GetText();
			virtual void RaiseEvent(Int32 ID, Event event, Window* sender);
			virtual void FocusChanged(bool got_focus);
			virtual void CaptureChanged(bool got_capture);
			virtual void LeftButtonDown(Point at);
			virtual void LeftButtonUp(Point at);
			virtual void LeftButtonDoubleClick(Point at);
			virtual void RightButtonDown(Point at);
			virtual void RightButtonUp(Point at);
			virtual void RightButtonDoubleClick(Point at);
			virtual void MouseMove(Point at);
			virtual void ScrollVertically(Int32 delta);
			virtual void ScrollHorizontally(Int32 delta);
			virtual void KeyDown(Int32 key_code);
			virtual void KeyUp(Int32 key_code);
			virtual void CharDown(UInt32 ucs_code);
			virtual Window* HitTest(Point at);

			Window* GetParent();
			WindowStation* GetStation();
			void SetOrder(DepthOrder order);
			Int32 ChildrenCount();
			Window* Child(Int32 index);
			void SetFocus();
			Window* GetFocus();
			void SetCapture();
			Window* GetCapture();
			void ReleaseCapture();
			void Destroy();
		};
		class WindowStation : public Object
		{
		private:
			SafePointer<IRenderingDevice> RenderingDevice;
			SafePointer<Engine::UI::TopLevelWindow> TopLevelWindow;
			SafePointer<Window> CaptureWindow;
			SafePointer<Window> FocusedWindow;
			Box Position;
			void DeconstructChain(Window* window);

		public:
			WindowStation();
			~WindowStation() override;
			template <class W>
			W* CreateWindow(Window* Parent, Template::ControlTemplate* Template)
			{
				if (!Parent)
					Parent = TopLevelWindow;
				auto New = gd_new W(Parent, this, Template);
				Parent->Children.InsertLast(SafePointer<Window>(New));
				return New;
			}
			template <class W>
			W* CreateWindow(Window* Parent)
			{
				if (!Parent)
					Parent = TopLevelWindow;
				auto New = gd_new W(Parent, this);
				Parent->Children.InsertLast(SafePointer<Window>(New));
				return New;
			}
			void DestroyWindow(Window* window);
			void SetBox(const Box& box);
			Box GetBox();
			void Render();
			void ResetCache();
			Engine::UI::TopLevelWindow* GetDesktop();
			Window* HitTest(Point at);
			Window* EnabledHitTest(Point at);
			void SetRenderingDevice(IRenderingDevice* Device);
			IRenderingDevice* GetRenderingDevice();
			Point CalculateLocalPoint(Window* window, Point global);
			Point CalculateGlobalPoint(Window* window, Point local);
			void GetMouseTarget(Point global, Window** target, Point* local);

			virtual void SetFocus(Window* window);
			virtual Window* GetFocus();
			virtual void SetCapture(Window* window);
			virtual Window* GetCapture();
			virtual void ReleaseCapture();
			virtual void FocusChanged(bool got_focus);
			virtual void CaptureChanged(bool got_capture);
			virtual void LeftButtonDown(Point at);
			virtual void LeftButtonUp(Point at);
			virtual void LeftButtonDoubleClick(Point at);
			virtual void RightButtonDown(Point at);
			virtual void RightButtonUp(Point at);
			virtual void RightButtonDoubleClick(Point at);
			virtual void MouseMove(Point at);
			virtual void ScrollVertically(Int32 delta);
			virtual void ScrollHorizontally(Int32 delta);
			virtual void KeyDown(Int32 key_code);
			virtual void KeyUp(Int32 key_code);
			virtual void CharDown(UInt32 ucs_code);
			virtual Point GetCursorPos();
		};
		class ParentWindow : public Window
		{
		public:
			ParentWindow(Window* parent, WindowStation* station);
			virtual Window* FindChild(Int32 ID) override;
			virtual void Render(const Box& at) override;
			virtual void ArrangeChildren() override;
			virtual void SetPosition(const Box& box) override;
			virtual Window* HitTest(Point at) override;
		};
		class TopLevelWindow : public ParentWindow
		{
		private:
			SafePointer<Shape> BackgroundShape;

		public:
			SafePointer<Template::Shape> Background;
			TopLevelWindow(Window* parent, WindowStation* station);
			virtual ~TopLevelWindow() override;
			virtual void Render(const Box& at) override;
			virtual void ResetCache() override;
			virtual Rectangle GetRectangle() override;
			virtual Box GetPosition() override;
			virtual bool IsOverlapped() override;

#pragma message("REMOVE!")
			virtual void RaiseEvent(Int32 ID, Event event, Window* sender) override;
		};
		class ZeroArgumentProvider : public IArgumentProvider
		{
		public:
			ZeroArgumentProvider();
			virtual void GetArgument(const string& name, Int32* value) override;
			virtual void GetArgument(const string& name, Float64* value) override;
			virtual void GetArgument(const string& name, Color* value) override;
			virtual void GetArgument(const string& name, string* value) override;
			virtual void GetArgument(const string& name, ITexture** value) override;
			virtual void GetArgument(const string& name, IFont** value) override;
		};
	}
}
