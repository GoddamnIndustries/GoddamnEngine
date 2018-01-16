// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Concurrency/JobManager.cpp
 * A very light-weight Parallel job manager subsystem.
 */
#include <GoddamnEngine/Core/Concurrency/JobManager.h>
#include <GoddamnEngine/Core/Concurrency/Thread.h>
#include <GoddamnEngine/Core/Concurrency/LockFreeStack.h>

#include <GoddamnEngine/Core/App/App.h>
#include <GoddamnEngine/Core/Misc/Misc.h>
#include <GoddamnEngine/Core/IO/FileStream.h>

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
		GD_DEBUG_VERIFY(job.Delegate != nullptr, "Invalid job was specified.");

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

	/*gd_testing_unit_test(JobManager)
	{
		JobManager::ParallelJobList jobList = {};

		clock_t tx = 0;
		for (int y = 0; y < 20; ++y)
		{
			auto const t0 = clock();

			WideString strings[] = { L"hello", L"world", L"fuck", L"everybody", L"rage" };
			for (UInt32 cnt = 0; cnt < GetLength(strings); ++cnt)
			{
				auto const& s = strings[cnt];
				GD_SUBMIT_PARALLEL_JOB(jobList, s, cnt,
				{
					WideString ss = s;
					for (UInt32 cnt1 = 0; cnt1 < 1000; ++cnt1)
					{
						ss += s.SubstringLen(1, s.GetLength() - 2);
						ss += s;
					}

					auto l = SharedPtr<OutputStream>(gd_new FileOutputStream(s.CStr()));
					l->Write(ss.CStr(), ss.GetLength(), sizeof(WideChar));
				});
			}

			jobList.Wait();

			auto const t1 = clock() - t0;
			tx += t1;
			printf_s("\n%d", t1);
		}
		printf_s("\n==================%d", tx / 20);
	};*/

GD_NAMESPACE_END
