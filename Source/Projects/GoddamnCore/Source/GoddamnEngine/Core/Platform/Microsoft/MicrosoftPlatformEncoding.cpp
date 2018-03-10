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
#include <GoddamnEngine/Core/Platform/PlatformEncoding.h>
#if GD_PLATFORM_API_MICROSOFT

#include <Windows.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Text encoding conversions on Microsoft platforms.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL MicrosoftPlatformTextEncoding final : public IPlatformTextEncoding
	{
	private:

		// ------------------------------------------------------------------------------------------
		// Encoding.
		// ------------------------------------------------------------------------------------------

		GDINT virtual bool CalculateEncodedLength(WideCStr const text, SizeTp const textLength, SizeTp& encodedLength) const override final;
		GDINT virtual bool EncodeUTF8(Char* const dst, SizeTp const dstLength, WideCStr const src, SizeTp const srcLength) const override final;

		// ------------------------------------------------------------------------------------------
		// Decoding.
		// ------------------------------------------------------------------------------------------

		GDINT virtual bool CalculateDecodedLength(CStr const text, SizeTp const textLength, SizeTp& decodedLength) const override final;
		GDINT virtual bool DecodeUTF8(WideChar* const dst, SizeTp const dstLength, CStr const src, SizeTp const srcLength) const override final;
	};  // class MicrosoftPlatformTextEncoding

	GD_IMPLEMENT_SINGLETON(IPlatformTextEncoding, MicrosoftPlatformTextEncoding)

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
	GDINT bool MicrosoftPlatformTextEncoding::CalculateEncodedLength(WideCStr const text, SizeTp const textLength, SizeTp& encodedLength) const
	{
		GD_ASSERT(text != nullptr && textLength > 0);
		auto const result = WideCharToMultiByte(CP_UTF8, 0, text, static_cast<int>(textLength), nullptr, 0, nullptr, nullptr);
		if (result != 0)
		{
			encodedLength = static_cast<SizeTp>(result);
			return true;
		}
		return false;
	}

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
	GDINT bool MicrosoftPlatformTextEncoding::EncodeUTF8(Char* const dst, SizeTp const dstLength, WideCStr const src, SizeTp const srcLength) const
	{
		GD_ASSERT(dst != nullptr && dstLength > 0);
		GD_ASSERT(src != nullptr && srcLength > 0);
		if (WideCharToMultiByte(CP_UTF8, 0, src, static_cast<int>(srcLength), dst, static_cast<int>(dstLength), nullptr, nullptr) != 0)
		{
			dst[dstLength] = '\0';
			return true;
		}
		return false;
	}

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
	GDINT bool MicrosoftPlatformTextEncoding::CalculateDecodedLength(CStr const text, SizeTp const textLength, SizeTp& decodedLength) const
	{
		GD_ASSERT(text != nullptr && textLength > 0);
		auto const result = MultiByteToWideChar(CP_UTF8, 0, text, static_cast<int>(textLength), nullptr, 0);
		if (result != 0)
		{
			decodedLength = static_cast<SizeTp>(result);
			return true;
		}
		return false;
	}

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
	GDINT bool MicrosoftPlatformTextEncoding::DecodeUTF8(WideChar* const dst, SizeTp const dstLength, CStr const src, SizeTp const srcLength) const
	{
		GD_ASSERT(dst != nullptr && dstLength > 0);
		GD_ASSERT(src != nullptr && srcLength > 0);
		if (MultiByteToWideChar(CP_UTF8, 0, src, static_cast<int>(srcLength), dst, static_cast<int>(dstLength)) != 0)
		{
			dst[dstLength] = L'\0';
			return true;
		}
		return false;
	}

GD_NAMESPACE_END

#endif  // if GD_PLATFORM_API_MICROSOFT
