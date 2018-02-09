// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Engine/Interface/Graphics/Graphics/GraphicsWindows.cpp
 * File contains Implementation for the export interface for Windows Implementation of the graphics interface.
 */
#include <GoddamnEngine/Engine/Interface/Graphics/GraphicsWindows.h>
#if GD_PLATFORM_WINDOWS || GD_PLATFORM_WINDOWS_UWP || GD_PLATFORM_WINDOWS_PHONE && 0

//#include <GoddamnEngine/Engine/Interface/OutputDevice/OutputDevice.h>
#define GD_DLOG_CAT "GFX device (Windows@Canvas)"

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Helper functions.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GDINT void IGraphicsWindows::_CheckInitialized() const
	{
		GD_DEBUG_VERIFY(m_HwndMain != nullptr
			, "'IGraphicsWindows::_CheckInitialized' error: it seems that DGX(Windows) was not initialized.");
	}

	GDINT void IGraphicsWindows::_CheckNotInitialized() const
	{
		GD_DEBUG_VERIFY(m_HwndMain == nullptr
			, "'IGraphicsDX11::_CheckNotInitialized' error: it seems that DGX(Windows) was already initialized.");
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Runtime interface's functionality Implementation.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// Classes for windows.
#if defined(GD_NAMESPACE)
	CHAR const IGraphicsWindows::s_HwndBackgroundClassName[] = TEXT(GD_GLUE(GD_STRIGIFY(GD_NAMESPACE), "IGraphicsWindowsBackgroundWindowClass"));
	CHAR const IGraphicsWindows::s_HwndMainClassName      [] = TEXT(GD_GLUE(GD_STRIGIFY(GD_NAMESPACE), "IGraphicsWindowsMainWindowClass"));
#else	// if defined(GD_NAMESPACE)
	CHAR const IGraphicsWindows::hwndBackgroundClassName[] = TEXT("GoddamnIGraphicsWindowsBackgroundWindowClass");
	CHAR const IGraphicsWindows::hwndMainClassName      [] = TEXT("GoddamnIGraphicsWindowsMainWindowClass");
#endif	// if defined(GD_NAMESPACE)

	// ------------------------------------------------------------------------------------------
	//! Function would be called on the global initialization step.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsWindows::OnRuntimeInitialize()
	{
		_CheckNotInitialized();
		/*ConsoleDevice->LogFormat(GD_DLOG_CAT ": going to initialize canvas devices:"
			"\n\tMode       - %d"
			"\n\tResolution - %dx%d@%d..."
			, m_GfxCanvasMode, m_GfxResolutionSelected->Width, m_GfxResolutionSelected->Height
			, m_GfxResolutionSelected->VsyncNumerator / m_GfxResolutionSelected->VsyncDenominator);*/

#if GD_DEBUG
		if (m_GfxCanvasMode == IGRAPHICS_OUTPUT_MODE_FULLSCREEN)
		{
			// In some cases and GPUs debugging in fullscreen may cause driver crashes or
			// inability to switch back to the canvas window and continue executing the app.
			// Pseudo-fullscreen mode can save for us a lot of health points.
			//
			// P.S. You can still manually set mode to true fullscreen. Happy debugging then.
			m_GfxCanvasMode = IGRAPHICS_OUTPUT_MODE_PSEUDO_FULLSCREEN;
			//ConsoleDevice->LogWarning(GD_DLOG_CAT ": ...default mode was set to be fullscreen - forcedly setting to pseudo-fullscreen for debug purposes.");
		}
#endif	// if GD_DEBUG

		// Loading module instance..
#if 0	// Retrieved HInstance cannot be used for the DirectInput and others.
		hwndHinstance = GetModuleHandle(nullptr);
		GD_VERIFY(hwndHinstance != nullptr
			, "'GetModuleHandle' error: failed to retrieve a module instance.");
#else	// if 0
#	pragma warning(push)
#	pragma warning(disable: 4047)
		// We need the correct HInstance of the GoddamnEngine.dll.
		// http://blogs.msdn.com/b/oldnewthing/archive/2004/10/25/247180.aspx
		m_HwndHinstance = reinterpret_cast<HINSTANCE>(&__ImageBase);
#	pragma warning(pop)
#endif	// if 0

		// Static background window that paints screen into black when 
		// the Engine is running in pseudo fullscreen mode.
		WNDCLASS hwndBackgroundClass      = { };
		hwndBackgroundClass.lpszClassName = s_HwndBackgroundClassName;
		hwndBackgroundClass.lpfnWndProc   = &DefWindowProc;
		hwndBackgroundClass.hInstance     = m_HwndHinstance;
		hwndBackgroundClass.hCursor		  = LoadCursor(nullptr, IDC_WAIT);
		hwndBackgroundClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
		GD_VERIFY(RegisterClass(&hwndBackgroundClass) != 0
			, "'RegisterClass' (background window) function returned zero value with specified parameters.");

		// Main window that handles all system canvas.
		WNDCLASS hwndMainClass            = { };
		hwndMainClass.lpszClassName       = s_HwndMainClassName;
		hwndMainClass.style               = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		hwndMainClass.lpfnWndProc         = &DefWindowProc;	//! @todo Define external proc for this. We need to Handle input events from it.
		hwndMainClass.hInstance			  = m_HwndHinstance;
		hwndMainClass.hCursor			  = LoadCursor(nullptr, IDC_WAIT);
		hwndMainClass.hbrBackground       = static_cast<HBRUSH>(GetStockObject(LTGRAY_BRUSH));
		hwndMainClass.hIcon               = LoadIcon(nullptr, IDI_APPLICATION);
		GD_VERIFY(RegisterClass(&hwndMainClass) != 0
			, "'RegisterClass' (main window) function returned zero value with specified parameters.");

		// Loading styles for main window.
		DWORD hwndMainStyle = 0;
		DWORD hwndMainStyleEx = 0;
		GeomRectTp<LONG> hwndRect = { 0, 0, static_cast<LONG>(m_GfxResolutionSelected->Width), static_cast<LONG>(m_GfxResolutionSelected->Height) };
		if (m_GfxCanvasMode == IGRAPHICS_OUTPUT_MODE_WINDOWED)
		{
			hwndMainStyle |= WS_POPUP | WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_BORDER | WS_CAPTION;
			hwndMainStyleEx |= WS_EX_APPWINDOW;

			RECT hwndAdjustedRect = { 0, 0, hwndRect.Width, hwndRect.Height };
			AdjustWindowRectEx(&hwndAdjustedRect, hwndMainStyle, GD_FALSE, hwndMainStyleEx);
			hwndRect.X += hwndAdjustedRect.left + 50;
			hwndRect.Y += hwndAdjustedRect.top + 50;
			hwndRect.Width = hwndAdjustedRect.right - hwndAdjustedRect.left;
			hwndRect.Height = hwndAdjustedRect.bottom - hwndAdjustedRect.top;

			m_HwndStateData.DataStyle = hwndMainStyle;
			m_HwndStateData.DataStyleEx = hwndMainStyleEx;
			m_HwndStateData.DataRect = { hwndRect.X, hwndRect.Y, hwndRect.X + hwndRect.Width, hwndRect.Y + hwndRect.Height };
			m_HwndStateData.DataWasMaximized = false;
		}
		else
		{
			hwndMainStyle |= WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
			hwndMainStyleEx |= WS_EX_WINDOWEDGE | WS_EX_APPWINDOW;
		}

		m_HwndBackground = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT
			, s_HwndBackgroundClassName, "(GoddamnEngine) Black background window."
			, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_DISABLED
			, 0, 0, 0, 0
			, nullptr, nullptr, m_HwndHinstance, nullptr
			);
		GD_VERIFY(m_HwndBackground != nullptr, "'CreateWindowEx' (background window) function returned invalid Handle.");
		m_HwndMain = CreateWindowEx(hwndMainStyleEx
			, s_HwndMainClassName, "(GoddamnEngine) Main window."
			, hwndMainStyle
			, hwndRect.X, hwndRect.Y, hwndRect.Width, hwndRect.Height
			, m_HwndBackground, nullptr, m_HwndHinstance, nullptr
			);
		GD_VERIFY(m_HwndMain != nullptr, "'CreateWindowEx' (main window) function returned invalid Handle.");
		
		//ConsoleDevice->Log(GD_DLOG_CAT ": ... initialized.");
		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Function would be called on the global initialization step, after all other interfaces
	//! are initialized.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsWindows::OnRuntimePostInitialize()
	{
		Canvas_SetMode(m_GfxCanvasMode, true);
		ShowWindow(m_HwndMain, SW_SHOW);
		ShowCursor(TRUE);
		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Function would be called on the global deinitialization step.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsWindows::OnRuntimeDeinitialize()
	{
		ShowWindow(m_HwndBackground, SW_HIDE);

		DestroyWindow(m_HwndBackground);
		DestroyWindow(m_HwndMain);
		m_HwndBackground = nullptr;
		m_HwndMain = nullptr;

		UnregisterClass(s_HwndBackgroundClassName, m_HwndHinstance);
		UnregisterClass(s_HwndMainClassName, m_HwndHinstance);

		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Function would be called once per frame.
	GDAPI void IGraphicsWindows::OnRuntimeUpdate()
	{
		_CheckInitialized();

		// Handing all OS messages for our main and background windows.
		MSG hwndMessage = { };
		while (PeekMessage(&hwndMessage, m_HwndMain, 0, 0, PM_REMOVE) != GD_FALSE
			|| PeekMessage(&hwndMessage, m_HwndBackground, 0, 0, PM_REMOVE) != GD_FALSE)
		{
			TranslateMessage(&hwndMessage);
			DispatchMessage(&hwndMessage);
		}
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Canvas functionality management.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ... Canvas mode ...

	// ------------------------------------------------------------------------------------------
	//! Retrieves the mode in which graphics canvas device is running. 
	//! @returns The mode in which graphics canvas device is running.
	GDAPI IGraphicsCanvasMode IGraphicsWindows::Canvas_GetMode() const
	{
		_CheckInitialized();
		return m_GfxCanvasMode;
	}

	// ------------------------------------------------------------------------------------------
	//! Changes the mode in which the graphics canvas device is running. 
	//! @param GfxCanvasMode The new canvas mode that would be set.
	//! @param gfxForcelyReapply Do forcedly reapply mode, even if device is running is the same mode.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsWindows::Canvas_SetMode(IGraphicsCanvasMode const gfxCanvasMode
		, bool const gfxForcelyReapply /*= false*/)
	{
		_CheckInitialized();
		//ConsoleDevice->LogFormat(GD_DLOG_CAT ": mode is going to be set to %d...", gfxCanvasMode);

		if (m_GfxCanvasMode != gfxCanvasMode || gfxForcelyReapply)
		{
			ShowWindow(m_HwndBackground, SW_HIDE);
			switch (gfxCanvasMode)
			{
			case IGRAPHICS_OUTPUT_MODE_WINDOWED:
				{
					ChangeDisplaySettings(nullptr, 0);
					if (gfxCanvasMode != IGRAPHICS_OUTPUT_MODE_UNKNOWN)
					{
						// Restoring the state from saved date.
						SetWindowLong(m_HwndMain, GWL_STYLE, m_HwndStateData.DataStyle);
						SetWindowLong(m_HwndMain, GWL_EXSTYLE, m_HwndStateData.DataStyleEx);
						SetWindowPos(m_HwndMain, nullptr
							, m_HwndStateData.DataRect.left, m_HwndStateData.DataRect.top
							, m_HwndStateData.DataRect.right - m_HwndStateData.DataRect.left
							, m_HwndStateData.DataRect.bottom - m_HwndStateData.DataRect.top
							, SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED
							);
						if (m_HwndStateData.DataWasMaximized)
							SendMessage(m_HwndMain, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
						ShowWindow(m_HwndMain, SW_RESTORE);
					}
				} break;

			case IGRAPHICS_OUTPUT_MODE_PSEUDO_FULLSCREEN:
			case IGRAPHICS_OUTPUT_MODE_FULLSCREEN:
				{
					if (gfxCanvasMode == IGRAPHICS_OUTPUT_MODE_WINDOWED)
					{
						// Saving state of the GFX's windowed mode.
						::ZeroMemory(&m_HwndStateData, sizeof(m_HwndStateData));
						GetWindowRect(m_HwndMain, &m_HwndStateData.DataRect);
						m_HwndStateData.DataStyle = GetWindowLong(m_HwndMain, GWL_STYLE);
						m_HwndStateData.DataStyleEx = GetWindowLong(m_HwndMain, GWL_EXSTYLE);
						m_HwndStateData.DataWasMaximized = !!IsZoomed(m_HwndMain);
						if (m_HwndStateData.DataWasMaximized)
							SendMessage(m_HwndMain, WM_SYSCOMMAND, SC_RESTORE, 0);

						SetWindowLong(m_HwndMain, GWL_STYLE, m_HwndStateData.DataStyle & ~(WS_CAPTION | WS_THICKFRAME));
						SetWindowLong(m_HwndMain, GWL_EXSTYLE, m_HwndStateData.DataStyleEx 
							| (WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE));
					}
					
					if (gfxCanvasMode == IGRAPHICS_OUTPUT_MODE_FULLSCREEN)
					{
						// Entering true-fullscreen mode:
						DEVMODE hwndMonitorMode      = {};
						hwndMonitorMode.dmSize       = sizeof(hwndMonitorMode);
						hwndMonitorMode.dmPelsWidth  = m_GfxResolutionSelected->Width;
						hwndMonitorMode.dmPelsHeight = m_GfxResolutionSelected->Height;
						hwndMonitorMode.dmBitsPerPel = 32;
						hwndMonitorMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
						ChangeDisplaySettings(&hwndMonitorMode, CDS_FULLSCREEN);
					}
					else
					{
						ChangeDisplaySettings(nullptr, 0);

						// Entering pseudo-fullscreen mode:
						// Selecting the nearest monitor to the one, on which our main window is rendered be default.
						HMONITOR hwndCurrentMonitor = MonitorFromWindow(m_HwndMain, MONITOR_DEFAULTTONEAREST);
						MONITORINFO hwndCurrentMonitorInfo = { sizeof(hwndCurrentMonitorInfo), 0 };
						GetMonitorInfo(hwndCurrentMonitor, &hwndCurrentMonitorInfo);
						GeomSizeTp<LONG> hwndCurrentMonitorSize = { 
							hwndCurrentMonitorInfo.rcMonitor.right - hwndCurrentMonitorInfo.rcMonitor.left,
							hwndCurrentMonitorInfo.rcMonitor.bottom - hwndCurrentMonitorInfo.rcMonitor.top,
						};

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
						GeomSizeTp<LONG> hwndUpscaledResolution;
						if (hwndCurrentMonitorSize.Height * m_GfxResolutionSelected->Width <= hwndCurrentMonitorSize.Width * m_GfxResolutionSelected->Height)
							hwndUpscaledResolution = {	// Vertical upscaling.
								static_cast<LONG>(m_GfxResolutionSelected->Width * hwndCurrentMonitorSize.Height / m_GfxResolutionSelected->Height),
								hwndCurrentMonitorSize.Height
							};
						else
							hwndUpscaledResolution = {	// Horizontal upscaling.
								hwndCurrentMonitorSize.Width,
								static_cast<LONG>(m_GfxResolutionSelected->Height * hwndCurrentMonitorSize.Width / m_GfxResolutionSelected->Width)
							};

						// Rendering fullscreen black quad, and in the top of it our main window, upscaled to match vertical.
						// We need to refill the background with black color (in case current monitor has different size to previous one.
						ShowWindow(m_HwndBackground, SW_SHOWNA);
						MoveWindow(m_HwndBackground
							, hwndCurrentMonitorInfo.rcMonitor.left, hwndCurrentMonitorInfo.rcMonitor.top
							, hwndCurrentMonitorSize.Width, hwndCurrentMonitorSize.Height
							, GD_TRUE	
							);
						ShowWindow(m_HwndMain, SW_SHOW);
						MoveWindow(m_HwndMain
							, hwndCurrentMonitorInfo.rcMonitor.left + (hwndCurrentMonitorSize.Width - hwndUpscaledResolution.Width) / 2
							, hwndCurrentMonitorInfo.rcMonitor.top + (hwndCurrentMonitorSize.Height - hwndUpscaledResolution.Height) / 2
							, hwndUpscaledResolution.Width, hwndUpscaledResolution.Height
							, GD_FALSE	
							);
					}
				} break;

			default: 
				GD_DEBUG_VERIFY_FALSE("'IGraphicsWindows::Canvas_SetMode' error: invalid GFX mode specified.");
			}
			m_GfxCanvasMode = gfxCanvasMode;
			return IResult::Ok;
		}
		return IResult::NothingWasDone;
	}
	
	// ... Canvas device resolutions ...

	// ------------------------------------------------------------------------------------------
	//! Returns a vector reference with all supported resolution descriptors.
	//! @returns A vector reference with all supported resolution descriptors.
	GDAPI GeomResolution const* IGraphicsWindows::Canvas_GetSupportedResolutions() const
	{
		_CheckInitialized();
		return m_GfxResolutionsList.GetData();
	}
	
	// ------------------------------------------------------------------------------------------
	//! Returns pointer to the resolution descriptor that is currently set.
	//! @returns Pointer to the resolution descriptor that is currently set or null on failure.
	GDAPI GeomResolution const* IGraphicsWindows::Canvas_GetResolution() const
	{
		_CheckInitialized();
		return m_GfxResolutionSelected;
	}
	
	// ------------------------------------------------------------------------------------------
	//! Changes resolution in which graphics canvas device is rendering and resize several frame
	//! buffers (those, that have a corresponding flag specified).
	//! @param gfxResolution Pointer to the resolution descriptor that would be set.
	//! @param gfxForcelyReapply Do forcedly resolution mode, even if device is running is the same resolution.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsWindows::Canvas_SetResolution(GeomResolution const* const gfxResolution
		, bool const gfxForcelyReapply /*= false*/)
	{
		_CheckInitialized();
		/*ConsoleDevice->LogFormat(GD_DLOG_CAT ": resolution is going to be set to %dx%d@%d"
			, m_GfxResolutionSelected->Width, m_GfxResolutionSelected->Height
			, m_GfxResolutionSelected->VsyncNumerator / m_GfxResolutionSelected->VsyncDenominator);*/

		if (m_GfxResolutionSelected != gfxResolution || gfxForcelyReapply)
		{
			if (m_GfxResolutionSelected->Width != gfxResolution->Width || m_GfxResolutionSelected->Height != gfxResolution->Height)
			{
				m_HwndStateData.DataRect.right += gfxResolution->Width - m_GfxResolutionSelected->Width;
				m_HwndStateData.DataRect.bottom += gfxResolution->Height - m_GfxResolutionSelected->Height;
				switch (m_GfxCanvasMode)
				{
					// Forcing GFX device to switch mode to apply new resolution.
					// Possibly there is a more efficient way do switch resolution, but fuck it.
					case IGRAPHICS_OUTPUT_MODE_FULLSCREEN: 
						Canvas_SetMode(m_GfxCanvasMode, true);
						break;
					case IGRAPHICS_OUTPUT_MODE_PSEUDO_FULLSCREEN:
						if (m_GfxResolutionSelected->Width * gfxResolution->Height != m_GfxResolutionSelected->Height * gfxResolution->Width)
							Canvas_SetMode(m_GfxCanvasMode, true);
						break;

					// In this case we just update size of the window.
					case IGRAPHICS_OUTPUT_MODE_WINDOWED:
						SetWindowPos(m_HwndMain, nullptr, 0, 0
							, gfxResolution->Width, gfxResolution->Height
							, SWP_NOMOVE | SWP_NOZORDER);
						break;

					default: 
						break;
				}
			}
			m_GfxResolutionSelected = gfxResolution;
			return IResult::Ok;
		}
		return IResult::NothingWasDone;
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_WINDOWS || GD_PLATFORM_WINDOWS_UWP || GD_PLATFORM_WINDOWS_PHONE
