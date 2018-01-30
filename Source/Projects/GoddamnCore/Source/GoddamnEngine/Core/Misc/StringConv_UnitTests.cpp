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
#include <GoddamnEngine/Core/Misc/StringConv.h>
#if GD_TESTING_ENABLED

GD_NAMESPACE_BEGIN

	gd_testing_unit_test(StringConvEncode)
	{
		// Somthing mysterious text in Russian.
		WideChar textInRussianWide[] = L"\x0413\x043E\x0432\x043D\x043E\x0020\x0436\x043E\x043F\x0430";
		Char textInRussianUTF8[] = "\xD0\x93\xD0\xBE\xD0\xB2\xD0\xBD\xD0\xBE\x20\xD0\xB6\xD0\xBE\xD0\xBF\xD0\xB0";  // NOLINT
		
		auto const textEncoded = StringConv::EncodeUTF8(textInRussianWide);
		gd_testing_assert(CString::Strcmp(textEncoded.Get(), textInRussianUTF8) == 0);

		auto const textDecoded = StringConv::DecodeUTF8(textInRussianUTF8);
		gd_testing_assert(CString::Strcmp(textDecoded.Get(), textInRussianWide) == 0);
	};

GD_NAMESPACE_END

#endif
