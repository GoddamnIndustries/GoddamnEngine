/// ==========================================================================================
/// Thread.hh: basic thread interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 21.05.2014 - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_THREADING_THREAD
#define GD_CORE_THREADING_THREAD

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/Text/String/String.hh>

GD_NAMESPACE_BEGIN

	/// @brief			Flags for thread creation
	enum class ThreadFlags : UInt8
	{
		None			= 0,
		UsingRenderer	= 1 << 0,		///< Thread uses renderer. So it creates shared context, etc
		All				= 0xFF
	};

	/// @brief			Describes 'Thread'`s state
	enum class ThreadState
	{
		Created,				///< Thread had been created, but not started
		Running,				///< Thread is running now	
		Terminated,				///< Thread was terminated in unusual way
		Finished				///< Thread has finished normally
	};

	/// @brief			Describes 'Thread'`s priority
	enum class ThreadPriority : int
	{
		NotHandle		= 100,	///< Lets OS handle 'Thread'`s priority 
		Lowest			= -2,	///< (-2) Lowest priority
		BelowNormal		= -1,	///< (-1) Below normal priority
		Normal			= 00,	///< ( 0) Normal priority
		AboveNormal		= +1,	///< (+1) Above normal
		Highest			= +2	///< (+2) highest priority
	};

	/// @brief Class for 'Thread' representation
	class Thread
	{
	private:
		ThreadFlags	flags;
		ThreadPriority priority;
		ThreadState state;
		String name;
		handle nativeThread;

		GDAPI static uint ThreadMain(Thread* thread);

	public:
		/// @name Constructor / Destructor
		/// @{

		/// @brief			Constructs thread, but not launches it
		/// @param name		Name of thread would be shown in VisualStudio
		/// @param flags	Flags for thread creation
		/// @param priority Priority of thread.
		GDAPI Thread(
			const String&			name		= "GoddamnEngine Thread",
			const ThreadFlags		flags		= ThreadFlags::None,
			const ThreadPriority	priority	= ThreadPriority::NotHandle
		);

		/// @brief			Terminates thread and destructs it
		GDAPI virtual ~Thread();

		/// @}

		/// @name Thread Execution order
		/// @{

		/// @brief			Executes the thread with delay
		/// @param seconds	Delay before thread started in seconds.
		GDAPI void ExecuteWithDelay(const float seconds);

		/// @brief			Executes the thread
		GDINL void Execute()
		{
			return self->ExecuteWithDelay(0.0f);
		}

		/// @brief			Terminates thread execution
		GDAPI void Terminate();

		/// @brief			Waits for finishing with delay 
		///					but if passed more time, that passed then terminates thread.
		/// @param seconds	Delay before thread termination in seconds, passed '-1.0f' 
		///					would wait for infinity.
		GDAPI void WaitForFinishing(const float seconds);

		/// @brief			Waits thread for finishing
		GDINL void WaitForFinishing()
		{
			return self->WaitForFinishing(-1.0f);
		}

		/// @}

		/// @name Getters 
		/// @{

		/// @brief			Returns pointer on native thread on this system
		/// @returns		Pointer on native thread on this system
		GDINL handle GetNativeThread() const
		{
			return self->nativeThread;
		}

		/// @brief			Returns 'Thread'`s state
		/// @returns		'Thread'`s state
		/// @see ThreadState
		GDINL ThreadState GetThreadState() const
		{ 
			return self->state; 
		}

		/// @brief			Test if thread is running now
		GDINL bool IsRunning() const 
		{ 
			return self->GetThreadState() == ThreadState::Running; 
		}
		
		/// @brief			Test if thread was finished
		GDINL bool IsFinished() const 
		{ 
			return self->GetThreadState() == ThreadState::Finished; 
		}

		/// @brief			Test if thread was finished
		GDINL bool IsStopped() const 
		{ 
			return self->GetThreadState() == ThreadState::Terminated; 
		}

		/// @}

	private:

		/// @brief			Overloadable method for thread execution
		GDAPI virtual void OnExecutionBegan() const abstract;
	};
	
GD_NAMESPACE_END

#endif