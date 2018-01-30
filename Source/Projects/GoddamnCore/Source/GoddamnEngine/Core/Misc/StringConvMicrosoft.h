// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file
 * Encoding conversion.
 */
#if !defined(GD_STRINGCONV_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Core/Misc/StringConv.h> instead.
#endif	// if !defined(GD_INSIDE_CSTRING_H)

#include <Windows.h>

GD_NAMESPACE_BEGIN

	// **------------------------------------------------------------------------------------------**
	//! @copydoc StringConvBaseGeneric
	// **------------------------------------------------------------------------------------------**
	class StringConvBaseMicrosoft : public StringConvBaseGeneric
	{
	protected:

		// ------------------------------------------------------------------------------------------
		// Encoding.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Calculates length of wide-character text in UTF-8 in bytes.
		 * 
		 * @param text Text for calculation.
		 * @param textLength Length of the text.
		 */
		GDINL static SizeTp CalculateEncodedLength(WideCStr const text, SizeTp const textLength)
		{
			GD_DEBUG_VERIFY(text != nullptr && textLength > 0);
			auto const result = static_cast<SizeTp>(WideCharToMultiByte(CP_UTF8, 0, text, static_cast<int>(textLength), nullptr, 0, nullptr, nullptr));
			GD_VERIFY(result != 0, "'WideCharToMultiByte' function has failed.");
			return result;
		}

		/*!
		 * Encodes the specified wide-character text in UTF-8.
		 * 
		 * @param dest Destination buffer.
		 * @param destLength Length of the destination buffer.
		 * @param source Text to encode.
		 * @param sourceLength Length of the specified text.
		 */
		GDINL static void EncodeUTF8(Char* const dest, SizeTp const destLength, WideChar const* const source, SizeTp const sourceLength)
		{
			GD_DEBUG_VERIFY(dest != nullptr && destLength > 0);
			GD_DEBUG_VERIFY(source != nullptr && sourceLength > 0);
			auto const result = WideCharToMultiByte(CP_UTF8, 0, source, static_cast<int>(sourceLength), dest, static_cast<int>(destLength), nullptr, nullptr);
			GD_VERIFY(result != 0, "'WideCharToMultiByte' function has failed.");
			dest[destLength] = '\0';
		}

		// ------------------------------------------------------------------------------------------
		// Decoding.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Calculates length of UTF-8 text in wide characters.
		 * 
		 * @param text Text for calculation.
		 * @param textLength Length of the text.
		 */
		GDINL static SizeTp CalculateDecodedLength(CStr const text, SizeTp const textLength)
		{
			GD_DEBUG_VERIFY(text != nullptr && textLength > 0);
			auto const result = static_cast<SizeTp>(MultiByteToWideChar(CP_UTF8, 0, text, static_cast<int>(textLength), nullptr, 0));
			GD_VERIFY(result != 0, "'MultiByteToWideChar' function has failed.");
			return result;
		}

		/*!
		 * Decodes the specified wide-character text in UTF-8.
		 * 
		 * @param dest Destination buffer.
		 * @param destLength Length of the destination buffer.
		 * @param source Text to decode.
		 * @param sourceLength Length of the specified text.
		 */
		GDINL static void DecodeUTF8(WideChar* const dest, SizeTp const destLength, Char const* const source, SizeTp const sourceLength)
		{
			GD_DEBUG_VERIFY(dest != nullptr && destLength > 0);
			GD_DEBUG_VERIFY(source != nullptr && sourceLength > 0);
			auto const result = MultiByteToWideChar(CP_UTF8, 0, source, static_cast<int>(sourceLength), dest, static_cast<int>(destLength));
			GD_VERIFY(result != 0, "'MultiByteToWideChar' function has failed.");
			dest[destLength] = L'\0';
		}

	};	// class StringConvBaseMicrosoft

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! @copydoc StringConvBaseMicrosoft
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	using StringConvBase = StringConvBaseMicrosoft;

GD_NAMESPACE_END
