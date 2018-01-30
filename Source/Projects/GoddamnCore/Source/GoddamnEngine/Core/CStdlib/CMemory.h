// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file
 * Wrappers, helper functions and definitions for standard memory functions.
 */
#pragma once
#define GD_INSIDE_CMEMORY_H

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/PlatformSpecificInclude.h>

#include <cstring>
#include <cwchar>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Provides functions for C memory. 
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class CMemoryGeneric : public TNonCreatable
	{
	public:

		// ------------------------------------------------------------------------------------------
		// Memory comparison functions.
		// ------------------------------------------------------------------------------------------

		/*! 
		 * @see @c "std::memcmp" function.
		 */
		GDINL static int Memcmp(CHandle const lhs, CHandle const rhs, SizeTp const rhsLength)
		{
			return ::memcmp(lhs, rhs, rhsLength);
		}

		// ------------------------------------------------------------------------------------------
		// Memory manipulation functions.
		// ------------------------------------------------------------------------------------------

		/*!
		 * @see @c "std::memset" function.
		 */
		//! @{
		GDINL static Handle Memset(Handle const dest, Byte const value, SizeTp const maxCount)
		{
			return ::memset(dest, static_cast<int>(value), maxCount);
		}
		GDINL static Char* CMemset(Char* const dest, Char const value, SizeTp const maxCount)
		{
			return static_cast<Char*>(Memset(dest, value, maxCount * sizeof(Char)));
		}
		GDINL static WideChar* CMemset(WideChar* const dest, WideChar const value, SizeTp const maxCount)
		{
			return ::wmemset(dest, value, maxCount);
		}
		//! @}

		/*!
		 * @see @c "std::memcpy" function.
		 */
		//! @{
		GDINL static Handle Memcpy(Handle const dest, CHandle const source, SizeTp const sourceLength)
		{
			return ::memcpy(dest, source, sourceLength);
		}
		GDINL static Char* CMemcpy(Char* const dest, CStr const source, SizeTp const sourceLength)
		{
			return static_cast<Char*>(Memcpy(dest, source, sourceLength));
		}
		GDINL static WideChar* CMemcpy(WideChar* const dest, WideCStr const source, SizeTp const sourceLength)
		{
			return ::wmemcpy(dest, source, sourceLength);
		}
		//! @}

		/*!
		 * @see @c "std::memmove" function.
		 */
		//! @{
		GDINL static Handle Memmove(Char* const dest, CHandle const source, SizeTp const sourceLength)
		{
			return ::memmove(dest, source, sourceLength);
		}
		GDINL static Char* CMemmove(Char* const dest, CStr const source, SizeTp const sourceLength)
		{
			return static_cast<Char*>(Memmove(dest, source, sourceLength));
		}
		GDINL static WideChar* CMemmove(WideChar* const dest, WideCStr const source, SizeTp const sourceLength)
		{
			return ::wmemmove(dest, source, sourceLength);
		}
		//! @}

	};	// class CMemoryGeneric

	enum
	{
		// ReSharper disable CppInconsistentNaming
		memcmp,
		memset,
		memcpy, memcpy_s,
		memmove, memmove_s,
		// ReSharper restore CppInconsistentNaming
	};	// enum

GD_NAMESPACE_END

#if GD_PLATFORM_API_MICROSOFT
#   include GD_PLATFORM_API_INCLUDE(GoddamnEngine/Core/CStdlib, CMemory)
#else   // if GD_PLATFORM_API_MICROSOFT

GD_NAMESPACE_BEGIN

    // **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
    //! Provides cross-platform functions for C memory.
    // **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
    using CMemory = CMemoryGeneric;

GD_NAMESPACE_END

#endif  // if GD_PLATFORM_API_MICROSOFT
#undef GD_INSIDE_CMEMORY_H
