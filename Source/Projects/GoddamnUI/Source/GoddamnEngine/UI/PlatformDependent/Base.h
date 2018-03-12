#pragma once

#include "StringConvert.h"

namespace Engine
{
#define ENGINE_WINDOWS
#define ENGINE_X64
#define ENGINE_PACKED_STRUCTURE __pragma(pack(push, 1))
#define ENGINE_END_PACKED_STRUCTURE __pragma(pack(pop))
#define ENGINE_PI 3.14159265358979323846

#ifdef ENGINE_WINDOWS
	constexpr Encoding SystemEncoding = Encoding::UTF16;
#endif
#ifdef ENGINE_UNIX
	constexpr Encoding SystemEncoding = Encoding::UTF32;
#endif

	typedef SizeTp eint;
	typedef wchar_t widechar;
	typedef void* handle;

#undef InterlockedIncrement
#undef InterlockedDecrement
#undef ZeroMemory
	// Atomic increment and decrement; memory initialization
//	UInt32 InterlockedIncrement(UInt32& Value);
//	UInt32 InterlockedDecrement(UInt32& Value);
	void ZeroMemory(void* Memory, SizeTp Size);

	// System timer's value in milliseconds. The beginning of this time axis is not important
	UInt32 GetTimerValue();

	// Some C standart library and language dependent case insensitive comparation
	void* MemoryCopy(void* Dest, const void* Source, SizeTp Length);
	widechar* StringCopy(widechar* Dest, const widechar* Source);
	Int32 StringCompare(const widechar* A, const widechar* B);
	Int32 SequenceCompare(const widechar* A, const widechar* B, Int32 Length);
	Int32 MemoryCompare(const void* A, const void* B, SizeTp Length);
	Int32 StringCompareCaseInsensitive(const widechar* A, const widechar* B);
	Int32 StringLength(const widechar* str);
	void StringAppend(widechar* str, widechar letter);

	// Case converters for fixed-length strings - should work with any language chars
	void StringLower(widechar* str, Int32 length);
	void StringUpper(widechar* str, Int32 length);
}
