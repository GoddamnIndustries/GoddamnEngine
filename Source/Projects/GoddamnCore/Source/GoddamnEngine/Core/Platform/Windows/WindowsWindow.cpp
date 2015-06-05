// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Platform/Windows/WindowsWindow.cpp
//! Windows Window definitions.

#include <GoddamnEngine/Core/Platform/Window.h>
#if GD_PLATFORM_WINDOWS 

#include <GoddamnEngine/Core/Platform/Atomics.h>
#include <GoddamnEngine/Core/Platform/Application.h>

#if WINVER > 0x502
#	include <DwmAPI.h>
#endif	// if WINVER > 0x502

GD_NAMESPACE_BEGIN

	// ------------------------------------------------------------------------------------------	
	// WindowsWindow : IMicrosoftWindow interface implementation.
	// ------------------------------------------------------------------------------------------

	//! Name of a window class.
#if defined(GD_NAMESPACE)
	CHAR const WindowsWindow::WndClassName[] = GD_GLUE(GD_STRIGIFY(GD_NAMESPACE), "_WindowsWindow");
#else	// if defined(GD_NAMESPACE)
	CHAR const WindowsWindow::WndClassName[] = "Goddamn_WindowsWindow";
#endif	// if defined(GD_NAMESPACE)

	//! Initializes a window.
	//! @param WndFlags Default flags of a window.
	//! @param WndMode Initial mode of a window.
	//! @param WndRect Initial rectangle of a window.
	//! @param WndTitle Initial title of a window.
	//! @param WndOpacity Initial opacity of a window.
	//! @param WndCornerRadius Default corner radius of a window.
	GDAPI WindowsWindow::WindowsWindow(WindowFlagsTp const WndFlags, WindowModeTp const WndMode, Rectangle const& WndRect, String const& WndTitle, Float32 const WndOpacity, Int32 const WndCornerRadius)
		: IGenericWindow(WndFlags, WndMode, WndRect, WndTitle, WndOpacity, WndCornerRadius)
		, WndNativeHandle(nullptr)
		, WndIUnknownRefCount(0)
		, WndIsVisible(false)
		, WndIsEnabled(false)
	{
		CMemory::Memset(&this->WndPlacementBeforeFullscreen, 0, sizeof(this->WndPlacementBeforeFullscreen));
		CMemory::Memset(&this->WndPlacementBeforeParentMinimized, 0, sizeof(this->WndPlacementBeforeParentMinimized));
		this->WndPlacementBeforeFullscreen.length = sizeof(this->WndPlacementBeforeFullscreen);
		this->WndPlacementBeforeParentMinimized.length = sizeof(this->WndPlacementBeforeParentMinimized);
	}

	//! Initializes a window.
	GDAPI void WindowsWindow::InitializeWindow()
	{
		GD_DEBUG_ASSERT(this->WndNativeHandle == nullptr, "Error: 'WindowWindow' class was already initialized.");

		DWORD WndStyle = 0;
		DWORD WndStyleEx = 0;

		LONG WndLeft = static_cast<LONG>(GD_TRUNC_TO_INT32(this->WndRect.Left));
		LONG WndTop = static_cast<LONG>(GD_TRUNC_TO_INT32(this->WndRect.Top));
		LONG WndClientWidth = static_cast<LONG>(GD_TRUNC_TO_INT32(this->WndRect.Width));
		LONG WndClientHeight = static_cast<LONG>(GD_TRUNC_TO_INT32(this->WndRect.Height));
		LONG WndWindowWidth = static_cast<LONG>(WndClientWidth);
		LONG WndWindowHeight = static_cast<LONG>(WndClientHeight);

		if ((this->WndFlags & WindowFlags::WindowHasBorder) == 0)
		{
			WndStyle |= WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
			WndStyleEx |= WS_EX_WINDOWEDGE;
			WndStyleEx |= (this->WndFlags & WindowFlags::WindowSupportsTransparency	) != 0 ? WS_EX_LAYERED : 0;
			WndStyleEx |= (this->WndFlags & WindowFlags::WindowAppearsInTaskbar		) != 0 ? WS_EX_APPWINDOW : WS_EX_TOOLWINDOW;
			WndStyleEx |= (this->WndFlags & WindowFlags::WindowIsTopmost			) != 0 ? WS_EX_TOPMOST : 0;
			WndStyleEx |= (this->WndFlags & WindowFlags::WindowAcceptsInput			) != 0 ? 0 : WS_EX_TRANSPARENT;
		}
		else
		{
			WndStyle |= WS_POPUP | WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_BORDER | WS_CAPTION;
			WndStyleEx |= WS_EX_APPWINDOW;

			RECT WndWindowRect = { 0, 0, WndClientWidth, WndClientHeight };
			AdjustWindowRectEx(&WndWindowRect, WndStyle, FALSE, WndStyleEx);

			WndLeft += WndWindowRect.left;
			WndTop += WndWindowRect.top;
			WndWindowWidth = WndWindowRect.right - WndWindowRect.left;
			WndWindowHeight = WndWindowRect.bottom - WndWindowRect.top;
		}

		this->WndNativeHandle = CreateWindowExA(WndStyleEx
			, WindowsWindow::WndClassName
			, this->WndTitle.CStr()
			, WndStyle
			, WndLeft, WndTop, WndWindowWidth, WndWindowHeight
			, nullptr	//! @todo Implement window parenting.
			, nullptr, GetApplicationHInstance(), nullptr
			);
		GD_ASSERT(this->WndNativeHandle != nullptr, "Window creation failed! (%s)", GetLastError());

		this->SetWindowRect(this->WndRect);
		this->SetWindowMode(this->WndMode);
	//	this->SetWindowTitle(this->WndTitle);
		if ((this->WndFlags & WindowFlags::WindowSupportsTransparency) == 0)
		{
			this->SetWindowOpacity(this->WndOpacity);
		}
		this->ShowWindow(true);

#if WINVER > 0x502
		if ((this->WndFlags & WindowFlags::WindowHasBorder) == 0)
		{
			DWMNCRENDERINGPOLICY static const WndRenderingPolicy = DWMNCRP_DISABLED;
			BOOL static const WndEnableNCPaint = false;
			GD_ASSERT(SUCCEEDED(DwmSetWindowAttribute(this->WndNativeHandle, DWMWA_NCRENDERING_POLICY, &WndRenderingPolicy, sizeof(WndRenderingPolicy))), "Failed to set rendering policy.");
			GD_ASSERT(SUCCEEDED(DwmSetWindowAttribute(this->WndNativeHandle, DWMWA_ALLOW_NCPAINT, &WndEnableNCPaint, sizeof(WndEnableNCPaint))), "Failed to set NCPaint value.");
		}
#endif	// if WINVER > 0x502

		if (((this->WndFlags & WindowFlags::WindowIsRegular) != 0) && ((this->WndFlags & WindowFlags::WindowHasBorder) == 0))
		{
			WndStyle |= WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
			WndStyle |= (this->WndFlags & WindowFlags::WindowSupportsMaximize) != 0 ? WS_MAXIMIZEBOX : 0;
			WndStyle |= (this->WndFlags & WindowFlags::WindowSupportsMinimize) != 0 ? WS_MINIMIZEBOX : 0;
			WndStyle |= (this->WndFlags & WindowFlags::WindowHasSizingFrame  ) != 0 ? WS_THICKFRAME : 0;

			GD_ASSERT(SetWindowLongA(this->WndNativeHandle, GWL_STYLE, WndStyle) != 0, "Failed to set window style");
			this->AdjustWindowRegion(WndClientWidth, WndClientHeight);
		}

		if ((this->WndFlags & WindowFlags::WindowIsRegular) != 0)
		{
			RegisterDragDrop(this->WndNativeHandle, this);
		}
	}

	//! Destroys a window.
	GDAPI void WindowsWindow::DestroyWindow()
	{
		GD_DEBUG_ASSERT(this->WndNativeHandle != nullptr, "Error: 'WindowWindow' class was not initialized.");

		if (this->WndIUnknownRefCount > 0)
		{
			RevokeDragDrop(this->WndNativeHandle);
			GD_ASSERT(this->WndIUnknownRefCount == 0, "Failed to deinitialize a drug/drop subsystem.");
		}

		::DestroyWindow(this->WndNativeHandle);
		this->WndNativeHandle = nullptr;
	}

	//! Disables or enables a window.
	//! @param WndDoEnable True to enable window, false to disable.
	GDAPI void WindowsWindow::EnableWindow(bool const WndDoEnable)
	{
		GD_DEBUG_ASSERT(this->WndNativeHandle != nullptr, "Error: 'WindowWindow' class was not initialized.");
		
		this->WndIsEnabled = WndDoEnable;
		::EnableWindow(this->WndNativeHandle, this->WndIsEnabled);
	}

	//! Returns true if window is enabled.
	//! @returns True if window is enabled.
	GDAPI bool WindowsWindow::IsWindowEnabled() const
	{
		GD_DEBUG_ASSERT(this->WndNativeHandle != nullptr, "Error: 'WindowWindow' class was not initialized.");

		return this->WndIsEnabled;
	}

	//! Shows or hides a window.
	//! @param DoShow True to show window, false to hide.
	GDAPI void WindowsWindow::ShowWindow(bool const DoShow)
	{
		GD_DEBUG_ASSERT(this->WndNativeHandle != nullptr, "Error: 'WindowWindow' class was not initialized.");

		if (DoShow)
		{
			if (this->WndIsVisible)
			{
				this->WndIsVisible = false;
				bool const WndShouldActivate = (this->WndFlags & (WindowFlags::WindowSupportsMaximize | WindowFlags::WindowActivateWhenFirstShown)) != 0;
				::ShowWindow(this->WndNativeHandle, WndShouldActivate ? SW_SHOW : SW_SHOWNA);
			}
		}
		else
		{
			if (!this->WndIsVisible)
			{
				this->WndIsVisible = true;
				::ShowWindow(this->WndNativeHandle, SW_HIDE);
			}
		}
	}

	//! Returns true if window is shown.
	//! @returns True if window is shown.
	GDAPI bool WindowsWindow::IsWindowShown() const
	{
		GD_DEBUG_ASSERT(this->WndNativeHandle != nullptr, "Error: 'WindowWindow' class was not initialized.");

		return this->WndIsVisible;
	}

	//! Maximizes a window.
	GDAPI void WindowsWindow::MaximizeWindow()
	{
		GD_DEBUG_ASSERT(this->WndNativeHandle != nullptr, "Error: 'WindowWindow' class was not initialized.");

		if ((this->WndFlags & WindowFlags::WindowSupportsMaximize) != 0)
		{
			::ShowWindow(this->WndNativeHandle, SW_MAXIMIZE);
		}
	}

	//! Returns true if window is maximized.
	//! @returns True if window is maximized.
	GDAPI bool WindowsWindow::IsWindowMaximized() const
	{
		GD_DEBUG_ASSERT(this->WndNativeHandle != nullptr, "Error: 'WindowWindow' class was not initialized.");

		if ((this->WndFlags & WindowFlags::WindowSupportsMaximize) != 0)
		{
			return IsZoomed(this->WndNativeHandle) == TRUE;
		}
		return false;
	}

	//! Minimizes a window.
	GDAPI void WindowsWindow::MininizeWindow()
	{
		GD_DEBUG_ASSERT(this->WndNativeHandle != nullptr, "Error: 'WindowWindow' class was not initialized.");

		if ((this->WndFlags & WindowFlags::WindowSupportsMinimize) != 0)
		{
			::ShowWindow(this->WndNativeHandle, SW_MINIMIZE);
		}
	}

	//! Returns true if window is minimized.
	//! @returns True if window is minimized.
	GDAPI bool WindowsWindow::IsWindowMinimized() const
	{
		GD_DEBUG_ASSERT(this->WndNativeHandle != nullptr, "Error: 'WindowWindow' class was not initialized.");

		if ((this->WndFlags & WindowFlags::WindowSupportsMinimize) != 0)
		{
			return IsIconic(this->WndNativeHandle) == TRUE;
		}
		return false;
	}

	//! Makes window a top-most window.
	//! @param Force Do bring window to front forcedly (via hacks).
	GDAPI void WindowsWindow::BringWindowToFront(bool const Force /*= false*/)
	{
		GD_DEBUG_ASSERT(this->WndNativeHandle != nullptr, "Error: 'WindowWindow' class was not initialized.");

		if ((this->WndFlags & WindowFlags::WindowIsRegular) != 0)
		{
			if (this->IsWindowMinimized())
			{
				this->RestoreWindow();
			}
			else
			{
				SetActiveWindow(this->WndNativeHandle);
			}
		}
		else
		{
			HWND const WndInsertAfter = (this->WndFlags & WindowFlags::WindowIsTopmost) != 0 ? HWND_TOPMOST : HWND_TOP;
			DWORD const WndFlags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | (Force ? 0 : SWP_NOACTIVATE);
			GD_ASSERT(SetWindowPos(this->WndNativeHandle, WndInsertAfter, 0, 0, 0, 0, WndFlags), "Failed to bring window to front.");
		}
	}

	//! Returns true if window is top most.
	//! @returns True if window is foreground.
	GDAPI bool WindowsWindow::IsWindowForeground() const
	{
		GD_DEBUG_ASSERT(this->WndNativeHandle != nullptr, "Error: 'WindowWindow' class was not initialized.");

		return GetForegroundWindow() == this->WndNativeHandle;
	}

	//! Restores a window (?!)
	GDAPI void WindowsWindow::RestoreWindow()
	{
		GD_DEBUG_ASSERT(this->WndNativeHandle != nullptr, "Error: 'WindowWindow' class was not initialized.");

		::ShowWindow(this->WndNativeHandle, SW_RESTORE);
	}

	//! Returns mode of a window.
	//! @returns Mode of a window.
	GDAPI WindowModeTp WindowsWindow::GetWindowMode() const
	{
		return this->WndMode;
	}

	//! Applies new mode to a window.
	//! @param WndMode New mode of a window.
	GDAPI void WindowsWindow::SetWindowMode(WindowModeTp const WndMode)
	{
		GD_DEBUG_ASSERT(this->WndNativeHandle != nullptr, "Error: 'WindowWindow' class was not initialized.");

		if (this->WndMode != WndMode)
		{
			this->WndMode = WndMode;

			LONG WndStyle = GetWindowLong(this->WndNativeHandle, GWL_STYLE);
			LONG const WndStyleFullscreen = WS_POPUP;
			LONG const WndStyleRestored = WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU | WS_OVERLAPPED | WS_BORDER;

			switch (this->WndMode)
			{
				case WindowMode::WindowModeFullscreen:
				case WindowMode::WindowModePseudoFullscreen:
				case WindowMode::WindowModeMirrorWindowed:
				{
					GetWindowPlacement(this->WndNativeHandle, &this->WndPlacementBeforeFullscreen);
					if (this->WndMode != WindowMode::WindowModeMirrorWindowed)
					{
						WndStyle &= ~WndStyleRestored;
						WndStyle |= WndStyleFullscreen;
					}
					SetWindowLongA(this->WndNativeHandle, GWL_STYLE, WndStyle);

					bool const WndIsTrueFullscreen = this->WndMode == WindowMode::WindowModeFullscreen;
					if (!WndIsTrueFullscreen)
					{
						this->RestoreWindow();
					}

					HMONITOR const WndMonitor = MonitorFromWindow(this->WndNativeHandle, WndIsTrueFullscreen ? MONITOR_DEFAULTTOPRIMARY : MONITOR_DEFAULTTONEAREST);
					MONITORINFO WndMonitorInfo = { 0 };
					WndMonitorInfo.cbSize = sizeof(WndMonitorInfo);
					GetMonitorInfoA(WndMonitor, &WndMonitorInfo);

					RECT WndClientRect = { 0 };
					GetClientRect(this->WndNativeHandle, &WndClientRect);

					LONG const WndMonitorWidth = WndMonitorInfo.rcMonitor.right - WndMonitorInfo.rcMonitor.left;
					LONG const WndMonitorHeight = WndMonitorInfo.rcMonitor.bottom - WndMonitorInfo.rcMonitor.top;
					LONG const WndTargetClientWidth = WndIsTrueFullscreen ? Min(WndMonitorWidth, WndClientRect.right - WndClientRect.left) : WndMonitorWidth;
					LONG const WndTargetClientHeight = WndIsTrueFullscreen ? Min(WndMonitorHeight, WndClientRect.bottom - WndClientRect.top) : WndMonitorHeight;

					Rectangle WndNewRect(static_cast<Float32>(WndMonitorInfo.rcMonitor.left)
						, static_cast<Float32>(WndMonitorInfo.rcMonitor.top)
						, static_cast<Float32>(WndTargetClientWidth)
						, static_cast<Float32>(WndTargetClientHeight)
						);
					this->SetWindowRect(WndNewRect);
				} break;

				case WindowMode::WindowModeWindowed:
				{
					WndStyle &= ~WndStyleFullscreen;
					WndStyle |= WndStyleRestored;
					SetWindowLongA(this->WndNativeHandle, GWL_STYLE, WndStyle);
					SetWindowPlacement(this->WndNativeHandle, &this->WndPlacementBeforeFullscreen);
				} break;

				default:
				{
					GD_DEBUG_ASSERT_FALSE("Unknown window mode specified.");
				} break;
			}

		}
	}

	//! Returns rectangle of a window.
	//! @param WndRect Output for rectangle of a window.
	//! @returns True if operation succeeded.
	GDAPI bool WindowsWindow::GetWindowRect(Rectangle& WndRect) const
	{
		GD_DEBUG_ASSERT(this->WndNativeHandle != nullptr, "Error: 'WindowWindow' class was not initialized.");

		WndRect = this->WndRect;
		return true;
	}

	//! Applies new position to a window.
	//! @param WndPosition New position of a window.
	GDAPI void WindowsWindow::SetWindowPosition(Vector2 const& WndPosition)
	{
		GD_DEBUG_ASSERT(this->WndNativeHandle != nullptr, "Error: 'WindowWindow' class was not initialized.");

		RECT WndRect = { 0 };
		::GetWindowRect(this->WndNativeHandle, &WndRect);

		POINT WndClientPoint = { 0 };
		ClientToScreen(this->WndNativeHandle, &WndClientPoint);

		LONG WndDeltaXCoord = static_cast<LONG>(WndPosition.Left) - WndClientPoint.x;
		LONG WndDeltaYCoord = static_cast<LONG>(WndPosition.Top) - WndClientPoint.y;
		MoveWindow(this->WndNativeHandle, WndRect.left + WndDeltaXCoord, WndRect.top + WndDeltaYCoord, WndRect.right - WndRect.left, WndRect.bottom - WndRect.top, TRUE);

		this->WndRect.Left = WndPosition.Left;
		this->WndRect.Top = WndPosition.Top;
	}

	//! Applies new rectangle to a window.
	//! @param WndRect New rectangle of a window.
	GDAPI void WindowsWindow::SetWindowRect(Rectangle const& WndRect)
	{
		GD_DEBUG_ASSERT(this->WndNativeHandle != nullptr, "Error: 'WindowWindow' class was not initialized.");

		this->WndRect = WndRect;

		LONG WndNewLeft   = static_cast<LONG>(GD_TRUNC_TO_INT32(WndRect.Left));
		LONG WndNewTop    = static_cast<LONG>(GD_TRUNC_TO_INT32(WndRect.Top));
		LONG WndNewWidth  = static_cast<LONG>(GD_TRUNC_TO_INT32(WndRect.Width));
		LONG WndNewHeight = static_cast<LONG>(GD_TRUNC_TO_INT32(WndRect.Height));

		WINDOWINFO WndWindowInfo = { 0 };
		WndWindowInfo.cbSize = sizeof(WndWindowInfo);
		GetWindowInfo(this->WndNativeHandle, &WndWindowInfo);

		RECT WndWindowRect = { 0, 0, WndNewWidth, WndNewHeight };
		if ((this->WndFlags & WindowFlags::WindowHasBorder) == 0)
		{
			AdjustWindowRectEx(&WndWindowRect, WndWindowInfo.dwStyle, FALSE, WndWindowInfo.dwExStyle);
			WndNewWidth = WndWindowRect.right - WndWindowRect.left;
			WndNewHeight = WndWindowRect.top - WndWindowRect.bottom;
		}

		bool const WndVirtualSizeChanged = (this->WndVirtualSize.Width != WndNewWidth) || (this->WndVirtualSize.Height != WndNewHeight);
		if (WndVirtualSizeChanged)
		{
			this->WndVirtualSize.Width = WndNewWidth;
			this->WndVirtualSize.Height = WndNewHeight;
		}

		if ((this->WndFlags & WindowFlags::WindowWillSizeChangeOften) != 0)
		{
			RECT WndOldWindowRect = WndWindowInfo.rcWindow;
			LONG const WndOldWidth = WndOldWindowRect.right - WndOldWindowRect.left;
			LONG const WndOldHeight = WndOldWindowRect.bottom - WndOldWindowRect.top;

			LONG const WndMinRetainedWidth = this->WndMaxExpectedSize.Width != 0 ? this->WndMaxExpectedSize.Width : WndOldWidth;
			LONG const WndMinRetainedHeight = this->WndMaxExpectedSize.Height != 0 ? this->WndMaxExpectedSize.Width : WndOldHeight;

			WndNewWidth = Max(WndNewWidth, Min(WndOldWidth, WndMinRetainedWidth));
			WndNewHeight = Max(WndNewHeight, Min(WndOldHeight, WndMinRetainedHeight));
		}

		::MoveWindow(this->WndNativeHandle, WndNewLeft, WndNewTop, WndNewWidth, WndNewHeight, TRUE);
		if (((this->WndFlags & WindowFlags::WindowWillSizeChangeOften) != 0) && WndVirtualSizeChanged)
		{
			this->AdjustWindowRegion(this->WndVirtualSize.Width, this->WndVirtualSize.Height);
		}
	}

	//! Returns title of a window.
	//! @returns Title of a window.
	GDAPI String const& WindowsWindow::GetWindowTitle() const
	{
		GD_DEBUG_ASSERT(this->WndNativeHandle != nullptr, "Error: 'WindowWindow' class was not initialized.");

		return this->WndTitle;
	}

	//! Applies new title to a window.
	//! @param WndTitle New title of a window.
	GDAPI void WindowsWindow::SetWindowTitle(String const& WndTitle)
	{
		GD_DEBUG_ASSERT(this->WndNativeHandle != nullptr, "Error: 'WindowWindow' class was not initialized.");

		this->WndTitle = WndTitle;
		GD_ASSERT(SetWindowTextA(this->WndNativeHandle, this->WndTitle.CStr()), "Failed to set window title.");
	}

	//! Returns opacity of a window.
	//! @returns Opacity of a window.
	GDAPI Float32 WindowsWindow::GetWindowOpacity() const
	{
		GD_DEBUG_ASSERT(this->WndNativeHandle != nullptr, "Error: 'WindowWindow' class was not initialized.");

		return this->WndOpacity;
	}

	//! Applies new opacity to a window.
	//! @param WndOpacity New opacity of a window.
	GDAPI void WindowsWindow::SetWindowOpacity(Float32 const WndOpacity)
	{
		GD_DEBUG_ASSERT(this->WndNativeHandle != nullptr, "Error: 'WindowWindow' class was not initialized.");

		if ((this->WndFlags & WindowFlags::WindowSupportsTransparency) != 0)
		{
			this->WndOpacity = WndOpacity;
			GD_ASSERT(SetLayeredWindowAttributes(this->WndNativeHandle, 0, static_cast<BYTE>(GD_TRUNC_TO_INT32(this->WndOpacity * 255.0f)), LWA_ALPHA), "Failed to set window opacity.");
		}
	}

	//! Returns native handle of a window.
	//! @returns Native handle of a window.
	GDAPI Handle WindowsWindow::GetWindowNativeHandle() const
	{
		return this->WndNativeHandle;
	}

	//! Returns native size of a window border.
	//! @returns Native size of a window border.
	GDAPI Int32 WindowsWindow::GetWindowBorderSize() const
	{
		GD_DEBUG_ASSERT(this->WndNativeHandle != nullptr, "Error: 'WindowWindow' class was not initialized.");

		WINDOWINFO WndWindowInfo = { 0 };
		WndWindowInfo.cbSize = sizeof(WndWindowInfo);
		GetWindowInfo(this->WndNativeHandle, &WndWindowInfo);

		return WndWindowInfo.cxWindowBorders;
	}

	//! Returns true if point with specified coordinates in inside window.
	//! @returns True if point with specified coordinates in inside window.
	GDAPI bool WindowsWindow::IsPointInWindow(Vector2 const& WndPoint) const
	{
		GD_DEBUG_ASSERT(this->WndNativeHandle != nullptr, "Error: 'WindowWindow' class was not initialized.");

		HRGN const WndRegion = this->CreateWindowRegion();
		bool const WndHitResult = PtInRegion(WndRegion, GD_TRUNC_TO_INT32(WndPoint.Left), GD_TRUNC_TO_INT32(WndPoint.Top)) == GD_TRUE;
		DeleteObject(WndRegion);
		return WndHitResult;
	}

	//! Creates a internal region of a window.
	//! @returns Creates window region.
	GDINT HRGN WindowsWindow::CreateWindowRegion() const
	{
		GD_DEBUG_ASSERT(this->WndNativeHandle != nullptr, "Error: 'WindowWindow' class was not initialized.");

		if ((this->WndRegionSize.Width != 0) && (this->WndRegionSize.Height != 0))
		{
			if (this->IsWindowMaximized())
			{
				Int32 const WndBorderSize = this->GetWindowBorderSize();
				return CreateRectRgn(WndBorderSize, WndBorderSize, this->WndRegionSize.Width - WndBorderSize, this->WndRegionSize.Height - WndBorderSize);
			}
			else
			{
				if (this->WndCornerRadius > 0)
				{
					HRGN const WndRegion = CreateRoundRectRgn(0, 0, this->WndRegionSize.Width + 1, this->WndRegionSize.Height + 1, this->WndCornerRadius, this->WndCornerRadius);
					GD_DEBUG_ASSERT(PtInRegion(WndRegion, this->WndRegionSize.Width - 1, this->WndRegionSize.Height / 2) == TRUE, "Internal test for regions failed.");
					return WndRegion;
				}
				else
				{
					return CreateRectRgn(0, 0, this->WndRegionSize.Width, this->WndRegionSize.Height);
				}
			}
		}
		else
		{
			RECT WndWindowRect = { 0 };
			::GetWindowRect(this->WndNativeHandle, &WndWindowRect);
			return CreateRectRgn(0, 0, WndWindowRect.right - WndWindowRect.left, WndWindowRect.bottom - WndWindowRect.top);
		}
	}

	//! Modifies a window region.
	//! @param WndRegionWidth New width of a window region.
	//! @param WndRegionHeight New height of a window region.
	GDINT void WindowsWindow::AdjustWindowRegion(Int32 const WndRegionWidth, Int32 const WndRegionHeight)
	{
		GD_DEBUG_ASSERT(this->WndNativeHandle != nullptr, "Error: 'WindowWindow' class was not initialized.");

		this->WndRegionSize.Width = WndRegionWidth;
		this->WndRegionSize.Height = WndRegionHeight;

		HRGN const WndRegion = this->CreateWindowRegion();
		GD_ASSERT(SetWindowRgn(this->WndNativeHandle, WndRegion, FALSE) != 0, "Failed to set window region.");
	}

	// ------------------------------------------------------------------------------------------	
	// WindowsWindow : IDropTarget interface implementation.
	// ------------------------------------------------------------------------------------------	

	// IUnknown interface:
	GDAPI HRESULT WINAPI WindowsWindow::QueryInterface(REFIID TheIID, void** TheObject)
	{
		if (TheIID == IID_IDropTarget || TheIID == IID_IUnknown)
		{
			this->AddRef();
			*TheObject = static_cast<IDropTarget*>(this);
			return S_OK;
		}
		else
		{
			*TheObject = nullptr;
			return E_NOINTERFACE;
		}
	}

	GDAPI ULONG WINAPI WindowsWindow::AddRef(void)
	{
		Atomics::InterlockedIncrement32(&this->WndIUnknownRefCount);
		return this->WndIUnknownRefCount;
	}

	GDAPI ULONG WINAPI WindowsWindow::Release(void)
	{
		Atomics::InterlockedDecrement32(&this->WndIUnknownRefCount);
		return this->WndIUnknownRefCount;
	}

	// IDropTarget interface:
#if 0
	GDAPI static WindowsOLEDrugDropData WindowsWindowDecihperOLEDrugDropData(IDataObject* DrugDropDataObj)
	{
		class WindowsOLEResourceLock final : IUncopiable
		{
		public:
			STGMEDIUM&	ResourceStorageMedium;
			LPVOID		ResourceDataPtr;
		public:
			GDINL WindowsOLEResourceLock(STGMEDIUM&	ResourceStorageMedium)
				: ResourceStorageMedium(ResourceStorageMedium)
				, ResourceDataPtr(GlobalLock(ResourceStorageMedium.hGlobal))
			{
			}
			GDINL ~WindowsOLEResourceLock()
			{
				GlobalUnlock(this->ResourceStorageMedium.hGlobal);
				ReleaseStgMedium(&this->ResourceStorageMedium);
			}
		};	// class WindowsOLEResourceLock

		FORMATETC static const DrugDropDataFormatANSI = { CF_TEXT, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
		FORMATETC static const DrugDropDataFormatUnicode = { CF_UNICODETEXT, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
		FORMATETC static const DrugDropDataFormatFiles = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

		STGMEDIUM DrugDropDataStorage = { 0 };
		if ((DrugDropDataObj->QueryGetData(const_cast<FORMATETC*>(&DrugDropDataFormatANSI)) == S_OK) 
			&& (DrugDropDataObj->GetData(const_cast<FORMATETC*>(&DrugDropDataFormatANSI), &DrugDropDataStorage) == S_OK))
		{
			WindowsOLEResourceLock DrugDropDataLock(DrugDropDataStorage);
			WindowsOLEDrugDropData DrugDropData;
			DrugDropData.DragDropDataType = WindowsOLEDragDropDataType::DragDropDataText;
			DrugDropData.DragDropDataString = reinterpret_cast<CStr>(DrugDropDataLock.ResourceDataPtr);
			return DrugDropData;
		}
		else if ((DrugDropDataObj->QueryGetData(const_cast<FORMATETC*>(&DrugDropDataFormatUnicode)) == S_OK)
			&& (DrugDropDataObj->GetData(const_cast<FORMATETC*>(&DrugDropDataFormatUnicode), &DrugDropDataStorage) == S_OK))
		{
			WindowsOLEResourceLock DrugDropDataLock(DrugDropDataStorage);
			WindowsOLEDrugDropData DrugDropData;
			DrugDropData.DragDropDataType = WindowsOLEDragDropDataType::DragDropDataText;
			DrugDropData.DragDropDataString = reinterpret_cast<WideCStr>(DrugDropDataLock.ResourceDataPtr);
			return DrugDropData;
		}
		else if ((DrugDropDataObj->QueryGetData(const_cast<FORMATETC*>(&DrugDropDataFormatFiles)) == S_OK)
			&& (DrugDropDataObj->GetData(const_cast<FORMATETC*>(&DrugDropDataFormatFiles), &DrugDropDataStorage) == S_OK))
		{
			WindowsOLEResourceLock DrugDropDataLock(DrugDropDataStorage);
			WindowsOLEDrugDropData DrugDropData;
			DrugDropData.DragDropDataType = WindowsOLEDragDropDataType::DragDropDataFiles;

			DROPFILES const* DrugDropDataDropFiles = reinterpret_cast<DROPFILES const*>(DrugDropDataLock.ResourceDataPtr);
			LPCVOID const DrugDropDataDropFilesStart = reinterpret_cast<UInt8 const*>(DrugDropDataLock.ResourceDataPtr) + DrugDropDataDropFiles->pFiles;
			if (DrugDropDataDropFiles->fWide == TRUE)
			{
				WideCStr DrugDropDataDropFile = reinterpret_cast<WideCStr>(DrugDropDataDropFilesStart);
				while (*DrugDropDataDropFile != L'\0')
				{
					WideString DrugDropDataDropFileStr = DrugDropDataDropFile;
					DrugDropData.DragDropDataFiles.InsertLast(DrugDropDataDropFileStr);
					DrugDropDataDropFile += DrugDropDataDropFileStr.GetLength() + 1;
				}
			}
			else
			{
				CStr DrugDropDataDropFile = reinterpret_cast<CStr>(DrugDropDataDropFilesStart);
				while (*DrugDropDataDropFile != L'\0')
				{
					WideString DrugDropDataDropFileStr = DrugDropDataDropFile;
					DrugDropData.DragDropDataFiles.InsertLast(DrugDropDataDropFileStr);
					DrugDropDataDropFile += DrugDropDataDropFileStr.GetLength() + 1;
				}
			}

			return DrugDropData;
		}

		return WindowsOLEDrugDropData();
	}
#endif	// if 0

	GDAPI HRESULT WINAPI WindowsWindow::DragEnter(__RPC__in_opt IDataObject* TheDataObject, DWORD const KeyState, POINTL CursorPosition, __RPC__inout DWORD* const CursorEffect)
	{
		return S_OK;
	}

	GDAPI HRESULT WINAPI WindowsWindow::DragOver(DWORD const KeyState, POINTL CursorPosition, __RPC__inout DWORD* const CursorEffect)
	{
		return S_OK;
	}

	GDAPI HRESULT WINAPI WindowsWindow::DragLeave(void)
	{
		return S_OK;
	}

	GDAPI HRESULT WINAPI WindowsWindow::Drop(__RPC__in_opt IDataObject* TheDataObject, DWORD const KeyState, POINTL CursorPosition, __RPC__inout DWORD* const CursorEffect)
	{
		return S_OK;
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_WINDOWS
