//////////////////////////////////////////////////////////////////////////
/// Atomic.h - Atomic variables interface and implementation.
/// Copyright (C) $(GD_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 29.04.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#pragma once 
#ifndef GD_CORE_THREADING_ATOMIC
#define GD_CORE_THREADING_ATOMIC

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/Diagnostics/Assertion/Assertion.hh>
#if (defined(GD_PLATFORM_WINDOWS) || defined(GD_PLATFORM_XBOX_ONE) || defined(GD_PLATFORM_WINPNONE8))
#	define GD_THREAD_WINAPI		1
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#else	// if (defined(GD_PLATFORM_WINDOWS) || defined(GD_PLATFORM_XBOX_ONE) || defined(GD_PLATFORM_WINPNONE8))
#	define GD_THREAD_PTHREAD	1
#	include <pthread.h>
#endif	// if (defined(GD_PLATFORM_WINDOWS) || defined(GD_PLATFORM_XBOX_ONE) || defined(GD_PLATFORM_WINPNONE8))

GD_NAMESPACE_BEGIN

	/// Defines a 64 bit signed counter that can be safely used across threads.
	template<class ElementType>
	class Atomic64 final
	{
	private:
		static_assert((sizeof(ElementType) <= sizeof(Int64)), "Atomic class allows storing only 64-bit sized (or less types)");
		union 
		{
			ElementType volatile Wrapped;
			Int64       volatile Value = 0;
		} volatile;

	public:
		GDINL Atomic64(Int64  const  Value) : Value(Value) { }
		GDINL Atomic64(Atomic64 const& OtherAtomic) : Value(OtherAtomic.Value) { }

#if (defined(GD_DOCUMENTATION))
		/// Increments value of counter and returns it to caller.
		GDINL Int64 Increment();
		/// Decrements value of counter and returns it to caller.
		GDINL Int64 Decrement();
		/// Changes counter value to specified one returns old value to caller.
		GDINL Int64 Exchange(Int64 const NewValue);
#else	// if (defined(GD_DOCUMENTATION))
#	if (defined(GD_PLATFORM_WINDOWS) || defined(GD_PLATFORM_XBOX_ONE) || defined(GD_PLATFORM_WINPNONE8))
//  if (defined(GD_PLATFORM_OS_X) || defined(GD_PLATFORM_IOS))
		GDINL Int64 Increment() { return Int64(::InterlockedIncrement64(static_cast<LONGLONG volatile*>(&self->Value))); }
		GDINL Int64 Decrement() { return Int64(::InterlockedDecrement64(static_cast<LONGLONG volatile*>(&self->Value))); }
		GDINL Int64 Exchange(Int64 const NewValue) { return Int64(::InterlockedExchange64(static_cast<LONGLONG volatile*>(&self->Value), LONGLONG(NewValue))); }
#	elif (defined(GD_PLATFORM_OS_X) || defined(GD_PLATFORM_IOS))
//	  if (defined(GD_PLATFORM_WINDOWS) || defined(GD_PLATFORM_XBOX_ONE) || defined(GD_PLATFORM_WINPNONE8))
		GDINL Int64 Increment() { return Int64(::OSAtomicIncrement64Barrier(static_cast<int64_t volatile*>(&self->Value))); }
		GDINL Int64 Decrement() { return Int64(::OSAtomicDecrement64Barrier(static_cast<int64_t volatile*>(&self->Value))); }
		GDINL Int64 Exchange(Int64 const NewValue) 
		{
			Int64 ResultValue;
			do  { ResultValue = *self->Value; } while (!OSAtomicCompareAndSwap64Barrier(RetVal, NewValue, static_cast<int64_t*>(&self->Value)));
			return ResultValue;
		}
#	else
#		error Atomic64 class is not implemented for this platform.
#	endif	//   if (defined(GD_PLATFORM_WINDOWS) || defined(GD_PLATFORM_XBOX_ONE) || defined(GD_PLATFORM_WINPNONE8))
			// elif (defined(GD_PLATFORM_OS_X) || defined(GD_PLATFORM_IOS))
#endif	// if (defined(GD_DOCUMENTATION))
	};	// class Atomic64

GD_NAMESPACE_END

#endif	// ifndef GD_CORE_THREADING_ATOMIC
