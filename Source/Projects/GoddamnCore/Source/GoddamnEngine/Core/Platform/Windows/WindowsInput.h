// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Platform/Windows/WindowsInput.h
//! @note This file should be never directly included, please consider using <GoddamnEngine/Platform/SplashScreen.h> instead.
//! Windows Input Interface definitions.
#pragma once
#if !defined(GD_INSIDE_INPUT_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Platform/SplashScreen.h> instead.
#endif	// if !defined(GD_INSIDE_INPUT_H)

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Platform/Platform.h>
#include <GoddamnEngine/Core/Containers/StaticVector.h>

#if !GD_PLATFORM_WINDOWS
#	error Current implementation of output device is not supported on this platform.
#endif	// if !GD_PLATFORM_WINDOWS

#include <Windows.h>

GD_NAMESPACE_BEGIN

	//! @brief Defines a Windows input interface.
	class WindowsInput final : public IGenericInput
	{

	public:

		//! @brief Initializes an input interface.
		GDINL WindowsInput()
		{
			CMemory::Memset(&this->InputGamepadStates, 0, sizeof(this->InputGamepadStates));
		}

		//! @brief Destroys an input interface.
		GDAPI virtual ~WindowsInput()
		{
			this->DestroyObject();
		}

	protected:

		//! @brief Initializes an input interface.
		GD_PLATFORM_OVERRIDE_METHOD0(void, InitializeObject, override final);

		//! @brief Destroys an input interface.
		GD_PLATFORM_OVERRIDE_METHOD0(void, DestroyObject, override final);

		//! @brief Ticks a frame.
		GD_PLATFORM_OVERRIDE_METHOD0(void, TickObject, override final);

	public:

	};	// class WindowsInput

	//! @brief Defines a Input interface.
	typedef WindowsInput Input;

GD_NAMESPACE_END
