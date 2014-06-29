//////////////////////////////////////////////////////////////////////////
/// Atomic.Posix.hh - AtomicsIntrinsics basics implementations for Posix.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 29.04.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#include <GoddamnEngine/Include.hh>

#pragma once 
#ifndef GD_CORE_THREADING_ATOMIC_POSIX
#define GD_CORE_THREADING_ATOMIC_POSIX

#if (!defined(GD_CORE_THREADING_ATOMIC))
#	error 'Please, do not include 'Atomic.Cocoa.inl' directly, use 'Atomic.hh' instead.'
#endif	// if (!defined(GD_CORE_THREADING_ATOMIC))

GD_NAMESPACE_BEGIN
	namespace AtomicsIntrinsics
	{
		GDINL static Int32 InterlockedIncrement(Int32 volatile* const Value)
		{
			return __sync_fetch_and_add(Value, 1) + 1;
		}

		GDINL static Int64 InterlockedIncrement(Int64 volatile* const Value)
		{
			return __sync_fetch_and_add(Value, 1) + 1;
		}

		GDINL static Int32 InterlockedDecrement(Int32 volatile* const Value)
		{
			return __sync_fetch_and_sub(Value, 1) - 1;
		}

		GDINL static Int64 InterlockedDecrement(Int64 volatile* const Value)
		{
			return __sync_fetch_and_sub(Value, 1) - 1;
		}

		GDINL static Int32 InterlockedAdd(Int32 volatile* const Value, Int32 const Amount)
		{
			return __sync_fetch_and_add(Value, Amount);
		}

		GDINL static Int64 InterlockedAdd(Int64 volatile* const Value, Int64 const Amount)
		{
			return __sync_fetch_and_add(Value, Amount);
		}

		GDINL static Int32 InterlockedExchange(Int32 volatile* const Value, Int32 const Other)
		{
			return __sync_lock_test_and_set(Value, Other);
		}

		GDINL static Int64 InterlockedExchange(Int64 volatile* const Value, Int64 const Other)
		{
			return __sync_lock_test_and_set(Value, Other);
		}

		GDINL static handle InterlockedExchangePtr(handle* const Destination, handle const Other)
		{
			return __sync_lock_test_and_set(Destination, Other);
		}

		GDINL static Int32 InterlockedCompareExchange(Int32 volatile* const Destination, Int32 const Other, Int32 const Comperand)
		{
			return __sync_val_compare_and_swap(Destination, Comperand, Other);
		}

		GDINL static Int64 InterlockedCompareExchange(Int64 volatile* const Destination, Int64 const Other, Int64 const Comperand)
		{
			return __sync_val_compare_and_swap(Destination, Comperand, Other);
		}

		GDINL static handle InterlockedCompareExchangePtr(handle* const Destination, handle const Other, handle const Comperand)
		{
			return __sync_val_compare_and_swap(Destination, Comperand, Other);
		}
	}	// namespace AtomicsIntrinsics
GD_NAMESPACE_END

#endif
