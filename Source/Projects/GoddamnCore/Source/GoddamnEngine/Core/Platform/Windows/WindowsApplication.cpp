// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Platform/Windows/WindowsApplication.cpp
//! Windows Application definitions.

#include <GoddamnEngine/Core/Platform/Application.h>
#if GD_PLATFORM_WINDOWS

GD_NAMESPACE_BEGIN

	static HINSTANCE ApplicationHInstance = nullptr;

	//! Returns hInstance of this application.
	//! @returns hInstance of this application.
	GDAPI extern HINSTANCE GetApplicationHInstance()
	{
		return ApplicationHInstance;
	}
	
	//!< Name of a application mutex.
#if defined(GD_NAMESPACE)
	CHAR const WindowsApplication::AppMutexName[] = GD_GLUE(GD_STRIGIFY(GD_NAMESPACE), "_WindowsApplicationNamedMutex");
#else	// if defined(GD_NAMESPACE)
	CHAR const WindowsApplication::AppMutexName[] = "Goddamn_WindowsApplicationNamedMutex";
#endif	// if defined(GD_NAMESPACE)

	//! Initializes an application.
	GDAPI WindowsApplication::WindowsApplication()
		: AppInstanceHandle(ApplicationHInstance)
		, AppNamedMutex(nullptr)
	{
	}

	//! Initializes all stuff that should be initialized before main components.
	GDAPI void WindowsApplication::PreInitializeObject_Inst()
	{
		IGenericApplication::PreInitializeObject_Inst();
		GD_DEBUG_ASSERT(this->AppInstanceHandle == nullptr, "Error: 'WindowsApplication' class was already initialized.");

		_set_invalid_parameter_handler([](WideCStr const Expression, WideCStr const Function, WideCStr const File, unsigned int const Line, UIntPtr const Unused)
		{
			GD_NOT_USED(Unused);
			//! @todo Uncomment this on support of WideString -> String conversions.
		//	RegularAssertData InvalidParameterData("Exception from '_set_invalid_parameter_handler' function.", String(File).CStr(), String(Function).CStr(), static_cast<SizeTp>(Line), String(Expression).CStr());
		//	HandleRegularAssert(&InvalidParameterData);
		});

#if GD_DEBUG
		_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
		_CrtSetDebugFillThreshold(0);
#endif	// if GD_DEBUG

		this->AppNamedMutex = CreateMutexA(nullptr, true, WindowsApplication::AppMutexName);
		if ((this->AppNamedMutex != nullptr) && (::GetLastError() != ERROR_ALREADY_EXISTS))
		{
			this->AppIsFirstInstance = true;
		}
		else
		{
			this->AppIsFirstInstance = false;
			if (this->AppNamedMutex != nullptr)
			{
				ReleaseMutex(this->AppNamedMutex);
				this->AppNamedMutex = nullptr;
			}
		}

		::DisableProcessWindowsGhosting();
		::CoInitialize(nullptr);
		::OleInitialize(nullptr);

		ApplicationHInstance = GetModuleHandleA(nullptr);
		WNDCLASSA AppWindowClass		= { 0 };
		AppWindowClass.style			= CS_DBLCLKS;
		AppWindowClass.cbClsExtra		= 0;
		AppWindowClass.cbWndExtra		= 0;
		AppWindowClass.lpfnWndProc		= &WindowsApplication::ApplicationWindowProc;
		AppWindowClass.lpszClassName	= WindowsWindow::WndClassName;
		AppWindowClass.hInstance		= ApplicationHInstance;
		AppWindowClass.hCursor			= nullptr;
		AppWindowClass.hbrBackground	= nullptr;
		AppWindowClass.lpszMenuName		= nullptr;
		AppWindowClass.hIcon			= LoadIcon(ApplicationHInstance, MAKEINTRESOURCE(0 /** @todo Insert icon loading here. */));
		if (AppWindowClass.hIcon		== nullptr)
		{
			AppWindowClass.hIcon		= LoadIcon(nullptr, IDI_APPLICATION);
		}
		ATOM const Result = ::RegisterClassA(&AppWindowClass);
		GD_DEBUG_ASSERT(Result != 0, "Error: 'WindowsApplication' failed to register window class.");
	}

	//! Initializes all stuff that should be initialized after main components.
	GDAPI void WindowsApplication::PostInitializeObject_Inst()
	{
	}

	//! Destroys an application.
	GDAPI void WindowsApplication::DestroyObject_Inst()
	{
		UnregisterClassA(WindowsWindow::WndClassName, ApplicationHInstance);

		ApplicationHInstance = nullptr;

		::OleUninitialize();
		::CoUninitialize();
	//	::EnableProcessWindowsGhosting();

		if (this->AppNamedMutex != nullptr)
		{
			ReleaseMutex(this->AppNamedMutex);
			this->AppNamedMutex = nullptr;
		}
	}

	//! Splash screen window routine.
	GDINT LRESULT WINAPI WindowsApplication::ApplicationWindowProc(HWND const AppWindowHandle, UINT const AppMessage, WPARAM const AppWParam, LPARAM const AppLParam)
	{
		return DefWindowProcA(AppWindowHandle, AppMessage, AppWParam, AppLParam);
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_WINDOWS
