// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file
 * Application implementation.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/SharedPtr.h>
#include <GoddamnEngine/Core/Templates/Singleton.h>
#include "PlatformWindow.h"

GD_NAMESPACE_BEGIN

	using IPlatformKeyCode = char;
	class IPlatformApplicationDelegate : public IWindowDelegate
	{
	public:

		GDINT virtual void OnAppUpdate() {}

		GDINT virtual void OnAppWindowClosed(SharedPtr<IPlatformWindow> const& window) {}

	};	// class IPlatformApplicationDelegate

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Application.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL IPlatformApplication : public Singleton<IPlatformApplication>
	{
	protected:
		GDINT static SharedPtr<IPlatformWindow> CreateApplicationWindowBase(IWindowDelegate& windowDelegate);

	public:
		GDINT virtual SharedPtr<IPlatformWindow> GetMainWindow() GD_PURE_VIRTUAL;

		GDINT virtual SharedPtr<IPlatformWindow> CreateApplicationWindow(IWindowDelegate& windowDelegate) GD_PURE_VIRTUAL;

		// ------------------------------------------------------------------------------------------
		// Main event loop.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Starts the main event loop.
		 * 
		 * @param appDelegate Application event delegate.
		 * @returns True if the main loop was succesfully ran.
		 */
		GDINT virtual bool Run(IPlatformApplicationDelegate& appDelegate) GD_PURE_VIRTUAL;

		/*!
		 * Stop the main event loop.
		 */
		GDINT virtual void Stop() GD_PURE_VIRTUAL;

		// ------------------------------------------------------------------------------------------
		// Windows.
		// ------------------------------------------------------------------------------------------

	};	// class IPlatformApplication

GD_NAMESPACE_END
