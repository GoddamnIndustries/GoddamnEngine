// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Platform/Window.h
//! Cross-platform Window definitions.
#pragma once
#define GD_INSIDE_WINDOW_H

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Platform/Platform.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Templates/SharedPtr.h>
#include <GoddamnEngine/Core/Math/Vector4.h>

GD_NAMESPACE_BEGIN

	//! @brief Defines a flags for a window.
	//! @{
	struct WindowFlags final
	{
		typedef UInt16 Type;
		Type static const WindowHasBorder				= GD_BIT(0);
		Type static const WindowHasSizingFrame			= GD_BIT(1);
		Type static const WindowAppearsInTaskbar		= GD_BIT(2);
		Type static const WindowIsTopmost				= GD_BIT(3);
		Type static const WindowIsModal					= GD_BIT(4);
		Type static const WindowIsRegular				= GD_BIT(5);
		Type static const WindowSupportsMaximize		= GD_BIT(6);
		Type static const WindowSupportsMinimize		= GD_BIT(7);
		Type static const WindowSupportsTransparency	= GD_BIT(8);
		Type static const WindowAcceptsInput			= GD_BIT(9);
		Type static const WindowActivateWhenFirstShown	= GD_BIT(10);
		Type static const WindowWillSizeChangeOften		= GD_BIT(11);
	};	// struct WindowFlags
	typedef WindowFlags::Type WindowFlagsTp;
	//! @}

	//! @brief Defines a mode for a window.
	//! @{
	struct WindowMode final
	{
		typedef UInt8 Type;
		Type static const WindowModeFullscreen			= 0;
		Type static const WindowModePseudoFullscreen	= 1;
		Type static const WindowModeWindowed			= 2;
		Type static const WindowModeMirrorWindowed		= 3;
	};	// struct WindowMode
	typedef WindowMode::Type WindowModeTp;
	//! @}

	//! @brief Defines an activation mode for a window.
	//! @{
	struct WindowActivation final
	{
		typedef UInt8 Type;
		Type static const WindowActivate				= 0;
		Type static const WindowActivateByMouse			= 1;
		Type static const WindowDeactivate				= 2;
	};	// struct WindowActivation
	typedef WindowActivation::Type WindowActivationTp;
	//! @}

	//! @brief Defines an action, done by user.
	//! @{
	struct WindowAction final
	{
		typedef UInt8 Type;
		Type static const WindowClickedNonClientArea	= 0;
		Type static const WindowMaximize				= 1;
		Type static const WindowRestore					= 2;
		Type static const WindowWindowMenu				= 3;
	};	// struct WindowAction
	typedef WindowAction::Type WindowActionTp;
	//! @}

	//! @brief Defines a zone of a window.
	//! @{
	struct WindowZone final
	{
		typedef UInt8 Type;
		Type static const WindowZoneNotInWindow			= 0;
		Type static const WindowZoneTopLeftBorder		= 1;
		Type static const WindowZoneTopBorder			= 2;	
		Type static const WindowZoneTopRightBorder		= 3;
		Type static const WindowZoneLeftBorder			= 4;
		Type static const WindowZoneClientArea			= 5;
		Type static const WindowZoneRightBorder			= 6;
		Type static const WindowZoneBottomLeftBorder	= 7;
		Type static const WindowZoneBottomBorder		= 8;
		Type static const WindowZoneBottomRightBorder	= 8;
		Type static const WindowZoneTitleBar			= 10;
		Type static const WindowZoneMinimizeButton		= 11;
		Type static const WindowZoneMaximizeButton		= 12;
		Type static const WindowZoneCloseButton			= 13;
		Type static const WindowZoneSysMenu				= 14;
		Type static const WindowZoneUnspecified			= 0;
	};	// struct WindowZone
	typedef WindowZone::Type WindowZoneTp;
	//! @}

	//! @brief Defines a generic window class.
	class IGenericWindow
	{
	protected:
		WindowFlagsTp	WndFlags;
		WindowModeTp	WndMode;
		Rectangle		WndRect;
		String			WndTitle;
		Float32			WndOpacity;
		Int32			WndCornerRadius;
		Int32x2			WndMaxExpectedSize;

	public:

		//! @brief Initializes a window.
		//! @param WndFlags Default flags of a window.
		//! @param WndMode Initial mode of a window.
		//! @param WndRect Initial rectangle of a window.
		//! @param WndTitle Initial title of a window.
		//! @param WndOpacity Initial opacity of a window.
		//! @param WndCornerRadius Default corner radius of a window.
		GDINL IGenericWindow(WindowFlagsTp const WndFlags, WindowModeTp const WndMode, Rectangle const& WndRect, String	const& WndTitle, Float32 const WndOpacity, Int32 const WndCornerRadius)
			: WndFlags(WndFlags)
			, WndMode(WndMode)
			, WndRect(WndRect)
			, WndTitle(WndTitle)
			, WndOpacity(WndOpacity)
			, WndCornerRadius(WndCornerRadius)
		{
		}

		//! @brief Destroys a window.
		GDAPI virtual ~IGenericWindow() 
		{
		}

	public:

		//! @brief Initializes a window.
		GDAPI virtual void InitializeWindow() abstract;

		//! @brief Destroys a window.
		GDAPI virtual void DestroyWindow() abstract;

		//! @brief Disables or enables a window.
		//! @param DoEnable True to enable window, false to disable.
		GDAPI virtual void EnableWindow(bool const DoEnable) abstract;

		//! @brief Returns true if window is enabled.
		//! @returns True if window is enabled.
		GDAPI virtual bool IsWindowEnabled() const abstract;

		//! @brief Shows or hides a window.
		//! @param DoShow True to show window, false to hide.
		GDAPI virtual void ShowWindow(bool const DoShow) abstract;

		//! @brief Returns true if window is shown.
		//! @returns True if window is shown.
		GDAPI virtual bool IsWindowShown() const abstract;

		//! @brief Maximizes a window.
		GDAPI virtual void MaximizeWindow() abstract;

		//! @brief Returns true if window is maximized.
		//! @returns True if window is maximized.
		GDAPI virtual bool IsWindowMaximized() const abstract;

		//! @brief Minimizes a window.
		GDAPI virtual void MininizeWindow() abstract;

		//! @brief Returns true if window is minimized.
		//! @returns True if window is minimized.
		GDAPI virtual bool IsWindowMinimized() const abstract;

		//! @brief Makes window a top-most window.
		//! @param Force Do bring window to front forcedly (via hacks).
		GDAPI virtual void BringWindowToFront(bool const Force = false) abstract;

		//! @brief Returns true if window is top most.
		//! @returns True if window is foreground.
		GDAPI virtual bool IsWindowForeground() const abstract;

		//! @brief Restores a window (?!)
		GDAPI virtual void RestoreWindow() abstract;

		//! @brief Return flags of a window.
		//! @returns Flags of a window.
		GDINL WindowFlagsTp GetWindowFlags() const
		{
			return this->WndFlags;
		}

		//! @brief Returns mode of a window.
		//! @returns Mode of a window.
		GDAPI virtual WindowModeTp GetWindowMode() const abstract;

		//! @brief Applies new mode to a window.
		//! @param WndMode New mode of a window.
		GDAPI virtual void SetWindowMode(WindowModeTp const WndMode) abstract;

		//! @brief Returns rectangle of a window.
		//! @param WndRect Output for rectangle of a window.
		//! @returns True if operation succeeded.
		GDAPI virtual bool GetWindowRect(Rectangle& WndRect) const abstract;

		//! @brief Applies new position to a window.
		//! @param WndPosition New position of a window.
		GDAPI virtual void SetWindowPosition(Vector2 const& WndPosition) abstract;

		//! @brief Applies new rectangle to a window.
		//! @param WndRect New rectangle of a window.
		GDAPI virtual void SetWindowRect(Rectangle const& WndRect) abstract;

		//! @brief Returns title of a window.
		//! @returns Title of a window.
		GDAPI virtual String const& GetWindowTitle() const abstract;
		
		//! @brief Applies new title to a window.
		//! @param WndTitle New title of a window.
		GDAPI virtual void SetWindowTitle(String const& WndTitle) abstract;

		//! @brief Returns opacity of a window.
		//! @returns Opacity of a window.
		GDAPI virtual Float32 GetWindowOpacity() const abstract;

		//! @brief Applies new opacity to a window.
		//! @param WndOpacity New opacity of a window.
		GDAPI virtual void SetWindowOpacity(Float32 const WndOpacity) abstract;

		//! @brief Returns native handle of a window.
		//! @returns Native handle of a window.
		GDAPI virtual Handle GetWindowNativeHandle() const abstract;

		//! @brief Returns native size of a window border.
		//! @returns Native size of a window border.
		GDAPI virtual Int32 GetWindowBorderSize() const abstract;

		//! @brief Returns true if point with specified coordinates in inside window.
		//! @returns True if point with specified coordinates in inside window.
		GDAPI virtual bool IsPointInWindow(Vector2 const& WndPoint) const abstract;
	};	// class IGenericWindow

GD_NAMESPACE_END

#include GD_PLATFORM_INCLUDE(GoddamnEngine/Core/Platform, Window.h)
#undef GD_INSIDE_WINDOW_H
