// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
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

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Wrapper around some custom or default memory allocator.
	//! @note No one of classes function should be used manually. Use corresponding macros instead.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class Allocator final : TNonCreatable
	{
	public:
#if !GD_DOCUMENTATION

#	if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64 && (!GD_PLATFORM_IOS)
		SizeTp static const DefaultAlignment = 16;
#	else	// if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64 && (!GD_PLATFORM_IOS)
		SizeTp static const DefaultAlignment = 8;
#	endif	// if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64 && (!GD_PLATFORM_IOS)

#else	// if !GD_DOCUMENTATION
		
		//! Represents default memory alignment value.
		SizeTp static const DefaultAlignment;

#endif	// if !GD_DOCUMENTATION

		struct SourceInfo final : public TNonCopyable
		{
			CStr const File;
			int  const Line;
			GDINL SourceInfo(CStr const file, int const line) : File(file), Line(line) {}
			GDINL SourceInfo() : File(nullptr), Line(0) {}
		};	// struct SourceInfo

	public:

		// ------------------------------------------------------------------------------------------
		// Normal allocation functions.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Allocates block of memory with specified size that is aligned by specified value and returns pointer to it.
		 *
		 * @param allocationSize Size of required memory in bytes.
		 * @param alignment pointer to memory would be aligned by this value.
		 *
		 * @returns Pointer on the allocated memory. This function never returns nullptr except specified size is 0.
		 * @note This function should not be directly invoked. Use 'GD_MALLOC'/'GD_MALLOC_ALIGNED' macro instead.
		 */
		GDAPI static Handle AllocateMemory(SourceInfo const& location, SizeTp const allocationSize, SizeTp const alignment = DefaultAlignment);
		
		/*!
		 * Deallocates block of memory.
		 *
		 * @param memory The memory that would be deallocated. If specified block is nullptr then does nothing.
		 * @note This function should not be directly invoked. Use 'GD_DEALLOC' macro instead.
		 */
		GDAPI static void DeallocateMemory(CHandle const memory);

		/*!
		 * Deallocates block of memory.
		 * @param memory The memory that would be deallocated. If specified block is nullptr then does nothing.
		 */
		#define GD_DEALLOC(memory) (Allocator::DeallocateMemory((memory)))

	};	// class Allocator

GD_NAMESPACE_END

enum gd_operator_new_delete_t { gd_operator_new_delete };

/*!
 * Allocates block of memory with specified size that is aligned by specified value and returns pointer to it.
 * @param allocationSize Size of required memory in bytes.
 */
#define GD_MALLOC(allocationSize) (Allocator::AllocateMemory(Allocator::SourceInfo(__FILE__, __LINE__), (allocationSize)))

/*!
 * Allocates block of memory with specified size that is aligned by specified value and returns pointer to it.
 *
 * @param allocationSize Size of required memory in bytes.
 * @param alignment pointer to memory would be aligned by this value.
 */
#define GD_MALLOC_ALIGNED(allocationSize, customAlignment)	(Allocator::AllocateMemory(Allocator::SourceInfo(__FILE__, __LINE__), (allocationSize), (customAlignment)))

/*!
 * Allocates and initializes array of objects of a specified type.
 */
#if GD_DEBUG || 1
#	define gd_new new(gd_operator_new_delete, __FILE__, __LINE__)
#else	// if GD_DEBUG
#	define gd_new new(gd_operator_new_delete)
#endif	// if GD_DEBUG

/*!
 * Deallocates and deinitializes object of a specified type.
 */
#define gd_delete delete 

#if GD_DEBUG || 1
inline void* operator new(size_t const size, gd_operator_new_delete_t, char const* file, int const line)
{ 
	return GD::Allocator::AllocateMemory(GD::Allocator::SourceInfo(file, line), size);
}
inline void* operator new[](size_t const size, gd_operator_new_delete_t, char const* file, int const line)
{
	return GD::Allocator::AllocateMemory(GD::Allocator::SourceInfo(file, line), size);
}
#endif	// if GD_DEBUG

inline void operator delete(void* ptr)
{
	GD::Allocator::DeallocateMemory(ptr);
}
inline void operator delete[](void* ptr)
{
	GD::Allocator::DeallocateMemory(ptr);
}
