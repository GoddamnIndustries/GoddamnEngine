// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file
 * Thread implementation.
 */
#pragma once
#if !defined(GD_INSIDE_THREAD_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Core/Concurrency/Atomics.h> instead.
#endif	// if !defined(GD_INSIDE_THREAD_H)

#include <Windows.h>
#include <GoddamnEngine/Core/Containers/String.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! @copydoc ThreadGeneric
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class ThreadMicrosoft : public ThreadGeneric
	{
	private:
		UInt32 m_ThreadID;
		HANDLE m_ThreadHandle;
		HANDLE m_ThreadStartEvent;
		String m_ThreadName;

		GDINT static unsigned __stdcall ThreadProc(ThreadMicrosoft* const threadObject);

	public:

		GDAPI explicit ThreadMicrosoft(CStr const threadName, ThreadPriority const threadPriority = ThreadPriority::Normal);

		GDAPI virtual ~ThreadMicrosoft();

		GDAPI void Wait() const;

		/*!
		 * Returns ID of the caller's thread.
		 */
		GDINL static ThreadID GetCurrentThreadID()
		{
			return static_cast<ThreadID>(::GetCurrentThreadId());
		}

	};	// class ThreadMicrosoft

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! @copydoc ThreadMicrosoft
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	using Thread = ThreadMicrosoft;

GD_NAMESPACE_END
