// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Platform/Windows/WindowsApplication.h
//! @note This file should be never directly included, please consider using <GoddamnEngine/Platform/Application.h> instead.
//! Windows Application definitions.
#pragma once
#if !defined(GD_INSIDE_APPLICATION_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Platform/Application.h> instead.
#endif	// if !defined(GD_INSIDE_APPLICATION_H)

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Platform/Platform.h>

#if !GD_PLATFORM_WINDOWS
#	error Current implementation of output device is not supported on this platform.
#endif	// if !GD_PLATFORM_WINDOWS

#include <Windows.h>
#include <Ole2.h>
#include <OleIdl.h>
#include <ShObjIdl.h>

GD_NAMESPACE_BEGIN

	//! @brief Returns hInstance of this application.
	//! @returns hInstance of this application.
	GDAPI extern HINSTANCE GetApplicationHInstance();

	//! @brief Windows Application class.
	class WindowsApplication : public IGenericApplication
	{
	public:
		CHAR static const	AppMutexName[];		//!< @brief Name of a application mutex.

	private:
		HINSTANCE&	AppInstanceHandle;
		HANDLE		AppNamedMutex;

	private:
		//! @brief Splash screen window routine.
		GDINT static LRESULT WINAPI ApplicationWindowProc(HWND const AppWindowHandle, UINT const AppMessage, WPARAM const AppWParam, LPARAM const AppLParam);

	public:

		//! @brief Initializes an application.
		GDAPI WindowsApplication();

		//! @brief Destroys an application.
		GDINL virtual ~WindowsApplication() { this->DestroyObject_Inst(); }

	protected:

		//! @brief Initializes all stuff that should be initialized before main components.
		GD_PLATFORM_OVERRIDE_METHOD0(void, PreInitializeObject, override);

		//! @brief Initializes all stuff that should be initialized after main components.
		GD_PLATFORM_OVERRIDE_METHOD0(void, PostInitializeObject, override);

		//! @brief Destroys an application.
		GD_PLATFORM_OVERRIDE_METHOD0(void, DestroyObject);

	};	// class WindowsApplication

	//! @brief Implements an Application class.
	typedef WindowsApplication Application;

GD_NAMESPACE_END
