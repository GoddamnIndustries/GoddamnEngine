#include "Base.h"

#include <Shlwapi.h>
#include <Windows.h>
#include <TimeAPI.h>

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "winmm.lib")

#undef InterlockedIncrement
#undef InterlockedDecrement
#undef ZeroMemory

namespace Engine
{
	UInt32 InterlockedIncrement(UInt32& Value)
	{
		return _InterlockedIncrement(&Value);
	}
	UInt32 InterlockedDecrement(UInt32& Value)
	{
		return _InterlockedDecrement(&Value);
	}
	void ZeroMemory(void* Memory, SizeTp Size)
	{
		memset(Memory, 0, Size);
	}
	UInt32 GetTimerValue()
	{
		return timeGetTime();
	}
	void* MemoryCopy(void* Dest, const void* Source, SizeTp Length)
	{
		return memcpy(Dest, Source, Length);
	}
	widechar* StringCopy(widechar* Dest, const widechar* Source)
	{
		Int32 i = -1;
		do
		{
			i++;
			Dest[i] = Source[i];
		} while (Source[i]);
		return Dest;
	}
	Int32 StringCompare(const widechar* A, const widechar* B)
	{
		Int32 i = 0;
		while (A[i] == B[i] && A[i])
			i++;
		if (A[i] == 0 && B[i] == 0)
			return 0;
		if (A[i] < B[i])
			return -1;
		return 1;
	}
	Int32 SequenceCompare(const widechar* A, const widechar* B, Int32 Length)
	{
		Int32 i = 0;
		while (A[i] == B[i] && i < Length)
			i++;
		if (i == Length)
			return 0;
		if (A[i] < B[i])
			return -1;
		return 1;
	}
	Int32 MemoryCompare(const void* A, const void* B, SizeTp Length)
	{
		return memcmp(A, B, Length);
	}
	Int32 StringCompareCaseInsensitive(const widechar* A, const widechar* B)
	{
		return StrCmpIW(A, B);
	}
	Int32 StringLength(const widechar* str)
	{
		Int32 l = 0;
		while (str[l])
			l++;
		return l;
	}
	void StringAppend(widechar* str, widechar letter)
	{
		auto len = StringLength(str);
		str[len + 1] = 0;
		str[len] = letter;
	}

	void StringLower(widechar* str, Int32 length)
	{
		for (Int32 i = 0; i < length; i++)
			str[i] = *(CharLowerW(reinterpret_cast<LPWSTR>(str[i])));
	}
	void StringUpper(widechar* str, Int32 length)
	{
		for (Int32 i = 0; i < length; i++)
			str[i] = *(CharUpperW(reinterpret_cast<LPWSTR>(str[i])));
	}
}
