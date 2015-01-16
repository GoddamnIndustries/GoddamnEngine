/// ==========================================================================================
/// Atomic.h - Atomic variables interface and implementation.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// ==========================================================================================

#pragma once 
#ifndef GD_CORE_THREADING_ATOMIC
#define GD_CORE_THREADING_ATOMIC

#include <GoddamnEngine/Include.h>

GD_NAMESPACE_BEGIN

	/// Includes stuff to provide atomic operations.
	namespace AtomicsIntrinsics
	{
		///	@brief Atomically increments pointer value.
		/// @param Value Pointer to value that would be incremented.
		/// @returns Incremented value.
		GDINL static Int32	InterlockedIncrement(Int32 volatile* const Value);
		GDINL static Int64	InterlockedIncrement(Int64 volatile* const Value);

		///	@brief Atomically decrements pointer value.
		/// @param Value Pointer to value that would be decremented.
		/// @returns Decremented value.
		GDINL static Int32	InterlockedDecrement(Int32 volatile* const Value);
		GDINL static Int64	InterlockedDecrement(Int64 volatile* const Value);

		/// @brief Atomically add specified amount to pointer value.
		/// @param Value Pointer to value that would be adjusted.
		/// @param Amount Amount that would be added to pointer value.
		/// @returns Adjusted value.
		GDINL static Int32	InterlockedAdd(Int32 volatile* const Value, Int32 const Amount);
		GDINL static Int64	InterlockedAdd(Int64 volatile* const Value, Int64 const Amount);

		/// @brief Atomically swaps pointer value to new specified one and returns original pointer value.
		/// @param Value Pointer to value that would be swapped.
		/// @param Other New pointer value.
		/// @returns Original value of pointer before it was swapped.
		GDINL static Int32	InterlockedExchange(Int32 volatile* const Value, Int32 const Other);
		GDINL static Int64	InterlockedExchange(Int64 volatile* const Value, Int64 const Other);

		/// @brief Atomically swaps pointer value to new specified one and returns original pointer value.
		/// @param Value Pointer to value that would be swapped.
		/// @param Other New pointer value.
		/// @returns Original value of pointer before it was swapped.
		GDINL static handle InterlockedExchangePtr(handle* const Destination, handle const Other);

		/// @brief Atomically compares value to comparand and swaps to new specified one if they are equal and returns the original value.
		/// @param Value Pointer to value that would be swapped.
		/// @param Comparand Value to which original pointer value would be compared with.
		/// @param Other New pointer value.
		/// @returns Original value of pointer before it was swapped.
		GDINL static Int32 InterlockedCompareExchange(Int32 volatile* const Destination, Int32 const Other, Int32 const Comparand);
		GDINL static Int64 InterlockedCompareExchange(Int64 volatile* const Destination, Int64 const Other, Int64 const Comparand);

		/// @brief Atomically compares value to comparand and swaps to new specified one if they are equal and returns the original value.
		/// @param Value Pointer to value that would be swapped.
		/// @param Comparand Value to which original pointer value would be compared with.
		/// @param Other New pointer value.
		/// @returns Original value of pointer before it was swapped.
		GDINL static handle InterlockedCompareExchangePtr(handle* const Destination, handle const Other, handle const Comparand);
	}	// namespace AtomicsIntrinsics

GD_NAMESPACE_END
#if (!defined(GD_DOCUMENTATION))
#	if (defined(GD_PLATFORM_WINDOWS) || defined(GD_PLATFORM_XBOX_ONE) || defined(GD_PLATFORM_WINPNONE8))
#		include <GoddamnEngine/Core/Threading/Atomic/Atomic.WinApi.inl>
//	endif	// if (defined(GD_PLATFORM_WINDOWS) || defined(GD_PLATFORM_XBOX_ONE) || defined(GD_PLATFORM_WINPNONE8))
#	elif (defined(GD_PLATFORM_LINUX) || defined(GD_PLATFORM_PS4) || defined(GD_PLATFORM_ANDROID))
#		include <GoddamnEngine/Core/Threading/Atomic/Atomic.Posix.inl>
//	endif	// elif (defined(GD_PLATFORM_LINUX) || defined(GD_PLATFORM_ANDROID) || defined(GD_PLATFORM_PS4))
#	elif (defined(GD_PLATFORM_OS_X) || defined(GD_PLATFORM_IOS))
#		include <GoddamnEngine/Core/Threading/Atomic/Atomic.Cocoa.inl>
//	endif	// if (defined(GD_PLATFORM_OS_X) || defined(GD_PLATFORM_IOS))
#	else
//#		error 'Atomic64 class is not implemented for this platform.'
#	endif
#endif	// if (!defined(GD_DOCUMENTATION))

#endif	// ifndef GD_CORE_THREADING_ATOMIC
