// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/CStdlib/CChar.h
 * Wrappers, helper functions and definitions for standard char functions.
 */
#pragma once

#include <GoddamnEngine/Include.h>

#include <cctype>
#include <cwctype>

/*!
 * Selects wide or ANSI character literal based on specified type.
 *
 * @param TChar Type of characters.
 * @param literal Literal.
 */
#define GD_TEXT(TChar, literal) (GD::Literal<TChar>::Select(literal, GD_WIDEN(literal)))

GD_NAMESPACE_BEGIN

	template<typename>
	struct Literal;
	template<>
	struct Literal<Char> final
	{
		GDINL static constexpr Char Select(Char const ansi, WideChar const wide)
		{
			GD_NOT_USED(wide);
			return ansi;
		}
		GDINL static constexpr Char const* Select(CStr const ansi, WideCStr const wide)
		{
			GD_NOT_USED(wide);
			return ansi;
		}
	};	// struct Literal<Char>
	template<>
	struct Literal<WideChar> final
	{
		GDINL static constexpr WideChar Select(Char const ansi, WideChar const wide)
		{
			GD_NOT_USED(ansi);
			return wide;
		}
		GDINL static constexpr WideChar const* Select(CStr const ansi, WideCStr const wide)
		{
			GD_NOT_USED(ansi);
			return wide;
		}
	};	// struct Literal<WideChar>

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Provides helper functions for processing characters. Contains methods from "cctype" and "cwctype".
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class CChar final : public TNonCreatable
	{
	public:

		// ------------------------------------------------------------------------------------------
		// Test functions.
		// ------------------------------------------------------------------------------------------

		/*!
		 * @see @c "::isdigit" function.
		 */
		//! @{
		GDINL static bool IsDigit(Char const character)
		{
			return ::isdigit(static_cast<int>(character)) != 0;
		}
		GDINL static bool IsDigit(WideChar const character)
		{
			return ::iswdigit(static_cast<::wint_t>(character)) != 0;
		}
		//! @}

		/*!
		 * @see @c "::isxdigit" function.
		 */
		//! @{
		GDINL static bool IsHexDigit(Char const character)
		{
			return ::isxdigit(static_cast<int>(character)) != 0;
		}
		GDINL static bool IsHexDigit(WideChar const character)
		{
			return ::iswxdigit(static_cast<::wint_t>(character)) != 0;
		}
		//! @}

		/*!
		 * @see @c "::isalpha" function.
		 */
		//! @{
		GDINL static bool IsAlphabetic(Char const character)
		{
			return ::isalpha(static_cast<int>(character)) != 0 || character == '_' || character == '$';
		}
		GDINL static bool IsAlphabetic(WideChar const character)
		{
			return ::iswalpha(static_cast<int>(character)) != 0 || character == L'_' || character == L'$';
		}
		//! @}

		/*!
		 * @see @c "::ispunct" function.
		 */
		//! @{
		GDINL static bool IsPunctuation(Char const character)
		{
			return ::ispunct(static_cast<int>(character)) != 0 && character != '_' && character != '$';
		}
		GDINL static bool IsPunctuation(WideChar const character)
		{
			return ::iswpunct(static_cast<::wint_t>(character)) != 0 && character != L'_' && character != L'$';
		}
		//! @}

		/*!
		 * @see @c "::isspace" function.
		 */
		//! @{
		GDINL static bool IsSpace(Char const character)
		{
			return ::isspace(static_cast<int>(character)) != 0;
		}
		GDINL static bool IsSpace(WideChar const character)
		{
			return ::iswspace(static_cast<wint_t>(character)) != 0;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// Conversion functions.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Converts this character in specified radix to decimal one.
		 * 
		 * @param character Specified character.
		 * @returns Integer representation of this character data.
		 */
		template<typename TChar>
		GDINL static UInt8 ToDigit(TChar const character)
		{
			GD_DEBUG_VERIFY(IsHexDigit(character), "Specified character is not a digit.");
			if ((character >= GD_TEXT(TChar, '0')) && (character <= GD_TEXT(TChar, '9')))
			{
				return character - GD_TEXT(TChar, '0');
			}
			if ((character >= GD_TEXT(TChar, 'A')) && (character <= GD_TEXT(TChar, 'Z')))
			{
				return (character - GD_TEXT(TChar, 'A')) + 10;
			}
			return (character - GD_TEXT(TChar, 'a')) + 10;
		}

		/*!
		 * @see @c "::toupper" function.
		 */
		//! @{
		GDINL static Char ToUpper(Char const character)
		{
			return static_cast<Char>(::toupper(static_cast<int>(character)));
		}
		GDINL static WideChar ToUpper(WideChar const character)
		{
			return static_cast<WideChar>(::towupper(static_cast<::wint_t>(character)));
		}
		//! @}

		/*!
		 * @see @c "::tolower" function.
		 */
		//! @{
		GDINL static Char ToLower(Char const character)
		{
			return static_cast<Char>(::tolower(static_cast<int>(character)));
		}
		GDINL static WideChar ToLower(WideChar const character)
		{
			return static_cast<WideChar>(::towlower(static_cast<::wint_t>(character)));
		}
		//! @}
	};	// struct CChar<T>

	/*!
	 * Declarations used to ban standard functions. 
	 */
	enum LibCharUnallowedFunctions
	{
		// ReSharper disable CppInconsistentNaming
		isdigit, iswdigit,
		isxdigit, iswxdigit,
		isalpha, iswalpha,
		ispunct, iswpunct,
		isspace, iswspace,
		toupper, towupper,
		tolower, towlower,
		// ReSharper restore CppInconsistentNaming
	};	// enum LibCharUnallowedFunctions

GD_NAMESPACE_END
