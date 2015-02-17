/// ==========================================================================================
/// CString.h - Traits, helper functions and definitions for C strings.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_MISC_CSTRING
#define GD_CORE_MISC_CSTRING

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Misc/CChar.h>

#include <cstring>	// string.h's 'str*' functions.
#include <cstdlib>	// stdlib.h's 'str*' functions.
#include <cstdarg>	// va_args functions.
#include <cstdio>	// stdio.h's '*print*' and '*scan*' functions.
#include <cwchar>	// wchar.h's 'wcs*' functions.

/// @brief Define this to 1 to forcedly use generic standart function instead of platform onces.
#define GD_CSTRING_FORCE_USE_GENERIC 0

#if GD_PLATFORM_API_WINAPI && (!GD_CSTRING_FORCE_USE_GENERIC)

/// @brief Enable this to use Microsoft-specific safe functions.
#define GD_CSTRING_USE_MICROSOFT_EXT 1

#endif	// if GD_PLATFORM_API_WINAPI && (!GD_CSTRING_FORCE_USE_GENERIC)

#if GD_CSTRING_USE_MICROSOFT_EXT

/// @brief Selects Microsoft-specific or generic code.
/// @param ... Specified code.
/// @{
#define GD_CSTRING_MICROSOFT(...) { __VA_ARGS__ }
#define GD_CSTRING_GENERIC(...)
/// @}

#else	// if GD_CSTRING_USE_MICROSOFT_EXT

/// @brief Selects Microsoft-specific or generic code.
/// @param ... Specified code.
/// @{
#define GD_CSTRING_MICROSOFT(...) 
#define GD_CSTRING_GENERIC(...) { __VA_ARGS__ }
/// @}

/// @brief Disables all deprecation function on all Microsoft platforms.
#define _CRT_SECURE_NO_WARNINGS 1

#endif	// if GD_CSTRING_USE_MICROSOFT_EXT

GD_NAMESPACE_BEGIN

	/// @brief Provides functions for C string. Contains wrapped "str*" methods and methods from 'string.h', 'stdlib.h' and 'stdio.h'.
	/// @tparam CharType Specified character type.
	template<typename CharType>
	class CStringTraits final
	{
	public:
		// ... string.h & wchar.h's functions ...

		/// @brief Copies C string from source to destination.
		/// @param Dest Destination pointer.
		/// @param DestLength Length of the destination buffer.
		/// @param Source Source pointer.
		/// @returns Pointer to destination buffer.
		/// @see @c "std::strcpy" function.
		/// @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, ANSIChar>::Value, TheCharType*>::Type Strcpy(CharType* const Dest, size_t const DestLength, CharType const* const Source)
		{
			GD_CSTRING_MICROSOFT(
				auto const Result = ::strcpy_s(Dest, DestLength, Source);
				GD_DEBUG_ASSERT(Result == 0, "strcpy_s failed.");
				return Dest;
			);
			GD_CSTRING_GENERIC(
				GD_NOT_USED(DestLength);
				return std::strcpy(Dest, Source);
			);
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, TheCharType*>::Type Strcpy(CharType* const Dest, size_t const DestLength, CharType const* const Source)
		{
			GD_CSTRING_MICROSOFT(
				auto const Result = ::wcscpy_s(Dest, DestLength, Source);
				GD_DEBUG_ASSERT(Result == 0, "wcscpy_s failed.");
				return Dest;
			);
			GD_CSTRING_GENERIC(
				GD_NOT_USED(DestLength);
				return std::wcscpy(Dest, Source);
			);
		}
		/// @}

		/// @brief Copies specified amount of bytes from source a C string to destination.
		/// @param Dest Destination pointer.
		/// @param DestLength Length of the destination buffer.
		/// @param Source Source pointer.
		/// @param MaxCount Amount of bytes to copy.
		/// @returns Pointer to destination buffer.
		/// @see @c "std::strncpy" function.
		/// @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, ANSIChar>::Value, TheCharType*>::Type Strncpy(CharType* const Dest, size_t const DestLength, CharType const* const Source, size_t const MaxCount)
		{
			GD_CSTRING_MICROSOFT(
				auto const Result = ::strncpy_s(Dest, DestLength, Source, MaxCount);
				GD_DEBUG_ASSERT(Result == 0, "strncpy_s failed.");
				return Dest;
			);
			GD_CSTRING_GENERIC(
				GD_NOT_USED(DestLength);
				return std::strncpy(Dest, Source, MaxCount);
			);
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, TheCharType*>::Type Strncpy(CharType* const Dest, size_t const DestLength, CharType const* const Source, size_t const MaxCount)
		{
			GD_CSTRING_MICROSOFT(
				auto const Result = ::wcsncpy_s(Dest, DestLength, Source, MaxCount);
				GD_DEBUG_ASSERT(Result == 0, "wcsncpy_s failed.");
				return Dest;
			);
			GD_CSTRING_GENERIC(
				GD_NOT_USED(DestLength);
				return std::wcsncpy(Dest, Source, MaxCount);
			);
		}
		/// @}

		/// @brief Copies specified source C string to the end of destination one.
		/// @param Dest Destination pointer.
		/// @param DestLength Length of the destination buffer.
		/// @param Source Source pointer.
		/// @returns Pointer to destination buffer.
		/// @see @c "std::strcat" function.
		/// @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, ANSIChar>::Value, TheCharType*>::Type Strcat(CharType* const Dest, size_t const DestLength, CharType const* const Source)
		{
			GD_CSTRING_MICROSOFT(
				auto const Result = ::strcat_s(Dest, DestLength, Source);
				GD_DEBUG_ASSERT(Result == 0, "strcat_s failed.");
				return Dest;
			);
			GD_CSTRING_GENERIC(
				GD_NOT_USED(DestLength);
				return std::strcat(Dest, Source);
			);
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, TheCharType*>::Type Strcat(CharType* const Dest, size_t const DestLength, CharType const* const Source)
		{
			GD_CSTRING_MICROSOFT(
				auto const Result = ::wcscat_s(Dest, DestLength, Source);
				GD_DEBUG_ASSERT(Result == 0, "wcscat_s failed.");
				return Dest;
			);
			GD_CSTRING_GENERIC(
				GD_NOT_USED(DestLength);
				return std::wcscat(Dest, Source);
			);
		}
		/// @}

		/// @brief Copies specified amount of bytes from source C string to the end of destination one.
		/// @param Dest Destination pointer.
		/// @param DestLength Length of the destination buffer.
		/// @param Source Source pointer.
		/// @param MaxCount Amount of bytes to copy.
		/// @returns Pointer to destination buffer.
		/// @see @c "std::strncat" function.
		/// @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, ANSIChar>::Value, TheCharType*>::Type Strncat(CharType* const Dest, size_t const DestLength, CharType const* const Source, size_t const MaxCount)
		{
			GD_CSTRING_MICROSOFT(
				auto const Result = ::strncat_s(Dest, DestLength, Source);
				GD_DEBUG_ASSERT(Result == 0, "strncat_s failed.");
				return Dest;
			);
			GD_CSTRING_GENERIC(
				GD_NOT_USED(DestLength);
				return std::strncat(Dest, Source, MaxCount);
			);
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, TheCharType*>::Type Strncat(CharType* const Dest, size_t const DestLength, CharType const* const Source, size_t const MaxCount)
		{
			GD_CSTRING_MICROSOFT(
				auto const Result = ::wcsncat_s(Dest, DestLength, Source, MaxCount);
				GD_DEBUG_ASSERT(Result == 0, "wcsncat_s failed.");
				return Dest;
			);
			GD_CSTRING_GENERIC(
				GD_NOT_USED(DestLength);
				return std::wcsncat(Dest, Source, MaxCount);
			);
		}
		/// @}

		/// @brief Compares two C strings.
		/// @param First First C string to compare.
		/// @param Second Second C string to compare.
		/// @returns Zero, if strings are equal, positive if second is greater, negative - if first.
		/// @see @c "std::strcmp" function.
		/// @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, ANSIChar>::Value, int>::Type Strcmp(CharType const* const First, CharType const* const Second)
		{
			return std::strcmp(First, Second);
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, int>::Type Strcmp(CharType const* const First, CharType const* const Second)
		{
			return std::wcscmp(First, Second);
		}
		/// @}

		/// @brief Compares specified amount of bytes of two C strings.
		/// @param First First C string to compare.
		/// @param Second Second C string to compare.
		/// @param MaxCount Amount of bytes to compare.
		/// @returns Zero, if strings are equal, positive if second is greater, negative - if first.
		/// @see @c "std::strncmp" function.
		/// @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, ANSIChar>::Value, int>::Type Strncmp(CharType const* const First, CharType const* const Second, size_t const MaxCount)
		{
			return std::strncmp(First, Second, MaxCount);
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, int>::Type Strncmp(CharType const* const First, CharType const* const Second, size_t const MaxCount)
		{
			return std::wcsncmp(First, Second, MaxCount);
		}
		/// @}

		/// @brief Returns length of a C string.
		/// @param CStr C string.
		/// @returns Length of a C string.
		/// @see @c "std::strlen" function.
		/// @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, ANSIChar>::Value, size_t>::Type Strlen(CharType const* const CStr)
		{
			return std::strlen(CStr);
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, size_t>::Type Strlen(CharType const* const CStr)
		{
			return std::wcslen(CStr);
		}
		/// @}

		/// @brief Returns pointer to first occurrence of a specified character in the C string.
		/// @param CStr C string.
		/// @param Chr C character to search for.
		/// @returns Pointer to first occurrence of a specified character in the C string.
		/// @see @c "std::strchr" function.
		/// @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, ANSIChar>::Value, CharType const*>::Type Strchr(CharType const* const CStr, CharType const Chr)
		{
			return std::strchr(CStr, static_cast<int>(Chr));
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, CharType const*>::Type Strchr(CharType const* const CStr, CharType const Chr)
		{
			return std::wcschr(CStr, static_cast<wchar_t>(Chr));
		}
		GDINL static CharType* Strchr(CharType* const CStr, CharType const Chr)
		{
			return const_cast<CharType*>(Strchr(const_cast<CharType const*>(CStr), Chr));
		}
		/// @}

		/// @brief Returns pointer to last occurrence of a specified character in the C string.
		/// @param CStr C string.
		/// @param Chr C character to search for.
		/// @returns Pointer to last occurrence of a specified character in the C string.
		/// @see @c "std::strrchr" function.
		/// @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, ANSIChar>::Value, CharType const*>::Type Strrchr(CharType const* const CStr, CharType const Chr)
		{
			return std::strrchr(CStr, static_cast<int>(Chr));
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, size_t>::Type Strrchr(CharType const* const CStr, CharType const Chr)
		{
			return std::wcsrchr(CStr, static_cast<wchar_t>(Chr));
		}
		GDINL static CharType* Strrchr(CharType* const CStr, CharType const Chr)
		{
			return const_cast<CharType*>(Strrchr(const_cast<CharType const*>(CStr), Chr));
		}
		/// @}

		/// @brief Returns pointer to first occurrence of a specified C sub-string in the C string.
		/// @param CStr C string.
		/// @param SubStr C sub-string to search for.
		/// @returns Pointer to first occurrence of a specified C sub-string in the string.
		/// @see @c "std::strstr" function.
		/// @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, ANSIChar>::Value, size_t>::Type Strstr(CharType const* const CStr, CharType const* const SubStr)
		{
			return std::strstr(CStr, SubStr);
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, size_t>::Type Strchr(CharType const* const CStr, CharType const* const SubStr)
		{
			return std::wcsstr(CStr, SubStr);
		}
		GDINL static CharType* Strstr(CharType* const CStr, CharType const* const SubStr)
		{
			return const_cast<CharType*>(Strstr(const_cast<CharType const*>(CStr), SubStr));
		}
		/// @}

		/// @brief Returns pointer to last occurrence of a specified C sub-string in the C string.
		/// @param CStr C string.
		/// @param SubStr C sub-string to search for.
		/// @returns Pointer to last occurrence of a specified C sub-string in the string.
		/// @see @c "std::strrstr" (does it exist?) function.
		/// @{
		GDINL static CharType const* Strrstr(CharType const* const CStr, CharType const* const SubStr)
		{
			/// @todo Maybe does exist a better way to do this?
			size_t const StrLen = Strlen(CStr);
			size_t const SubStrLength = Strlen(SubStr);
			if (StrLen >= SubStrLength) {
				typedef CharType const* IteratorType;
				IteratorType const StartIterator = CStr + StrLen - SubStrLength;
				IteratorType const EndIterator = CStr - 1;
				for (IteratorType const Iterator = StartIterator; Iterator != EndIterator; --Iterator) {
					CharType const* const Location = Strstr(Iterator, SubStr);
					if (Location != nullptr) {
						return Location;
					}
				}
			}

			return nullptr;
		}
		GDINL static CharType* Strrstr(CharType* const CStr, CharType const* const SubStr)
		{
			return const_cast<CharType*>(Strrstr(const_cast<CharType const*>(CStr), SubStr));
		}
		/// @}

		// ... stdlib.h's functions ...

		/// @brief Converts a amount of character of a C string into ANSI C string.
		/// @param Dest Destination pointer.
		/// @param DestLength Length of the destination buffer.
		/// @param Source Source pointer.
		/// @param MaxCount Amount of bytes to convert.
		/// @returns Number of elements been converted.
		/// @see @c "std::wcstombs/std::mbstowcs" function.
		/// @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, ANSIChar>::Value, size_t>::Type Strtombs(ANSIChar* const Dest, size_t const DestLength, CharType const* const Source, size_t const MaxCount)
		{
			Strncpy(Dest, DestLength, Source, MaxCount);
			return MaxCount;
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, size_t>::Type Strtombs(ANSIChar* const Dest, size_t const DestLength, CharType const* const Source, size_t const MaxCount)
		{
			GD_CSTRING_MICROSOFT(
				size_t Output = 0;
				auto const Result = ::wcstombs_s(&Output, Dest, DestLength, Source, MaxCount);
				GD_DEBUG_ASSERT(Result == 0, "wcstombs_s failed.");
				return Output;
			);
			GD_CSTRING_GENERIC(
				GD_NOT_USED(DestLength);
				return std::wcstombs(Dest, Source, MaxCount);
			);
		}
		/// @}

		/// @brief Converts a amount of character of a C string into ANSI C string.
		/// @param Dest Destination pointer.
		/// @param DestLength Length of the destination buffer.
		/// @param Source Source pointer.
		/// @param MaxCount Amount of bytes to convert.
		/// @returns Number of elements been converted.
		/// @see @c "std::wcstombs/std::mbstowcs" function.
		/// @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, ANSIChar>::Value, size_t>::Type Strtowcs(ANSIChar* const Dest, size_t const DestLength, CharType const* const Source, size_t const MaxCount)
		{
			GD_CSTRING_MICROSOFT(
				size_t Output = 0;
				auto const Result = ::mbstowcs_s(&Output, Dest, DestLength, Source, MaxCount);
				GD_DEBUG_ASSERT(Result == 0, "mbstowcs_s failed.");
				return Output;
			);
			GD_CSTRING_GENERIC(
				GD_NOT_USED(DestLength);
				return std::mbstowcs(Dest, Source, MaxCount);
			);
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, size_t>::Type Strtowcs(ANSIChar* const Dest, size_t const DestLength, CharType const* const Source, size_t const MaxCount)
		{
			Strncpy(Dest, DestLength, Source, MaxCount);
			return MaxCount;
		}
		/// @}

		/// @brief Converts a C string into 64-bit unsigned integer value.
		/// @param CStr C String to convert.
		/// @param EndPtr Pointer to pointer, into which would be written pointer to the next to last parsed character. Is optional.
		/// @param Notation Notation in which value is represented.
		/// @returns 64-bit unsigned integer representation of a string.
		/// @see @c "std::strtoul{l}" function.
		/// @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, ANSIChar>::Value, UInt64>::Type Strtoui64(CharType const* const CStr, Char const** const EndPtr = nullptr, size_t const Notation = 0)
		{
#if GD_CSTRING_USE_MICROSOFT_EXT
			return ::_strtoui64(CStr, EndPtr, static_cast<int>(Notation));
#else	// if GD_CSTRING_USE_MICROSOFT_EXT
			static_assert(sizeof(UInt64) <= sizeof(unsigned long long), "ULL cannot store UInt64 type.");
			return static_cast<UInt64>(std::strtoull(CStr, EndPtr, static_cast<int>(Notation)));
#endif	// if GD_CSTRING_USE_MICROSOFT_EXT
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, UInt64>::Type Strtoui64(CharType const* const CStr, Char const** const EndPtr = nullptr, size_t const Notation = 0)
		{
#if GD_CSTRING_USE_MICROSOFT_EXT
			return ::_wcstoui64(CStr, EndPtr, static_cast<int>(Notation));
#else	// if GD_CSTRING_USE_MICROSOFT_EXT
			static_assert(sizeof(UInt64) <= sizeof(unsigned long long), "ULL cannot store UInt64 type.");
			return static_cast<UInt64>(std::wcstoull(CStr, EndPtr, static_cast<int>(Notation)));
#endif	// if GD_CSTRING_USE_MICROSOFT_EXT
		}
		/// @}

		/// @brief Converts a C string into 64-bit signed integer value.
		/// @param CStr C String to convert.
		/// @param EndPtr Pointer to pointer, into which would be written pointer to the next to last parsed character. Is optional.
		/// @param Notation Notation in which value is represented.
		/// @returns 64-bit signed integer representation of a string.
		/// @see @c "std::strtol{l}" function.
		/// @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, ANSIChar>::Value, Int64>::Type Strtoi64(CharType const* const CStr, Char const** const EndPtr = nullptr, size_t const Notation = 0)
		{
#if GD_CSTRING_USE_MICROSOFT_EXT
			return ::_strtoi64(CStr, EndPtr, static_cast<int>(Notation));
#else	// if GD_CSTRING_USE_MICROSOFT_EXT
			static_assert(sizeof(Int64) <= sizeof(long long), "LL cannot store Int64 type.");
			return static_cast<Int64>(std::strtoll(CStr, EndPtr, static_cast<int>(Notation)));
#endif	// if GD_CSTRING_USE_MICROSOFT_EXT
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, Int64>::Type Strtoi64(CharType const* const CStr, Char const** const EndPtr = nullptr, size_t const Notation = 0)
		{
#if GD_CSTRING_USE_MICROSOFT_EXT
			return ::_wcstoi64(CStr, EndPtr, static_cast<int>(Notation));
#else	// if GD_CSTRING_USE_MICROSOFT_EXT
			static_assert(sizeof(Int64) <= sizeof(long long), "LL cannot store Int64 type.");
			return static_cast<Int64>(std::wcstoll(CStr, EndPtr, static_cast<int>(Notation)));
#endif	// if GD_CSTRING_USE_MICROSOFT_EXT
		}
		/// @}

		/// @brief Converts a C string into 32-bit floating point value. value.
		/// @param CStr C String to convert.
		/// @param EndPtr Pointer to pointer, into which would be written pointer to the next to last parsed character. Is optional.
		/// @returns 32-bit floating point representation of a string.
		/// @see @c "std::strtof" function.
		/// @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, ANSIChar>::Value, Float32>::Type Strtof32(CharType const* const CStr, Char const** const EndPtr = nullptr)
		{
			return std::strtof(CStr, EndPtr);
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, Float32>::Type Strtof32(CharType const* const CStr, Char const** const EndPtr = nullptr)
		{
			return std::wcstof(CStr, EndPtr);
		}
		/// @}

		/// @brief Converts a C string into 64-bit floating point value. value.
		/// @param CStr C String to convert.
		/// @param EndPtr Pointer to pointer, into which would be written pointer to the next to last parsed character. Is optional.
		/// @returns 64-bit floating point representation of a string.
		/// @see @c "std::strtod" function.
		/// @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, ANSIChar>::Value, Float64>::Type Strtof64(CharType const* const CStr, Char const** const EndPtr = nullptr)
		{
			return std::strtod(CStr, EndPtr);
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, Float64>::Type Strtof64(CharType const* const CStr, Char const** const EndPtr = nullptr)
		{
			return std::wcstod(CStr, EndPtr);
		}
		/// @}

		// ... stdio.h functions ...

		/// @brief Formats specified C string into sized buffer.
		/// @param Dest Destination pointer.
		/// @param DestLength Length of the destination buffer.
		/// @param Format Format C string.
		/// @param Arguments Format arguments.
		/// @returns Number of the characters been written.
		/// @see @c "std::vsnprintf" function.
		/// @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, ANSIChar>::Value, int>::Type Vsnprintf(CharType* const Dest, size_t const DestLength, CharType const* const Format, va_list Arguments)
		{
			GD_CSTRING_MICROSOFT(return ::_vsnprintf_s(Dest, DestLength, _TRUNCATE, Format, Arguments););
			GD_CSTRING_GENERIC(
#if GD_PLATFORM_API_WINAPI
				return ::_vsnprintf(Dest, DestLength, Format, Arguments);
#else	// if GD_PLATFORM_API_WINAPI
				return std::vsnprintf(Dest, DestLength, Format, Arguments);
#endif	// if GD_PLATFORM_API_WINAPI
			);
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, int>::Type Vsnprintf(CharType* const Dest, size_t const DestLength, CharType const* const Format, va_list Arguments)
		{
			GD_CSTRING_MICROSOFT(return ::_vsnwprintf_s(Dest, DestLength, _TRUNCATE, Format, Arguments););
			GD_CSTRING_GENERIC(
#if GD_PLATFORM_API_WINAPI
				return _vswprintf(Dest, DestLength, Format, Arguments);
#else	// if GD_PLATFORM_API_WINAPI
				return std::vswprintf(Dest, DestLength, Format, Arguments);
#endif	// if GD_PLATFORM_API_WINAPI
			);
		}
		/// @}

		/// @brief Formats specified C string into sized buffer.
		/// @param Dest Destination pointer.
		/// @param DestLength Length of the destination buffer.
		/// @param Format Format C string.
		/// @param ... Format arguments.
		/// @returns Number of the characters been written.
		/// @see @c "std::snprintf" function.
		GDINL static int Snprintf(CharType* const Dest, size_t const DestLength, CharType const* const Format, ...)
		{
			va_list Arguments;
			va_start(Arguments, Format);
			int const Result = Vsnprintf(Dest, DestLength, Format, Arguments);
			va_end(Arguments);
			return Result;
		}

		/// @brief Pareses formatted data from a C string.
		/// @param Source Source C string.
		/// @param Format Format C string.
		/// @param Arguments Format arguments.
		/// @returns Number of the elements parsed.
		/// @see @c "std::vsscanf" function.
		/// @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, ANSIChar>::Value, int>::Type Vsscanf(CharType const* const Source, CharType const* const Format, va_list Arguments)
		{
			GD_CSTRING_MICROSOFT(return ::vsscanf_s(Source, Format, Arguments););
			GD_CSTRING_GENERIC(return std::vsscanf(Source, Format, Arguments););
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, int>::Type Vsscanf(CharType const* const Source, CharType const* const Format, va_list Arguments)
		{
			GD_CSTRING_MICROSOFT(return ::vswscanf_s(Source, Format, Arguments););
			GD_CSTRING_GENERIC(return std::vswscanf(Source, Format, Arguments););
		}
		/// @}

		/// @brief Pareses formatted data from a C string.
		/// @param Source Source C string.
		/// @param Format Format C string.
		/// @param ... Format arguments.
		/// @returns Number of the elements parsed.
		/// @see @c "std::sscanf" function.
		GDINL static int Sscanf(CharType const* const Source, CharType const* const Format, ...)
		{
			va_list Arguments;
			va_start(Arguments, Format);
			int const Result = Vsscanf(Source, Format, Arguments);
			va_end(Arguments);
			return Result;
		}

	};	// class CStringTraits

	typedef CStringTraits<ANSIChar> ANSIStringTraits;
	typedef CStringTraits<WideChar> WideCStringTraits;
	typedef CStringTraits<Char> CString;

GD_NAMESPACE_END

#undef GD_CSTRING_MICROSOFT
#undef GD_CSTRING_GENERIC

#undef GD_CSTRING_USE_MICROSOFT_EXT

#if (defined(_CRT_SECURE_NO_WARNINGS))
#	undef _CRT_SECURE_NO_WARNINGS
#endif	// if (defined(_CRT_SECURE_NO_WARNINGS))

#endif	// ifndef GD_CORE_MISC_CSTRING
