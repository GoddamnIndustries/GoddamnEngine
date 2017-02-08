// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
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

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Provides functions for C string. Contains wrapped "str*" methods and methods from 'string.h', 
	//! 'stdlib.h' and 'stdio.h'.
	//!
	//! @tparam TChar Specified character type.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	template<typename TChar>
	class BaseCString final : public TNonCreatable
	{
	public:
		// ... string.h & wchar.h's functions ...

		/*!
		 * @see @c "std::strcpy" function.
		 */
		//! @{
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, TTChar*>::Type Strcpy_s(
			TChar* const dest, SizeTp const destLength, TChar const* const source)
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
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, TTChar*>::Type Strcpy_s(
			TChar* const dest, SizeTp const destLength, TChar const* const source)
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
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, TTChar*>::Type Strncpy_s(
			TChar* const dest, SizeTp const destLength, TChar const* const source, SizeTp const maxCount)
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
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, TTChar*>::Type Strncpy_s(
			TChar* const dest, SizeTp const destLength, TChar const* const source, SizeTp const maxCount)
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
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, TTChar*>::Type Strcat_s(
			TChar* const dest, SizeTp const destLength, TChar const* const source)
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
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, TTChar*>::Type Strcat_s(
			TChar* const dest, SizeTp const destLength, TChar const* const source)
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
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, TTChar*>::Type Strncat_s(
			TChar* const dest, SizeTp const destLength, TChar const* const source, SizeTp const maxCount)
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
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, TTChar*>::Type Strncat_s(
			TChar* const dest, SizeTp const destLength, TChar const* const source, SizeTp const maxCount)
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
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, int>::Type Strcmp(
			TChar const* const lhs, TChar const* const rhs)
		{
			return ::strcmp(lhs, rhs);
		}
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, int>::Type Strcmp(
			TChar const* const lhs, TChar const* const rhs)
		{
			return ::wcscmp(lhs, rhs);
		}
		//! @}

		/*!
		 * @see @c "std::strncmp" function.
		 */
		//! @{
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, int>::Type Strncmp(
			TChar const* const lhs, TChar const* const rhs, SizeTp const maxCount)
		{
			return ::strncmp(lhs, rhs, maxCount);
		}
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, int>::Type Strncmp(
			TChar const* const lhs, TChar const* const rhs, SizeTp const maxCount)
		{
			return ::wcsncmp(lhs, rhs, maxCount);
		}
		//! @}

		/*!
		 * @see @c "std::strlen" function.
		 */
		//! @{
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, SizeTp>::Type Strlen(
			TChar const* const cstr)
		{
			return ::strlen(cstr);
		}
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, SizeTp>::Type Strlen(
			TChar const* const cstr)
		{
			return ::wcslen(cstr);
		}
		//! @}

		/*!
		 * @see @c "std::strchr" function.
		 */
		//! @{
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, TChar const*>::Type Strchr(
			TChar const* const cstr, TChar const Chr)
		{
			return ::strchr(cstr, static_cast<int>(Chr));
		}
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, TChar const*>::Type Strchr(
			TChar const* const cstr, TChar const Chr)
		{
			return ::wcschr(cstr, static_cast<wchar_t>(Chr));
		}
		GDINL static TChar* Strchr(TChar* const cstr, TChar const Chr)
		{
			return const_cast<TChar*>(Strchr(const_cast<TChar const*>(cstr), Chr));
		}
		//! @}

		/*!
		 * @see @c "std::strrchr" function.
		 */
		//! @{
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, TChar const*>::Type Strrchr(
			TChar const* const cstr, TChar const Chr)
		{
			return ::strrchr(cstr, static_cast<int>(Chr));
		}
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, TChar const*>::Type Strrchr(
			TChar const* const cstr, TChar const Chr)
		{
			return ::wcsrchr(cstr, static_cast<wchar_t>(Chr));
		}
		GDINL static TChar* Strrchr(TChar* const cstr, TChar const Chr)
		{
			return const_cast<TChar*>(Strrchr(const_cast<TChar const*>(cstr), Chr));
		}
		//! @}

		/*!
		 * @see @c "std::strstr" function.
		 */
		//! @{
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, TChar const*>::Type Strstr(
			TChar const* const cstr, TChar const* const subStr)
		{
			return ::strstr(cstr, subStr);
		}
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, TChar const*>::Type Strstr(
			TChar const* const cstr, TChar const* const subStr)
		{
			return ::wcsstr(cstr, subStr);
		}
		GDINL static TChar* Strstr(TChar* const cstr, TChar const Chr)
		{
			return const_cast<TChar*>(Strstr(const_cast<TChar const*>(cstr), Chr));
		}
		//! @}

		/*!
		 * Returns pointer to last occurrence of a specified C sub-string in the C string.
		 *
		 * @param cstr C string.
		 * @param subStr C sub-string to search for.
		 *
		 * @returns Pointer to last occurrence of a specified C sub-string in the string.
		 *
		 * @see @c "std::strrstr" (does it exist?) function.
		 */
		//! @{
		GDINL static TChar const* Strrstr(TChar const* const cstr, TChar const* const subStr)
		{
			//! @todo Maybe does exist a better way to do this?
			SizeTp const strLen = Strlen(cstr);
			SizeTp const subStrLength = Strlen(subStr);
			if (strLen >= subStrLength) 
			{
				typedef TChar const* IteratorType;
				IteratorType const startIterator = cstr + strLen - subStrLength;
				IteratorType const endIterator = cstr - 1;
				for (IteratorType iterator = startIterator; iterator != endIterator; --iterator) 
				{
					TChar const* const Location = Strstr(iterator, subStr);
					if (Location != nullptr)
					{
						return Location;
					}
				}
			}
			return nullptr;
		}
		GDINL static TChar* Strrstr(TChar* const cstr, TChar const Chr)
		{
			return const_cast<TChar*>(Strrstr(const_cast<TChar const*>(cstr), Chr));
		}
		//! @}

		// ... stdlib.h's functions ...

		/*!
		 * @see @c "std::wcstombs/::mbstowcs" function.
		 */
		//! @{
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, SizeTp>::Type Strtombs_s(
			Char* const dest, SizeTp const destLength, TChar const* const source, SizeTp const maxCount)
		{
			Strncpy_s(dest, destLength, source, maxCount);
			return maxCount;
		}
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, SizeTp>::Type Strtombs_s(
			Char* const dest, SizeTp const destLength, TChar const* const source, SizeTp const maxCount)
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
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, SizeTp>::Type Strtowcs_s(
			Char* const dest, SizeTp const destLength, TChar const* const source, SizeTp const maxCount)
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
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, SizeTp>::Type Strtowcs_s(
			Char* const dest, SizeTp const destLength, TChar const* const source, SizeTp const maxCount)
		{
			Strncpy_s(dest, destLength, source, maxCount);
			return maxCount;
		}
		//! @}

		/*!
		 * @see @c "std::strtoull" function.
		 */
		//! @{
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, UInt64>::Type Strtoui64(
			TChar const* const cstr, TChar const** const endPtr = nullptr, SizeTp const radix = 0)
		{
#if GD_PLATFORM_API_MICROSOFT
			return ::_strtoui64(cstr, endPtr, static_cast<int>(radix));
#else	// if GD_PLATFORM_API_MICROSOFT
			static_assert(sizeof(UInt64) <= sizeof(unsigned long long), "'unsigned long long' is not large enough to store 'UInt64' type.");
			return static_cast<UInt64>(::strtoull(cstr, endPtr, static_cast<int>(radix)));
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, UInt64>::Type Strtoui64(
			TChar const* const cstr, TChar const** const endPtr = nullptr, SizeTp const radix = 0)
		{
#if GD_PLATFORM_API_MICROSOFT
			return ::_wcstoui64(cstr, endPtr, static_cast<int>(radix));
#else	// if GD_PLATFORM_API_MICROSOFT
			static_assert(sizeof(UInt64) <= sizeof(unsigned long long), "'unsigned long long' is not large enough to store 'UInt64' type.");
			return static_cast<UInt64>(::wcstoull(cstr, endPtr, static_cast<int>(radix)));
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		//! @}

		/*!
		 * @see @c "std::strtoll" function.
		 */
		//! @{
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, Int64>::Type Strtoi64(
			TChar const* const cstr, TChar const** const endPtr = nullptr, SizeTp const radix = 0)
		{
#if GD_PLATFORM_API_MICROSOFT
			return ::_strtoi64(cstr, const_cast<char**>(endPtr), static_cast<int>(radix));
#else	// if GD_PLATFORM_API_MICROSOFT
			static_assert(sizeof(Int64) <= sizeof(long long), "'long long' is not large enough to store 'Int64' type.");
			return static_cast<Int64>(::strtoll(cstr, const_cast<char**>(endPtr), static_cast<int>(radix)));
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, Int64>::Type Strtoi64(
			TChar const* const cstr, TChar const** const endPtr = nullptr, SizeTp const radix = 0)
		{
#if GD_PLATFORM_API_MICROSOFT
			return ::_wcstoi64(cstr, const_cast<wchar_t**>(endPtr), static_cast<int>(radix));
#else	// if GD_PLATFORM_API_MICROSOFT
			static_assert(sizeof(Int64) <= sizeof(long long), "'long long' is not large enough to store 'Int64' type.");
			return static_cast<Int64>(::wcstoll(cstr, const_cast<wchar_t**>(endPtr), static_cast<int>(radix)));
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		//! @}

		/*!
		 * @see @c "std::strtod" function.
		 */
		//! @{
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, Float64>::Type Strtof64(
			TChar const* const cstr, TChar const** const endPtr = nullptr)
		{
			return ::strtod(cstr, const_cast<char**>(endPtr));
		}
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, Float64>::Type Strtof64(
			TChar const* const cstr, TChar const** const endPtr = nullptr)
		{
			return ::wcstod(cstr, const_cast<wchar_t**>(endPtr));
		}
		//! @}

		// ... stdio.h functions ...

		/*!
		 * @see @c "std::vsnprintf" function.
		 */
		//! @{
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, int>::Type Vsnprintf_s(
			TChar* const dest, SizeTp const destLength, TChar const* const format, va_list arguments)
		{
#if GD_PLATFORM_API_MICROSOFT
			__pragma(warning(suppress : 4996))
			return ::_vsnprintf_s(dest, destLength, _TRUNCATE, format, arguments);
#else	// if GD_PLATFORM_API_MICROSOFT
			return ::vsnprintf(dest, destLength, format, arguments);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, int>::Type Vsnprintf_s(
			TChar* const dest, SizeTp const destLength, TChar const* const format, va_list arguments)
		{
#if GD_PLATFORM_API_MICROSOFT
			__pragma(warning(suppress : 4996))
			return ::_vsnwprintf_s(dest, destLength, _TRUNCATE, format, arguments);
#else	// if GD_PLATFORM_API_MICROSOFT
			return ::vswprintf(dest, destLength, format, arguments);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		//! @}

		/*!
		 * @see @c "std::snprintf" function.
		 */
		GDINL static int Snprintf_s(TChar* const dest, SizeTp const destLength, TChar const* const format, ...)
		{
			va_list arguments;
			va_start(arguments, format);
			int const result = Vsnprintf_s(dest, destLength, format, arguments);
			va_end(arguments);
			return result;
		}

		/*!
		 * @see @c "std::vsscanf" function.
		 */
		//! @{
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, int>::Type Vsscanf_s(
			TChar const* const source, TChar const* const format, va_list arguments)
		{
#if GD_PLATFORM_API_MICROSOFT
			return ::vsscanf_s(source, format, arguments);
#else	// if GD_PLATFORM_API_MICROSOFT
			return ::vsscanf(source, format, arguments);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, int>::Type Vsscanf_s(
			TChar const* const source, TChar const* const format, va_list arguments)
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
		GDINL static int Sscanf(TChar const* const source, TChar const* const format, ...)
		{
			va_list arguments;
			va_start(arguments, format);
			int const result = Vsscanf_s(source, format, arguments);
			va_end(arguments);
			return result;
		}
	};	// class BaseCString

	/*!
	 * Declarations used to ban standard functions. 
	 */
	enum LibStringUnallowedFunctions
	{
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
	};	// enum LibCharUnallowedFunctions

	typedef BaseCString<Char> CString;
	typedef BaseCString<WideChar> WideCString;

GD_NAMESPACE_END
