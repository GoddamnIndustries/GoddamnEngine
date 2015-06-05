// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Definitions/Allocator.cpp
//! Contains memory allocator implementation.

#include <GoddamnEngine/Include.h>

// ------------------------------------------------------------------------------------------
// Microsoft CRT's Debug Allocator.
// ------------------------------------------------------------------------------------------
#if GD_ALLOCATOR_DEBUG_LEAKS
#	define GD_PLATFORM_HAS_DEBUG_CRT_MALLOC								GD_TRUE
#	define GD_ALLOCATOR_IS_THREAD_SAFE									GD_TRUE
#endif	// if GD_ALLOCATOR_DEBUG_LEAKS

// ------------------------------------------------------------------------------------------
// Intel Threading Building Blocks' Scalable Allocator.
// ------------------------------------------------------------------------------------------
#if (GD_PLATFORM_WINDOWS || GD_PLATFORM_OS_X || GD_PLATFORM_GNU_LINUX) && (GD_ARCHITECTURE_X86 || GD_ARCHITECTURE_X64) && (!defined(GD_PLATFORM_HAS_DEBUG_CRT_MALLOC))
#	define GD_PLATFORM_HAS_INTEL_TBB									GD_TRUE
#	define GD_ALLOCATOR_IS_THREAD_SAFE									GD_TRUE
#endif	//	if if (GD_PLATFORM_WINDOWS || GD_PLATFORM_OS_X || GD_PLATFORM_GNU_LINUX) && (GD_ARCHITECTURE_X86 || GD_ARCHITECTURE_X64) && (!defined(GD_PLATFORM_HAS_DEBUG_CRT_MALLOC))

// ------------------------------------------------------------------------------------------
// Jason Evans Allocator.
// ------------------------------------------------------------------------------------------
#if (0) && (!defined(GD_PLATFORM_HAS_DEBUG_CRT_MALLOC))	//! @todo No platforms that use JEMalloc? Really?
#	define GD_PLATFORM_HAS_JEMALLOC										GD_TRUE
#	define GD_ALLOCATOR_IS_THREAD_SAFE									GD_TRUE
#endif	// if (0) && (!defined(GD_PLATFORM_HAS_INTEL_TBB)) && (!defined(GD_PLATFORM_HAS_DEBUG_CRT_MALLOC))

// ------------------------------------------------------------------------------------------
// ANSI C standart library's Allocator.
// ------------------------------------------------------------------------------------------
#if (GD_PLATFORM_MOBILE || GD_PLATFORM_CONSOLE || GD_PLATFORM_HTML5) && (!defined(GD_PLATFORM_HAS_DEBUG_CRT_MALLOC))	//! @todo Consoles use ANSI Malloc? Really? (2)
#	define GD_PLATFORM_HAS_ANSI_MALLOC									GD_TRUE
#	if GD_PLATFORM_API_MICROSOFT || GD_PLATFORM_API_COCOA					// On this platforms Allocator is thread safe.
#		define GD_ALLOCATOR_IS_THREAD_SAFE								GD_TRUE
#	else	// if GD_PLATFORM_API_MICROSOFT || GD_PLATFORM_API_COCOA		// Generally ANSI Allocator is not guaranteed to be thread safe.
#		define GD_ALLOCATOR_IS_THREAD_SAFE								GD_FALSE
#	endif	// if GD_PLATFORM_API_MICROSOFT || GD_PLATFORM_API_COCOA
#endif	// if (GD_PLATFORM_MOBILE || GD_PLATFORM_CONSOLE || GD_PLATFORM_HTML5) && (!defined(GD_PLATFORM_HAS_DEBUG_CRT_MALLOC))

// Checking if there is actually a single allocator selected.
#if (!defined(GD_PLATFORM_HAS_DEBUG_CRT_MALLOC))
#	define GD_PLATFORM_HAS_DEBUG_CRT_MALLOC								GD_FALSE
#endif	// if (!defined(GD_PLATFORM_HAS_DEBUG_CRT_ALLOCATOR))
#if (!defined(GD_PLATFORM_HAS_INTEL_TBB))
#	define GD_PLATFORM_HAS_INTEL_TBB									GD_FALSE
#endif	// if (!defined(GD_PLATFORM_HAS_JEMALLOC))
#if (!defined(GD_PLATFORM_HAS_JEMALLOC))
#	define GD_PLATFORM_HAS_JEMALLOC										GD_FALSE
#endif	// if (!defined(GD_PLATFORM_HAS_JEMALLOC))
#if (!defined(GD_PLATFORM_HAS_ANSI_MALLOC))
#	define GD_PLATFORM_HAS_ANSI_MALLOC									GD_FALSE
#endif	// if (!defined(GD_PLATFORM_HAS_ANSI_MALLOC))
#if (!(0 \
	^ GD_PLATFORM_HAS_DEBUG_CRT_MALLOC \
	^ GD_PLATFORM_HAS_INTEL_TBB \
	^ GD_PLATFORM_HAS_JEMALLOC \
	^ GD_PLATFORM_HAS_ANSI_MALLOC))
#	error No target allocator was selected OR multiple target allocators were selected.
#endif	// ** All possible allocators XORed **

#if (!defined(GD_ALLOCATOR_IS_THREAD_SAFE))
#	define GD_ALLOCATOR_IS_THREAD_SAFE									GD_FALSE
#endif	// if (!defined(GD_ALLOCATOR_IS_THREAD_SAFE))

#if GD_PLATFORM_HAS_DEBUG_CRT_MALLOC
#	include <Windows.h>
#	include <crtdbg.h>
#endif	// if GD_PLATFORM_HAS_DEBUG_CRT_MALLOC
#if GD_PLATFORM_HAS_INTEL_TBB
#	include <tbb/scalable_allocator.h>
#endif	// if GD_PLATFORM_HAS_INTEL_TBB
#if GD_PLATFORM_HAS_JEMALLOC
#	include <jemalloc/jemalloc.h>
#endif	// if GD_PLATFORM_HAS_JEMALLOC

#include <GoddamnEngine/Core/Platform/Atomics.h>
#if !GD_ALLOCATOR_IS_THREAD_SAFE
#	include <GoddamnEngine/Core/Platform/Threading.h>
#endif	// if !GD_ALLOCATOR_IS_THREAD_SAFE

GD_NAMESPACE_BEGIN
 
#if !GD_ALLOCATOR_IS_THREAD_SAFE
	//! Returns pointer to sync object of an allocator.
	GDINL static CriticalSection* GetAllocatorLock()
	{
		static CriticalSection AllocatorLock;
		return &AllocatorLock;
	}
#endif	// if !GD_ALLOCATOR_IS_THREAD_SAFE

	// ------------------------------------------------------------------------------------------
	//! Allocates block of memory with specified size that is aligned by specified value and returns pointer to it.
	//! @param Location Information about place in code, where this allocation accures.
	//! @param AllocationSize Size of required memory in bytes.
	//! @param Alignment Pointer to memory would be aligned by this value.
	//! @returns Pointer on the allocated memory. This function never returns nullptr except specified size is 0.
	//! @note This function should not be directly invoked. Use 'GD_MALLOC'/'GD_MALLOC_ALIGNED' macro instead.
	GDAPI Handle Allocator::AllocateMemory(SourceInfo const& Location, SizeTp const AllocationSize, SizeTp const Alignment /*= DefaultAlignment*/)
	{
		GD_NOT_USED(Location);	// In case it will be never used.
		if (AllocationSize != 0) 
		{
#if !GD_ALLOCATOR_IS_THREAD_SAFE
			ScopedLock AllocatorLock(GetAllocatorLock());
#endif	// if !GD_ALLOCATOR_IS_THREAD_SAFE

			static Int32 AllocatorNotInitialized = GD_TRUE;
			if (Atomics::InterlockedCompareExchange32(&AllocatorNotInitialized, GD_FALSE, GD_TRUE) == GD_TRUE) 
			{

				// Configuring new fragmentation value for Win32's allocators.
#if (GD_PLATFORM_HAS_ANSI_MALLOC || GD_PLATFORM_HAS_DEBUG_CRT_MALLOC) && GD_PLATFORM_API_MICROSOFT
				ULONG static const LowFragmentationHeapValue = 2;
				HeapSetInformation(reinterpret_cast<HANDLE>(_get_heap_handle()), HeapCompatibilityInformation, const_cast<PULONG>(&LowFragmentationHeapValue), sizeof(LowFragmentationHeapValue));
#endif	// if (GD_PLATFORM_HAS_ANSI_MALLOC || GD_PLATFORM_HAS_DEBUG_CRT_MALLOC) && GD_PLATFORM_API_MICROSOFT

				// Configuring the CRT memory debugging.
#if GD_PLATFORM_HAS_DEBUG_CRT_MALLOC
				_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_FILE);
				_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
				struct MemoryLeakReporter {
					_CrtMemState InitialMemoryState;
					GDINT  MemoryLeakReporter() { _CrtMemCheckpoint(&this->InitialMemoryState); }
					GDINT ~MemoryLeakReporter() { _CrtMemDumpAllObjectsSince(&this->InitialMemoryState); }
				}  static  MemoryLeakReporterInstance;
#endif	// if GD_PLATFORM_HAS_DEBUG_CRT_MALLOC
			}

			Handle const Memory = 
#if GD_PLATFORM_HAS_DEBUG_CRT_MALLOC
				_aligned_malloc_dbg(AllocationSize, Alignment, Location.File, Location.Line);
#endif	// if GD_PLATFORM_HAS_DEBUG_CRT_MALLOC
#if GD_PLATFORM_HAS_INTEL_TBB
				scalable_aligned_malloc(AllocationSize, Alignment);
#endif	// if GD_PLATFORM_HAS_INTEL_TBB
#if GD_PLATFORM_HAS_JEMALLOC
				je_aligned_malloc(Alignment, AllocationSize);
#endif	// if GD_PLATFORM_HAS_JEMALLOC
#if GD_PLATFORM_HAS_ANSI_MALLOC
				aligned_alloc(Alignment, AllocationSize);
#endif	// if GD_PLATFORM_HAS_ANSI_MALLOC

			GD_ASSERT(Memory != nullptr, "Failed to allocate memory.");
			return Memory;
		}

		return nullptr;
	}

	// ------------------------------------------------------------------------------------------
	//! Deallocates block of memory.
	//! @param Memory Memory that would be deallocated. If specified block is nullptr then does nothing.
	//! @note This function should not be directly invoked. Use 'GD_DEALLOC' macro instead.
	GDAPI void Allocator::DeallocateMemory(Handle const Memory)
	{
		if (Memory != nullptr) 
		{
#if !GD_ALLOCATOR_IS_THREAD_SAFE
			ScopedLock AllocatorLock(GetAllocatorLock());
#endif	// if !GD_ALLOCATOR_IS_THREAD_SAFE

#if GD_PLATFORM_HAS_DEBUG_CRT_MALLOC
			_aligned_free_dbg(Memory);
#endif	// if GD_PLATFORM_HAS_DEBUG_CRT_MALLOC
#if GD_PLATFORM_HAS_INTEL_TBB
			scalable_aligned_free(Memory);
#endif	// if GD_PLATFORM_HAS_INTEL_TBB
#if GD_PLATFORM_HAS_JEMALLOC
			je_aligned_free(Memory);
#endif	// if GD_PLATFORM_HAS_JEMALLOC
#if GD_PLATFORM_HAS_ANSI_MALLOC
			free(Memory);
#endif	// if GD_PLATFORM_HAS_ANSI_MALLOC
		}
	}
	
GD_NAMESPACE_END
