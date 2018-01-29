// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Base/Allocator.h
 * @note This file should be never directly included, please consider using <GoddamnEngine/Include.h> instead.
 * Contains memory allocator interface.
 */
#pragma once
#if !defined(GD_INSIDE_INCLUDE_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Include.h> instead.
#endif	// if !defined(GD_INSIDE_INCLUDE_H)

#include <new>

// **------------------------------------------------------------------------------------------**
// ******                                GD_DEALLOC macro.                                 ******
// **------------------------------------------------------------------------------------------**

/*!
 * Deallocates block of memory.
 * @param memory The memory that would be deallocated. If specified block is nullptr then does nothing.
 */
#define GD_FREE(memory) (GD::Allocator::DeallocateMemory((memory)))

// **------------------------------------------------------------------------------------------**
// ******                                 GD_MALLOC macro.                                 ******
// **------------------------------------------------------------------------------------------**

/*!
 * Allocates block of memory with specified size that is aligned by default value and returns pointer to it.
 * @param allocationSize Size of required memory in bytes.
 */
#if GD_DEBUG
#	define GD_MALLOC(allocationSize) (GD::Allocator::AllocateMemory((allocationSize), __FILE__, __LINE__))
#else	// if GD_DEBUG
#	define GD_MALLOC(allocationSize) (GD::Allocator::AllocateMemory((allocationSize)))
#endif	// if GD_DEBUG

/*!
 * Allocates block of memory for a single instance of type.
 * @param TType Type of element.
 */
#define GD_MALLOC_T(TType) static_cast<TType*>(GD_MALLOC(sizeof(TType)))

/*!
 * Allocates block of memory for a array of instances of type.
 *
 * @param TType Type of element.
 * @param Length The length of the array.
 */
#define GD_MALLOC_ARRAY_T(TType, length) static_cast<TType*>(GD_MALLOC(length * sizeof(TType)))

/*!
 * Allocates block of memory with specified size that is aligned by specified value and returns pointer to it.
 *
 * @param allocationSize Size of required memory in bytes.
 * @param alignment pointer to memory would be aligned by this value.
 */
#if GD_DEBUG
#	define GD_MALLOC_ALIGNED(allocationSize, alignment) (GD::Allocator::AllocateMemory((allocationSize), (alignment), __FILE__, __LINE__))
#else	// if GD_DEBUG
#	define GD_MALLOC_ALIGNED(allocationSize, alignment) (GD::Allocator::AllocateMemory((allocationSize), (alignment)))
#endif	// if GD_DEBUG

// **------------------------------------------------------------------------------------------**
// ******                                 GD_REALLOC macro.                                ******
// **------------------------------------------------------------------------------------------**

/*!
 * Reallocates block of memory with specified size that is aligned by default value and returns pointer to it.
 * @param memory Block of memory to reallocate.
 * @param allocationSize Size of required memory in bytes.
 */
#if GD_DEBUG
#	define GD_REALLOC(memory, allocationSize) (GD::Allocator::ReallocateMemory((memory), (allocationSize), __FILE__, __LINE__))
#else	// if GD_DEBUG
#	define GD_REALLOC(memory, allocationSize) (GD::Allocator::ReallocateMemory((memory), (allocationSize)))
#endif	// if GD_DEBUG

/*!
 * Allocates block of memory for a array of instances of type.
 *
 * @param TType Type of element.
 * @param Length The length of the array.
 */
#define GD_REALLOC_ARRAY_T(TType, memory, length) static_cast<TType*>(GD_REALLOC(memory, length * sizeof(TType)))

// **------------------------------------------------------------------------------------------**
// ******                              Operator new/delete.                                ******
// **------------------------------------------------------------------------------------------**

/*!
 * Deallocates and deinitializes object of a specified type.
 */
#define gd_delete delete 

/*!
 * Allocates and initializes array of objects of a specified type.
 */
#if GD_DEBUG
#	define gd_new new(gd_operator_new_delete, __FILE__, __LINE__)
#else	// if GD_DEBUG
#	define gd_new new(gd_operator_new_delete)
#endif	// if GD_DEBUG

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Wrapper around some custom or default memory allocator.
	//! @note No one of classes function should be used manually. Use corresponding macros instead.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class Allocator 
	{
	public:

		/*!
		 * Allocates block of memory with specified size that is aligned by specified value and returns pointer to it.
		 *
		 * @param allocationSize Size of required memory in bytes.
		 * @param alignment pointer to memory would be aligned by this value.
		 *
		 * @returns Pointer on the allocated memory. This function never returns nullptr.
		 * @note This function should not be directly invoked. Use 'GD_MALLOC'/'GD_MALLOC_ALIGNED' macro instead.
		 */
		//! @{
		GDAPI static Handle AllocateMemory(SizeTp const allocationSize
#if GD_DEBUG
			, CStr const allocationFilename, Int32 const allocationLineNumber
#endif	// if GD_DEBUG
			);
		GDAPI static Handle AllocateMemory(SizeTp const allocationSize, SizeTp const alignment
#if GD_DEBUG
			, CStr const allocationFilename, Int32 const allocationLineNumber
#endif	// if GD_DEBUG
			);
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
		GDAPI static Handle ReallocateMemory(Handle const memory, SizeTp const allocationSize
#if GD_DEBUG
			, CStr const allocationFilename, Int32 const allocationLineNumber
#endif	// if GD_DEBUG
			);
		GDAPI static Handle ReallocateMemory(Handle const memory, SizeTp const allocationSize, SizeTp const alignment
#if GD_DEBUG
			, CStr const allocationFilename, Int32 const allocationLineNumber
#endif	// if GD_DEBUG
			);
		//! @}

		/*!
		 * Deallocates block of memory.
		 *
		 * @param memory The memory that would be deallocated. If specified block is nullptr then does nothing.
		 * @note This function should not be directly invoked. Use 'GD_DEALLOC' macro instead.
		 */
		GDAPI static void DeallocateMemory(Handle const memory);

		/*!
		 * Returns the length of the allocated block.
		 */
		GDAPI static SizeTp GetAllocationSize(CHandle const memory);

	};	// class Allocator

	static constexpr auto InlineAllocatorCapacityTiny = 1024;
	static constexpr auto InlineAllocatorCapacityNormal = 1024 * 8;
	static constexpr auto InlineAllocatorCapacityHuge = 1024 * 16;

	using HeapAllocator = Allocator;
	template<int>
	using InlineDoubleEndedStackAllocator = Allocator;
	template<int>
	using InlineStackAllocator = Allocator;
	template<int, int>
	using InlineFixedSizedPoolAllocator = Allocator;

GD_NAMESPACE_END

enum gd_operator_new_delete_t { gd_operator_new_delete };

// new T && new T[N]
inline void* operator new(size_t const size, gd_operator_new_delete_t
#if GD_DEBUG
	, char const* file, int const line
#endif	// if GD_DEBUG
	)
{ 
	return GD::Allocator::AllocateMemory(size
#if GD_DEBUG
		, file, line
#endif	// if GD_DEBUG
		);
}
inline void* operator new[](size_t const size, gd_operator_new_delete_t
#if GD_DEBUG
	, char const* file, int const line
#endif	// if GD_DEBUG
	)
{
	return GD::Allocator::AllocateMemory(size
#if GD_DEBUG
		, file, line
#endif	// if GD_DEBUG
		);
}

// delete T && delete[] T
inline void operator delete(void* ptr) noexcept
{
	GD::Allocator::DeallocateMemory(ptr);
}
inline void operator delete[](void* ptr) noexcept
{
	GD::Allocator::DeallocateMemory(ptr);
}
