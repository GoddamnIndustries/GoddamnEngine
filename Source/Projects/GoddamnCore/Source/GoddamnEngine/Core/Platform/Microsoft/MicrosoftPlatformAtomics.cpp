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
#include <GoddamnEngine/Core/Platform/PlatformAtomics.h>
#if GD_PLATFORM_API_MICROSOFT

#include <Windows.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Atomic instructions on Microsoft platforms.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL MicrosoftPlatformAtomics final : public IPlatformAtomics
	{
	private:

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
		GDINT virtual Int32 AtomicCompareExchange(Int32 volatile* const value, Int32 const exchange, Int32 const comparand) const override final
		{
			GD_ASSERT(IsPointerAligned(value));
			return static_cast<Int32>(InterlockedCompareExchange(reinterpret_cast<LONG volatile*>(value), static_cast<LONG>(exchange), static_cast<LONG>(comparand)));
		}
		GDINT virtual Int64 AtomicCompareExchange(Int64 volatile* const value, Int64 const exchange, Int64 const comparand) const override final
		{
			GD_ASSERT(IsPointerAligned(value));
			return static_cast<Int64>(InterlockedCompareExchange64(reinterpret_cast<LONG64 volatile*>(value), static_cast<LONG64>(exchange), static_cast<LONG64>(comparand)));
		}
		GDINT virtual CHandle AtomicCompareExchangePtr(CHandle volatile* const value, CHandle const exchange, CHandle const comparand) const override final
		{
			GD_ASSERT(IsPointerAligned(value));
			return InterlockedCompareExchangePointer(static_cast<PVOID volatile*>(const_cast<Handle volatile*>(value)), const_cast<Handle>(exchange), const_cast<Handle>(comparand));
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
		GDINT virtual Int32 AtomicExchange(Int32 volatile* const value, Int32 const exchange) const override final
		{
			GD_ASSERT(IsPointerAligned(value));
			return static_cast<Int32>(InterlockedExchange(reinterpret_cast<LONG volatile*>(value), static_cast<LONG>(exchange)));
		}
		GDINT virtual Int64 AtomicExchange(Int64 volatile* const value, Int64 const exchange) const override final
		{
			GD_ASSERT(IsPointerAligned(value));
			return static_cast<Int64>(InterlockedExchange64(reinterpret_cast<LONG64 volatile*>(value), static_cast<LONG64>(exchange)));
		}
		GDINT virtual CHandle AtomicExchangePtr(CHandle volatile* const value, CHandle const exchange) const override final
		{
			GD_ASSERT(IsPointerAligned(value));
			return InterlockedExchangePointer(static_cast<PVOID volatile*>(const_cast<Handle volatile*>(value)), const_cast<Handle>(exchange));
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
		GDINT virtual Int32 AtomicIncrement(Int32 volatile* const value) const override final
		{
			GD_ASSERT(IsPointerAligned(value));
			return static_cast<Int32>(InterlockedIncrement(reinterpret_cast<LONG volatile*>(value)));
		}
		GDINT virtual Int64 AtomicIncrement(Int64 volatile* const value) const override final
		{
			GD_ASSERT(IsPointerAligned(value));
			return static_cast<Int64>(InterlockedIncrement64(reinterpret_cast<LONG64 volatile*>(value)));
		}
		//! @}

		/*!
		 * Atomically decrements specified value.
		 *
		 * @param value The value to decrement.
		 * @returns Decremented value.
		 */
		//! @{
		GDINT virtual Int32 AtomicDecrement(Int32 volatile* const value) const override final
		{
			GD_ASSERT(IsPointerAligned(value));
			return static_cast<Int32>(InterlockedDecrement(reinterpret_cast<LONG volatile*>(value)));
		}
		GDINT virtual Int64 AtomicDecrement(Int64 volatile* const value) const override final
		{
			GD_ASSERT(IsPointerAligned(value));
			return static_cast<Int64>(InterlockedDecrement64(reinterpret_cast<LONG64 volatile*>(value)));
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
		GDINT virtual Int32 AtomicAdd(Int32 volatile* const value, Int32 const amount) const override final
		{
			GD_ASSERT(IsPointerAligned(value));
			return static_cast<Int32>(InterlockedAdd(reinterpret_cast<LONG volatile*>(value), static_cast<LONG>(amount)));
		}
		GDINT virtual Int64 AtomicAdd(Int64 volatile* const value, Int64 const amount) const override final
		{
			GD_ASSERT(IsPointerAligned(value));
			return static_cast<Int64>(InterlockedAdd64(reinterpret_cast<LONG64 volatile*>(value), static_cast<LONG64>(amount)));
		}
		//! @}

	};	// class MicrosoftPlatformAtomics

	GD_IMPLEMENT_SINGLETON(IPlatformAtomics, MicrosoftPlatformAtomics);

GD_NAMESPACE_END

#endif
