//! ==========================================================================================
//! cstring.h - Traits, helper functions and definitions for C strings.
//! Copyright (C) Goddamn Industries 2015. All Rights Reserved.
//! ==========================================================================================

//! @file GoddamnEngine/Core/Misc/CString.h
//! Traits, helper functions and definitions for standard string/memory functions.
#pragma once

#include <GoddamnEngine/Core/Base/CStdlib/CChar.h>

GD_NAMESPACE_BEGIN

	// ------------------------------------------------------------------------------------------
	//! Provides functions for C string. Contains wrapped "str*" methods and methods from 'string.h', 
	//! 'stdlib.h' and 'stdio.h'.
	//! @tparam CharType Specified character type.
	template<typename CharType>
	class BaseCString final : public IUncreatable
	{
	public:
		// ... string.h & wchar.h's functions ...

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::strcpy" function.
		//! @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, Char>::Value, TheCharType*>::Type Strcpy(
			CharType* const dest, SizeTp const destLength, CharType const* const source)
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
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, TheCharType*>::Type Strcpy(
			CharType* const dest, SizeTp const destLength, CharType const* const source)
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

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::strncpy" function.
		//! @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, Char>::Value, TheCharType*>::Type Strncpy(
			CharType* const dest, SizeTp const destLength, CharType const* const source, SizeTp const maxCount)
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
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, TheCharType*>::Type Strncpy(
			CharType* const dest, SizeTp const destLength, CharType const* const source, SizeTp const maxCount)
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

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::strcat" function.
		//! @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, Char>::Value, TheCharType*>::Type Strcat(
			CharType* const dest, SizeTp const destLength, CharType const* const source)
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
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, TheCharType*>::Type Strcat(
			CharType* const dest, SizeTp const destLength, CharType const* const source)
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

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::strncat" function.
		//! @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, Char>::Value, TheCharType*>::Type Strncat(
			CharType* const dest, SizeTp const destLength, CharType const* const source, SizeTp const maxCount)
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
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, TheCharType*>::Type Strncat(
			CharType* const dest, SizeTp const destLength, CharType const* const source, SizeTp const maxCount)
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

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::strcmp" function.
		//! @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, Char>::Value, int>::Type Strcmp(
			CharType const* const lhs, CharType const* const rhs)
		{
			return ::strcmp(lhs, rhs);
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, int>::Type Strcmp(
			CharType const* const lhs, CharType const* const rhs)
		{
			return ::wcscmp(lhs, rhs);
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::strncmp" function.
		//! @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, Char>::Value, int>::Type Strncmp(
			CharType const* const lhs, CharType const* const rhs, SizeTp const maxCount)
		{
			return ::strncmp(lhs, rhs, maxCount);
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, int>::Type Strncmp(
			CharType const* const lhs, CharType const* const rhs, SizeTp const maxCount)
		{
			return ::wcsncmp(lhs, rhs, maxCount);
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::strlen" function.
		//! @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, Char>::Value, SizeTp>::Type Strlen(
			CharType const* const cstr)
		{
			return ::strlen(cstr);
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, SizeTp>::Type Strlen(
			CharType const* const cstr)
		{
			return ::wcslen(cstr);
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::strchr" function.
		//! @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, Char>::Value, CharType const*>::Type Strchr(
			CharType const* const cstr, CharType const Chr)
		{
			return ::strchr(cstr, static_cast<int>(Chr));
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, CharType const*>::Type Strchr(
			CharType const* const cstr, CharType const Chr)
		{
			return ::wcschr(cstr, static_cast<wchar_t>(Chr));
		}
		GDINL static CharType* Strchr(CharType* const cstr, CharType const Chr)
		{
			return const_cast<CharType*>(Strchr(const_cast<CharType const*>(cstr), Chr));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::strrchr" function.
		//! @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, Char>::Value, CharType const*>::Type Strrchr(
			CharType const* const cstr, CharType const Chr)
		{
			return ::strrchr(cstr, static_cast<int>(Chr));
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, CharType const*>::Type Strrchr(
			CharType const* const cstr, CharType const Chr)
		{
			return ::wcsrchr(cstr, static_cast<wchar_t>(Chr));
		}
		GDINL static CharType* Strrchr(CharType* const cstr, CharType const Chr)
		{
			return const_cast<CharType*>(Strrchr(const_cast<CharType const*>(cstr), Chr));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::strstr" function.
		//! @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, Char>::Value, CharType const*>::Type Strstr(
			CharType const* const cstr, CharType const* const subStr)
		{
			return ::strstr(cstr, subStr);
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, CharType const*>::Type Strchr(
			CharType const* const cstr, CharType const* const subStr)
		{
			return ::wcsstr(cstr, subStr);
		}
		GDINL static CharType* Strstr(CharType* const cstr, CharType const* const subStr)
		{
			return const_cast<CharType*>(Strstr(const_cast<CharType const*>(cstr), subStr));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns pointer to last occurrence of a specified C sub-string in the C string.
		//! @param cstr C string.
		//! @param subStr C sub-string to search for.
		//! @returns pointer to last occurrence of a specified C sub-string in the string.
		//! @see @c "std::strrstr" (does it exist?) function.
		//! @{
		GDINL static CharType const* Strrstr(CharType const* const cstr, CharType const* const subStr)
		{
			//! @todo Maybe does exist a better way to do this?
			SizeTp const strLen = Strlen(cstr);
			SizeTp const subStrLength = Strlen(subStr);
			if (strLen >= subStrLength) 
			{
				typedef CharType const* IteratorType;
				IteratorType const startIterator = cstr + strLen - subStrLength;
				IteratorType const endIterator = cstr - 1;
				for (IteratorType iterator = startIterator; iterator != endIterator; --iterator) 
				{
					CharType const* const Location = Strstr(iterator, subStr);
					if (Location != nullptr) 
						return Location;
				}
			}

			return nullptr;
		}
		GDINL static CharType* Strrstr(CharType* const cstr, CharType const* const subStr)
		{
			return const_cast<CharType*>(Strrstr(const_cast<CharType const*>(cstr), subStr));
		}
		//! @}

		// ... stdlib.h's functions ...

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::wcstombs/::mbstowcs" function.
		//! @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, Char>::Value, SizeTp>::Type Strtombs(
			Char* const dest, SizeTp const destLength, CharType const* const source, SizeTp const maxCount)
		{
			Strncpy(dest, destLength, source, maxCount);
			return maxCount;
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, SizeTp>::Type Strtombs(
			Char* const dest, SizeTp const destLength, CharType const* const source, SizeTp const maxCount)
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

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::wcstombs/::mbstowcs" function.
		//! @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, Char>::Value, SizeTp>::Type Strtowcs(
			Char* const dest, SizeTp const destLength, CharType const* const source, SizeTp const maxCount)
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
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, SizeTp>::Type Strtowcs(
			Char* const dest, SizeTp const destLength, CharType const* const source, SizeTp const maxCount)
		{
			Strncpy(dest, destLength, source, maxCount);
			return maxCount;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::strtoul{l}" function.
		//! @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, Char>::Value, UInt64>::Type Strtoui64(
			CharType const* const cstr, CharType const** const EndPtr = nullptr, SizeTp const Notation = 0)
		{
#if GD_PLATFORM_API_MICROSOFT
			return ::_strtoui64(cstr, EndPtr, static_cast<int>(Notation));
#else	// if GD_PLATFORM_API_MICROSOFT
			static_assert(sizeof(UInt64) <= sizeof(unsigned long long), "'unsigned long long' is not large enough to store 'UInt64' type.");
			return static_cast<UInt64>(::strtoull(cstr, EndPtr, static_cast<int>(Notation)));
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, UInt64>::Type Strtoui64(
			CharType const* const cstr, CharType const** const EndPtr = nullptr, SizeTp const Notation = 0)
		{
#if GD_PLATFORM_API_MICROSOFT
			return ::_wcstoui64(cstr, EndPtr, static_cast<int>(Notation));
#else	// if GD_PLATFORM_API_MICROSOFT
			static_assert(sizeof(UInt64) <= sizeof(unsigned long long), "'unsigned long long' is not large enough to store 'UInt64' type.");
			return static_cast<UInt64>(::wcstoull(cstr, EndPtr, static_cast<int>(Notation)));
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::strtol{l}" function.
		//! @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, Char>::Value, Int64>::Type Strtoi64(
			CharType const* const cstr, CharType const** const EndPtr = nullptr, SizeTp const Notation = 0)
		{
#if GD_PLATFORM_API_MICROSOFT
			return ::_strtoi64(cstr, const_cast<char**>(EndPtr), static_cast<int>(Notation));
#else	// if GD_PLATFORM_API_MICROSOFT
			static_assert(sizeof(Int64) <= sizeof(long long), "'long long' is not large enough to store 'Int64' type.");
			return static_cast<Int64>(::strtoll(cstr, const_cast<char**>(EndPtr), static_cast<int>(Notation)));
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, Int64>::Type Strtoi64(
			CharType const* const cstr, CharType const** const EndPtr = nullptr, SizeTp const Notation = 0)
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
		// ------------------------------------------------------------------------------------------
		//! @see @c "std::strtof" function.
		//! @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, Char>::Value, Float32>::Type Strtof32(
			CharType const* const cstr, CharType const** const EndPtr = nullptr)
		{
			return ::strtof(cstr, const_cast<char**>(EndPtr));
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, Float32>::Type Strtof32(
			CharType const* const cstr, CharType const** const EndPtr = nullptr)
		{
			return ::wcstof(cstr, const_cast<wchar_t**>(EndPtr));
		}
		//! @}
#endif	// if GD_FALSE

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::strtod" function.
		//! @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, Char>::Value, Float64>::Type Strtof64(
			CharType const* const cstr, CharType const** const EndPtr = nullptr)
		{
			return ::strtod(cstr, const_cast<char**>(EndPtr));
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, Float64>::Type Strtof64(
			CharType const* const cstr, CharType const** const EndPtr = nullptr)
		{
			return ::wcstod(cstr, const_cast<wchar_t**>(EndPtr));
		}
		//! @}

		// ... stdio.h functions ...

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::vsnprintf" function.
		//! @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, Char>::Value, int>::Type Vsnprintf(
			CharType* const dest, SizeTp const destLength, CharType const* const format, va_list Arguments)
		{
#if GD_PLATFORM_API_MICROSOFT
			__pragma(warning(suppress : 4996))
			return ::_vsnprintf_s(dest, destLength, _TRUNCATE, format, Arguments);
#else	// if GD_PLATFORM_API_MICROSOFT
			return ::vsnprintf(dest, destLength, format, Arguments);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, int>::Type Vsnprintf(
			CharType* const dest, SizeTp const destLength, CharType const* const format, va_list Arguments)
		{
#if GD_PLATFORM_API_MICROSOFT
			__pragma(warning(suppress : 4996))
			return ::_vsnwprintf_s(dest, destLength, _TRUNCATE, format, Arguments);
#else	// if GD_PLATFORM_API_MICROSOFT
			return ::vswprintf(dest, destLength, format, Arguments);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::snprintf" function.
		GDINL static int Snprintf(CharType* const dest, SizeTp const destLength, CharType const* const format, ...)
		{
			va_list Arguments;
			va_start(Arguments, format);
			int const result = Vsnprintf(dest, destLength, format, Arguments);
			va_end(Arguments);
			return result;
		}

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::vsscanf" function.
		//! @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, Char>::Value, int>::Type Vsscanf(
			CharType const* const source, CharType const* const format, va_list Arguments)
		{
#if GD_PLATFORM_API_MICROSOFT
			return ::vsscanf_s(source, format, Arguments);
#else	// if GD_PLATFORM_API_MICROSOFT
			return ::vsscanf(source, format, Arguments);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, int>::Type Vsscanf(
			CharType const* const source, CharType const* const format, va_list Arguments)
		{
#if GD_PLATFORM_API_MICROSOFT
			return ::vswscanf_s(source, format, Arguments);
#else	// if GD_PLATFORM_API_MICROSOFT
			return ::vswscanf(source, format, Arguments);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::sscanf" function.
		GDINL static int Sscanf(CharType const* const source, CharType const* const format, ...)
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
