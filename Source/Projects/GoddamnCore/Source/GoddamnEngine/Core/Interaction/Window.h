// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Interaction/Windows.h
 * Cross-platform windows.
 */
#pragma once
#define GD_INSIDE_WINDOW_H

#include <GoddamnEngine/Include.h>

#include <GoddamnEngine/Core/PlatformSpecificInclude.h>
#include <GoddamnEngine/Core/Object/Object.h>

GD_NAMESPACE_BEGIN

	/*!
	 * Defines the list of the supported window output modes.
	 */
	enum class WindowMode : UInt8
	{
		Windowed,
		PseudoFullscreen,
		Fullscreen,
		Unknown,
	};	// enum class WindowMode

	/*!
	 * Defines the list of the supported window flags.
	 */
	//! @{
	enum class WindowFlags : UInt32
	{
		None			= 0,
#if GD_PLATFORM_DESKTOP
		Borderless		= GD_BIT(0),	//!< Initializes a borderless window.
		Titled			= GD_BIT(1),	//!< Initializes a window with title bar.
		Closable		= GD_BIT(3),	//!< Initializes a window with close button.
		Miniaturizable	= GD_BIT(4),	//!< Initializes a window with minimize button.
		Resizable		= GD_BIT(5),	//!< Initializes a resizable window with maximize button.
		WithOpenGL      = GD_BIT(6),	//!< Initializes a window with OpenGL 1.1 Context.
		Centered		= GD_BIT(7),	//!< Window would be centered.
#endif	// if GD_PLATFORM_DESKTOP
	};	// enum class WindowFlags
	GD_ENUM_CLASS_DEFINE_FLAG_OPERATORS(WindowFlags);
	//! @}

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Generic window class. 
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class WindowGeneric : public Object
	{
		GD_DECLARE_OBJECT_INTRINSIC(WindowGeneric, Object)
		GD_PROPERTY(protected, WindowMode, m_WindowMode);
		GD_PROPERTY(protected, UInt32, m_WindowWidth);
		GD_PROPERTY(protected, UInt32, m_WindowHeight);

	protected:

		/*!
		 * Initializes a new generic window.
		 * 
		 * @param windowMode The window mode that would be set.
		 * @param windowFlags The window flags that would be set.
		 * @param windowWidth Window width.
		 * @param windowHeight Window height.
		 */
		GDINL WindowGeneric(WindowMode const windowMode, WindowFlags const windowFlags, UInt32 const windowWidth, UInt32 const windowHeight)
			: m_WindowMode(windowMode), m_WindowWidth(windowWidth), m_WindowHeight(windowHeight)
		{
			GD_NOT_USED(windowFlags);
		}

	public:

		/*!
		 * Shows or hides a window.
		 * @param doShow Do show or hide window.
		 */
		GDAPI virtual void ShowWindow(bool const doShow = true) const GD_PURE_VIRTUAL;

		/*!
		 * Retrieves the mode in which this window is running. 
		 */
		GDAPI virtual WindowMode GetWindowMode() const
		{
			return m_WindowMode;
		}

		/*!
		 * Changes the mode in which the graphics canvas device is running.
		 *
		 * @param windowMode The new window mode that would be set.
		 * @param doForceReapply Do forcedly reapply mode, even if device is running is the same mode.
		 */
		GDAPI virtual void SetWindowMode(WindowMode const windowMode, bool const doForceReapply = false) GD_PURE_VIRTUAL
		{
			m_WindowMode = windowMode;
			GD_NOT_USED(doForceReapply);
		}

		/*!
		 * Retrieves the dimensions in which this window is running.
		 *
		 * @param windowWidth Output for window width.
		 * @param windowHeight Output for window height.
		 */
		GDAPI virtual void GetWindowDimensions(UInt32& windowWidth, UInt32& windowHeight) const
		{
			windowWidth = m_WindowWidth;
			windowHeight = m_WindowHeight;
		}

		/*!
		 * Retrieves the dimensions in which this window is running.
		 *
		 * @param windowWidth New window width.
		 * @param windowHeight New window height.
		 * @param doForceReapply Do forcedly reapply dimensions, even if device is running is the same mode.
		 */
		GDAPI virtual void SetWindowDimensions(UInt32 const windowWidth, UInt32 const windowHeight, bool const doForceReapply = false) GD_PURE_VIRTUAL
		{
			m_WindowWidth = windowWidth;
			m_WindowHeight = windowHeight;
			GD_NOT_USED(doForceReapply);
		}

		/*!
		 * Callback being called to update the window.
		 */
		GDAPI virtual void OnUpdate() GD_PURE_VIRTUAL;

	};	// class WindowGeneric

GD_NAMESPACE_END

// ReSharper disable once CppUnusedIncludeDirective
#include GD_PLATFORM_INCLUDE(GoddamnEngine/Core/Interaction, Window)
#undef GD_INSIDE_WINDOW_H
