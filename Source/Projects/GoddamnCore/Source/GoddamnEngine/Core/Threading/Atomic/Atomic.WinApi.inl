/// ==========================================================================================
/// Atomic.WinApi.h - AtomicsIntrinsics basics implementations for WinApi.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// 
/// History:
///		* 29.04.2014 - Created by James Jhuighuy
/// ==========================================================================================

#include <GoddamnEngine/Include.h>
#include <Windows.h>

#pragma once 
#ifndef GD_CORE_THREADING_ATOMIC_WINAPI
#define GD_CORE_THREADING_ATOMIC_WINAPI

#if (!defined(GD_CORE_THREADING_ATOMIC))
#	error 'Please, do not include 'Atomic.WinApi.inl' directly, use 'Atomic.h' instead.'
#endif	// if (!defined(GD_CORE_THREADING_ATOMIC))

GD_NAMESPACE_BEGIN
	namespace AtomicsIntrinsics
	{
		GDINL static Int32 InterlockedIncrement(Int32 volatile* const Value)
		{
			return static_cast<Int32>(::InterlockedIncrement(reinterpret_cast<LPLONG>(const_cast<Int32*>(Value))));
		}

		GDINL static Int64 InterlockedIncrement(Int64 volatile* const Value)
		{
			return static_cast<Int64>(::InterlockedIncrement64(reinterpret_cast<LONGLONG*>(const_cast<Int64*>(Value))));
		}

		GDINL static Int32 InterlockedDecrement(Int32 volatile* const Value)
		{
			return static_cast<Int32>(::InterlockedDecrement(reinterpret_cast<LPLONG>(const_cast<Int32*>(Value))));
		}

		GDINL static Int64 InterlockedDecrement(Int64 volatile* const Value)
		{
			return static_cast<Int64>(::InterlockedDecrement64(reinterpret_cast<LONGLONG*>(const_cast<Int64*>(Value))));
		}

		GDINL static Int32 InterlockedAdd(Int32 volatile* const Value, Int32 const Amount)
		{
			return static_cast<Int32>(::InterlockedExchangeAdd(reinterpret_cast<LPLONG>(const_cast<Int32*>(Value)), static_cast<LONG>(Amount)));
		}

		GDINL static Int64 InterlockedAdd(Int64 volatile* const Value, Int64 const Amount)
		{
			return static_cast<Int64>(::InterlockedExchangeAdd64(reinterpret_cast<LONGLONG*>(const_cast<Int64*>(Value)), static_cast<LONGLONG>(Amount)));
		}

		GDINL static Int32 InterlockedExchange(Int32 volatile* const Value, Int32 const Other)
		{
			return static_cast<Int32>(::InterlockedExchange(reinterpret_cast<LPLONG>(const_cast<Int32*>(Value)), static_cast<LONG>(Other)));
		}

		GDINL static Int64 InterlockedExchange(Int64 volatile* const Value, Int64 const Other)
		{
			return static_cast<Int64>(::InterlockedExchange64(reinterpret_cast<LONGLONG*>(const_cast<Int64*>(Value)), static_cast<LONGLONG>(Other)));
		}

		GDINL static handle InterlockedExchangePtr(handle* const Destination, handle const Other)
		{
			return ::InterlockedExchangePointer(Destination, Other);
		}

		GDINL static Int32 InterlockedCompareExchange(Int32 volatile* const Destination, Int32 const Other, Int32 const Comparand)
		{
			return static_cast<Int32>(::InterlockedCompareExchange(reinterpret_cast<LPLONG>(const_cast<Int32*>(Destination)), static_cast<LONG>(Other), static_cast<LONG>(Comparand)));
		}

		GDINL static Int64 InterlockedCompareExchange(Int64 volatile* const Destination, Int64 const Other, Int64 const Comparand)
		{
			return static_cast<Int64>(::InterlockedCompareExchange64(reinterpret_cast<LONGLONG*>(const_cast<Int64*>(Destination)), static_cast<LONGLONG>(Other), static_cast<LONGLONG>(Comparand)));
		}

		GDINL static handle InterlockedCompareExchangePtr(handle* const Destination, handle const Other, handle const Comparand)
		{
			return ::InterlockedCompareExchangePointer(Destination, Other, Comparand);
		}
	}	// namespace AtomicsIntrinsics
GD_NAMESPACE_END

#endif
