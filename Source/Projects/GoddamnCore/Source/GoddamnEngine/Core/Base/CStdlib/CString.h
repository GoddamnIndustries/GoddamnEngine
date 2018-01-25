// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Base/CStdlib/CString.h
 * @note This file should be never directly included, please consider using <GoddamnEngine/Include.h> instead.
 * Traits, helper functions and definitions for standard string/memory functions.
 */
#pragma once
#if !defined(GD_INSIDE_INCLUDE_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Include.h> instead.
#endif	// if !defined(GD_INSIDE_INCLUDE_H)

//#if GD_PLATFORM_API_MICROSOFT
extern "C" {
	extern char const* goddamn_strrstr(char const*, char const*);
	extern wchar_t const* goddamn_wcsrstr(wchar_t const*, wchar_t const*);
}
//#endif	// if GD_PLATFORM_API_MICROSOFT

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Provides functions for C string. Contains wrapped "str*" methods and methods from 'string.h', 
	//! 'stdlib.h' and 'stdio.h'.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class CString final : public TNonCreatable
	{
	public:
		// ... string.h & wchar.h's functions ...

		/*!
		 * @see @c "std::strcpy" function.
		 */
		//! @{
		GDINL static CStr StrcpySafe(Char* const dest, SizeTp const destLength, CStr const source)
		{
#if GD_PLATFORM_API_MICROSOFT
			auto const result = ::strcpy_s(dest, destLength, source);
			GD_DEBUG_VERIFY(result == 0, "strcpy_s failed.");
			return dest;
#else	// if GD_PLATFORM_API_MICROSOFT
			GD_NOT_USED(destLength);
			return ::strcpy(dest, source);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		GDINL static WideCStr StrcpySafe(WideChar* const dest, SizeTp const destLength, WideCStr const source)
		{
#if GD_PLATFORM_API_MICROSOFT
			auto const result = ::wcscpy_s(dest, destLength, source);
			GD_DEBUG_VERIFY(result == 0, "wcscpy_s failed.");
			return dest;
#else	// if GD_PLATFORM_API_MICROSOFT
			GD_NOT_USED(destLength);
			return ::wcscpy(dest, source);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		//! @}

		/*!
		 * @see @c "std::strncpy" function.
		 */
		//! @{
		GDINL static CStr StrncpySafe(Char* const dest, SizeTp const destLength, CStr const source, SizeTp const maxCount)
		{
#if GD_PLATFORM_API_MICROSOFT
			auto const result = ::strncpy_s(dest, destLength, source, maxCount);
			GD_DEBUG_VERIFY(result == 0, "strncpy_s failed.");
			return dest;
#else	// if GD_PLATFORM_API_MICROSOFT
			GD_NOT_USED(destLength);
			return ::strncpy(dest, source, maxCount);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		GDINL static WideCStr StrncpySafe(WideChar* const dest, SizeTp const destLength, WideCStr const source, SizeTp const maxCount)
		{
#if GD_PLATFORM_API_MICROSOFT
			auto const result = ::wcsncpy_s(dest, destLength, source, maxCount);
			GD_DEBUG_VERIFY(result == 0, "wcsncpy_s failed.");
			return dest;
#else	// if GD_PLATFORM_API_MICROSOFT
			GD_NOT_USED(destLength);
			return ::wcsncpy(dest, source, maxCount);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		//! @}

		/*!
		 * @see @c "std::strcat" function.
		 */
		//! @{
		GDINL static CStr StrcatSafe(Char* const dest, SizeTp const destLength, CStr const source)
		{
#if GD_PLATFORM_API_MICROSOFT
			auto const result = ::strcat_s(dest, destLength, source);
			GD_DEBUG_VERIFY(result == 0, "strcat_s failed.");
			return dest;
#else	// if GD_PLATFORM_API_MICROSOFT
			GD_NOT_USED(destLength);
			return ::strcat(dest, source);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		GDINL static WideCStr StrcatSafe(WideChar* const dest, SizeTp const destLength, WideCStr const source)
		{
#if GD_PLATFORM_API_MICROSOFT
			auto const result = ::wcscat_s(dest, destLength, source);
			GD_DEBUG_VERIFY(result == 0, "wcscat_s failed.");
			return dest;
#else	// if GD_PLATFORM_API_MICROSOFT
			GD_NOT_USED(destLength);
			return ::wcscat(dest, source);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		//! @}

		/*!
		 * @see @c "std::strncat" function.
		 */
		//! @{
		GDINL static CStr StrncatSafe(Char* const dest, SizeTp const destLength, CStr const source, SizeTp const maxCount)
		{
#if GD_PLATFORM_API_MICROSOFT
			auto const result = ::strncat_s(dest, destLength, source, maxCount);
			GD_DEBUG_VERIFY(result == 0, "strncat_s failed.");
			return dest;
#else	// if GD_PLATFORM_API_MICROSOFT
			GD_NOT_USED(destLength);
			return ::strncat(dest, source, maxCount);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		GDINL static WideCStr StrncatSafe(WideChar* const dest, SizeTp const destLength, WideCStr const source, SizeTp const maxCount)
		{
#if GD_PLATFORM_API_MICROSOFT
			auto const result = ::wcsncat_s(dest, destLength, source, maxCount);
			GD_DEBUG_VERIFY(result == 0, "wcsncat_s failed.");
			return dest;
#else	// if GD_PLATFORM_API_MICROSOFT
			GD_NOT_USED(destLength);
			return ::wcsncat(dest, source, maxCount);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		//! @}

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
		 * @see @c "std::strrstr" (does it exist?) function.
		 *
		 * @param cstr C string.
		 * @param sub�Str C sub-string to search for.
		 */
		//! @{
		GDINL static CStr Strrstr(CStr const cstr, CStr const subCStr)
		{
			return ::goddamn_strrstr(cstr, subCStr);
		}
		GDINL static WideChar const* Strrstr(WideChar const* const cstr, WideChar const* const subCStr)
		{
			return ::goddamn_wcsrstr(cstr, subCStr);
		}
		//! @}

		// ... stdlib.h's functions ...

		/*!
		 * @see @c "std::wcstombs/::mbstowcs" function.
		 */
		//! @{
		GDINL static SizeTp StrtombsSafe(Char* const dest, SizeTp const destLength, CStr const source, SizeTp const maxCount)
		{
			StrncpySafe(dest, destLength, source, maxCount);
			return maxCount;
		}
		GDINL static SizeTp StrtombsSafe(Char* const dest, SizeTp const destLength, WideChar const* const source, SizeTp const maxCount)
		{
#if GD_PLATFORM_API_MICROSOFT
			SizeTp output = 0;
			auto const result = ::wcstombs_s(&output, dest, destLength, source, maxCount);
			GD_DEBUG_VERIFY(result == 0, "wcstombs_s failed.");
			return output;
#else	// if GD_PLATFORM_API_MICROSOFT
			GD_NOT_USED(destLength);
			return ::wcstombs(dest, source, maxCount);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		//! @}

		/*!
		 * @see @c "std::wcstombs/::mbstowcs" function.
		 */
		//! @{
		GDINL static SizeTp StrtowcsSafe(WideChar* const dest, SizeTp const destLength, CStr const source, SizeTp const maxCount)
		{
#if GD_PLATFORM_API_MICROSOFT
			SizeTp output = 0;
			auto const result = ::mbstowcs_s(&output, dest, destLength, source, maxCount);
			GD_DEBUG_VERIFY(result == 0, "mbstowcs_s failed.");
			return output;
#else	// if GD_PLATFORM_API_MICROSOFT
			GD_NOT_USED(destLength);
			return ::mbstowcs(dest, source, maxCount);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		GDINL static SizeTp StrtowcsSafe(Char* const dest, SizeTp const destLength, CStr const source, SizeTp const maxCount)
		{
			StrncpySafe(dest, destLength, source, maxCount);
			return maxCount;
		}
		//! @}

		/*!
		 * @see @c "std::strtoull" function.
		 */
		//! @{
		GDINL static UInt64 Strtoui64(CStr const cstr, Char** const endPtr = nullptr, SizeTp const base = 0)
		{
#if GD_PLATFORM_API_MICROSOFT
			return ::_strtoui64(cstr, endPtr, static_cast<int>(base));
#else	// if GD_PLATFORM_API_MICROSOFT
			static_assert(sizeof(UInt64) <= sizeof(unsigned long long), "'unsigned long long' is not large enough to store 'UInt64' type.");
			return static_cast<UInt64>(::strtoull(cstr, endPtr, static_cast<int>(base)));
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		GDINL static UInt64 Strtoui64(WideChar const* const cstr, WideChar** const endPtr = nullptr, SizeTp const base = 0)
		{
#if GD_PLATFORM_API_MICROSOFT
			return ::_wcstoui64(cstr, endPtr, static_cast<int>(base));
#else	// if GD_PLATFORM_API_MICROSOFT
			static_assert(sizeof(UInt64) <= sizeof(unsigned long long), "'unsigned long long' is not large enough to store 'UInt64' type.");
			return static_cast<UInt64>(::wcstoull(cstr, endPtr, static_cast<int>(base)));
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		//! @}

		/*!
		 * @see @c "std::strtoll" function.
		 */
		//! @{
		GDINL static Int64 Strtoi64(CStr const cstr, CStr* const endPtr = nullptr, SizeTp const base = 0)
		{
#if GD_PLATFORM_API_MICROSOFT
			return ::_strtoi64(cstr, const_cast<char**>(endPtr), static_cast<int>(base));
#else	// if GD_PLATFORM_API_MICROSOFT
			static_assert(sizeof(Int64) <= sizeof(long long), "'long long' is not large enough to store 'Int64' type.");
			return static_cast<Int64>(::strtoll(cstr, const_cast<char**>(endPtr), static_cast<int>(base)));
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		GDINL static Int64 Strtoi64(WideChar const* const cstr, WideChar const** const endPtr = nullptr, SizeTp const base = 0)
		{
#if GD_PLATFORM_API_MICROSOFT
			return ::_wcstoi64(cstr, const_cast<wchar_t**>(endPtr), static_cast<int>(base));
#else	// if GD_PLATFORM_API_MICROSOFT
			static_assert(sizeof(Int64) <= sizeof(long long), "'long long' is not large enough to store 'Int64' type.");
			return static_cast<Int64>(::wcstoll(cstr, const_cast<wchar_t**>(endPtr), static_cast<int>(base)));
#endif	// if GD_PLATFORM_API_MICROSOFT
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

		// ... stdio.h functions ...

		/*!
		 * @see @c "std::vsnprintf" function.
		 */
		//! @{
		GDINL static Int32 VsnprintfSafe(Char* const dest, SizeTp const destLength, CStr const format, va_list arguments)
		{
#if GD_PLATFORM_API_MICROSOFT
			__pragma(warning(push))
			__pragma(warning(suppress : 4996))
			return ::_vsnprintf_s(dest, destLength, _TRUNCATE, format, arguments);	// NOLINT
			__pragma(warning(pop))
#else	// if GD_PLATFORM_API_MICROSOFT
			return ::vsnprintf(dest, destLength, format, arguments);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		GDINL static Int32 VsnprintfSafe(WideChar* const dest, SizeTp const destLength, WideCStr const format, va_list arguments)
		{
#if GD_PLATFORM_API_MICROSOFT
			__pragma(warning(push))
			__pragma(warning(suppress : 4996))
			return ::_vsnwprintf_s(dest, destLength, _TRUNCATE, format, arguments);	// NOLINT
			__pragma(warning(pop))
#else	// if GD_PLATFORM_API_MICROSOFT
			return ::vswprintf(dest, destLength, format, arguments);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		//! @}

		/*!
		 * @see @c "std::snprintf" function.
		 */
		template<typename TChar>
		GDINL static int SnprintfSafe(TChar* const dest, SizeTp const destLength, TChar const* const format, ...)
		{
			va_list arguments;
			va_start(arguments, format);
			int const result = VsnprintfSafe(dest, destLength, format, arguments);
			va_end(arguments);
			return result;
		}

		/*!
		 * @see @c "std::vsscanf" function.
		 */
		//! @{
		GDINL static Int32 VsscanfSafe(CStr const source, CStr const format, va_list arguments)	// NOLINT
		{
#if GD_PLATFORM_API_MICROSOFT
			return ::vsscanf_s(source, format, arguments);
#else	// if GD_PLATFORM_API_MICROSOFT
			return ::vsscanf(source, format, arguments);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		GDINL static Int32 VsscanfSafe(WideCStr const source, WideCStr const format, va_list arguments)	// NOLINT
		{
#if GD_PLATFORM_API_MICROSOFT
			return ::vswscanf_s(source, format, arguments);
#else	// if GD_PLATFORM_API_MICROSOFT
			return ::vswscanf(source, format, arguments);
#endif	// if GD_PLATFORM_API_MICROSOFT
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
			int const result = VsscanfSafe(source, format, arguments);
			va_end(arguments);
			return result;
		}
	};	// class BaseCString

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
