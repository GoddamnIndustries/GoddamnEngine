// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Platform/Windows/WindowsSplashScreen.h
//! @note This file should be never directly included, please consider using <GoddamnEngine/Platform/SplashScreen.h> instead.
//! Windows Splash screen definitions.
#pragma once
#if !defined(GD_INSIDE_SPLASHSCREEN_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Platform/SplashScreen.h> instead.
#endif	// if !defined(GD_INSIDE_SPLASHSCREEN_H)

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Platform/Platform.h>

#if !GD_PLATFORM_WINDOWS
#	error Current implementation of output device is not supported on this platform.
#endif	// if !GD_PLATFORM_WINDOWS

#include <Windows.h>

GD_NAMESPACE_BEGIN

	//! @brief Defines a Windows splash screen class.
	class WindowsSplashScreen final : public IGenericSplashScreen
	{
	public:
		//! @brief Name of a Splash screen window class.
		CHAR static const	SplashScreenClassName[];

	private:
		HANDLE				SplashScreenThread;
		HBITMAP				SplashScreenBitmap;
		HWND				SplashScreenWindow;
		HWND				SplashScreenGuard;
		bool				SplashScreenIsShown;
		RECT				SplashScreenRegionRects[SplashScreenTextRegion::RegionsCount];
		HFONT				SplashScreenSmallTextFont;
		HFONT				SplashScreenNormalTextFont;
		HFONT				SplashScreenLargeTextFont;

	public:

		//! @brief Initializes a splash screen.
		GDINL WindowsSplashScreen()
			: SplashScreenThread(nullptr)
			, SplashScreenBitmap(nullptr)
			, SplashScreenWindow(nullptr)
			, SplashScreenGuard(nullptr)
			, SplashScreenIsShown(false)
			, SplashScreenSmallTextFont(nullptr), SplashScreenNormalTextFont(nullptr), SplashScreenLargeTextFont(nullptr)
		{
			CMemory::Memset(&this->SplashScreenRegionRects, 0, sizeof(this->SplashScreenRegionRects));
		}

		//! @brief Destroys a splash screen.
		GDINL virtual ~WindowsSplashScreen()
		{ 
			this->DestroyObject(); 
		}

	private:

		//! @brief Splash screen thread routine.
		GDINT static DWORD WINAPI SplashScreenThreadProc(LPVOID const SplashScreenThreadParameter);

		//! @brief Splash screen window routine.
		GDINT static LRESULT WINAPI SplashScreenWindowProc(HWND const SplashScreenWindowHandle, UINT const SplashScreenMessage, WPARAM const SplashScreenWParam, LPARAM const SplashScreenLParam);

	protected:

		//! @brief Initializes a splash screen.
		GD_PLATFORM_OVERRIDE_METHOD0(void, InitializeObject, override final);

		//! @brief Destroys a splash screen.
		GD_PLATFORM_OVERRIDE_METHOD0(void, DestroyObject, override final);

	public:

		//! @brief Shows or hides a splash screen.
		//! @param SplashDoShow Whether to show or hide splash screen.
		GD_PLATFORM_OVERRIDE_METHOD1(void, ShowSplash, bool const, SplashDoShow, override final);

		//! @brief Returns true, if splash is shown.
		//! @returns True, if splash is shown.
		GD_PLATFORM_OVERRIDE_METHOD0(bool, IsSplashShown, const override final);

		//! @brief Sets text that is displayed in some region of a splash screen.
		//! @param SplashTextRegion Region to which set new text.
		//! @param SplashText Text to set.
		GD_PLATFORM_OVERRIDE_METHOD2(void, SetSplashText, SplashScreenTextRegionTp const, SplashTextRegion, String const&, SplashText, override final);

	};	// class WindowsSplashScreen

	//! @brief Implements a splash screen class.
	typedef WindowsSplashScreen SplashScreen;

GD_NAMESPACE_END
