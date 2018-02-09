// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file
 * Critical section implementation.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/UniquePtr.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Critical section (Mutex).
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL IPlatformCriticalSection : public IVirtuallyDestructible
	{
	public:

		/*!
		 * Creates the critical section.
		 */
		GDAPI static IPlatformCriticalSection* Create();

		/*!
		 * Locks the critical Section.
		 */
		GDINT virtual void Enter() const GD_PURE_VIRTUAL;

		/*!
		 * Unlocks the critical Section.
		 */ 
		GDINT virtual void Leave() const GD_PURE_VIRTUAL;

	};	// class IPlatformCriticalSection

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Critical section (Mutex).
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	struct GD_PLATFORM_WRAPPER CriticalSection : public TNonCopyable
	{
	private:
		UniquePtr<IPlatformCriticalSection> m_PlatformCriticalSection;

	public:

		/*!
		 * Initializes a new critical section.
		 */
		GDINL CriticalSection()
			: m_PlatformCriticalSection(IPlatformCriticalSection::Create())
		{
		}

		/*!
		 * Locks the critical Section.
		 */
		GDINL void Enter() const
		{
			m_PlatformCriticalSection->Enter();
		}

		/*!
		 * Unlocks the critical Section.
		 */ 
		GDINL void Leave() const
		{
			m_PlatformCriticalSection->Leave();
		}

	};	// struct CriticalSection

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Scoped lock.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	struct ScopedCriticalSection final : TNonCopyable
	{
	private:
		CriticalSection const& m_CriticalSection;

	public:

		/*!
		 * Initializes scoped lock and locks sync object.
		 * @param criticalSection Object which would be locked.
		 */
		GDINL explicit ScopedCriticalSection(CriticalSection const& criticalSection)
			: m_CriticalSection(criticalSection)
		{
			m_CriticalSection.Enter();
		}

		/*!
		 * Deinitializes scoped lock and unlocks sync object.
		 */
		GDINL ~ScopedCriticalSection()
		{
			m_CriticalSection.Leave();
		}

	};	// struct ScopedCriticalSection

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Scoped conditional lock.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	struct ScopedConditionalCriticalSection final : TNonCopyable
	{
	private:
		CriticalSection const& m_CriticalSection;
		bool const m_Condition;

	public:

		/*!
		 * Initializes scoped lock and locks sync object.
		 *
		 * @param criticalSection Object which would be locked.
		 * @param condition 
		 */
		GDINL explicit ScopedConditionalCriticalSection(CriticalSection const& criticalSection, bool const condition)
			: m_CriticalSection(criticalSection), m_Condition(condition)
		{
			if (m_Condition)
			{
				m_CriticalSection.Enter();
			}
		}

		/*!
		 * Deinitializes scoped lock and unlocks sync object.
		 */
		GDINL ~ScopedConditionalCriticalSection()
		{
			if (m_Condition)
			{
				m_CriticalSection.Leave();
			}
		}

	};	// struct ScopedConditionalCriticalSection

GD_NAMESPACE_END
