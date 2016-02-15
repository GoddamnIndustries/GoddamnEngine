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

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Provides functions for C string. Contains wrapped "str*" methods and methods from 'string.h', 
	//! 'stdlib.h' and 'stdio.h'.
	//! @tparam TChar Specified character type.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	template<typename TChar>
	class BaseCString final : public TNonCreatable
	{
	public:
		// ... string.h & wchar.h's functions ...

		//! @see @c "std::strcpy" function.
		//! @{
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, TTChar*>::Type Strcpy(
			TChar* const dest, SizeTp const destLength, TChar const* const source)
		{
#if GD_PLATFORM_API_MICROSOFT
			auto const result = ::strcpy_s(dest, destLength, source);
			GD_DEBUG_ASSERT(result == 0, "strcpy_s failed.");
			return dest;
#else	// if GD_PLATFORM_API_MICROSOFT
			GD_NOT_USED(destLength);
			return ::strcpy(dest, source);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, TTChar*>::Type Strcpy(
			TChar* const dest, SizeTp const destLength, TChar const* const source)
		{
#if GD_PLATFORM_API_MICROSOFT
			auto const result = ::wcscpy_s(dest, destLength, source);
			GD_DEBUG_ASSERT(result == 0, "wcscpy_s failed.");
			return dest;
#else	// if GD_PLATFORM_API_MICROSOFT
			GD_NOT_USED(destLength);
			return ::wcscpy(dest, source);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		//! @}

		//! @see @c "std::strncpy" function.
		//! @{
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, TTChar*>::Type Strncpy(
			TChar* const dest, SizeTp const destLength, TChar const* const source, SizeTp const maxCount)
		{
#if GD_PLATFORM_API_MICROSOFT
			auto const result = ::strncpy_s(dest, destLength, source, maxCount);
			GD_DEBUG_ASSERT(result == 0, "strncpy_s failed.");
			return dest;
#else	// if GD_PLATFORM_API_MICROSOFT
			GD_NOT_USED(destLength);
			return ::strncpy(dest, source, maxCount);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, TTChar*>::Type Strncpy(
			TChar* const dest, SizeTp const destLength, TChar const* const source, SizeTp const maxCount)
		{
#if GD_PLATFORM_API_MICROSOFT
			auto const result = ::wcsncpy_s(dest, destLength, source, maxCount);
			GD_DEBUG_ASSERT(result == 0, "wcsncpy_s failed.");
			return dest;
#else	// if GD_PLATFORM_API_MICROSOFT
			GD_NOT_USED(destLength);
			return ::wcsncpy(dest, source, maxCount);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		//! @}

		//! @see @c "std::strcat" function.
		//! @{
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, TTChar*>::Type Strcat(
			TChar* const dest, SizeTp const destLength, TChar const* const source)
		{
#if GD_PLATFORM_API_MICROSOFT
			auto const result = ::strcat_s(dest, destLength, source);
			GD_DEBUG_ASSERT(result == 0, "strcat_s failed.");
			return dest;
#else	// if GD_PLATFORM_API_MICROSOFT
			GD_NOT_USED(destLength);
			return ::strcat(dest, source);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, TTChar*>::Type Strcat(
			TChar* const dest, SizeTp const destLength, TChar const* const source)
		{
#if GD_PLATFORM_API_MICROSOFT
			auto const result = ::wcscat_s(dest, destLength, source);
			GD_DEBUG_ASSERT(result == 0, "wcscat_s failed.");
			return dest;
#else	// if GD_PLATFORM_API_MICROSOFT
			GD_NOT_USED(destLength);
			return ::wcscat(dest, source);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		//! @}

		//! @see @c "std::strncat" function.
		//! @{
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, TTChar*>::Type Strncat(
			TChar* const dest, SizeTp const destLength, TChar const* const source, SizeTp const maxCount)
		{
#if GD_PLATFORM_API_MICROSOFT
			auto const result = ::strncat_s(dest, destLength, source, maxCount);
			GD_DEBUG_ASSERT(result == 0, "strncat_s failed.");
			return dest;
#else	// if GD_PLATFORM_API_MICROSOFT
			GD_NOT_USED(destLength);
			return ::strncat(dest, source, maxCount);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, TTChar*>::Type Strncat(
			TChar* const dest, SizeTp const destLength, TChar const* const source, SizeTp const maxCount)
		{
#if GD_PLATFORM_API_MICROSOFT
			auto const result = ::wcsncat_s(dest, destLength, source, maxCount);
			GD_DEBUG_ASSERT(result == 0, "wcsncat_s failed.");
			return dest;
#else	// if GD_PLATFORM_API_MICROSOFT
			GD_NOT_USED(destLength);
			return ::wcsncat(dest, source, maxCount);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		//! @}

		//! @see @c "std::strcmp" function.
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

		//! @see @c "std::strncmp" function.
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

		//! @see @c "std::strlen" function.
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

		//! @see @c "std::strchr" function.
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

		//! @see @c "std::strrchr" function.
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

		//! @see @c "std::strstr" function.
		//! @{
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, TChar const*>::Type Strstr(
			TChar const* const cstr, TChar const* const subStr)
		{
			return ::strstr(cstr, subStr);
		}
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, TChar const*>::Type Strchr(
			TChar const* const cstr, TChar const* const subStr)
		{
			return ::wcsstr(cstr, subStr);
		}
		GDINL static TChar* Strstr(TChar* const cstr, TChar const* const subStr)
		{
			return const_cast<TChar*>(Strstr(const_cast<TChar const*>(cstr), subStr));
		}
		//! @}

		//! Returns pointer to last occurrence of a specified C sub-string in the C string.
		//! @param cstr C string.
		//! @param subStr C sub-string to search for.
		//! @returns pointer to last occurrence of a specified C sub-string in the string.
		//! @see @c "std::strrstr" (does it exist?) function.
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
						return Location;
				}
			}

			return nullptr;
		}
		GDINL static TChar* Strrstr(TChar* const cstr, TChar const* const subStr)
		{
			return const_cast<TChar*>(Strrstr(const_cast<TChar const*>(cstr), subStr));
		}
		//! @}

		// ... stdlib.h's functions ...

		//! @see @c "std::wcstombs/::mbstowcs" function.
		//! @{
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, SizeTp>::Type Strtombs(
			Char* const dest, SizeTp const destLength, TChar const* const source, SizeTp const maxCount)
		{
			Strncpy(dest, destLength, source, maxCount);
			return maxCount;
		}
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, SizeTp>::Type Strtombs(
			Char* const dest, SizeTp const destLength, TChar const* const source, SizeTp const maxCount)
		{
#if GD_PLATFORM_API_MICROSOFT
			SizeTp output = 0;
			auto const result = ::wcstombs_s(&output, dest, destLength, source, maxCount);
			GD_DEBUG_ASSERT(result == 0, "wcstombs_s failed.");
			return output;
#else	// if GD_PLATFORM_API_MICROSOFT
			GD_NOT_USED(destLength);
			return ::wcstombs(dest, source, maxCount);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		//! @}

		//! @see @c "std::wcstombs/::mbstowcs" function.
		//! @{
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, SizeTp>::Type Strtowcs(
			Char* const dest, SizeTp const destLength, TChar const* const source, SizeTp const maxCount)
		{
#if GD_PLATFORM_API_MICROSOFT
			SizeTp output = 0;
			auto const result = ::mbstowcs_s(&output, dest, destLength, source, maxCount);
			GD_DEBUG_ASSERT(result == 0, "mbstowcs_s failed.");
			return output;
#else	// if GD_PLATFORM_API_MICROSOFT
			GD_NOT_USED(destLength);
			return ::mbstowcs(dest, source, maxCount);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, SizeTp>::Type Strtowcs(
			Char* const dest, SizeTp const destLength, TChar const* const source, SizeTp const maxCount)
		{
			Strncpy(dest, destLength, source, maxCount);
			return maxCount;
		}
		//! @}

		//! @see @c "std::strtoul{l}" function.
		//! @{
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, UInt64>::Type Strtoui64(
			TChar const* const cstr, TChar const** const EndPtr = nullptr, SizeTp const Notation = 0)
		{
#if GD_PLATFORM_API_MICROSOFT
			return ::_strtoui64(cstr, EndPtr, static_cast<int>(Notation));
#else	// if GD_PLATFORM_API_MICROSOFT
			static_assert(sizeof(UInt64) <= sizeof(unsigned long long), "'unsigned long long' is not large enough to store 'UInt64' type.");
			return static_cast<UInt64>(::strtoull(cstr, EndPtr, static_cast<int>(Notation)));
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, UInt64>::Type Strtoui64(
			TChar const* const cstr, TChar const** const EndPtr = nullptr, SizeTp const Notation = 0)
		{
#if GD_PLATFORM_API_MICROSOFT
			return ::_wcstoui64(cstr, EndPtr, static_cast<int>(Notation));
#else	// if GD_PLATFORM_API_MICROSOFT
			static_assert(sizeof(UInt64) <= sizeof(unsigned long long), "'unsigned long long' is not large enough to store 'UInt64' type.");
			return static_cast<UInt64>(::wcstoull(cstr, EndPtr, static_cast<int>(Notation)));
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		//! @}

		//! @see @c "std::strtol{l}" function.
		//! @{
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, Int64>::Type Strtoi64(
			TChar const* const cstr, TChar const** const EndPtr = nullptr, SizeTp const Notation = 0)
		{
#if GD_PLATFORM_API_MICROSOFT
			return ::_strtoi64(cstr, const_cast<char**>(EndPtr), static_cast<int>(Notation));
#else	// if GD_PLATFORM_API_MICROSOFT
			static_assert(sizeof(Int64) <= sizeof(long long), "'long long' is not large enough to store 'Int64' type.");
			return static_cast<Int64>(::strtoll(cstr, const_cast<char**>(EndPtr), static_cast<int>(Notation)));
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, Int64>::Type Strtoi64(
			TChar const* const cstr, TChar const** const EndPtr = nullptr, SizeTp const Notation = 0)
		{
#if GD_PLATFORM_API_MICROSOFT
			return ::_wcstoi64(cstr, const_cast<wchar_t**>(EndPtr), static_cast<int>(Notation));
#else	// if GD_PLATFORM_API_MICROSOFT
			static_assert(sizeof(Int64) <= sizeof(long long), "'long long' is not large enough to store 'Int64' type.");
			return static_cast<Int64>(::wcstoll(cstr, const_cast<wchar_t**>(EndPtr), static_cast<int>(Notation)));
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		//! @}

#if GD_FALSE
		//! @see @c "std::strtof" function.
		//! @{
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, Float32>::Type Strtof32(
			TChar const* const cstr, TChar const** const EndPtr = nullptr)
		{
			return ::strtof(cstr, const_cast<char**>(EndPtr));
		}
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, Float32>::Type Strtof32(
			TChar const* const cstr, TChar const** const EndPtr = nullptr)
		{
			return ::wcstof(cstr, const_cast<wchar_t**>(EndPtr));
		}
		//! @}
#endif	// if GD_FALSE

		//! @see @c "std::strtod" function.
		//! @{
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, Float64>::Type Strtof64(
			TChar const* const cstr, TChar const** const EndPtr = nullptr)
		{
			return ::strtod(cstr, const_cast<char**>(EndPtr));
		}
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, Float64>::Type Strtof64(
			TChar const* const cstr, TChar const** const EndPtr = nullptr)
		{
			return ::wcstod(cstr, const_cast<wchar_t**>(EndPtr));
		}
		//! @}

		// ... stdio.h functions ...

		//! @see @c "std::vsnprintf" function.
		//! @{
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, int>::Type Vsnprintf(
			TChar* const dest, SizeTp const destLength, TChar const* const format, va_list Arguments)
		{
#if GD_PLATFORM_API_MICROSOFT
			__pragma(warning(suppress : 4996))
			return ::_vsnprintf_s(dest, destLength, _TRUNCATE, format, Arguments);
#else	// if GD_PLATFORM_API_MICROSOFT
			return ::vsnprintf(dest, destLength, format, Arguments);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, int>::Type Vsnprintf(
			TChar* const dest, SizeTp const destLength, TChar const* const format, va_list Arguments)
		{
#if GD_PLATFORM_API_MICROSOFT
			__pragma(warning(suppress : 4996))
			return ::_vsnwprintf_s(dest, destLength, _TRUNCATE, format, Arguments);
#else	// if GD_PLATFORM_API_MICROSOFT
			return ::vswprintf(dest, destLength, format, Arguments);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		//! @}

		//! @see @c "std::snprintf" function.
		GDINL static int Snprintf(TChar* const dest, SizeTp const destLength, TChar const* const format, ...)
		{
			va_list Arguments;
			va_start(Arguments, format);
			int const result = Vsnprintf(dest, destLength, format, Arguments);
			va_end(Arguments);
			return result;
		}

		//! @see @c "std::vsscanf" function.
		//! @{
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, int>::Type Vsscanf(
			TChar const* const source, TChar const* const format, va_list Arguments)
		{
#if GD_PLATFORM_API_MICROSOFT
			return ::vsscanf_s(source, format, Arguments);
#else	// if GD_PLATFORM_API_MICROSOFT
			return ::vsscanf(source, format, Arguments);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, int>::Type Vsscanf(
			TChar const* const source, TChar const* const format, va_list Arguments)
		{
#if GD_PLATFORM_API_MICROSOFT
			return ::vswscanf_s(source, format, Arguments);
#else	// if GD_PLATFORM_API_MICROSOFT
			return ::vswscanf(source, format, Arguments);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		//! @}

		//! @see @c "std::sscanf" function.
		GDINL static int Sscanf(TChar const* const source, TChar const* const format, ...)
		{
			va_list Arguments;
			va_start(Arguments, format);
			int const result = Vsscanf(source, format, Arguments);
			va_end(Arguments);
			return result;
		}
	};	// class BaseCString

	namespace CStdLibBannedFunctions
	{
		enum
		{
			strcpy, strcpy_s, wcscpy, wcscpy_s,

		};	// anonymous enum
	}	// namespace CStdLibBannedFunctions

	typedef BaseCString<Char> CString;
	typedef BaseCString<WideChar> WideCString;

GD_NAMESPACE_END
