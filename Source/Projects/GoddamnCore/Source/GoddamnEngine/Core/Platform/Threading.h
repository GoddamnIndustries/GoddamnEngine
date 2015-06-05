// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Platform/Threading.h
//! Cross-platform Threading implementation.

#pragma once
#define GD_INSIDE_THREADING_H

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Platform/Platform.h>

GD_NAMESPACE_BEGIN

	typedef Int32 TimeoutTp;
	TimeoutTp static const TimeoutInfinite = -1;

	// ------------------------------------------------------------------------------------------
	//! Generic Critical Section class.
	class IGenericCriticalSection
	{
	protected:
		GDINL IGenericCriticalSection() {}
		GDINL virtual ~IGenericCriticalSection() {}

	public:

		// ------------------------------------------------------------------------------------------
		//! Locks the Critical Section.
		GDINL virtual void Enter()
		{
		}

		// ------------------------------------------------------------------------------------------
		//! Unlocks the Critical Section.
		GDINL virtual void Leave()
		{
		}
	};	// class IGenericCriticalSection
		
	// ------------------------------------------------------------------------------------------
	//! RAII object that locks specified object.
	class ScopedLock final : IUncopiable
	{
	private:
		IGenericCriticalSection* const SynchObject;

	public:

		// ------------------------------------------------------------------------------------------
		//! Initializes scoped lock and locks sync object.
		//! @param SynchObject Object which would be locked.
		GDINL explicit ScopedLock(IGenericCriticalSection* const SynchObject)
			: SynchObject(SynchObject)
		{
			GD_DEBUG_ASSERT(SynchObject != nullptr, "Null pointer critical section specified.");
			this->SynchObject->Enter();
		}

		// ------------------------------------------------------------------------------------------
		//! Deinitializes scoped lock and unlocks sync object.
		GDINL ~ScopedLock()
		{
			this->SynchObject->Leave();
		}
	};	// class ScopedLock

	// ------------------------------------------------------------------------------------------
	//! Generic signal/event class.
	class IGenericThreadSignal : public IUncopiable
	{
	protected:

		// ------------------------------------------------------------------------------------------
		//! Initializes the current signal.
		//! @param RequiresManualReset Does the signal requires to be manually reseted.
		GDINL IGenericThreadSignal(bool const RequiresManualReset = false) {}
		GDINL virtual ~IGenericThreadSignal() {}

	public:

		// ------------------------------------------------------------------------------------------
		//! Raises the current signal.
		GDAPI virtual void RaiseSignal()
		{
		}

		// ------------------------------------------------------------------------------------------
		//! Clears the current signal.
		GDAPI virtual void ClearSignal()
		{
		}

		// ------------------------------------------------------------------------------------------
		//! Waits for signal to be raised with specified timeout.
		//! @param TimeoutInMilliseconds Specified timeout in milliseconds. Can be specified as infinite.
		//! @returns @c true, if signal was raised in the timeout period, @c false if timeout was reached.
		GDAPI virtual bool WaitForSignal(TimeoutTp const TimeoutInMilliseconds) const
		{
			GD_NOT_USED(TimeoutInMilliseconds);
			return true;
		}
	};	// class IGenericThreadSignal

GD_NAMESPACE_END

#if GD_PLATFORM_HAS_MULTITHREADING
#	include GD_PLATFORM_INCLUDE(GoddamnEngine/Core/Platform, Threading.h)
#else	// if GD_PLATFORM_HAS_MULTITHREADING
GD_NAMESPACE_BEGIN
	//! Represents definitions for Critical Section.
	typedef IGenericCriticalSection CriticalSection;
GD_NAMESPACE_END
#endif	// if GD_PLATFORM_HAS_MULTITHREADING

#undef GD_INSIDE_THREADING_H
