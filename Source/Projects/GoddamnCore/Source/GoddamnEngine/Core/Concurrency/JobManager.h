// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Concurrency/JobManager.h
 * A very light-weight Parallel job manager subsystem.
 */
#pragma once
#define GD_INSIDE_JOBMANAGER_H

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Concurrency/Atomics.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Contains utilities for parallelizing your code.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	namespace JobManager
	{
		typedef Byte JobDelegateArgs[sizeof(Handle) * 2];
		typedef void(*JobDelegate)(JobDelegateArgs* const);

		struct Job
		{
			JobDelegate     Delegate;
			JobDelegateArgs DelegateArgs;
			AtomicUInt32*	NumJobs;
		};	// struct Job

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//! A container for jobs.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		class ParallelJobList
		{
		private:
			AtomicUInt32 m_NumJobs;

		public:
			
			/*!
			 * Initializes a new parallel job list.
			 */
			GDAPI ParallelJobList();

		public:

			/*!
			 * Submits a new parallel job.
			 * Never use this function directly, use 'GD_SUBMIT_PARALLEL_JOB*' macros.
			 *
			 * @param job A job to submit.
			 */
			GDAPI void SubmitJob(Job& job);

			/*!
			 * Waits until this list proceeds all submitted jobs. 
			 */
			GDAPI void Wait() const;

		};	// class ParallelJobList

	}	// namespace JobManager

GD_NAMESPACE_END

// ReSharper disable once CppUnusedIncludeDirective
#include <GoddamnEngine/Core/Concurrency/JobManagerDelegates.h>
#undef GD_INSIDE_JOBMANAGER_H
