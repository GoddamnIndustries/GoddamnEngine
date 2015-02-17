/// ==========================================================================================
/// CMemory.h - Traits, helper functions and definitions for memory.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_MISC_CMEMORY
#define GD_CORE_MISC_CMEMORY

#include <GoddamnEngine/Include.h>

#include <cstring>	// string.h's 'str*' functions.
#include <cstdlib>	// stdlib.h's 'str*' functions.

/// @brief Define this to 1 to forcedly use generic standart function instead of platform onces.
#define GD_CMEMORY_FORCE_USE_GENERIC 0

#if GD_PLATFORM_API_WINAPI && (!GD_CMEMORY_FORCE_USE_GENERIC)

/// @brief Enable this to use Microsoft-specific safe functions.
#define GD_CMEMORY_USE_MICROSOFT_EXT 1

#endif	// if GD_PLATFORM_API_WINAPI && (!GD_CMEMORY_FORCE_USE_GENERIC)

#if GD_CMEMORY_USE_MICROSOFT_EXT

/// @brief Selects Microsoft-specific or generic code.
/// @param ... Specified code.
/// @{
#define GD_CMEMORY_MICROSOFT(...) { __VA_ARGS__ }
#define GD_CMEMORY_GENERIC(...)
/// @}

#else	// if GD_CMEMORY_USE_MICROSOFT_EXT

/// @brief Selects Microsoft-specific or generic code.
/// @param ... Specified code.
/// @{
#define GD_CMEMORY_MICROSOFT(...) 
#define GD_CMEMORY_GENERIC(...) { __VA_ARGS__ }
/// @}

/// @brief Disables all deprecation function on all Microsoft platforms.
#define _CRT_SECURE_NO_WARNINGS 1

#endif	// if GD_CMEMORY_USE_MICROSOFT_EXT


GD_NAMESPACE_BEGIN

	/// @brief Provides functions for C memory. Contains wrapped "mem*" methods and methods from "cstdio".
	class CMemory final
	{
	public:
		// ... string.h's functions ...

		/// @brief Fills C memory buffer with a specified value.
		/// @param Dest Destination pointer.
		/// @param Source Source pointer.
		/// @param Value Value to fill into the buffer.
		/// @param MaxCount Amount of bytes to fill.
		/// @returns Pointer to destination buffer.
		/// @see @c "std::memset" function.
		/// @{
		GDINL static Handle Memset(Handle const Dest, UInt8 const Value, size_t const MaxCount)
		{
			return std::memset(Dest, static_cast<int>(Value), MaxCount);
		}

		/// @brief Copies C memory buffer from source to destination. Buffers should not overlap.
		/// @param Dest Destination pointer.
		/// @param DestLength Length of the destination buffer.
		/// @param Source Source pointer.
		/// @param MaxCount Amount of bytes to copy.
		/// @returns Pointer to destination buffer.
		/// @see @c "std::memcpy" function.
		/// @{
		GDINL static Handle Memcpy(Handle const Dest, size_t const DestLength, CHandle const Source, size_t const MaxCount)
		{
			GD_CMEMORY_MICROSOFT(
				auto const Result = ::memcpy_s(Dest, DestLength, Source, MaxCount);
				GD_DEBUG_ASSERT(Result == 0, "memcpy_s failed.");
				return Dest;
			);
			GD_CMEMORY_GENERIC(
				GD_NOT_USED(DestLength);
				return std::memcpy(Dest, Source, MaxCount);
			);
		}

		/// @brief Copies C memory buffer from source to destination. Buffers may not overlap.
		/// @param Dest Destination pointer.
		/// @param DestLength Length of the destination buffer.
		/// @param Source Source pointer.
		/// @param MaxCount Amount of bytes to copy.
		/// @returns Pointer to destination buffer.
		/// @see @c "std::memmove" function.
		/// @{
		GDINL static Handle Memmove(Handle const Dest, size_t const DestLength, CHandle const Source, size_t const MaxCount)
		{
			GD_CMEMORY_MICROSOFT(
				auto const Result = ::memmove_s(Dest, DestLength, Source, MaxCount);
				GD_DEBUG_ASSERT(Result == 0, "memcpy_s failed.");
				return Dest;
			);
			GD_CMEMORY_GENERIC(
				GD_NOT_USED(DestLength);
				return std::memmove(Dest, Source, MaxCount);
			);
		}

	};	// class CMemory

GD_NAMESPACE_END

#undef GD_CMEMORY_MICROSOFT
#undef GD_CMEMORY_GENERIC

#undef GD_CMEMORY_USE_MICROSOFT_EXT

#if (defined(_CRT_SECURE_NO_WARNINGS))
#	undef _CRT_SECURE_NO_WARNINGS
#endif	// if (defined(_CRT_SECURE_NO_WARNINGS))

#endif	// ifndef GD_CORE_MISC_CMEMORY
