/// ==========================================================================================
/// CharTraits.h - helper functions implemenetations for character types.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 23.11.2014 - Created by James Jhuighuy.
/// ==========================================================================================

#if (!defined(GD_CORE_TEXT_CHARTRAITS))
#	pragma error("Attempting to include 'CharTraits.inl' file. Please, use 'CharTraits.h' instead")
#endif	// if (!defined(GD_CORE_TEXT_CHARTRAITS))

GD_NAMESPACE_BEGIN

	/// ==========================================================================================
	/// CharTraits structure.
	/// ==========================================================================================
	
	/// ------------------------------------------------------------------------------------------
	/// 'Is?' functions.
	/// ------------------------------------------------------------------------------------------

	template<typename CharType>
	inline bool CharTraits<CharType>::IsDigit(CharType const Character, size_t const Notation /* = 10 */)
	{
		GD_ASSERT((Notation >= 2) && (Notation <= static_cast<size_t>(('9' - '0') + ('Z' - 'A') + 2)), "This notation is invalid or not supported");
		if (Notation > 10) {
			if ((Character >= CharType('0')) && (Character <= CharType('9'))) {
				return true;
			} else if (Character >= CharType('A') && (Character <= (CharType('A') + Notation - 10))) {
				return true;
			} else if (Character >= CharType('a') && (Character <= (CharType('z') + Notation - 10))) {
				return true;
			} else {
				return false;
			}
		} else {
			return (Character >= CharType('0')) && (Character <= (CharType('0') + Notation));
		}
	}

	template<typename CharType>
	inline bool CharTraits<CharType>::IsAlphabetic(CharType const Character)
	{
		if ((Character == CharType('_')) || (Character == CharType('$'))) {
			return true;
		} else if (Character >= CharType('A') && (Character <= CharType('Z'))) {
			return true;
		} else if (Character >= CharType('a') && (Character <= CharType('z'))) {
			return true;
		} else {
			return false;
		}
	}

	template<typename CharType>
	inline bool CharTraits<CharType>::IsSpecialCharacter(CharType const Character)
	{
		switch (Character) {
			case CharType('~'): case CharType('`'): case CharType('!'): case CharType('@'): case CharType('#'): case CharType('%'): 
			case CharType('^'): case CharType('&'): case CharType('*'): case CharType('('): case CharType(')'): case CharType('-'): 
			case CharType('+'): case CharType('='): case CharType('{'): case CharType('}'): case CharType('['): case CharType(']'): 
			case CharType('<'): case CharType('>'): case CharType(','): case CharType('.'): case CharType('?'): case CharType('/'):
			case CharType(':'): case CharType(';'): case CharType('"'): case CharType('|'): case CharType('\''): case CharType('\\'): {
				return true;
			}
		}

		return false;
	}
	
	template<typename CharType>
	inline bool CharTraits<CharType>::IsSpace(CharType const Character)
	{
		switch (Character) {
			case CharType(' '): case CharType('\t'): case CharType('\n'): case CharType('\r'): {
				return true;
			}
		}

		return false;
	}

	/// ------------------------------------------------------------------------------------------
	/// 'To' functions.
	/// ------------------------------------------------------------------------------------------

	template<typename CharType>
	inline UInt8 CharTraits<CharType>::ToDigit(Char const Character)
	{
		return ((Character >= CharType('0')) && (Character <= CharType('9')))
			  ? (Character  - CharType('0')) : ((Character  - CharType('A')) + 10);
	}

GD_NAMESPACE_END
