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
#include <GoddamnEngine/Core/Platform/PlatformCriticalSection.h>
#if GD_PLATFORM_API_MICROSOFT

#include <Windows.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Critical section (Mutex) on Microsoft platforms.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL MicrosoftPlatformCriticalSection final : public IPlatformCriticalSection
	{
	private:
		mutable CRITICAL_SECTION m_CriticalSection;

	public:
		GDINT MicrosoftPlatformCriticalSection()
		{
			InitializeCriticalSectionEx(&m_CriticalSection, 4000
#if GD_DEBUG
				, 0
#else	// if GD_DEBUG
				, CRITICAL_SECTION_NO_DEBUG_INFO
#endif	// if GD_DEBUG
				);
		}

		GDINT virtual ~MicrosoftPlatformCriticalSection()
		{
			DeleteCriticalSection(&m_CriticalSection);
		}

		/*!
		 * Locks the Critical Section.
		 */
		GDINT virtual void Enter() const override final
		{
			EnterCriticalSection(&m_CriticalSection);
		}

		/*!
		 * Unlocks the Critical Section.
		 */ 
		GDINT virtual void Leave() const override final
		{
			LeaveCriticalSection(&m_CriticalSection);
		}
	};	// struct MicrosoftPlatformCriticalSection

	/*!
	 * Creates the critical section.
	 */
	GDAPI IPlatformCriticalSection* IPlatformCriticalSection::Create()
	{
		return gd_new MicrosoftPlatformCriticalSection();
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_API_MICROSOFT
