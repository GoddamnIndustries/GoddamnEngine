// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Platform/SplashScreen.cpp
//! Cross-platform Splash screen definitions.

#include <GoddamnEngine/Core/Platform/SplashScreen.h>

GD_NAMESPACE_BEGIN

	GD_SINGLETON_IMPLEMENTATION(IGenericSplashScreen);

	//! Initializes a splash screen.
	//! @note This method should be invoked in child class.
	GDAPI void IGenericSplashScreen::InitializeObject_Inst()
	{
		// Trying to locate Splash screen bitmap..
		//! @todo Add loading code here.
		this->SplashScreenFilename = LR"(D:\GoddamnEngine\Content\SplashScreens\SplashScreen.bmp)";

		this->SplashScreenRegionTexts[SplashScreenTextRegion::RegionApplicationName] = String::Format("GoddamnEngine Runtime %d.%d.%d", GD_ENGINE_VERSION_MAJOR, GD_ENGINE_VERSION_MINOR, GD_ENGINE_VERSION_PATCH);
		this->SplashScreenRegionTexts[SplashScreenTextRegion::RegionStartupProgress] = "Initializing...";
		this->SplashScreenRegionTexts[SplashScreenTextRegion::RegionCopyrightInfo] = "Copyright (C) Goddamn Industries 2015. All Rights Reserved.";
	}

GD_NAMESPACE_END
