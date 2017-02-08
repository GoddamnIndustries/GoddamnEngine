// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Interaction/WindowWindows.h
 * Windows window.
 */
#include <GoddamnEngine/Core/Interaction/Window.h>
#include <GoddamnEngine/Core/Interaction/Debug.h>
#if GD_PLATFORM_WINDOWS 

#pragma comment(lib, "opengl32.lib")

GD_NAMESPACE_BEGIN

#if defined(GD_NAMESPACE)
	GDINT CHAR static const g_WindowBackgroundClassName[] = TEXT(GD_GLUE(GD_STRIGIFY(GD_NAMESPACE), "WindowWindowsBackgroundWindowClass"));
	GDINT CHAR static const g_WindowMainClassName[] = TEXT(GD_GLUE(GD_STRIGIFY(GD_NAMESPACE), "WindowWindowsMainWindowClass"));
#else	// if defined(GD_NAMESPACE)
	GDINT CHAR static const g_WindowBackgroundClassName[] = TEXT("Goddamn_WindowWindowsBackgroundWindowClass");
	GDINT CHAR static const g_WindowMainClassName[] = TEXT("Goddamn_WindowWindowsMainWindowClass");
#endif	// if defined(GD_NAMESPACE)

	/*!
	 * Initializes a new generic window.
	 * 
	 * @param windowMode The window mode that would be set.
	 * @param windowFlags The window flags that would be set.
	 * @param windowWidth Window width.
	 * @param windowHeight Window height.
	 */
	GDAPI WindowWindows::WindowWindows(WindowMode const windowMode, WindowFlags const windowFlags, UInt32 const windowWidth, UInt32 const windowHeight)
		: WindowGeneric(windowMode, windowFlags, windowWidth, windowHeight)
		, m_WindowBackgroundHandle(nullptr), m_WindowMainHandle(nullptr), m_WindowMainStateData{}, m_WindowOpenGL(nullptr)
	{
#if GD_DEBUG
		if (m_WindowMode == WindowMode::Fullscreen)
		{
			// In some cases and GPUs debugging in fullscreen may cause driver crashes or
			// inability to switch back to the canvas window and continue executing the app.
			// Pseudo-fullscreen mode can save for us a lot of health points.
			//
			// P.S. You can still manually set mode to true fullscreen. Happy debugging then.
			m_WindowMode = WindowMode::PseudoFullscreen;
			Debug::LogWarning("WindowsWindow: initial mode was set to be fullscreen - forcedly setting to pseudo-fullscreen for debug purposes.");
		}
#endif	// if GD_DEBUG

		auto const windowHinstance = GetModuleHandle(nullptr);

		// Next, trying to register our window classes.
		auto static windowClassesWereRegistered = false;
		if (!windowClassesWereRegistered)
		{
			windowClassesWereRegistered = true;

			// Static background window that paints screen into black when the application is running in pseudo fullscreen mode.
			WNDCLASS windowBackgroundClass = {};
			windowBackgroundClass.lpszClassName = g_WindowBackgroundClassName;
			windowBackgroundClass.lpfnWndProc = &DefWindowProc;
			windowBackgroundClass.hInstance = windowHinstance;
			windowBackgroundClass.hCursor = LoadCursor(nullptr, IDC_WAIT);
			windowBackgroundClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
			GD_VERIFY(RegisterClass(&windowBackgroundClass) != 0, "'RegisterClass' (background window) function returned zero value with specified parameters.");

			// Main window that handles all system canvas.
			WNDCLASS windowMainClass = {};
			windowMainClass.lpszClassName = g_WindowMainClassName;
			windowMainClass.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
			windowMainClass.lpfnWndProc = &DefWindowProc;
			windowMainClass.hInstance = windowHinstance;
			windowMainClass.hCursor = LoadCursor(nullptr, IDC_WAIT);
			windowMainClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(LTGRAY_BRUSH));
			windowMainClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
			GD_VERIFY(RegisterClass(&windowMainClass) != 0, "'RegisterClass' (main window) function returned zero value with specified parameters.");
		}

		// With window classes ready, we can proceed setting up window parameters.
		DWORD windowMainStyle = 0;
		DWORD windowMainStyleEx = 0;
		LONG windowMainX = 0, windowMainY = 0, windowMainWidth = m_WindowWidth, windowMainHeight = m_WindowHeight;
		if (m_WindowMode == WindowMode::Windowed)
		{
			windowMainStyle |= WS_POPUP | WS_OVERLAPPED;
			windowMainStyleEx |= WS_EX_APPWINDOW;
			if ((windowFlags & WindowFlags::Borderless) == 0)
			{
				windowMainStyle |= WS_SYSMENU | WS_MINIMIZEBOX | WS_BORDER | WS_CAPTION;
			}

			RECT windowMainRect = { 0, 0, windowMainWidth, windowMainHeight };
			AdjustWindowRectEx(&windowMainRect, windowMainStyle, FALSE, windowMainStyleEx);
			windowMainX += windowMainRect.left;
			windowMainY += windowMainRect.top;
			windowMainWidth = windowMainRect.right - windowMainRect.left;
			windowMainHeight = windowMainRect.bottom - windowMainRect.top;

			m_WindowMainStateData.DataStyle = windowMainStyle;
			m_WindowMainStateData.DataStyleEx = windowMainStyleEx;
			m_WindowMainStateData.DataRect = { windowMainX, windowMainY, windowMainX + windowMainWidth, windowMainY + windowMainHeight };
		}
		else
		{
			windowMainStyle |= WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
			windowMainStyleEx |= WS_EX_WINDOWEDGE | WS_EX_APPWINDOW;
		}

		// And finally creating the window.
		m_WindowBackgroundHandle = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT
			, g_WindowBackgroundClassName, "(GoddamnEngine) Black background window."
			, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_DISABLED
			, 0, 0, 0, 0
			, nullptr, nullptr, windowHinstance, nullptr
			);
		GD_VERIFY(m_WindowBackgroundHandle != nullptr, "'CreateWindowEx' (background window) function returned invalid Handle.");

		m_WindowMainHandle = CreateWindowEx(windowMainStyleEx
			, g_WindowMainClassName, "(GoddamnEngine) Main window."
			, windowMainStyle
			, windowMainX, windowMainY, windowMainWidth, windowMainHeight
			, m_WindowBackgroundHandle, nullptr, windowHinstance, nullptr
			);
		GD_VERIFY(m_WindowMainHandle != nullptr, "'CreateWindowEx' (main window) function returned invalid Handle.");

		// Initializing OpenGL if it is required.
		if ((windowFlags & WindowFlags::WithOpenGL) != 0)
		{
			m_WindowDeviceContext = GetDC(m_WindowMainHandle);
			GD_VERIFY(m_WindowDeviceContext != nullptr, "'GetDC' (main window) function returned invalid Handle.");

			PIXELFORMATDESCRIPTOR windowPixelFormatDescriptor = {};
			windowPixelFormatDescriptor.nSize = sizeof windowPixelFormatDescriptor;
			windowPixelFormatDescriptor.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
			windowPixelFormatDescriptor.iLayerType = PFD_MAIN_PLANE;
			windowPixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
			windowPixelFormatDescriptor.cColorBits = 32;
			windowPixelFormatDescriptor.cDepthBits = 32;
			auto const windowPixelFormat = ChoosePixelFormat(m_WindowDeviceContext, &windowPixelFormatDescriptor);
			GD_VERIFY(windowPixelFormat != 0, "'ChoosePixelFormat' error: failed to select correct pixel format.");
			SetPixelFormat(m_WindowDeviceContext, windowPixelFormat, &windowPixelFormatDescriptor);

			m_WindowOpenGL = wglCreateContext(m_WindowDeviceContext);
			GD_VERIFY(m_WindowOpenGL != nullptr, "'wglCreateContext' (main window) function returned invalid Handle.");
			
			wglMakeCurrent(m_WindowDeviceContext, m_WindowOpenGL);
		}

		SetWindowMode(m_WindowMode, true);
		::ShowWindow(m_WindowMainHandle, SW_SHOW);
		ShowCursor(TRUE);
	}

	GDAPI WindowWindows::~WindowWindows()
	{
		::ShowWindow(m_WindowBackgroundHandle, SW_HIDE);

		ReleaseDC(m_WindowMainHandle, m_WindowDeviceContext);

		DestroyWindow(m_WindowBackgroundHandle);
		DestroyWindow(m_WindowMainHandle);
		m_WindowBackgroundHandle = nullptr;
		m_WindowMainHandle = nullptr;

		auto const windowHinstance = GetModuleHandle(nullptr);
		UnregisterClass(g_WindowBackgroundClassName, windowHinstance);
		UnregisterClass(g_WindowMainClassName, windowHinstance);
	}

	/*!
	 * Shows or hides a window.
	 * @param doShow Do show or hide window.
	 */
	GDAPI void WindowWindows::ShowWindow(bool const doShow /*= true*/) const
	{
		auto const windowCommand = doShow ? SW_SHOW : SW_HIDE;
		::ShowWindow(m_WindowBackgroundHandle, windowCommand);
		::ShowWindow(m_WindowMainHandle, windowCommand);
	}

	/*!
	 * Changes the mode in which the graphics canvas device is running.
	 *
	 * @param windowMode The new window mode that would be set.
	 * @param doForceReapply Do forcedly reapply mode, even if device is running is the same mode.
	 */
	GDAPI void WindowWindows::SetWindowMode(WindowMode const windowMode, bool const doForceReapply /*= false*/)
	{
		if (m_WindowMode != windowMode || doForceReapply)
		{
			::ShowWindow(m_WindowBackgroundHandle, SW_HIDE);
			switch (windowMode)
			{
				// Switching to the windowed mode.
				case WindowMode::Windowed:
					ChangeDisplaySettings(nullptr, 0);
					if (m_WindowMode != WindowMode::Unknown)
					{
						// Restoring the state from saved date.
						SetWindowLong(m_WindowMainHandle, GWL_STYLE, m_WindowMainStateData.DataStyle);
						SetWindowLong(m_WindowMainHandle, GWL_EXSTYLE, m_WindowMainStateData.DataStyleEx);
						SetWindowPos(m_WindowMainHandle, nullptr
							, m_WindowMainStateData.DataRect.left, m_WindowMainStateData.DataRect.top, m_WindowMainStateData.DataRect.right - m_WindowMainStateData.DataRect.left
							, m_WindowMainStateData.DataRect.bottom - m_WindowMainStateData.DataRect.top
							, SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED
							);
						::ShowWindow(m_WindowMainHandle, SW_RESTORE);
					}
					break;

				// Switching to the fullscreen mode.
				case WindowMode::Fullscreen:
				case WindowMode::PseudoFullscreen:
					if (m_WindowMode == WindowMode::Windowed)
					{
						// Saving state of the GFX's windowed mode.
						m_WindowMainStateData = {};
						m_WindowMainStateData.DataStyle   = GetWindowLong(m_WindowMainHandle, GWL_STYLE);
						m_WindowMainStateData.DataStyleEx = GetWindowLong(m_WindowMainHandle, GWL_EXSTYLE);
						GetWindowRect(m_WindowMainHandle, &m_WindowMainStateData.DataRect);
						SetWindowLong(m_WindowMainHandle, GWL_STYLE, m_WindowMainStateData.DataStyle & ~(WS_CAPTION | WS_THICKFRAME));
						SetWindowLong(m_WindowMainHandle, GWL_EXSTYLE, m_WindowMainStateData.DataStyleEx | (WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE));
					}
					
					if (windowMode == WindowMode::Fullscreen)
					{
						// Entering true-fullscreen mode:
						DEVMODE windowMonitorMode      = {};
						windowMonitorMode.dmSize       = sizeof(windowMonitorMode);
						windowMonitorMode.dmPelsWidth  = m_WindowWidth;
						windowMonitorMode.dmPelsHeight = m_WindowHeight;
						windowMonitorMode.dmBitsPerPel = 32;
						windowMonitorMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
						ChangeDisplaySettings(&windowMonitorMode, CDS_FULLSCREEN);
					}
					else
					{
						// Entering pseudo-fullscreen mode:
						ChangeDisplaySettings(nullptr, 0);

						// Selecting the nearest monitor to the one, on which our main window is rendered be default.
						auto windowCurrentMonitor = MonitorFromWindow(m_WindowMainHandle, MONITOR_DEFAULTTONEAREST);
						MONITORINFO windowCurrentMonitorInfo = { sizeof windowCurrentMonitorInfo, 0 };
						GetMonitorInfo(windowCurrentMonitor, &windowCurrentMonitorInfo);
						auto const windowCurrentMonitorWidth = windowCurrentMonitorInfo.rcMonitor.right - windowCurrentMonitorInfo.rcMonitor.left;
						auto const windowCurrentMonitorHeight = windowCurrentMonitorInfo.rcMonitor.bottom - windowCurrentMonitorInfo.rcMonitor.top;

						// We need to determine, how to upscale our window.
						// +-------------------------------------+    +-----+-------------------------+-----+
						// |                                     |    |     |/////////////////////////|     |
						// +-------------------------------------+    |     |/////////////////////////|     |
						// |/////////////////////////////////////|    |     |/////////////////////////|     |
						// |/////////////////////////////////////|    |     |/////////////////////////|     |
						// |/////////////////////////////////////|    |     |/////////////////////////|     |
						// +-------------------------------------+    |     |/////////////////////////|     |
						// |                                     |    |     |/////////////////////////|     |
						// +-------------------------------------+    +-----+-------------------------+-----+
						//       a. (Vertical upscaling)                     b. (Horizontal upscaling)
						// Vertical upscaling is applied when aspect ratio of the GFX resolution is greater, than the 
						// aspect ratio of the resolution of current monitor.
						LONG windowUpscaledMonitorWidth;
						LONG windowUpscaledMonitorHeight;
						if (windowCurrentMonitorHeight * m_WindowWidth <= windowCurrentMonitorWidth * m_WindowHeight)
						{
							// Vertical upscaling.
							windowUpscaledMonitorWidth = m_WindowWidth * windowCurrentMonitorHeight / m_WindowHeight;
							windowUpscaledMonitorHeight = windowCurrentMonitorHeight;
						}
						else
						{
							// Horizontal upscaling.
							windowUpscaledMonitorWidth = windowCurrentMonitorWidth;
							windowUpscaledMonitorHeight = m_WindowHeight * windowCurrentMonitorWidth / m_WindowWidth;
						}

						// Rendering fullscreen black quad, and in the top of it our main window, upscaled to match vertical.
						// We need to refill the background with black color (in case current monitor has different size to previous one.
						::ShowWindow(m_WindowBackgroundHandle, SW_SHOWNA);
						::MoveWindow(m_WindowBackgroundHandle
							, windowCurrentMonitorInfo.rcMonitor.left, windowCurrentMonitorInfo.rcMonitor.top
							, windowCurrentMonitorWidth, windowCurrentMonitorHeight
							, GD_TRUE	
							);
						::ShowWindow(m_WindowMainHandle, SW_SHOW);
						::MoveWindow(m_WindowMainHandle
							, windowCurrentMonitorInfo.rcMonitor.left + (windowCurrentMonitorWidth - windowUpscaledMonitorWidth) / 2
							, windowCurrentMonitorInfo.rcMonitor.top + (windowCurrentMonitorHeight - windowUpscaledMonitorHeight) / 2
							, windowUpscaledMonitorWidth, windowUpscaledMonitorHeight
							, GD_FALSE	
							);
					}
					break;

				default: 
					GD_DEBUG_VERIFY_FALSE("Invalid window mode was specified.");
			}
			m_WindowMode = windowMode;
		}
	}

	/*!
	 * Retrieves the dimensions in which this window is running.
	 *
	 * @param windowWidth New window width.
	 * @param windowHeight New window height.
	 * @param doForceReapply Do forcedly reapply dimensions, even if device is running is the same mode.
	 */
	GDAPI void WindowWindows::SetWindowDimensions(UInt32 const windowWidth, UInt32 const windowHeight, bool const doForceReapply /*= false*/)
	{
		if (m_WindowWidth != windowWidth || m_WindowHeight != windowHeight || doForceReapply)
		{
			m_WindowMainStateData.DataRect.right += windowWidth - m_WindowWidth;
			m_WindowMainStateData.DataRect.bottom += windowHeight - m_WindowHeight;
			switch (m_WindowMode)
			{
				// Forcing graphics device to switch mode to apply new resolution.
				// Possibly there is a more efficient way do switch resolution, but fuck it.
				case WindowMode::Fullscreen:
					SetWindowMode(m_WindowMode, true);
					break;
				case WindowMode::PseudoFullscreen:
					if (m_WindowWidth * windowHeight != m_WindowHeight * windowWidth)
					{
						SetWindowMode(m_WindowMode, true);
					}
					break;

				// In this case we just update size of the window.
				case WindowMode::Windowed:
					SetWindowPos(m_WindowMainHandle, nullptr, 0, 0, windowWidth, windowHeight, SWP_NOMOVE | SWP_NOZORDER);
					break;

				default:
					break;
			}
			m_WindowWidth = windowWidth;
			m_WindowHeight = windowHeight;
		}
	}

	/*!
	 * Callback being called to update the window.
	 */
	GDAPI void WindowWindows::OnUpdate()
	{
		// Handing all OS messages for our main and background windows.
		MSG windowMessage = {};
		while (PeekMessage(&windowMessage, m_WindowMainHandle, 0, 0, PM_REMOVE) != GD_FALSE
			|| PeekMessage(&windowMessage, m_WindowBackgroundHandle, 0, 0, PM_REMOVE) != GD_FALSE)
		{
			TranslateMessage(&windowMessage);
			DispatchMessage(&windowMessage);
		}
		if (m_WindowOpenGL != nullptr)
		{
			SwapBuffers(m_WindowDeviceContext);
		}
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_WINDOWS
