// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Misc/Allocator.h
 * File contains generic allocator interface.
 */
#pragma once
#define GD_INSIDE_ALLOCATOR_H

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/BasePlatform.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Generic allocator interface.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class PlatformAllocatorGeneric : public TNonCreatable
	{
	public:
		SizeTp static const DefaultAlignment = 2 * sizeof(Handle);

		/*!
		 * Initializes current allocator.
		 */
		GDINL static void OnInitializeAllocator()
		{
		}

		/*!
		 * Deinitializes current allocator.
		 */
		GDINL static void OnDeinitializeAllocator()
		{
		}

		/*!
		 * Returns true if this allocator is thread-safe.
		 */
		GDINL constexpr static bool IsThreadSafe()
		{
			return false;
		}

		/*!
		 * Returns true if this allocator contains aligned allocation functions.
		 */
		GDINL constexpr static bool CanAllocateAligned()
		{
			return false;
		}

		/*!
		 * Allocates unaligned block of memory with specified size.
		 * @returns Pointer on the allocated memory.
		 */
		GDINL static Handle AllocateUnaligned(SizeTp const allocationSize
#if GD_DEBUG
			, CStr const allocationFilename, Int32 const allocationLineNumber
#endif	// if GD_DEBUG
			)
		{
#if GD_DEBUG
			GD_NOT_USED(allocationFilename);
			GD_NOT_USED(allocationLineNumber);
#endif	// if GD_DEBUG
			return allocationSize != 0 ? ::malloc(allocationSize) : nullptr;
		}

		/*!
		 * Deallocates unaligned block of memory.
		 * @param memory The memory that would be deallocated. If specified block is nullptr then does nothing.
		 */
		GDINL static void DeallocateUnaligned(Handle const memory)
		{
			::free(memory);
		}

		/*!
		 * Allocates block of memory with specified size that is aligned by specified value and returns pointer to it.
		 * @returns Pointer on the allocated memory.
		 */
		GDINL static Handle AllocateAligned(SizeTp const allocationSize, SizeTp const alignment
#if GD_DEBUG
			, CStr const allocationFilename, Int32 const allocationLineNumber
#endif	// if GD_DEBUG
			)
		{
#if GD_DEBUG
			GD_NOT_USED(allocationFilename);
			GD_NOT_USED(allocationLineNumber);
#endif	// if GD_DEBUG
			GD_NOT_USED(allocationSize);
			GD_NOT_USED(alignment);
			GD_NOT_SUPPORTED();
		}

		/*!
		 * Deallocates aligned block of memory.
		 * @param memory The memory that would be deallocated. If specified block is nullptr then does nothing.
		 */
		GDINL static void DeallocateAligned(Handle const memory)
		{
			GD_NOT_USED(memory);
			GD_NOT_SUPPORTED();
		}

		/*!
		 * Returns the length of the allocated block.
		 */
		GDAPI static SizeTp GetAllocationSize(Handle const memory)
		{
			GD_NOT_USED(memory);
			GD_NOT_SUPPORTED();
		}

	};	// class PlatformAllocatorGeneric

GD_NAMESPACE_END

#include GD_PLATFORM_INCLUDE(GoddamnEngine/Core/Misc, Allocator)
#undef GD_INSIDE_DIRECTORY_H
