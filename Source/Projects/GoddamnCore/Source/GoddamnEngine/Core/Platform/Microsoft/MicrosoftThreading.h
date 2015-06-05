// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Platform/Microsoft/MicrosoftThreading.h
//! @note This file should be never directly included, please consider using <GoddamnEngine/Platform/Threading.h> instead.
//! File contains Microsoft Threading implementation.
#pragma once
#if !defined(GD_INSIDE_THREADING_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Platform/Threading.h> instead.
#endif	// if !defined(GD_INSIDE_INCLUDE_H)

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Platform/Platform.h>
#include <GoddamnEngine/Core/Platform/Threading.h>

#if !(GD_PLATFORM_API_MICROSOFT && GD_PLATFORM_HAS_MULTITHREADING)
#	error Current implementation of Threading is not supported on this platform.
#endif	// if !(GD_PLATFORM_API_MICROSOFT && GD_PLATFORM_HAS_MULTITHREADING)

#include <Windows.h>

GD_NAMESPACE_BEGIN

	// ------------------------------------------------------------------------------------------
	//! Microsoft Critical Section class.
	class MicrosoftCriticalSection final : public IGenericCriticalSection
	{
	private:
		::CRITICAL_SECTION CriticalSection;

	public:

		// ------------------------------------------------------------------------------------------
		//! Initializes a Critical Section.
		GDINL MicrosoftCriticalSection()
		{
			InitializeCriticalSection(&this->CriticalSection);
			SetCriticalSectionSpinCount(&this->CriticalSection, 4000);
		}

		// ------------------------------------------------------------------------------------------
		//! Deinitializes a Critical Section.
		GDINL ~MicrosoftCriticalSection()
		{
			DeleteCriticalSection(&this->CriticalSection);
		}

	public:

		// ------------------------------------------------------------------------------------------
		//! Locks the Critical Section.
		GDINL void Enter() override final
		{
			if (TryEnterCriticalSection(&this->CriticalSection)) 
				EnterCriticalSection(&this->CriticalSection);
		}

		// ------------------------------------------------------------------------------------------
		//! Unlocks the Critical Section.
		GDINL void Leave() override final
		{
			LeaveCriticalSection(&this->CriticalSection);
		}
	};	// class GenericCriticalSection

	// ------------------------------------------------------------------------------------------
	//! Microsoft signal/event class.
	class MicrosoftThreadSignal : public IGenericThreadSignal
	{
	private:
		HANDLE ThreadSignal;

	public:

		// ------------------------------------------------------------------------------------------
		//! Initializes the current signal.
		//! @param RequiresManualReset Does the signal requires to be manually reseted.
		GDINL MicrosoftThreadSignal(bool const RequiresManualReset = false)
			: IGenericThreadSignal(RequiresManualReset)
		{
			this->ThreadSignal = CreateEventA(nullptr, RequiresManualReset ? TRUE : FALSE, FALSE, nullptr);
			GD_ASSERT(this->ThreadSignal != nullptr, "Failed to initialize a thread signal");
		}
	
		// ------------------------------------------------------------------------------------------
		//! Deinitializes the current signal.
		GDINL virtual ~MicrosoftThreadSignal()
		{
			CloseHandle(this->ThreadSignal);
			this->ThreadSignal = nullptr;
		}

	public:

		// ------------------------------------------------------------------------------------------
		//! Raises the current signal.
		GDAPI virtual void RaiseSignal() override final
		{
			SetEvent(this->ThreadSignal);
		}

		// ------------------------------------------------------------------------------------------
		//! Clears the current signal.
		GDAPI virtual void ClearSignal() override final
		{
			ResetEvent(this->ThreadSignal);
		}

		// ------------------------------------------------------------------------------------------
		//! Waits for signal to be raised with specified timeout.
		//! @param TimeoutInMilliseconds Specified timeout in milliseconds. Can be specified as infinite.
		//! @returns @c true, if signal was raised in the timeout period, @c false if timeout was reached.
		GDAPI virtual bool WaitForSignal(TimeoutTp const TimeoutInMilliseconds) const override final
		{
			DWORD const Result = WaitForSingleObject(this->ThreadSignal, TimeoutInMilliseconds != TimeoutInfinite ? TimeoutInMilliseconds : INFINITE);
			GD_ASSERT(Result == WAIT_OBJECT_0 || ((TimeoutInMilliseconds != TimeoutInfinite) && (Result == WAIT_TIMEOUT)), "'WaitForSingleObject' failed.");
			return Result == WAIT_OBJECT_0;
		}
	};	// class MicrosoftThreadSignal

GD_NAMESPACE_END
