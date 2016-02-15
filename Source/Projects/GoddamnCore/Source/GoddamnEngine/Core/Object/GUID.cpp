// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Object/GUID.cpp
 * File contains global unique IDs for objects.
 */
#include <GoddamnEngine/Core/Object/GUID.h>
#include <GoddamnEngine/Core/Containers/StringBuilder.h>
#include <GoddamnEngine/Core/System/Misc/Endian.h>

#if GD_PLATFORM_API_MICROSOFT
#	include <combaseapi.h>
#endif	// if GD_PLATFORM_API_MICROSOFT

GD_NAMESPACE_BEGIN

	// Size of GUID should be 16 bytes on all platforms.
	static_assert(sizeof(GUID) == 16, "Error in size of GUID.");

	/*!
	 * Generates a new GUID.
	 * @returns A brand new GUID.
	 */
	GDAPI GUID GUID::New()
	{
		GUID result;

		// Let platform-dependent code generate GUID where possible.
#if GD_PLATFORM_API_MICROSOFT

		::GUID resultCom;
		CoCreateGuid(&resultCom);
		result = GUID(resultCom.Data1, resultCom.Data2, resultCom.Data3, resultCom.Data4);

#else	// if GD_PLATFORM_API_MICROSOFT

		GD_NOT_IMPLEMENTED();

#endif	// if GD_PLATFORM_API_MICROSOFT

		return result;
	}

	/*!
	 * Tries to parse string to the GUID representation in the exact format.
	 *
	 * @param string The string GUID representation.
	 * @param format The exact format that this string should match.
	 * @param result Reference to output.
	 *
	 * @returns True if this string matches GUID.
	 */
	GDAPI bool GUID::TryParseExact(String const& string, Format const format, GUID& result)
	{
		// Stripping all additional characters from the input string..
		StringBuilder stringNormalized;
		stringNormalized.Reserve(33);
		switch (format)
		{
			// "00000000000000000000000000000000".
			case Format::HexDigits: 
				if (string.GetLength() != 32)
				{
					return false;
				}
				stringNormalized.Append(string);
				break;

			// "00000000-0000-0000-0000-000000000000".
			case Format::HexDigitsWithHyphens: 
				if (string.GetLength() != 32
					|| string[ 8] != '-'
					|| string[13] != '-'
					|| string[18] != '-'
					|| string[23] != '-'
				)
				{
					return false;
				}
				stringNormalized.Append(string.SubstringLen(0, 8));
				stringNormalized.Append(string.SubstringLen(9, 4));
				stringNormalized.Append(string.SubstringLen(14, 4));
				stringNormalized.Append(string.SubstringLen(19, 4));
				stringNormalized.Append(string.SubstringLen(24, 12));
				break;

			// "{00000000-0000-0000-0000-000000000000}", "(00000000-0000-0000-0000-000000000000)".
			case Format::HexDigitsWithHyphensInBraces:
			case Format::HexDigitsWithHyphensInParentheses:
				if (string.GetLength() != 38 
					|| string[ 0] != (format == Format::HexDigitsWithHyphensInBraces ? '{' : '(')
					|| string[ 9] != '-'
					|| string[14] != '-' 
					|| string[19] != '-' 
					|| string[24] != '-' 
					|| string[37] != (format == Format::HexDigitsWithHyphensInBraces ? '}' : ')')
				) 
				{
					return false; 
				} 
				stringNormalized.Append(string.SubstringLen(1, 8));
				stringNormalized.Append(string.SubstringLen(10, 4));
				stringNormalized.Append(string.SubstringLen(15, 4));
				stringNormalized.Append(string.SubstringLen(20, 4));
				stringNormalized.Append(string.SubstringLen(25, 12));
				break; 

			// "{0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}}".
			case Format::HexValuesInBraces:
				if (string.GetLength() != 68
					|| string[ 0] != '{'
					|| string[ 1] != '0'
					|| string[ 2] != 'x'
					|| string[11] != ','
					|| string[12] != '0'
					|| string[13] != 'x'
					|| string[18] != ','
					|| string[19] != '0'
					|| string[20] != 'x'
					|| string[25] != ','
					|| string[26] != '{'
					|| string[27] != '0'
					|| string[28] != 'x'
					|| string[31] != ','
					|| string[32] != '0'
					|| string[33] != 'x'
					|| string[36] != ','
					|| string[37] != '0'
					|| string[38] != 'x'
					|| string[41] != ','
					|| string[42] != '0'
					|| string[43] != 'x'
					|| string[46] != ','
					|| string[47] != '0'
					|| string[48] != 'x'
					|| string[51] != ','
					|| string[52] != '0'
					|| string[53] != 'x'
					|| string[56] != ','
					|| string[57] != '0'
					|| string[58] != 'x'
					|| string[61] != ','
					|| string[62] != '0'
					|| string[63] != 'x'
					|| string[66] != '}'
					|| string[67] != '}')
				{
					return false;
				}
				stringNormalized.Append(string.SubstringLen(3, 8));
				stringNormalized.Append(string.SubstringLen(14, 4));
				stringNormalized.Append(string.SubstringLen(21, 4));
				stringNormalized.Append(string.SubstringLen(29, 2));
				stringNormalized.Append(string.SubstringLen(34, 2));
				stringNormalized.Append(string.SubstringLen(39, 2));
				stringNormalized.Append(string.SubstringLen(44, 2));
				stringNormalized.Append(string.SubstringLen(49, 2));
				stringNormalized.Append(string.SubstringLen(54, 2));
				stringNormalized.Append(string.SubstringLen(59, 2));
				stringNormalized.Append(string.SubstringLen(64, 2));
				break;

			// "0000000000000000-0000000000000000".
			case Format::HexDigits64BitWithHyphen: 
				if (string.GetLength() != 33
					|| string[16] != '-'
					)
				{
					return false;
				}
				stringNormalized.Append(string.SubstringLen(0, 8));
				stringNormalized.Append(string.SubstringLen(17, 4));
				break;

			default: 
				GD_DEBUG_ASSERT_FALSE("Invalid format was specified.");
		}
		
		// Converting normalized sting to the bytes sequence..
		Byte resultBytes[16] = {};
		for (auto cnt = 0; cnt < 16; ++cnt)
		{
			if (!CChar::IsHexDigit(stringNormalized[cnt]) || !CChar::IsHexDigit(stringNormalized[cnt * 2]))
			{
				return false;
			}
			resultBytes[cnt] += CChar::ToDigit(stringNormalized[cnt * 2]) * 0x10;
			resultBytes[cnt] += CChar::ToDigit(stringNormalized[cnt * 2 + 1]);
		}

		result = GUID(resultBytes);
#if GD_ENDIANITY_LITTLE
		result.m_A = SwapEndian(result.m_A);
		result.m_B = SwapEndian(result.m_B);
		result.m_C = SwapEndian(result.m_C);
#endif	// if GD_ENDIANITY_LITTLE
		return true;
	}

	/*!
	 * Tries to parse string to the GUID representation.
	 *
	 * @param string The string GUID representation.
	 * @param result Reference to output.
	 *
	 * @returns True if this string matches GUID.
	 */
	GDAPI bool GUID::TryParse(String const& string, GUID& result)
	{
		switch(string.GetLength())
		{
			case 32:
				return TryParseExact(string, Format::HexDigits, result);
			case 38:
				return string.StartsWith('{') ? TryParseExact(string, Format::HexDigitsWithHyphensInBraces, result) : TryParseExact(string, Format::HexDigitsWithHyphensInParentheses, result);
			case 68:
				return TryParseExact(string, Format::HexValuesInBraces, result);
			case 33:
				return TryParseExact(string, Format::HexDigits64BitWithHyphen, result);
		}
		return false;
	}

	/*!
	 * Parses string to the GUID representation in the exact format.
	 *
	 * @param string The string GUID representation.
	 * @param format The exact format that this string should match.
	 * @param result Reference to output.
	 *
	 * @returns Parsed GUID or the invalid GUID.
	 */
	GDAPI GUID GUID::ParseExact(String const& string, Format const format)
	{
		GUID result;
		if (!TryParseExact(string, format, result))
		{
			result.Invalidate();
		}
		return result;
	}

	/*!
	 * Parses string to the GUID representation in the exact format.
	 *
	 * @param string The string GUID representation.
	 * @returns Parsed GUID or the invalid GUID.
	 */
	GDAPI GUID GUID::Parse(String const& string)
	{
		GUID result;
		if (!TryParse(string, result))
		{
			result.Invalidate();
		}
		return result;
	}

	/*!
	 * Returns string representation of this GUID in string format.
	 * @param format The format of the GUID string representation.
	 */
	GDAPI String GUID::ToString(Format const format /*= Format::HexDigits64BitWithHyphen*/) const
	{
		switch (format)
		{
			case Format::HexDigits: 
				return String::Format("%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X"
					, m_A, m_B, m_C, m_D, m_E, m_F, m_G, m_H, m_I, m_J, m_K);

			case Format::HexDigitsWithHyphens: 
				return String::Format("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X"
					, m_A, m_B, m_C, m_D, m_E, m_F, m_G, m_H, m_I, m_J, m_K);

			case Format::HexDigitsWithHyphensInBraces: 
				return String::Format("{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}"
					, m_A, m_B, m_C, m_D, m_E, m_F, m_G, m_H, m_I, m_J, m_K);

			case Format::HexDigitsWithHyphensInParentheses: 
				return String::Format("(%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X)"
					, m_A, m_B, m_C, m_D, m_E, m_F, m_G, m_H, m_I, m_J, m_K);

			case Format::HexValuesInBraces: 
				return String::Format("{0x%08X,0x%04X,0x%04X,{0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X}"
					, m_A, m_B, m_C, m_D, m_E, m_F, m_G, m_H, m_I, m_J, m_K);

			case Format::HexDigits64BitWithHyphen: 
				return String::Format("%08X%04X%04X-%02X%02X%02X%02X%02X%02X%02X%02X"
					, m_A, m_B, m_C, m_D, m_E, m_F, m_G, m_H, m_I, m_J, m_K);

			default: 
				GD_DEBUG_ASSERT_FALSE("Invalid format was specified.");
		}
	}

GD_NAMESPACE_END
