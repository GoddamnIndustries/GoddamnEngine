// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Concurrency/JobManagerDelegates.h
 * @note This file should be never directly included, please consider using <GoddamnEngine/Core/Concurrency/JobManager.h> instead.
 * Template delegates wrappers for job Delegates.
 */
#pragma once
#if !defined(GD_INSIDE_JOBMANAGER_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Core/Concurrency/JobManager.h> instead.
#endif	// if !defined(GD_INSIDE_JOBMANAGER_H)

#include <GoddamnEngine/Include.h>

/*!
 * Submits a generic job.
 * NEVER USE THIS MACRO DIRECTLY!
 */
#define GD_SUBMIT_PARALLEL_JOB_INTERNAL(jobList,  _TARGS_COUNT_ , _TARGS_DECLARATIONS_ , _TARGS_TYPES_ , _TARGS_NAMES_ , ... ) \
	do { \
		struct JobDelegateType { \
			GDINT static void DelegateFunction( _TARGS_DECLARATIONS_ ) { \
				__VA_ARGS__; \
			} \
		}; \
		\
		JobManager::Job job = {}; \
		job.Delegate = JobManager::JobDelegateWrap ## _TARGS_COUNT_ < _TARGS_TYPES_ , &JobDelegateType::DelegateFunction >::Get(); \
		new (&job.DelegateArgs) JobManager::JobDelegateArgsWrap ## _TARGS_COUNT_ < _TARGS_TYPES_ > ( _TARGS_NAMES_ ); \
		jobList.SubmitJob(job); \
	} while(false)

/*!
 * Submits a job without arguments.
 * @param jobList Parallel job list to attach this job to.
 */
#define GD_SUBMIT_PARALLEL_JOB0(jobList, ...) \
		GD_SUBMIT_PARALLEL_JOB_INTERNAL(jobList, 0 \
			, GD_EMPTY \
			, void /*some template argument to match the prototype*/ \
			, GD_EMPTY \
			, __VA_ARGS__ \
			)

/*!
 * Submits a job with one argument.
 *
 * @param jobList Parallel job list to attach this job to.
 * @param arg1 Name of the argument.
 */
#define GD_SUBMIT_PARALLEL_JOB1(jobList, arg1, ...) \
		GD_SUBMIT_PARALLEL_JOB_INTERNAL(jobList, 1 \
			, GD_PASS(decltype(arg1) arg1) \
			, GD_PASS(decltype(arg1)) \
			, GD_PASS(arg1) \
			, __VA_ARGS__ \
			)

/*!
 * Submits a job with two arguments.
 *
 * @param jobList Parallel job list to attach this job to.
 * @param argN Name of the N-th argument.
 */
#define GD_SUBMIT_PARALLEL_JOB2(jobList, arg1, arg2, ...) \
		GD_SUBMIT_PARALLEL_JOB_INTERNAL(jobList, 2 \
			, GD_PASS(decltype(arg1) arg1, decltype(arg2) arg2) \
			, GD_PASS(decltype(arg1), decltype(arg2)) \
			, GD_PASS(arg1, arg2) \
			, __VA_ARGS__ \
			)

/*!
 * Submits a job with three arguments.
 *
 * @param jobList Parallel job list to attach this job to.
 * @param argN Name of the N-th argument.
 */
#define GD_SUBMIT_PARALLEL_JOB3(jobList, arg1, arg2, arg3, ...) \
		GD_SUBMIT_PARALLEL_JOB_INTERNAL(jobList, 3 \
			, GD_PASS(decltype(arg1) arg1, decltype(arg2) arg2, decltype(arg3) arg3) \
			, GD_PASS(decltype(arg1), decltype(arg2), decltype(arg3)) \
			, GD_PASS(arg1, arg2, arg3) \
			, __VA_ARGS__ \
			)

/*!
 * Submits a job with four arguments.
 *
 * @param jobList Parallel job list to attach this job to.
 * @param argN Name of the N-th argument.
 */
#define GD_SUBMIT_PARALLEL_JOB4(jobList, arg1, arg2, arg3, arg4, ...) \
		GD_SUBMIT_PARALLEL_JOB_INTERNAL(jobList, 4 \
			, GD_PASS(decltype(arg1) arg1, decltype(arg2) arg2, decltype(arg3) arg3, decltype(arg4) arg4) \
			, GD_PASS(decltype(arg1), decltype(arg2), decltype(arg3), decltype(arg4)) \
			, GD_PASS(arg1, arg2, arg3, arg4) \
			, __VA_ARGS__ \
			)

/*!
 * Submits a job with five arguments.
 *
 * @param jobList Parallel job list to attach this job to.
 * @param argN Name of the N-th argument.
 */
#define GD_SUBMIT_PARALLEL_JOB5(jobList, arg1, arg2, arg3, arg4, arg5, ...) \
		GD_SUBMIT_PARALLEL_JOB_INTERNAL(jobList, 5 \
			, GD_PASS(decltype(arg1) arg1, decltype(arg2) arg2, decltype(arg3) arg3, decltype(arg4) arg4, decltype(arg5) arg5) \
			, GD_PASS(decltype(arg1), decltype(arg2), decltype(arg3), decltype(arg4), decltype(arg5)) \
			, GD_PASS(arg1, arg2, arg3, arg4, arg5) \
			, __VA_ARGS__ \
			)

/*!
 * Submits a job with fix arguments.
 *
 * @param jobList Parallel job list to attach this job to.
 * @param argN Name of the N-th argument.
 */
#define GD_SUBMIT_PARALLEL_JOB6(jobList, arg1, arg2, arg3, arg4, arg5, arg6, ...) \
		GD_SUBMIT_PARALLEL_JOB_INTERNAL(jobList, 6 \
			, GD_PASS(decltype(arg1) arg1, decltype(arg2) arg2, decltype(arg3) arg3, decltype(arg4) arg4, decltype(arg5) arg5, decltype(arg6) arg6) \
			, GD_PASS(decltype(arg1), decltype(arg2), decltype(arg3), decltype(arg4), decltype(arg5), decltype(arg6)) \
			, GD_PASS(arg1, arg2, arg3, arg4, arg5, arg6) \
			, __VA_ARGS__ \
			)

/*!
 * Submits a job with seven arguments.
 *
 * @param jobList Parallel job list to attach this job to.
 * @param argN Name of the N-th argument.
 */
#define GD_SUBMIT_PARALLEL_JOB7(jobList, arg1, arg2, arg3, arg4, arg5, arg6, arg7, ...) \
		GD_SUBMIT_PARALLEL_JOB_INTERNAL(jobList, 7 \
			, GD_PASS(decltype(arg1) arg1, decltype(arg2) arg2, decltype(arg3) arg3, decltype(arg4) arg4, decltype(arg5) arg5, decltype(arg6),arg6, decltype(arg7) arg7) \
			, GD_PASS(decltype(arg1), decltype(arg2), decltype(arg3), decltype(arg4), decltype(arg5), decltype(arg6), decltype(arg7)) \
			, GD_PASS(arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
			, __VA_ARGS__ \
			)

/*!
 * Submits a job with eight arguments.
 *
 * @param jobList Parallel job list to attach this job to.
 * @param argN Name of the N-th argument.
 */
#define GD_SUBMIT_PARALLEL_JOB8(jobList, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, ...) \
		GD_SUBMIT_PARALLEL_JOB_INTERNAL(jobList, 8 \
			, GD_PASS(decltype(arg1) arg1, decltype(arg2) arg2, decltype(arg3) arg3, decltype(arg4) arg4, decltype(arg5) arg5, decltype(arg6) arg6, decltype(arg7) arg7, decltype(arg8) arg8) \
			, GD_PASS(decltype(arg1), decltype(arg2), decltype(arg3), decltype(arg4), decltype(arg5), decltype(arg6), decltype(arg7), decltype(arg8)) \
			, GD_PASS(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) \
			, __VA_ARGS__ \
			)

#define GD_JOB_DELEGATES_PACK_PUSH __pragma(pack(push, 2))
#define GD_JOB_DELEGATES_PACK_POP  __pragma(pack(pop))

GD_NAMESPACE_BEGIN

	/*!
	 * Internal namespace that contains details for Job system implementation.
	 */
	namespace JobManager
	{
		// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
		// ******                             Job Arguments Wrappers.                              ******
		// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

		GD_JOB_DELEGATES_PACK_PUSH
		template<typename = void /*some template argument to match the prototype*/>
		struct JobDelegateArgsWrap0
		{
		public:
			GDINL explicit JobDelegateArgsWrap0()
			{}
		};	// struct JobDelegateArgsWrap0
		GD_JOB_DELEGATES_PACK_POP

		GD_JOB_DELEGATES_PACK_PUSH
		template<typename TArg1>
		struct JobDelegateArgsWrap1
		{
			TArg1 Arg1;

		public:
			GDINL explicit JobDelegateArgsWrap1(TArg1 arg1)
				: Arg1(arg1)
			{
				static_assert(sizeof(JobDelegateArgsWrap1) <= sizeof(JobDelegateArgs), "Total size of arguments runs out of bounds.");
			}
		} GD_ALIGN_GCC(16);	// struct JobDelegateArgsWrap1<TArg1>
		GD_JOB_DELEGATES_PACK_POP

		GD_JOB_DELEGATES_PACK_PUSH
		template<typename TArg1, typename TArg2>
		struct JobDelegateArgsWrap2 final : public TNonCopyable
		{
			TArg1 Arg1;
			TArg2 Arg2;

		public:
			GDINL explicit JobDelegateArgsWrap2(TArg1 arg1, TArg2 arg2)
				: Arg1(arg1), Arg2(arg2)
			{
				static_assert(sizeof(JobDelegateArgsWrap2) <= sizeof(JobDelegateArgs), "Total size of arguments runs out of bounds.");
			}
		} GD_ALIGN_GCC(16);	// struct JobDelegateArgsWrap2<TArg1, TArg2>
		GD_JOB_DELEGATES_PACK_POP

		GD_JOB_DELEGATES_PACK_PUSH
		template<typename TArg1, typename TArg2, typename TArg3>
		struct JobDelegateArgsWrap3 final : public TNonCopyable
		{
			TArg1 Arg1;
			TArg2 Arg2;
			TArg3 Arg3;

		public:
			GDINL explicit JobDelegateArgsWrap3(TArg1 arg1, TArg2 arg2, TArg3 arg3)
				: Arg1(arg1), Arg2(arg2), Arg3(arg3)
			{
				static_assert(sizeof(JobDelegateArgsWrap3) <= sizeof(JobDelegateArgs), "Total size of arguments runs out of bounds.");
			}
		} GD_ALIGN_GCC(16);	// struct JobDelegateArgsWrap3<TArg1, TArg2, TArg3>
		GD_JOB_DELEGATES_PACK_POP

		GD_JOB_DELEGATES_PACK_PUSH
		template<typename TArg1, typename TArg2, typename TArg3, typename TArg4>
		struct JobDelegateArgsWrap4 final : public TNonCopyable
		{
			TArg1 Arg1;
			TArg2 Arg2;
			TArg3 Arg3;
			TArg4 Arg4;

		public:
			GDINL explicit JobDelegateArgsWrap4(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4)
				: Arg1(arg1), Arg2(arg2), Arg3(arg3), Arg4(arg4)
			{
				static_assert(sizeof(JobDelegateArgsWrap4) <= sizeof(JobDelegateArgs), "Total size of arguments runs out of bounds.");
			}
		} GD_ALIGN_GCC(16);	// struct JobDelegateArgsWrap4<TArg1, TArg2, TArg3, TArg4>
		GD_JOB_DELEGATES_PACK_POP

		GD_JOB_DELEGATES_PACK_PUSH
		template<typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5>
		struct JobDelegateArgsWrap5 final : public TNonCopyable
		{
			TArg1 Arg1;
			TArg2 Arg2;
			TArg3 Arg3;
			TArg4 Arg4;
			TArg5 Arg5;

		public:
			GDINL explicit JobDelegateArgsWrap5(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4, TArg5 arg5)
				: Arg1(arg1), Arg2(arg2), Arg3(arg3), Arg4(arg4), Arg5(arg5)
			{
				static_assert(sizeof(JobDelegateArgsWrap5) <= sizeof(JobDelegateArgs), "Total size of arguments runs out of bounds.");
			}
		} GD_ALIGN_GCC(16);	// struct JobDelegateArgsWrap5<TArg1, TArg2, TArg3, TArg4, TArg5>
		GD_JOB_DELEGATES_PACK_POP

		GD_JOB_DELEGATES_PACK_PUSH
		template<typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6>
		struct JobDelegateArgsWrap6 final : public TNonCopyable
		{
			TArg1 Arg1;
			TArg2 Arg2;
			TArg3 Arg3;
			TArg4 Arg4;
			TArg5 Arg5;
			TArg6 Arg6;

		public:
			GDINL explicit JobDelegateArgsWrap6(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4, TArg5 arg5, TArg6 arg6)
				: Arg1(arg1), Arg2(arg2), Arg3(arg3), Arg4(arg4), Arg5(arg5), Arg6(arg6)
			{
				static_assert(sizeof(JobDelegateArgsWrap6) <= sizeof(JobDelegateArgs), "Total size of arguments runs out of bounds.");
			}
		} GD_ALIGN_GCC(16);	// struct JobDelegateArgsWrap6<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6>
		GD_JOB_DELEGATES_PACK_POP

		GD_JOB_DELEGATES_PACK_PUSH
		template<typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, typename TArg7>
		struct JobDelegateArgsWrap7 final : public TNonCopyable
		{
			TArg1 Arg1;
			TArg2 Arg2;
			TArg3 Arg3;
			TArg4 Arg4;
			TArg5 Arg5;
			TArg6 Arg6;
			TArg7 Arg7;

		public:
			GDINL explicit JobDelegateArgsWrap7(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4, TArg5 arg5, TArg6 arg6, TArg7 arg7)
				: Arg1(arg1), Arg2(arg2), Arg3(arg3), Arg4(arg4), Arg5(arg5), Arg6(arg6), Arg7(arg7)
			{
				static_assert(sizeof(JobDelegateArgsWrap7) <= sizeof(JobDelegateArgs), "Total size of arguments runs out of bounds.");
			}
		} GD_ALIGN_GCC(16);	// struct JobDelegateArgsWrap7<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7>
		GD_JOB_DELEGATES_PACK_POP

		GD_JOB_DELEGATES_PACK_PUSH
		template<typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, typename TArg7, typename TArg8>
		struct JobDelegateArgsWrap8 final : public TNonCopyable
		{
			TArg1 Arg1;
			TArg2 Arg2;
			TArg3 Arg3;
			TArg4 Arg4;
			TArg5 Arg5;
			TArg6 Arg6;
			TArg7 Arg7;
			TArg8 Arg8;

		public:
			GDINL explicit JobDelegateArgsWrap8(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4, TArg5 arg5, TArg6 arg6, TArg7 arg7, TArg8 arg8)
				: Arg1(arg1), Arg2(arg2), Arg3(arg3), Arg4(arg4), Arg5(arg5), Arg6(arg6), Arg7(arg7), Arg8(arg8)
			{
				static_assert(sizeof(JobDelegateArgsWrap8) <= sizeof(JobDelegateArgs), "Total size of arguments runs out of bounds.");
			}
		} GD_ALIGN_GCC(16);	// struct JobDelegateArgsWrap8<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8>
		GD_JOB_DELEGATES_PACK_POP

		// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
		// ******                             Job Delegates Wrappers.                              ******
		// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

		template<typename /*some template argument to match the prototype*/
			, void(*TFunc)()>
		struct JobDelegateWrap0 final : public TNonCreatable
		{
		private:
			GDINT static void Delegate(JobDelegateArgs* const DelegateArgs)
			{
				GD_NOT_USED(DelegateArgs);
				TFunc();
			}
		public:
			GDINL static JobDelegate Get()
			{
				return &Delegate;
			}
		};	//	struct JobDelegateWrap<TArg1, TFunc>

		template<typename TArg1, void(*TFunc)(TArg1)>
		struct JobDelegateWrap1 final : public TNonCreatable
		{
		private:
			GDINT static void Delegate(JobDelegateArgs* const DelegateArgs)
			{
				typedef JobDelegateArgsWrap1<TArg1> JobDelegateArgsWrap;
				auto const args = reinterpret_cast<JobDelegateArgsWrap*>(DelegateArgs);
				TFunc(args->Arg1);
				args->~JobDelegateArgsWrap();
			}
		public:
			GDINL static JobDelegate Get()
			{
				return &Delegate;
			}
		};	//	struct JobDelegateWrap<TArg1, TFunc>

		template<typename TArg1, typename TArg2
			, void(*TFunc)(TArg1, TArg2)>
		struct JobDelegateWrap2 final : public TNonCreatable
		{
		private:
			GDINT static void Delegate(JobDelegateArgs* const DelegateArgs)
			{
				typedef JobDelegateArgsWrap2<TArg1, TArg2> JobDelegateArgsWrap;
				auto const args = reinterpret_cast<JobDelegateArgsWrap*>(DelegateArgs);
				TFunc(args->Arg1, args->Arg2);
				args->~JobDelegateArgsWrap();
			}
		public:
			GDINL static JobDelegate Get()
			{
				return &Delegate;
			}
		};	//	struct JobDelegateWrap<TArg1, TArg2, TFunc>

		template<typename TArg1, typename TArg2, typename TArg3
			, void(*TFunc)(TArg1, TArg2, TArg3)>
		struct JobDelegateWrap3 final : public TNonCreatable
		{
		private:
			GDINT static void Delegate(JobDelegateArgs* const DelegateArgs)
			{
				typedef JobDelegateArgsWrap3<TArg1, TArg2, TArg3> JobDelegateArgsWrap;
				auto const args = reinterpret_cast<JobDelegateArgsWrap*>(DelegateArgs);
				TFunc(args->Arg1, args->Arg2, args->Arg3);
				args->~JobDelegateArgsWrap();
			}
		public:
			GDINL static JobDelegate Get()
			{
				return &Delegate;
			}
		};	//	struct JobDelegateWrap<TArg1, TArg2, TArg3, TFunc>

		template<typename TArg1, typename TArg2, typename TArg3, typename TArg4
			, void(*TFunc)(TArg1, TArg2, TArg3, TArg4)>
		struct JobDelegateWrap4 final : public TNonCreatable
		{
		private:
			GDINT static void Delegate(JobDelegateArgs* const DelegateArgs)
			{
				typedef JobDelegateArgsWrap4<TArg1, TArg2, TArg3, TArg4> JobDelegateArgsWrap;
				auto const args = reinterpret_cast<JobDelegateArgsWrap*>(DelegateArgs);
				TFunc(args->Arg1, args->Arg2, args->Arg3, args->Arg4);
				args->~JobDelegateArgsWrap();
			}
		public:
			GDINL static JobDelegate Get()
			{
				return &Delegate;
			}
		};	//	struct JobDelegateWrap<TArg1, TArg2, TArg3, TArg4, TFunc>

		template<typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5
			, void(*TFunc)(TArg1, TArg2, TArg3, TArg4, TArg5)>
		struct JobDelegateWrap5 final : public TNonCreatable
		{
		private:
			GDINT static void Delegate(JobDelegateArgs* const DelegateArgs)
			{
				typedef JobDelegateArgsWrap5<TArg1, TArg2, TArg3, TArg4, TArg5> JobDelegateArgsWrap;
				auto const args = reinterpret_cast<JobDelegateArgsWrap*>(DelegateArgs);
				TFunc(args->Arg1, args->Arg2, args->Arg3, args->Arg4, args->Arg5);
				args->~JobDelegateArgsWrap();
			}
		public:
			GDINL static JobDelegate Get()
			{
				return &Delegate;
			}
		};	//	struct JobDelegateWrap<TArg1, TArg2, TArg3, TArg4, TArg5, TFunc>

		template<typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, void(*TFunc)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6)>
		struct JobDelegateWrap6 final : public TNonCreatable
		{
		private:
			GDINT static void Delegate(JobDelegateArgs* const DelegateArgs)
			{
				typedef JobDelegateArgsWrap6<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6> JobDelegateArgsWrap;
				auto const args = reinterpret_cast<JobDelegateArgsWrap*>(DelegateArgs);
				TFunc(args->Arg1, args->Arg2, args->Arg3, args->Arg4, args->Arg5, args->Arg6);
				args->~JobDelegateArgsWrap();
			}
		public:
			GDINL static JobDelegate Get()
			{
				return &Delegate;
			}
		};	//	struct JobDelegateWrap<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TFunc>

		template<typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, typename TArg7
			, void(*TFunc)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7)>
		struct JobDelegateWrap7 final : public TNonCreatable
		{
		private:
			GDINT static void Delegate(JobDelegateArgs* const DelegateArgs)
			{
				typedef JobDelegateArgsWrap7<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7> JobDelegateArgsWrap;
				auto const args = reinterpret_cast<JobDelegateArgsWrap*>(DelegateArgs);
				TFunc(args->Arg1, args->Arg2, args->Arg3, args->Arg4, args->Arg5, args->Arg6, args->Arg7);
				args->~JobDelegateArgsWrap();
			}
		public:
			GDINL static JobDelegate Get()
			{
				return &Delegate;
			}
		};	//	struct JobDelegateWrap7<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TFunc>

		template<typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, typename TArg7, typename TArg8
			, void(*TFunc)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8)>
		struct JobDelegateWrap8 final : public TNonCreatable
		{
		private:
			GDINT static void Delegate(JobDelegateArgs* const DelegateArgs)
			{
				typedef JobDelegateArgsWrap8<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8> JobDelegateArgsWrap;
				auto const args = reinterpret_cast<JobDelegateArgsWrap*>(DelegateArgs);
				TFunc(args->Arg1, args->Arg2, args->Arg3, args->Arg4, args->Arg5, args->Arg6, args->Arg7, args->Arg8);
				args->~JobDelegateArgsWrap();
			}
		public:
			GDINL static JobDelegate Get()
			{
				return &Delegate;
			}
		};	//	struct JobDelegateWrap8<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8, TFunc>

	}	// namespace JobManager

GD_NAMESPACE_END
