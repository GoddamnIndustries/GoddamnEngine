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
#pragma once
#define GD_STRINGCONV_H

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Templates/UniquePtr.h>

GD_NAMESPACE_BEGIN

	// **------------------------------------------------------------------------------------------**
	//! UTF-8 wide-character text base encoder.
	// **------------------------------------------------------------------------------------------**
	class StringConvBaseGeneric : public TNonCreatable
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
			GD_NOT_USED_L(text, textLength);
			GD_NOT_IMPLEMENTED();
		}

		/*!
		 * Encodes the specified wide-character text in UTF-8.
		 * 
		 * @param dest Destination buffer.
		 * @param destLength Length of the destination buffer.
		 * @param source Text to encode.
		 * @param sourceLength Length of the specified text.
		 */
		GDINL static void EncodeUTF8(Char* const dest, SizeTp const destLength, WideChar const* const source, SizeTp const sourceLength)	// NOLINT
		{
			GD_NOT_USED_L(dest, destLength, source, sourceLength);
			GD_NOT_IMPLEMENTED();
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
			GD_NOT_USED_L(text, textLength);
			GD_NOT_IMPLEMENTED();
		}

		/*!
		 * Decodes the specified wide-character text in UTF-8.
		 * 
		 * @param dest Destination buffer.
		 * @param destLength Length of the destination buffer.
		 * @param source Text to decode.
		 * @param sourceLength Length of the specified text.
		 */
		GDINL static void DecodeUTF8(WideChar* const dest, SizeTp const destLength, Char const* const source, SizeTp const sourceLength)	// NOLINT
		{
			GD_NOT_USED_L(dest, destLength, source, sourceLength);
			GD_NOT_IMPLEMENTED();
		}

	};	// class StringConvBaseGeneric

GD_NAMESPACE_END

#include GD_PLATFORM_API_INCLUDE(GoddamnEngine/Core/Misc, StringConv)
#undef GD_STRINGCONV_H

GD_NAMESPACE_BEGIN

	// **------------------------------------------------------------------------------------------**
	//! UTF-8 wide-character text encoder.
	// **------------------------------------------------------------------------------------------**
	class StringConv : public StringConvBase
	{
	public:
		
		// ------------------------------------------------------------------------------------------
		// Encoding.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Encodes the specified wide-character text in UTF-8.
		 * 
		 * @param text Text to encode.
		 * @param textLength Length of the specified text.
		 */
		GDINL static String EncodeUTF8(WideCStr const text, SizeTp const textLength)
		{
			GD_DEBUG_VERIFY(text != nullptr);
			auto const textEncodedLength = CalculateEncodedLength(text, textLength);
			String textEncoded(textEncodedLength);
			StringConvBase::EncodeUTF8(textEncoded.CStr(), textEncodedLength, text, textLength);
			return textEncoded;
		}

		/*!
		 * Encodes the specified wide-character text in UTF-8.
		 * @param text Text to encode.
		 */
		//! @{
		GDINL static String EncodeUTF8(WideString const& text)
		{
			return EncodeUTF8(text.CStr(), text.GetLength());
		}
		GDINL static String EncodeUTF8(WideCStr const text)
		{
			return EncodeUTF8(text, CString::Strlen(text));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// Decoding.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Decodes the specified wide-character text in UTF-8.
		 * 
		 * @param text Text to decode.
		 * @param textLength Length of the specified text.
		 */
		GDINL static WideString DecodeUTF8(CStr const text, SizeTp const textLength)
		{
			GD_DEBUG_VERIFY(text != nullptr);
			auto const textDecodedLength = CalculateDecodedLength(text, textLength);
			WideString textDecoded(textDecodedLength);
			StringConvBase::DecodeUTF8(textDecoded.CStr(), textDecodedLength, text, textLength);
			return textDecoded;
		}

		/*!
		 * Decodes the specified wide-character text in UTF-8.
		 * @param text Text to decode.
		 */
		//! @{
		GDINL static WideString DecodeUTF8(String const& text)
		{
			return DecodeUTF8(text.CStr(), text.GetLength());
		}
		GDINL static WideString DecodeUTF8(CStr const text)
		{
			return DecodeUTF8(text, CString::Strlen(text));
		}
		//! @}

	};	// class StringConv

GD_NAMESPACE_END