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
#if GD_PLATFORM_API_POSIX

#include <iconv.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Text encoding conversions on Posix platforms.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL PosixPlatformTextEncoding final : public IPlatformTextEncoding
	{
	private:
		iconv_t m_WideCharToUTF8;
		iconv_t m_UTF8ToWideChar;

	public:
		GDINT PosixPlatformTextEncoding();
		GDINT virtual ~PosixPlatformTextEncoding();

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
	};  // class PosixPlatformTextEncoding

	GD_IMPLEMENT_SINGLETON(IPlatformTextEncoding, PosixPlatformTextEncoding)

	GDINT PosixPlatformTextEncoding::PosixPlatformTextEncoding()
		: m_WideCharToUTF8(), m_UTF8ToWideChar()
	{
		m_WideCharToUTF8 = iconv_open("UTF-8", "UTF-32LE");
		m_UTF8ToWideChar = iconv_open("UTF-32LE", "UTF-8");
	}

	GDINT PosixPlatformTextEncoding::~PosixPlatformTextEncoding()
	{
		iconv_close(m_WideCharToUTF8);
		iconv_close(m_UTF8ToWideChar);
	}

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
	GDINT bool PosixPlatformTextEncoding::CalculateEncodedLength(WideCStr const text, SizeTp const textLength, SizeTp& encodedLength) const
	{
		if (m_WideCharToUTF8 != nullptr)
		{
			auto inBuffer = reinterpret_cast<Char*>(const_cast<WideChar*>(text));
			auto inBufferBytesLeft = ::size_t(sizeof(*text) * textLength);
			String textEncoded(textLength * 4);
			auto outBuffer = textEncoded.CStr();
			auto outBufferBytesLeft = ::size_t(sizeof(textEncoded[0]) * textEncoded.GetLength());
			if (iconv(m_WideCharToUTF8, &inBuffer, &inBufferBytesLeft, &outBuffer, &outBufferBytesLeft) != -1)
			{
				auto const encodedLengthInBytes = sizeof(textEncoded[0]) * textEncoded.GetLength() - outBufferBytesLeft;
				encodedLength = encodedLengthInBytes / sizeof(textEncoded[0]);
				return true;
			}
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
	GDINT bool PosixPlatformTextEncoding::EncodeUTF8(Char* const dst, SizeTp const dstLength, WideCStr const src, SizeTp const srcLength) const
	{
		if (m_WideCharToUTF8 != nullptr)
		{
			auto inBuffer = reinterpret_cast<Char*>(const_cast<WideChar*>(src));
			auto inBufferBytesLeft = ::size_t(sizeof(*src) * srcLength);
			auto outBuffer = dst;
			auto outBufferBytesLeft = ::size_t(sizeof(*dst) * dstLength);
			return iconv(m_WideCharToUTF8, &inBuffer, &inBufferBytesLeft, &outBuffer, &outBufferBytesLeft) != -1;
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
	GDINT bool PosixPlatformTextEncoding::CalculateDecodedLength(CStr const text, SizeTp const textLength, SizeTp& decodedLength) const
	{
		if (m_WideCharToUTF8 != nullptr)
		{
			auto inBuffer = const_cast<Char*>(text);
			auto inBufferBytesLeft = ::size_t(sizeof(*text) * textLength);
			WideString textDecoded(textLength);
			auto outBuffer = reinterpret_cast<Char*>(textDecoded.CStr());
			auto outBufferBytesLeft = ::size_t(sizeof(textDecoded[0]) * textDecoded.GetLength());
			if (iconv(m_UTF8ToWideChar, &inBuffer, &inBufferBytesLeft, &outBuffer, &outBufferBytesLeft) != -1)
			{
				auto const decodedLengthInBytes = sizeof(textDecoded[0]) * textDecoded.GetLength() - outBufferBytesLeft;
				decodedLength = decodedLengthInBytes / sizeof(textDecoded[0]);
				return true;
			}
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
	GDINT bool PosixPlatformTextEncoding::DecodeUTF8(WideChar* const dst, SizeTp const dstLength, CStr const src, SizeTp const srcLength) const
	{
		if (m_UTF8ToWideChar != nullptr)
		{
			auto inBuffer = const_cast<Char*>(src);
			auto inBufferBytesLeft = ::size_t(sizeof(*src) * srcLength);
			auto outBuffer = reinterpret_cast<Char*>(dst);
			auto outBufferBytesLeft = ::size_t(sizeof(*dst) * dstLength);
			return iconv(m_UTF8ToWideChar, &inBuffer, &inBufferBytesLeft, &outBuffer, &outBufferBytesLeft) != -1;
		}
		return false;
	}

GD_NAMESPACE_END

#endif  // if GD_PLATFORM_API_POSIX
