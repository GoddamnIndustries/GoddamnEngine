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
#if GD_PLATFORM_API_COCOA

#import <Foundation/Foundation.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Text encoding conversions on Apple platforms.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL ApplePlatformTextEncoding final : public IPlatformTextEncoding
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
	};  // class ApplePlatformTextEncoding

	GD_IMPLEMENT_SINGLETON(IPlatformTextEncoding, ApplePlatformTextEncoding)

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
	GDINT bool ApplePlatformTextEncoding::CalculateEncodedLength(WideCStr const text, SizeTp const textLength, SizeTp& encodedLength) const
	{
		GD_ASSERT(text != nullptr && textLength > 0);
		auto const textApple = [[NSString alloc] initWithBytes:text length:textLength * sizeof(*text) encoding:NSUTF32LittleEndianStringEncoding];
        auto const textAppleUTF8 = [textApple UTF8String];
		if (textAppleUTF8 != nullptr)
		{
			encodedLength = CString::Strlen(textAppleUTF8);
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
	GDINT bool ApplePlatformTextEncoding::EncodeUTF8(Char* const dst, SizeTp const dstLength, WideCStr const src, SizeTp const srcLength) const
	{
		GD_ASSERT(dst != nullptr && dstLength > 0);
		GD_ASSERT(src != nullptr && srcLength > 0);
		auto const srcApple = [[NSString alloc] initWithBytes:src length:srcLength * sizeof(*src) encoding:NSUTF32LittleEndianStringEncoding];
        auto const srcAppleUTF8 = [sourceApple UTF8String];
		if (srcAppleUTF8 != nullptr)
		{
			CMemory::CMemcpy(dst, srcAppleUTF8, (dstLength + 1) * sizeof(*dst));
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
	GDINT bool ApplePlatformTextEncoding::CalculateDecodedLength(CStr const text, SizeTp const textLength, SizeTp& decodedLength) const
	{
		GD_ASSERT(text != nullptr && textLength > 0);
        auto const textApple = [NSString stringWithUTF8String:text];
        decodedLength = static_cast<SizeTp>([textApple length]);
		return true;
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
	GDINT bool ApplePlatformTextEncoding::DecodeUTF8(WideChar* const dst, SizeTp const dstLength, CStr const src, SizeTp const srcLength) const
	{
		GD_ASSERT(dst != nullptr && dstLength > 0);
		GD_ASSERT(src != nullptr && srcLength > 0);
		auto const srcApple = [NSString stringWithUTF8String:src];
		auto const srcAppleUTF32 = reinterpret_cast<WideCStr>([srcApple cStringUsingEncoding:NSUTF32LittleEndianStringEncoding]);
		if (srcAppleUTF32 != nullptr)
		{
			CMemory::CMemcpy(dst, sourceAppleUTF32, (dstLength + 1) * sizeof(*dst));
			return true;
		}
		return false;
	}

GD_NAMESPACE_END

#endif  // if GD_PLATFORM_API_COCOA
