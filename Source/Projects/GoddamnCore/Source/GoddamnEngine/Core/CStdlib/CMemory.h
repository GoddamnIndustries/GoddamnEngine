// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/CStdlib/CChar.h
 * Wrappers, helper functions and definitions for standard memory functions.
 */
#pragma once
#define GD_INSIDE_CMEMORY_H

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/TypeTraits.h>
#include <GoddamnEngine/Core/PlatformSpecificInclude.h>

#include <cstring>
#include <cwchar>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Provides functions for C memory. Contains wrapped "mem*" methods and methods from "string.h",
	//! "stdlib.h" and etc.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class CMemoryGeneric : public TNonCreatable
	{
	public:

		// ------------------------------------------------------------------------------------------
		// ... string.h's functions ...
		// ------------------------------------------------------------------------------------------

		/*! 
		 * @see @c "std::memcmp" function.
		 */
		//! @{
		GDINL static int Memcmp(CHandle const lhs, CHandle const rhs, SizeTp const maxCount)
		{
			return ::memcmp(lhs, rhs, maxCount);
		}
		template<typename TPointee = void>
		GDINL static int TMemcmp(TPointee const* const lhs, TPointee const* const rhs, SizeTp const maxCount)
		{
			return Memcmp(lhs, rhs, maxCount * sizeof(TPointee));
		}
		//! @}

		/*!
		 * @see @c "std::memset" function.
		 */
		//! @{
		GDINL static Handle Memset(Handle const dest, Byte const value, SizeTp const maxCount)
		{
			return ::memset(dest, static_cast<int>(value), maxCount);
		}
		GDINL static WideChar* Memset(WideChar* const dest, WideChar const value, SizeTp const maxCount)
		{
			return ::wmemset(dest, static_cast<int>(value), maxCount);
		}
		template<typename TPointee = void>
		GDINL static TPointee* TMemset(TPointee* const dest, Byte const value, SizeTp const maxCount)
		{
			return static_cast<TPointee*>(Memset(dest, value, maxCount * sizeof(TPointee)));
		}
		template<typename TPointee>
		GDINL static TPointee* TMemset(TPointee* const dest, WideChar const value, SizeTp const maxCount)
		{
			return static_cast<TPointee*>(Memset(dest, value, maxCount * sizeof(TPointee)));
		}
		template<typename TValue>
		GDINL static typename DisableIf<TypeTraits::IsSame<TValue, Byte>::Value || TypeTraits::IsSame<TValue, WideChar>::Value, TValue>::Type*
			TMemset(TValue* const dest, TValue const value, SizeTp const maxCount)
		{
			for (auto iter = dest; iter != dest + maxCount; ++iter)
			{
				*iter = value;
			}
			return dest;
		}
		//! @}

		/*!
		 * @see @c "std::memcpy" function.
		 */
		//! @{
		GDINL static Handle MemcpySafe(Handle const dest, SizeTp const destLength, CHandle const source, SizeTp const maxCount)
		{
			GD_NOT_USED(destLength);
			return ::memcpy(dest, source, maxCount);
		}
		GDINL static Handle Memcpy(Handle const dest, CHandle const source, SizeTp const maxCount)
		{
			// as an exception, we force using unsafe 'memcpy'.
			return MemcpySafe(dest, maxCount, source, maxCount);
		}
		template<typename TPointee>
		GDINL static TPointee* TMemcpy(TPointee* const dest, TPointee const* const source, SizeTp const maxCount)
		{
			// as an exception, we force using unsafe 'memcpy'.
			return static_cast<TPointee*>(Memcpy(dest, source, maxCount * sizeof(TPointee)));
		}
		//! @}

		/*!
		 * @see @c "std::memmove" function.
		 */
		//! @{
		GDINL static Handle MemmoveSafe(Handle const dest, SizeTp const destLength, CHandle const source, SizeTp const maxCount)
		{
			GD_NOT_USED(destLength);
			return ::memmove(dest, source, maxCount);
		}
		GDINL static Handle Memmove(Handle const dest, CHandle const source, SizeTp const maxCount)
		{
			// as an exception, we force using unsafe 'memmove'.
			return MemmoveSafe(dest, maxCount, source, maxCount);
		}
		template<typename TPointee>
		GDINL static TPointee* Memmove(TPointee* const dest, TPointee const* const source, SizeTp const maxCount)
		{
			// as an exception, we force using unsafe 'memmove'.
			return static_cast<TPointee*>(Memmove(dest, source, maxCount * sizeof(TPointee)));
		}
		//! @}

	};	// class CMemoryGeneric

	/*!
	 * Declarations used to ban standard functions. 
	 */
	enum LibMemoryUnallowedFunctions
	{
		// ReSharper disable CppInconsistentNaming
		memcmp,
		memset,
		memcpy, memcpy_s,
		memmove, memmove_s,
		// ReSharper restore CppInconsistentNaming
	};	// enum LibMemoryUnallowedFunctions

GD_NAMESPACE_END

#include GD_PLATFORM_API_INCLUDE(GoddamnEngine/Core/CStdlib, CMemory)
#undef GD_INSIDE_CMEMORY_H
