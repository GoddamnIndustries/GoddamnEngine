// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Platform/Threading/ThreadingWindows.h
 * Desktop Windows Threading implementation.
 */
#pragma once
#if !defined(GD_INSIDE_THREADING_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Platform/Threading/Threading.h> instead.
#endif	// if !defined(GD_INSIDE_INPUT_H)

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Platform/Platform.h>
#include <GoddamnEngine/Core/Platform/Threading/Threading.h>

#include <Windows.h>
#include <Process.h>

GD_NAMESPACE_BEGIN
	
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// CriticalSection&Co class.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Desktop Windows critical Section class.
	struct CriticalSectionWindows final : public CriticalSectionGeneric
	{
	private:
		CRITICAL_SECTION _CsCriticalSection;

	public:

		// ------------------------------------------------------------------------------------------
		//! Initializes a desktop-Windows specific critical section.
		GDINL CriticalSectionWindows()
		{
			InitializeCriticalSection(&_CsCriticalSection);
		}

		// ------------------------------------------------------------------------------------------
		//! Deletes the desktop-Windows specific critical section.
		GDINL virtual ~CriticalSectionWindows()
		{
			DeleteCriticalSection(&_CsCriticalSection);
		}

		// ------------------------------------------------------------------------------------------
		//! Locks the Critical Section.
		//! @param IsBlocking Do block the critical section.
		//! @returns True if critical section was entered successfully.
		GDINL virtual bool Enter(bool const IsBlocking = true) override final
		{
			if (TryEnterCriticalSection(&_CsCriticalSection) == FALSE)
			{
				if (!IsBlocking)
					return false;
				EnterCriticalSection(&_CsCriticalSection);
			}
			return true;
		}

		// ------------------------------------------------------------------------------------------
		//! Unlocks the Critical Section.
		GDINL virtual void Leave() override final 
		{
			LeaveCriticalSection(&_CsCriticalSection);
		}
	};	// struct CriticalSectionWindows

	typedef CriticalSectionWindows CriticalSection;

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ThreadEvent class
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! The event object that could be triggered between threads.
	struct ThreadEventWindows final : public ThreadEventGeneric
	{
	private:
		HANDLE _TeThreadEvent = nullptr;;

	public:

		// ------------------------------------------------------------------------------------------
		//! Initializes a new thread event.
		//! @param RequiresManualReset Does this event require the manual reset after it was triggered.
		GDINL explicit ThreadEventWindows(bool const RequiresManualReset = false)
			: ThreadEventGeneric(RequiresManualReset)
		{
			_TeThreadEvent = CreateEvent(nullptr, BOOL(RequiresManualReset), FALSE, nullptr);
			GD_ASSERT(_TeThreadEvent != nullptr, "Failed to create the thread event object");
		}
		GDINL virtual ~ThreadEventWindows()
		{
			CloseHandle(_TeThreadEvent);
			_TeThreadEvent = nullptr;
		}

	public:

		// ------------------------------------------------------------------------------------------
		//! Triggers the event.
		GDINL virtual void Trigger() override final
		{
			SetEvent(_TeThreadEvent);
		}

		// ------------------------------------------------------------------------------------------
		//! Clears the event.
		GDINL virtual void Clear() override final
		{
			ResetEvent(_TeThreadEvent);
		}

		// ------------------------------------------------------------------------------------------
		//! Waits for the event to be triggered.
		//! @param Timeout The timeout in ms.
		GDINL virtual bool Wait(TimeoutTp const Timeout = TimeoutInfinite) override final
		{
			DWORD const _TeWaitResule = WaitForSingleObject(_TeThreadEvent, Timeout == TimeoutInfinite ? INFINITE : Timeout);
			GD_ASSERT(_TeWaitResule == WAIT_OBJECT_0 || (_TeWaitResule == WAIT_TIMEOUT && Timeout != TimeoutInfinite), "Unknown waiting result.");
			return _TeWaitResule == WAIT_OBJECT_0;
		}
	};	// struct ThreadEventWindows

	typedef ThreadEventWindows ThreadEvent;


	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Thread class.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Represents a cross-platform thread class.
	struct ThreadWindows : public ThreadGeneric
	{
	private:
		HANDLE   _Thread = nullptr;
		HANDLE   _ThreadStartEvent = nullptr;

	private:

		// ------------------------------------------------------------------------------------------
		//! Main thread entry point.
		//! @param _Thread The corresponding @c ThreadWindows class instance.
		//! @returns Execution result.
		GDAPI static unsigned __stdcall _ThreadNativeProc(ThreadWindows* const _Thread)
		{
			// Waiting for the creator thread to finish setting up the current thread..
			if (WaitForSingleObject(_Thread->_ThreadStartEvent, INFINITE) == WAIT_OBJECT_0)
			{
				// Executing the procedure of the current thread..
				_Thread->_ThreadProc(_Thread->_ThreadProcParam);

				// Terminating the current thread..
				_endthreadex(ERROR_SUCCESS);
				return ERROR_SUCCESS;
			}
			_endthreadex(ERROR_NOT_READY);
			return ERROR_NOT_READY;
		}

	public:

		// ------------------------------------------------------------------------------------------
		//! Initializes a new thread.
		//! @param ThreadProc pointer to the thread procedure.
		//! @param ThreadProcParam Parameter of the thread routine
		//! @param ThreadPriority Priority of the thread object.
		//! @param ThreadName Name of the thread object.
		GDINL explicit ThreadWindows(ThreadProcTp ThreadProc, Handle const ThreadProcParam = nullptr
			, ThreadPriority const ThreadPriority = THREAD_PRIORITY_MID, CStr const ThreadName = nullptr) 
			: ThreadGeneric(ThreadProc, ThreadProcParam, ThreadPriority, ThreadName)
		{
			// Creating the thread handle..
			_ThreadStartEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			_Thread = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 1024 * 1024
				, reinterpret_cast<_beginthreadex_proc_type>(_ThreadNativeProc), this, STACK_SIZE_PARAM_IS_A_RESERVATION, &_ThreadID));
			GD_ASSERT(_Thread != nullptr, "'_beginthreadex' function failed.");

			if (ThreadPriority != THREAD_PRIORITY_MID)
			{
				// Setting the thread priority..
				int static const ThreadPriorityTable[THREAD_PRIORITIES_COUNT] = {
					/* THREAD_PRIORITY_MIN       */ THREAD_PRIORITY_LOWEST,
					/* THREAD_PRIORITY_BELOW_MID */ THREAD_PRIORITY_BELOW_NORMAL,
					/* THREAD_PRIORITY_MID       */ THREAD_PRIORITY_NORMAL,
					/* THREAD_PRIORITY_ABOVE_MID */ THREAD_PRIORITY_ABOVE_NORMAL,
					/* THREAD_PRIORITY_MAX       */ THREAD_PRIORITY_HIGHEST,
					/* THREAD_PRIORITY_UNKNOWN   */ 0,
				};
				SetThreadPriority(_Thread, ThreadPriorityTable[ThreadPriority]);
			}

#if !GD_RELEASE
			if (ThreadName != nullptr)
			{
				// We will try to rename our thread for the Visual Studio Debugger..
				// https://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
#pragma pack(push, 8/*sizeof(ULONG_PTR)*/)
				typedef struct tagTHREADNAME_INFO {
					DWORD  dwType;		// Must be 0x1000.
					LPCSTR szName;		// pointer to name (in user address space).
					DWORD  dwThreadID;	// Thread ID (-1=caller thread).
					DWORD  dwFlags;		// Reserved for future use, must be zero.
				} THREADNAME_INFO;
#pragma pack(pop)
				THREADNAME_INFO _ThreadInfo = {};
				_ThreadInfo.dwType = 0x1000;
				_ThreadInfo.szName = ThreadName;
				_ThreadInfo.dwThreadID = _ThreadID;
				[&_ThreadInfo]() {
					// We wrap this code into the lambda expression to prevent the C2712 error.
					// https://msdn.microsoft.com/en-us/library/xwtb73ad.aspx
					__try { RaiseException(0x406D1388, 0, sizeof(_ThreadInfo) / sizeof(ULONG_PTR), reinterpret_cast<ULONG_PTR const*>(&_ThreadInfo)); }
					__except (EXCEPTION_EXECUTE_HANDLER) {}
				}();
			}
#endif	// if !GD_RELEASE

			SetEvent(_ThreadStartEvent);
		}
		GDINL virtual ~ThreadWindows()
		{
			WaitForSingleObject(_Thread, INFINITE);
			CloseHandle(_ThreadStartEvent);
			CloseHandle(_Thread);
		}

		// ------------------------------------------------------------------------------------------
		//! Waits for the thread to finish the execution.
		//! @param Timeout The timeout in ms.
		//! @returns True if operation succeeded.
		GDINL virtual bool Wait(TimeoutTp const Timeout = TimeoutInfinite) override final
		{
			DWORD const _TeWaitResule = WaitForSingleObject(_Thread, Timeout == TimeoutInfinite ? INFINITE : Timeout);
			GD_ASSERT(_TeWaitResule == WAIT_OBJECT_0 || (_TeWaitResule == WAIT_TIMEOUT && Timeout != TimeoutInfinite), "Unknown waiting result.");
			return _TeWaitResule == WAIT_OBJECT_0;
		}

		// ------------------------------------------------------------------------------------------
		//! Returns the ID of the current thread.
		//! @returns The ID of the current thread.
		GDINL static ThreadIDTp GetCurrentThreadID()
		{
			return GetCurrentThreadId();
		}
	};	// struct ThreadWindows

	typedef ThreadWindows Thread;

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Atomic&Co class.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	template<typename Tp>
	struct AtomicWindowsBase;

	template<>
	struct AtomicWindowsBase<Int32> final
	{
	private:
		Int32 _AtomicValue;

	public:

		// ------------------------------------------------------------------------------------------
		// Atomically increments current value.
		//! @{
		GDINL AtomicWindowsBase<Int32>& operator++()
		{
			InterlockedIncrement(reinterpret_cast<LONG volatile*>(&_AtomicValue));
			return *this;
		}
		GDINL AtomicWindowsBase<Int32> operator++(int const unused)
		{
			GD_NOT_USED(unused);
			return ++AtomicWindowsBase<Int32>(*this);
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// Atomically decrements current value.
		//! @{
		GDINL AtomicWindowsBase<Int32>& operator--()
		{
			InterlockedDecrement(reinterpret_cast<LONG volatile*>(&_AtomicValue));
			return *this;
		}
		GDINL AtomicWindowsBase<Int32> operator--(int const unused)
		{
			GD_NOT_USED(unused);
			return --AtomicWindowsBase<Int32>(*this);
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// Atomically adds two value.
		//! @{
		GDINL AtomicWindowsBase<Int32>& operator+=(AtomicWindowsBase<Int32> const& other)
		{
			InterlockedExchangeAdd(reinterpret_cast<LONG volatile*>(&_AtomicValue), static_cast<LONG>(other._AtomicValue));
			return *this;
		}
		GDINL AtomicWindowsBase<Int32> operator+(AtomicWindowsBase<Int32> const& other)
		{
			return AtomicWindowsBase<Int32>(*this) += other;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// Atomically adds two value.
		//! @{
		GDINL AtomicWindowsBase<Int32>& operator&=(AtomicWindowsBase<Int32> const& other)
		{
		//	InterlockedExchangeAnd(reinterpret_cast<LONG volatile*>(&_AtomicValue), static_cast<LONG>(other._AtomicValue));
			return *this;
		}
		GDINL AtomicWindowsBase<Int32> operator&(AtomicWindowsBase<Int32> const& other)
		{
			return AtomicWindowsBase<Int32>(*this) += other;
		}
		//! @}

	};	// struct AtomicWindowsBase<Int32>

GD_NAMESPACE_END
