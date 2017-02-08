// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Base/CStdlib/CChar.h
 * @note This file should be never directly included, please consider using <GoddamnEngine/Include.h> instead.
 * Wrappers, helper functions and definitions for standard char functions.
 */
#pragma once
#if !defined(GD_INSIDE_INCLUDE_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Include.h> instead.
#endif	// if !defined(GD_INSIDE_INCLUDE_H)

/*!
 * Selects wide or ANSI character literal based on specified type.
 *
 * @param TChar Type of characters.
 * @param StringLiteral Literal.
 */
#define GD_LITERAL(TChar, literal) (GD::Literal<TChar>::Select(literal, L##literal))

GD_NAMESPACE_BEGIN

	template<typename>
	struct Literal;
	
	template<>
	struct Literal<Char> final
	{
		GDINL static constexpr Char Select(Char const ANSI, WideChar const Wide)
		{
		//	GD_NOT_USED(Wide);
			return ANSI;
		}
		GDINL static constexpr Char const* Select(Char const* const ANSI, WideChar const* const Wide)
		{
			//GD_NOT_USED(Wide);
			return ANSI;
		}
	};	// struct Literal<Char>

	template<>
	struct Literal<WideChar> final
	{
		GDINL static constexpr WideChar Select(Char const ANSI, WideChar const Wide)
		{
			//GD_NOT_USED(ANSI);
			return Wide;
		}
		GDINL static constexpr WideChar const* Select(Char const* const ANSI, WideChar const* const Wide)
		{
			//GD_NOT_USED(ANSI);
			return Wide;
		}
	};	// struct Literal<WideChar>

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Provides helper functions for processing characters. Contains methods from "cctype" and "cwctype".
	//! @tparam TChar Specified character type.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	template<typename TChar>
	class BaseCChar final : public TNonCreatable
	{
	public:

		/*! 
		 * Returns true if this character is valid digit in specified radix.
		 *
		 * @param character Specified character.
		 * @param radix The radix in which value is represented.
		 *
		 * @returns True if this character is valid digit in specified radix.
		 */
		GDINL static bool IsDigit(TChar const character, SizeTp const radix)
		{
			// This code is useless for debug check of functions.
			switch (radix)
			{
				case 10: 
					return IsDigit(character);
				case 16: 
					return IsHexDigit(character);
				default: 
					GD_VERIFY((radix >= 2) && (radix <= static_cast<SizeTp>(('9' - '0') + ('Z' - 'A') + 2)), "This radix is invalid or not supported");
					if (radix > 10)
					{
						if ((character >= GD_LITERAL(TChar, '0')) && (character <= GD_LITERAL(TChar, '9')))
						{
							return true;
						}
						if (character >= GD_LITERAL(TChar, 'A') && (character <= (GD_LITERAL(TChar, 'A') + radix - 10)))
						{
							return true;
						}
						if (character >= GD_LITERAL(TChar, 'a') && (character <= (GD_LITERAL(TChar, 'z') + radix - 10)))
						{
							return true;
						}
						return false;
					}
					return (character >= GD_LITERAL(TChar, '0')) && (character <= (GD_LITERAL(TChar, '0') + radix));
			}
		}

		/*!
		 * @see @c "::isdigit" function.
		 */
		//! @{
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, bool>::Type IsDigit(TChar const character)
		{
			return ::isdigit(static_cast<int>(character)) != 0;
		}
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, bool>::Type IsDigit(TChar const character)
		{
			return ::iswdigit(static_cast<::wint_t>(character)) != 0;
		}
		//! @}

		/*!
		 * @see @c "::isxdigit" function.
		 */
		//! @{
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, bool>::Type IsHexDigit(TChar const character)
		{
			return ::isxdigit(static_cast<int>(character)) != 0;
		}
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, bool>::Type IsHexDigit(TChar const character)
		{
			return ::iswxdigit(static_cast<::wint_t>(character)) != 0;
		}
		//! @}

		/*!
		 * @see @c "::isalpha" function.
		 */
		//! @{
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, bool>::Type IsAlphabetic(TChar const character)
		{
			return ::isalpha(static_cast<int>(character)) != 0 || character == GD_LITERAL(TChar, '_') || character == GD_LITERAL(TChar, '$');
		}
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, bool>::Type IsAlphabetic(TChar const character)
		{
			return ::iswalpha(static_cast<int>(character)) != 0 || character == GD_LITERAL(TChar, '_') || character == GD_LITERAL(TChar, '$');
		}
		//! @}

		/*!
		 * @see @c "::ispunct" function.
		 */
		//! @{
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, bool>::Type IsPunctuation(TChar const character)
		{
			return ::ispunct(static_cast<int>(character)) != 0 && character != GD_LITERAL(TChar, '_') && character != GD_LITERAL(TChar, '$');
		}
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, bool>::Type IsPunctuation(TChar const character)
		{
			return ::iswpunct(static_cast<::wint_t>(character)) != 0 && character != GD_LITERAL(TChar, '_') && character != GD_LITERAL(TChar, '$');
		}
		//! @}

		/*!
		 * @see @c "::isspace" function.
		 */
		//! @{
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, bool>::Type IsSpace(TChar const character)
		{
			return ::isspace(static_cast<int>(character)) != 0;
		}
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, bool>::Type IsSpace(TChar const character)
		{
			return ::iswspace(static_cast<wint_t>(character)) != 0;
		}
		//! @}

		/*!
		 * Converts this character in specified radix to decimal one.
		 * 
		 * @param character Specified character.
		 * @returns Integer representation of this character data.
		 */
		GDINL static UInt8 ToDigit(TChar const character)
		{
			GD_DEBUG_VERIFY(IsHexDigit(character), "Specified character is not a digit.");
			if ((character >= GD_LITERAL(TChar, '0')) && (character <= GD_LITERAL(TChar, '9')))
			{
				return character - GD_LITERAL(TChar, '0');
			}
			if ((character >= GD_LITERAL(TChar, 'A')) && (character <= GD_LITERAL(TChar, 'Z')))
			{
				return (character - GD_LITERAL(TChar, 'A')) + 10;
			}
			return (character - GD_LITERAL(TChar, 'a')) + 10;
		}

		/*!
		 * @see @c "::toupper" function.
		 */
		//! @{
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, TChar>::Type ToUpper(TChar const character)
		{
			return static_cast<TChar>(::toupper(static_cast<int>(character)));
		}
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, TChar>::Type ToUpper(TChar const character)
		{
			return static_cast<TChar>(::towupper(static_cast<::wint_t>(character)));
		}
		//! @}

		/*!
		 * @see @c "::tolower" function.
		 */
		//! @{
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, Char>::Value, TChar>::Type ToLower(TChar const character)
		{
			return static_cast<TChar>(::tolower(static_cast<int>(character)));
		}
		template<typename TTChar = TChar>
		GDINL static typename EnableIf<TypeTraits::IsSame<TTChar, WideChar>::Value, TChar>::Type ToLower(TChar const character)
		{
			return static_cast<TChar>(::towlower(static_cast<::wint_t>(character)));
		}
		//! @}
	};	// struct BaseCChar<T>

	typedef BaseCChar<Char> CChar;
	typedef BaseCChar<WideChar> WideCChar;

	/*!
	 * Declarations used to ban standard functions. 
	 */
	enum LibCharUnallowedFunctions
	{
		isdigit, iswdigit,
		isxdigit, iswxdigit,
		isalpha, iswalpha,
		ispunct, iswpunct,
		isspace, iswspace,
		toupper, towupper,
		tolower, towlower,
	};	// enum LibCharUnallowedFunctions

GD_NAMESPACE_END
