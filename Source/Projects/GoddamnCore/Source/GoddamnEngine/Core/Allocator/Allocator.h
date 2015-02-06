/// ==========================================================================================
/// Allocator.h: memory allocator interface.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_ALLOCATOR_ALLOCATOR
#define GD_CORE_ALLOCATOR_ALLOCATOR

#include <GoddamnEngine/Include.h>

GD_NAMESPACE_BEGIN

	/// Wrapper around default allocator.
	class Allocator final
	{
	private:
		GD_CLASS_UNASSIGNABLE(Allocator);
		GD_CLASS_UNCOPIABLE(Allocator);

		GDINL  Allocator() { }
		GDINL ~Allocator() { }
		
	public:
		/// @brief Allocates block of memory with specified size and returns pointer to it.
		/// @param AllocationSize Size of required memory in bytes.
		/// @returns Pointer on the allocated memory. This function never returns nullptr except specified size is 0.
		GDAPI static Handle AllocateMemory(size_t const AllocationSize);

		/// @brief Allocates block of memory with specified size that is aligned by specified value and returns pointer to it.
		/// @param AllocationSize Size of required memory in bytes.
		/// @param Alignment Pointer to memory would be aligned by this value.
		/// @returns Pointer on the allocated memory. This function never returns nullptr except specified size is 0.
		GDAPI static Handle AllocateMemoryAligned(size_t const AllocationSize, size_t const Alignment);

		/// @brief Reallocates block of memory to specified size.
		/// @param Memory Memory that would be reallocated.
		/// @param AllocationSize New size of required memory in bytes.
		/// @returns Pointer on the reallocated memory. This function never returns nullptr except specified size is 0.
		GDAPI static Handle ReallocateMemory(Handle const Memory, size_t const AllocationSize);

		/// @brief Deallocates block of memory.
		/// @param Memory Memory that would be deallocated. If specified block is nullptr then does nothing.
		GDAPI static void DeallocateMemory(Handle const Memory);

	};	// class Allocator

GD_NAMESPACE_END

GDINL void* operator new(size_t const Size)
{
	return GD::Allocator::AllocateMemory(Size);
}

GDINL void  operator delete(void* const Pointer)
{
	GD::Allocator::DeallocateMemory(Pointer);
}

GDINL void* operator new[](size_t const Size)
{
	return GD::Allocator::AllocateMemory(Size);
}

GDINL void  operator delete[](void* const Pointer)
{
	GD::Allocator::DeallocateMemory(Pointer);
}

#endif
