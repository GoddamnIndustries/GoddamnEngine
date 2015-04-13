//! ==========================================================================================
//! CMemory.h - Traits, helper functions and definitions for memory.
//! Copyright (C) Goddamn Industries 2015. All Rights Reserved.
//! ==========================================================================================

#pragma once
#ifndef GD_CORE_MISC_CMEMORY
#define GD_CORE_MISC_CMEMORY

#include <GoddamnEngine/Include.h>

GD_NAMESPACE_BEGIN

	//! @brief Provides functions for C memory. Contains wrapped "mem*" methods and methods from "cstdio".
	class CMemory final
	{
	public:
		// ... string.h's functions ...

		//! @brief Fills C memory buffer with a specified value.
		//! @param Dest Destination pointer.
		//! @param Source Source pointer.
		//! @param Value Value to fill into the buffer.
		//! @param MaxCount Amount of bytes to fill.
		//! @returns Pointer to destination buffer.
		//! @see @c "std::memset" function.
		//! @{
		GDINL static Handle Memset(Handle const Dest, UInt8 const Value, SizeTp const MaxCount)
		{
			return ::std::memset(Dest, static_cast<int>(Value), MaxCount);
		}

		//! @brief Copies C memory buffer from source to destination. Buffers should not overlap.
		//! @param Dest Destination pointer.
		//! @param DestLength Length of the destination buffer.
		//! @param Source Source pointer.
		//! @param MaxCount Amount of bytes to copy.
		//! @returns Pointer to destination buffer.
		//! @see @c "std::memcpy" function.
		//! @{
		GDINL static Handle Memcpy(Handle const Dest, SizeTp const DestLength, CHandle const Source, SizeTp const MaxCount)
		{
#if GD_PLATFORM_API_MICROSOFT
			auto const Result = ::memcpy_s(Dest, DestLength, Source, MaxCount);
			GD_DEBUG_ASSERT(Result == 0, "memcpy_s failed.");
			return Dest;
#else	// if GD_PLATFORM_API_MICROSOFT
			GD_NOT_USED(DestLength);
			return ::std::memcpy(Dest, Source, MaxCount);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}

		//! @brief Copies C memory buffer from source to destination. Buffers may not overlap.
		//! @param Dest Destination pointer.
		//! @param DestLength Length of the destination buffer.
		//! @param Source Source pointer.
		//! @param MaxCount Amount of bytes to copy.
		//! @returns Pointer to destination buffer.
		//! @see @c "std::memmove" function.
		//! @{
		GDINL static Handle Memmove(Handle const Dest, SizeTp const DestLength, CHandle const Source, SizeTp const MaxCount)
		{
#if GD_PLATFORM_API_MICROSOFT
			auto const Result = ::memmove_s(Dest, DestLength, Source, MaxCount);
			GD_DEBUG_ASSERT(Result == 0, "memcpy_s failed.");
			return Dest;
#else	// if GD_PLATFORM_API_MICROSOFT
			GD_NOT_USED(DestLength);
			return ::std::memmove(Dest, Source, MaxCount);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
	};	// class CMemory

GD_NAMESPACE_END

#endif	// ifndef GD_CORE_MISC_CMEMORY
