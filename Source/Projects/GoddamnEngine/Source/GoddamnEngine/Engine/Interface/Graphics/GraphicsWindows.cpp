// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
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
#if GD_PLATFORM_WINDOWS || GD_PLATFORM_WINDOWS_UAP || GD_PLATFORM_WINDOWS_PHONE

#include <GoddamnEngine/Engine/Interface/OutputDevice/OutputDevice.h>
#define GD_DLOG_CAT "GFX device (Windows@Canvas)"

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Helper functions.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GDINT void IGraphicsWindows::_CheckInitialized() const
	{
		GD_DEBUG_ASSERT(hwndMain != nullptr
			, "'IGraphicsWindows::_CheckInitialized' error: it seems that DGX(Windows) was not initialized.");
	}

	GDINT void IGraphicsWindows::_CheckNotInitialized() const
	{
		GD_DEBUG_ASSERT(hwndMain == nullptr
			, "'IGraphicsDX11::_CheckNotInitialized' error: it seems that DGX(Windows) was already initialized.");
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Runtime interface's functionality Implementation.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if GD_PLATFORM_WINDOWS

	// Classes for windows.
#if defined(GD_NAMESPACE)
	CHAR const IGraphicsWindows::hwndBackgroundClassName[] = TEXT(GD_GLUE(GD_STRIGIFY(GD_NAMESPACE), "IGraphicsWindowsBackgroundWindowClass"));
	CHAR const IGraphicsWindows::hwndMainClassName      [] = TEXT(GD_GLUE(GD_STRIGIFY(GD_NAMESPACE), "IGraphicsWindowsMainWindowClass"));
#else	// if defined(GD_NAMESPACE)
	CHAR const IGraphicsWindows::hwndBackgroundClassName[] = TEXT("GoddamnIGraphicsWindowsBackgroundWindowClass");
	CHAR const IGraphicsWindows::hwndMainClassName      [] = TEXT("GoddamnIGraphicsWindowsMainWindowClass");
#endif	// if defined(GD_NAMESPACE)

#else	// if GD_PLATFORM_WINDOWS

	// CLR classes for our window runtime.
	using namespace Windows::System;
	using namespace Windows::Foundation;
	using namespace Windows::UI::Core;
	using namespace Windows::UI::Popups;
	using namespace Windows::ApplicationModel;
	using namespace Windows::ApplicationModel::Core;
	using namespace Windows::ApplicationModel::Activation;
	using namespace Windows::Graphics::Display;
	using namespace Platform;

	ref class App sealed : public IFrameworkView
	{
	public:
		// some functions called by Windows
		virtual void Initialize(CoreApplicationView^ AppView)
		{
			// set the OnActivated function to Handle to Activated "event"
			AppView->Activated += ref new TypedEventHandler
				<CoreApplicationView^, IActivatedEventArgs^>(this, &App::OnActivated);
		}
		virtual void SetWindow(CoreWindow^ Window) {}
		virtual void Load(String^ EntryPoint) {}
		virtual void Run() {}
		virtual void Uninitialize() {}

		// an "event" that is called when the application window is ready to be activated
		void OnActivated(CoreApplicationView^ CoreAppView, IActivatedEventArgs^ Args)
		{
			CoreWindow^ Window = CoreWindow::GetForCurrentThread();
			Window->Activate();
		}
	};

#endif	// if GD_PLATFORM_WINDOWS

	// ------------------------------------------------------------------------------------------
	//! Function would be called on the global initialization step.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsWindows::OnRuntimeInitialize()
	{
		_CheckNotInitialized();
		ConsoleDevice->LogFormat(GD_DLOG_CAT ": going to initialize canvas devices:"
			"\n\tMode       - %d"
			"\n\tResolution - %dx%d@%d..."
			, GfxCanvasMode, GfxResolutionSelected->Width, GfxResolutionSelected->Height
			, GfxResolutionSelected->VsyncNumerator / GfxResolutionSelected->VsyncDenominator);

#if GD_DEBUG
		if (GfxCanvasMode == IGRAPHICS_OUTPUT_MODE_FULLSCREEN)
		{
			// In some cases and GPUs debugging in fullscreen may cause driver crashes or
			// inability to switch back to the canvas window and continue executing the app.
			// Pseudo-fullscreen mode can save for us a lot of health points.
			//
			// P.S. You can still manually set mode to true fullscreen. Happy debugging then.
			GfxCanvasMode = IGRAPHICS_OUTPUT_MODE_PSEUDO_FULLSCREEN;
			ConsoleDevice->LogWarning(GD_DLOG_CAT ": ...default mode was set to be fullscreen - forcedly setting to pseudo-fullscreen for debug purposes.");
		}
#endif	// if GD_DEBUG

		// Loading module instance..
#if 0	// Retrieved HInstance cannot be used for the DirectInput and others.
		hwndHinstance = GetModuleHandle(nullptr);
		GD_ASSERT(hwndHinstance != nullptr
			, "'GetModuleHandle' error: failed to retrieve a module instance.");
#else	// if 0
#	pragma warning(push)
#	pragma warning(disable: 4047)
		// We need the correct HInstance of the GoddamnEngine.dll.
		// http://blogs.msdn.com/b/oldnewthing/archive/2004/10/25/247180.aspx
		hwndHinstance = reinterpret_cast<HINSTANCE>(&__ImageBase);
#	pragma warning(pop)
#endif	// if 0

		// Static background window that paints screen into black when 
		// the Engine is running in pseudo fullscreen mode.
		WNDCLASS hwndBackgroundClass      = { };
		hwndBackgroundClass.lpszClassName = hwndBackgroundClassName;
		hwndBackgroundClass.lpfnWndProc   = &DefWindowProc;
		hwndBackgroundClass.hInstance     = hwndHinstance;
		hwndBackgroundClass.hCursor		  = LoadCursor(nullptr, IDC_WAIT);
		hwndBackgroundClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
		GD_ASSERT(RegisterClass(&hwndBackgroundClass) != 0
			, "'RegisterClass' (background window) function returned zero value with specified parameters.");

		// Main window that handles all system canvas.
		WNDCLASS hwndMainClass            = { };
		hwndMainClass.lpszClassName       = hwndMainClassName;
		hwndMainClass.style               = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		hwndMainClass.lpfnWndProc         = &DefWindowProc;	//! @todo Define external proc for this. We need to Handle input events from it.
		hwndMainClass.hInstance			  = hwndHinstance;
		hwndMainClass.hCursor			  = LoadCursor(nullptr, IDC_WAIT);
		hwndMainClass.hbrBackground       = static_cast<HBRUSH>(GetStockObject(LTGRAY_BRUSH));
		hwndMainClass.hIcon               = LoadIcon(nullptr, IDI_APPLICATION);
		GD_ASSERT(RegisterClass(&hwndMainClass) != 0
			, "'RegisterClass' (main window) function returned zero value with specified parameters.");

		// Loading styles for main window.
		DWORD hwndMainStyle = 0;
		DWORD hwndMainStyleEx = 0;
		GeomRectTp<LONG> hwndRect = { 0, 0, static_cast<LONG>(GfxResolutionSelected->Width), static_cast<LONG>(GfxResolutionSelected->Height) };
		if (GfxCanvasMode == IGRAPHICS_OUTPUT_MODE_WINDOWED)
		{
			hwndMainStyle |= WS_POPUP | WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_BORDER | WS_CAPTION;
			hwndMainStyleEx |= WS_EX_APPWINDOW;

			RECT hwndAdjustedRect = { 0, 0, hwndRect.Width, hwndRect.Height };
			AdjustWindowRectEx(&hwndAdjustedRect, hwndMainStyle, GD_FALSE, hwndMainStyleEx);
			hwndRect.X += hwndAdjustedRect.left + 50;
			hwndRect.Y += hwndAdjustedRect.top + 50;
			hwndRect.Width = hwndAdjustedRect.right - hwndAdjustedRect.left;
			hwndRect.Height = hwndAdjustedRect.bottom - hwndAdjustedRect.top;

			hwndStateData.DataStyle = hwndMainStyle;
			hwndStateData.DataStyleEx = hwndMainStyleEx;
			hwndStateData.DataRect = { hwndRect.X, hwndRect.Y, hwndRect.X + hwndRect.Width, hwndRect.Y + hwndRect.Height };
			hwndStateData.DataWasMaximized = false;
		}
		else
		{
			hwndMainStyle |= WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
			hwndMainStyleEx |= WS_EX_WINDOWEDGE | WS_EX_APPWINDOW;
		}

		hwndBackground = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT
			, hwndBackgroundClassName, "(GoddamnEngine) Black background window."
			, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_DISABLED
			, 0, 0, 0, 0
			, nullptr, nullptr, hwndHinstance, nullptr
			);
		GD_ASSERT(hwndBackground != nullptr, "'CreateWindowEx' (background window) function returned invalid Handle.");
		hwndMain = CreateWindowEx(hwndMainStyleEx
			, hwndMainClassName, "(GoddamnEngine) Main window."
			, hwndMainStyle
			, hwndRect.X, hwndRect.Y, hwndRect.Width, hwndRect.Height
			, hwndBackground, nullptr, hwndHinstance, nullptr
			);
		GD_ASSERT(hwndMain != nullptr, "'CreateWindowEx' (main window) function returned invalid Handle.");
		
		ConsoleDevice->Log(GD_DLOG_CAT ": ... initialized.");
		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Function would be called on the global initialization step, after all other interfaces
	//! are initialized.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsWindows::OnRuntimePostInitialize()
	{
		Canvas_SetMode(GfxCanvasMode, true);
		ShowWindow(hwndMain, SW_SHOW);
		ShowCursor(TRUE);
		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Function would be called on the global deinitialization step.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsWindows::OnRuntimeDeinitialize()
	{
		ShowWindow(hwndBackground, SW_HIDE);

		DestroyWindow(hwndBackground);
		DestroyWindow(hwndMain);
		hwndBackground = nullptr;
		hwndMain = nullptr;

		UnregisterClass(hwndBackgroundClassName, hwndHinstance);
		UnregisterClass(hwndMainClassName, hwndHinstance);

		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Function would be called once per frame.
	GDAPI void IGraphicsWindows::OnRuntimeUpdate()
	{
		_CheckInitialized();

		// Handing all OS messages for our main and background windows.
		MSG hwndMessage = { };
		while (PeekMessage(&hwndMessage, hwndMain, 0, 0, PM_REMOVE) != GD_FALSE
			|| PeekMessage(&hwndMessage, hwndBackground, 0, 0, PM_REMOVE) != GD_FALSE)
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
		return GfxCanvasMode;
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
		ConsoleDevice->LogFormat(GD_DLOG_CAT ": mode is going to be set to %d...", gfxCanvasMode);

		if (GfxCanvasMode != gfxCanvasMode || gfxForcelyReapply)
		{
			ShowWindow(hwndBackground, SW_HIDE);
			switch (gfxCanvasMode)
			{
			case IGRAPHICS_OUTPUT_MODE_WINDOWED:
				{
					ChangeDisplaySettings(nullptr, 0);
					if (gfxCanvasMode != IGRAPHICS_OUTPUT_MODE_UNKNOWN)
					{
						// Restoring the state from saved date.
						SetWindowLong(hwndMain, GWL_STYLE, hwndStateData.DataStyle);
						SetWindowLong(hwndMain, GWL_EXSTYLE, hwndStateData.DataStyleEx);
						SetWindowPos(hwndMain, nullptr
							, hwndStateData.DataRect.left, hwndStateData.DataRect.top
							, hwndStateData.DataRect.right - hwndStateData.DataRect.left
							, hwndStateData.DataRect.bottom - hwndStateData.DataRect.top
							, SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED
							);
						if (hwndStateData.DataWasMaximized)
							SendMessage(hwndMain, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
						ShowWindow(hwndMain, SW_RESTORE);
					}
				} break;

			case IGRAPHICS_OUTPUT_MODE_PSEUDO_FULLSCREEN:
			case IGRAPHICS_OUTPUT_MODE_FULLSCREEN:
				{
					if (gfxCanvasMode == IGRAPHICS_OUTPUT_MODE_WINDOWED)
					{
						// Saving state of the GFX's windowed mode.
						ZeroMemory(&hwndStateData, sizeof(hwndStateData));
						GetWindowRect(hwndMain, &hwndStateData.DataRect);
						hwndStateData.DataStyle = GetWindowLong(hwndMain, GWL_STYLE);
						hwndStateData.DataStyleEx = GetWindowLong(hwndMain, GWL_EXSTYLE);
						hwndStateData.DataWasMaximized = !!IsZoomed(hwndMain);
						if (hwndStateData.DataWasMaximized)
							SendMessage(hwndMain, WM_SYSCOMMAND, SC_RESTORE, 0);

						SetWindowLong(hwndMain, GWL_STYLE, hwndStateData.DataStyle & ~(WS_CAPTION | WS_THICKFRAME));
						SetWindowLong(hwndMain, GWL_EXSTYLE, hwndStateData.DataStyleEx 
							| (WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE));
					}
					
					if (gfxCanvasMode == IGRAPHICS_OUTPUT_MODE_FULLSCREEN)
					{
						// Entering true-fullscreen mode:
						DEVMODE hwndMonitorMode      = {};
						hwndMonitorMode.dmSize       = sizeof(hwndMonitorMode);
						hwndMonitorMode.dmPelsWidth  = GfxResolutionSelected->Width;
						hwndMonitorMode.dmPelsHeight = GfxResolutionSelected->Height;
						hwndMonitorMode.dmBitsPerPel = 32;
						hwndMonitorMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
						ChangeDisplaySettings(&hwndMonitorMode, CDS_FULLSCREEN);
					}
					else
					{
						ChangeDisplaySettings(nullptr, 0);

						// Entering pseudo-fullscreen mode:
						// Selecting the nearest monitor to the one, on which our main window is rendered be default.
						HMONITOR hwndCurrentMonitor = MonitorFromWindow(hwndMain, MONITOR_DEFAULTTONEAREST);
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
						if (hwndCurrentMonitorSize.Height * GfxResolutionSelected->Width <= hwndCurrentMonitorSize.Width * GfxResolutionSelected->Height)
							hwndUpscaledResolution = {	// Vertical upscaling.
								static_cast<LONG>(GfxResolutionSelected->Width * hwndCurrentMonitorSize.Height / GfxResolutionSelected->Height),
								hwndCurrentMonitorSize.Height
							};
						else
							hwndUpscaledResolution = {	// Horizontal upscaling.
								hwndCurrentMonitorSize.Width,
								static_cast<LONG>(GfxResolutionSelected->Height * hwndCurrentMonitorSize.Width / GfxResolutionSelected->Width)
							};

						// Rendering fullscreen black quad, and in the top of it our main window, upscaled to match vertical.
						// We need to refill the background with black color (in case current monitor has different size to previous one.
						ShowWindow(hwndBackground, SW_SHOWNA);
						MoveWindow(hwndBackground
							, hwndCurrentMonitorInfo.rcMonitor.left, hwndCurrentMonitorInfo.rcMonitor.top
							, hwndCurrentMonitorSize.Width, hwndCurrentMonitorSize.Height
							, GD_TRUE	
							);
						ShowWindow(hwndMain, SW_SHOW);
						MoveWindow(hwndMain
							, hwndCurrentMonitorInfo.rcMonitor.left + (hwndCurrentMonitorSize.Width - hwndUpscaledResolution.Width) / 2
							, hwndCurrentMonitorInfo.rcMonitor.top + (hwndCurrentMonitorSize.Height - hwndUpscaledResolution.Height) / 2
							, hwndUpscaledResolution.Width, hwndUpscaledResolution.Height
							, GD_FALSE	
							);
					}
				} break;

			default: 
				GD_DEBUG_ASSERT_FALSE("'IGraphicsWindows::Canvas_SetMode' error: invalid GFX mode specified.");
			}
			GfxCanvasMode = gfxCanvasMode;
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
		return GfxResolutionsList.GetData();
	}
	
	// ------------------------------------------------------------------------------------------
	//! Returns pointer to the resolution descriptor that is currently set.
	//! @returns Pointer to the resolution descriptor that is currently set or null on failure.
	GDAPI GeomResolution const* IGraphicsWindows::Canvas_GetResolution() const
	{
		_CheckInitialized();
		return GfxResolutionSelected;
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
		ConsoleDevice->LogFormat(GD_DLOG_CAT ": resolution is going to be set to %dx%d@%d"
			, GfxResolutionSelected->Width, GfxResolutionSelected->Height
			, GfxResolutionSelected->VsyncNumerator / GfxResolutionSelected->VsyncDenominator);

		if (GfxResolutionSelected != gfxResolution || gfxForcelyReapply)
		{
			if (GfxResolutionSelected->Width != gfxResolution->Width || GfxResolutionSelected->Height != gfxResolution->Height)
			{
				hwndStateData.DataRect.right += gfxResolution->Width - GfxResolutionSelected->Width;
				hwndStateData.DataRect.bottom += gfxResolution->Height - GfxResolutionSelected->Height;
				switch (GfxCanvasMode)
				{
					// Forcing GFX device to switch mode to apply new resolution.
					// Possibly there is a more efficient way do switch resolution, but fuck it.
					case IGRAPHICS_OUTPUT_MODE_FULLSCREEN: 
						Canvas_SetMode(GfxCanvasMode, true);
						break;
					case IGRAPHICS_OUTPUT_MODE_PSEUDO_FULLSCREEN:
						if (GfxResolutionSelected->Width * gfxResolution->Height != GfxResolutionSelected->Height * gfxResolution->Width)
							Canvas_SetMode(GfxCanvasMode, true);
						break;

					// In this case we just update size of the window.
					case IGRAPHICS_OUTPUT_MODE_WINDOWED:
						SetWindowPos(hwndMain, nullptr, 0, 0
							, gfxResolution->Width, gfxResolution->Height
							, SWP_NOMOVE | SWP_NOZORDER);
						break;

					default: 
						break;
				}
			}
			GfxResolutionSelected = gfxResolution;
			return IResult::Ok;
		}
		return IResult::NothingWasDone;
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_WINDOWS || GD_PLATFORM_WINDOWS_UAP || GD_PLATFORM_WINDOWS_PHONE
