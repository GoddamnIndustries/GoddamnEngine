#pragma once

#include <GoddamnEngine/Include.h>
GD_USING_NAMESPACE;
#undef EOF

namespace Engine
{
	enum class Encoding
	{
		ANSI = 0,
		UTF8 = 1,
		UTF16 = 2,
		UTF32 = 3
	};

	Int32 MeasureSequenceLength(const void* Source, Int32 SourceLength, Encoding From, Encoding To);
	void ConvertEncoding(void* Dest, const void* Source, Int32 SourceLength, Encoding From, Encoding To);
	Int32 GetBytesPerChar(Encoding encoding);
}
