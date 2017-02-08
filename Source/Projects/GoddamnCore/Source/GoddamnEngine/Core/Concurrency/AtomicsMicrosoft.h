// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Concurrency/AtomicsMicrosoft.h
 * @note This file should be never directly included, please consider using <GoddamnEngine/Core/Concurrency/Atomics.h> instead.
 * File contains Microsoft Atomics implementation.
 */
#pragma once
#if !defined(GD_INSIDE_ATOMICS_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Core/Concurrency/Atomics.h> instead.
#endif	// if !defined(GD_INSIDE_ATOMICS_H)

#include <Windows.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Represents Microsoft-platform implementation for atomic instructions.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class AtomicsMicrosoft : public AtomicsGeneric
	{
	public:
		
		// ------------------------------------------------------------------------------------------
		// Atomic operations.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Atomically increments specified value.
		 *
		 * @param value The value to increment.
		 * @returns Incremented value.
		 */
		//! @{
		GDINL static Int32 InterlockedIncrementInt32(Int32 volatile* const value)
		{
			return static_cast<Int32>(InterlockedIncrement(reinterpret_cast<LONG volatile*>(value)));
		}
		GDINL static Int64 InterlockedIncrementInt64(Int64 volatile* const value)
		{
			return static_cast<Int64>(InterlockedIncrement64(reinterpret_cast<LONG64 volatile*>(value)));
		}
		//! @}

		/*!
		 * Atomically decrements specified value.
		 *
		 * @param value The value to decrement.
		 * @returns Decremented value.
		 */
		//! @{
		GDINL static Int32 InterlockedDecrementInt32(Int32 volatile* const value)
		{
			return static_cast<Int32>(InterlockedDecrement(reinterpret_cast<LONG volatile*>(value)));
		}
		GDINL static Int64 InterlockedDecrementInt64(Int64 volatile* const value)
		{
			return static_cast<Int64>(InterlockedDecrement64(reinterpret_cast<LONG64 volatile*>(value)));
		}
		//! @}

		/*!
		 * Atomically add specified amount to specified value.
		 *
		 * @param value The value, to which amount would be added.
		 * @param amount The amount to add.
		 *
		 * @returns Original value.
		 */
		//! @{
		GDINL static Int32 InterlockedAddInt32(Int32 volatile* const value, Int32 const amount)
		{
			return static_cast<Int32>(InterlockedAdd(reinterpret_cast<LONG volatile*>(value), static_cast<LONG>(amount)));
		}
		GDINL static Int64 InterlockedAddInt64(Int64 volatile* const value, Int64 const amount)
		{
			return static_cast<Int64>(InterlockedAdd64(reinterpret_cast<LONG64 volatile*>(value), static_cast<LONG64>(amount)));
		}
		//! @}

		/*!
		 * Atomically exchanges specified value to other one.
		 *
		 * @param value The value on which exchange is performed
		 * @param exchange Value which is exchanged.
		 *
		 * @returns Original value.
		 */
		//! @{
		GDINL static Int32 InterlockedExchangeInt32(Int32 volatile* const value, Int32 const exchange)
		{
			return static_cast<Int32>(InterlockedExchange(reinterpret_cast<LONG volatile*>(value), static_cast<LONG>(exchange)));
		}
		GDINL static Int64 InterlockedExchangeInt64(Int64 volatile* const value, Int64 const exchange)
		{
			return static_cast<Int64>(InterlockedExchange64(reinterpret_cast<LONG64 volatile*>(value), static_cast<LONG64>(exchange)));
		}
		GDINL static CHandle InterlockedExchangePtr(CHandle volatile* const value, CHandle const exchange)
		{
			return InterlockedExchangePointer(static_cast<PVOID volatile*>(const_cast<Handle volatile*>(value)), const_cast<Handle>(exchange));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// Atomic CAS operations.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Atomically exchanges specified value to other one if original value is equal to comparand.
		 *
		 * @param value The value on which exchange is performed
		 * @param exchange The value which is exchanged.
		 * @param comparand The comparand with which original value is compared.
		 *
		 * @returns Original value.
		 */
		//! @{
		GDINL static Int32 InterlockedCompareExchangeInt32(Int32 volatile* const value, Int32 const exchange, Int32 const comparand)
		{
			return static_cast<Int32>(InterlockedCompareExchange(reinterpret_cast<LONG volatile*>(value), static_cast<LONG>(exchange), static_cast<LONG>(comparand)));
		}
		GDINL static Int64 InterlockedCompareExchangeInt64(Int64 volatile* const value, Int64 const exchange, Int64 const comparand)
		{
			return static_cast<Int64>(InterlockedCompareExchange64(reinterpret_cast<LONG64 volatile*>(value), static_cast<LONG64>(exchange), static_cast<LONG64>(comparand)));
		}
		GDINL static CHandle InterlockedCompareExchangePtr(CHandle volatile* const value, CHandle const exchange, CHandle const comparand)
		{
			return InterlockedCompareExchangePointer(static_cast<PVOID volatile*>(const_cast<Handle volatile*>(value)), const_cast<Handle>(exchange), const_cast<Handle>(comparand));
		}
		//! @}
	};	// class AtomicsMicrosoft

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Represents cross-platform implementation for atomic instructions.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	using Atomics = AtomicsMicrosoft;

GD_NAMESPACE_END
