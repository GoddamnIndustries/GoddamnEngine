/// ==========================================================================================
/// CharTraits.h - Traits, helper functions and definitions for cahracter types.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 23.11.2014 - Created by James Jhuighuy.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_TEXT_CHARTRAITS
#define GD_CORE_TEXT_CHARTRAITS

#include <GoddamnEngine/Include.h>

#include <cstdarg>	// va_* functions.
#include <cstring>	// str* functions.
#include <cwctype>	// tow* functions.
#include <cwchar>	// wcs* functions.
#include <cctype>	// tow* functions.

GD_NAMESPACE_BEGIN

	/// ANSI Character.
	typedef char CharAnsi;

	/// Unicode Character.
#if defined(GD_COMPILER_MSVC)
	static_assert(sizeof(wchar_t) == 2, "Error in 'wchar_t' size.");
	typedef wchar_t CharUtf16;
#else	// if defined(GD_COMPILER_MSVC)
	typedef char16_t CharUtf16;
#endif	// if defined(GD_COMPILER_MSVC)

#if (defined(_UNICODE))
#	define GD_TEXT(Text) L##Text
	typedef CharUtf16 Char;
#else	// if (defined(_UNICODE))
#	define GD_TEXT(Text) Text
	typedef CharAnsi Char;
#endif	// if (defined(_UNICODE))

#if (defined(GD_DOCUMENTATION))
	template<typename CharType>
	struct CharTraitsBase
	{
		/// Analog for standarts 'to[w]lower' function for speficied character type.
		GDINL static CharAnsi        ToLower(CharAnsi const Character);

		/// Analog for standarts 'to[w]upper' function for speficied character type.
		GDINL static CharAnsi        ToUpper(CharAnsi const Character);

		/// Analog for standarts '[str|wcs]len' function for speficied character type.
		GDINL static size_t          StrLen(CharAnsi const* const CString);

		/// Analog for standarts '[str|wcs]chr' function for speficied character type.
		GDINL static CharAnsi const* StrChr(CharAnsi const* const CString, CharAnsi const Character);

		/// Analog for standarts '[str|wcs]rchr' function for speficied character type.
		GDINL static CharAnsi const* StrRChr(CharAnsi const* const CString, CharAnsi const Character);

		/// Analog for standarts '[str|wcs]str' function for speficied character type.
		GDINL static CharAnsi const* StrStr(CharAnsi const* const First, CharAnsi const* const Second);

		/// Analog for standarts '[str|wcs]ncmp' function for speficied character type.
		GDINL static int             StrNCmp(CharAnsi const* const First, CharAnsi const* const Second, size_t const MaxLength);

		/// Analog for standarts 'vs[n|w]printf' function for speficied character type.
		GDINL static int             VSNPrintF(CharAnsi* CString, size_t const Count, CharAnsi const* const Format, va_list const Args);
	};
#else	// if (defined(GD_DOCUMENTATION))
	template<typename CharType>
	struct CharTraitsBase;

	template<>
	struct CharTraitsBase<CharAnsi>
	{
		GDINL static CharAnsi         ToLower  (CharAnsi const Character)                                                                  { return static_cast<CharAnsi>(std::tolower(Character)); }
		GDINL static CharAnsi         ToUpper  (CharAnsi const Character)                                                                  { return static_cast<CharAnsi>(std::toupper(Character)); }
		GDINL static size_t           StrLen   (CharAnsi const* const CString)                                                             { return std::strlen(CString); }
		GDINL static CharAnsi  const* StrChr   (CharAnsi const* const CString, CharAnsi const Character)                                   { return std::strchr(CString, Character); }
		GDINL static CharAnsi  const* StrRChr  (CharAnsi const* const CString, CharAnsi const Character)                                   { return std::strrchr(CString, Character); }
		GDINL static CharAnsi  const* StrStr   (CharAnsi const* const First, CharAnsi const* const Second)                                 { return std::strstr(First, Second); }
		GDINL static int              StrNCmp  (CharAnsi const* const First, CharAnsi const* const Second, size_t const MaxLength)         { return std::strncmp(First, Second, MaxLength); }
		GDINL static int              VSNPrintF(CharAnsi* CString, size_t const Count, CharAnsi const* const Format, va_list const Args)   { return /*std*/::vsnprintf(CString, Count, Format, Args); }
	};	// struct CharTraitsBase<CharAnsi>

	template<>
	struct CharTraitsBase<CharUtf16>
	{
		GDINL static CharUtf16        ToLower  (CharUtf16 const Character)                                                                 { return static_cast<CharUtf16>(std::towlower(Character)); }
		GDINL static CharUtf16        ToUpper  (CharUtf16 const Character)                                                                 { return static_cast<CharUtf16>(std::towupper(Character)); }
		GDINL static size_t           StrLen   (CharUtf16 const* const CString)                                                            { return std::wcslen(CString); }
		GDINL static CharUtf16 const* StrChr   (CharUtf16 const* const CString, CharUtf16 const Character)                                 { return std::wcschr(CString, Character); }
		GDINL static CharUtf16 const* StrRChr  (CharUtf16 const* const CString, CharUtf16 const Character)                                 { return std::wcsrchr(CString, Character); }
		GDINL static CharUtf16 const* StrStr   (CharUtf16 const* const First, CharUtf16 const* const Second)                               { return std::wcsstr(First, Second); }
		GDINL static int              StrNCmp  (CharUtf16 const* const First, CharUtf16 const* const Second, size_t const MaxLength)       { return std::wcsncmp(First, Second, MaxLength); }
		GDINL static int              VSNPrintF(CharUtf16* CString, size_t const Count, CharUtf16 const* const Format, va_list const Args) { return std::vswprintf(CString, Count, Format, Args); }
	};	// struct CharTraitsBase<CharUtf16>
#endif	// if (defined(GD_DOCUMENTATION))

	/// Provides helper functions for processing ANSI characters.
	template<typename CharType>
	struct CharTraits final : public CharTraitsBase<CharType>
	{
		/// Returns true if this character is valid digit in specified notation.
		/// Currently supports notations range between 2 and 36.
		/// Full list of supported characters: @c 0123456789
		inline static bool IsDigit(CharType const Character, size_t const Notation = 10);

		/// Returns true if this character can be used in identifier name.
		/// Full list of supported characters: @c _$QqWwEeRrTtYyUuIiOoPpAaSsDdFfGgHhJjKkLlZzXxCcVvBbNnMm
		inline static bool IsAlphabetic(CharType const Character);

		/// Returns true if this character is valid special character
		/// Full list of supported characters: @c ~`!@#$%^&*()-_+={}[]:;"'|\<>,.?/
		inline static bool IsSpecialCharacter(CharType const Character);

		/// Returns true if this character is valid special character
		/// Full list of supported characters: space (@c ' '), tabulation (@c '\t') and end-of-line characters (@c '\n', '\r')
		inline static bool IsSpace(CharType const Character);

		/// Converts this character in specified notation to decimal one
		/// Currently supports notations range between 2 and 36.
		inline static UInt8 ToDigit(Char const Character);
	};	// struct CharAnsiTraits

	typedef CharTraits<CharAnsi> CharAnsiTraits;
	typedef CharTraits<CharUtf16> CharUtf16Traits;

GD_NAMESPACE_END

#include <GoddamnEngine/Core/Text/CharTraits.inl>

#endif	// ifndef GD_CORE_TEXT_CHARTRAITS
