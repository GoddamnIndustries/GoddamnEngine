// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Platform/SDL2/SDL2SplashScreen.h
//! @note This file should be never directly included, please consider using <GoddamnEngine/Platform/SplashScreen.h> instead.
//! LibSDL2 Splash screen definitions.
#pragma once
#if !defined(GD_INSIDE_SPLASHSCREEN_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Platform/SplashScreen.h> instead.
#endif	// if !defined(GD_INSIDE_INCLUDE_H)

#undef GD_PLATFORM_API_LIBSDL2
#define GD_PLATFORM_API_LIBSDL2 GD_TRUE

#if !GD_PLATFORM_API_LIBSDL2
#	error Current implementation of output device is not supported on this platform.
#endif	// if !GD_PLATFORM_WINDOWS

#include <GoddamnEngine/Core/Platform/Threading.h>

GD_NAMESPACE_BEGIN

	//! Defines a LibSDL2 splash screen class.
	class SDL2SplashScreen : public IGenericSplashScreen
	{
	public:
		//! Name of a Splash screen window class.
		CHAR static const	SplashScreenClassName[];

	private:

	public:
		//! Initializes a splash screen.
		GDINL SDL2SplashScreen()
		{
		}

		//! Destroys a splash screen.
		GDINL virtual ~SDL2SplashScreen()
		{ 
			this->DestroySplash(); 
		}

	public:
		//! Initializes a splash screen.
		GDAPI virtual void InitializeSplash() override;

		//! Destroys a splash screen.
		GDAPI virtual void DestroySplash() override;

		//! Shows or hides a splash screen.
		//! @param SplashDoShow Whether to show or hide splash screen.
		GDAPI virtual void ShowSplash(bool const SplashDoShow) override;

		//! Returns true, if splash is shown.
		//! @returns True, if splash is shown.
		GDAPI virtual bool IsSplashShown() const override;

		//! Sets text that is displayed in some region of a splash screen.
		//! @param SplashTextRegion Region to which set new text.
		//! @param SplashText Text to set.
		GDAPI virtual void SetSplashText(SplashScreenTextRegionTp const SplashTextRegion, String const& SplashText) override;
	};	// class SDL2SplashScreen

GD_NAMESPACE_END
