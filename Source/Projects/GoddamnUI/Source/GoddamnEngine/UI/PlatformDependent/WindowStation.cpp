#include "WindowStation.h"

namespace Engine
{
	namespace UI
	{
		HandleWindowStation::HandleWindowStation(HWND window)
		    : _window(window)
		{
		}
		HandleWindowStation::~HandleWindowStation()
		{
		}
		void HandleWindowStation::SetFocus(Window* window)
		{
			if (::SetFocus(_window))
				WindowStation::SetFocus(window);
		}
		Window* HandleWindowStation::GetFocus()
		{
			if (::GetFocus() == _window)
				return WindowStation::GetFocus();
			else
				return 0;
		}
		void HandleWindowStation::SetCapture(Window* window)
		{
			if (window)
				::SetCapture(_window);
			else
				::ReleaseCapture();
			WindowStation::SetCapture(window);
		}
		Window* HandleWindowStation::GetCapture()
		{
			if (::GetCapture() == _window)
				return WindowStation::GetCapture();
			else
				return 0;
		}
		void HandleWindowStation::ReleaseCapture()
		{
			::ReleaseCapture();
			WindowStation::SetCapture(0);
		}
		Point HandleWindowStation::GetCursorPos()
		{
			POINT p;
			::GetCursorPos(&p);
			ScreenToClient(_window, &p);
			return Point(p.x, p.y);
		}
		eint HandleWindowStation::ProcessWindowEvents(UInt32 Msg, eint WParam, eint LParam)
		{
			if (Msg == WM_KEYDOWN || Msg == WM_SYSKEYDOWN)
			{
				KeyDown((Int32)WParam);
			}
			else if (Msg == WM_KEYUP || Msg == WM_SYSKEYUP)
			{
				KeyUp((Int32)WParam);
			}
			else if (Msg == WM_UNICHAR)
			{
				if (WParam == UNICODE_NOCHAR)
					return TRUE;
				else
				{
					CharDown(UInt32(WParam));
					return FALSE;
				}
			}
			else if (Msg == WM_MOUSEMOVE)
			{
				POINTS p = MAKEPOINTS(LParam);
				MouseMove(Point(p.x, p.y));
			}
			else if (Msg == WM_LBUTTONDOWN)
			{
				POINTS p = MAKEPOINTS(LParam);
				LeftButtonDown(Point(p.x, p.y));
			}
			else if (Msg == WM_LBUTTONUP)
			{
				POINTS p = MAKEPOINTS(LParam);
				LeftButtonUp(Point(p.x, p.y));
			}
			else if (Msg == WM_LBUTTONDBLCLK)
			{
				POINTS p = MAKEPOINTS(LParam);
				LeftButtonDoubleClick(Point(p.x, p.y));
			}
			else if (Msg == WM_RBUTTONDOWN)
			{
				POINTS p = MAKEPOINTS(LParam);
				RightButtonDown(Point(p.x, p.y));
			}
			else if (Msg == WM_RBUTTONUP)
			{
				POINTS p = MAKEPOINTS(LParam);
				RightButtonUp(Point(p.x, p.y));
			}
			else if (Msg == WM_RBUTTONDBLCLK)
			{
				POINTS p = MAKEPOINTS(LParam);
				RightButtonDoubleClick(Point(p.x, p.y));
			}
			else if (Msg == WM_MOUSEWHEEL)
			{
				ScrollVertically(GET_WHEEL_DELTA_WPARAM(WParam));
			}
			else if (Msg == WM_MOUSEHWHEEL)
			{
				ScrollHorizontally(GET_WHEEL_DELTA_WPARAM(WParam));
			}
			else if (Msg == WM_SIZE)
			{
				RECT Rect;
				GetClientRect(_window, &Rect);
				SetBox(Box(0, 0, Rect.right, Rect.bottom));
			}
			else if (Msg == WM_SETFOCUS)
			{
				FocusChanged(true);
			}
			else if (Msg == WM_KILLFOCUS)
			{
				FocusChanged(false);
			}
			else if (Msg == WM_CAPTURECHANGED)
			{
				if (reinterpret_cast<HWND>(LParam) != _window)
					CaptureChanged(false);
				else
					CaptureChanged(true);
			}
			return DefWindowProcW(_window, Msg, WParam, LParam);
		}
	}
}
