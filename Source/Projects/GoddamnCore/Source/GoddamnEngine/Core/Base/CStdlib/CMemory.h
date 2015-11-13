// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Misc/CMemory.h
//! Traits, helper functions and definitions for standard memory functions.
#pragma once

#include <GoddamnEngine/Include.h>

GD_NAMESPACE_BEGIN

	// ------------------------------------------------------------------------------------------
	//! Provides functions for C memory. Contains wrapped "mem*" methods and methods from "string.h",
	//! "stdlib.h" and etc.
	class CMemory final : public IUncreatable
	{
	public:
		// ... string.h's functions ...

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::memcmp" function.
		GDINL static int Memcmp(CHandle const lhs, CHandle const rhs, SizeTp const maxCount)
		{
			return ::memcmp(lhs, rhs, maxCount);
		}

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::memset" function.
		GDINL static Handle Memset(Handle const dest, UInt8 const value, SizeTp const maxCount)
		{
			return ::memset(dest, static_cast<int>(value), maxCount);
		}

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::memcpy" function.
		//! @{
		GDINL static Handle Memcpy_s(Handle const dest, SizeTp const destLength, CHandle const source, SizeTp const maxCount)
		{
#if GD_PLATFORM_API_MICROSOFT
			auto const result = ::memcpy_s(dest, destLength, source, maxCount);
			GD_DEBUG_ASSERT(result == 0, "memcpy_s failed.");
			return dest;
#else	// if GD_PLATFORM_API_MICROSOFT
			GD_NOT_USED(destLength);
			return ::memcpy(dest, source, maxCount);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
	//	GD_DEPRECATED("This function is insecure on several platforms.")
		GDINL static Handle Memcpy(Handle const dest, SizeTp const destLength, CHandle const source)
		{
			return Memcpy_s(dest, destLength, source, destLength);
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::memmove" function.
		//! @{
		GDINL static Handle Memmove(Handle const dest, SizeTp const destLength, CHandle const source, SizeTp const maxCount)
		{
#if GD_PLATFORM_API_MICROSOFT
			auto const result = ::memmove_s(dest, destLength, source, maxCount);
			GD_DEBUG_ASSERT(result == 0, "memcpy_s failed.");
			return dest;
#else	// if GD_PLATFORM_API_MICROSOFT
			GD_NOT_USED(destLength);
			return ::memmove(dest, source, maxCount);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
	};	// class CMemory

GD_NAMESPACE_END
