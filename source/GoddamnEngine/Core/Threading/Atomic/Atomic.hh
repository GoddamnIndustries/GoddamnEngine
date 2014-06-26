//////////////////////////////////////////////////////////////////////////
/// Atomic.hh - Atomics variables interface and implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 29.04.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#pragma once 
#ifndef GD_CORE_THREADING_ATOMIC
#define GD_CORE_THREADING_ATOMIC

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/TypeTraits.hh>
#include <GoddamnEngine/Core/Diagnostics/Assertion/Assertion.hh>

#if (defined(GD_DOCUMENTATION))
GD_NAMESPACE_BEGIN

	/// Includes stuff to provide atomic operations.
	namespace Atomics
	{
		/// Atomically increments pointer value.
		/// @param   Value Pointer to value that would be incremented.
		/// @returns Incremented value.
		GDAPI static Int32 InterlockedIncrement(Int32 volatile* const Value);
		GDAPI static Int64 InterlockedIncrement(Int64 volatile* const Value);

		/// Atomically decrements pointer value.
		/// @param   Value Pointer to value that would be decremented.
		/// @returns Decremented value.
		GDAPI static Int32 InterlockedDecrement(Int32 volatile* const Value);
		GDAPI static Int64 InterlockedDecrement(Int64 volatile* const Value);

		/// Atomically add specified amount to pointer value.
		/// @param   Value  Pointer to value that would be adjusted.
		/// @param   Amount Amount that would be added to pointer value.
		/// @returns Adjusted value.
		GDAPI static Int32 InterlockedAdd(Int32 volatile* const Value, Int32 const Amount);
		GDAPI static Int64 InterlockedAdd(Int64 volatile* const Value, Int64 const Amount);

		/// Atomically swaps pointer value to new specified one and returns original pointer value.
		/// @param   Value Pointer to value that would be swapped.
		/// @param   Other New pointer value.
		/// @returns Original value of pointer before it was swapped.
		GDAPI static Int32 InterlockedExchange(Int32 volatile* const Value, Int32 const Other);
		GDAPI static Int64 InterlockedExchange(Int64 volatile* const Value, Int64 const Other);

		/// Atomically swaps pointer value to new specified one and returns original pointer value.
		/// @param   Value Pointer to value that would be swapped.
		/// @param   Other New pointer value.
		/// @returns Original value of pointer before it was swapped.
		GDAPI static handle InterlockedExchangePtr(handle* const Destination, handle const Other);

		/// Atomically compares value to comparand and swaps to new specified one if they are equal and returns the original value.
		/// @param   Value     Pointer to value that would be swapped.
		/// @param   Comparand Value to which original pointer value would be compared with.
		/// @param   Other     New pointer value.
		/// @returns Original value of pointer before it was swapped.
		GDAPI static Int32 InterlockedCompareExchange(Int32 volatile* const Destination, Int32 const Other, Int32 const Comparand);
		GDAPI static Int64 InterlockedCompareExchange(Int64 volatile* const Destination, Int64 const Other, Int64 const Comparand);

		/// Atomically compares value to comparand and swaps to new specified one if they are equal and returns the original value.
		/// @param   Value     Pointer to value that would be swapped.
		/// @param   Comparand Value to which original pointer value would be compared with.
		/// @param   Other     New pointer value.
		/// @returns Original value of pointer before it was swapped.
		GDINL static handle InterlockedCompareExchangePtr(handle* const Destination, handle const Other, handle const Comparand);
	}	// namespace Atomics

GD_NAMESPACE_END
#else	// if (defined(GD_DOCUMENTATION))
#	if (defined(GD_PLATFORM_WINDOWS) || defined(GD_PLATFORM_XBOX_ONE) || defined(GD_PLATFORM_WINPNONE8))
#		include <GoddamnEngine/Core/Threading/Atomic/Atomic.WinApi.hh>
//	endif	// if (defined(GD_PLATFORM_WINDOWS) || defined(GD_PLATFORM_XBOX_ONE) || defined(GD_PLATFORM_WINPNONE8))
#	elif (defined(GD_PLATFORM_LINUX) || defined(GD_PLATFORM_PS4) || defined(GD_PLATFORM_ANDROID))
#		include <GoddamnEngine/Core/Threading/Atomic/Atomic.Posix.hh>
//	endif	// elif (defined(GD_PLATFORM_LINUX) || defined(GD_PLATFORM_ANDROID) || defined(GD_PLATFORM_PS4))
#	elif (defined(GD_PLATFORM_OS_X) || defined(GD_PLATFORM_IOS))
#		include <GoddamnEngine/Core/Threading/Atomic/Atomic.Cocoa.hh>
//	endif	// if (defined(GD_PLATFORM_OS_X) || defined(GD_PLATFORM_IOS))
#	else
#		error 'Atomic64 class is not implemented for this platform.'
#	endif
#endif	// if (defined(GD_DOCUMENTATION))
#endif	// ifndef GD_CORE_THREADING_ATOMIC
