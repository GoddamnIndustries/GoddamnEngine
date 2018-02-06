// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file
 * Traits, helper functions and definitions for standard string/memory functions.
 */
#pragma once
#define GD_INSIDE_CSTRING_H

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/PlatformSpecificInclude.h>

#include <cstring>
#include <cstdarg>
#include <cstdio>
#include <cwchar>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Provides functions for C string.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class CString : public TNonCreatable
	{
	public:

		// ------------------------------------------------------------------------------------------
		// String copy functions.
		// ------------------------------------------------------------------------------------------

		/*!
		 * @see @c "std::strcpy" function.
		 */
		//! @{
		GDINL static CStr Strcpy(Char* const dest, SizeTp const destLength, CStr const source)
		{
			GD_NOT_USED(destLength);
			GD_MSVC_PRAGMA(warning(push))
			GD_MSVC_PRAGMA(warning(suppress : 4996))
			return ::strcpy(dest, source);
			GD_MSVC_PRAGMA(warning(pop))
		}
		GDINL static WideCStr Strcpy(WideChar* const dest, SizeTp const destLength, WideCStr const source)
		{
			GD_NOT_USED(destLength);
			GD_MSVC_PRAGMA(warning(push))
			GD_MSVC_PRAGMA(warning(suppress : 4996))
			return ::wcscpy(dest, source);
			GD_MSVC_PRAGMA(warning(pop))
		}
		//! @}

		/*!
		 * @see @c "std::strncpy" function.
		 */
		//! @{
		GDINL static CStr Strncpy(Char* const dest, SizeTp const destLength, CStr const source, SizeTp const maxCount)
		{
			GD_NOT_USED(destLength);
			GD_MSVC_PRAGMA(warning(push))
			GD_MSVC_PRAGMA(warning(suppress : 4996))
			return ::strncpy(dest, source, maxCount);
			GD_MSVC_PRAGMA(warning(pop))
		}
		GDINL static WideCStr Strncpy(WideChar* const dest, SizeTp const destLength, WideCStr const source, SizeTp const maxCount)
		{
			GD_NOT_USED(destLength);
			GD_MSVC_PRAGMA(warning(push))
			GD_MSVC_PRAGMA(warning(suppress : 4996))
			return ::wcsncpy(dest, source, maxCount);
			GD_MSVC_PRAGMA(warning(pop))
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// String concatenation functions.
		// ------------------------------------------------------------------------------------------

		/*!
		 * @see @c "std::strcat" function.
		 */
		//! @{
		GDINL static CStr Strcat(Char* const dest, SizeTp const destLength, CStr const source)
		{
			GD_NOT_USED(destLength);
			GD_MSVC_PRAGMA(warning(push))
			GD_MSVC_PRAGMA(warning(suppress : 4996))
			return ::strcat(dest, source);
			GD_MSVC_PRAGMA(warning(pop))
		}
		GDINL static WideCStr Strcat(WideChar* const dest, SizeTp const destLength, WideCStr const source)
		{
			GD_NOT_USED(destLength);
			GD_MSVC_PRAGMA(warning(push))
			GD_MSVC_PRAGMA(warning(suppress : 4996))
			return ::wcscat(dest, source);
			GD_MSVC_PRAGMA(warning(pop))
		}
		//! @}

		/*!
		 * @see @c "std::strncat" function.
		 */
		//! @{
		GDINL static CStr Strncat(Char* const dest, SizeTp const destLength, CStr const source, SizeTp const maxCount)
		{
			GD_NOT_USED(destLength);
			GD_MSVC_PRAGMA(warning(push))
			GD_MSVC_PRAGMA(warning(suppress : 4996))
			return ::strncat(dest, source, maxCount);
			GD_MSVC_PRAGMA(warning(pop))
		}
		GDINL static WideCStr Strncat(WideChar* const dest, SizeTp const destLength, WideCStr const source, SizeTp const maxCount)
		{
			GD_NOT_USED(destLength);
			GD_MSVC_PRAGMA(warning(push))
			GD_MSVC_PRAGMA(warning(suppress : 4996))
			return ::wcsncat(dest, source, maxCount);
			GD_MSVC_PRAGMA(warning(pop))
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// String comparison functions.
		// ------------------------------------------------------------------------------------------

		/*!
		 * @see @c "std::strcmp" function.
		 */
		//! @{
		GDINL static Int32 Strcmp(CStr const lhs, CStr const rhs)
		{
			return ::strcmp(lhs, rhs);
		}
		GDINL static Int32 Strcmp(WideCStr const lhs, WideCStr const rhs)
		{
			return ::wcscmp(lhs, rhs);
		}
		//! @}

		/*!
		 * @see @c "std::strncmp" function.
		 */
		//! @{
		GDINL static Int32 Strncmp(CStr const lhs, CStr const rhs, SizeTp const maxCount)
		{
			return ::strncmp(lhs, rhs, maxCount);
		}
		GDINL static Int32 Strncmp(WideCStr const lhs, WideCStr const rhs, SizeTp const maxCount)
		{
			return ::wcsncmp(lhs, rhs, maxCount);
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// String length functions.
		// ------------------------------------------------------------------------------------------

		/*!
		 * @see @c "std::strlen" function.
		 */
		//! @{
		GDINL static SizeTp Strlen(CStr const cstr)
		{
			return ::strlen(cstr);
		}
		GDINL static SizeTp Strlen(WideCStr const cstr)
		{
			return ::wcslen(cstr);
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// String search functions.
		// ------------------------------------------------------------------------------------------

		/*!
		 * @see @c "std::strchr" function.
		 */
		//! @{
		GDINL static CStr Strchr(CStr const cstr, Char const chr)
		{
			return ::strchr(cstr, static_cast<int>(chr));
		}
		GDINL static WideChar const* Strchr(WideChar const* const cstr, WideChar const chr)
		{
			return ::wcschr(cstr, static_cast<wchar_t>(chr));
		}
		//! @}

		/*!
		 * @see @c "std::strrchr" function.
		 */
		//! @{
		GDINL static CStr Strrchr(CStr const cstr, Char const chr)
		{
			return ::strrchr(cstr, static_cast<int>(chr));
		}
		GDINL static WideChar const* Strrchr(WideChar const* const cstr, WideChar const chr)
		{
			return ::wcsrchr(cstr, static_cast<wchar_t>(chr));
		}
		//! @}

		/*!
		 * @see @c "std::strstr" function.
		 */
		//! @{
		GDINL static CStr Strstr(CStr const cstr, CStr const subCStr)
		{
			return ::strstr(cstr, subCStr);
		}
		GDINL static WideChar const* Strstr(WideChar const* const cstr, WideChar const* const subCStr)
		{
			return ::wcsstr(cstr, subCStr);
		}
		//! @}

		/*!
		 * Returns pointer to last occurrence of a specified C sub-string in the C string.
		 *
		 * @param cstr C string.
		 * @param subCStr C sub-string to search for.
		 */
		template<typename TChar>
		GDINL static TChar const* Strrstr(TChar const* const cstr, TChar const* const subCStr)
		{
			if (*subCStr == '\0')
			{
				return cstr;
			}

			auto cstrMut = cstr;
			TChar const* result = nullptr;
			while (true)
			{
				auto const found = Strstr(cstrMut, subCStr);
				if (found == nullptr)
				{
					break;
				}
				result = found;
				cstrMut = found + 1;
			}
			return result;
		}

		// ------------------------------------------------------------------------------------------
		// String conversion functions.
		// ------------------------------------------------------------------------------------------

		/*!
		 * @see @c "std::wcstombs" function.
		 */
		GDINL static SizeTp Wcstombs(Char* const dest, SizeTp const destLength, WideChar const* const source, SizeTp const maxCount)
		{
			GD_NOT_USED(destLength);
			GD_MSVC_PRAGMA(warning(push))
			GD_MSVC_PRAGMA(warning(suppress : 4996))
			return ::wcstombs(dest, source, maxCount);
			GD_MSVC_PRAGMA(warning(pop))
		}

		/*!
		 * @see @c "::mbstowcs" function.
		 */
		GDINL static SizeTp Strtowcs(WideChar* const dest, SizeTp const destLength, CStr const source, SizeTp const maxCount)
		{
			GD_NOT_USED(destLength);
			GD_MSVC_PRAGMA(warning(push))
			GD_MSVC_PRAGMA(warning(suppress : 4996))
			return ::mbstowcs(dest, source, maxCount);
			GD_MSVC_PRAGMA(warning(pop))
		}

		/*!
		 * @see @c "std::strtoull" function.
		 */
		//! @{
		GDINL static UInt64 Strtoui64(CStr const cstr, Char** const endPtr = nullptr, SizeTp const base = 0)
		{
			// ReSharper disable once CppIdenticalOperandsInBinaryExpression
			static_assert(sizeof(UInt64) <= sizeof(unsigned long long), "'unsigned long long' is not large enough to store 'UInt64' type.");
			return static_cast<UInt64>(::strtoull(cstr, endPtr, static_cast<int>(base)));
		}
		GDINL static UInt64 Strtoui64(WideChar const* const cstr, WideChar** const endPtr = nullptr, SizeTp const base = 0)
		{
			// ReSharper disable once CppIdenticalOperandsInBinaryExpression
			static_assert(sizeof(UInt64) <= sizeof(unsigned long long), "'unsigned long long' is not large enough to store 'UInt64' type.");
			return static_cast<UInt64>(::wcstoull(cstr, endPtr, static_cast<int>(base)));
		}
		//! @}

		/*!
		 * @see @c "std::strtoll" function.
		 */
		//! @{
		GDINL static Int64 Strtoi64(CStr const cstr, CStr* const endPtr = nullptr, SizeTp const base = 0)
		{
			// ReSharper disable once CppIdenticalOperandsInBinaryExpression
			static_assert(sizeof(Int64) <= sizeof(long long), "'long long' is not large enough to store 'Int64' type.");
			return static_cast<Int64>(::strtoll(cstr, const_cast<char**>(endPtr), static_cast<int>(base)));
		}
		GDINL static Int64 Strtoi64(WideChar const* const cstr, WideChar const** const endPtr = nullptr, SizeTp const base = 0)
		{
			// ReSharper disable once CppIdenticalOperandsInBinaryExpression
			static_assert(sizeof(Int64) <= sizeof(long long), "'long long' is not large enough to store 'Int64' type.");
			return static_cast<Int64>(::wcstoll(cstr, const_cast<wchar_t**>(endPtr), static_cast<int>(base)));
		}
		//! @}

		/*!
		 * @see @c "std::strtod" function.
		 */
		//! @{
		GDINL static Float64 Strtof64(CStr const cstr, CStr* const endPtr = nullptr)
		{
			return ::strtod(cstr, const_cast<char**>(endPtr));
		}
		GDINL static Float64 Strtof64(WideChar const* const cstr, WideChar const** const endPtr = nullptr)
		{
			return ::wcstod(cstr, const_cast<wchar_t**>(endPtr));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// String formatting functions.
		// ------------------------------------------------------------------------------------------

		/*!
		 * @see @c "std::vsnprintf" function.
		 */
		//! @{
		GDINL static Int32 Vsnprintf(Char* const dest, SizeTp const destLength, CStr const format, va_list arguments)
		{
			GD_MSVC_PRAGMA(warning(push))
			GD_MSVC_PRAGMA(warning(suppress : 4996))
			return ::vsnprintf(dest, destLength, format, arguments);
			GD_MSVC_PRAGMA(warning(pop))
		}
		GDINL static Int32 Vsnprintf(WideChar* const dest, SizeTp const destLength, WideCStr const format, va_list arguments)
		{
			GD_MSVC_PRAGMA(warning(push))
			GD_MSVC_PRAGMA(warning(suppress : 4996))
			return ::vswprintf(dest, destLength, format, arguments);
			GD_MSVC_PRAGMA(warning(pop))
		}
		//! @}

		/*!
		 * @see @c "std::snprintf" function.
		 */
		template<typename TChar>
		GDINL static int Snprintf(TChar* const dest, SizeTp const destLength, TChar const* const format, ...)
		{
			va_list arguments;
			va_start(arguments, format);
			int const result = Vsnprintf(dest, destLength, format, arguments);
			va_end(arguments);
			return result;
		}

		/*!
		 * @see @c "std::vsscanf" function.
		 */
		//! @{
		GDINL static Int32 Vsscanf(CStr const source, CStr const format, va_list arguments)	// NOLINT
		{
			return ::vsscanf(source, format, arguments);
		}
		GDINL static Int32 Vsscanf(WideCStr const source, WideCStr const format, va_list arguments)	// NOLINT
		{
			return ::vswscanf(source, format, arguments);
		}
		//! @}

		/*!
		 * @see @c "std::sscanf" function.
		 */
		template<typename TChar>
		GDINL static int Sscanf(TChar const* const source, TChar const* const format, ...)
		{
			va_list arguments;
			va_start(arguments, format);
			int const result = Vsscanf(source, format, arguments);
			va_end(arguments);
			return result;
		}
	};	// class CStringGeneric

	/*!
	 * Declarations used to ban standard functions. 
	 */
	enum LibStringUnallowedFunctions
	{
		// ReSharper disable CppInconsistentNaming
		strcpy_s, strcpy, wcscpy_s, wcscpy,
		strncpy_s, strncpy, wcsncpy_s, wcsncpy,
		strcat_s, strcat, wcscat_s, wcscat,
		strncat_s, strncat, wcsncat_s, wcsncat,
		strcmp, wcscmp,
		strncmp, wcsncmp,
		strlen, wcslen,
		strchr, wcschr,
		strrchr, wcsrchr,
		strstr, wcsstr,
		strrstr, wcsrstr,
		wcstombs_s, wcstombs, mbstowcs_s, mbstowcs,
		_strtoui64, strtoull, _wcstoui64, wcstoull,
		_strtoi64, strtoll, _wcstoi64, wcstoll,
		strtod, wcstod,
		_vsnprintf_s, vsnprintf, _vsnwprintf_s, vswprintf,
		_snprintf_s, snprintf, _snwprintf_s, snwprintf,
		vsscanf_s, vsscanf, vswscanf_s, vswscanf,
		sscanf_s, sscanf, wscanf_s, wscanf,
		// ReSharper restore CppInconsistentNaming
	};	// enum LibCharUnallowedFunctions

GD_NAMESPACE_END

//#include GD_PLATFORM_API_INCLUDE(GoddamnEngine/Core/CStdlib, CString)
#undef GD_INSIDE_CSTRING_H

GD_NAMESPACE_BEGIN
GD_NAMESPACE_END
