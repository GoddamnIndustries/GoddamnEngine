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

	//! @brief Generic Critical Section class.
	class IGenericCriticalSection
	{
	public:
		//! @brief Initializes a Critical Section.
		GDINL IGenericCriticalSection() {}

		//! @brief Deinitializes a Critical Section.
		GDINL virtual ~IGenericCriticalSection()
		{
		}

		//! @brief Locks the Critical Section.
		GDINL virtual void Enter() { }

		//! @brief Unlocks the Critical Section.
		GDINL virtual void Leave() {}
	};	// class IGenericCriticalSection
		
	//! @brief RAII object that locks specified object.
	class ScopedLock final : IUncopiable
	{
	private:
		IGenericCriticalSection* const SynchObject;

	public:
		//! @brief Initializes scoped lock and locks sync object.
		//! @param SynchObject Object which would be locked.
		GDINL explicit ScopedLock(IGenericCriticalSection* const SynchObject)
			: SynchObject(SynchObject)
		{
			GD_ASSERT(SynchObject != nullptr, "Null pointer critical section specified.");
			this->SynchObject->Enter();
		}

		//! @brief Deinitializes scoped lock and unlocks sync object.
		GDINL ~ScopedLock()
		{
			this->SynchObject->Leave();
		}
	};	// class ScopedLock

	class IGenericEvent
	{
	public:
		GDAPI IGenericEvent() { }
		GDAPI virtual ~IGenericEvent() { }

		GDAPI virtual bool CreateEvent(bool const RequiresManualResetting = false) abstract;
		GDAPI virtual void TriggerEvent() abstract;
		GDAPI virtual bool AwaitForEvent() abstract;
	};	// class IGenericEvent



GD_NAMESPACE_END

#if GD_PLATFORM_HAS_MULTITHREADING
#	include GD_PLATFORM_INCLUDE(GoddamnEngine/Core/Platform, Threading.h)
#else	// if GD_PLATFORM_HAS_MULTITHREADING
GD_NAMESPACE_BEGIN
	//! @brief Represents definitions for Critical Section.
	typedef IGenericCriticalSection CriticalSection;
GD_NAMESPACE_END
#endif	// if GD_PLATFORM_HAS_MULTITHREADING

#undef GD_INSIDE_THREADING_H
