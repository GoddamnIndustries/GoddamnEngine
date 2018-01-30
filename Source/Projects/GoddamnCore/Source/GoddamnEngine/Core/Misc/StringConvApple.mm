// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
//
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#include <GoddamnEngine/Core/Misc/StringConv.h>
#if GD_PLATFORM_API_COCOA

#import <Foundation/Foundation.h>

GD_NAMESPACE_BEGIN

    // ------------------------------------------------------------------------------------------
    // Encoding.
    // ------------------------------------------------------------------------------------------

    /*!
     * Calculates length of wide-character text in UTF-8 in bytes.
     *
     * @param text Text for calculation.
     * @param textLength Length of the text.
     */
    GDAPI SizeTp StringConvBaseApple::CalculateEncodedLength(WideCStr const text, SizeTp const textLength)
    {
        GD_DEBUG_VERIFY(text != nullptr && textLength > 0);
        
        auto const textApple = [[NSString alloc] initWithBytes:text length:textLength * sizeof(*text) encoding:NSUTF32LittleEndianStringEncoding];
        auto const textAppleUTF8 = [textApple UTF8String];
        GD_VERIFY(textAppleUTF8 != nullptr, "'[NSString UTF8String]' function has failed.");
        return CString::Strlen(textAppleUTF8);
    }

    /*!
     * Encodes the specified wide-character text in UTF-8.
     *
     * @param dest Destination buffer.
     * @param destLength Length of the destination buffer.
     * @param source Text to encode.
     * @param sourceLength Length of the specified text.
     */
    GDAPI void StringConvBaseApple::EncodeUTF8(Char* const dest, SizeTp const destLength, WideChar const* const source, SizeTp const sourceLength)
    {
        GD_DEBUG_VERIFY(dest != nullptr && destLength > 0);
        GD_DEBUG_VERIFY(source != nullptr && sourceLength > 0);
        
        auto const sourceApple = [[NSString alloc] initWithBytes:source length:sourceLength * sizeof(*source) encoding:NSUTF32LittleEndianStringEncoding];
        auto const sourceAppleUTF8 = [sourceApple UTF8String];
        auto const sourceAppleUTF8Length = CString::Strlen(sourceAppleUTF8);
        GD_VERIFY(sourceAppleUTF8 != nullptr, "'[NSString UTF8String]' function has failed.");
        GD_DEBUG_VERIFY(destLength >= sourceAppleUTF8Length);
        CMemory::CMemcpy(dest, sourceAppleUTF8, sourceAppleUTF8Length + 1);
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
    GDAPI SizeTp StringConvBaseApple::CalculateDecodedLength(CStr const text, SizeTp const textLength)
    {
        GD_DEBUG_VERIFY(text != nullptr && textLength > 0);
        auto const textApple = [NSString stringWithUTF8String:text];
        return static_cast<SizeTp>([textApple length]);
    }

    /*!
     * Decodes the specified wide-character text in UTF-8.
     *
     * @param dest Destination buffer.
     * @param destLength Length of the destination buffer.
     * @param source Text to decode.
     * @param sourceLength Length of the specified text.
     */
    GDAPI void StringConvBaseApple::DecodeUTF8(WideChar* const dest, SizeTp const destLength, Char const* const source, SizeTp const sourceLength)
    {
        GD_DEBUG_VERIFY(dest != nullptr && destLength > 0);
        GD_DEBUG_VERIFY(source != nullptr && sourceLength > 0);
        
        auto const sourceApple = [NSString stringWithUTF8String:source];
        auto const sourceAppleUTF32 = reinterpret_cast<WideCStr>([sourceApple cStringUsingEncoding:NSUTF32LittleEndianStringEncoding]);
        auto const sourceAppleUTF32Length = CString::Strlen(sourceAppleUTF32);
        GD_VERIFY(sourceAppleUTF32 != nullptr, "'[NSString cStringUsingEncoding]' function has failed.");
        GD_DEBUG_VERIFY(destLength >= sourceAppleUTF32Length);
        CMemory::CMemcpy(dest, sourceAppleUTF32, sourceAppleUTF32Length + 1);
    }

GD_NAMESPACE_END

#endif  // if GD_PLATFORM_API_COCOA

int main()
{
    return 0;
}
