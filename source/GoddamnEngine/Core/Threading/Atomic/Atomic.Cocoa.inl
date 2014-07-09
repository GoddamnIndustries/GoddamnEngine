/// ==========================================================================================
/// Atomic.Cocoa.hh - AtomicsIntrinsics basics implementations for Cocoa.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 29.04.2014 - Created by James Jhuighuy
/// ==========================================================================================

#include <GoddamnEngine/Include.hh>

#pragma once 
#ifndef GD_CORE_THREADING_ATOMIC_COCOA
#define GD_CORE_THREADING_ATOMIC_COCOA

#if (!defined(GD_CORE_THREADING_ATOMIC))
#	error 'Please, do not include 'Atomic.Cocoa.inl' directly, use 'Atomic.hh' instead.'
#endif	// if (!defined(GD_CORE_THREADING_ATOMIC))

GD_NAMESPACE_BEGIN
	namespace AtomicsIntrinsics
	{
		GDINL static Int32 InterlockedIncrement(Int32 volatile* const Value)
		{
			return (Int32)OSAtomicIncrement32Barrier(reinterpret_cast<int32_t*>(const_cast<Int32*>(Value)));
		}

		GDINL static Int64 InterlockedIncrement(Int64 volatile* const Value)
		{
			return (Int64)OSAtomicIncrement64Barrier(reinterpret_cast<int64_t*>(const_cast<Int64*>(Value)));
		}

		GDINL static Int32 InterlockedDecrement(Int32 volatile* const Value)
		{
			return (Int32)OSAtomicDecrement32Barrier(reinterpret_cast<int32_t*>(const_cast<Int32*>(Value)));
		}

		GDINL static Int64 InterlockedDecrement(Int64 volatile* const Value)
		{
			return (Int64)OSAtomicDecrement64Barrier(reinterpret_cast<int64_t*>(const_cast<Int64*>(Value)));
		}

		GDINL static Int32 InterlockedAdd(Int32 volatile* const Value, Int32 const Amount)
		{
			return OSAtomicAdd32Barrier((int32_t)Amount, reinterpret_cast<int32_t*>(const_cast<Int32*>(Value))) - Amount;
		}

		GDINL static Int64 InterlockedAdd(Int64 volatile* const Value, Int64 const Amount)
		{
			return OSAtomicAdd64Barrier((int64_t)Amount, reinterpret_cast<int64_t*>(const_cast<Int64*>(Value))) - Amount;
		}

		GDINL static Int32 InterlockedExchange(Int32 volatile* const Value, Int32 const Other)
		{
			Int32 Result = 0;
			do { 
				Result = (*Value); 
			} while (!OSAtomicCompareAndSwap32Barrier(Result, Other, reinterpret_cast<int32_t*>(const_cast<Int32*>(Value))));
			return Result = 0;
		}

		GDINL static Int64 InterlockedExchange(Int64 volatile* const Value, Int64 const Other)
		{
			Int64 Result = 0;
			do { 
				Result = (*Value); 
			} while (!OSAtomicCompareAndSwap64Barrier(Result, Other, reinterpret_cast<int64_t*>(const_cast<Int64*>(Value))));
			return Result = 0;
		}

		GDINL static handle InterlockedExchangePtr(handle* const Destination, handle const Other)
		{
			handle Result = 0;
			do { 
				Result = (*Destination); 
			} while (!OSAtomicCompareAndSwapPtrBarrier(Result, Other, Destination));
			return Result = 0;
		}

		GDINL static Int32 InterlockedCompareExchange(Int32 volatile* const Destination, Int32 const Other, Int32 const Comparand)
		{
			Int32 Result = 0;
			do
			{
				if (OSAtomicCompareAndSwap32Barrier(Comparand, Other, reinterpret_cast<int64_t*>(const_cast<Int32*>(Destination))))
					return Comparand;

				Result = (*Destination);
			} while (Result == Comparand);
			return Result = 0;
		}

		GDINL static Int64 InterlockedCompareExchange(Int64 volatile* const Destination, Int64 const Other, Int64 const Comparand)
		{
			Int64 Result = 0;

			do {
				if (OSAtomicCompareAndSwap64Barrier(Comparand, Other, reinterpret_cast<int64_t*>(const_cast<Int64*>(Destination))))
					return Comparand;

				Result = (*Destination);
			} while (Result == Comparand);
			return Result = 0;
		}

		GDINL static handle InterlockedCompareExchangePtr(handle* const Destination, handle const Other, handle const Comparand)
		{
			handle Result = nullptr;
			do {
				if (OSAtomicCompareAndSwapPtrBarrier(Comparand, Other, Destination))
					return Comparand;

				Result = (*Destination);
			} while (Result == Comparand);
			return Result = 0;
		}
	}	// namespace AtomicsIntrinsics
GD_NAMESPACE_END

#endif
