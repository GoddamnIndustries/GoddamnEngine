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
#if GD_PLATFORM_API_POSIX

GD_NAMESPACE_BEGIN

    // **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
    //! Atomic instructions on Posix platforms.
    // **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
    class GD_PLATFORM_KERNEL PosixPlatformAtomics final : public IPlatformAtomics
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
            GD_DEBUG_VERIFY(IsPointerAligned(value));
            return __sync_val_compare_and_swap(value, comparand, exchange);
        }
        GDINT virtual Int64 AtomicCompareExchange(Int64 volatile* const value, Int64 const exchange, Int64 const comparand) const override final
        {
            GD_DEBUG_VERIFY(IsPointerAligned(value));
            return __sync_val_compare_and_swap(value, comparand, exchange);
        }
        GDINT virtual CHandle AtomicCompareExchangePtr(CHandle volatile* const value, CHandle const exchange, CHandle const comparand) const override final
        {
            GD_DEBUG_VERIFY(IsPointerAligned(value));
            return __sync_val_compare_and_swap(value, comparand, exchange);
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
            GD_DEBUG_VERIFY(IsPointerAligned(value));
            return __sync_lock_test_and_set(value, exchange);
        }
        GDINT virtual Int64 AtomicExchange(Int64 volatile* const value, Int64 const exchange) const override final
        {
            GD_DEBUG_VERIFY(IsPointerAligned(value));
            return __sync_lock_test_and_set(value, exchange);
        }
        GDINT virtual CHandle AtomicExchangePtr(CHandle volatile* const value, CHandle const exchange) const override final
        {
            GD_DEBUG_VERIFY(IsPointerAligned(value));
            return __sync_lock_test_and_set(value, exchange);
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
            GD_DEBUG_VERIFY(IsPointerAligned(value));
            return __sync_fetch_and_add(value, 1) + 1;
        }
        GDINT virtual Int64 AtomicIncrement(Int64 volatile* const value) const override final
        {
            GD_DEBUG_VERIFY(IsPointerAligned(value));
            return __sync_fetch_and_add(value, 1) + 1;
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
            GD_DEBUG_VERIFY(IsPointerAligned(value));
            return __sync_fetch_and_sub(value, 1) - 1;
        }
        GDINT virtual Int64 AtomicDecrement(Int64 volatile* const value) const override final
        {
            GD_DEBUG_VERIFY(IsPointerAligned(value));
            return __sync_fetch_and_sub(value, 1) - 1;
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
            GD_DEBUG_VERIFY(IsPointerAligned(value));
            return __sync_fetch_and_add(value, amount);
        }
        GDINT virtual Int64 AtomicAdd(Int64 volatile* const value, Int64 const amount) const override final
        {
            GD_DEBUG_VERIFY(IsPointerAligned(value));
            return __sync_fetch_and_add(value, amount);
        }
        //! @}
    };    // class PosixPlatformAtomics

    GD_IMPLEMENT_SINGLETON(IPlatformAtomics, PosixPlatformAtomics);

GD_NAMESPACE_END

#endif  // if GD_PLATFORM_API_POSIX
