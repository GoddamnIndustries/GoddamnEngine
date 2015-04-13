// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Platform/Windows/WindowsWindow.h
//! @note This file should be never directly included, please consider using <GoddamnEngine/Platform/Window.h> instead.
//! Windows Window definitions.
#pragma once
#if !defined(GD_INSIDE_WINDOW_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Platform/Window.h> instead.
#endif	// if !defined(GD_INSIDE_WINDOW_H)

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Platform/Platform.h>

#if !GD_PLATFORM_WINDOWS
#	error Current implementation of output device is not supported on this platform.
#endif	// if !GD_PLATFORM_WINDOWS

#include <Windows.h>
#include <Ole2.h>
#include <OleIdl.h>

GD_NAMESPACE_BEGIN

	//! @brief
	//! @{
	struct WindowsOLEDragDropDataType final
	{
		typedef UInt8 Type;
		Type static const DragDropDataNone	= 0;
		Type static const DragDropDataText	= 1;
		Type static const DragDropDataFiles = 2;
	};	// struct WindowsOLEDragDropDataType
	typedef WindowsOLEDragDropDataType::Type WindowsOLEDragDropDataTypeTp;
	//! @}

	//! @brief
	class WindowsOLEDrugDropData final
	{
	public:
		WindowsOLEDragDropDataTypeTp	DragDropDataType;
		WideString						DragDropDataString;
		Vector<WideString>				DragDropDataFiles;

	public:

		//! @brief
		GDINL WindowsOLEDrugDropData()
			: DragDropDataType(WindowsOLEDragDropDataType::DragDropDataNone)
		{
		}
	};	// struct WindowsOLEDrugDropData

	//! @brief Implements a Windows window class.
	class WindowsWindow : public IGenericWindow, private IDropTarget
	{
	public:
		CHAR static const	WndClassName[];	//!< @brief Name of a window class.

	private:
		HWND				WndNativeHandle;
		Int32				WndIUnknownRefCount;
		WINDOWPLACEMENT		WndPlacementBeforeFullscreen;
		WINDOWPLACEMENT		WndPlacementBeforeParentMinimized;
		bool				WndIsVisible;
		bool				WndIsEnabled;
		Int32x2				WndVirtualSize;
		Int32x2				WndRegionSize;

	private:
		// IUnknown interface:
		GDAPI virtual HRESULT WINAPI QueryInterface(REFIID TheIID, void** TheObject) override;
		GDAPI virtual ULONG WINAPI AddRef(void) override;
		GDAPI virtual ULONG WINAPI Release(void) override;

		// IDropTarget interface:
		GDAPI virtual HRESULT WINAPI DragEnter(__RPC__in_opt IDataObject* TheDataObject, DWORD const KeyState, POINTL CursorPosition, __RPC__inout DWORD* const CursorEffect) override;
		GDAPI virtual HRESULT WINAPI DragOver(DWORD const KeyState, POINTL CursorPosition, __RPC__inout DWORD* const CursorEffect) override;
		GDAPI virtual HRESULT WINAPI DragLeave(void) override;
		GDAPI virtual HRESULT WINAPI Drop(__RPC__in_opt IDataObject* TheDataObject, DWORD const KeyState, POINTL CursorPosition, __RPC__inout DWORD* const CursorEffect) override;

	private:

		//! @brief Creates a internal region of a window.
		//! @returns Creates window region.
		GDINT HRGN CreateWindowRegion() const;

		//! @brief Modifies a window region.
		//! @param WndRegionWidth New width of a window region.
		//! @param WndRegionHeight New height of a window region.
		GDINT void AdjustWindowRegion(Int32 const WndRegionWidth, Int32 const WndRegionHeight);

	public:

		//! @brief Initializes a window.
		//! @param WndFlags Default flags of a window.
		//! @param WndMode Initial mode of a window.
		//! @param WndRect Initial rectangle of a window.
		//! @param WndTitle Initial title of a window.
		//! @param WndOpacity Initial opacity of a window.
		//! @param WndCornerRadius Default corner radius of a window.
		GDAPI WindowsWindow(WindowFlagsTp const WndFlags, WindowModeTp const WndMode, Rectangle const& WndRect, String	const& WndTitle, Float32 const WndOpacity, Int32 const WndCornerRadius);

		//! @brief Destroys a window.
		GDAPI virtual ~WindowsWindow()
		{
			this->DestroyWindow();
		}

	public:

		//! @brief Initializes a window.
		GDAPI virtual void InitializeWindow() override;

		//! @brief Destroys a window.
		GDAPI virtual void DestroyWindow() override;

		//! @brief Disables or enables a window.
		//! @param DoEnable True to enable window, false to disable.
		GDAPI virtual void EnableWindow(bool const DoEnable) override;

		//! @brief Returns true if window is enabled.
		//! @returns True if window is enabled.
		GDAPI virtual bool IsWindowEnabled() const override;

		//! @brief Shows or hides a window.
		//! @param DoShow True to show window, false to hide.
		GDAPI virtual void ShowWindow(bool const DoShow) override;

		//! @brief Returns true if window is shown.
		//! @returns True if window is shown.
		GDAPI virtual bool IsWindowShown() const override;

		//! @brief Maximizes a window.
		GDAPI virtual void MaximizeWindow() override;

		//! @brief Returns true if window is maximized.
		//! @returns True if window is maximized.
		GDAPI virtual bool IsWindowMaximized() const override;

		//! @brief Minimizes a window.
		GDAPI virtual void MininizeWindow() override;

		//! @brief Returns true if window is minimized.
		//! @returns True if window is minimized.
		GDAPI virtual bool IsWindowMinimized() const override;

		//! @brief Makes window a top-most window.
		//! @param Force Do bring window to front forcedly (via hacks).
		GDAPI virtual void BringWindowToFront(bool const Force = false) override;

		//! @brief Returns true if window is top most.
		//! @returns True if window is foreground.
		GDAPI virtual bool IsWindowForeground() const override;

		//! @brief Restores a window (?!)
		GDAPI virtual void RestoreWindow() override;

		//! @brief Returns mode of a window.
		//! @returns Mode of a window.
		GDAPI virtual WindowModeTp GetWindowMode() const override;

		//! @brief Applies new mode to a window.
		//! @param WndMode New mode of a window.
		GDAPI virtual void SetWindowMode(WindowModeTp const WndMode) override;

		//! @brief Returns rectangle of a window.
		//! @param WndRect Output for rectangle of a window.
		//! @returns True if operation succeeded.
		GDAPI virtual bool GetWindowRect(Rectangle& WndRect) const override;

		//! @brief Applies new position to a window.
		//! @param WndPosition New position of a window.
		GDAPI virtual void SetWindowPosition(Vector2 const& WndPosition) override;

		//! @brief Applies new rectangle to a window.
		//! @param WndRect New rectangle of a window.
		GDAPI virtual void SetWindowRect(Rectangle const& WndRect) override;

		//! @brief Returns title of a window.
		//! @returns Title of a window.
		GDAPI virtual String const& GetWindowTitle() const override;
		
		//! @brief Applies new title to a window.
		//! @param WndTitle New title of a window.
		GDAPI virtual void SetWindowTitle(String const& WndTitle) override;

		//! @brief Returns opacity of a window.
		//! @returns Opacity of a window.
		GDAPI virtual Float32 GetWindowOpacity() const override;

		//! @brief Applies new opacity to a window.
		//! @param WndOpacity New opacity of a window.
		GDAPI virtual void SetWindowOpacity(Float32 const WndOpacity) override;

		//! @brief Returns native handle of a window.
		//! @returns Native handle of a window.
		GDAPI virtual Handle GetWindowNativeHandle() const override;

		//! @brief Returns native size of a window border.
		//! @returns Native size of a window border.
		GDAPI virtual Int32 GetWindowBorderSize() const override;

		//! @brief Returns true if point with specified coordinates in inside window.
		//! @returns True if point with specified coordinates in inside window.
		GDAPI virtual bool IsPointInWindow(Vector2 const& WndPoint) const override;
	};	// class IGenericWindow

	//! @brief Implements a window class.
	typedef WindowsWindow Window;

GD_NAMESPACE_END

