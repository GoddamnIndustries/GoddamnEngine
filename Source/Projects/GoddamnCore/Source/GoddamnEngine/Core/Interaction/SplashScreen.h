// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Interaction/SplashScreen.h
 * Cross-platform splash screens.
 */
#pragma once
#define GD_INSIDE_SPLASHSCREEN_H

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/PlatformSpecificInclude.h>
#include <GoddamnEngine/Core/Interaction/Window.h>
#include <GoddamnEngine/Core/Concurrency/CriticalSection.h>
#include <GoddamnEngine/Core/Containers/String.h>

GD_NAMESPACE_BEGIN

	/*!
	 * Defines a region inside the splash screen.
	 */
	//! @{
	enum class SplashScreenText : UInt8 
	{
		ApplicationName,
		StartupProgress,
		CopyrightInfo,
		Count
	};	// enum class SplashScreenText
	static SizeTp const SplashScreenTextsCount = static_cast<SizeTp>(SplashScreenText::Count);
	//! @}

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Generic splash screen UI class. 
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class SplashScreenGeneric : public WindowGeneric
	{
	protected:
		CriticalSection m_SplashLock;
		WideString      m_SplashScreenPath;
		String          m_SplashTexts[SplashScreenTextsCount];

	public:

		/*!
		 * Initializes a new splash screen.
		 */
		GDAPI SplashScreenGeneric();

		GDAPI virtual ~SplashScreenGeneric() {}

	public:

		/*! 
		 * Sets text that is displayed in some region of a splash screen.
		 * 
		 * @param splashTextRegion Region to which set new text.
		 * @param splashText Text to set.
		 */
		GDAPI virtual void SetSplashText(SplashScreenText const splashTextRegion, String const& splashText) {}

		/*!
		 * Callback being called to update the splash screen window.
		 */
		GDAPI virtual void OnUpdate() override;

	};	// class SplashScreenGeneric

GD_NAMESPACE_END

#include GD_PLATFORM_INCLUDE(GoddamnEngine/Core/Interaction, SplashScreen)
#undef GD_INSIDE_SPLASHSCREEN_H
