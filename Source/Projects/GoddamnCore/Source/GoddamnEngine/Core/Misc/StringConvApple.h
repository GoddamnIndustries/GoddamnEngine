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
#    error This file should be never directly included, please consider using <GoddamnEngine/Core/Misc/StringConv.h> instead.
#endif    // if !defined(GD_INSIDE_CSTRING_H)

GD_NAMESPACE_BEGIN

    // **------------------------------------------------------------------------------------------**
    //! @copydoc StringConvBaseGeneric
    // **------------------------------------------------------------------------------------------**
    class StringConvBaseApple : public StringConvBaseGeneric
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
        GDAPI static SizeTp CalculateEncodedLength(WideCStr const text, SizeTp const textLength);
        
        /*!
         * Encodes the specified wide-character text in UTF-8.
         *
         * @param dest Destination buffer.
         * @param destLength Length of the destination buffer.
         * @param source Text to encode.
         * @param sourceLength Length of the specified text.
         */
        GDAPI static void EncodeUTF8(Char* const dest, SizeTp const destLength, WideChar const* const source, SizeTp const sourceLength);
        
        // ------------------------------------------------------------------------------------------
        // Decoding.
        // ------------------------------------------------------------------------------------------
        
        /*!
         * Calculates length of UTF-8 text in wide characters.
         *
         * @param text Text for calculation.
         * @param textLength Length of the text.
         */
        GDAPI static SizeTp CalculateDecodedLength(CStr const text, SizeTp const textLength);
        
        /*!
         * Decodes the specified wide-character text in UTF-8.
         *
         * @param dest Destination buffer.
         * @param destLength Length of the destination buffer.
         * @param source Text to decode.
         * @param sourceLength Length of the specified text.
         */
        GDAPI static void DecodeUTF8(WideChar* const dest, SizeTp const destLength, Char const* const source, SizeTp const sourceLength);
        
    };    // class StringConvBaseApple

    // **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
    //! @copydoc StringConvBaseApple
    // **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
    using StringConvBase = StringConvBaseApple;

GD_NAMESPACE_END

