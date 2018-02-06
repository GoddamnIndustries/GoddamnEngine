// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Base/Allocator.cpp
 * Contains memory allocator interface.
 */
#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Misc/PlatformAllocator.h>
#include <GoddamnEngine/Core/Concurrency/LiteMutex.h>

GD_NAMESPACE_BEGIN
 
	struct AllocatorInitializer
	{
		GDINL AllocatorInitializer() { PlatformAllocator::OnInitializeAllocator(); }
		GDINL ~AllocatorInitializer() { PlatformAllocator::OnDeinitializeAllocator(); }
	};	// struct AllocatorInitializer
	GDINT static AllocatorInitializer g_AllocatorInitializer;
	GDINT static LiteMutex g_AllocatorMutex;

	/*!
	 * Allocates block of memory with specified size that is aligned by specified value and returns pointer to it.
	 *
	 * @param location Information about place in code, where this allocation occurs.
	 * @param allocationSize Size of required memory in bytes.
	 * @param alignment pointer to memory would be aligned by this value.
	 *
	 * @returns Pointer on the allocated memory. This function never returns nullptr except specified size is 0.
	 * @note This function should not be directly invoked. Use 'GD_MALLOC'/'GD_MALLOC_ALIGNED' macro instead.
	 */
	//! @{
	GDAPI Handle Allocator::AllocateMemory(SizeTp const allocationSize
#if GD_DEBUG
		, CStr const allocationFilename, Int32 const allocationLineNumber
#endif	// if GD_DEBUG
		)
	{
		return AllocateMemory(allocationSize, PlatformAllocator::DefaultAlignment
#if GD_DEBUG
			, allocationFilename, allocationLineNumber
#endif	// if GD_DEBUG
			);
	}
	GDAPI Handle Allocator::AllocateMemory(SizeTp const allocationSize, SizeTp const alignment
#if GD_DEBUG
		, CStr const allocationFilename, Int32 const allocationLineNumber
#endif	// if GD_DEBUG
		)
	{
		GD_DEBUG_VERIFY(alignment > 1, "Alignment should be greater than 1.");
		GD_DEBUG_VERIFY((alignment & alignment - 1) == 0, "Alignment should power of 2.");

		if (allocationSize == 0)
		{
			return nullptr;
		}

		ScopedConditionalCriticalSection const allocatorMutexLock(g_AllocatorMutex, !PlatformAllocator::IsThreadSafe());

		Handle allocatedMemory;
		if (!PlatformAllocator::CanAllocateAligned())
		{
			// Manually aligning memory for platforms without aligned_alloc.
			auto const offset = alignment - 1 + sizeof(Handle);
			auto const allocatedUnalignedMemory = PlatformAllocator::AllocateUnaligned(allocationSize + offset
#if GD_DEBUG
				, allocationFilename, allocationLineNumber
#endif	// if GD_DEBUG
				);
			GD_VERIFY(allocatedUnalignedMemory != nullptr, "Failed to allocate the memory block.");

			auto const allocatedMemoryPtr = reinterpret_cast<Handle*>(reinterpret_cast<SizeTp>(allocatedUnalignedMemory) + offset & ~(alignment - 1));
			allocatedMemoryPtr[-1] = allocatedUnalignedMemory;
			allocatedMemory = allocatedMemoryPtr;
		}
		else
		{
			// Simply allocating aligned memory.
			allocatedMemory = PlatformAllocator::AllocateAligned(allocationSize, alignment
#if GD_DEBUG
				, allocationFilename, allocationLineNumber
#endif	// if GD_DEBUG
				);
			GD_VERIFY(allocatedMemory != nullptr, "Failed to allocate the memory block.");
		}

		return allocatedMemory;
	}
	//! @}

	/*!
	 * Reallocates block of memory with specified size that is aligned by specified value and returns pointer to it.
	 *
	 * @param memory Block of memory to reallocate.
	 * @param allocationSize Size of required memory in bytes.
	 * @param alignment pointer to memory would be aligned by this value.
	 *
	 * @returns Pointer on the allocated memory. This function never returns nullptr.
	 * @note This function should not be directly invoked. Use 'GD_MALLOC'/'GD_MALLOC_ALIGNED' macro instead.
	 */
	//! @{
	GDAPI Handle Allocator::ReallocateMemory(Handle const memory, SizeTp const allocationSize
#if GD_DEBUG
		, CStr const allocationFilename, Int32 const allocationLineNumber
#endif	// if GD_DEBUG
		)
	{
		return ReallocateMemory(memory, allocationSize, PlatformAllocator::DefaultAlignment
#if GD_DEBUG
			, allocationFilename, allocationLineNumber
#endif	// if GD_DEBUG
		);
	}
	GDAPI Handle Allocator::ReallocateMemory(Handle const memory, SizeTp const allocationSize, SizeTp const alignment
#if GD_DEBUG
		, CStr const allocationFilename, Int32 const allocationLineNumber
#endif	// if GD_DEBUG
		)
	{
		GD_DEBUG_VERIFY(alignment > 1, "Alignment should be greater than 1.");
		GD_DEBUG_VERIFY((alignment & alignment - 1) == 0, "Alignment should power of 2.");
		return _aligned_realloc_dbg(memory, allocationSize, alignment, allocationFilename, allocationLineNumber);
	}
	//! @}

	/*!
	 * Deallocates block of memory.
	 *
	 * @param memory The memory that would be deallocated. If specified block is nullptr then does nothing.
	 * @note This function should not be directly invoked. Use 'GD_DEALLOC' macro instead.
	 */
	GDAPI void Allocator::DeallocateMemory(Handle const memory)
	{
		ScopedConditionalCriticalSection const allocatorMutexLock(g_AllocatorMutex, !PlatformAllocator::IsThreadSafe());

		if (!PlatformAllocator::CanAllocateAligned())
		{
			// Restoring physical address to the memory block. 
			auto const allocatedUnalignedMemory = static_cast<Handle*>(memory)[-1];
			PlatformAllocator::DeallocateUnaligned(allocatedUnalignedMemory);
		}
		else
		{
			// Simply deallocating memory block.
			PlatformAllocator::DeallocateAligned(memory);
		}
	}

	/*!
	 * Returns the length of the allocated block.
	 */
	GDAPI SizeTp Allocator::GetAllocationSize(CHandle const memory)
	{
		return PlatformAllocator::GetAllocationSize(const_cast<Handle>(memory));
	}
	
GD_NAMESPACE_END
