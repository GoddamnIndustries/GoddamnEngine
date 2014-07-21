/// ==========================================================================================
/// Allocator.h: memory allocator interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 12.06.2014 - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_ALLOCATOR_ALLOCATOR
#define GD_CORE_ALLOCATOR_ALLOCATOR

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/TypeTraits.h>
#include <GoddamnEngine/Core/Object/Singleton/Singleton.h>

GD_NAMESPACE_BEGIN

	/// Wrapper around default allocator with support of non-POD types
	class Allocator final
	{
	private:
		GD_CLASS_UNASSIGNABLE(Allocator);
		GD_CLASS_UNCOPIABLE(Allocator);

		GDINL  Allocator() { }
		GDINL ~Allocator() { }
		
	public:
		/// Allocates block of memory with specified size and returns pointer to it.
		/// This function never returns nullptr except specified size is 0.
		GDAPI static handle AllocateMemory(size_t const AllocationSize);

		/// Reallocates block of memory to specified size.
		/// This function never returns nullptr except specified size is 0.
		GDAPI static handle ReallocateMemory(handle const Memory, size_t const AllocationSize);

		/// Deallocates block of memory.
		/// If specified block is nullptr then does nothing.
		GDAPI static void DeallocateMemory(handle const Memory);
	};	// class Allocator

GD_NAMESPACE_END

GDINL void* operator new(size_t const size)
{
	return GD Allocator::AllocateMemory(size);
}

GDINL void  operator delete(void* const pointer)
{
	GD Allocator::DeallocateMemory(pointer);
}

GDINL void* operator new[](size_t const size)
{
	return GD Allocator::AllocateMemory(size);
}

GDINL void  operator delete[](void* const pointer)
{
	GD Allocator::DeallocateMemory(pointer);
}

// #include <GoddamnEngine/Core/Allocator/Allocator.inl>

#endif
