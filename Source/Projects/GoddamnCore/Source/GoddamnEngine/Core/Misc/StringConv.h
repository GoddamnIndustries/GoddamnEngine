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
		GDINL static UniquePtr<Char[]> EncodeUTF8(WideCStr const text, SizeTp const textLength)
		{
			GD_DEBUG_VERIFY(text != nullptr);
			auto const textEncodedLength = CalculateEncodedLength(text, textLength);
			auto const textEncoded = gd_new Char[textEncodedLength + 1];
			StringConvBase::EncodeUTF8(textEncoded, textEncodedLength, text, textLength);
			return UniquePtr<Char[]>(textEncoded);
		}

		/*!
		 * Encodes the specified wide-character text in UTF-8.
		 * @param text Text to encode.
		 */
		//! @{
		GDINL static UniquePtr<Char[]> EncodeUTF8(WideString const& text)
		{
			return EncodeUTF8(text.CStr(), text.GetLength());
		}
		GDINL static UniquePtr<Char[]> EncodeUTF8(WideCStr const text)
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
		GDINL static UniquePtr<WideChar[]> DecodeUTF8(CStr const text, SizeTp const textLength)
		{
			GD_DEBUG_VERIFY(text != nullptr);
			auto const textDecodedLength = CalculateDecodedLength(text, textLength);
			auto const textDecoded = gd_new WideChar[textDecodedLength + 1];
			StringConvBase::DecodeUTF8(textDecoded, textDecodedLength, text, textLength);
			return UniquePtr<WideChar[]>(textDecoded);
		}

		/*!
		 * Decodes the specified wide-character text in UTF-8.
		 * @param text Text to decode.
		 */
		//! @{
		GDINL static UniquePtr<WideChar[]> DecodeUTF8(String const& text)
		{
			return DecodeUTF8(text.CStr(), text.GetLength());
		}
		GDINL static UniquePtr<WideChar[]> DecodeUTF8(CStr const text)
		{
			return DecodeUTF8(text, CString::Strlen(text));
		}
		//! @}

	};	// class StringConv

GD_NAMESPACE_END

/*!
 * Encodes the specified wide-character text in UTF-8.
 * Note that value of the macro is not persistent, consider using it
 * in temporary context, like a parameter of a function call.
 * 
 * @param text Text to encode.
 */
#define GD_ENCODE_UTF8(text) (GD::StringConv::EncodeUTF8(text).Get())

/*!
 * Decodes the specified wide-character text in UTF-8.
 * Note that value of the macro is not persistent, consider using it
 * in temporary context, like a parameter of a function call.
 * 
 * @param text Text to decode.
 */
#define GD_DECODE_UTF8(text) (GD::StringConv::DecodeUTF8(text).Get())
