// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Platform/Microsoft/MicrosoftAtomics.h
//! @note This file should be never directly included, please consider using <GoddamnEngine/Platform/Atomics.h> instead.
//! File contains Microsoft Atomics implementation.
#pragma once
#if !defined(GD_INSIDE_ATOMICS_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Platform/Atomics.h> instead.
#endif	// if !defined(GD_INSIDE_ATOMICS_H)

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Platform/Platform.h>
#include <GoddamnEngine/Core/Platform/Atomics.h>

#if !(GD_PLATFORM_API_MICROSOFT && GD_PLATFORM_HAS_MULTITHREADING)
#	error Current implementation of Atomic instructions is not supported on this platform.
#endif	// if !(GD_PLATFORM_API_MICROSOFT && GD_PLATFORM_HAS_MULTITHREADING)

#include <Windows.h>
#if (defined(InterlockedIncrement32))
#	undef InterlockedIncrement32
#endif	// if (defined(InterlockedIncrement32))
#if (defined(InterlockedIncrement64))
#	undef InterlockedIncrement64
#endif	// if (defined(InterlockedIncrement64))
#if (defined(InterlockedDecrement32))
#	undef InterlockedDecrement32
#endif	// if (defined(InterlockedDecrement32))
#if (defined(InterlockedDecrement64))
#	undef InterlockedDecrement64
#endif	// if (defined(InterlockedDecrement64))
#if (defined(InterlockedAdd32))
#	undef InterlockedAdd32
#endif	// if (defined(InterlockedAdd32))
#if (defined(InterlockedAdd64))
#	undef InterlockedAdd64
#endif	// if (defined(InterlockedAdd64))
#if (defined(InterlockedExchange32))
#	undef InterlockedExchange32
#endif	// if (defined(InterlockedExchange32))
#if (defined(InterlockedExchange64))
#	undef InterlockedExchange64
#endif	// if (defined(InterlockedExchange64))
#if (defined(InterlockedExchangePtr))
#	undef InterlockedExchangePtr
#endif	// if (defined(InterlockedExchangePtr))
#if (defined(InterlockedCompareExchange32))
#	undef InterlockedCompareExchange32
#endif	// if (defined(InterlockedCompareExchange32))
#if (defined(InterlockedCompareExchange64))
#	undef InterlockedCompareExchange64
#endif	// if (defined(InterlockedCompareExchange64))
#if (defined(InterlockedCompareExchangePtr))
#	undef InterlockedCompareExchangePtr
#endif	// if (defined(InterlockedCompareExchangePtr))

GD_NAMESPACE_BEGIN

	//! Represents Windows definitions for atomic instructions.
	class MicrosoftAtomics final : public GenericAtomics
	{
	public:
		//! Atomically increments specified value.
		//! @param Value to increment.
		//! @returns Incremented value.
		//! @{
		GDINL static Int32 InterlockedIncrement32(Int32 volatile* const Value)
		{
			return static_cast<Int32>(::_InterlockedIncrement(reinterpret_cast<LONG volatile*>(Value)));
		}
#if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64
		GDINL static Int64 InterlockedIncrement64(Int64 volatile* const Value)
		{
			return static_cast<Int64>(::_InterlockedIncrement64(reinterpret_cast<LONG64 volatile*>(Value)));
		}
#endif	// if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64
		//! @}

		//! Atomically decrements specified value.
		//! @param Value to decrement.
		//! @returns Decremented value.
		//! @{
		GDINL static Int32 InterlockedDecrement32(Int32 volatile* const Value)
		{
			return static_cast<Int32>(::_InterlockedDecrement(reinterpret_cast<LONG volatile*>(Value)));
		}
#if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64
		GDINL static Int64 InterlockedDecrement64(Int64 volatile* const Value)
		{
			return static_cast<Int64>(::_InterlockedDecrement64(reinterpret_cast<LONG64 volatile*>(Value)));
		}
#endif	// if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64
		//! @}

		//! Atomically add specified amount to specified value.
		//! @param Value Value, to which amount would be added.
		//! @param Amount Amount to add.
		//! @returns Original value.
		//! @{
		GDINL static Int32 InterlockedAdd32(Int32 volatile* const Value, Int32 const Amount)
		{
			return static_cast<Int32>(::_InterlockedAdd(reinterpret_cast<LONG volatile*>(Value), static_cast<LONG>(Amount)));
		}
#if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64
		GDINL static Int64 InterlockedAdd64(Int64 volatile* const Value, Int64 const Amount)
		{
			return static_cast<Int64>(::_InterlockedAdd64(reinterpret_cast<LONG64 volatile*>(Value), static_cast<LONG64>(Amount)));
		}
#endif	// if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64
		//! @}

		//! Atomically exchanges specified value to other one.
		//! @param Value Value on which exchange is performed
		//! @param Exchange Value which is exchanged.
		//! @returns Original value.
		//! @{
		GDINL static Int32 InterlockedExchange32(Int32 volatile* const Value, Int32 const Exchange)
		{
			return static_cast<Int32>(::_InterlockedExchange(reinterpret_cast<LONG volatile*>(Value), static_cast<LONG>(Exchange)));
		}
#if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64
		GDINL static Int64 InterlockedExchange64(Int64 volatile* const Value, Int64 const Exchange)
		{
			return static_cast<Int64>(::_InterlockedExchange64(reinterpret_cast<LONG64 volatile*>(Value), static_cast<LONG64>(Exchange)));
		}
#endif	// if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64
		GDINL static CHandle InterlockedExchangePtr(CHandle volatile* const Value, CHandle const Exchange)
		{
			return ::_InterlockedExchangePointer(static_cast<PVOID volatile*>(const_cast<Handle volatile*>(Value)), const_cast<Handle>(Exchange));
		}
		//! @}

		//! Atomically exchanges specified value to other one if original value is equal to comparand.
		//! @param Value Value on which exchange is performed
		//! @param Exchange Value which is exchanged.
		//! @param Comparand Comparand with which original value is compared.
		//! @returns Original value.
		//! @{
		GDINL static Int32 InterlockedCompareExchange32(Int32 volatile* const Value, Int32 const Exchange, Int32 const Comparand)
		{
			return static_cast<Int32>(::_InterlockedCompareExchange(reinterpret_cast<LONG volatile*>(Value), static_cast<LONG>(Exchange), static_cast<LONG>(Comparand)));
		}
#if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64
		GDINL static Int64 InterlockedCompareExchange64(Int64 volatile* const Value, Int64 const Exchange, Int64 const Comparand)
		{
			return static_cast<Int64>(::_InterlockedCompareExchange64(reinterpret_cast<LONG64 volatile*>(Value), static_cast<LONG64>(Exchange), static_cast<LONG64>(Comparand)));
		}
#endif	// if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64
		GDINL static CHandle InterlockedCompareExchangePtr(CHandle volatile* const Value, CHandle const Exchange, CHandle const Comparand)
		{
			return ::_InterlockedCompareExchangePointer(static_cast<PVOID volatile*>(const_cast<Handle volatile*>(Value)), const_cast<Handle>(Exchange), const_cast<Handle>(Comparand));
		}
		//! @}
	};	// class MicrosoftAtomics

GD_NAMESPACE_END
