/// ==========================================================================================
/// Base64.h - Base64 implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 22.06.2014 - Created by James Jhuighuy
/// ==========================================================================================

#include <GoddamnEngine/Core/Text/Base64/Base64.h>
#include <GoddamnEngine/Core/IO/Stream/Stream.h>
#include <cctype>

GD_NAMESPACE_BEGIN

	static String const Base64Chars("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
	GDINL static bool IsBase64Char(char const Char) 
	{
		return (::isalnum(Char) || (Char == '+') || (Char == '/'));
	}

	extern void Base64::Encode(InputStream* const Input, OutputStream* const Output)
	{
		size_t I = 0;
		UInt8 CharArray3[3], CharArray4[4];

		while (Input->GetPosition() != Input->GetLength())
		{
			CharArray3[I++] = Input->Read();
			if (I == 3)
			{
				CharArray4[0] =  (CharArray3[0] & 0xFC) >> 2;
				CharArray4[1] = ((CharArray3[0] & 0x03) << 4) + ((CharArray3[1] & 0xF0) >> 4);
				CharArray4[2] = ((CharArray3[1] & 0x0F) << 2) + ((CharArray3[2] & 0xC0) >> 6);
				CharArray4[3] =   CharArray3[2] & 0x3F;

				for (I = 0; I < 4; I++)
					Output->Write(Base64Chars[CharArray4[I]]);

				I = 0;
			}
		}

		if (I != 0)
		{
			for (size_t J = I; J < 3; J++)
				CharArray3[J] = '\0';

			CharArray4[0] =  (CharArray3[0] & 0xFC) >> 2;
			CharArray4[1] = ((CharArray3[0] & 0x03) << 4) + ((CharArray3[1] & 0xF0) >> 4);
			CharArray4[2] = ((CharArray3[1] & 0x0F) << 2) + ((CharArray3[2] & 0xC0) >> 6);
			CharArray4[3] =   CharArray3[2] & 0x3F;

			for (size_t J = 0; (J < I + 1); J++)
				Output->Write(Base64Chars[CharArray4[J]]);

			while (I++ < 3)
				Output->Write('=');
		}
	}

	extern void Base64::Decode(InputStream* const Input, OutputStream* const Output)
	{
		size_t I = 0;
		UInt8 CharArray4[4], CharArray3[3];

		while (Input->GetPosition() != Input->GetLength())
		{
			CharArray4[I++] = Input->Read();
			if ((CharArray4[I] == '=') || (!IsBase64Char(CharArray4[I])))
				break;

			if (I == 4)
			{
				for (I = 0; I < 4; I++)
					CharArray4[I] = Base64Chars[Base64Chars.Find(CharArray4[I])];

				CharArray3[0] = ((CharArray4[1] & 0x30) >> 4) +  (CharArray4[0] << 2);
				CharArray3[1] = ((CharArray4[1] & 0x0F) << 4) + ((CharArray4[2] & 0x3C) >> 2);
				CharArray3[2] = ((CharArray4[2] & 0x03) << 6) +   CharArray4[3];

				for (I = 0; (I < 3); I++)
					Output->Write(CharArray3[I]);

				I = 0;
			}
		}

		if (I != 0) 
		{
			for (size_t J = I; J < 4; J++) CharArray4[J] = 0;
			for (size_t J = 0; J < 4; J++) CharArray4[J] = Base64Chars[Base64Chars.Find(CharArray4[J])];

			CharArray3[0] = ((CharArray4[1] & 0x30) >> 4) +  (CharArray4[0] << 2);
			CharArray3[1] = ((CharArray4[1] & 0x0f) << 4) + ((CharArray4[2] & 0x3C) >> 2);
			CharArray3[2] = ((CharArray4[2] & 0x03) << 6) +   CharArray4[3];

			for (size_t J = 0; (J < I - 1); J++)
				Output->Write(CharArray3[J]);
		}
	}

GD_NAMESPACE_END
