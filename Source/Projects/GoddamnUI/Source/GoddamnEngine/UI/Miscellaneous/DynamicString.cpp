#include "DynamicString.h"

namespace Engine
{
#if 0
	DynamicString::DynamicString()
	    : data(0x100)
	{
		data << L'\0';
	}
	DynamicString::DynamicString(const ImmutableString& src)
	    : data(0x100)
	{
		data += Array<widechar>(src.CStr(), src.CStr() + src.GetLength() + 1);
	}
	DynamicString::DynamicString(Int32 BlockSize)
	    : data(BlockSize)
	{
		data << L'\0';
	}
	DynamicString::DynamicString(const ImmutableString& src, Int32 BlockSize)
	    : data(BlockSize)
	{
		data += Array<widechar>(src.CStr(), src.CStr() + src.GetLength() + 1);
	}
	DynamicString::operator ImmutableString() const
	{
		return ToString();
	}
	DynamicString::operator widechar*()
	{
		return data.GetData();
	}
	DynamicString::operator const widechar*() const
	{
		return data.GetData();
	}
	ImmutableString DynamicString::ToString() const
	{
		return ImmutableString(data.GetData(), data.GetLength());
	}
	Int32 DynamicString::Length() const
	{
		return StringLength(data.GetData());
	}
	widechar DynamicString::CharAt(Int32 index) const
	{
		return data[index];
	}
	widechar& DynamicString::CharAt(Int32 index)
	{
		return data[index];
	}
	widechar DynamicString::operator[](Int32 index) const
	{
		return data[index];
	}
	widechar& DynamicString::operator[](Int32 index)
	{
		return data[index];
	}
	void DynamicString::Concatenate(const ImmutableString& str)
	{
		Int32 len = Length();
		data.Resize(len + str.GetLength() + 1);
		StringCopy(data.GetData() + len, str.CStr());
	}
	void DynamicString::Concatenate(widechar letter)
	{
		Int32 len = Length();
		data[len] = letter;
		data << L'\0';
	}
	DynamicString& DynamicString::operator+=(const ImmutableString& str)
	{
		Concatenate(str);
		return *this;
	}
	DynamicString& DynamicString::operator+=(widechar letter)
	{
		Concatenate(letter);
		return *this;
	}
	DynamicString& DynamicString::operator<<(const ImmutableString& str)
	{
		Concatenate(str);
		return *this;
	}
	DynamicString& DynamicString::operator<<(widechar letter)
	{
		Concatenate(letter);
		return *this;
	}
	void DynamicString::Insert(const ImmutableString& str, Int32 at)
	{
		Int32 len = Length();
		Int32 ins = str.GetLength();
		data.Resize(len + ins + 1);
		for (Int32 i = len; i >= at; i--)
			data[i + ins] = data[i];
		MemoryCopy(data.GetData() + at, str.CStr(), ins * sizeof(widechar));
	}
	void DynamicString::RemoveRange(Int32 start, Int32 amount)
	{
		Int32 len = Length();
		for (Int32 i = start + amount; i <= len; i++)
			data[i - amount] = data[i];
		data.Resize(len - amount + 1);
	}
	void DynamicString::Clear()
	{
		data.Clear();
		data << L'\0';
	}
	void DynamicString::ReserveLength(Int32 length)
	{
		data.Resize(length + 1);
	}
	Int32 DynamicString::ReservedLength() const
	{
		return data.GetLength();
	}
#endif 
}
