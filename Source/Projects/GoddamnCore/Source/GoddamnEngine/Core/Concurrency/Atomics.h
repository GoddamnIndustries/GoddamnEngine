// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Concurrency/Atomics.h
 * File contains cross-platform Atomics implementation.
 */
#pragma once
#define GD_INSIDE_ATOMICS_H

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/TypeTraits.h>
#include <GoddamnEngine/Core/PlatformSpecificInclude.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Represents generic definitions for atomic instructions.
	//! All operations are implemented non-atomically.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class AtomicsGeneric : public TNonCreatable
	{
	public:

		// ------------------------------------------------------------------------------------------
		// Helper methods.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Checks if a pointer is aligned and can be used with atomic functions.
		 *
		 * @param pointer The pointer to check.
		 * @param alignment The alignment value.
		 * @returns True if pointer is aligned.
		 */
		GDINL static bool IsPointerAligned(VHandle const pointer, UInt32 const alignment = sizeof(Handle))
		{
			return (reinterpret_cast<IntPtr>(pointer) & alignment - 1) == 0;
		}

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
			return ++*value;
		}
#if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64
		GDINL static Int64 InterlockedIncrementInt64(Int64 volatile* const value)
		{
			return ++*value;
		}
#endif	// if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64
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
			return --*value;
		}
#if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64
		GDINL static Int64 InterlockedDecrementInt64(Int64 volatile* const value)
		{
			return --*value;
		}
#endif	// if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64
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
			auto const oldValue = *value;
			*value += amount;
			return oldValue;
		}
#if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64
		GDINL static Int64 InterlockedAddInt64(Int64 volatile* const value, Int64 const amount)
		{
			auto const oldValue = *value;
			*value += amount;
			return oldValue;
		}
#endif	// if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64
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
			auto const oldValue = *value;
			*value = exchange;
			return oldValue;
		}
#if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64
		GDINL static Int64 InterlockedExchangeInt64(Int64 volatile* const value, Int64 const exchange)
		{
			auto const oldValue = *value;
			*value = exchange;
			return oldValue;
		}
#endif	// if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64
		GDINL static CHandle InterlockedExchangePtr(CHandle volatile* const value, CHandle const exchange)
		{
			auto const oldValue = *value;
			*value = exchange;
			return oldValue;
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
			auto const oldValue = *value;
			if (oldValue == comparand) 
			{
				*value = exchange;
			}
			return oldValue;
		}
#if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64
		GDINL static Int64 InterlockedCompareExchangeInt64(Int64 volatile* const value, Int64 const exchange, Int64 const comparand)
		{
			auto const oldValue = *value;
			if (oldValue == comparand) 
			{
				*value = exchange;
			}
			return oldValue;
		}
#endif	// if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64
		GDINL static CHandle InterlockedCompareExchangePtr(CHandle volatile* const value, CHandle const exchange, CHandle const comparand)
		{
			auto const oldValue = *value;
			if (oldValue == comparand) 
			{
				*value = exchange;
			}
			return oldValue;
		}
		//! @}
	};	// class AtomicsGeneric

GD_NAMESPACE_END

#include GD_PLATFORM_API_INCLUDE(GoddamnEngine/Core/Concurrency, Atomics)
#undef GD_INSIDE_ATOMICS_H

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! 32-bit integral atomic value.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	template<typename TValue>
	struct AtomicValue32 final : public TNonCopyable
	{
		static_assert(TypeTraits::IsIntegral<TValue>::Value || TypeTraits::IsEnum<TValue>::Value, "Template parameter should be integral value.");
		static_assert(sizeof(TValue) == 4, "Template parameter should be 32-bit integral value.");

	private:
		union {
			TValue volatile m_Value;
			Int32  volatile m_ValueInt;
		};

	public:

		/*!
		 * Initializes an empty atomic value.
		 */
		GDINL constexpr AtomicValue32()
			: m_ValueInt(0)
		{}

		/*!
		 * Initializes an atomic value.
		 * @param value The initial value.
		 */
		GDINL constexpr explicit AtomicValue32(TValue const value)
			: m_ValueInt(value)
		{}

	public:

		/*!
		 * Atomically reads this value.
		 * @returns The value.
		 */
		GDINL TValue Get() volatile const
		{
			return static_cast<TValue>(Atomics::InterlockedAddInt32(const_cast<Int32 volatile*>(&m_ValueInt), 0));
		}

		/*!
		 * Atomically reads this value.
		 * @returns Original value.
		 */
		GDINL TValue Set(TValue const value) volatile
		{
			return static_cast<TValue>(Atomics::InterlockedExchangeInt32(&m_ValueInt, static_cast<Int32>(value)));
		}

		/*!
		 * Atomically exchanges this value to other one if original value is equal to comparand.
		 *
		 * @param exchange The value which is exchanged.
		 * @param comparand The comparand with which original value is compared.
		 *
		 * @returns Original value.
		 */
		GDINL TValue CompareExchange(TValue const exchange, TValue const comparand) volatile
		{
			return static_cast<TValue>(Atomics::InterlockedCompareExchangeInt32(&m_ValueInt, static_cast<Int32>(exchange), static_cast<Int32>(comparand)));
		}

		/*!
		 * Atomically add specified amount to this value.
		 *
		 * @param amount The amount to add.
		 * @returns Original value.
		 */
		GDINL TValue FetchAdd(TValue const amount) volatile
		{
			return static_cast<TValue>(Atomics::InterlockedAddInt32(&m_ValueInt, static_cast<Int32>(amount)));
		}

		/*!
		 * Atomically subtracts specified amount from this value.
		 *
		 * @param amount The amount to subtract.
		 * @returns Original value.
		 */
		GDINL TValue FetchSub(TValue const amount) volatile
		{ 
			return static_cast<TValue>(Atomics::InterlockedAddInt32(&m_ValueInt, -static_cast<Int32>(amount)));
		}

	public:
		GDINL TValue operator=(TValue const value) volatile
		{
			Set(value);
			return value;
		}

		GDINL TValue operator+=(TValue const amount) volatile
		{
			return FetchAdd(amount) + amount;
		}
		GDINL TValue operator-=(TValue const amount) volatile
		{
			return FetchSub(amount) - amount;
		}

		GDINL TValue operator++() volatile
		{
			return static_cast<TValue>(Atomics::InterlockedIncrementInt32(&m_ValueInt));
		}
		GDINL TValue operator--() volatile
		{
			return static_cast<TValue>(Atomics::InterlockedDecrementInt32(&m_ValueInt));
		}

		GDINL TValue operator++(int) volatile
		{
			return static_cast<TValue>(Atomics::InterlockedIncrementInt32(&m_ValueInt)) + 1;
		}
		GDINL TValue operator--(int) volatile
		{
			return static_cast<TValue>(Atomics::InterlockedDecrementInt32(&m_ValueInt)) - 1;
		}

		GDINL implicit operator TValue() const volatile
		{
			return Get();
		}

	};	// struct AtomicValue32<T32>

	typedef AtomicValue32<Int32> AtomicInt32;
	typedef AtomicValue32<UInt32> AtomicUInt32;
	typedef AtomicValue32<UInt32> AtomicBool;

#if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! 64-bit integral atomic value.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	template<typename TValue>
	struct AtomicValue64 final : public TNonCopyable
	{
		static_assert(TypeTraits::IsIntegral<TValue>::Value || TypeTraits::IsEnum<TValue>::Value, "Template parameter should be integral value.");
		static_assert(sizeof(TValue) == 8, "Template parameter should be 64-bit integral value.");

	private:
		union {
			TValue volatile m_Value;
			Int64  volatile m_ValueInt;
		};

	public:

		/*!
		 * Initializes an empty atomic value.
		 */
		GDINL constexpr AtomicValue64()
			: m_ValueInt(0)
		{}

		/*!
		 * Initializes an atomic value.
		 * @param value The initial value.
		 */
		GDINL constexpr explicit AtomicValue64(TValue const value)
			: m_ValueInt(value)
		{}

	public:

		/*!
		 * Atomically reads this value.
		 * @returns The value.
		 */
		GDINL TValue Get() volatile const
		{
			return static_cast<TValue>(Atomics::InterlockedAddInt64(const_cast<Int64 volatile*>(&m_ValueInt), 0));
		}

		/*!
		 * Atomically reads this value.
		 * @returns Original value.
		 */
		GDINL TValue Set(TValue const value) volatile
		{
			return static_cast<TValue>(Atomics::InterlockedExchangeInt64(&m_ValueInt, static_cast<Int64>(value)));
		}

		/*!
		 * Atomically exchanges this value to other one if original value is equal to comparand.
		 *
		 * @param exchange The value which is exchanged.
		 * @param comparand The comparand with which original value is compared.
		 *
		 * @returns Original value.
		 */
		GDINL TValue CompareExchange(TValue const exchange, TValue const comparand) volatile
		{
			return static_cast<TValue>(Atomics::InterlockedCompareExchangeInt64(&m_ValueInt, static_cast<Int64>(exchange), static_cast<Int64>(comparand)));
		}

		/*!
		 * Atomically add specified amount to this value.
		 *
		 * @param amount The amount to add.
		 * @returns Original value.
		 */
		GDINL TValue FetchAdd(TValue const amount) volatile
		{
			return static_cast<TValue>(Atomics::InterlockedAddInt64(&m_ValueInt, static_cast<Int64>(amount)));
		}

		/*!
		 * Atomically subtracts specified amount from this value.
		 *
		 * @param amount The amount to subtract.
		 * @returns Original value.
		 */
		GDINL TValue FetchSub(TValue const amount) volatile
		{ 
			return static_cast<TValue>(Atomics::InterlockedAddInt64(&m_ValueInt, -static_cast<Int64>(amount)));
		}

	public:
		GDINL TValue operator=(TValue const value) volatile
		{
			Set(value);
			return value;
		}

		GDINL TValue operator+=(TValue const amount) volatile
		{
			return FetchAdd(amount) + amount;
		}
		GDINL TValue operator-=(TValue const amount) volatile
		{
			return FetchSub(amount) - amount;
		}

		GDINL TValue operator++() volatile
		{
			return static_cast<TValue>(Atomics::InterlockedIncrementInt64(&m_ValueInt)) + 1;
		}
		GDINL TValue operator--() volatile
		{
			return static_cast<TValue>(Atomics::InterlockedDecrementInt64(&m_ValueInt)) - 1;
		}

		GDINL TValue operator++(int) volatile
		{
			return static_cast<TValue>(Atomics::InterlockedIncrementInt64(&m_ValueInt));
		}
		GDINL TValue operator--(int) volatile
		{
			return static_cast<TValue>(Atomics::InterlockedDecrementInt64(&m_ValueInt));
		}

		GDINL operator TValue() const volatile
		{
			return Get();
		}
	};	// struct AtomicValue64<T64>

#endif	// if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64

	typedef AtomicValue64<Int64> AtomicInt64;
	typedef AtomicValue64<UInt64> AtomicUInt64;

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Pointer atomic value.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	template<typename TValue>
	struct AtomicValuePtr final : public TNonCopyable
	{
	private:
		TValue m_ValuePtr;

	public:

		/*!
		 * Initializes an empty atomic value.
		 */
		GDINL constexpr AtomicValuePtr()
			: m_ValuePtr(nullptr)
		{}

		/*!
		 * Initializes an atomic value.
		 * @param value The initial value.
		 */
		GDINL constexpr implicit AtomicValuePtr(TValue const value)
			: m_ValuePtr(value)
		{}

	public:

		/*!
		 * Atomically reads this value.
		 * @returns The value.
		 */
		GDINL TValue Get() volatile const
		{
			return const_cast<AtomicValuePtr*>(Set(m_ValuePtr));
		}

		/*!
		 * Atomically reads this value.
		 * @returns Original value.
		 */
		GDINL TValue Set(TValue const value) volatile
		{
			return static_cast<TValue volatile*>(Atomics::InterlockedExchangePtr(&m_ValuePtr, value));
		}

		/*!
		 * Atomically exchanges this value to other one if original value is equal to comparand.
		 *
		 * @param exchange The value which is exchanged.
		 * @param comparand The comparand with which original value is compared.
		 *
		 * @returns Original value.
		 */
		GDINL TValue CompareExchange(TValue const exchange, TValue const comparand) volatile
		{
			return static_cast<TValue volatile*>(Atomics::InterlockedCompareExchangePtr(&m_ValuePtr, exchange, comparand));
		}

	public:
		GDINL TValue operator=(TValue const value) volatile
		{
			Set(value);
			return value;
		}

		GDINL implicit operator TValue() const volatile
		{
			return Get();
		}
	};	// struct AtomicValuePtr<TValue>

	template<typename TValue>
	using AtomicPtr = AtomicValuePtr<TValue volatile*>;

GD_NAMESPACE_END
