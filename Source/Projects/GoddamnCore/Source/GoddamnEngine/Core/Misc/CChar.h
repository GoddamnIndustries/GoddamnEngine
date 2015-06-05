//! ==========================================================================================
//! CChar.h - Traits, helper functions and definitions for character types.
//! Copyright (C) Goddamn Industries 2015. All Rights Reserved.
//! ==========================================================================================

#pragma once
#ifndef GD_CORE_MISC_CCHAR
#define GD_CORE_MISC_CCHAR

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/TypeTraits.h>

GD_NAMESPACE_BEGIN

	//! ANSI Character.
	typedef char Char;

	//! Unicode Character.
	typedef wchar_t WideChar;

	template <typename CharType>
	struct Literal final
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

	//! Selects wide or ANSI character literal based on specified type.
	//! @param CharType Type of characters.
	//! @param StringLiteral Literal.
	#define GD_LITERAL(CharType, StringLiteral) Literal<CharType>::Select(StringLiteral, L##StringLiteral)

	//! Provides helper functions for processing characters. Contains methods from "cctype" and "cwctype".
	//! @tparam CharType Specified character type.
	template<typename CharType>
	class CCharTraits final
	{
	public:

		//! Returns true if this character is valid digit in specified notation. Currently supports notations range between 2 and 36.
		//! @param Character Specified character.
		//! @param Notation Notation in which value is represented.
		//! @returns True if this character is valid digit in specified notation.
		GDINL static bool IsDigit(CharType const Character, SizeTp const Notation = 10)
		{
			// This code is useless for debug check of functions.
			switch (Notation)
			{
				case 10: {
					return IsDigit(Character);
				};
				case 16: {
					return IsHexDigit(Character);
				};
				default: {
					GD_ASSERT((Notation >= 2) && (Notation <= static_cast<SizeTp>(('9' - '0') + ('Z' - 'A') + 2)), "This notation is invalid or not supported");
					if (Notation > 10)
					{
						if ((Character >= GD_LITERAL(CharType, '0')) && (Character <= GD_LITERAL(CharType, '9')))
						{
							return true;
						}
						else if (Character >= GD_LITERAL(CharType, 'A') && (Character <= (GD_LITERAL(CharType, 'A') + Notation - 10)))
						{
							return true;
						}
						else if (Character >= GD_LITERAL(CharType, 'a') && (Character <= (GD_LITERAL(CharType, 'z') + Notation - 10)))
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
						return (Character >= GD_LITERAL(CharType, '0')) && (Character <= (GD_LITERAL(CharType, '0') + Notation));
					}
				};
			}
		}

		//! Returns true if this character is valid digit.
		//! @param Character Specified character.
		//! @returns True if this character is valid digit.
		//! @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, Char>::Value, bool>::Type IsDigit(CharType const Character)
		{
			return ::std::isdigit(static_cast<int>(Character)) != 0;
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, bool>::Type IsDigit(CharType const Character)
		{
			return ::std::iswdigit(static_cast<::std::wint_t>(Character)) != 0;
		}
		//! @}

		//! Returns true if this character is valid hex-a-decimal digit.
		//! @param Character Specified character.
		//! @returns True if this character is valid hex-a-decimal digit.
		//! @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, Char>::Value, bool>::Type IsHexDigit(CharType const Character)
		{
			return ::std::isxdigit(static_cast<int>(Character)) != 0;
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, bool>::Type IsHexDigit(CharType const Character)
		{
			return ::std::iswxdigit(static_cast<::std::wint_t>(Character)) != 0;
		}
		//! @}

		//! Returns true if this character can be used in identifier name.
		//! @param Character Specified character.
		//! @returns True if this character can be used in identifier name.
		//! @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, Char>::Value, bool>::Type IsAlphabetic(CharType const Character)
		{
			return (::std::isalpha(static_cast<int>(Character)) != 0)(Character == GD_LITERAL(CharType, '_')) || (Character == GD_LITERAL(CharType, '$'));
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, bool>::Type IsAlphabetic(CharType const Character)
		{
			return (::std::iswalpha(static_cast<int>(Character)) != 0)(Character == GD_LITERAL(CharType, '_')) || (Character == GD_LITERAL(CharType, '$'));
		}
		//! @}

		//! Returns true if this character is valid special character.
		//! @param Character Specified character.
		//! @returns True if this character is valid special character.
		//! @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, Char>::Value, bool>::Type IsPunctuation(CharType const Character)
		{
			return (::std::ispunct(static_cast<int>(Character)) != 0) && (Character != GD_LITERAL(CharType, '_')) && (Character != GD_LITERAL(CharType, '$'));
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, bool>::Type IsPunctuation(CharType const Character)
		{
			return (::std::iswpunct(static_cast<::std::wint_t>(Character)) != 0) && (Character != GD_LITERAL(CharType, '_')) && (Character != GD_LITERAL(CharType, '$'));
		}
		//! @}

		//! Returns true if this character is valid space character.
		//! @param Character Specified character.
		//! True if this character is valid special character
		//! @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, Char>::Value, bool>::Type IsSpace(CharType const Character)
		{
			return ::std::isspace(static_cast<int>(Character)) != 0;
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, bool>::Type IsSpace(CharType const Character)
		{
			return ::std::iswspace(static_cast<::std::wint_t>(Character)) != 0;
		}
		//! @}

		//! Converts this character in specified notation to decimal one. Currently supports notations range between 2 and 36.
		//! @param Character Specified character.
		//! Integer representation of this character data.
		GDINL static UInt8 ToDigit(Char const Character)
		{
			GD_DEBUG_ASSERT(IsDigit(Character), "Specified character is not a digit.");
			if ((Character >= GD_LITERAL(CharType, '0')) && (Character <= GD_LITERAL(CharType, '9')))
			{
				return Character - GD_LITERAL(CharType, '0');
			}
			else if ((Character >= GD_LITERAL(CharType, 'A')) && (Character <= GD_LITERAL(CharType, 'Z')))
			{
				return (Character - GD_LITERAL(CharType, 'A')) + 10;
			}
			else
			{
				return (Character - GD_LITERAL(CharType, 'a')) + 10;
			}
		}

		//! Returns lower-cased equivalent of the character.
		//! @param Character Specified character to convert.
		//! @returns Lower-cased equivalent of the character.
		//! @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, Char>::Value, CharType>::Type ToUpper(CharType const Character)
		{
			return static_cast<CharType>(::std::toupper(static_cast<int>(Character)));
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, CharType>::Type ToUpper(CharType const Character)
		{
			return static_cast<CharType>(::std::towupper(static_cast<::std::wint_t>(Character)));
		}
		//! @}

		//! Returns upper-cased equivalent of the character.
		//! @param Character Specified character to convert.
		//! @returns Upper-cased equivalent of the character.
		//! @{
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, Char>::Value, CharType>::Type ToLower(CharType const Character)
		{
			return static_cast<CharType>(::std::tolower(static_cast<int>(Character)));
		}
		template<typename TheCharType = CharType>
		GDINL static typename EnableIf<TypeTraits::IsSame<TheCharType, WideChar>::Value, CharType>::Type ToLower(CharType const Character)
		{
			return static_cast<CharType>(::std::towlower(static_cast<::std::wint_t>(Character)));
		}
		//! @}
	};	// struct CCharTraits<T>

	typedef CCharTraits<Char> CChar;
	typedef CCharTraits<WideChar> WideCChar;

GD_NAMESPACE_END

#endif	// ifndef GD_CORE_MISC_CCHAR
