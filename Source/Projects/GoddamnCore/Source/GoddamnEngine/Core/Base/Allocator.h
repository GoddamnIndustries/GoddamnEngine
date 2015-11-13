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
#include <GoddamnEngine/Core/Utility.h>
#include <GoddamnEngine/Core/TypeTraits.h>

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
			GDINL SourceInfo(CStr const file, int  const line) : File(file), Line(line) {}
			GDINL SourceInfo() : File(nullptr), Line(0) {}
		};	// struct SourceInfo

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Normal allocation functions.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Allocates block of memory with specified size that is aligned by specified value and returns pointer to it.
		//! @param location Information about place in code, where this allocation occurs.
		//! @param allocationSize Size of required memory in bytes.
		//! @param alignment pointer to memory would be aligned by this value.
		//! @returns pointer on the allocated memory. This function never returns nullptr except specified size is 0.
		//! @note This function should not be directly invoked. Use 'GD_MALLOC'/'GD_MALLOC_ALIGNED' macro instead.
		GDAPI static Handle AllocateMemory(SourceInfo const& location, SizeTp const allocationSize, SizeTp const alignment = DefaultAlignment);

		// ------------------------------------------------------------------------------------------
		//! Deallocates block of memory.
		//! @param memory The memory that would be deallocated. If specified block is nullptr then does nothing.
		//! @note This function should not be directly invoked. Use 'GD_DEALLOC' macro instead.
		GDAPI static void DeallocateMemory(CHandle const memory);

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Object allocation functions.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Allocates and initializes object of a specified type.
		//! @param location Information about place in code, where this allocation happens.
		//! @param arguments The arguments passed to object constructor.
		//! @returns Allocated object.
		//! @note This function should not be directly invoked. Use 'GD_NEW' macro instead.
		template<typename Tp, typename... ArgumentTypes>
		GDINL static Tp* New(SourceInfo const& location, ArgumentTypes&&... arguments)
		{
			Tp* const allocated = reinterpret_cast<Tp*>(AllocateMemory(location, sizeof(Tp)));
			if (TypeTraits::IsPOD<Tp>::Value == GD_FALSE)
				new (allocated) Tp(Forward<ArgumentTypes>(arguments)...);
			return allocated;
		}

		// ------------------------------------------------------------------------------------------
		//! Allocates and initializes array of objects of a specified type.
		//! @param location Information about place in code, where this allocation happens.
		//! @param count m_Length of an array.
		//! @returns Allocated object.
		//! @note This function should not be directly invoked. Use 'GD_NEW_ARRAY' macro instead.
		template<typename Tp>
		GDINL static Tp* NewArray(SourceInfo const& location, SizeTp const count)
		{
			if (TypeTraits::IsPOD<Tp>::Value == GD_FALSE)
			{
				if (count != 0)
				{
					UInt8* const allocated = reinterpret_cast<UInt8*>(AllocateMemory(location, sizeof(Tp) * count + sizeof(SizeTp)));
					*reinterpret_cast<SizeTp*>(allocated) = count;
					Tp* const arrayStart = reinterpret_cast<Tp*>(allocated + sizeof(SizeTp));
					for (SizeTp cnt = 0; cnt < count; ++cnt)
						new (&arrayStart[cnt]) Tp();
					return arrayStart;
				}
				return nullptr;
			}
			return reinterpret_cast<Tp*>(AllocateMemory(location, sizeof(Tp) * count));
		}

		// ------------------------------------------------------------------------------------------
		//! Wrapped operator 'delete'.
		//! @param memory The memory that would be deallocated. If specified block is nullptr then does nothing.
		//! @note This function should not be directly invoked. Use 'GD_DELETE' macro instead.
		template<typename Tp>
		GDINL static void Delete(Tp* const memory)
		{
			if (!TypeTraits::IsPOD<Tp>::Value && (memory != nullptr))
				memory->~Tp();
			DeallocateMemory(memory);
		}

		// ------------------------------------------------------------------------------------------
		//! Wrapped operator 'delete[]'.
		//! @param array memory that would be deallocated. If specified block is nullptr then does nothing.
		//! @note This function should not be directly invoked. Use 'GD_DELETE_ARRAY' macro instead.
		template<typename Tp>
		GDINL static void DeleteArray(Tp* const array)
		{
			if (TypeTraits::IsPOD<Tp>::Value == GD_FALSE)
			{
				if (array != nullptr)
				{
					UInt8* const allocated = reinterpret_cast<UInt8*>(array) - sizeof(SizeTp);
					SizeTp const count = *reinterpret_cast<SizeTp*>(allocated);
					for (SizeTp cnt = 0; cnt < count; ++cnt)
						array[cnt].~Tp();
					DeallocateMemory(allocated);
				}
			}
			else
			{
				DeallocateMemory(array);
			}
		}
	};	// class Allocator

GD_NAMESPACE_END

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Wrapper macros.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ------------------------------------------------------------------------------------------
//! Allocates block of memory with specified size that is aligned by default value and returns pointer to it.
//! @param allocationSize Size of required memory in bytes.
#define GD_MALLOC(allocationSize)	(GD::Allocator::AllocateMemory(GD::Allocator::SourceInfo(__FILE__, __LINE__), (allocationSize)))

// ------------------------------------------------------------------------------------------
//! Allocates block of memory with specified size that is aligned by specified value and returns pointer to it.
//! @param allocationSize Size of required memory in bytes.
//! @param customAlignment pointer to memory would be aligned by this value.
#define GD_MALLOC_ALIGNED(allocationSize, customAlignment)	(GD::Allocator::SourceInfo(__FILE__, __LINE__), (allocationSize), (customAlignment)))

// ------------------------------------------------------------------------------------------
//! Wrapped operator 'new'.
//! @param Tp Type to allocate.
//! @param ... Constructor arguments.
#define GD_NEW(Tp, ...)				(GD::Allocator::New<Tp>(GD::Allocator::SourceInfo(__FILE__, __LINE__), ##__VA_ARGS__))

// ------------------------------------------------------------------------------------------
//! Wrapped operator 'new[]'.
//! @param Tp Type to allocate.
//! @param count m_Length of an array.
#define GD_NEW_ARRAY(Tp, count)		(GD::Allocator::NewArray<Tp>(GD::Allocator::SourceInfo(__FILE__, __LINE__), count))

// ------------------------------------------------------------------------------------------
//! Deallocates block of memory.
//! @param memory The memory that would be deallocated. If specified block is nullptr then does nothing.
#define GD_DEALLOC(memory)			(GD::Allocator::DeallocateMemory((memory)))

// ------------------------------------------------------------------------------------------
//! Wrapped operator 'delete'.
//! @param memory The memory that would be deallocated. If specified block is nullptr then does nothing.
#define GD_DELETE(memory)			(GD::Allocator::Delete((memory)))

// ------------------------------------------------------------------------------------------
//! Wrapped operator 'delete'.
//! @param memory The memory that would be deallocated. If specified block is nullptr then does nothing.
#define GD_DELETE_ARRAY(memory)		(GD::Allocator::DeleteArray((memory)))

// // I hope, this can prevent using of default operator new.
// // But this lines may cause conflicts with some external libraries.
// //! @todo Find better way to disable 'new'/'delete' operators in our code.
// #if !GD_DOCUMENTATION
// #	define new		"Please, consider using 'GD_NEW'/'GD_NEW_ARRAY' macro instead."
// #	define delete	"Please, consider using 'GD_DELETE'/'GD_DELETE_ARRAY' macro instead."
// #endif	// if !GD_DOCUMENTATION

#if !GD_DOCUMENTATION
GD_NAMESPACE_BEGIN

	template<typename TKey, typename TValue>
	GDINL static Pair<TKey, TValue>* AllocatePair(TKey&& Key, TValue&& Value)
	{
		typedef Pair<TKey, TValue> PairType;
		return GD_NEW(PairType, Forward<TKey>(Key), Forward<TValue>(Value));
	}
	template<typename TKey, typename TValue>
	GDINL static Pair<TKey, TValue>* AllocatePair(TKey const& Key, TValue const& Value)
	{
		typedef Pair<TKey, TValue> PairType;
		return GD_NEW(PairType, Key, Value);
	}

GD_NAMESPACE_END
#endif	// if !GD_DOCUMENTATION
