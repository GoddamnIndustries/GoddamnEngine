//////////////////////////////////////////////////////////////////////////
/// Allocator.hh: memory allocator interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 12.06.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_CORE_ALLOCATOR_ALLOCATOR
#define GD_CORE_ALLOCATOR_ALLOCATOR

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/TypeTraits.hh>
#include <GoddamnEngine/Core/Object/Singleton/Singleton.hh>

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

		/*
		/// Allocates block of memory with specified block type and number of elements and returns pointer to it.
		/// @note This function never returns nullptr except specified count is 0.
		template<typename BlockType>
		inline static BlockType* AllocateMemory(size_t const ElementsCount);
		*/

		/// Reallocates block of memory to specified size.
		/// This function never returns nullptr except specified size is 0.
		GDAPI static handle ReallocateMemory(handle const Memory, size_t const AllocationSize);

		/*
		/// Reallocates block of memory with specified block type to specified elements count.
		/// This function never returns nullptr except specified count is 0.
		template<typename BlockType>
		inline static BlockType* ReallocateMemory(BlockType* const Memory, size_t const OldElementsCount, size_t const NewElementsCount);
		*/

		/// Deallocates block of memory.
		/// If specified block is nullptr then does nothing.
		GDAPI static void DeallocateMemory(handle const Memory);

		/*
		/// Deallocates block of memory with specified block type
		/// If specified block is nullptr then does nothing
		template<typename BlockType>
		inline static void DeallocateMemory(BlockType* const Memory, size_t const NewElementsCount);
		*/
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
