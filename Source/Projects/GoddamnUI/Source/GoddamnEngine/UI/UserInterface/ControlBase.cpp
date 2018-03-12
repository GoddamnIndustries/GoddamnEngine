#include "ControlBase.h"

extern Engine::SafePointer<Engine::Streaming::TextWriter> conout;

namespace Engine
{
	namespace UI
	{
		Window::Window(Window* parent, WindowStation* station)
		{
			Parent.operator=(parent);
			Station.operator=(station);
			WindowPosition = Box(0, 0, 0, 0);
		}
		Window::~Window()
		{
		}
		void Window::Render(const Box& at)
		{
		}
		void Window::ResetCache()
		{
			for (Int32 i = 0; i < Children.GetLength(); i++)
				Children[i]->ResetCache();
		}
		void Window::ArrangeChildren()
		{
		}
		void Window::Enable(bool enable)
		{
		}
		bool Window::IsEnabled()
		{
			return true;
		}
		void Window::Show(bool visible)
		{
		}
		bool Window::IsVisible()
		{
			return true;
		}
		bool Window::IsTabStop()
		{
			return false;
		}
		bool Window::IsOverlapped()
		{
			return false;
		}
		void Window::SetID(Int32 ID)
		{
		}
		Int32 Window::GetID()
		{
			return 0;
		}
		Window* Window::FindChild(Int32 ID)
		{
			return 0;
		}
		void Window::SetRectangle(const Rectangle& rect)
		{
		}
		Rectangle Window::GetRectangle()
		{
			return Rectangle::Invalid();
		}
		void Window::SetPosition(const Box& box)
		{
			WindowPosition = box;
		}
		Box Window::GetPosition()
		{
			return WindowPosition;
		}
		void Window::SetText(const string& text)
		{
		}
		string Window::GetText()
		{
			return L"";
		}
		void Window::RaiseEvent(Int32 ID, Event event, Window* sender)
		{
			if (Parent)
				Parent->RaiseEvent(ID, event, sender);
		}
		void Window::FocusChanged(bool got_focus)
		{
		}
		void Window::CaptureChanged(bool got_capture)
		{
		}
		void Window::LeftButtonDown(Point at)
		{
		}
		void Window::LeftButtonUp(Point at)
		{
		}
		void Window::LeftButtonDoubleClick(Point at)
		{
		}
		void Window::RightButtonDown(Point at)
		{
		}
		void Window::RightButtonUp(Point at)
		{
		}
		void Window::RightButtonDoubleClick(Point at)
		{
		}
		void Window::MouseMove(Point at)
		{
		}
		void Window::ScrollVertically(Int32 delta)
		{
			if (Parent)
				Parent->ScrollVertically(delta);
		}
		void Window::ScrollHorizontally(Int32 delta)
		{
			if (Parent)
				Parent->ScrollHorizontally(delta);
		}
		void Window::KeyDown(Int32 key_code)
		{
		}
		void Window::KeyUp(Int32 key_code)
		{
		}
		void Window::CharDown(UInt32 ucs_code)
		{
		}
		Window* Window::HitTest(Point at)
		{
			return this;
		}
		Window* Window::GetParent()
		{
			return Parent;
		}
		WindowStation* Window::GetStation()
		{
			return Station;
		}
		void Window::SetOrder(DepthOrder order)
		{
			if (!Parent)
				return;
			Int32 index = -1;
			for (Int32 i = 0; i < Parent->Children.GetLength(); i++)
				if (Parent->Children[i].Get() == this)
				{
					index = i;
					break;
				}
			if (index == -1)
				return;
			if (order == DepthOrder::SetFirst)
			{
				for (Int32 i = index + 1; i < Parent->Children.GetLength(); i++)
				{
					Swap(Parent->Children[i - 1], Parent->Children[i]);
				}
			}
			else if (order == DepthOrder::SetLast)
			{
				for (Int32 i = index - 1; i >= 0; i--)
					Swap(Parent->Children[i + 1], Parent->Children[i]);
			}
			else if (order == DepthOrder::MoveUp)
			{
				if (index < Parent->Children.GetLength() - 1)
					Swap(Parent->Children[index + 1], Parent->Children[index]);
			}
			else if (order == DepthOrder::MoveDown)
			{
				if (index > 0)
					Swap(Parent->Children[index - 1], Parent->Children[index]);
			}
		}
		Int32 Window::ChildrenCount()
		{
			return Children.GetLength();
		}
		Window* Window::Child(Int32 index)
		{
			if (index < 0 || index >= Children.GetLength())
				return 0;
			return Children[index];
		}
		void Window::SetFocus()
		{
			Station->SetFocus(this);
		}
		Window* Window::GetFocus()
		{
			return Station->GetFocus();
		}
		void Window::SetCapture()
		{
			Station->SetCapture(this);
		}
		Window* Window::GetCapture()
		{
			return Station->GetCapture();
		}
		void Window::ReleaseCapture()
		{
			Station->ReleaseCapture();
		}
		void Window::Destroy()
		{
			Station->DestroyWindow(this);
		}

		void WindowStation::DeconstructChain(Window* window)
		{
			window->Parent.operator=(0);
			window->Station.operator=(0);
			for (Int32 i = 0; i < window->Children.GetLength(); i++)
				DeconstructChain(window->Children[i]);
			if (CaptureWindow.Get() == window)
				ReleaseCapture();
			if (FocusedWindow.Get() == window)
				SetFocus(TopLevelWindow);
		}
		WindowStation::WindowStation()
		    : Position(0, 0, 0, 0)
		{
			TopLevelWindow.operator=(gd_new Engine::UI::TopLevelWindow(0, this));
		}
		WindowStation::~WindowStation()
		{
			if (TopLevelWindow.Get())
				DeconstructChain(TopLevelWindow);
		}
		void WindowStation::DestroyWindow(Window* window)
		{
			if (!window->Parent.Get())
				return;
			DeconstructChain(window);
			for (Int32 i = 0; i < window->Parent->Children.GetLength(); i++)
				if (window->Parent->Children[i].Get() == window)
				{
					window->Parent->Children.EraseAt(i);
					break;
				}
		}
		void WindowStation::SetBox(const Box& box)
		{
			Position = box;
			if (TopLevelWindow)
			{
				TopLevelWindow->WindowPosition = box;
				TopLevelWindow->ArrangeChildren();
			}
		}
		Box WindowStation::GetBox()
		{
			return Position;
		}
		void WindowStation::Render()
		{
			if (TopLevelWindow.Get())
				TopLevelWindow->Render(Position);
		}
		void WindowStation::ResetCache()
		{
			if (TopLevelWindow.Get())
				TopLevelWindow->ResetCache();
		}
		Engine::UI::TopLevelWindow* WindowStation::GetDesktop()
		{
			return TopLevelWindow;
		}
		Window* WindowStation::HitTest(Point at)
		{
			if (TopLevelWindow.Get())
				return TopLevelWindow->HitTest(at);
			else
				return 0;
		}
		Window* WindowStation::EnabledHitTest(Point at)
		{
			Window* target = (TopLevelWindow.Get()) ? TopLevelWindow->HitTest(at) : 0;
			while (target && !target->IsEnabled())
				target = target->Parent;
			return target;
		}
		void WindowStation::SetRenderingDevice(IRenderingDevice* Device)
		{
			RenderingDevice.operator=(Device);
		}
		IRenderingDevice* WindowStation::GetRenderingDevice()
		{
			return RenderingDevice;
		}
		Point WindowStation::CalculateLocalPoint(Window* window, Point global)
		{
			Point result = global;
			Window* current = window;
			while (current)
			{
				result.x -= current->WindowPosition.Left;
				result.y -= current->WindowPosition.Top;
				current = current->Parent;
			}
			return result;
		}
		Point WindowStation::CalculateGlobalPoint(Window* window, Point local)
		{
			Point result = local;
			Window* current = window;
			while (current)
			{
				result.x += current->WindowPosition.Left;
				result.y += current->WindowPosition.Top;
				current = current->Parent;
			}
			return result;
		}
		void WindowStation::GetMouseTarget(Point global, Window** target, Point* local)
		{
			Window* Target = EnabledHitTest(global);
			*target = Target;
			*local = CalculateLocalPoint(Target, global);
		}
		void WindowStation::SetFocus(Window* window)
		{
			if (window == FocusedWindow.Get())
				return;
			if (FocusedWindow.Get())
				FocusedWindow->FocusChanged(false);
			FocusedWindow.operator=(window);
			if (FocusedWindow.Get())
				FocusedWindow->FocusChanged(true);
		}
		Window* WindowStation::GetFocus()
		{
			return FocusedWindow;
		}
		void WindowStation::SetCapture(Window* window)
		{
			if (window == CaptureWindow.Get())
				return;
			if (CaptureWindow.Get())
				CaptureWindow->CaptureChanged(false);
			CaptureWindow.operator=(window);
			if (CaptureWindow.Get())
			{
				CaptureWindow->CaptureChanged(true);
			}
			MouseMove(GetCursorPos());
		}
		Window* WindowStation::GetCapture()
		{
			return CaptureWindow;
		}
		void WindowStation::ReleaseCapture()
		{
			SetCapture(0);
		}
		void WindowStation::FocusChanged(bool got_focus)
		{
			if (!got_focus)
			{
				if (FocusedWindow.Get())
					FocusedWindow->FocusChanged(false);
				FocusedWindow.operator=(0);
			}
		}
		void WindowStation::CaptureChanged(bool got_capture)
		{
			if (!got_capture)
			{
				if (CaptureWindow.Get())
					CaptureWindow->CaptureChanged(false);
				CaptureWindow.operator=(0);
			}
		}
		void WindowStation::LeftButtonDown(Point at)
		{
			if (CaptureWindow)
				CaptureWindow->LeftButtonDown(CalculateLocalPoint(CaptureWindow, at));
			else
			{
				Window* Target;
				Point At;
				GetMouseTarget(at, &Target, &At);
				if (Target)
					Target->LeftButtonDown(At);
			}
		}
		void WindowStation::LeftButtonUp(Point at)
		{
			if (CaptureWindow)
				CaptureWindow->LeftButtonUp(CalculateLocalPoint(CaptureWindow, at));
			else
			{
				Window* Target;
				Point At;
				GetMouseTarget(at, &Target, &At);
				if (Target)
					Target->LeftButtonUp(At);
			}
		}
		void WindowStation::LeftButtonDoubleClick(Point at)
		{
			if (CaptureWindow != nullptr)
				CaptureWindow->LeftButtonDoubleClick(CalculateLocalPoint(CaptureWindow, at));
			else
			{
				Window* Target;
				Point At;
				GetMouseTarget(at, &Target, &At);
				if (Target)
					Target->LeftButtonDoubleClick(At);
			}
		}
		void WindowStation::RightButtonDown(Point at)
		{
			if (CaptureWindow)
				CaptureWindow->RightButtonDown(CalculateLocalPoint(CaptureWindow, at));
			else
			{
				Window* Target;
				Point At;
				GetMouseTarget(at, &Target, &At);
				if (Target)
					Target->RightButtonDown(At);
			}
		}
		void WindowStation::RightButtonUp(Point at)
		{
			if (CaptureWindow)
				CaptureWindow->RightButtonUp(CalculateLocalPoint(CaptureWindow, at));
			else
			{
				Window* Target;
				Point At;
				GetMouseTarget(at, &Target, &At);
				if (Target)
					Target->RightButtonUp(At);
			}
		}
		void WindowStation::RightButtonDoubleClick(Point at)
		{
			if (CaptureWindow)
				CaptureWindow->RightButtonDoubleClick(CalculateLocalPoint(CaptureWindow, at));
			else
			{
				Window* Target;
				Point At;
				GetMouseTarget(at, &Target, &At);
				if (Target)
					Target->RightButtonDoubleClick(At);
			}
		}
		void WindowStation::MouseMove(Point at)
		{
			if (CaptureWindow)
				CaptureWindow->MouseMove(CalculateLocalPoint(CaptureWindow, at));
			else
			{
				Window* Target;
				Point At;
				GetMouseTarget(at, &Target, &At);
				if (Target)
					Target->MouseMove(At);
			}
		}
		void WindowStation::ScrollVertically(Int32 delta)
		{
			if (CaptureWindow)
				CaptureWindow->ScrollVertically(delta);
			else
			{
				Window* Target = EnabledHitTest(GetCursorPos());
				if (Target)
					Target->ScrollVertically(delta);
			}
		}
		void WindowStation::ScrollHorizontally(Int32 delta)
		{
			if (CaptureWindow)
				CaptureWindow->ScrollHorizontally(delta);
			else
			{
				Window* Target = EnabledHitTest(GetCursorPos());
				if (Target)
					Target->ScrollHorizontally(delta);
			}
		}

		void WindowStation::KeyDown(Int32 key_code)
		{
			if (FocusedWindow)
				FocusedWindow->KeyDown(key_code);
		}
		void WindowStation::KeyUp(Int32 key_code)
		{
			if (FocusedWindow)
				FocusedWindow->KeyUp(key_code);
		}
		void WindowStation::CharDown(UInt32 ucs_code)
		{
			if (FocusedWindow)
				FocusedWindow->CharDown(ucs_code);
		}
		Point WindowStation::GetCursorPos()
		{
			return Point(0, 0);
		}

		ParentWindow::ParentWindow(Window* parent, WindowStation* station)
		    : Window(parent, station)
		{
		}
		Window* ParentWindow::FindChild(Int32 ID)
		{
			if (!ID)
				return 0;
			if (ID == GetID())
				return this;
			for (Int32 i = 0; i < Children.GetLength(); i++)
			{
				auto result = Children[i]->FindChild(ID);
				if (result)
					return result;
			}
			return 0;
		}
		void ParentWindow::Render(const Box& at)
		{
			auto Device = Station->GetRenderingDevice();
			for (Int32 i = 0; i < Children.GetLength(); i++)
			{
				auto& child = Children[i];
				if (!child->IsVisible())
					continue;
				Box rect = Box(child->WindowPosition.Left + at.Left, child->WindowPosition.Top + at.Top, child->WindowPosition.Right + at.Left, child->WindowPosition.Bottom + at.Top);
				Device->PushClip(rect);
				child->Render(rect);
				Device->PopClip();
			}
		}
		void ParentWindow::ArrangeChildren()
		{
			for (Int32 i = 0; i < Children.GetLength(); i++)
			{
				Box inner = Box(0, 0, WindowPosition.Right - WindowPosition.Left, WindowPosition.Bottom - WindowPosition.Top);
				auto rect = Children[i]->GetRectangle();
				if (rect.IsValid())
				{
					Children[i]->SetPosition(Box(rect, inner));
				}
			}
		}
		void ParentWindow::SetPosition(const Box& box)
		{
			Window::SetPosition(box);
			ArrangeChildren();
		}
		Window* ParentWindow::HitTest(Point at)
		{
			for (Int32 i = Children.GetLength() - 1; i >= 0; i--)
			{
				if (!Children[i]->IsVisible())
					continue;
				auto& box = Children[i]->WindowPosition;
				if (box.IsInside(at))
				{
					return Children[i]->HitTest(Point(at.x - box.Left, at.y - box.Top));
				}
			}
			return this;
		}

		TopLevelWindow::TopLevelWindow(Window* parent, WindowStation* station)
		    : ParentWindow(parent, station)
		{
		}
		TopLevelWindow::~TopLevelWindow()
		{
		}
		void TopLevelWindow::Render(const Box& at)
		{
			if (!BackgroundShape && Background)
			{
				ZeroArgumentProvider zeroArgumentProvider;
				BackgroundShape.operator=(Background->Initialize(&zeroArgumentProvider));
			}
			if (BackgroundShape)
			{
				BackgroundShape->Render(GetStation()->GetRenderingDevice(), at);
			}
			ParentWindow::Render(at);
		}
		void TopLevelWindow::ResetCache()
		{
			BackgroundShape.operator=(0);
		}
		Rectangle TopLevelWindow::GetRectangle()
		{
			return Rectangle::Entire();
		}
		Box TopLevelWindow::GetPosition()
		{
			return GetStation()->GetBox();
		}

		bool TopLevelWindow::IsOverlapped()
		{
			return true;
		}

		void TopLevelWindow::RaiseEvent(Int32 ID, Event event, Window* sender)
		{
			//(*conout) << L"DEBUG: Event with ID = " << ID << L", sender = " << string(static_cast<handle>(sender)) << IO::NewLineChar;
		}

		ZeroArgumentProvider::ZeroArgumentProvider()
		{
		}
		void ZeroArgumentProvider::GetArgument(const string& name, Int32* value)
		{
			*value = 0;
		}
		void ZeroArgumentProvider::GetArgument(const string& name, Float64* value)
		{
			*value = 0.0;
		}
		void ZeroArgumentProvider::GetArgument(const string& name, Color* value)
		{
			*value = 0;
		}
		void ZeroArgumentProvider::GetArgument(const string& name, string* value)
		{
			*value = L"";
		}
		void ZeroArgumentProvider::GetArgument(const string& name, ITexture** value)
		{
			*value = 0;
		}
		void ZeroArgumentProvider::GetArgument(const string& name, IFont** value)
		{
			*value = 0;
		}
	}
}
