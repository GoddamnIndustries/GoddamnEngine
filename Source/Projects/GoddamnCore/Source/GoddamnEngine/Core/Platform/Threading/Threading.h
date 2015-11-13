// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Platform/Threading/Threading.h
 * Cross-platform Threading implementation.
 */
#pragma once
#define GD_INSIDE_THREADING_H

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Platform/Platform.h>

GD_NAMESPACE_BEGIN

	typedef Int32 TimeoutTp;
	TimeoutTp static const TimeoutInfinite = -1;

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// CriticalSection&Co class.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Generic Critical Section class.
	struct CriticalSectionGeneric : public IUncopiable
	{
	public:

		GDINL CriticalSectionGeneric() {}
		GDINL virtual ~CriticalSectionGeneric() {}

		// ------------------------------------------------------------------------------------------
		//! Locks the Critical Section.
		//! @param IsBlocking Do block the critical section.
		//! @returns True if critical section was entered successfully.
		GDINL virtual bool Enter(bool const IsBlocking = true)
		{
			GD_NOT_USED(IsBlocking);
			return true;
		}

		// ------------------------------------------------------------------------------------------
		//! Unlocks the Critical Section.
		GDINL virtual void Leave() {}
	};	// struct CriticalSectionGeneric
		
	// ------------------------------------------------------------------------------------------
	//! RAII object that locks specified object.
	struct ScopedCriticalSection final : IUncopiable
	{
	private:
		CriticalSectionGeneric* const CriticalSection;

	public:

		// ------------------------------------------------------------------------------------------
		//! Initializes scoped lock and locks sync object.
		//! @param SynchObject Object which would be locked.
		GDINL explicit ScopedCriticalSection(CriticalSectionGeneric* const CriticalSection)
			: CriticalSection(CriticalSection)
		{
			GD_DEBUG_ASSERT(CriticalSection != nullptr, "Null pointer critical section specified.");
			this->CriticalSection->Enter();
		}

		// ------------------------------------------------------------------------------------------
		//! Deinitializes scoped lock and unlocks sync object.
		GDINL ~ScopedCriticalSection()
		{
			this->CriticalSection->Leave();
		}
	};	// struct ScopedCriticalSection

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ThreadEvent class
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! The event object that could be triggered between threads.
	struct ThreadEventGeneric : public IUncopiable
	{
	public:

		// ------------------------------------------------------------------------------------------
		//! Initializes a new thread event.
		//! @param RequiresManualReset Does this event require the manual reset after it was triggered.
		GDINL explicit ThreadEventGeneric(bool const RequiresManualReset = false) {}
		GDINL virtual ~ThreadEventGeneric() {}

	public:

		// ------------------------------------------------------------------------------------------
		//! Triggers the event.
		GDINL virtual void Trigger() {}

		// ------------------------------------------------------------------------------------------
		//! Clears the event.
		GDINL virtual void Clear() {}

		// ------------------------------------------------------------------------------------------
		//! Waits for the event to be triggered.
		//! @param Timeout The timeout in ms.
		//! @returns True if operation succeeded.
		GDINL virtual bool Wait(TimeoutTp const Timeout = TimeoutInfinite)
		{
			GD_NOT_USED(Timeout);
			return true;
		}

	};	// struct ThreadEventGeneric

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Thread class.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	typedef void(*const ThreadProcTp)(Handle const);
	typedef UInt32 ThreadIDTp;

	// ------------------------------------------------------------------------------------------
	//! Represents the priority of the thread.
	enum ThreadPriority : UInt8
	{	
		THREAD_PRIORITY_MIN = 0,
		THREAD_PRIORITY_BELOW_MID,
		THREAD_PRIORITY_MID,
		THREAD_PRIORITY_ABOVE_MID,
		THREAD_PRIORITY_MAX,
		THREAD_PRIORITY_UNKNOWN,
		THREAD_PRIORITIES_COUNT,
	};	// enum ThreadPriority

	// ------------------------------------------------------------------------------------------
	//! Represents a cross-platform thread class.
	struct ThreadGeneric : public IUncopiable
	{
	protected:
		ThreadProcTp const _ThreadProc = nullptr;
		Handle       const _ThreadProcParam = nullptr;
		ThreadIDTp         _ThreadID = UInt32Max;

	public:
		 
		// ------------------------------------------------------------------------------------------
		//! Initializes a new thread.
		//! @param ThreadProc pointer to the thread procedure.
		//! @param ThreadProcParam Parameter of the thread routine
		//! @param ThreadPriority Priority of the thread object.
		//! @param ThreadName Name of the thread object.
		GDINL explicit ThreadGeneric(ThreadProcTp ThreadProc, Handle const ThreadProcParam = nullptr
			, ThreadPriority const ThreadPriority = THREAD_PRIORITY_MID, CStr const ThreadName = nullptr) 
			: _ThreadProc(ThreadProc), _ThreadProcParam(ThreadProcParam) {}
		GDINL virtual ~ThreadGeneric() {}

		// ------------------------------------------------------------------------------------------
		//! Waits for the thread to finish the execution.
		//! @param Timeout The timeout in ms.
		//! @returns True if operation succeeded.
		GDINL virtual bool Wait(TimeoutTp const Timeout = TimeoutInfinite)
		{
			GD_NOT_USED(Timeout);
			return true;
		}

		// ------------------------------------------------------------------------------------------
		//! Returns the ID of the thread.
		//! @returns The ID of the thread.
		GDINL ThreadIDTp GetThreadID() const { return _ThreadID; }

		// ------------------------------------------------------------------------------------------
		//! Returns the ID of the current thread.
		//! @returns The ID of the current thread.
		GDINL static ThreadIDTp GetCurrentThreadID() { return 0; }

	};	// struct ThreadGeneric

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Atomic&Co classes.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if !GD_PLATFORM_HAS_MULTITHREADING || 1
	template<typename Tp>
	using Atomic = Tp;
#endif	// if !GD_PLATFORM_HAS_MULTITHREADING

GD_NAMESPACE_END

#if GD_PLATFORM_HAS_MULTITHREADING
#	include GD_PLATFORM_INCLUDE(GoddamnEngine/Core/Platform/Threading, Threading)
#else	// if GD_PLATFORM_HAS_MULTITHREADING
GD_NAMESPACE_BEGIN
	//! Represents definitions for Critical Section.
	typedef CriticalSectionGeneric CriticalSection;
GD_NAMESPACE_END
#endif	// if GD_PLATFORM_HAS_MULTITHREADING

#undef GD_INSIDE_THREADING_H
