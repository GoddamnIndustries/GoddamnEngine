// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/System/Threading/ThreadWindows.cpp
 * File contains Windows thread implementation.
 */
#include <GoddamnEngine/Core/System/Threading/Thread.h>
#if GD_PLATFORM_WINDOWS

#include <process.h>

GD_NAMESPACE_BEGIN

	/*!
	 * Entry point for each new thread.
	 *
	 * @param threadObject Thread object to execute.
	 * @returns Thread execution status.
	 */
	GDINT unsigned __stdcall ThreadWindows::ThreadWindowsProc(ThreadWindows* const threadObject)
	{
		GD_DEBUG_ASSERT(threadObject != nullptr, "Null pointer thread object was specified.");

#if GD_DEBUG
		// Setting name of the thread. 
		// https://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
		// https://msdn.microsoft.com/en-us/library/xwtb73ad.aspx
		[=]()
		{
			DWORD const MS_VC_EXCEPTION = 0x406D1388;

			__pragma(pack(push, 8))
				typedef struct tagTHREADNAME_INFO
			{
				DWORD  dwType;		// Must be 0x1000.
				LPCSTR szName;		// pointer to name (in user address space).
				DWORD  dwThreadID;	// Thread ID (-1=caller thread).
				DWORD  dwFlags;		// Reserved for future use, must be zero.
			} THREADNAME_INFO;
			__pragma(pack(pop))

			THREADNAME_INFO threadInfo = {};
			threadInfo.dwType = 0x1000;
			threadInfo.szName = threadObject->m_ThreadName.CStr();
			threadInfo.dwThreadID = static_cast<DWORD>(-1);

			__try
			{
				RaiseException(MS_VC_EXCEPTION, 0, sizeof threadInfo / sizeof(ULONG_PTR), reinterpret_cast<ULONG_PTR const*>(&threadInfo));
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{}
		}();
#endif	// if GD_DEBUG

		if (WaitForSingleObject(threadObject->m_ThreadStartEvent, INFINITE) == WAIT_OBJECT_0)
		{
			threadObject->OnRun();
			_endthreadex(ERROR_SUCCESS);
			return ERROR_SUCCESS;
		}
		_endthreadex(ERROR_NOT_READY);
		return ERROR_NOT_READY;
	}

	/*!
	 * Initializes a new thread.
	 *
	 * @param threadName Name of the thread.
	 * @param threadPriority Priority of the thread.
	 */
	GDAPI ThreadWindows::ThreadWindows(CStr const threadName, ThreadPriority const threadPriority /*= ThreadPriority::Normal*/)
		: ThreadGeneric(threadName, threadPriority)
		, m_ThreadID(0), m_ThreadHandle(nullptr), m_ThreadStartEvent(nullptr), m_ThreadName(threadName)
	{
		// Creating the event that indicates that thread is setup and ready for work.
		m_ThreadStartEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		GD_ASSERT(m_ThreadStartEvent != INVALID_HANDLE_VALUE && m_ThreadStartEvent != nullptr, "'CreateEvent' function has failed.");

		// Creating the thread itself and setting up the parameters.
		m_ThreadHandle = reinterpret_cast<HANDLE>(
			_beginthreadex(nullptr, 5 * 1024 * 1024
			, reinterpret_cast<_beginthreadex_proc_type>(&ThreadWindowsProc)
			, this, STACK_SIZE_PARAM_IS_A_RESERVATION, nullptr)
			);
		GD_ASSERT(m_ThreadHandle != nullptr, "'_beginthreadex' function has failed.");

		// Getting thread ID.
		m_ThreadID = GetThreadId(m_ThreadHandle);
		GD_ASSERT(m_ThreadID != 0, "'GetThreadId' function has failed.");

		if (threadPriority != ThreadPriority::Normal)
		{
			static int winThreadPriorityTable[] = {
				/* ThreadPriority::Lowest      */ THREAD_PRIORITY_LOWEST,
				/* ThreadPriority::BelowNormal */ THREAD_PRIORITY_BELOW_NORMAL,
				/* ThreadPriority::Normal      */ THREAD_PRIORITY_NORMAL,
				/* ThreadPriority::AboveNormal */ THREAD_PRIORITY_ABOVE_NORMAL,
				/* ThreadPriority::Highest     */ THREAD_PRIORITY_HIGHEST,
			};
			GD_ASSERT(SetThreadPriority(m_ThreadHandle, winThreadPriorityTable[static_cast<SizeTp>(threadPriority)]) == TRUE
				, "'SetThreadPriority' function has failed.");
		}

		// We are ready to start the thread.
		GD_ASSERT(SetEvent(m_ThreadStartEvent) == TRUE, "'SetEvent' function has failed.");
	}

	GDAPI ThreadWindows::~ThreadWindows()
	{
		Wait();
		CloseHandle(m_ThreadHandle);
		CloseHandle(m_ThreadStartEvent);
	}

	/*!
	 * Waits for thread to end execution.
	 */
	GDAPI void ThreadWindows::Wait() const
	{
		GD_ASSERT(WaitForSingleObject(m_ThreadHandle, INFINITE) == WAIT_OBJECT_0, "'WaitForSingleObject' function has failed.");
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_WINDOWS
