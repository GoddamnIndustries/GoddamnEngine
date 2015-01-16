/// ==========================================================================================
/// CriticalSection.h - Critical section interface and implementation.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// 
/// History:
///		* 29.04.2014 - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_THREADING_CRITICAL_SECTION
#define GD_CORE_THREADING_CRITICAL_SECTION

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Diagnostics/Assertion/Assertion.h>
#if (defined(GD_PLATFORM_WINDOWS) || defined(GD_PLATFORM_XBOX_ONE) || defined(GD_PLATFORM_WINPNONE8))
#	define GD_THREAD_WINAPI		1
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#else	// if (defined(GD_PLATFORM_WINDOWS) || defined(GD_PLATFORM_XBOX_ONE) || defined(GD_PLATFORM_WINPNONE8))
#	define GD_THREAD_PTHREAD	1
#	include <pthread.h>
#endif	// if (defined(GD_PLATFORM_WINDOWS) || defined(GD_PLATFORM_XBOX_ONE) || defined(GD_PLATFORM_WINPNONE8))

GD_NAMESPACE_BEGIN

	/// Accesses a shared resource (data structure or device) that must 
	/// not be concurrently accessed by more than one thread of execution.
	class CriticalSection final
	{
	private:
		GD_CLASS_UNASSIGNABLE(CriticalSection);
		GD_CLASS_UNSWAPPABLE (CriticalSection);
		GD_CLASS_UNCOPIABLE  (CriticalSection);
		GD_CLASS_UNMOVABLE   (CriticalSection);

#if (defined(GD_DOCUMENTATION))
	public:
		GDINT  CriticalSection();
		GDINT ~CriticalSection();

		/// Locks the critical section.
		GDINT void Enter();

		/// Unlocks the critical section.
		GDINT void Leave();
#else	// if (defined(GD_DOCUMENTATION))
#	if (defined(GD_THREAD_WINAPI))
//  if (defined(GD_THREAD_PTHREAD)) 
#	undef GD_THREAD_WINAPI
	private: // Here we have native implementation with Windows API.
		::CRITICAL_SECTION NativeCriticalSection;

	public:
		GDINL CriticalSection() 
		{	// Creating a critical section here.
			InitializeCriticalSection  (&this->NativeCriticalSection);
			SetCriticalSectionSpinCount(&this->NativeCriticalSection, 4000);
		}

		GDINL ~CriticalSection() 
		{	// Destroying critical section here.
			::DeleteCriticalSection(&this->NativeCriticalSection); 
		}

		GDINL void Enter() 
		{	// Spin first before entering critical section, causing ring-0 transition and context switch.
			if (::TryEnterCriticalSection(&this->NativeCriticalSection) == 0)
				::   EnterCriticalSection(&this->NativeCriticalSection); 
		}
		
		GDINL void Leave() 
		{	// Leaving critical section
			::LeaveCriticalSection(&this->NativeCriticalSection); 
		}

//	  if (defined(GD_THREAD_WINAPI))
#	elif (defined(GD_THREAD_PTHREAD))
#	undef GD_THREAD_PTHREAD
	private:// Assuming PThread has an implementation for this platform.
		::pthread_mutex_t NativeCriticalSection;

	public:
		GDINL CriticalSection() 
		{	// Creating a recursive mutex here.
			::pthread_mutexattr_t NativeCriticalSectionAttributes;
			::pthread_mutexattr_init   (&NativeCriticalSectionAttributes);
			::pthread_mutexattr_settype(&NativeCriticalSectionAttributes, PTHREAD_MUTEX_RECURSIVE);
			::pthread_mutex_init(&this->NativeCriticalSection, &NativeCriticalSectionAttributes);
		}

		GDINL ~CriticalSection() 
		{	// Destroying mutex here.
			::pthread_mutex_destroy(&this->NativeCriticalSection); 
		}

		GDINL void Enter() { ::pthread_mutex_lock  (&this->NativeCriticalSection); }
		GDINL void Leave() { ::pthread_mutex_unlock(&this->NativeCriticalSection); }
#	endif	//   if (defined(GD_THREAD_PTHREAD))
			// elif (defined(GD_THREAD_WINAPI))
#endif	// if (defined(GD_DOCUMENTATION))
	};	// class CriticalSection

	/// Carries that task for entering / leaving critical sections inside scopes.
	class ScopedLock final
	{
	private:
		GD_CLASS_UNASSIGNABLE(ScopedLock);
		GD_CLASS_UNSWAPPABLE (ScopedLock);
		GD_CLASS_UNCOPIABLE  (ScopedLock);
		GD_CLASS_UNMOVABLE   (ScopedLock);
		CriticalSection& ReferencedCriticalSection;

	public:
		GDINL  ScopedLock(CriticalSection& ReferencedCriticalSection) : ReferencedCriticalSection(ReferencedCriticalSection) { this->ReferencedCriticalSection.Enter(); }
		GDINL ~ScopedLock()  { this->ReferencedCriticalSection.Leave(); }
	};	// class ScopedLock

GD_NAMESPACE_END

#endif	// ifndef GD_CORE_THREADING_CRITICAL_SECTION
