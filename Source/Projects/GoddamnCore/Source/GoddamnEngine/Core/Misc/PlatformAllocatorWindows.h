// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file
 * Windows allocator.
 */
#pragma once
#if !defined(GD_INSIDE_ALLOCATOR_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Core/Misc/Allocator.h> instead.
#endif	// if !defined(GD_INSIDE_ALLOCATOR_H)

#if GD_DEBUG
#	include <crtdbg.h>
#else	// if GD_DEBUG
#	include <GoddamnEngine/Core/Misc/PlatformAllocatorTBB.h>
#endif	// if GD_DEBUG

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Windows allocator interface.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
#if GD_DEBUG
	class PlatformAllocatorWindows : public PlatformAllocatorGeneric
	{
	public:
		
		/*!
		 * Initializes current allocator.
		 */
		GDINL static void OnInitializeAllocator()
		{
			_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_FILE);
			_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
		}

		/*!
		 * Deinitializes current allocator.
		 */
		GDINL static void OnDeinitializeAllocator()
		{
			_CrtMemDumpAllObjectsSince(nullptr);
		}

		/*!
		 * Returns true if this allocator is thread-safe.
		 */
		GDINL constexpr static bool IsThreadSafe()
		{
			return true;
		}

		/*!
		 * Returns true if this allocator contains aligned allocation functions.
		 */
		GDINL constexpr static bool CanAllocateAligned()
		{
			return true;
		}

		/*!
		 * Allocates unaligned block of memory with specified size.
		 * @returns Pointer on the allocated memory.
		 */
		GDINL static Handle AllocateUnaligned(SizeTp const allocationSize, CStr const allocationFilename, Int32 const allocationLineNumber)
		{
			return _malloc_dbg(allocationSize, _NORMAL_BLOCK, allocationFilename, allocationLineNumber);
		}

		/*!
		 * Deallocates unaligned block of memory.
		 * @param memory The memory that would be deallocated. If specified block is nullptr then does nothing.
		 */
		GDINL static void DeallocateUnaligned(Handle const memory)
		{
			_free_dbg(memory, _NORMAL_BLOCK);
		}

		/*!
		 * Allocates block of memory with specified size that is aligned by specified value and returns pointer to it.
		 * @returns Pointer on the allocated memory.
		 */
		GDINL static Handle AllocateAligned(SizeTp const allocationSize, SizeTp const alignment, CStr const allocationFilename, Int32 const allocationLineNumber)
		{
			return _aligned_malloc_dbg(allocationSize, alignment, allocationFilename, allocationLineNumber);
		}

		/*!
		 * Deallocates aligned block of memory.
		 * @param memory The memory that would be deallocated. If specified block is nullptr then does nothing.
		 */
		GDINL static void DeallocateAligned(Handle const memory)
		{
			_aligned_free_dbg(memory);
		}

	};	// class PlatformAllocatorWindows
#else	// if GD_DEBUG
	using PlatformAllocatorWindows = PlatformAllocatorTBB;
#endif	// if GD_DEBUG

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Cross-platform allocator interface.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	using PlatformAllocator = PlatformAllocatorWindows;

GD_NAMESPACE_END
