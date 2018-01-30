// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file
 * Jason Evans' allocator.
 */
#pragma once
#if !defined(GD_INSIDE_ALLOCATOR_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Core/Misc/Allocator.h> instead.
#endif	// if !defined(GD_INSIDE_ALLOCATOR_H)

#include <jemalloc/jemalloc.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Jason Evans' allocator interface.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class PlatformAllocatorJE : public PlatformAllocatorGeneric
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
			GD_NOT_USED_L(allocationFilename, allocationLineNumber);
#endif	// if GD_DEBUG
			return je_malloc(allocationSize);
		}

		/*!
		 * Deallocates unaligned block of memory.
		 * @param memory The memory that would be deallocated. If specified block is nullptr then does nothing.
		 */
		GDINL static void DeallocateUnaligned(Handle const memory)
		{
			je_free(memory);
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
			GD_NOT_USED_L(allocationFilename, allocationLineNumber);
#endif	// if GD_DEBUG
			return je_aligned_alloc(allocationSize, alignment);
		}

		/*!
		 * Deallocates aligned block of memory.
		 * @param memory The memory that would be deallocated. If specified block is nullptr then does nothing.
		 */
		GDINL static void DeallocateAligned(Handle const memory)
		{
			je_free(memory);
		}

		/*!
		 * Returns the length of the allocated block.
		 */
		GDAPI static SizeTp GetAllocationSize(Handle const memory)
		{
			return je_malloc_usable_size(memory);
		}

	};	// class PlatformAllocatorJE

GD_NAMESPACE_END
