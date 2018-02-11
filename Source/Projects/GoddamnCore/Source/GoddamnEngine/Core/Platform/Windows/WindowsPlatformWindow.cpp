// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file
 * Platform window.
 */
#include <GoddamnEngine/Core/Platform/PlatformWindow.h>
#include <GoddamnEngine/Core/Platform/PlatformApplication.h>
#if GD_PLATFORM_WINDOWS

#include <GoddamnEngine/Core/Templates/SharedPtr.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Containers/Vector.h>

#include <Windows.h>
#include <WindowsX.h>
#include <shellapi.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! System window on Windows platform.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL WindowsPlatformWindow : public IPlatformWindow
	{	
	private:
		struct HwndStateData final
		{
			RECT  DataRect;
			DWORD DataStyle;
			DWORD DataStyleEx;
		};	// struct HwndStateData

		HWND		  m_WindowBackgroundHandle;
		HWND		  m_WindowHandle;
		HwndStateData m_WindowMainStateData;

	public:
		GDINT explicit WindowsPlatformWindow(IWindowDelegate& windowDelegate): m_WindowBackgroundHandle(nullptr)
		{
			WNDCLASSA windowClass = {};
			windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
			windowClass.lpszClassName = "GoddamnWindowClass";
			windowClass.lpfnWndProc = &DelegateWindowMessage;
			RegisterClassA(&windowClass);

			m_WindowHandle = CreateWindowExA(WS_EX_APPWINDOW, "GoddamnWindowClass", "GoddamnWindow"
				, WS_POPUP | WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_BORDER | WS_CAPTION | WS_SIZEBOX | WS_MAXIMIZEBOX
				, 10, 10, 500, 500, nullptr, nullptr, GetModuleHandleA(nullptr), nullptr);
			ShowWindow(m_WindowHandle, SW_SHOW);

			DragAcceptFiles(m_WindowHandle, TRUE);
			SetWindowLongPtrA(m_WindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&windowDelegate));
		}

		/*!
		 * Returns system handle of this window.
		 */
		GDINT virtual CHandle GetSystemHandle() const override final
		{
			return m_WindowHandle;
		}

	public:

		// ------------------------------------------------------------------------------------------
		// Window sizing.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Sets size of the window client area.
		 * 
		 * @param windowSize New window size.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool SetWindowSize(WindowSize const& windowSize) override final
		{
			return false;
		}

		/*!
		 * Sets type of the window.
		 * 
		 * @param windowType New window type.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool SetWindowType(WindowType const windowType) override final
		{
			return false;
		}

		/*!
		 * Sets flags of the window.
		 * 
		 * @param windowFlags New window flags.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool SetWindowFlags(WindowFlags const windowFlags) override final
		{
			return false;
		}

	public:
		GDINT virtual void Update() const override final
		{
			MSG windowMessage = {};
			while (PeekMessage(&windowMessage, m_WindowHandle, 0, 0, PM_REMOVE) != GD_FALSE
				|| PeekMessage(&windowMessage, m_WindowBackgroundHandle, 0, 0, PM_REMOVE) != GD_FALSE)
			{
				TranslateMessage(&windowMessage);
				DispatchMessage(&windowMessage);
			}
		}

		// ------------------------------------------------------------------------------------------
		// Window event delegators.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Delegates window events.
		 *
		 * @param windowSystemHandle Window system handle.
		 * @param windowMessage Window event type.
		 * @param windowWParam First event argument.
		 * @param windowLParam Second event argument.
		 * 
		 * @returns Zero if window message was handled.
		 */
		GDINT static LRESULT CALLBACK DelegateWindowMessage(HWND const windowSystemHandle, UINT const windowMessage, WPARAM const windowWParam, LPARAM const windowLParam)
		{
			auto const windowDelegatePtr = reinterpret_cast<IWindowDelegate*>(GetWindowLongPtrA(windowSystemHandle, GWLP_USERDATA));
			if (windowDelegatePtr == nullptr)
			{
				// No window delegate present, using default window proc.
				return DefWindowProcA(windowSystemHandle, windowMessage, windowWParam, windowLParam);
			}

			auto& windowDelegate = *windowDelegatePtr;
			switch (windowMessage)
			{
				// Window focused event.
				case WM_ACTIVATE:
					{
						if ((LOWORD(windowWParam) & WA_ACTIVE) != 0 || (LOWORD(windowWParam) & WA_CLICKACTIVE) != 0)
						{
							windowDelegate.OnWindowFocused();
						}
						else
						{
							windowDelegate.OnWindowLostFocus();
						}
						return 0;
					}
				// Window moved event.
				case WM_MOVE:
					{
						WindowPosition const windowPosition(GET_X_LPARAM(windowLParam), GET_Y_LPARAM(windowLParam));
						windowDelegate.OnWindowMoved(windowPosition);
						return 0;
					}
				// Window minimized, maximized, restored and resized event.
				case WM_SIZE:
					{
						WindowSize const windowSize(GET_X_LPARAM(windowLParam), GET_Y_LPARAM(windowLParam));
						switch (windowWParam)
						{
							case SIZE_MINIMIZED:
								windowDelegate.OnWindowMinimized();
								break;
							case SIZE_MAXIMIZED:
								windowDelegate.OnWindowMaximized(windowSize);
								break;
							case SIZE_RESTORED:
								{
									//! @todo Corretly determine whether it is a restore event or a resize event
									windowDelegate.OnWindowResized(windowSize);
									windowDelegate.OnWindowRestored();
									break;
								}
							default:
								goto DefaultWindowProc;
						}
						return 0;
					}
				// Window closed event.
				case WM_CLOSE:
					windowDelegate.OnWindowClosed();
					return 0;

				// Mouse button down event.
				case WM_LBUTTONDOWN:
					windowDelegate.OnMouseButtonDown(MouseButton::Left);
					return 0;
				case WM_MBUTTONDOWN:
					windowDelegate.OnMouseButtonDown(MouseButton::Middle);
					return 0;
				case WM_RBUTTONDOWN:
					windowDelegate.OnMouseButtonDown(MouseButton::Right);
					return 0;
				case WM_XBUTTONDOWN:
					{
						auto const mouseButton = (HIWORD(windowWParam) & XBUTTON1) != 0 ? MouseButton::Thumb1 : MouseButton::Thumb2;
						windowDelegate.OnMouseButtonDown(mouseButton);
						return 0;
					}
				// Mouse double click event.
				case WM_LBUTTONDBLCLK:
					windowDelegate.OnMouseButtonDoubleClick(MouseButton::Left);
					return 0;
				case WM_MBUTTONDBLCLK:
					windowDelegate.OnMouseButtonDoubleClick(MouseButton::Middle);
					return 0;
				case WM_RBUTTONDBLCLK:
					windowDelegate.OnMouseButtonDoubleClick(MouseButton::Right);
					return 0;
				case WM_XBUTTONDBLCLK:
					{
						auto const mouseButton = (HIWORD(windowWParam) & XBUTTON1) != 0 ? MouseButton::Thumb1 : MouseButton::Thumb2;
						windowDelegate.OnMouseButtonDoubleClick(mouseButton);
						return 0;
					}
				// Mouse button up event.
				case WM_LBUTTONUP:
					windowDelegate.OnMouseButtonUp(MouseButton::Left);
					return 0;
				case WM_MBUTTONUP:
					windowDelegate.OnMouseButtonUp(MouseButton::Middle);
					return 0;
				case WM_RBUTTONUP:
					windowDelegate.OnMouseButtonUp(MouseButton::Right);
					return 0;
				case WM_XBUTTONUP:
					{
						auto const mouseButton = (HIWORD(windowWParam) & XBUTTON1) != 0 ? MouseButton::Thumb1 : MouseButton::Thumb2;
						windowDelegate.OnMouseButtonUp(mouseButton);
						return 0;
					}
				// Mouse wheel event.
				case WM_MOUSEWHEEL:
					{
						auto const mouseWheelDelta = GET_WHEEL_DELTA_WPARAM(windowWParam) / 120.0f;
						windowDelegate.OnMouseWheel(mouseWheelDelta);
						return 0;
					}
				// Mouse movement event.
				case WM_MOUSEMOVE:
					{
						WindowPosition const mouseRelativePosition(GET_X_LPARAM(windowLParam), GET_Y_LPARAM(windowLParam));
						windowDelegate.OnMouseMovement(mouseRelativePosition);
						return 0;
					}

				// Keyboard button down and up events.
				case WM_KEYDOWN:
				case WM_KEYUP:
					{
						KeyboardButton keyboardButton;
						switch (windowWParam)
						{
							// Tab, Space, Backspace, Caps Lock buttons.
							case VK_TAB:
								keyboardButton = KeyboardButton::Tab;
								break;
							case VK_SPACE:
								keyboardButton = KeyboardButton::Space;
								break;
							case VK_BACK:
								keyboardButton = KeyboardButton::Backspace;
								break;
							case VK_CAPITAL:
								keyboardButton = KeyboardButton::CapsLock;
								break;
							// Returns and Escape buttons.
							case VK_RETURN:
								if ((windowLParam & 0x1000000) != 0)
									keyboardButton = KeyboardButton::NumpadReturn;
								else
									keyboardButton = KeyboardButton::Return;
								break;
							case VK_ESCAPE:
								keyboardButton = KeyboardButton::Escape;
								break;
							// Modifier buttons.
							case VK_SHIFT:
								if ((windowLParam & 0x1000000) != 0)
									keyboardButton = KeyboardButton::RightShift;
								else 
									keyboardButton = KeyboardButton::LeftShift;
								break;
							case VK_CONTROL:
								if ((windowLParam & 0x1000000) != 0) 
									keyboardButton = KeyboardButton::RightControl;
								else 
									keyboardButton = KeyboardButton::LeftControl;
								break;
							case VK_MENU:
								if ((windowLParam & 0x1000000) != 0) 
									keyboardButton = KeyboardButton::RightAlt;
								else 
									keyboardButton = KeyboardButton::LeftAlt;
								break;
							case VK_RWIN:
								keyboardButton = KeyboardButton::RightWindows;
								break;
							case VK_LWIN:
								keyboardButton = KeyboardButton::LeftWindows;
								break;
							// Print Screen and Pause buttons.
							case VK_PAUSE:
								keyboardButton = KeyboardButton::Pause;
								break;
							case VK_SNAPSHOT:
								keyboardButton = KeyboardButton::PrintScreen;
								break;
							// Insert, Delete, Home, End, Page Up and Page Down buttons.
							case VK_INSERT:
								keyboardButton = KeyboardButton::Insert;
								break;
							case VK_DELETE:
								keyboardButton = KeyboardButton::Delete;
								break;
							case VK_HOME:
								keyboardButton = KeyboardButton::Home;
								break;
							case VK_END:
								keyboardButton = KeyboardButton::End;
								break;
							case VK_NEXT:
								keyboardButton = KeyboardButton::PageUp;
								break;
							case VK_PRIOR:
								keyboardButton = KeyboardButton::PageDown;
								break;
							// Arrow buttons.
							case VK_LEFT:
								keyboardButton = KeyboardButton::ArrowLeft;
								break;
							case VK_UP:
								keyboardButton = KeyboardButton::ArrowUp;
								break;
							case VK_RIGHT:
								keyboardButton = KeyboardButton::ArrowRight;
								break;
							case VK_DOWN:
								keyboardButton = KeyboardButton::ArrowDown;
								break;
							// Alpha-numeric buttons.
							case '0':
							case '1':
							case '2':
							case '3':
							case '4':
							case '5':
							case '6':
							case '7':
							case '8':
							case '9':
								keyboardButton = static_cast<KeyboardButton>(windowWParam);
								break;
							case 'A':
							case 'B':
							case 'C':
							case 'D':
							case 'E':
							case 'F':
							case 'G':
							case 'H':
							case 'I':
							case 'J':
							case 'K':
							case 'L':
							case 'M':
							case 'N':
							case 'O':
							case 'P':
							case 'Q':
							case 'R':
							case 'S':
							case 'T':
							case 'U':
							case 'V':
							case 'W':
							case 'X':
							case 'Y':
							case 'Z':
								keyboardButton = static_cast<KeyboardButton>(windowWParam);
								break;
							// OEM buttons.
							case VK_OEM_PLUS:
								keyboardButton = KeyboardButton::Plus;
								break;
							case VK_OEM_MINUS:
								keyboardButton = KeyboardButton::Minus;
								break;
							case VK_OEM_COMMA:
								keyboardButton = KeyboardButton::Comma;
								break;
							case VK_OEM_PERIOD:
								keyboardButton = KeyboardButton::Period;
								break;
							case VK_OEM_1:
								keyboardButton = KeyboardButton::Colon;
								break;
							case VK_OEM_2:
								keyboardButton = KeyboardButton::Slash;
								break;
							case VK_OEM_3:
								keyboardButton = KeyboardButton::Tilde;
								break;
							case VK_OEM_4:
								keyboardButton = KeyboardButton::LeftBracket;
								break;
							case VK_OEM_5:
								keyboardButton = KeyboardButton::Backslash;
								break;
							case VK_OEM_6:
								keyboardButton = KeyboardButton::RigthBracket;
								break;
							case VK_OEM_7:
								keyboardButton = KeyboardButton::Quote;
								break;
							// Numpad buttons.
							case VK_NUMPAD0:
								keyboardButton = KeyboardButton::Numpad0;
								break;
							case VK_NUMPAD1:
								keyboardButton = KeyboardButton::Numpad1;
								break;
							case VK_NUMPAD2:
								keyboardButton = KeyboardButton::Numpad2;
								break;
							case VK_NUMPAD3:
								keyboardButton = KeyboardButton::Numpad3;
								break;
							case VK_NUMPAD4:
								keyboardButton = KeyboardButton::Numpad4;
								break;
							case VK_NUMPAD5:
								keyboardButton = KeyboardButton::Numpad5;
								break;
							case VK_NUMPAD6:
								keyboardButton = KeyboardButton::Numpad6;
								break;
							case VK_NUMPAD7:
								keyboardButton = KeyboardButton::Numpad7;
								break;
							case VK_NUMPAD8:
								keyboardButton = KeyboardButton::Numpad8;
								break;
							case VK_NUMPAD9:
								keyboardButton = KeyboardButton::Numpad9;
								break;
							case VK_ADD:
								keyboardButton = KeyboardButton::NumpadPlus;
								break;
							case VK_SUBTRACT:
								keyboardButton = KeyboardButton::NumpadMinus;
								break;
							case VK_MULTIPLY:
								keyboardButton = KeyboardButton::NumpadAsterisc;
								break;
							case VK_DIVIDE:
								keyboardButton = KeyboardButton::NumpadSlash;
								break;
							case VK_DECIMAL:
								keyboardButton = KeyboardButton::NumpadPeriod;
								break;
							// Functional buttons.
							case VK_F1:
								keyboardButton = KeyboardButton::F1;
								break;
							case VK_F2:
								keyboardButton = KeyboardButton::F2;
								break;
							case VK_F3:
								keyboardButton = KeyboardButton::F3;
								break;
							case VK_F4:
								keyboardButton = KeyboardButton::F4;
								break;
							case VK_F5:
								keyboardButton = KeyboardButton::F5;
								break;
							case VK_F6:
								keyboardButton = KeyboardButton::F6;
								break;
							case VK_F7:
								keyboardButton = KeyboardButton::F7;
								break;
							case VK_F8:
								keyboardButton = KeyboardButton::F8;
								break;
							case VK_F9:
								keyboardButton = KeyboardButton::F8;
								break;
							case VK_F10:
								keyboardButton = KeyboardButton::F10;
								break;
							case VK_F11:
								keyboardButton = KeyboardButton::F11;
								break;
							case VK_F12:
								keyboardButton = KeyboardButton::F12;
								break;
							default:
								goto DefaultWindowProc;
						}
						if (windowMessage == WM_KEYDOWN)
						{
							windowDelegate.OnKeyboardButtonDown(keyboardButton);
						}
						else
						{
							windowDelegate.OnKeyboardButtonUp(keyboardButton);
						}
						return 0;
					}

				// Window drop files event.
				case WM_DROPFILES:
					{
						// http://www.dreamincode.net/forums/topic/293568-drag-and-drop-files-demo-in-c/
						auto const windowDropHandle = reinterpret_cast<HDROP>(windowWParam);
						auto const windowDroppedFilesCount = DragQueryFileW(windowDropHandle, 0xFFFFFFFF, nullptr, 0);
						return 0;
					}

				default:
				DefaultWindowProc:
					return DefWindowProcA(windowSystemHandle, windowMessage, windowWParam, windowLParam);
			}
		}
	};	// class WindowsPlatformWindow

	GDINT SharedPtr<IPlatformWindow> IPlatformApplication::CreateApplicationWindowBase(IWindowDelegate& windowDelegate)
	{
		return gd_new WindowsPlatformWindow(windowDelegate);
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_WINDOWS
