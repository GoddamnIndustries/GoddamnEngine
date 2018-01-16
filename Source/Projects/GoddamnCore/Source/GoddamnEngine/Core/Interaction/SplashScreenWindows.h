// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Interaction/SplashScreenWindows.h
 * Windows splash screens.
 */
#pragma once
#if !defined(GD_INSIDE_SPLASHSCREEN_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Core/Interaction/SplashScreen.h> instead.
#endif	// if !defined(GD_INSIDE_FILESTREAM_H)

#include <Windows.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Windows splash screen UI class. 
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class SplashScreenWindows final : public SplashScreenGeneric
	{
		friend class SplashScreenThread;

	private:
		HBITMAP m_SplashBitmap;
		HWND    m_SplashWindow;
		HWND	m_SplashGuard;
		bool	m_SplashIsShown;
		RECT	m_SplashRects[SplashScreenTextsCount];
		HFONT   m_SplashSmallTextFont;
		HFONT   m_SplashNormalTextFont;
		HFONT   m_SplashLargeTextFont;

	public:
		GDAPI SplashScreenWindows();
		GDAPI ~SplashScreenWindows();

	public:
		GDAPI void ShowSplash(bool const doShow);
		GDAPI bool IsSplashShown() const;
		GDAPI void SetSplashText(SplashScreenText const splashTextRegion, String const& splashText);

	public:
		GDAPI void OnInitialize();
		GDAPI void OnUpdate();
	};	// class SplashScreenGeneric

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Cross-platform splash screen UI class. 
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	using SplashScreen = SplashScreenWindows;

GD_NAMESPACE_END
