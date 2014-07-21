/// ==========================================================================================
/// Base64.h - Base64 interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 22.06.2014 - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_TEXT_BASE64
#define GD_CORE_TEXT_BASE64

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Text/String/String.h>
#include <GoddamnEngine/Core/Containers/Vector/Vector.h>

GD_NAMESPACE_BEGIN
	
	class InputStream;
	class OutputStream;

	/// Base64 algorithm implementation based on following http://www.adp-gmbh.ch/cpp/common/base64.html.
	/// Can convert exsisting binary data to/from plain text data.
	namespace Base64
	{
		/// Encodes binary data to plain text using BASE-64 algorithm.
		/// @param Input  Input stream that contains binary data that requires to be encoded.
		/// @param Output Output stream to which encoded string data would be written.
		GDAPI extern void Encode(InputStream* const Input, OutputStream* const Output);

		/// Decodes plain text into binary data using BASE-64 algorithm.
		/// @param Input  Input stream that BASE-64 encoded string data
		/// @param Output Output stream to which decoded binary data would be written.
		GDAPI extern void Decode(InputStream* const Input, OutputStream* const Output);
	}	/// namespace Base64

GD_NAMESPACE_END

#endif
