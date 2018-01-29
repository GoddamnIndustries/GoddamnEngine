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

GD_NAMESPACE_BEGIN

	class StringConv : public TNonCreatable
	{
	public:

		GDINL static UniquePtr<Char[]> EncodeUTF8(WideCStr const text, SizeTp const textLength)
		{
			GD_DEBUG_VERIFY(text != nullptr);
			auto const textEncodedLength = CString::WcsToUTF8(nullptr, 0, text, textLength);
			auto const textEncoded = gd_new Char[textEncodedLength];
			CString::WcsToUTF8(textEncoded, textEncodedLength, text, textLength);
			return UniquePtr<Char[]>(textEncoded);
		}
		GDINL static UniquePtr<Char[]> EncodeUTF8(WideCStr const text)
		{
			GD_DEBUG_VERIFY(text != nullptr);
			return EncodeUTF8(text, CString::Strlen(text));
		}
	};

GD_NAMESPACE_END
