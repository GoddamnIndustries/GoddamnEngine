//! ==========================================================================================
//! CChar.h - Traits, helper functions and definitions for character types.
//! Copyright (C) Goddamn Industries 2015. All Rights Reserved.
//! ==========================================================================================

#pragma once
#ifndef GD_CORE_MISC_CCHAR
#define GD_CORE_MISC_CCHAR

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/TypeTraits.h>

GD_NAMESPACE_BEGIN

	// ------------------------------------------------------------------------------------------
	//! ANSI character.
	typedef char Char;
	static_assert(sizeof(Char) == 1, "Invalid 'Char' size.");

	// ------------------------------------------------------------------------------------------
	//! Unicode character.
	typedef wchar_t WideChar;
//	static_assert(sizeof(WideChar) == 2, "Invalid 'WideChar' size.");

	// ------------------------------------------------------------------------------------------
	//! Selects wide or ANSI character literal based on specified type.
	//! @param CharType Type of characters.
	//! @param StringLiteral Literal.
#define GD_LITERAL(CharType, StringLiteral) (GD::Literal<CharType>::Select(StringLiteral, L##StringLiteral))
#if !GD_DOCUMENTATION
	template <typename CharType>
	struct Literal;
	template <>
	struct Literal<Char> final
	{
		GDINL static Char Select(Char const ANSI, WideChar const Wide)
		{
			GD_NOT_USED(Wide);
			return ANSI;
		}
		GDINL static Char const* Select(Char const* const ANSI, WideChar const* const Wide)
		{
			GD_NOT_USED(Wide);
			return ANSI;
		}
	};	// struct Literal<T>
	template <>
	struct Literal<WideChar> final
	{
		GDINL static WideChar Select(Char const ANSI, WideChar const Wide)
		{
			GD_NOT_USED(ANSI);
			return Wide;
		}
		GDINL static WideChar const* Select(Char const* const ANSI, WideChar const* const Wide)
		{
			GD_NOT_USED(ANSI);
			return Wide;
		}
	};	// struct Literal<WideChar>
#endif	// if !GD_DOCUMENTATION

	// ------------------------------------------------------------------------------------------
	//! Provides helper functions for processing characters. Contains methods from "cctype" and "cwctype".
	//! @tparam CharType Specified character type.
	template<typename CharType>
	class BaseCChar final
	{
	public:

		// ------------------------------------------------------------------------------------------
		//! Returns true if this character is valid digit in specified notation. Currently supports notations range between 2 and 36.
		//! @param character Specified character.
		//! @param notation The notation in which value is represented.
		//! @returns True if this character is valid digit in specified notation.
		GDINL static bool IsDigit(CharType const character, SizeTp const notation)
		{
			// This code is useless for debug check of functions.
			switch (notation)
			{
				case 10: {
					return IsDigit(character);
				};
				case 16: {
					return IsHexDigit(character);
				};
				default: {
					GD_ASSERT((notation >= 2) && (notation <= static_cast<SizeTp>(('9' - '0') + ('Z' - 'A') + 2)), "This notation is invalid or not supported");
					if (notation > 10)
					{
						if ((character >= GD_LITERAL(CharType, '0')) && (character <= GD_LITERAL(CharType, '9')))
						{
							return true;
						}
						else if (character >= GD_LITERAL(CharType, 'A') && (character <= (GD_LITERAL(CharType, 'A') + notation - 10)))
						{
							return true;
						}
						else if (character >= GD_LITERAL(CharType, 'a') && (character <= (GD_LITERAL(CharType, 'z') + notation - 10)))
						{
							return true;
						}
						else
						{
							return false;
						}
					}
					else
					{
						return (character >= GD_LITERAL(CharType, '0')) && (character <= (GD_LITERAL(CharType, '0') + notation));
					}
				};
			}
		}

		// ------------------------------------------------------------------------------------------
		//! Returns true if this character is valid digit.
		//! @param character Specified character.
		//! @returns True if this character is valid digit.
		//! @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, Char>::Value, bool>::Type IsDigit(CharType const character)
		{
			return ::isdigit(static_cast<int>(character)) != 0;
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, bool>::Type IsDigit(CharType const character)
		{
			return ::iswdigit(static_cast< ::wint_t>(character)) != 0;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns true if this character is valid hex-a-decimal digit.
		//! @param character Specified character.
		//! @returns True if this character is valid hex-a-decimal digit.
		//! @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, Char>::Value, bool>::Type IsHexDigit(CharType const character)
		{
			return ::isxdigit(static_cast<int>(character)) != 0;
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, bool>::Type IsHexDigit(CharType const character)
		{
			return ::iswxdigit(static_cast< ::wint_t>(character)) != 0;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns true if this character can be used in identifier name.
		//! @param character Specified character.
		//! @returns True if this character can be used in identifier name.
		//! @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, Char>::Value, bool>::Type IsAlphabetic(CharType const character)
		{
			return (::isalpha(static_cast<int>(character)) != 0) || (character == GD_LITERAL(CharType, '_')) || (character == GD_LITERAL(CharType, '$'));
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, bool>::Type IsAlphabetic(CharType const character)
		{
			return (::iswalpha(static_cast<int>(character)) != 0) || (character == GD_LITERAL(CharType, '_')) || (character == GD_LITERAL(CharType, '$'));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns true if this character is valid special character.
		//! @param character Specified character.
		//! @returns True if this character is valid special character.
		//! @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, Char>::Value, bool>::Type IsPunctuation(CharType const character)
		{
			return (::ispunct(static_cast<int>(character)) != 0) && (character != GD_LITERAL(CharType, '_')) && (character != GD_LITERAL(CharType, '$'));
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, bool>::Type IsPunctuation(CharType const character)
		{
			return (::iswpunct(static_cast< ::wint_t>(character)) != 0) && (character != GD_LITERAL(CharType, '_')) && (character != GD_LITERAL(CharType, '$'));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns true if this character is valid space character.
		//! @param character Specified character.
		//! True if this character is valid special character
		//! @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, Char>::Value, bool>::Type IsSpace(CharType const character)
		{
			return ::isspace(static_cast<int>(character)) != 0;
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, bool>::Type IsSpace(CharType const character)
		{
			return ::iswspace(static_cast< ::wint_t>(character)) != 0;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Converts this character in specified notation to decimal one. Currently supports notations range between 2 and 36.
		//! @param character Specified character.
		//! Integer representation of this character data.
		GDINL static UInt8 ToDigit(Char const character)
		{
			GD_DEBUG_ASSERT(IsDigit(character), "Specified character is not a digit.");
			if ((character >= GD_LITERAL(CharType, '0')) && (character <= GD_LITERAL(CharType, '9')))
			{
				return character - GD_LITERAL(CharType, '0');
			}
			else if ((character >= GD_LITERAL(CharType, 'A')) && (character <= GD_LITERAL(CharType, 'Z')))
			{
				return (character - GD_LITERAL(CharType, 'A')) + 10;
			}
			else
			{
				return (character - GD_LITERAL(CharType, 'a')) + 10;
			}
		}

		// ------------------------------------------------------------------------------------------
		//! Returns lower-cased equivalent of the character.
		//! @param character Specified character to convert.
		//! @returns Lower-cased equivalent of the character.
		//! @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, Char>::Value, CharType>::Type ToUpper(CharType const character)
		{
			return static_cast<CharType>(::toupper(static_cast<int>(character)));
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, CharType>::Type ToUpper(CharType const character)
		{
			return static_cast<CharType>(::towupper(static_cast< ::wint_t>(character)));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns upper-cased equivalent of the character.
		//! @param character Specified character to convert.
		//! @returns Upper-cased equivalent of the character.
		//! @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, Char>::Value, CharType>::Type ToLower(CharType const character)
		{
			return static_cast<CharType>(::tolower(static_cast<int>(character)));
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, CharType>::Type ToLower(CharType const character)
		{
			return static_cast<CharType>(::towlower(static_cast< ::wint_t>(character)));
		}
		//! @}
	};	// struct BaseCChar<T>

	typedef BaseCChar<Char> CChar;
	typedef BaseCChar<WideChar> WideCChar;

GD_NAMESPACE_END

#endif	// ifndef GD_CORE_MISC_CCHAR
