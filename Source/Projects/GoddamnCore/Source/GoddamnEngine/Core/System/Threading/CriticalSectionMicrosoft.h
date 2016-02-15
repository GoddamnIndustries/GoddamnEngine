// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/System/Threading/CriticalSectionMicrosoft.h
 * File contains Microsoft critical section implementation.
 */
#pragma once
#if !defined(GD_INSIDE_CRITICALSECTION_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Core/System/Threading/CriticalSection.h> instead.
#endif	// if !defined(GD_INSIDE_CRITICALSECTION_H)

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/System/Threading/CriticalSection.h>

#include <Windows.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Microsoft critical section class.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	struct CriticalSectionMicrosoft : public CriticalSectionGeneric
	{
	private:
		mutable CRITICAL_SECTION m_CriticalSection;

	public:

		/*!
		 * Initializes the Critical section.
		 */
		GDINL CriticalSectionMicrosoft()
		{
			InitializeCriticalSectionEx(&m_CriticalSection, 4000
#if GD_DEBUG
				, 0
#else	// if GD_DEBUG
				, CRITICAL_SECTION_NO_DEBUG_INFO
#endif	// if GD_DEBUG
				);
		}
		GDINL ~CriticalSectionMicrosoft()
		{
			DeleteCriticalSection(&m_CriticalSection);
		}

		/*!
		 * Locks the Critical Section.
		 */
		GDINL void Enter() const
		{
			EnterCriticalSection(&m_CriticalSection);
		}

		/*!
		 * Unlocks the Critical Section.
		 */ 
		GDINL void Leave() const
		{
			LeaveCriticalSection(&m_CriticalSection);
		}

	};	// struct CriticalSectionMicrosoft

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Cross-platform critical section class.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	using CriticalSection = CriticalSectionMicrosoft;

GD_NAMESPACE_END
