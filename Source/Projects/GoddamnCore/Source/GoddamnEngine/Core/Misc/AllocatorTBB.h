// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Misc/AllocatorTBB.h
 * File contains allocator interface that uses Intel Threading Building Blocks.
 */
#pragma once

#include <tbb/scalable_allocator.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Intel Threading Building Blocks allocator interface.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class PlatformAllocatorTBB : public PlatformAllocatorGeneric
	{
	public:
		
		/*!
		 * Returns true if this allocator is thread-safe.
		 */
		GDINL constexpr static bool IsThreadSafe()
		{
			return true;
		}

		/*!
		 * Returns true if this allocator contains aligned allocation functions.
		 */
		GDINL constexpr static bool CanAllocateAligned()
		{
			return true;
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
			return scalable_malloc(allocationSize);
		}

		/*!
		 * Deallocates unaligned block of memory.
		 * @param memory The memory that would be deallocated. If specified block is nullptr then does nothing.
		 */
		GDINL static void DeallocateUnaligned(Handle const memory)
		{
			scalable_free(memory);
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
			return scalable_aligned_malloc(allocationSize, alignment);
		}

		/*!
		 * Deallocates aligned block of memory.
		 * @param memory The memory that would be deallocated. If specified block is nullptr then does nothing.
		 */
		GDINL static void DeallocateAligned(Handle const memory)
		{
			scalable_aligned_free(memory);
		}

		/*!
		 * Returns the length of the allocated block.
		 */
		GDAPI static SizeTp GetAllocationSize(Handle const memory)
		{
			return scalable_msize(memory);
		}

	};	// class PlatformAllocatorTBB

GD_NAMESPACE_END
