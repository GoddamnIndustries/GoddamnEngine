// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file
 * Atomics implementation.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/Singleton.h>
#include <GoddamnEngine/Core/Templates/TypeTraits.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Atomic instructions.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL IPlatformAtomics : public Singleton<IPlatformAtomics>
	{
	public:

		// ------------------------------------------------------------------------------------------
		// Atomic exchange operations.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Atomically exchanges specified value to other one if original value is equal to comparand.
		 *
		 * @param value The value on which exchange is performed.
		 * @param exchange The value which is exchanged.
		 * @param comparand The comparand with which original value is compared.
		 *
		 * @returns Original value.
		 */
		//! @{
		GDINT virtual Int32 AtomicCompareExchange(Int32 volatile* const value, Int32 const exchange, Int32 const comparand) const GD_PURE_VIRTUAL;
		GDINT virtual Int64 AtomicCompareExchange(Int64 volatile* const value, Int64 const exchange, Int64 const comparand) const GD_PURE_VIRTUAL;
		GDINT virtual CHandle AtomicCompareExchangePtr(CHandle volatile* const value, CHandle const exchange, CHandle const comparand) const
		{
			return reinterpret_cast<CHandle>(AtomicCompareExchange(reinterpret_cast<IntPtr volatile*>(value), reinterpret_cast<IntPtr>(exchange), reinterpret_cast<IntPtr>(comparand)));
		}
		//! @}

		/*!
		 * Atomically exchanges specified value to other one.
		 *
		 * @param value The value on which exchange is performed.
		 * @param exchange Value which is exchanged.
		 *
		 * @returns Original value.
		 */
		//! @{
		GDINT virtual Int32 AtomicExchange(Int32 volatile* const value, Int32 const exchange) const
		{
			Int32 resultValue;
			do
			{
				resultValue = *value;
			} while (AtomicCompareExchange(value, exchange, resultValue) != resultValue);
			return resultValue;
		}
		GDINT virtual Int64 AtomicExchange(Int64 volatile* const value, Int64 const exchange) const
		{
			Int64 resultValue;
			do
			{
				resultValue = *value;
			} while (AtomicCompareExchange(value, exchange, resultValue) != resultValue);
			return resultValue;
		}
		GDINT virtual CHandle AtomicExchangePtr(CHandle volatile* const value, CHandle const exchange) const
		{
			return reinterpret_cast<CHandle>(AtomicExchange(reinterpret_cast<IntPtr volatile*>(value), reinterpret_cast<IntPtr>(exchange)));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// Atomic arithmetic operations.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Atomically increments specified value.
		 *
		 * @param value The value to increment.
		 * @returns Incremented value.
		 */
		//! @{
		GDINT virtual Int32 AtomicIncrement(Int32 volatile* const value) const
		{
			Int32 resultValue;
			do
			{
				resultValue = *value;
			} while (AtomicCompareExchange(value, resultValue + 1, resultValue) != resultValue);
			return resultValue + 1;
		}
		GDINT virtual Int64 AtomicIncrement(Int64 volatile* const value) const
		{
			Int64 resultValue;
			do
			{
				resultValue = *value;
			} while (AtomicCompareExchange(value, resultValue + 1, resultValue) != resultValue);
			return resultValue + 1;
		}
		//! @}

		/*!
		 * Atomically decrements specified value.
		 *
		 * @param value The value to decrement.
		 * @returns Decremented value.
		 */
		//! @{
		GDINT virtual Int32 AtomicDecrement(Int32 volatile* const value) const
		{
			Int32 resultValue;
			do
			{
				resultValue = *value;
			} while (AtomicCompareExchange(value, resultValue - 1, resultValue) != resultValue);
			return resultValue - 1;
		}
		GDINT virtual Int64 AtomicDecrement(Int64 volatile* const value) const
		{
			Int64 resultValue;
			do
			{
				resultValue = *value;
			} while (AtomicCompareExchange(value, resultValue - 1, resultValue) != resultValue);
			return resultValue - 1;
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
		GDINT virtual Int32 AtomicAdd(Int32 volatile* const value, Int32 const amount) const
		{
			Int32 resultValue;
			do
			{
				resultValue = *value;
			} while (AtomicCompareExchange(value, resultValue + amount, resultValue) != resultValue);
			return resultValue;
		}
		GDINT virtual Int64 AtomicAdd(Int64 volatile* const value, Int64 const amount) const
		{
			Int64 resultValue;
			do
			{
				resultValue = *value;
			} while (AtomicCompareExchange(value, resultValue + amount, resultValue) != resultValue);
			return resultValue;
		}
		//! @}

	protected:

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
	};	// class IPlatformAtomics

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Integral atomic value.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	template<typename TValue>
	struct GD_PLATFORM_WRAPPER AtomicInteger final : public TNonCopyable
	{
		static_assert(TypeTraits::IsIntegral<TValue>::Value || TypeTraits::IsEnum<TValue>::Value, "Template parameter should be integral value.");
		static_assert(sizeof(TValue) == 4 || sizeof(TValue) == 8, "Template parameter should be 32-bit or 64-bit integral value.");

	private:
		using Integer = Conditional<sizeof(TValue) == 4, Int32, Int64>;
		union {
			TValue  volatile m_Value;
			Integer volatile m_ValueInt;
		};

	public:

		/*!
		 * Initializes an empty atomic value.
		 */
		GDINL constexpr AtomicInteger()
			: m_ValueInt(0)
		{}

		/*!
		 * Initializes an atomic value.
		 * @param value The initial value.
		 */
		GDINL constexpr explicit AtomicInteger(TValue const value)
			: m_ValueInt(value)
		{}

	public:

		/*!
		 * Atomically reads this value.
		 * @returns The value.
		 */
		GDINL TValue Get() volatile const
		{
			return static_cast<TValue>(IPlatformAtomics::Get().AtomicAdd(const_cast<Int32 volatile*>(&m_ValueInt), 0));
		}

		/*!
		 * Atomically reads this value.
		 * @returns Original value.
		 */
		GDINL TValue Set(TValue const value) volatile
		{
			return static_cast<TValue>(IPlatformAtomics::Get().AtomicExchange(&m_ValueInt, static_cast<Integer>(value)));
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
			return static_cast<TValue>(IPlatformAtomics::Get().AtomicCompareExchange(&m_ValueInt, static_cast<Int32>(exchange), static_cast<Int32>(comparand)));
		}

		/*!
		 * Atomically add specified amount to this value.
		 *
		 * @param amount The amount to add.
		 * @returns Original value.
		 */
		GDINL TValue FetchAdd(TValue const amount) volatile
		{
			return static_cast<TValue>(IPlatformAtomics::Get().AtomicAdd(&m_ValueInt, static_cast<Int32>(amount)));
		}

		/*!
		 * Atomically subtracts specified amount from this value.
		 *
		 * @param amount The amount to subtract.
		 * @returns Original value.
		 */
		GDINL TValue FetchSub(TValue const amount) volatile
		{ 
			return static_cast<TValue>(IPlatformAtomics::Get().AtomicAdd(&m_ValueInt, -static_cast<Int32>(amount)));
		}

	public:
		// atomic_integer = integer
		GDINL TValue operator=(TValue const value) volatile
		{
			Set(value);
			return value;
		}

		// atomic_integer += integer
		GDINL TValue operator+=(TValue const amount) volatile
		{
			return FetchAdd(amount);
		}
		GDINL TValue operator-=(TValue const amount) volatile
		{
			return FetchSub(amount);
		}

		// ++atomic_integer
		GDINL TValue operator++() volatile
		{
			return static_cast<TValue>(IPlatformAtomics::Get().AtomicIncrement(&m_ValueInt));
		}
		GDINL TValue operator--() volatile
		{
			return static_cast<TValue>(IPlatformAtomics::Get().AtomicDecrement(&m_ValueInt));
		}

		// atomic_integer--
		GDINL TValue operator++(int) volatile
		{
			return static_cast<TValue>(IPlatformAtomics::Get().AtomicIncrement(&m_ValueInt)) + 1;
		}
		GDINL TValue operator--(int) volatile
		{
			return static_cast<TValue>(IPlatformAtomics::Get().AtomicDecrement(&m_ValueInt)) - 1;
		}

		// (integer)atomic_integer
		GDINL implicit operator TValue() const volatile
		{
			return Get();
		}
	};	// struct AtomicInteger<T>

	using AtomicInt32  = AtomicInteger<Int32>;
	using AtomicUInt32 = AtomicInteger<UInt32>;
	using AtomicInt64  = AtomicInteger<Int64>;
	using AtomicUInt64 = AtomicInteger<UInt64>;
	using AtomicBool   = AtomicUInt32;

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Pointer atomic value.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	template<typename TValue>
	struct GD_PLATFORM_WRAPPER AtomicPointer final : public TNonCopyable
	{
	private:
		TValue m_ValuePtr;

	public:

		/*!
		 * Initializes an empty atomic value.
		 */
		GDINL constexpr AtomicPointer()
			: m_ValuePtr(nullptr)
		{}

		/*!
		 * Initializes an atomic value.
		 * @param value The initial value.
		 */
		GDINL constexpr implicit AtomicPointer(TValue const value)
			: m_ValuePtr(value)
		{}

	public:

		/*!
		 * Atomically reads this value.
		 * @returns The value.
		 */
		GDINL TValue Get() volatile const
		{
			return const_cast<AtomicPointer*>(Set(m_ValuePtr));
		}

		/*!
		 * Atomically reads this value.
		 * @returns Original value.
		 */
		GDINL TValue Set(TValue const value) volatile
		{
			return static_cast<TValue volatile*>(IPlatformAtomics::Get().AtomicExchangePtr(&m_ValuePtr, value));
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
			return static_cast<TValue volatile*>(IPlatformAtomics::Get().AtomicCompareExchangePtr(&m_ValuePtr, exchange, comparand));
		}

	public:
		// atomic_pointer = pointer
		GDINL TValue operator=(TValue const value) volatile
		{
			Set(value);
			return value;
		}

		// (pointer)atomic_pointer
		GDINL implicit operator TValue() const volatile
		{
			return Get();
		}
	};	// struct AtomicPointer<T>

	template<typename TValue>
	using AtomicPtr = AtomicPointer<TValue volatile*>;

GD_NAMESPACE_END
