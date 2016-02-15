// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/System/Misc/AllocatorWindows.h
 * File contains Windows allocator interface.
 */
#pragma once
#if !defined(GD_INSIDE_ALLOCATOR_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Core/System/Misc/Allocator.h> instead.
#endif	// if !defined(GD_INSIDE_ALLOCATOR_H)

#include <Windows.h>
#include <crtdbg.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Windows allocator interface.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class PlatformAllocatorWindows : public PlatformAllocatorGeneric
	{
	public:

		/*!
		 * Allocates unaligned block of memory with specified size.
		 * @returns Pointer on the allocated memory.
		 */
		GDINL static Handle AllocateUnaligned(Allocator::SourceInfo const& location, SizeTp const allocationSize)
		{
			struct AllocatorInitializer final : public TNonCopyable
			{
				_CrtMemState InitialMemoryState;
				GDINL AllocatorInitializer()
				{
					ULONG lowFragmentationHeapValue = 2;
					HeapSetInformation(reinterpret_cast<HANDLE>(_get_heap_handle()), HeapCompatibilityInformation, &lowFragmentationHeapValue, sizeof lowFragmentationHeapValue);
					_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_FILE);
					_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
					_CrtMemCheckpoint(&InitialMemoryState);
				}
				GDINL ~AllocatorInitializer()
				{
					_CrtMemDumpAllObjectsSince(&InitialMemoryState);
				}
			};	// struct AllocatorInitializer
			static AllocatorInitializer const allocatorInitializer;
			return allocationSize != 0 ? _malloc_dbg(allocationSize, 0, location.File, location.Line) : nullptr;
		}

		/*!
		 * Deallocates unaligned block of memory.
		 * @param memory The memory that would be deallocated. If specified block is nullptr then does nothing.
		 */
		GDINL static void DeallocateUnaligned(CHandle const memory)
		{
			_free_dbg(const_cast<Handle>(memory), 0);
		}

		/*!
		 * Allocates block of memory with specified size that is aligned by specified value and returns pointer to it.
		 * @returns Pointer on the allocated memory.
		 */
		GDINL static Handle AllocateAligned(Allocator::SourceInfo const& location, SizeTp const allocationSize, SizeTp const alignment)
		{
			return allocationSize != 0 ? _aligned_malloc_dbg(allocationSize, alignment, location.File, location.Line) : nullptr;
		}

		/*!
		 * Deallocates aligned block of memory.
		 * @param memory The memory that would be deallocated. If specified block is nullptr then does nothing.
		 */
		GDINL static void DeallocateAligned(CHandle const memory)
		{
			_aligned_free_dbg(const_cast<Handle>(memory));
		}

	};	// class PlatformAllocatorWindows

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Cross-platform allocator interface.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	using PlatformAllocator = PlatformAllocatorWindows;

GD_NAMESPACE_END
