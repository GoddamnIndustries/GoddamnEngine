// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Base/CStdlib/CChar.h
 * @note This file should be never directly included, please consider using <GoddamnEngine/Include.h> instead.
 * Wrappers, helper functions and definitions for standard memory functions.
 */
#pragma once

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Provides functions for C memory. Contains wrapped "mem*" methods and methods from "string.h",
	//! "stdlib.h" and etc.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class CMemory final : public TNonCreatable
	{
	public:
		// ... string.h's functions ...

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
	private:
		GDINL static Handle Memcpy_s(Handle const dest, SizeTp const destLength, CHandle const source, SizeTp const maxCount)
		{
#if GD_PLATFORM_API_MICROSOFT
			auto const result = ::memcpy_s(dest, destLength, source, maxCount);
			GD_DEBUG_VERIFY(result == 0, "memcpy_s failed.");
			return dest;
#else	// if GD_PLATFORM_API_MICROSOFT
			GD_NOT_USED(destLength);
			return ::memcpy(dest, source, maxCount);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
	public:
		GDINL static Handle Memcpy(Handle const dest, CHandle const source, SizeTp const maxCount)
		{
			// as an exception, we force using unsafe 'memcpy'.
			return Memcpy_s(dest, maxCount, source, maxCount);
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
	private:
		GDINL static Handle Memmove_s(Handle const dest, SizeTp const destLength, CHandle const source, SizeTp const maxCount)
		{
#if GD_PLATFORM_API_MICROSOFT
			auto const result = ::memmove_s(dest, destLength, source, maxCount);
			GD_DEBUG_VERIFY(result == 0, "memcpy_s failed.");
			return dest;
#else	// if GD_PLATFORM_API_MICROSOFT
			GD_NOT_USED(destLength);
			return ::memmove(dest, source, maxCount);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
	public:
		GDINL static Handle Memmove(Handle const dest, CHandle const source, SizeTp const maxCount)
		{
			// as an exception, we force using unsafe 'memmove'.
			return Memmove_s(dest, maxCount, source, maxCount);
		}
		template<typename TPointee>
		GDINL static TPointee* Memmove(TPointee* const dest, TPointee const* const source, SizeTp const maxCount)
		{
			// as an exception, we force using unsafe 'memmove'.
			return static_cast<TPointee*>(Memmove(dest, source, maxCount * sizeof(TPointee)));
		}
		//! @}

	};	// class CMemory

	/*!
	 * Declarations used to ban standard functions. 
	 */
	enum LibMemoryUnallowedFunctions
	{
		memcmp,
		memset,
		memcpy, memcpy_s,
		memmove, memmove_s,
	};	// enum LibMemoryUnallowedFunctions

GD_NAMESPACE_END
