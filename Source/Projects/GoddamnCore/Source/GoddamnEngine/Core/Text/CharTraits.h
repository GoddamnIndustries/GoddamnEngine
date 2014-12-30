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
	typedef wchar_t CharUnicode;

#if (defined(_UNICODE))
#	define GD_TEXT(Text) L##Text
	typedef CharUnicode Char;
#else	// if (defined(_UNICODE))
#	define GD_TEXT(Text) Text
	typedef CharAnsi Char;
#endif	// if (defined(_UNICODE))

#if (defined(GD_DOCUMENTATION))
	template<typename CharType>
	struct CharTraitsBase
	{
		/// Analog for standards 'to[w]lower' function for specified character type.
		GDINL static CharAnsi  ToLower(CharAnsi const Character);

		/// Analog for standards 'to[w]upper' function for specified character type.
		GDINL static CharAnsi  ToUpper(CharAnsi const Character);

		/// Analog for standards '[str|wcs]len' function for specified character type.
		GDINL static size_t    StrLen(CharAnsi const* const CString);

		/// Analog for standards '[str|wcs]chr' function for specified character type.
		GDINL static CharAnsi const* StrChr(CharAnsi const* const CString, CharAnsi const Character);

		/// Analog for standards '[str|wcs]rchr' function for specified character type.
		GDINL static CharAnsi const* StrRChr(CharAnsi const* const CString, CharAnsi const Character);

		/// Analog for standards '[str|wcs]str' function for specified character type.
		GDINL static CharAnsi const* StrStr(CharAnsi const* const First, CharAnsi const* const Second);

		/// Analog for standards '[str|wcs]ncmp' function for specified character type.
		GDINL static int    StrNCmp(CharAnsi const* const First, CharAnsi const* const Second, size_t const MaxLength);

		/// Analog for standards 'vs[n|w]printf' function for specified character type.
		GDINL static int    VSNPrintF(CharAnsi* CString, size_t const Count, CharAnsi const* const Format, va_list const Args);
	};
#else	// if (defined(GD_DOCUMENTATION))
	template<typename CharType>
	struct CharTraitsBase;

	template<>
	struct CharTraitsBase<CharAnsi>
	{
		GDINL static CharAnsi   ToLower  (CharAnsi const Character)                  { return static_cast<CharAnsi>(::tolower(Character)); }
		GDINL static CharAnsi   ToUpper  (CharAnsi const Character)                  { return static_cast<CharAnsi>(::toupper(Character)); }
		GDINL static size_t     StrLen   (CharAnsi const* const CString)                { return ::strlen(CString); }
		GDINL static CharAnsi  const* StrChr   (CharAnsi const* const CString, CharAnsi const Character)           { return ::strchr(CString, Character); }
		GDINL static CharAnsi  const* StrRChr  (CharAnsi const* const CString, CharAnsi const Character)           { return ::strrchr(CString, Character); }
		GDINL static CharAnsi  const* StrStr   (CharAnsi const* const First, CharAnsi const* const Second)         { return ::strstr(First, Second); }
		GDINL static int     StrNCmp  (CharAnsi const* const First, CharAnsi const* const Second, size_t const MaxLength)   { return ::strncmp(First, Second, MaxLength); }
		GDINL static int     VSNPrintF(CharAnsi* CString, size_t const Count, CharAnsi const* const Format, va_list Args)		   { return ::vsnprintf(CString, Count, Format, Args); }
	};	// struct CharTraitsBase<CharAnsi>

	template<>
	struct CharTraitsBase<CharUnicode>
	{
		GDINL static CharUnicode  ToLower  (CharUnicode const Character)                 { return static_cast<CharUnicode>(::towlower(Character)); }
		GDINL static CharUnicode  ToUpper  (CharUnicode const Character)                 { return static_cast<CharUnicode>(::towupper(Character)); }
		GDINL static size_t     StrLen   (CharUnicode const* const CString)               { return ::wcslen(CString); }
		GDINL static CharUnicode const* StrChr   (CharUnicode const* const CString, CharUnicode const Character)         { return ::wcschr(CString, Character); }
		GDINL static CharUnicode const* StrRChr  (CharUnicode const* const CString, CharUnicode const Character)         { return ::wcsrchr(CString, Character); }
		GDINL static CharUnicode const* StrStr   (CharUnicode const* const First, CharUnicode const* const Second)          { return ::wcsstr(First, Second); }
		GDINL static int     StrNCmp  (CharUnicode const* const First, CharUnicode const* const Second, size_t const MaxLength)    { return ::wcsncmp(First, Second, MaxLength); }
		GDINL static int     VSNPrintF(CharUnicode* CString, size_t const Count, CharUnicode const* const Format, va_list Args)	   { return ::vswprintf(CString, Count, Format, Args); }
	};	// struct CharTraitsBase<CharUnicode>
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
	typedef CharTraits<CharUnicode> CharUnicodeTraits;

GD_NAMESPACE_END

#include <GoddamnEngine/Core/Text/CharTraits.inl>

#endif	// ifndef GD_CORE_TEXT_CHARTRAITS
