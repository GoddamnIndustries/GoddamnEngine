/// ==========================================================================================
/// CharTraits.h - Traits, helper functions and definitions for character types.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_MISC_CHARTRAITS
#define GD_CORE_MISC_CHARTRAITS

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/TypeTraits.h>

#include <cstdarg>	// va_* functions.
#include <cwctype>	// tow* functions.
#include <cctype>	// to* functions.

GD_NAMESPACE_BEGIN

	/// @brief ANSI Character.
	typedef char ANSIChar;

	/// @brief Unicode Character.
	typedef wchar_t WideChar;

#if (defined(_UNICODE))


#define GD_TEXT(Text) L##Text

	/// @brief Compiler-selected Wide-character.
	typedef WideChar Char;

#else	// if (defined(_UNICODE))

#define GD_TEXT(Text) Text

	/// @brief Compiler-selected ANSI-character.
	typedef ANSIChar Char;

#endif	// if (defined(_UNICODE))
	
	template <typename CharType> 
	struct Literal final
	{
		GDINL static ANSIChar Select(ANSIChar const ANSI, WideChar const Wide)
		{ 
			GD_NOT_USED(Wide);
			return ANSI;
		}

		GDINL static ANSIChar const* Select(ANSIChar const* const ANSI, WideChar const* const Wide)
		{
			GD_NOT_USED(Wide);
			return ANSI;
		}
	};	// struct Literal<T>
	template <> 
	struct Literal<WideChar> final
	{
		GDINL static WideChar Select(ANSIChar const ANSI, WideChar const Wide)
		{
			GD_NOT_USED(ANSI);
			return Wide;
		}

		GDINL static WideChar const* Select(ANSIChar const* const ANSI, WideChar const* const Wide)
		{ 
			GD_NOT_USED(ANSI);
			return Wide;
		}
	};	// struct Literal<WideChar>

	/// @brief Selects wide or ANSI character literal based on specified type.
	/// @param CharType Type of characters.
	/// @param StringLiteral Literal.
#define GD_LITERAL(CharType, StringLiteral) Literal<CharType>::Select(StringLiteral, L##StringLiteral)

	/// @brief Provides helper functions for processing characters.
	/// @tparam CharType Specified character type.
	template<typename CharType>
	class CharTraits final
	{
	public:

		/// @brief Returns true if this character is valid digit in specified notation. Currently supports notations range between 2 and 36.
		/// @param Character Specified character.
		/// @returns True if this character is valid digit in specified notation.
		GDINL static bool IsDigit(CharType const Character, size_t const Notation = 10)
		{
#if !GD_DEBUG
			// This code is useless for debug check of functions.
			if (Notation == 10) {
				return IsDigit(Character);
			} else if (Notation == 16) {
				return IsHexDigit(Character);
			} else {
#endif	// if !GD_DEBUG
				GD_ASSERT((Notation >= 2) && (Notation <= static_cast<size_t>(('9' - '0') + ('Z' - 'A') + 2)), "This notation is invalid or not supported");
				if (Notation > 10) {
					if ((Character >= GD_LITERAL(CharType, '0')) && (Character <= GD_LITERAL(CharType, '9'))) {
						return true;
					} else if (Character >= GD_LITERAL(CharType, 'A') && (Character <= (GD_LITERAL(CharType, 'A') + Notation - 10))) {
						return true;
					} else if (Character >= GD_LITERAL(CharType, 'a') && (Character <= (GD_LITERAL(CharType, 'z') + Notation - 10))) {
						return true;
					} else {
						return false;
					}
				} else {
					return (Character >= GD_LITERAL(CharType, '0')) && (Character <= (GD_LITERAL(CharType, '0') + Notation));
				}
#if !GD_DEBUG
			}
#endif	// if !GD_DEBUG
		}

		/// @brief Returns true if this character is valid digit.
		/// @param Character Specified character.
		/// @returns True if this character is valid digit.
		/// @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, ANSIChar>::Value, bool>::Type IsDigit(CharType const Character)
		{
			// This test is useless due std::isdigit does not depend on locale.
			bool const Result = static_cast<bool>(std::isdigit(static_cast<int>(Character)));
			GD_DEBUG_ASSERT(Result == IsDigit(Character, 10), "Method's result differs with generic implementation result.");
			return Result;
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, bool>::Type IsDigit(CharType const Character)
		{
			// This test is useless due std::iswdigit does not depend on locale.
			bool const Result = static_cast<bool>(std::iswdigit(static_cast<std::wint_t>(Character)));
			GD_DEBUG_ASSERT(Result == IsDigit(Character, 10), "Method's result differs with generic implementation result.");
			return Result;
		}
		/// @}

		/// @brief Returns true if this character is valid hex-a-decimal digit.
		/// @param Character Specified character.
		/// @returns True if this character is valid hex-a-decimal digit.
		/// @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, ANSIChar>::Value, bool>::Type IsHexDigit(CharType const Character)
		{
			// This test is useless due std::isxdigit does not depend on locale.
			bool const Result = static_cast<bool>(std::isxdigit(static_cast<int>(Character)));
			GD_DEBUG_ASSERT(Result == IsDigit(Character, 16), "Method's result differs with generic implementation result.");
			return Result;
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, bool>::Type IsHexDigit(CharType const Character)
		{
			// This test is useless due std::iswxdigit does not depend on locale.
			bool const Result = static_cast<bool>(std::iswxdigit(static_cast<std::wint_t>(Character)));
			GD_DEBUG_ASSERT(Result == IsDigit(Character, 16), "Method's result differs with generic implementation result.");
			return Result;
		}
		/// @}

		/// @brief Returns true if this character can be used in identifier name.
		/// @param Character Specified character.
		/// @returns True if this character can be used in identifier name.
		/// @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, void>::Value, bool>::Type IsAlphabetic(CharType const Character)
		{	// Replaced with standart functions. Use only for debug purposes.
			if ((Character == GD_LITERAL(CharType, '_')) || (Character == GD_LITERAL(CharType, '$'))) {
				return true;
			} else if (Character >= GD_LITERAL(CharType, 'A') && (Character <= GD_LITERAL(CharType, 'Z'))) {
				return true;
			} else if (Character >= GD_LITERAL(CharType, 'a') && (Character <= GD_LITERAL(CharType, 'z'))) {
				return true;
			} else {
				return false;
			}
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, ANSIChar>::Value, bool>::Type IsAlphabetic(CharType const Character)
		{
			bool const Result = (Character == GD_LITERAL(CharType, '_')) || (Character == GD_LITERAL(CharType, '$')) || static_cast<bool>(std::isalpha(static_cast<int>(Character)));
			GD_DEBUG_ASSERT(Result == IsAlphabetic<void>(Character), "Method's result differs with generic implementation result.");
			return Result;
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, bool>::Type IsAlphabetic(CharType const Character)
		{
			bool const Result = (Character == GD_LITERAL(CharType, '_')) || (Character == GD_LITERAL(CharType, '$')) || static_cast<bool>(std::iswalpha(static_cast<std::wint_t>(Character)));
			GD_DEBUG_ASSERT(Result == IsAlphabetic<void>(Character), "Method's result differs with generic implementation result.");
			return Result;
		}
		/// @}

		/// @brief Returns true if this character is valid special character.
		/// @param Character Specified character.
		/// @returns True if this character is valid special character.
		/// @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, void>::Value, bool>::Type IsPunctuation(CharType const Character)
		{	// Replaced with standart functions. Use only for debug purposes.
			switch (Character) {
				case GD_LITERAL(CharType, '~'): case GD_LITERAL(CharType, '`'): case GD_LITERAL(CharType, '!'): case GD_LITERAL(CharType, '@'): case GD_LITERAL(CharType, '#'): case GD_LITERAL(CharType, '%'):
				case GD_LITERAL(CharType, '^'): case GD_LITERAL(CharType, '&'): case GD_LITERAL(CharType, '*'): case GD_LITERAL(CharType, '('): case GD_LITERAL(CharType, ')'): case GD_LITERAL(CharType, '-'):
				case GD_LITERAL(CharType, '+'): case GD_LITERAL(CharType, '='): case GD_LITERAL(CharType, '{'): case GD_LITERAL(CharType, '}'): case GD_LITERAL(CharType, '['): case GD_LITERAL(CharType, ']'):
				case GD_LITERAL(CharType, '<'): case GD_LITERAL(CharType, '>'): case GD_LITERAL(CharType, ','): case GD_LITERAL(CharType, '.'): case GD_LITERAL(CharType, '?'): case GD_LITERAL(CharType, '/'):
				case GD_LITERAL(CharType, ':'): case GD_LITERAL(CharType, ';'): case GD_LITERAL(CharType, '"'): case GD_LITERAL(CharType, '|'): case GD_LITERAL(CharType, '\''): case GD_LITERAL(CharType, '\\'): {
					return true;
				}
			}

			return false;
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, ANSIChar>::Value, bool>::Type IsPunctuation(CharType const Character)
		{
			bool const Result = static_cast<bool>(std::ispunct(static_cast<int>(Character))) && (Character != GD_LITERAL(CharType, '_')) && (Character != GD_LITERAL(CharType, '$'));
			GD_DEBUG_ASSERT(Result == IsPunctuation<void>(Character), "Method's result differs with generic implementation result.");
			return Result;
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, bool>::Type IsPunctuation(CharType const Character)
		{
			bool const Result = static_cast<bool>(std::iswpunct(static_cast<std::wint_t>(Character))) && (Character != GD_LITERAL(CharType, '_')) && (Character != GD_LITERAL(CharType, '$'));
			GD_DEBUG_ASSERT(Result == IsPunctuation<void>(Character), "Method's result differs with generic implementation result.");
			return Result;
		}
		/// @}

		/// @brief Returns true if this character is valid space character.
		/// @param Character Specified character.
		/// @brief True if this character is valid special character
		/// @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, void>::Value, bool>::Type IsSpace(CharType const Character)
		{	// Replaced with standart functions. Use only for debug purposes.
			switch (Character) {
				case GD_LITERAL(CharType, ' '): case GD_LITERAL(CharType, '\t'): case GD_LITERAL(CharType, '\n'): case GD_LITERAL(CharType, '\r'): {
					return true;
				}
			}

			return false;
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, ANSIChar>::Value, bool>::Type IsSpace(CharType const Character)
		{
			bool const Result = static_cast<bool>(std::isspace(static_cast<int>(Character)));
			GD_DEBUG_ASSERT(Result == IsSpace<void>(Character), "Method's result differs with generic implementation result.");
			return Result;
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, bool>::Type IsSpace(CharType const Character)
		{
			bool const Result = static_cast<bool>(std::iswspace(static_cast<std::wint_t>(Character)));
			GD_DEBUG_ASSERT(Result == IsSpace<void>(Character), "Method's result differs with generic implementation result.");
			return Result;
		}
		/// @}

		/// @brief Converts this character in specified notation to decimal one. Currently supports notations range between 2 and 36.
		/// @param Character Specified character.
		/// @brief Integer representation of this character data.
		GDINL static UInt8 ToDigit(Char const Character)
		{
			return ((Character >= GD_LITERAL(CharType, '0')) && (Character <= GD_LITERAL(CharType, '9'))) ? (Character -  GD_LITERAL(CharType, '0')) 
				:  ((Character >= GD_LITERAL(CharType, 'A')) && (Character <= GD_LITERAL(CharType, 'Z'))) ? ((Character - GD_LITERAL(CharType, 'A')) + 10)
				:  ((Character >= GD_LITERAL(CharType, 'a')) && (Character <= GD_LITERAL(CharType, 'z'))) ? ((Character - GD_LITERAL(CharType, 'a')) + 10);
		}

		/// @brief Returns lower-cased equivalent of the character.
		/// @param Character Specified character to convert.
		/// @returns Lower-cased equivalent of the character.
		/// @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, ANSIChar>::Value, CharType>::Type ToUpper(CharType const Character)
		{
			return static_cast<CharType>(std::toupper(static_cast<int>(Character)));
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, CharType>::Type ToUpper(CharType const Character)
		{
			return static_cast<CharType>(std::towupper(static_cast<std::wint_t>(Character)));
		}
		/// @}

		/// @brief Returns upper-cased equivalent of the character.
		/// @param Character Specified character to convert.
		/// @returns Upper-cased equivalent of the character.
		/// @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, ANSIChar>::Value, CharType>::Type ToLower(CharType const Character)
		{
			return static_cast<CharType>(std::tolower(static_cast<int>(Character)));
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, CharType>::Type ToLower(CharType const Character)
		{
			return static_cast<CharType>(std::towlower(static_cast<std::wint_t>(Character)));
		}
		/// @}
	};	// struct CharTraits<>

	typedef CharTraits<ANSIChar> ANSICharTraits;
	typedef CharTraits<WideChar> WideCharTraits;

GD_NAMESPACE_END

#endif	// ifndef GD_CORE_TEXT_CHARTRAITS
