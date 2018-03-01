// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
//
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file
 * Text encoding conversions.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Templates/Singleton.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Text encoding conversions.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL IPlatformTextEncoding : public Singleton<IPlatformTextEncoding>
	{
	public:

		// ------------------------------------------------------------------------------------------
		// Encoding.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Calculates length of wide-character text in UTF-8 in bytes.
		 *
		 * @param text Text for calculation.
		 * @param textLength Length of the text.
		 * @param encodedLength Length of wide-character text in UTF-8 in bytes.
		 *
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool CalculateEncodedLength(WideCStr const text, SizeTp const textLength, SizeTp& encodedLength) const GD_PURE_VIRTUAL;

		/*!
		 * Encodes the specified wide-character text in UTF-8.
		 *
		 * @param dst Destination buffer.
		 * @param dstLength Length of the destination buffer.
		 * @param src Text to encode.
		 * @param srcLength Length of the specified text.
		 *
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool EncodeUTF8(Char* const dst, SizeTp const dstLength, WideCStr const src, SizeTp const srcLength) const GD_PURE_VIRTUAL;

		// ------------------------------------------------------------------------------------------
		// Decoding.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Calculates length of UTF-8 text in wide characters.
		 *
		 * @param text Text for calculation.
		 * @param textLength Length of the text.
		 * @param decodedLength Length of UTF-8 text in wide characters.
		 *
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool CalculateDecodedLength(CStr const text, SizeTp const textLength, SizeTp& decodedLength) const GD_PURE_VIRTUAL;

		/*!
		 * Decodes the specified wide-character text in UTF-8.
		 *
		 * @param dst Destination buffer.
		 * @param dstLength Length of the destination buffer.
		 * @param src Text to decode.
		 * @param srcLength Length of the specified text.
		 *
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool DecodeUTF8(WideChar* const dst, SizeTp const dstLength, CStr const src, SizeTp const srcLength) const GD_PURE_VIRTUAL;
	};  // class IPlatformTextEncoding

	template<>
	GDAPI IPlatformTextEncoding& Singleton<IPlatformTextEncoding>::Get();

	// **------------------------------------------------------------------------------------------**
	//! UTF-8 wide-character text encoder.
	// **------------------------------------------------------------------------------------------**
	class StringConv final
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
		 * @param resultPtr Pointer to the function call result. If no parameter presented and function fails it will throw.
		 */
		GDINL static String EncodeUTF8(WideCStr const text, SizeTp const textLength, bool* const resultPtr = nullptr)
		{
			GD_DEBUG_VERIFY(text != nullptr);
			SizeTp textEncodedLength = 0;
			if (IPlatformTextEncoding::Get().CalculateEncodedLength(text, textLength, textEncodedLength))
			{
				String textEncoded(textEncodedLength);
				if (IPlatformTextEncoding::Get().EncodeUTF8(textEncoded.CStr(), textEncodedLength, text, textLength))
				{
					if (resultPtr != nullptr)
					{
						*resultPtr = true;
					}
					return textEncoded;
				}
			}
			if (resultPtr == nullptr)
			{
				GD_VERIFY_FALSE("Unhandled encoding error: encoding to UTF-8 failed.");
			}
			return "";
		}

		/*!
		 * Encodes the specified wide-character text in UTF-8.
		 *
		 * @param text Text to encode.
		 * @param resultPtr Pointer to the function call result. If no parameter presented and function fails it will throw.
		 */
		//! @{
		GDINL static String EncodeUTF8(WideString const& text, bool* const resultPtr = nullptr)
		{
			return EncodeUTF8(text.CStr(), text.GetLength(), resultPtr);
		}
		GDINL static String EncodeUTF8(WideCStr const text, bool* const resultPtr = nullptr)
		{
			return EncodeUTF8(text, CString::Strlen(text), resultPtr);
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
		 * @param resultPtr Pointer to the function call result. If no parameter presented and function fails it will throw.
		 */
		GDINL static WideString DecodeUTF8(CStr const text, SizeTp const textLength, bool* const resultPtr = nullptr)
		{
			GD_DEBUG_VERIFY(text != nullptr);
			SizeTp textDecodedLength = 0;
			if (IPlatformTextEncoding::Get().CalculateDecodedLength(text, textLength, textDecodedLength))
			{
				WideString textDecoded(textDecodedLength);
				if (IPlatformTextEncoding::Get().DecodeUTF8(textDecoded.CStr(), textDecodedLength, text, textLength))
				{
					if (resultPtr != nullptr)
					{
						*resultPtr = true;
					}
					return textDecoded;
				}
			}
			if (resultPtr == nullptr)
			{
				GD_VERIFY_FALSE("Unhandled encoding error: decoding from UTF-8 failed.");
			}
			return L"";
		}

		/*!
		 * Decodes the specified wide-character text in UTF-8.
		 *
		 * @param text Text to decode.
		 * @param resultPtr Pointer to the function call result. If no parameter presented and function fails it will throw.
		 */
		//! @{
		GDINL static WideString DecodeUTF8(String const& text, bool* const resultPtr = nullptr)
		{
			return DecodeUTF8(text.CStr(), text.GetLength(), resultPtr);
		}
		GDINL static WideString DecodeUTF8(CStr const text, bool* const resultPtr = nullptr)
		{
			return DecodeUTF8(text, CString::Strlen(text), resultPtr);
		}
		//! @}
	};	// class StringConv

GD_NAMESPACE_END
