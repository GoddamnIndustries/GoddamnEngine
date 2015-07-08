// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/DInterface/DGFX/_DGFXWindows.cpp
//! File contains implementation for the export interface for Windows implementation of the graphics interface.
#include <GoddamnEngine/Include.h>
#if GD_PLATFORM_WINDOWS

#include <GoddamnEngine/DInterface/DGFX/_DGFXWindows.h>
#define GD_DLOG_CAT "GFX device (Windows@Output)"

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Helper functions.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GDINT void _DGFXWindows::_CheckInitialized() const
	{
		GD_DEBUG_ASSERT(_HwndMain != nullptr
			, "'_DGFXWindows::_CheckInitialized' error: it seems that DGX(Windows) was not initialized.");
	}

	GDINT void _DGFXWindows::_CheckNotInitialized() const
	{
		GD_DEBUG_ASSERT(_HwndMain == nullptr
			, "'_DGFXDX11::_CheckNotInitialized' error: it seems that DGX(Windows) was already initialized.");
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Runtime interface's functionality implementation.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// Classes for windows.
#if defined(GD_NAMESPACE)
	CHAR const _DGFXWindows::_HwndBackgroundClassName[] = TEXT(GD_GLUE(GD_STRIGIFY(GD_NAMESPACE), "_DGFXWindowsBackgroundWindowClass"));
	CHAR const _DGFXWindows::_HwndMainClassName      [] = TEXT(GD_GLUE(GD_STRIGIFY(GD_NAMESPACE), "_DGFXWindowsMainWindowClass"));
#else	// if defined(GD_NAMESPACE)
	CHAR const _DGFXWindows::_HwndBackgroundClassName[] = TEXT("Goddamn_DGFXWindowsBackgroundWindowClass");
	CHAR const _DGFXWindows::_HwndMainClassName      [] = TEXT("Goddamn_DGFXWindowsMainWindowClass");
#endif	// if defined(GD_NAMESPACE)

	// ------------------------------------------------------------------------------------------
	//! Function would be called on the global initialization step.
	//! @returns Non-negative value if the operation succeeded.
	GD_DFUNCTION("public DIResult Runtime_Initialize() @nogc nothrow")
	GDAPI DIResult _DGFXWindows::Runtime_Initialize()
	{
		_CheckNotInitialized();
		GD_DLOG(GD_DLOG_CAT ": going to initialize output devices:"
			"\n\tMode       - %d"
			"\n\tResolution - %dx%d@%d..."
			, _GfxOutputMode, _GfxResolutionSelected->Width, _GfxResolutionSelected->Height
			, _GfxResolutionSelected->VsyncNumerator / _GfxResolutionSelected->VsyncDenominator);

#if GD_DEBUG
		if (_GfxOutputMode == DGFX_OUTPUT_MODE_FULLSCREEN)
		{
			// In some cases and GPUs debugging in fullscreen may cause driver crashes or
			// inability to switch back to the output window and continue executing the app.
			// Pseudo-fullscreen mode can save for us a lot of health points.
			//
			// P.S. You can still manually set mode to true fullscreen. Happy debugging then.
			_GfxOutputMode = DGFX_OUTPUT_MODE_PSEUDO_FULLSCREEN;
			GD_DLOG(GD_DLOG_CAT ": ...default mode was set to be fullscreen - forcedly setting to pseudo-fullscreen for debug purposes.");
		}
#endif	// if GD_DEBUG

		// Loading module instance.
		_HwndHinstance = GetModuleHandle(nullptr);
		GD_DASSERT(_HwndHinstance != nullptr
			, "'GetModuleHandle' error: failed to retrieve a module instance.");

		// Static background window that paints screen into black when 
		// the Engine is running in pseudo fullscreen mode.
		WNDCLASS _HwndBackgroundClass      = { };
		_HwndBackgroundClass.lpszClassName = _HwndBackgroundClassName;
		_HwndBackgroundClass.lpfnWndProc   = &DefWindowProc;
		_HwndBackgroundClass.hInstance     = _HwndHinstance;
		_HwndBackgroundClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
		GD_DASSERT(RegisterClass(&_HwndBackgroundClass) != 0
			, "'RegisterClass' (background window) function returned zero value with specified parameters.");

		// Main window that handles all system output.
		WNDCLASS _HwndMainClass            = { };
		_HwndMainClass.lpszClassName       = _HwndMainClassName;
		_HwndMainClass.style               = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		_HwndMainClass.lpfnWndProc         = &DefWindowProc;	//! @todo Define external proc for this. We need to handle input events from it.
		_HwndMainClass.hInstance	       = _HwndHinstance;
		_HwndMainClass.hbrBackground       = static_cast<HBRUSH>(GetStockObject(LTGRAY_BRUSH));
		_HwndMainClass.hIcon               = LoadIcon(nullptr, IDI_APPLICATION);
		GD_DASSERT(RegisterClass(&_HwndMainClass) != 0
			, "'RegisterClass' (main window) function returned zero value with specified parameters.");

		// Loading styles for main window.
		DWORD _HwndMainStyle = 0;
		DWORD _HwndMainStyleEx = 0;
		GeomRectTp<LONG> _HwndRect = { 0, 0, _GfxResolutionSelected->Width, _GfxResolutionSelected->Height };

		if (_GfxOutputMode == DGFX_OUTPUT_MODE_WINDOWED)
		{
			_HwndMainStyle |= WS_POPUP | WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_BORDER | WS_CAPTION;
			_HwndMainStyleEx |= WS_EX_APPWINDOW;

			RECT _HwndAdjustedRect = { 0, 0, _HwndRect.Width, _HwndRect.Height };
			AdjustWindowRectEx(&_HwndAdjustedRect, _HwndMainStyle, GD_FALSE, _HwndMainStyleEx);
			_HwndRect.X += _HwndAdjustedRect.left + 50;
			_HwndRect.Y += _HwndAdjustedRect.top + 50;
			_HwndRect.Width = _HwndAdjustedRect.right - _HwndAdjustedRect.left;
			_HwndRect.Height = _HwndAdjustedRect.bottom - _HwndAdjustedRect.top;

			_HwndStateData.DataStyle = _HwndMainStyle;
			_HwndStateData.DataStyleEx = _HwndMainStyleEx;
			_HwndStateData.DataRect = { _HwndRect.X, _HwndRect.Y, _HwndRect.X + _HwndRect.Width, _HwndRect.Y + _HwndRect.Height };
			_HwndStateData.DataWasMaximized = false;
		}
		else
		{
			_HwndMainStyle |= WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
			_HwndMainStyleEx |= WS_EX_WINDOWEDGE | WS_EX_APPWINDOW;
		}

		_HwndBackground = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT
			, _HwndBackgroundClassName, "(GoddamnEngine) Black background window."
			, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_DISABLED
			, 0, 0, 0, 0
			, nullptr, nullptr, _HwndHinstance, nullptr
			);
		GD_DASSERT(_HwndBackground != nullptr, "'CreateWindowEx' (background window) function returned invalid handle.");
		_HwndMain = CreateWindowEx(_HwndMainStyleEx
			, _HwndMainClassName, "(GoddamnEngine) Main window."	// String::Format("%s%s(GoddamnEngine) Main window.", _GfxTitle.CStr(), _GfxTitle.IsEmpty() ? "" : " | ").CStr()
			, _HwndMainStyle
			, _HwndRect.X, _HwndRect.Y, _HwndRect.Width, _HwndRect.Height
			, _HwndBackground, nullptr, _HwndHinstance, nullptr
			);
		GD_DASSERT(_HwndMain != nullptr, "'CreateWindowEx' (main window) function returned invalid handle.");
		
		GD_DLOG(GD_DLOG_CAT ": ... initialized.");
		return DIResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Function would be called on the global initialization step, after all other interfaces
	//! are initialized.
	//! @returns Non-negative value if the operation succeeded.
	GD_DFUNCTION("public DIResult Runtime_PostInitialize() @nogc nothrow")
	GDAPI DIResult _DGFXWindows::Runtime_PostInitialize()
	{
		this->Output_SetMode(_GfxOutputMode, true);
		ShowWindow(_HwndMain, SW_SHOW);
		return DIResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Function would be called on the global deinitialization step.
	//! @returns Non-negative value if the operation succeeded.
	GD_DFUNCTION("public DIResult Runtime_Deinitialize() @nogc nothrow")
	GDAPI DIResult _DGFXWindows::Runtime_Deinitialize()
	{
		ShowWindow(_HwndBackground, SW_HIDE);

		DestroyWindow(_HwndBackground);
		DestroyWindow(_HwndMain);
		_HwndBackground = nullptr;
		_HwndMain = nullptr;

		UnregisterClass(_HwndBackgroundClassName, _HwndHinstance);
		UnregisterClass(_HwndMainClassName, _HwndHinstance);

		return DIResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	GDAPI void _DGFXWindows::Runtime_Update()
	{
		_CheckInitialized();

		// Handing all OS messages for our main and background windows.
		MSG _HwndMessage = { };
		while (PeekMessage(&_HwndMessage, _HwndMain, 0, 0, PM_REMOVE) != GD_FALSE
			|| PeekMessage(&_HwndMessage, _HwndBackground, 0, 0, PM_REMOVE) != GD_FALSE)
		{
			TranslateMessage(&_HwndMessage);
			DispatchMessage(&_HwndMessage);
		}
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Output functionality management.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ... Output mode ...

	// ------------------------------------------------------------------------------------------
	//! Retrieves the mode in which graphics output device is running. 
	//! @returns The mode in which graphics output device is running.
	GD_DFUNCTION("public DGFXOutputMode Output_GetMode() @nogc nothrow")
	GDAPI DGFXOutputMode _DGFXWindows::Output_GetMode() const
	{
		_CheckInitialized();
		return _GfxOutputMode;
	}

	// ------------------------------------------------------------------------------------------
	//! Changes the mode in which the graphics device is running. 
	//! @returns Non-negative value if the operation succeeded.
	GD_DFUNCTION("public DIResult Output_SetMode(DGFXOutputMode GfxOutputMode, bool GfxForcelyReapply = false) @nogc nothrow")
	GDAPI DIResult _DGFXWindows::Output_SetMode(
		DGFXOutputMode const GfxOutputMode,		//!< The new output mode that would be set.
		bool           const GfxForcelyReapply	//!< Do forcedly reapply mode, even if device is running is the same mode.
	){
		_CheckInitialized();
		GD_DLOG(GD_DLOG_CAT ": mode is going to be set to %d...", GfxOutputMode);

		if (_GfxOutputMode != GfxOutputMode || GfxForcelyReapply)
		{
			ShowWindow(_HwndBackground, SW_HIDE);
			switch (GfxOutputMode)
			{
				case DGFX_OUTPUT_MODE_WINDOWED:
				{
					ChangeDisplaySettings(nullptr, 0);
					if (_GfxOutputMode != DGFX_OUTPUT_MODE_UNKNOWN)
					{
						// Restoring the state from saved date.
						SetWindowLong(_HwndMain, GWL_STYLE, _HwndStateData.DataStyle);
						SetWindowLong(_HwndMain, GWL_EXSTYLE, _HwndStateData.DataStyleEx);
						SetWindowPos(_HwndMain, nullptr
							, _HwndStateData.DataRect.left, _HwndStateData.DataRect.top
							, _HwndStateData.DataRect.right - _HwndStateData.DataRect.left
							, _HwndStateData.DataRect.bottom - _HwndStateData.DataRect.top
							, SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED
							);
						if (_HwndStateData.DataWasMaximized)
							SendMessage(_HwndMain, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
					}
				} break;

				case DGFX_OUTPUT_MODE_PSEUDO_FULLSCREEN:
				case DGFX_OUTPUT_MODE_FULLSCREEN:
				{
					if (_GfxOutputMode == DGFX_OUTPUT_MODE_WINDOWED)
					{
						// Saving state of the GFX's windowed mode.
						ZeroMemory(&_HwndStateData, sizeof(_HwndStateData));
						GetWindowRect(_HwndMain, &_HwndStateData.DataRect);
						_HwndStateData.DataStyle = GetWindowLong(_HwndMain, GWL_STYLE);
						_HwndStateData.DataStyleEx = GetWindowLong(_HwndMain, GWL_EXSTYLE);
						_HwndStateData.DataWasMaximized = !!IsZoomed(_HwndMain);
						if (_HwndStateData.DataWasMaximized)
							SendMessage(_HwndMain, WM_SYSCOMMAND, SC_RESTORE, 0);

						SetWindowLong(_HwndMain, GWL_STYLE, _HwndStateData.DataStyle & ~(WS_CAPTION | WS_THICKFRAME));
						SetWindowLong(_HwndMain, GWL_EXSTYLE, _HwndStateData.DataStyleEx | (WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE));
					}
					
					if (GfxOutputMode == DGFX_OUTPUT_MODE_FULLSCREEN)
					{
						// Entering true-fullscreen mode:
						DEVMODE _HwndMonitorMode      = {};
						_HwndMonitorMode.dmSize       = sizeof(_HwndMonitorMode);
						_HwndMonitorMode.dmPelsWidth  = _GfxResolutionSelected->Width;
						_HwndMonitorMode.dmPelsHeight = _GfxResolutionSelected->Height;
						_HwndMonitorMode.dmBitsPerPel = 32;
						_HwndMonitorMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
						ChangeDisplaySettings(&_HwndMonitorMode, CDS_FULLSCREEN);
					}
					else
					{
						ChangeDisplaySettings(nullptr, 0);

						// Entering pseudo-fullscreen mode:
						// Selecting the nearest monitor to the one, on which our main window is rendered be default.
						HMONITOR _HwndCurrentMonitor = MonitorFromWindow(_HwndMain, MONITOR_DEFAULTTONEAREST);
						MONITORINFO _HwndCurrentMonitorInfo = { sizeof(_HwndCurrentMonitorInfo), 0 };
						GetMonitorInfo(_HwndCurrentMonitor, &_HwndCurrentMonitorInfo);
						GeomSizeTp<LONG> _HwndCurrentMonitorSize = { 
							_HwndCurrentMonitorInfo.rcMonitor.right - _HwndCurrentMonitorInfo.rcMonitor.left,
							_HwndCurrentMonitorInfo.rcMonitor.bottom - _HwndCurrentMonitorInfo.rcMonitor.top,
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
						GeomSizeTp<LONG> _HwndUpscaledResolution;
						if (_HwndCurrentMonitorSize.Height * _GfxResolutionSelected->Width <= _HwndCurrentMonitorSize.Width * _GfxResolutionSelected->Height)
							_HwndUpscaledResolution = {	// Vertical upscaling.
								static_cast<LONG>(_GfxResolutionSelected->Width * _HwndCurrentMonitorSize.Height / _GfxResolutionSelected->Height),
								_HwndCurrentMonitorSize.Height
							};
						else
							_HwndUpscaledResolution = {	// Horizontal upscaling.
								_HwndCurrentMonitorSize.Width,
								static_cast<LONG>(_GfxResolutionSelected->Height * _HwndCurrentMonitorSize.Width / _GfxResolutionSelected->Width)
							};

						// Rendering fullscreen black quad, and in the top of it our main window, upscaled to match vertical.
						// We need to refill the background with black color (in case current monitor has different size to previous one.
						ShowWindow(_HwndBackground, SW_SHOWNA);
						MoveWindow(_HwndBackground
							, _HwndCurrentMonitorInfo.rcMonitor.left, _HwndCurrentMonitorInfo.rcMonitor.top
							, _HwndCurrentMonitorSize.Width, _HwndCurrentMonitorSize.Height
							, GD_TRUE	
							);
						ShowWindow(_HwndMain, SW_SHOW);
						MoveWindow(_HwndMain
							, _HwndCurrentMonitorInfo.rcMonitor.left + (_HwndCurrentMonitorSize.Width - _HwndUpscaledResolution.Width) / 2
							, _HwndCurrentMonitorInfo.rcMonitor.top + (_HwndCurrentMonitorSize.Height - _HwndUpscaledResolution.Height) / 2
							, _HwndUpscaledResolution.Width, _HwndUpscaledResolution.Height
							, GD_FALSE	
							);
					}
				} break;

				default: 
					GD_DEBUG_ASSERT_FALSE("'_DGFXWindows::Output_SetMode' error: invalid GFX mode specified.");
			}
			_GfxOutputMode = GfxOutputMode;
			return DIResult::Ok;
		}
		return DIResult::NothingWasDone;
	}
	
	// ... Output device resolutions ...

	// ------------------------------------------------------------------------------------------
	//! Returns a vector reference with all supported resolution descriptors.
	//! @returns A vector reference with all supported resolution descriptors.
	GD_DFUNCTION("public DVectorRef!GeomResolution Output_GetResolution() @nogc nothrow")
	GDAPI DVectorRef<GeomResolution> _DGFXWindows::Output_GetSupportedResolutions() const
	{
		_CheckInitialized();
		return MakeDVectorRef(_GfxResolutionsList);
	}
	
	// ------------------------------------------------------------------------------------------
	//! Returns pointer to the resolution descriptor that is currently set.
	//! @returns Pointer to the resolution descriptor that is currently set or null on failure.
	GD_DFUNCTION("public const(GeomResolution)* Output_GetResolution() @nogc nothrow")
	GDAPI GeomResolution const* _DGFXWindows::Output_GetResolution() const
	{
		_CheckInitialized();
		return _GfxResolutionSelected;
	}
	
	// ------------------------------------------------------------------------------------------
	//! Changes resolution in which graphics output device is rendering and resize several frame
	//! buffers (those, that have a corresponding flag specified).
	//! @returns Non-negative value if the operation succeeded.
	GD_DFUNCTION("public DIResult Output_SetResolution(const(GeomResolution)* GfxResolution, bool GfxForcelyReapply = false) @nogc nothrow")
	GDAPI DIResult _DGFXWindows::Output_SetResolution(
		GeomResolution const* const GfxResolution,    //!< Pointer to the resolution descriptor that would be set.
		bool                  const GfxForcelyReapply //!< Do forcedly resolution mode, even if device is running is the same resolution.
	){
		GD_NOT_USED_L(GfxResolution, GfxForcelyReapply);
		return DIResult::NotImplemented;
#if 0
		_CheckInitialized();
		GD_DLOG(GD_DLOG_CAT ": resolution is going to be set to %dx%d@%d"
			, _GfxResolutionSelected->Width, _GfxResolutionSelected->Height, _GfxResolutionSelected->VsyncNumerator / _GfxResolutionSelected->VsyncDenominator);
		if (_GfxResolutionSelected != GfxResolution || GfxForcelyReapply)
		{
			if (_GfxResolutionSelected->Width != GfxResolution->Width || _GfxResolutionSelected->Height != GfxResolution->Height)
			{
				_HwndStateData.DataRect.right += GfxResolution->Width - _GfxResolutionSelected->Width;
				_HwndStateData.DataRect.bottom += GfxResolution->Height - _GfxResolutionSelected->Height;
				switch (_GfxOutputMode)
				{
					// Forcing GFX device to switch mode to apply new resolution.
					// Possibly there is a more efficient way do switch resolution, but fuck it.
					case DGFX_OUTPUT_MODE_FULLSCREEN: 
						Output_SetMode(_GfxOutputMode, true);
						break;
					case DGFX_OUTPUT_MODE_PSEUDO_FULLSCREEN:
						if (_GfxResolutionSelected->Width * GfxResolution->Height != _GfxResolutionSelected->Height * GfxResolution->Width)
							Output_SetMode(_GfxOutputMode, true);
						break;

					// In this case we just update size of the window.
					case DGFX_OUTPUT_MODE_WINDOWED:
						SetWindowPos(_HwndMain, nullptr, 0, 0
							, GfxResolution->Width, GfxResolution->Height
							, SWP_NOMOVE | SWP_NOZORDER);
						break;

					default: 
						break;
				}
			}
			_GfxResolutionSelected = GfxResolution;
			return DIResult::Ok;
		}
		return DIResult::NothingWasDone;
#endif	// if 0
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_WINDOWS
