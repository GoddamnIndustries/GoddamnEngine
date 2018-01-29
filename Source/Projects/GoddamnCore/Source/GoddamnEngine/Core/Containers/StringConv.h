// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Containers/StringConv.h
 * Encoding conversion.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Templates/UniquePtr.h>

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

GD_NAMESPACE_BEGIN

	// **------------------------------------------------------------------------------------------**
	//! UTF-8 wide-character text encoder.
	// **------------------------------------------------------------------------------------------**
	class StringConv : public TNonCreatable
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
			auto const textEncodedLength = CString::WcsToUTF8(nullptr, 0, text, textLength);
			auto const textEncoded = gd_new Char[textEncodedLength + 1];
			CString::WcsToUTF8(textEncoded, textEncodedLength, text, textEncodedLength);
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
			auto const textDecodedLength = CString::UTF8ToWcs(nullptr, 0, text, textLength);
			auto const textDecoded = gd_new WideChar[textDecodedLength + 1];
			CString::UTF8ToWcs(textDecoded, textDecodedLength, text, textLength);
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
