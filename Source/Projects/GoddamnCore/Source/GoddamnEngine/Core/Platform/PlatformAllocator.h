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
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/Singleton.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Memory allocator.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL IPlatformAllocator : public Singleton<IPlatformAllocator>
	{
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
		GDINT virtual bool MemoryAllocate(Handle& allocationPointer, SizeTp const allocationSizeBytes) GD_PURE_VIRTUAL;

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
		GDINT virtual bool MemoryFreeAlignedDebug(Handle const allocationPointer) GD_PURE_VIRTUAL
		{
			return MemoryFreeAligned(allocationPointer);
		}
#endif	// if GD_DEBUG
	};	// class IPlatformAllocator

	/*!
	 * Helper function for memory allocation.
	 * @note Use @c GD_MALLOC instead.
	 */
#if GD_DEBUG
	GDINT static Handle GD_PLATFORM_WRAPPER AllocateMemory(SizeTp const allocationSizeBytes, CStr const allocationFilename, UInt32 const allocationLineNumber, bool* const resultPtr = nullptr)
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
	GDINT static Handle GD_PLATFORM_WRAPPER AllocateMemory(SizeTp const allocationSizeBytes, bool* const resultPtr = nullptr)
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

	/*!
	 * Helper function for memory deallocation.
	 * @note Use @c GD_MALLOC instead.
	 */
#if GD_DEBUG
	GDINT static void GD_PLATFORM_WRAPPER FreeMemory(Handle const allocationPointer, bool* const resultPtr = nullptr)
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
	GDINT static void GD_PLATFORM_WRAPPER FreeMemory(Handle const allocationPointer, bool* const resultPtr = nullptr)
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

GD_NAMESPACE_END

// ------------------------------------------------------------------------------------------
// Memory allocation with alignment and leak detection.
// ------------------------------------------------------------------------------------------

/*!
 * Allocates block of memory with specified size that is aligned by default value and returns pointer to it.
 * @param allocationSize Size of required memory in bytes.
 */
#if GD_DEBUG
#	define GD_MALLOC(allocationSize, ...) (GD::AllocateMemory((allocationSize), __FILE__, __LINE__, ##__VA_ARGS__))
#else	// if GD_DEBUG
#	define GD_MALLOC(allocationSize, ...) (GD::AllocateMemory((allocationSize), ##__VA_ARGS__))
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
#define GD_MALLOC_ARRAY_T(TType, length) static_cast<TType*>(GD_MALLOC((length) * sizeof(TType)))

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

/*!
 * Deallocates block of memory.
 * @param memory The memory that would be deallocated. If specified block is nullptr then does nothing.
 */
#define GD_FREE(memory) (GD::FreeMemory((memory)))

// ------------------------------------------------------------------------------------------
// Memory allocation operators with alignment and leak detection.
// ------------------------------------------------------------------------------------------

#include <new>

/*!
 * Allocates and initializes array of objects of a specified type.
 */
#if GD_DEBUG
#	define gd_new new(gd_operator_new_delete, __FILE__, __LINE__)
#else	// if GD_DEBUG
#	define gd_new new(gd_operator_new_delete)
#endif	// if GD_DEBUG

/*!
 * Deallocates and deinitializes object of a specified type.
 */
#define gd_delete delete 

enum gd_operator_new_delete_t { gd_operator_new_delete };

// new T && new T[N]
inline void* operator new(size_t const size, gd_operator_new_delete_t
#if GD_DEBUG
	, char const* file, int const line
#endif	// if GD_DEBUG
	)
{
	return GD::AllocateMemory(size
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
	return GD::AllocateMemory(size
#if GD_DEBUG
		, file, line
#endif	// if GD_DEBUG
	);
}

// delete T && delete[] T
inline void operator delete(void* ptr) noexcept
{
	GD::FreeMemory(ptr);
}
inline void operator delete[](void* ptr) noexcept
{
	GD::FreeMemory(ptr);
}
