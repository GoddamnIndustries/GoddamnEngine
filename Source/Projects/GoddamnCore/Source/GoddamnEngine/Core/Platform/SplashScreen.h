// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Platform/SplashScreen.h
//! Cross-platform Splash screen definitions.
#pragma once
#define GD_INSIDE_SPLASHSCREEN_H

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Platform/Platform.h>
#include <GoddamnEngine/Core/Platform/Threading.h>
#include <GoddamnEngine/Core/Containers/String.h>

GD_NAMESPACE_BEGIN

	//! Defines regions of a splash screen.
	//! @{
	struct SplashScreenTextRegion
	{
		typedef UInt8 Type;
		Type static const	RegionApplicationName	= 0;
		Type static const	RegionStartupProgress	= 1;
		Type static const	RegionCopyrightInfo		= 2;
		SizeTp static const RegionsCount			= 3;
	};	// struct SplashScreenTextRegion
	typedef SplashScreenTextRegion::Type SplashScreenTextRegionTp;
	//! @}

	//! Defines a generic splash screen class.
	class IGenericSplashScreen 
		: public IPlatformObject<IGenericSplashScreen, PlatformObjectFlags::FlagsGenericInterfaceIsDummy | PlatformObjectFlags::FlagsHasNoTickFunction>
	{
	protected:
		WideString			SplashScreenFilename;
		String				SplashScreenRegionTexts[SplashScreenTextRegion::RegionsCount];
		CriticalSection		SplashScreenSyncObject;

	protected:
		//! Gays are here
		//! Big dicks are back in town	
		//! Initializes a splash screen.
		GDINL IGenericSplashScreen() { }

		//! Destroys a splash screen.
		GDINL virtual ~IGenericSplashScreen() { this->DestroyObject(); }

	protected:

		//! Initializes a splash screen.
		//! @note This method should be invoked in child class.
		GD_PLATFORM_DEFINE_METHOD0(void, InitializeObject, override);

		//! Destroys a splash screen.
		GD_PLATFORM_DEFINE_METHOD0(void, DestroyObject, {});

	public:

		//! Shows or hides a splash screen.
		//! @param SplashDoShow Whether to show or hide splash screen.
		GD_PLATFORM_DEFINE_METHOD1(void, ShowSplash, bool const, SplashDoShow, {});

		//! Returns true, if splash is shown.
		//! @returns True, if splash is shown.
		GD_PLATFORM_DEFINE_METHOD0(bool, IsSplashShown, const { return false; });

		//! Sets text that is displayed in some region of a splash screen.
		//! @param SplashTextRegion Region to which set new text.
		//! @param SplashText Text to set.
		GD_PLATFORM_DEFINE_METHOD2(void, SetSplashText, SplashScreenTextRegionTp const, SplashTextRegion, String const&, SplashText, {});

	};	// class IGenericSplashScreen

GD_NAMESPACE_END

#include GD_PLATFORM_INCLUDE(GoddamnEngine/Core/Platform, SplashScreen.h)
#undef GD_INSIDE_SPLASHSCREEN_H
