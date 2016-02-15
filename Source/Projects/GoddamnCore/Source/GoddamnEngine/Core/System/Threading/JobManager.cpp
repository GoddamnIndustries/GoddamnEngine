// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/System/Threading/JobManager.cpp
 * A very light-weight Parallel job manager subsystem.
 */
#include <GoddamnEngine/Core/System/Threading/JobManager.h>
#include <GoddamnEngine/Core/System/Threading/Thread.h>
#include <GoddamnEngine/Core/System/Threading/LockFreeStack.h>

#include <GoddamnEngine/Core/System/App/App.h>
#include <GoddamnEngine/Core/System/Misc/Misc.h>

#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Containers/Vector.h>
#include <GoddamnEngine/Core/Templates/UniquePtr.h>

GD_NAMESPACE_BEGIN

	namespace JobManager
	{
		GDINT static void InitManager();

		class JobWorkerThread final : public Thread
		{
		public:
			LockFreeStack<Job> m_Jobs;

		public:
			GDINT explicit JobWorkerThread(UInt32 const workerId);

		public:
			GDINT virtual void OnRun() override final;
		};	// class JobWorkerThread
		
		Vector<UniquePtr<JobWorkerThread>> g_WorkerThreads;
		AtomicUInt32                       g_LastUsedWorkerThread;
	}	// namespace JobManager

	GDINT void JobManager::InitManager()
	{
		UInt32 const workerThreadsCount = 4;
		GD_STUBBED(workerThreadsCount);

		g_WorkerThreads.Resize(workerThreadsCount);
		for (UInt32 cnt = 0; cnt < g_WorkerThreads.GetLength(); ++cnt)
		{
			g_WorkerThreads[cnt] = gd_new JobWorkerThread(cnt);
		}
	}

	// ------------------------------------------------------------------------------------------
	// JobWorkerThread class.
	// ------------------------------------------------------------------------------------------

	/*!
	 * Initializes a new worker thread.
	 */
	GDINT JobManager::JobWorkerThread::JobWorkerThread(UInt32 const workerId)
		: Thread(String::Format("Worker thread #%d", workerId).CStr())
	{}

	/*!
	 * Entry point for the job worker.
	 * Executes all upcoming jobs and updates stats.
	 */
	GDINT void JobManager::JobWorkerThread::OnRun()
	{
		while (!App::IsTerminating())
		{
			Job job;
			while (m_Jobs.PopBack(job))
			{
				// Executing the job and marking it complete.
				job.Delegate(&job.DelegateArgs);
				--*job.NumJobs;
			}
		}
	}

	// ------------------------------------------------------------------------------------------
	// ParallelJobList class.
	// ------------------------------------------------------------------------------------------

	/*!
	 * Initializes a new parallel job list.
	 */
	GDAPI JobManager::ParallelJobList::ParallelJobList()
	{
		static auto initialized = false;
		if (initialized)
		{
			return;
		}

		// Initializing the manager.
		InitManager();
		initialized = true;
	}

	/*!
	 * Submits a new parallel job.
	 * Never use this function directly, use 'GD_SUBMIT_PARALLEL_JOB*' macros.
	 *
	 * @param job A job to submit.
	 */
	GDAPI void JobManager::ParallelJobList::SubmitJob(Job& job)
	{
		GD_DEBUG_ASSERT(job.Delegate != nullptr, "Invalid job was specified.");

		++m_NumJobs;

		// Adding job to the next used worker.
		job.NumJobs = &m_NumJobs;
		g_WorkerThreads[g_LastUsedWorkerThread % g_WorkerThreads.GetLength()]->m_Jobs.PushBack(job);
		g_LastUsedWorkerThread += 1;
	}

	/*!
	 * Waits until this list proceeds all submitted jobs. 
	 */
	GDAPI void JobManager::ParallelJobList::Wait() const
	{
		// Just waiting for amount of jobs in this list reaches zero. 
		while (m_NumJobs != 0)
		{
			PlatformMisc::Sleep(1);
		}
	}

GD_NAMESPACE_END
