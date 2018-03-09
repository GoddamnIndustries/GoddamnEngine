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
#if GD_PLATFORM_API_MICROSOFT

#include <crtdbg.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Memory allocator on Microsoft platforms.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL MicrosoftPlatformAllocator final : public IPlatformAllocator
	{
	public:
		GDINL MicrosoftPlatformAllocator()
		{
#if GD_DEBUG
			_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_FILE);
			_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
#endif	// if GD_DEBUG
		}

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
		GDINT virtual bool MemoryAllocateDebug(Handle& allocationPointer, SizeTp const allocationSizeBytes, CStr const allocationFilename, UInt32 const allocationLineNumber) override final
		{
			if (allocationSizeBytes == 0)
			{
				allocationPointer = nullptr;
				return true;
			}
			allocationPointer = _malloc_dbg(allocationSizeBytes, _NORMAL_BLOCK, allocationFilename, allocationLineNumber);
			return allocationPointer != nullptr;
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
		GDINT virtual bool MemoryAllocateAligned(Handle& allocationPointer, SizeTp const allocationSizeBytes, SizeTp const allocationAlignment) override final
		{
			if (allocationSizeBytes == 0)
			{
				allocationPointer = nullptr;
				return true;
			}
			allocationPointer = _aligned_malloc(allocationSizeBytes, allocationAlignment);
			return allocationPointer != nullptr;
		}

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
		GDINT virtual bool MemoryAllocateAlignedDebug(Handle& allocationPointer, SizeTp const allocationSizeBytes, SizeTp const allocationAlignment, CStr const allocationFilename, UInt32 const allocationLineNumber) override final
		{
			if (allocationSizeBytes == 0)
			{
				allocationPointer = nullptr;
				return true;
			}
			allocationPointer = _aligned_malloc_dbg(allocationSizeBytes, allocationAlignment, allocationFilename, allocationLineNumber);
			return allocationPointer != nullptr;
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
		GDINT virtual bool MemoryFree(Handle const allocationPointer) override final
		{
			free(allocationPointer);
			return true;
		}

#if GD_DEBUG
		/*!
		 * Deallocates the specified memory block.
		 * Memory should be allocated with @c MemoryAllocateDebug function.
		 * 
		 * @param allocationPointer Allocated memory pointer.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool MemoryFreeDebug(Handle const allocationPointer) override final
		{
			_free_dbg(allocationPointer, _NORMAL_BLOCK);
			return true;
		}
#endif	// if GD_DEBUG

		/*!
		 * Deallocates the specified aligned memory block.
		 * Memory should be allocated with @c MemoryAllocateAligned function.
		 * 
		 * @param allocationPointer Allocated memory pointer.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool MemoryFreeAligned(Handle const allocationPointer) override final
		{
			_aligned_free(allocationPointer);
			return true;
		}

#if GD_DEBUG
		/*!
		 * Deallocates the specified aligned memory block.
		 * Memory should be allocated with @c MemoryAllocateAligned function.
		 * 
		 * @param allocationPointer Allocated memory pointer.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool MemoryFreeAlignedDebug(Handle const allocationPointer) override final
		{
			_aligned_free_dbg(allocationPointer);
			return true;
		}
#endif	// if GD_DEBUG
	};	// class IPlatformAllocator

	GD_IMPLEMENT_SINGLETON(IPlatformAllocator, MicrosoftPlatformAllocator);

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_API_MICROSOFT
