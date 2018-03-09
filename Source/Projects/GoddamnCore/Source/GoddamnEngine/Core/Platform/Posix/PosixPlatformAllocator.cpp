// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
//
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file
 * Allocator implementation.
 */
#include <GoddamnEngine/Core/Platform/PlatformAllocator.h>
#if GD_PLATFORM_API_POSIX

#include <stdlib.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Memory allocator on Posix platforms.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL PosixPlatformAllocator final : public IPlatformAllocator
	{
	private:

		// ------------------------------------------------------------------------------------------
		// Memory allocation.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Allocates a memory block of the specified size.
		 *
		 * @param allocationPointer Allocated memory pointer.
		 * @param allocationSizeBytes Size of memory block to allocate in bytes.
		 *
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool MemoryAllocate(Handle& allocationPointer, SizeTp const allocationSizeBytes) override final
		{
			if (allocationSizeBytes == 0)
			{
				allocationPointer = nullptr;
				return true;
			}
			allocationPointer = malloc(allocationSizeBytes);
			return allocationPointer != nullptr;
		}

		/*!
		 * Allocates an aligned memory block of the specified size.
		 *
		 * @param allocationPointer Allocated memory pointer.
		 * @param allocationAlignment Memory alignment.
		 * @param allocationSizeBytes Size of memory block to allocate in bytes.
		 *
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool MemoryAllocateAligned(Handle& allocationPointer, SizeTp const allocationSizeBytes, SizeTp const allocationAlignment) override final
		{
			if (allocationSizeBytes == 0)
			{
				allocationPointer = nullptr;
				return true;
			}
			return posix_memalign(&allocationPointer, allocationAlignment, allocationSizeBytes) == 0;
		}

		// ------------------------------------------------------------------------------------------
		// Memory deallocation.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Deallocates the specified memory block.
		 * Memory should be allocated with @c MemoryAllocate function.
		 *
		 * @param allocationPointer Allocated memory pointer.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool MemoryFree(Handle const allocationPointer) override final
		{
			free(allocationPointer);
			return true;
		}

		/*!
		 * Deallocates the specified aligned memory block.
		 * Memory should be allocated with @c MemoryAllocateAligned function.
		 *
		 * @param allocationPointer Allocated memory pointer.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool MemoryFreeAligned(Handle const allocationPointer) override final
		{
			free(allocationPointer);
			return true;
		}
	};	// class IPlatformAllocator

	GD_IMPLEMENT_SINGLETON(IPlatformAllocator, PosixPlatformAllocator);

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_API_POSIX
