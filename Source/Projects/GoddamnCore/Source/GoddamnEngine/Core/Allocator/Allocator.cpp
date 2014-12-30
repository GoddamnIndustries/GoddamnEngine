/// ==========================================================================================
/// Allocator.cpp: memory allocator implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 12.06.2014 - Created by James Jhuighuy
/// ==========================================================================================

#include <GoddamnEngine/Core/Allocator/Allocator.h>
#include <GoddamnEngine/Core/Diagnostics/Assertion/Assertion.h>

#define GD_USE_JEMALLOC 1

#if (defined(GD_USE_JEMALLOC))
#	define JEMALLOC_MANGLE 1
#	include <jemalloc/jemalloc.h>
#	include <jemalloc/jemalloc_mangle.h>
#else	// if (defined(GD_USE_JEMALLOC))
#	include <malloc.h>
#endif	// if (defined(GD_USE_JEMALLOC))

GD_NAMESPACE_BEGIN
 
	handle Allocator::AllocateMemory(size_t const AllocationSize)
	{
		handle const AllocatedMemory = handle(::malloc(AllocationSize));
		GD_ASSERT(AllocatedMemory != nullptr, "Failed to allocate memory");
		return AllocatedMemory;
	}

	handle Allocator::ReallocateMemory(handle const Memory, size_t const AllocationSize)
	{
		handle const ReallocatedMemory = handle(::realloc(Memory, AllocationSize));
		GD_ASSERT(ReallocatedMemory != nullptr, "Failed to reallocate memory");
		return ReallocatedMemory;
	}

	void Allocator::DeallocateMemory(handle const Memory)
	{
		if (Memory != nullptr) {
			::free(Memory);
		}
	}
	
#if 0	// Code left here just for history purposes.
	void Allocator::ShiftRight(handle _memory, const size_t memorySize, const size_t from, const size_t to, const size_t offset)
	{
		UInt8* memory = (UInt8*)_memory;
		UInt8* result = (UInt8*)Allocator::Allocate(memorySize);

		// Copying front non-shifted data
		memcpy(result, memory, from);	

		// Copying back non-shifted data
		memcpy(result + (to + offset), memory + (to + offset), memorySize - (to + offset));	

		// Copying data, shifted left
		memcpy(result + from, memory + to, offset);

		// Copying data, shifted right
		memcpy(result + (from + offset), memory + from, to - from);

		// Copying result back
		memcpy(memory, result, memorySize);
		
		// Deallocating
		Allocator::Deallocate(result);
	}

	void Allocator::ShiftLeft(handle _memory, const size_t memorySize, const size_t from, const size_t to, const size_t offset)
	{
		UInt8* memory = (UInt8*)_memory;
		UInt8* result = (UInt8*)Allocator::Allocate(memorySize);

		// Copying front non-shifted data
		memcpy(result, memory, from - offset);	

		// Copying back non-shifted data
		memcpy(result + to, memory + to, memorySize - to);	

		// Copying data, shifted left
		memcpy(result + (from - offset), memory + from, to - from);

		// Copying data, shifted right
		memcpy(result + (to - offset), memory + (from - offset), offset);

		// Copying result back
		memcpy(memory, result, memorySize);
		
		// Deallocating
		Allocator::Deallocate(result);
	}
#endif

GD_NAMESPACE_END
