// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Concurrency/ThreadWindows.h
 * File contains Windows thread implementation.
 */
#pragma once
#if !defined(GD_INSIDE_THREAD_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Core/Concurrency/Atomics.h> instead.
#endif	// if !defined(GD_INSIDE_THREAD_H)

#include <Windows.h>
#include <GoddamnEngine/Core/Containers/String.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Windows thread implementation.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class ThreadWindows : public ThreadGeneric
	{
	private:
		UInt32 m_ThreadID;
		HANDLE m_ThreadHandle;
		HANDLE m_ThreadStartEvent;
		String m_ThreadName;

		GDINT static unsigned __stdcall ThreadWindowsProc(ThreadWindows* const threadObject);

	public:

		GDAPI explicit ThreadWindows(CStr const threadName, ThreadPriority const threadPriority = ThreadPriority::Normal);

		GDAPI virtual ~ThreadWindows();

		GDAPI void Wait() const;

		/*!
		 * Returns ID of the caller's thread.
		 */
		GDINL static ThreadID GetCurrentThreadID()
		{
			return static_cast<ThreadID>(::GetCurrentThreadId());
		}

	};	// class ThreadWindows

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Cross-platform thread implementation.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	typedef ThreadWindows Thread;

GD_NAMESPACE_END
