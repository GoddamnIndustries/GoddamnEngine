// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Definitions/Allocator.h
//! @note This file should be never directly included, please consider using <GoddamnEngine/Include.h> instead.
//! Contains memory allocator interface.
#pragma once
#if !defined(GD_INSIDE_INCLUDE_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Include.h> instead.
#endif	// if !defined(GD_INSIDE_INCLUDE_H)

//#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/Utility.h>
#include <GoddamnEngine/Core/Templates/TypeTraits.h>

#if GD_PLATFORM_API_MICROSOFT && GD_DEBUG && (!defined(GD_ALLOCATOR_DEBUG_LEAKS))
#	define GD_ALLOCATOR_DEBUG_LEAKS GD_TRUE
#endif	// if GD_PLATFORM_API_MICROSOFT && GD_DEBUG && (!defined(GD_ALLOCATOR_DEBUG_LEAKS))
#if (!defined(GD_ALLOCATOR_DEBUG_LEAKS))
#	define GD_ALLOCATOR_DEBUG_LEAKS GD_FALSE
#endif	// if (!defined(GD_ALLOCATOR_DEBUG_LEAKS))

#if GD_ALLOCATOR_DEBUG_LEAKS && !GD_DOCUMENTATION
#	define GD_ALLOCTOR_DEBUG_SPECIFIC(...) __VA_ARGS__
#	define GD_ALLOCTOR_NDEBUG_SPECIFIC(...) 
#else	// if GD_ALLOCATOR_DEBUG_LEAKS
#	define GD_ALLOCTOR_DEBUG_SPECIFIC(...) 
#	define GD_ALLOCTOR_NDEBUG_SPECIFIC(...) __VA_ARGS__
#endif	// if GD_ALLOCATOR_DEBUG_LEAKS

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Allocator class.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	// ------------------------------------------------------------------------------------------
	//! Wrapper around some custom or default memory allocator.
	//! @note No one of classes function should be used manually. Use corresponding macros instead.
	class Allocator final : IUncopiable
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

		struct SourceInfo final : public IUncopiable
		{
			CStr const File;
			int  const Line;
			GDINL SourceInfo(CStr const File, int  const Line) : File(File), Line(Line) {}
			GDINL SourceInfo() : File(nullptr), Line(0) {}
		};	// struct SourceInfo

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Normal allocation functions.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Allocates block of memory with specified size that is aligned by specified value and returns pointer to it.
		//! @param Location Information about place in code, where this allocation accures.
		//! @param AllocationSize Size of required memory in bytes.
		//! @param Alignment Pointer to memory would be aligned by this value.
		//! @returns Pointer on the allocated memory. This function never returns nullptr except specified size is 0.
		//! @note This function should not be directly invoked. Use 'GD_MALLOC'/'GD_MALLOC_ALIGNED' macro instead.
		GDAPI static Handle AllocateMemory(SourceInfo const& Location, SizeTp const AllocationSize, SizeTp const Alignment = DefaultAlignment);

		// ------------------------------------------------------------------------------------------
		//! Deallocates block of memory.
		//! @param Memory Memory that would be deallocated. If specified block is nullptr then does nothing.
		//! @note This function should not be directly invoked. Use 'GD_DEALLOC' macro instead.
		GDAPI static void DeallocateMemory(Handle const Memory);

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Object allocation functions.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Allocates and initializes object of a specified type.
		//! @param Location Information about place in code, where this allocation accures.
		//! @param Arguments Arguments passed to object constructor.
		//! @returns Allocated object.
		//! @note This function should not be directly invoked. Use 'GD_NEW' macro instead.
		template<typename Tp, typename... ArgumentTypes>
		GDINL static Tp* New(SourceInfo const& Location, ArgumentTypes&&... Arguments)
		{
			Tp* const Allocated = reinterpret_cast<Tp*>(AllocateMemory(Location, sizeof(Tp)));
			if (TypeTraits::IsPOD<Tp>::Value == GD_FALSE)
				new (Allocated) Tp(Forward<ArgumentTypes>(Arguments)...);
			return Allocated;
		}

		// ------------------------------------------------------------------------------------------
		//! Allocates and initializes array of objects of a specified type.
		//! @param Location Information about place in code, where this allocation accures.
		//! @param Count Length of an array.
		//! @returns Allocated object.
		//! @note This function should not be directly invoked. Use 'GD_NEW_ARRAY' macro instead.
		template<typename Tp>
		GDINL static Tp* NewArray(SourceInfo const& Location, SizeTp const Count)
		{
			if (TypeTraits::IsPOD<Tp>::Value == GD_FALSE)
			{
				if (Count != 0)
				{
					UInt8* const Allocated = reinterpret_cast<UInt8*>(AllocateMemory(Location, sizeof(Tp) * Count + sizeof(SizeTp)));
					*reinterpret_cast<SizeTp*>(Allocated) = Count;
					Tp* const ArrayStart = reinterpret_cast<Tp*>(Allocated + sizeof(SizeTp));
					for (SizeTp Cnt = 0; Cnt < Count; ++Cnt)
						new (&ArrayStart[Cnt]) Tp();
					return ArrayStart;
				}
				return nullptr;
			}
			return reinterpret_cast<Tp*>(AllocateMemory(GD_ALLOCTOR_DEBUG_SPECIFIC(File, Line, ) sizeof(Tp) * Count));
		}

		// ------------------------------------------------------------------------------------------
		//! Wrapped operator 'delete'.
		//! @param Memory Memory that would be deallocated. If specified block is nullptr then does nothing.
		//! @note This function should not be directly invoked. Use 'GD_DELETE' macro instead.
		template<typename Tp>
		GDINL static void Delete(Tp* const Memory)
		{
			if (!TypeTraits::IsPOD<Tp>::Value && (Memory != nullptr))
				Memory->~Tp();
			DeallocateMemory(Memory);
		}

		// ------------------------------------------------------------------------------------------
		//! Wrapped operator 'delete[]'.
		//! @param Array Memory that would be deallocated. If specified block is nullptr then does nothing.
		//! @note This function should not be directly invoked. Use 'GD_DELETE_ARRAY' macro instead.
		template<typename Tp>
		GDINL static void DeleteArray(Tp* const Array)
		{
			if (TypeTraits::IsPOD<Tp>::Value == GD_FALSE)
			{
				if (Array != nullptr)
				{
					UInt8* const Allocated = reinterpret_cast<UInt8*>(Array) -sizeof(SizeTp);
					SizeTp const Count = *reinterpret_cast<SizeTp*>(Allocated);
					for (SizeTp Cnt = 0; Cnt < Count; ++Cnt)
						Array[Cnt].~Tp();
					DeallocateMemory(Allocated);
				}
			}
			else
			{
				DeallocateMemory(Array);
			}
		}
	};	// class Allocator

GD_NAMESPACE_END

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Wrapper macros.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ------------------------------------------------------------------------------------------
//! Allocates block of memory with specified size that is aligned by default value and returns pointer to it.
//! @param AllocationSize Size of required memory in bytes.
#define GD_MALLOC(AllocationSize)	(GD::Allocator::AllocateMemory(GD::Allocator::SourceInfo(__FILE__, __LINE__), (AllocationSize)))

// ------------------------------------------------------------------------------------------
//! Allocates block of memory with specified size that is aligned by specified value and returns pointer to it.
//! @param AllocationSize Size of required memory in bytes.
//! @param CustomAlignment Pointer to memory would be aligned by this value.
#define GD_MALLOC_ALIGNED(AllocationSize, CustomAlignment)	(GD::Allocator::SourceInfo(__FILE__, __LINE__), (AllocationSize), (CustomAlignment)))

// ------------------------------------------------------------------------------------------
//! Wrapped operator 'new'.
//! @param Tp Type to allocate.
//! @param ... Constructor arguments.
#define GD_NEW(Tp, ...)				(GD::Allocator::New<Tp>(GD::Allocator::SourceInfo(__FILE__, __LINE__), ##__VA_ARGS__))

// ------------------------------------------------------------------------------------------
//! Wrapped operator 'new[]'.
//! @param Tp Type to allocate.
//! @param Count Length of an array.
#define GD_NEW_ARRAY(Tp, Count)		(GD::Allocator::NewArray<Tp>(GD::Allocator::SourceInfo(__FILE__, __LINE__) Count))

// ------------------------------------------------------------------------------------------
//! Deallocates block of memory.
//! @param Memory Memory that would be deallocated. If specified block is nullptr then does nothing.
#define GD_DEALLOC(Memory)			(GD::Allocator::DeallocateMemory((Memory)))

// ------------------------------------------------------------------------------------------
//! Wrapped operator 'delete'.
//! @param Memory Memory that would be deallocated. If specified block is nullptr then does nothing.
#define GD_DELETE(Memory)			(GD::Allocator::Delete((Memory)))

// // I hope, this can prevent using of default operator new.
// // But this lines may cause conflicts with some external libraries.
// //! @todo Find better way to disable 'new'/'delete' operators in our code.
// #if !GD_DOCUMENTATION
// #	define new		"Please, consider using 'GD_NEW'/'GD_NEW_ARRAY' macro instead."
// #	define delete	"Please, consider using 'GD_DELETE'/'GD_DELETE_ARRAY' macro instead."
// #endif	// if !GD_DOCUMENTATION
