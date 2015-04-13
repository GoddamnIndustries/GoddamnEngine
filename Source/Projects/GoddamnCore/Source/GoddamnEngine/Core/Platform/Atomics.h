// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Platform/Atomics.h
//! File contains cross-platform Atomics implementation.
#pragma once
#define GD_INSIDE_ATOMICS_H

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Platform/Platform.h>

GD_NAMESPACE_BEGIN

	//! @brief Represents generic definitions for atomic instructions.
	class GenericAtomics
	{
	public:
		//! @brief Checks if a pointer is aligned and can be used with atomic functions.
		//! @param Pointer Pointer to check.
		//! @param Alignment Alignment value.
		//! @returns True if pointer is aligned.
		GDINL static bool IsPointerAligned(VHandle const Pointer, UInt32 const Alignment = sizeof(Handle))
		{
			return (reinterpret_cast<IntPtr>(Pointer) & (Alignment - 1)) == 0;
		}

		//! @brief Atomically increments specified value.
		//! @param Value to increment.
		//! @returns Incremented value.
		//! @{
		GDINL static Int32 InterlockedIncrement32(Int32 volatile* const Value)
		{
			return (++*Value);
		}
#if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64
		GDINL static Int64 InterlockedIncrement64(Int64 volatile* const Value)
		{
			return (++*Value);
		}
#endif	// if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64
		//! @}

		//! @brief Atomically decrements specified value.
		//! @param Value to decrement.
		//! @returns Decremented value.
		//! @{
		GDINL static Int32 InterlockedDecrement32(Int32 volatile* const Value)
		{
			return (--*Value);
		}
#if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64
		GDINL static Int64 InterlockedDecrement64(Int64 volatile* const Value)
		{
			return (--*Value);
		}
#endif	// if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64
		//! @}

		//! @brief Atomically add specified amount to specified value.
		//! @param Value Value, to which amount would be added.
		//! @param Amount Amount to add.
		//! @returns Original value.
		//! @{
		GDINL static Int32 InterlockedAdd32(Int32 volatile* const Value, Int32 const Amount)
		{
			Int32 const OldValue = *Value;
			*Value += Amount;
			return OldValue;
		}
#if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64
		GDINL static Int64 InterlockedAdd64(Int64 volatile* const Value, Int64 const Amount)
		{
			Int64 const OldValue = *Value;
			*Value += Amount;
			return OldValue;
		}
#endif	// if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64
		//! @}

		//! @brief Atomically exchanges specified value to other one.
		//! @param Value Value on which exchange is performed
		//! @param Exchange Value which is exchanged.
		//! @returns Original value.
		//! @{
		GDINL static Int32 InterlockedExchange32(Int32 volatile* const Value, Int32 const Exchange)
		{
			Int32 const OldValue = *Value;
			*Value = Exchange;
			return OldValue;
		}
#if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64
		GDINL static Int64 InterlockedExchange64(Int64 volatile* const Value, Int64 const Exchange)
		{
			Int64 const OldValue = *Value;
			*Value = Exchange;
			return OldValue;
		}
#endif	// if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64
		GDINL static CHandle InterlockedExchangePtr(CHandle volatile* const Value, CHandle const Exchange)
		{
			CHandle const OldValue = *Value;
			*Value = Exchange;
			return OldValue;
		}
		//! @}

		//! @brief Atomically exchanges specified value to other one if original value is equal to comparand.
		//! @param Value Value on which exchange is performed
		//! @param Exchange Value which is exchanged.
		//! @param Comparand Comparand with which original value is compared.
		//! @returns Original value.
		//! @{
		GDINL static Int32 InterlockedCompareExchange32(Int32 volatile* const Value, Int32 const Exchange, Int32 const Comparand)
		{
			Int32 const OldValue = *Value;
			if (OldValue == Comparand) 
			{
				*Value = Exchange;
			}
			return OldValue;
		}
#if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64
		GDINL static Int64 InterlockedCompareExchange64(Int64 volatile* const Value, Int64 const Exchange, Int64 const Comparand)
		{
			Int64 const OldValue = *Value;
			if (OldValue == Comparand) 
			{
				*Value = Exchange;
			}
			return OldValue;
		}
#endif	// if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64
		GDINL static CHandle InterlockedCompareExchangePtr(CHandle volatile* const Value, CHandle const Exchange, CHandle const Comparand)
		{
			CHandle const OldValue = *Value;
			if (OldValue == Comparand) 
			{
				*Value = Exchange;
			}
			return OldValue;
		}
		//! @}
	};	// class GenericAtomics

GD_NAMESPACE_END

#if GD_PLATFORM_HAS_MULTITHREADING
#	include GD_PLATFORM_INCLUDE(GoddamnEngine/Core/Platform, Atomics.h)
#else	// if GD_PLATFORM_HAS_MULTITHREADING

GD_NAMESPACE_BEGIN

	//! @brief  Represents definitions for atomic instructions.
	typedef GenericAtomics Atomics;

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_HAS_MULTITHREADING

#undef GD_INSIDE_ATOMICS_H
