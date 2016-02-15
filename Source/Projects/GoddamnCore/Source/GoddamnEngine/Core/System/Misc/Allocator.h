// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/System/Misc/Allocator.h
 * File contains generic allocator interface.
 */
#pragma once
#define GD_INSIDE_ALLOCATOR_H

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/System/Platform.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Generic allocator interface.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class PlatformAllocatorGeneric : public TNonCreatable
	{
	public:

		/*!
		 * Allocates unaligned block of memory with specified size.
		 * @returns Pointer on the allocated memory.
		 */
		GDINL static Handle AllocateUnaligned(Allocator::SourceInfo const& location, SizeTp const allocationSize);

		/*!
		 * Deallocates unaligned block of memory.
		 * @param memory The memory that would be deallocated. If specified block is nullptr then does nothing.
		 */
		GDINL static void DeallocateUnaligned(CHandle const memory);

		/*!
		 * Allocates block of memory with specified size that is aligned by specified value and returns pointer to it.
		 * @returns Pointer on the allocated memory.
		 */
		GDINL static Handle AllocateAligned(Allocator::SourceInfo const& location, SizeTp const allocationSize, SizeTp const alignment);

		/*!
		 * Deallocates aligned block of memory.
		 * @param memory The memory that would be deallocated. If specified block is nullptr then does nothing.
		 */
		GDINL static void DeallocateAligned(CHandle const memory);

	};	// class PlatformAllocatorGeneric

GD_NAMESPACE_END

#include GD_PLATFORM_INCLUDE(GoddamnEngine/Core/System/Misc, Allocator)
#undef GD_INSIDE_DIRECTORY_H

GD_NAMESPACE_BEGIN

	/*!
	 * Allocates unaligned block of memory with specified size.
	 * @returns Pointer on the allocated memory.
	 */
	GDINL Handle PlatformAllocatorGeneric::AllocateUnaligned(Allocator::SourceInfo const& location, SizeTp const allocationSize)
	{
		GD_NOT_USED(location);
		return allocationSize != 0 ? malloc(allocationSize) : nullptr;
	}

	/*!
	 * Deallocates block of memory.
	 * @param memory The memory that would be deallocated. If specified block is nullptr then does nothing.
	 */
	GDINL void PlatformAllocatorGeneric::DeallocateUnaligned(CHandle const memory)
	{
		free(const_cast<Handle>(memory));
	}

	/*!
	 * Allocates block of memory with specified size that is aligned by specified value and returns pointer to it.
	 * @returns Pointer on the allocated memory.
	 */
	GDINL Handle PlatformAllocatorGeneric::AllocateAligned(Allocator::SourceInfo const& location, SizeTp const allocationSize, SizeTp const alignment)
	{
		if (allocationSize != 0)
		{
			GD_DEBUG_ASSERT(alignment >= sizeof(SizeTp), "Use unaligned allocation for this alignment.");
			auto const rawAddress = reinterpret_cast<SizeTp>(PlatformAllocator::AllocateUnaligned(location, allocationSize + alignment));
			if (rawAddress != 0)
			{
				auto const adjustment = alignment - (rawAddress & alignment - 1);
				auto const alignedAddress = rawAddress + adjustment;

				*reinterpret_cast<SizeTp*>(alignedAddress - sizeof(SizeTp)) = adjustment;
				return reinterpret_cast<Handle>(alignedAddress);
			}
		}
		return nullptr;
	}

	/*!
	 * Deallocates aligned block of memory.
	 * @param memory The memory that would be deallocated. If specified block is nullptr then does nothing.
	 */
	GDINL void PlatformAllocatorGeneric::DeallocateAligned(CHandle const memory)
	{
		if (memory != nullptr)
		{
			auto const alignedAddress = reinterpret_cast<SizeTp>(memory);
			auto const adjustment = *reinterpret_cast<SizeTp*>(alignedAddress - sizeof(SizeTp));

			auto const rawAddress = alignedAddress - adjustment;
			PlatformAllocator::DeallocateUnaligned(reinterpret_cast<CHandle>(rawAddress));
		}
	}

GD_NAMESPACE_END
