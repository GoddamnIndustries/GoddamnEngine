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

#define tlsf_assert GD_VERIFY
#include <tlsf.c>

GD_NAMESPACE_BEGIN

#if 0
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! TLSF Memory allocator.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL TlsfAllocator : public IPlatformAllocator
	{
	private:
		tlsf_t m_Pool = nullptr;

	public:

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
		GDINT virtual bool MemoryAllocate(Handle& allocationPointer, SizeTp const allocationSizeBytes)
		{
			if (m_Pool == nullptr)
			{
				//m_Pool = tlsf_create();
			}

			tlsf_malloc(m_Pool, allocationSizeBytes);
		}

#if GD_DEBUG
		/*!
		 * Allocates a memory block of the specified size.
		 * 
		 * @param allocationPointer Allocated memory pointer.
		 * @param allocationSizeBytes Size of memory block to allocate in bytes.
		 * @param allocationFilename File in which allocation is requested.
		 * @param allocationLineNumber Line number in which allocation is requested.
		 *
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool MemoryAllocateDebug(Handle& allocationPointer, SizeTp const allocationSizeBytes, CStr const allocationFilename, UInt32 const allocationLineNumber)
		{
			GD_NOT_USED_L(allocationFilename, allocationLineNumber);
			return MemoryAllocate(allocationPointer, allocationSizeBytes);
		}
#endif	// if GD_DEBUG

		/*!
		 * Allocates an aligned memory block of the specified size.
		 * 
		 * @param allocationPointer Allocated memory pointer.
		 * @param allocationAlignment Memory alignment.
		 * @param allocationSizeBytes Size of memory block to allocate in bytes.
		 *
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool MemoryAllocateAligned(Handle& allocationPointer, SizeTp const allocationSizeBytes, SizeTp const allocationAlignment) GD_PURE_VIRTUAL;

#if GD_DEBUG
		/*!
		 * Allocates an aligned memory block of the specified size.
		 * 
		 * @param allocationPointer Allocated memory pointer.
		 * @param allocationAlignment Memory alignment.
		 * @param allocationSizeBytes Size of memory block to allocate in bytes.
		 * @param allocationFilename File in which allocation is requested.
		 * @param allocationLineNumber Line number in which allocation is requested.
		 *
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool MemoryAllocateAlignedDebug(Handle& allocationPointer, SizeTp const allocationSizeBytes, SizeTp const allocationAlignment, CStr const allocationFilename, UInt32 const allocationLineNumber)
		{
			GD_NOT_USED_L(allocationFilename, allocationLineNumber);
			return MemoryAllocateAligned(allocationPointer, allocationSizeBytes, allocationAlignment);
		}
#endif	// if GD_DEBUG

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
		GDINT virtual bool MemoryFree(Handle const allocationPointer) GD_PURE_VIRTUAL;

#if GD_DEBUG
		/*!
		 * Deallocates the specified memory block.
		 * Memory should be allocated with @c MemoryAllocateDebug function.
		 * 
		 * @param allocationPointer Allocated memory pointer.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool MemoryFreeDebug(Handle const allocationPointer)
		{
			return MemoryFree(allocationPointer);
		}
#endif	// if GD_DEBUG

		/*!
		 * Deallocates the specified aligned memory block.
		 * Memory should be allocated with @c MemoryAllocateAligned function.
		 * 
		 * @param allocationPointer Allocated memory pointer.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool MemoryFreeAligned(Handle const allocationPointer) GD_PURE_VIRTUAL;

#if GD_DEBUG
		/*!
		 * Deallocates the specified aligned memory block.
		 * Memory should be allocated with @c MemoryAllocateAligned function.
		 * 
		 * @param allocationPointer Allocated memory pointer.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool MemoryFreeAlignedDebug(Handle const allocationPointer)
		{
			return MemoryFreeAligned(allocationPointer);
		}
#endif	// if GD_DEBUG
	};	// class TlsfAllocator
#endif

	/*!
	 * Helper function for memory allocation.
	 * @note Use @c GD_MALLOC instead.
	 */
	//! @{
#if GD_DEBUG
	GDAPI Handle GD_PLATFORM_WRAPPER AllocateMemory(SizeTp const allocationSizeBytes, CStr const allocationFilename, UInt32 const allocationLineNumber, bool* const resultPtr)
	{
		Handle allocationPointer = nullptr;
		auto const result = IPlatformAllocator::Get().MemoryAllocateAlignedDebug(allocationPointer, allocationSizeBytes, 2 * sizeof(allocationPointer), allocationFilename, allocationLineNumber);
		if (resultPtr != nullptr)
		{
			*resultPtr = result;
		}
		else if (!result)
		{
			GD_VERIFY_FALSE("Unhandled allocation error: failed to free memory block.");
		}
		return allocationPointer;
	}
#else	// if GD_DEBUG
	GDAPI Handle GD_PLATFORM_WRAPPER AllocateMemory(SizeTp const allocationSizeBytes, bool* const resultPtr)
	{
		Handle allocationPointer = nullptr;
		auto const result = IPlatformAllocator::Get().MemoryAllocateAligned(allocationPointer, allocationSizeBytes, 2 * sizeof(allocationPointer));
		if (resultPtr != nullptr)
		{
			*resultPtr = result;
		}
		else if (!result)
		{
			GD_VERIFY_FALSE("Unhandled allocation error: failed to free memory block.");
		}
		return allocationPointer;
	}
#endif	// if GD_DEBUG
	//! @}

	/*!
	 * Helper function for memory deallocation.
	 * @note Use @c GD_MALLOC instead.
	 */
	//! @{
#if GD_DEBUG
	GDAPI void GD_PLATFORM_WRAPPER FreeMemory(Handle const allocationPointer, bool* const resultPtr)
	{
		auto const result = IPlatformAllocator::Get().MemoryFreeAlignedDebug(allocationPointer);
		if (resultPtr != nullptr)
		{
			*resultPtr = result;
		}
		else if (!result)
		{
			GD_VERIFY_FALSE("Unhandled allocation error: failed to free memory block.");
		}
	}
#else	// if GD_DEBUG
	GDAPI void GD_PLATFORM_WRAPPER FreeMemory(Handle const allocationPointer, bool* const resultPtr)
	{
		auto const result = IPlatformAllocator::Get().MemoryFreeAligned(allocationPointer);
		if (resultPtr != nullptr)
		{
			*resultPtr = result;
		}
		else if (!result)
		{
			GD_VERIFY_FALSE("Unhandled allocation error: failed to free memory block.");
		}
	}
#endif	// if GD_DEBUG
	//! @}

GD_NAMESPACE_END
