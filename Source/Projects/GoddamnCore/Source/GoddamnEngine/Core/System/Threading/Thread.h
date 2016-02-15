// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/System/Threading/Thread.h
 * File contains cross-platform thread implementation.
 */
#pragma once
#define GD_INSIDE_THREAD_H

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/System/Platform.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Defines a thread priority.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	enum class ThreadPriority : UInt8
	{
		Lowest,
		BelowNormal,
		Normal,
		AboveNormal,
		Highest,
	};	// enum class ThreadPriority

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Defines a thread ID.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	typedef UInt32 ThreadID;

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Generic thread implementation.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class ThreadGeneric : public IVirtuallyDestructible
	{
	public:

		/*!
		 * Initializes a new thread.
		 *
		 * @param threadName Name of the thread. 
		 * @param threadPriority Priority of the thread.
		 */
		GDINL explicit ThreadGeneric(CStr const threadName, ThreadPriority const threadPriority = ThreadPriority::Normal)
		{
			GD_DEBUG_ASSERT(threadName != nullptr, "Null pointer thread name was specified.");
			GD_NOT_USED(threadPriority);
		}

		GDINL virtual ~ThreadGeneric()
		{
		}

	public:

		/*!
		 * Waits for thread to end execution.
		 */
		GDINL void Wait() const
		{
			GD_NOT_IMPLEMENTED();
		}

		/*!
		 * Returns ID of the caller's thread.
		 */
		GDINL static ThreadID GetCurrentThreadID()
		{
			GD_NOT_IMPLEMENTED();
		}

		/*!
		 * Returns ID of the main thread.
		 */
		GDINL static ThreadID GetMainThreadID()
		{
			GD_NOT_IMPLEMENTED();
		}

	protected:

		/*!
		 * @brief Callback being triggered when thread starts executing.
		 */
		GDAPI virtual void OnRun() {}

	};	// class ThreadGeneric

GD_NAMESPACE_END

#include GD_PLATFORM_INCLUDE(GoddamnEngine/Core/System/Threading, Thread)
#undef GD_INSIDE_ATOMICS_H
