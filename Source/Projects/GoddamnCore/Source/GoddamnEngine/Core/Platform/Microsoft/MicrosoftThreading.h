// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Platform/Microsoft/MicrosoftThreading.h
//! @note This file should be never directly included, please consider using <GoddamnEngine/Platform/Threading.h> instead.
//! File contains Microsoft Threading implementation.
#pragma once
#if !defined(GD_INSIDE_THREADING_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Platform/Threading.h> instead.
#endif	// if !defined(GD_INSIDE_INCLUDE_H)

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Platform/Platform.h>
#include <GoddamnEngine/Core/Platform/Threading.h>

#if !(GD_PLATFORM_API_MICROSOFT && GD_PLATFORM_HAS_MULTITHREADING)
#	error Current implementation of Threading is not supported on this platform.
#endif	// if !(GD_PLATFORM_API_MICROSOFT && GD_PLATFORM_HAS_MULTITHREADING)

#include <Windows.h>

GD_NAMESPACE_BEGIN

	//! @brief Microsoft Critical Section class.
	class MicrosoftCriticalSection final : public IGenericCriticalSection
	{
	private:
		::CRITICAL_SECTION CriticalSection;

	public:
		//! @brief Initializes a Critical Section.
		GDINL MicrosoftCriticalSection()
		{
			::InitializeCriticalSection(&this->CriticalSection);
			::SetCriticalSectionSpinCount(&this->CriticalSection, 4000);
		}

		//! @brief Deinitializes a Critical Section.
		GDINL ~MicrosoftCriticalSection()
		{
			::DeleteCriticalSection(&this->CriticalSection);
		}

		//! @brief Locks the Critical Section.
		GDINL void Enter()
		{
			if (::TryEnterCriticalSection(&this->CriticalSection)) {
				::EnterCriticalSection(&this->CriticalSection);
			}
		}

		//! @brief Unlocks the Critical Section.
		GDINL void Leave()
		{
			::LeaveCriticalSection(&this->CriticalSection);
		}
	};	// class GenericCriticalSection

GD_NAMESPACE_END
