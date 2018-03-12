#include "EngineBase.h"

namespace Engine
{
	namespace SymbolicMath
	{
		string Summ(const string& a, const string& b)
		{
			Int32 dot1 = a.Find(L'.');
			Int32 dot2 = b.Find(L'.');
			string a1 = (dot1 < dot2) ? string(L'0', dot2 - dot1) + a : a;
			string b1 = (dot2 < dot1) ? string(L'0', dot1 - dot2) + b : b;
			if (a1.GetLength() > b1.GetLength())
				b1 += string(L'0', a1.GetLength() - b1.GetLength());
			else
				a1 += string(L'0', b1.GetLength() - a1.GetLength());
			widechar* s1 = gd_new widechar[a1.GetLength() + 1];
			if (!s1)
				throw OutOfMemoryException();
			Int32 carry = 0;
			for (Int32 i = a1.GetLength() - 1; i >= 0; i--)
			{
				if (a1[i] == L'.')
				{
					s1[i + 1] = L'.';
				}
				else
				{
					Int32 summ = Int32(a1[i] - L'0') + Int32(b1[i] - L'0') + carry;
					s1[i + 1] = (summ % 10) + L'0';
					carry = summ / 10;
				}
			}
			s1[0] = L'0' + (widechar)carry;
			try
			{
				string s = (s1[0] == L'0') ? string(s1 + 1, a1.GetLength()) : string(s1, a1.GetLength() + 1);
				gd_delete[] s1;
				s1 = 0;
				return s;
			}
			catch (...)
			{
				gd_delete[] s1;
				throw;
			}
		}
		string DivideByTwo(const string& a)
		{
			widechar* s1 = gd_new widechar[a.GetLength() + 1];
			if (!s1)
				throw OutOfMemoryException();
			Int32 carry = 0;
			for (Int32 i = 0; i < a.GetLength(); i++)
			{
				if (a[i] == L'.')
				{
					s1[i] = L'.';
				}
				else
				{
					Int32 value = carry * 10 + Int32(a[i] - L'0');
					s1[i] = L'0' + (widechar)(value / 2);
					carry = value % 2;
				}
			}
			if (carry)
				s1[a.GetLength()] = L'5';
			try
			{
				string s = (carry) ? string(s1, a.GetLength() + 1) : string(s1, a.GetLength());
				gd_delete[] s1;
				s1 = 0;
				return s;
			}
			catch (...)
			{
				gd_delete[] s1;
				throw;
			}
		}
		string TwoPower(Int32 power)
		{
			if (power > 0)
			{
				Int32 len = power / 2 + 5;
				widechar* s1 = gd_new widechar[len];
				if (!s1)
					throw OutOfMemoryException();
				for (Int32 i = 0; i < len - 3; i++)
					s1[i] = L'0';
				s1[len - 3] = L'1';
				s1[len - 2] = L'.';
				s1[len - 1] = L'0';
				Int32 carry, nsig = len - 3;
				for (Int32 j = 0; j < power; j++)
				{
					carry = 0;
					for (Int32 i = len - 3; i >= 0; i--)
					{
						Int32 value = Int32(s1[i] - L'0') * 2 + carry;
						s1[i] = L'0' + (value % 10);
						carry = value / 10;
						if (!carry && i <= nsig)
						{
							nsig = i;
							break;
						}
					}
				}
				try
				{
					string s = string(s1, len);
					gd_delete[] s1;
					s1 = 0;
					return s;
				}
				catch (...)
				{
					gd_delete[] s1;
					throw;
				}
			}
			else if (power < 0)
			{
				string base = L"1.0";
				for (Int32 i = 0; i < -power; i++)
					base = DivideByTwo(base);
				return base;
			}
			else
				return L"1.0";
		}
		string Round(const string& a, Int32 signchars, widechar separator)
		{
			Int32 fnz = 0;
			while (a[fnz] == L'0' || a[fnz] == L'.')
				fnz++;
			Int32 truncf = fnz;
			for (Int32 i = 0; i < signchars; i++)
			{
				truncf++;
				if (truncf >= a.GetLength())
					break;
				if (a[truncf] == L'.')
					truncf++;
			}
			Int32 len = truncf + 1;
			Int32 dot = a.Find(L'.');
			Int32 ll;
			if (truncf > dot)
				ll = len;
			else
				ll = len + (dot - truncf) + 1;
			widechar* s1 = gd_new widechar[ll];
			if (!s1)
				throw OutOfMemoryException();
			for (Int32 i = len; i < ll; i++)
				s1[i] = L'0';
			if (len != ll)
				s1[ll - 1] = L'.';
			Int32 carry = ((truncf < a.GetLength()) && (a[truncf] >= L'5')) ? 1 : 0;
			if (carry)
			{
				for (Int32 i = len - 1; i >= 1; i--)
				{
					if (a[i - 1] == L'.')
					{
						s1[i] = L'.';
					}
					else
					{
						Int32 value = Int32(a[i - 1] - L'0') + carry;
						s1[i] = L'0' + (value % 10);
						carry = value / 10;
					}
				}
				s1[0] = L'0' + (widechar)carry;
			}
			else
			{
				for (Int32 i = 1; i < len; i++)
					s1[i] = a[i - 1];
				s1[0] = L'0';
			}
			Int32 begin = 0;
			while (s1[begin] == L'0')
				begin++;
			if (s1[begin] == L'.')
				begin--;
			Int32 len2 = ll - 1;
			while (s1[len2] == L'0')
				len2--;
			if (s1[len2] == L'.')
				len2--;
			for (Int32 i = 0; i < ll; i++)
				if (s1[i] == L'.')
				{
					s1[i] = separator;
					break;
				}
			try
			{
				string s = string(s1 + begin, len2 + 1 - begin);
				gd_delete[] s1;
				s1 = 0;
				return s;
			}
			catch (...)
			{
				gd_delete[] s1;
				throw;
			}
		}
	}

#if 0
	Object::Object()
	    : _refcount(1)
	{
	}
	UInt32 Object::AddRef()
	{
		return InterlockedIncrement(_refcount);
	}
	UInt32 Object::Release()
	{
		UInt32 result = InterlockedDecrement(_refcount);
		if (result == 0)
			gd_delete this;
		return result;
	}
	Object::~Object()
	{
	}
	ImmutableString Object::ToString() const
	{
		return ImmutableString(L"Object");
	}
	UInt32 Object::GetReferenceCount() const
	{
		return _refcount;
	}
#endif

	Exception::Exception()
	{
	}
	Exception::Exception(const Exception& e)
	{
	}
	Exception::Exception(Exception&& e)
	{
	}
	Exception& Exception::operator=(const Exception& e)
	{
		return *this;
	}
	ImmutableString Exception::ToString() const
	{
		return ImmutableString(L"Exception");
	}
	ImmutableString OutOfMemoryException::ToString() const
	{
		return ImmutableString(L"OutOfMemoryException");
	}
	ImmutableString InvalidArgumentException::ToString() const
	{
		return ImmutableString(L"InvalidArgumentException");
	}
	ImmutableString InvalidFormatException::ToString() const
	{
		return ImmutableString(L"InvalidFormatException");
	}

#if 0
	ImmutableString::ImmutableString() { text = gd_new widechar[1]; if (!text) throw OutOfMemoryException(); text[0] = 0; }
	ImmutableString::ImmutableString(const ImmutableString & src) { text = gd_new widechar[src.GetLength() + 1]; if (!text) throw OutOfMemoryException(); StringCopy(text, src.text); }
	ImmutableString::ImmutableString(ImmutableString && src) { text = src.text; src.text = 0; }
	ImmutableString::ImmutableString(const widechar * src) { text = gd_new widechar[StringLength(src) + 1]; if (!text) throw OutOfMemoryException(); StringCopy(text, src); }
	ImmutableString::ImmutableString(const widechar * sequence, Int32 length) { text = gd_new widechar[length + 1]; if (!text) throw OutOfMemoryException(); text[length] = 0; MemoryCopy(text, sequence, sizeof(widechar) * length); }
	ImmutableString::ImmutableString(Int32 src)
	{
		widechar * conv = gd_new widechar[0x10];
		if (!conv) throw OutOfMemoryException();
		conv[0] = 0;
		if (src == 0xFFFFFFFF) {
			StringCopy(conv, L"-2147483648");
		} else {
			bool neg = false;
			if (src < 0) {
				src = -src; neg = true;
			}
			do {
				Int32 r = src % 10;
				src /= 10;
				StringAppend(conv, L"0123456789"[r]);
			} while (src);
			if (neg) StringAppend(conv, L'-');
		}
		Int32 len = StringLength(conv);
		text = gd_new widechar[len + 1];
		if (!text) { gd_delete[] conv; throw OutOfMemoryException(); }
		for (Int32 i = 0; i < len; i++) text[i] = conv[len - i - 1];
		text[len] = 0;
		gd_delete[] conv;
	}
	ImmutableString::ImmutableString(UInt32 src)
	{
		widechar * conv = gd_new widechar[0x10];
		if (!conv) throw OutOfMemoryException();
		conv[0] = 0;
		do {
			Int32 r = src % 10;
			src /= 10;
			StringAppend(conv, L"0123456789"[r]);
		} while (src);
		Int32 len = StringLength(conv);
		text = gd_new widechar[len + 1];
		if (!text) { gd_delete[] conv; throw OutOfMemoryException(); }
		for (Int32 i = 0; i < len; i++) text[i] = conv[len - i - 1];
		text[len] = 0;
		gd_delete[] conv;
	}
	ImmutableString::ImmutableString(Int64 src)
	{
		widechar * conv = gd_new widechar[0x20];
		if (!conv) throw OutOfMemoryException();
		conv[0] = 0;
		if (src == 0xFFFFFFFFFFFFFFFF) {
			StringCopy(conv, L"-9223372036854775808");
		} else {
			bool neg = false;
			if (src < 0) {
				src = -src; neg = true;
			}
			do {
				Int32 r = src % 10;
				src /= 10;
				StringAppend(conv, L"0123456789"[r]);
			} while (src);
			if (neg) StringAppend(conv, L'-');
		}
		Int32 len = StringLength(conv);
		text = gd_new widechar[len + 1];
		if (!text) { gd_delete[] conv; throw OutOfMemoryException(); }
		for (Int32 i = 0; i < len; i++) text[i] = conv[len - i - 1];
		text[len] = 0;
		gd_delete[] conv;
	}
	ImmutableString::ImmutableString(UInt64 src)
	{
		widechar * conv = gd_new widechar[0x20];
		if (!conv) throw OutOfMemoryException();
		conv[0] = 0;
		do {
			Int32 r = src % 10;
			src /= 10;
			StringAppend(conv, L"0123456789"[r]);
		} while (src);
		Int32 len = StringLength(conv);
		text = gd_new widechar[len + 1];
		if (!text) { gd_delete[] conv; throw OutOfMemoryException(); }
		for (Int32 i = 0; i < len; i++) text[i] = conv[len - i - 1];
		text[len] = 0;
		gd_delete[] conv;
	}
	ImmutableString::ImmutableString(UInt32 value, const ImmutableString & digits, Int32 minimal_length)
	{
		if (minimal_length < 0 || minimal_length > 0x20) throw InvalidArgumentException();
		widechar * conv = gd_new widechar[0x40];
		if (!conv) throw OutOfMemoryException();
		conv[0] = 0;
		Int32 radix = digits.GetLength();
		if (radix <= 1) { gd_delete[] conv; throw InvalidArgumentException(); }
		do {
			Int32 r = value % radix;
			value /= radix;
			StringAppend(conv, digits[r]);
		} while (value);
		while (StringLength(conv) < minimal_length) StringAppend(conv, digits[0]);
		Int32 len = StringLength(conv);
		text = gd_new widechar[len + 1];
		if (!text) { gd_delete[] conv; throw OutOfMemoryException(); }
		for (Int32 i = 0; i < len; i++) text[i] = conv[len - i - 1];
		text[len] = 0;
		gd_delete[] conv;
	}
	ImmutableString::ImmutableString(UInt64 value, const ImmutableString & digits, Int32 minimal_length)
	{
		if (minimal_length < 0 || minimal_length > 0x40) throw InvalidArgumentException();
		widechar * conv = gd_new widechar[0x80];
		if (!conv) throw OutOfMemoryException();
		conv[0] = 0;
		Int32 radix = digits.GetLength();
		if (radix <= 1) { gd_delete[] conv; throw InvalidArgumentException(); }
		do {
			Int32 r = value % radix;
			value /= radix;
			StringAppend(conv, digits[r]);
		} while (value);
		while (StringLength(conv) < minimal_length) StringAppend(conv, digits[0]);
		Int32 len = StringLength(conv);
		text = gd_new widechar[len + 1];
		if (!text) { gd_delete[] conv; throw OutOfMemoryException(); }
		for (Int32 i = 0; i < len; i++) text[i] = conv[len - i - 1];
		text[len] = 0;
		gd_delete[] conv;
	}
	ImmutableString::ImmutableString(const void * src) : ImmutableString(SizeTp(src), L"0123456789ABCDEF", sizeof(void*) * 2) {}
	ImmutableString::ImmutableString(const void * Sequence, Int32 Length, Encoding SequenceEncoding)
	{
		Int32 len = MeasureSequenceLength(Sequence, Length, SequenceEncoding, SystemEncoding);
		text = gd_new widechar[len + 1];
		if (!text) throw OutOfMemoryException();
		ConvertEncoding(text, Sequence, Length, SequenceEncoding, SystemEncoding);
		text[len] = 0;
	}
	ImmutableString::ImmutableString(Float32 src, widechar separator) : ImmutableString()
	{
		UInt32 & value = reinterpret_cast<UInt32&>(src);
		bool negative = (value & 0x80000000) != 0;
		Int32 exp = (value & 0x7F800000) >> 23;
		value &= 0x007FFFFF;
		if (exp == 0) {
			if (value == 0) *this = L"0";
			else {
				string power = SymbolicMath::TwoPower(-126);
				string base = L"0.0";
				for (Int32 i = 22; i >= 0; i--) {
					power = SymbolicMath::DivideByTwo(power);
					if ((value >> i) & 1) base = SymbolicMath::Summ(base, power);
				}
				base = SymbolicMath::Round(base, 7, separator);
				*this = (negative) ? (L"-" + base) : base;
			}
		} else if (exp == 0xFF) {
			if (value == 0) *this = (negative) ? L"-\x221E" : L"+\x221E";
			else *this = L"NaN";
		} else {
			exp -= 127;
			string power = SymbolicMath::TwoPower(exp);
			string base = power;
			for (Int32 i = 22; i >= 0; i--) {
				power = SymbolicMath::DivideByTwo(power);
				if ((value >> i) & 1) base = SymbolicMath::Summ(base, power);
			}
			base = SymbolicMath::Round(base, 7, separator);
			*this = (negative) ? (L"-" + base) : base;
		}
	}
	ImmutableString::ImmutableString(Float64 src, widechar separator) : ImmutableString()
	{
		UInt64 & value = reinterpret_cast<UInt64&>(src);
		bool negative = (value & 0x8000000000000000) != 0;
		Int32 exp = (value & 0x7FF0000000000000) >> 52;
		value &= 0x000FFFFFFFFFFFFF;
		if (exp == 0) {
			if (value == 0) *this = L"0";
			else {
				string power = SymbolicMath::TwoPower(-1022);
				string base = L"0.0";
				for (Int32 i = 51; i >= 0; i--) {
					power = SymbolicMath::DivideByTwo(power);
					if ((value >> i) & 1) base = SymbolicMath::Summ(base, power);
				}
				base = SymbolicMath::Round(base, 16, separator);
				*this = (negative) ? (L"-" + base) : base;
			}
		} else if (exp == 0x7FF) {
			if (value == 0) *this = (negative) ? L"-\x221E" : L"+\x221E";
			else *this = L"NaN";
		} else {
			exp -= 1023;
			string power = SymbolicMath::TwoPower(exp);
			string base = power;
			for (Int32 i = 51; i >= 0; i--) {
				power = SymbolicMath::DivideByTwo(power);
				if ((value >> i) & 1) base = SymbolicMath::Summ(base, power);
			}
			base = SymbolicMath::Round(base, 16, separator);
			*this = (negative) ? (L"-" + base) : base;
		}
	}
	ImmutableString::ImmutableString(bool src) : ImmutableString(src ? L"true" : L"false") {}
	ImmutableString::ImmutableString(widechar src) { text = gd_new widechar[2]; if (!text) throw OutOfMemoryException(); text[0] = src; text[1] = 0; }
	ImmutableString::ImmutableString(widechar src, Int32 repeats)
	{
		if (repeats < 0) throw InvalidArgumentException();
		text = gd_new widechar[repeats + 1]; if (!text) throw OutOfMemoryException();
		for (Int32 i = 0; i < repeats; i++) text[i] = src; text[repeats] = 0;
	}
	ImmutableString::ImmutableString(const Object * object) : ImmutableString(object->ToString()) {}
	ImmutableString::~ImmutableString() { gd_delete[] text; }
	ImmutableString & ImmutableString::operator=(const ImmutableString & src)
	{
		if (this == &src) return *this;
		widechar * alloc = gd_new widechar[src.GetLength() + 1];
		if (!alloc) throw gd_new OutOfMemoryException();
		gd_delete[] text; text = alloc;
		StringCopy(text, src.text);
		return *this;
	}
	ImmutableString & ImmutableString::operator=(const widechar * src)
	{
		if (text == src) return *this;
		widechar * alloc = gd_new widechar[StringLength(src) + 1];
		if (!alloc) throw gd_new OutOfMemoryException();
		gd_delete[] text; text = alloc;
		StringCopy(text, src);
		return *this;
	}
	ImmutableString::operator const widechar*() const { return text; }
	Int32 ImmutableString::Length() const { return StringLength(text); }
	Int32 ImmutableString::Compare(const ImmutableString & a, const ImmutableString & b) { return StringCompare(a.text, b.text); }
	Int32 ImmutableString::CompareIgnoreCase(const ImmutableString & a, const ImmutableString & b) { return StringCompareCaseInsensitive(a.text, b.text); }
	widechar ImmutableString::operator[](Int32 index) const { return text[index]; }
	widechar ImmutableString::CharAt(Int32 index) const { return text[index]; }
	ImmutableString ImmutableString::ToString() const { return *this; }
	void ImmutableString::Concatenate(const ImmutableString & str)
	{
		Int32 len = Length();
		widechar * alloc = gd_new widechar[len + str.GetLength() + 1];
		if (!alloc) throw OutOfMemoryException();
		StringCopy(alloc, text);
		StringCopy(alloc + len, str.text);
		gd_delete[] text;
		text = alloc;
	}
	ImmutableString & ImmutableString::operator+=(const ImmutableString & str) { Concatenate(str); return *this; }
	Int32 ImmutableString::FindFirst(widechar letter) const { for (Int32 i = 0; i < Length(); i++) if (text[i] == letter) return i; return -1; }
	Int32 ImmutableString::FindFirst(const ImmutableString & str) const
	{
		Int32 len = str.GetLength();
		if (!len) throw InvalidArgumentException();
		for (Int32 i = 0; i < Length() - len + 1; i++) if (SequenceCompare(text + i, str.text, len) == 0) return i;
		return -1;
	}
	Int32 ImmutableString::FindLast(widechar letter) const { for (Int32 i = Length() - 1; i >= 0; i--) if (text[i] == letter) return i; return -1; }
	Int32 ImmutableString::FindLast(const ImmutableString & str) const
	{
		Int32 len = str.GetLength();
		if (!len) throw InvalidArgumentException();
		for (Int32 i = Length() - len; i >= 0; i--) if (SequenceCompare(text + i, str.text, len) == 0) return i;
		return -1;
	}
	ImmutableString ImmutableString::Fragment(Int32 PosStart, Int32 CharLength) const
	{
		Int32 len = Length();
		if (PosStart < 0 || PosStart > len || !CharLength) return L"";
		if (CharLength < -1) return L"";
		if (PosStart + CharLength > len || CharLength == -1) CharLength = len - PosStart;
		return ImmutableString(text + PosStart, CharLength);
	}
	Int32 ImmutableString::GeneralizedFindFirst(Int32 from, const widechar * seq, Int32 length) const
	{
		for (Int32 i = from; i < Length() - length + 1; i++) if (SequenceCompare(text + i, seq, length) == 0) return i;
		return -1;
	}
	ImmutableString ImmutableString::GeneralizedReplace(const widechar * seq, Int32 length, const widechar * with, Int32 withlen) const
	{
		if (!length) throw InvalidArgumentException();
		ImmutableString result;
		ImmutableString With(with, withlen);
		Int32 pos = 0, len = Length();
		while (pos < len) {
			Int32 next = GeneralizedFindFirst(pos, seq, length);
			if (next != -1) {
				Int32 copylen = next - pos;
				if (copylen) result += Fragment(pos, copylen);
				result += With;
				pos = next + length;
			} else {
				result += Fragment(pos, len - pos);
				len = pos;
			}
		}
		return result;
	}
	UInt64 ImmutableString::GeneralizedToUInt64(Int32 dfrom, Int32 dto) const
	{
		UInt64 value = 0;
		for (Int32 i = dfrom; i < dto; i++) {
			if (text[i] < L'0' || text[i] > L'9') throw InvalidFormatException();
			UInt64 digit = text[i] - L'0';
			if (value >= 0x199999999999999A) throw InvalidFormatException();
			value *= 10;
			if (value > 0xFFFFFFFFFFFFFFFF - digit) throw InvalidFormatException();
			value += digit;
		}
		return value;
	}
	UInt64 ImmutableString::GeneralizedToUInt64(Int32 dfrom, Int32 dto, const ImmutableString & digits, bool case_sensitive) const
	{
		UInt64 value = 0;
		auto base = digits.GetLength();
		if (base < 2) throw InvalidArgumentException();
		widechar input[2], compare[2];
		input[1] = compare[1] = 0;
		UInt64 max_prem = 0xFFFFFFFFFFFFFFFF / base + 1;
		for (Int32 i = dfrom; i < dto; i++) {
			Int32 dn = -1;
			input[0] = text[i];
			for (Int32 j = 0; j < digits.GetLength(); j++) {
				compare[0] = digits[j];
				if ((case_sensitive && StringCompare(input, compare) == 0) || (!case_sensitive && StringCompareCaseInsensitive(input, compare) == 0)) { dn = j; break; }
			}
			if (dn == -1) throw InvalidFormatException();
			UInt64 digit = dn;
			if (value >= max_prem) throw InvalidFormatException();
			value *= base;
			if (value > 0xFFFFFFFFFFFFFFFF - digit) throw InvalidFormatException();
			value += digit;
		}
		return value;
	}
	ImmutableString ImmutableString::Replace(const ImmutableString & Substring, const ImmutableString & with) const { return GeneralizedReplace(Substring, Substring.GetLength(), with, with.GetLength()); }
	ImmutableString ImmutableString::Replace(widechar Substring, const ImmutableString & with) const { return GeneralizedReplace(&Substring, 1, with, with.GetLength()); }
	ImmutableString ImmutableString::Replace(const ImmutableString & Substring, widechar with) const { return GeneralizedReplace(Substring, Substring.GetLength(), &with, 1); }
	ImmutableString ImmutableString::Replace(widechar Substring, widechar with) const { return GeneralizedReplace(&Substring, 1, &with, 1); }
	ImmutableString ImmutableString::LowerCase() const { ImmutableString result = *this; StringLower(result.text, Length()); return result; }
	ImmutableString ImmutableString::UpperCase() const { ImmutableString result = *this; StringUpper(result.text, Length()); return result; }
	Int32 ImmutableString::GetEncodedLength(Encoding encoding) const { return MeasureSequenceLength(text, Length(), SystemEncoding, encoding); }
	void ImmutableString::Encode(void * buffer, Encoding encoding, bool include_terminator) const { ConvertEncoding(buffer, text, Length() + (include_terminator ? 1 : 0), SystemEncoding, encoding); }
	Array<UInt8>* ImmutableString::EncodeSequence(Encoding encoding, bool include_terminator) const
	{
		Int32 char_length = GetEncodedLength(encoding) + (include_terminator ? 1 : 0);
		Int32 src_length = Length() + (include_terminator ? 1 : 0);
		Array<UInt8> * result = gd_new Array<UInt8>;
		try {
			result->SetLength(char_length * GetBytesPerChar(encoding));
			ConvertEncoding(*result, text, src_length, SystemEncoding, encoding);
		}
		catch (...) { result->Release(); throw; }
		return result;
	}
	Array<ImmutableString> ImmutableString::Split(widechar divider) const
	{
		Array<ImmutableString> result;
		Int32 pos = -1, len = Length();
		while (pos < len) {
			Int32 div = GeneralizedFindFirst(pos + 1, &divider, 1);
			pos++;
			if (div == -1) {
				result << Fragment(pos, len - pos);
				pos = len;
			} else {
				result << Fragment(pos, div - pos);
				pos = div;
			}
		}
		return result;
	}
	UInt64 ImmutableString::ToUInt64() const { return GeneralizedToUInt64(0, Length()); }
	UInt64 ImmutableString::ToUInt64(const ImmutableString & digits, bool case_sensitive) const { return GeneralizedToUInt64(0, Length(), digits, case_sensitive); }
	Int64 ImmutableString::ToInt64() const
	{
		bool negative = false;
		Int32 start = 0;
		auto len = Length();
		if (!len) return 0;
		if (text[0] == L'-') { start = 1; negative = true; }
		else if (text[0] == L'+') { start = 1; }
		auto absolute = GeneralizedToUInt64(start, len);
		if (negative) {
			if (absolute > 0x8000000000000000) throw InvalidFormatException();
			else return -Int64(absolute);
		} else {
			if (absolute > 0x7FFFFFFFFFFFFFFF) throw InvalidFormatException();
			return absolute;
		}
	}
	Int64 ImmutableString::ToInt64(const ImmutableString & digits, bool case_sensitive) const
	{
		bool negative = false;
		Int32 start = 0;
		auto len = Length();
		if (!len) return 0;
		if (text[0] == L'-') { start = 1; negative = true; } else if (text[0] == L'+') { start = 1; }
		auto absolute = GeneralizedToUInt64(start, len, digits, case_sensitive);
		if (negative) {
			if (absolute > 0x8000000000000000) throw InvalidFormatException();
			else return -Int64(absolute);
		} else {
			if (absolute > 0x7FFFFFFFFFFFFFFF) throw InvalidFormatException();
			return absolute;
		}
	}
	UInt32 ImmutableString::ToUInt32() const
	{
		UInt64 parsed = GeneralizedToUInt64(0, Length());
		if (parsed > 0xFFFFFFFF) throw InvalidFormatException();
		return UInt32(parsed);
	}
	UInt32 ImmutableString::ToUInt32(const ImmutableString & digits, bool case_sensitive) const
	{
		UInt64 parsed = GeneralizedToUInt64(0, Length(), digits, case_sensitive);
		if (parsed > 0xFFFFFFFF) throw InvalidFormatException();
		return UInt32(parsed);
	}
	Int32 ImmutableString::ToInt32() const
	{
		bool negative = false;
		Int32 start = 0;
		auto len = Length();
		if (!len) return 0;
		if (text[0] == L'-') { start = 1; negative = true; } else if (text[0] == L'+') { start = 1; }
		auto absolute = GeneralizedToUInt64(start, len);
		if (negative) {
			if (absolute > 0x80000000) throw InvalidFormatException();
			else return -Int32(absolute);
		} else {
			if (absolute > 0x7FFFFFFF) throw InvalidFormatException();
			return Int32(absolute);
		}
	}
	Int32 ImmutableString::ToInt32(const ImmutableString & digits, bool case_sensitive) const
	{
		bool negative = false;
		Int32 start = 0;
		auto len = Length();
		if (!len) return 0;
		if (text[0] == L'-') { start = 1; negative = true; } else if (text[0] == L'+') { start = 1; }
		auto absolute = GeneralizedToUInt64(start, len, digits, case_sensitive);
		if (negative) {
			if (absolute > 0x80000000) throw InvalidFormatException();
			else return -Int32(absolute);
		} else {
			if (absolute > 0x7FFFFFFF) throw InvalidFormatException();
			return Int32(absolute);
		}
	}
	Float32 ImmutableString::ToFloat() const
	{
		Float32 value = 0.0f;
		bool negative = false;
		Int32 start = 0;
		auto len = Length();
		if (!len) return 0.0f;
		if (text[0] == L'-') { start = 1; negative = true; } else if (text[0] == L'+') { start = 1; }
		Int32 point = FindFirst(L',');
		if (point == -1) point = FindFirst(L'.');
		if (point == -1) point = len;
		if (point != max(FindLast(L','), FindLast(L'.'))) throw InvalidFormatException();
		for (Int32 i = start; i < point; i++) {
			if (text[i] < L'0' || text[i] > L'9') throw InvalidFormatException();
			Int32 digit = text[i] - L'0';
			value *= 10.0f;
			value += Float32(digit);
		}
		Float32 exp = 1.0f;
		for (Int32 i = point + 1; i < len; i++) {
			if (text[i] < L'0' || text[i] > L'9') throw InvalidFormatException();
			Int32 digit = text[i] - L'0';
			exp /= 10.0f;
			value += exp * Float32(digit);
		}
		return negative ? -value : value;
	}
	Float32 ImmutableString::ToFloat(const ImmutableString & separators) const
	{
		Float32 value = 0.0f;
		bool negative = false;
		Int32 start = 0;
		auto len = Length();
		if (!len) return 0.0f;
		if (text[0] == L'-') { start = 1; negative = true; } else if (text[0] == L'+') { start = 1; }
		Int32 point = len;
		for (Int32 i = 0; i < separators.GetLength(); i++) {
			Int32 fp = FindFirst(separators[i]);
			if (fp != -1 && fp < point) point = fp;
		}
		for (Int32 i = 0; i < separators.GetLength(); i++) {
			if (FindLast(separators[i]) > point) throw InvalidFormatException();
		}
		for (Int32 i = start; i < point; i++) {
			if (text[i] < L'0' || text[i] > L'9') throw InvalidFormatException();
			Int32 digit = text[i] - L'0';
			value *= 10.0f;
			value += Float32(digit);
		}
		Float32 exp = 1.0f;
		for (Int32 i = point + 1; i < len; i++) {
			if (text[i] < L'0' || text[i] > L'9') throw InvalidFormatException();
			Int32 digit = text[i] - L'0';
			exp /= 10.0f;
			value += exp * Float32(digit);
		}
		return negative ? -value : value;
	}
	Float64 ImmutableString::ToDouble() const
	{
		Float64 value = 0.0;
		bool negative = false;
		Int32 start = 0;
		auto len = Length();
		if (!len) return 0.0;
		if (text[0] == L'-') { start = 1; negative = true; } else if (text[0] == L'+') { start = 1; }
		Int32 point = FindFirst(L',');
		if (point == -1) point = FindFirst(L'.');
		if (point == -1) point = len;
		if (point != max(FindLast(L','), FindLast(L'.'))) throw InvalidFormatException();
		for (Int32 i = start; i < point; i++) {
			if (text[i] < L'0' || text[i] > L'9') throw InvalidFormatException();
			Int32 digit = text[i] - L'0';
			value *= 10.0;
			value += Float64(digit);
		}
		Float64 exp = 1.0;
		for (Int32 i = point + 1; i < len; i++) {
			if (text[i] < L'0' || text[i] > L'9') throw InvalidFormatException();
			Int32 digit = text[i] - L'0';
			exp /= 10.0;
			value += exp * Float64(digit);
		}
		return negative ? -value : value;
	}
	Float64 ImmutableString::ToDouble(const ImmutableString & separators) const
	{
		Float64 value = 0.0;
		bool negative = false;
		Int32 start = 0;
		auto len = Length();
		if (!len) return 0.0;
		if (text[0] == L'-') { start = 1; negative = true; } else if (text[0] == L'+') { start = 1; }
		Int32 point = len;
		for (Int32 i = 0; i < separators.GetLength(); i++) {
			Int32 fp = FindFirst(separators[i]);
			if (fp != -1 && fp < point) point = fp;
		}
		for (Int32 i = 0; i < separators.GetLength(); i++) {
			if (FindLast(separators[i]) > point) throw InvalidFormatException();
		}
		for (Int32 i = start; i < point; i++) {
			if (text[i] < L'0' || text[i] > L'9') throw InvalidFormatException();
			Int32 digit = text[i] - L'0';
			value *= 10.0;
			value += Float64(digit);
		}
		Float64 exp = 1.0;
		for (Int32 i = point + 1; i < len; i++) {
			if (text[i] < L'0' || text[i] > L'9') throw InvalidFormatException();
			Int32 digit = text[i] - L'0';
			exp /= 10.0;
			value += exp * Float64(digit);
		}
		return negative ? -value : value;
	}
	bool ImmutableString::ToBoolean() const
	{
		if (CompareIgnoreCase(*this, L"true") == 0 || CompareIgnoreCase(*this, L"1") == 0) return true;
		else if (CompareIgnoreCase(*this, L"false") == 0 || CompareIgnoreCase(*this, L"0") == 0 || Length() == 0) return false;
		else throw InvalidFormatException();
	}
	bool operator==(const ImmutableString & a, const ImmutableString & b) { return ImmutableString::Compare(a, b) == 0; }
	bool operator==(const widechar * a, const ImmutableString & b) { return StringCompare(a, b) == 0; }
	bool operator==(const ImmutableString & a, const widechar * b) { return StringCompare(a, b) == 0; }
	bool operator!=(const ImmutableString & a, const ImmutableString & b) { return ImmutableString::Compare(a, b) != 0; }
	bool operator!=(const widechar * a, const ImmutableString & b) { return StringCompare(a, b) != 0; }
	bool operator!=(const ImmutableString & a, const widechar * b) { return StringCompare(a, b) != 0; }
	bool operator<=(const ImmutableString & a, const ImmutableString & b) { return StringCompare(a, b) <= 0; }
	bool operator>=(const ImmutableString & a, const ImmutableString & b) { return StringCompare(a, b) >= 0; }
	bool operator<(const ImmutableString & a, const ImmutableString & b) { return StringCompare(a, b) < 0; }
	bool operator>(const ImmutableString & a, const ImmutableString & b) { return StringCompare(a, b) > 0; }
	ImmutableString operator+(const ImmutableString & a, const ImmutableString & b) { ImmutableString result = a; return result += b; }
	ImmutableString operator+(const widechar * a, const ImmutableString & b) { return ImmutableString(a) + b; }
	ImmutableString operator+(const ImmutableString & a, const widechar * b) { return a + ImmutableString(b); }
#endif
	Float64 sgn(Float64 x)
	{
		return (x > 0.0) ? 1.0 : ((x < 0.0) ? -1.0 : 0.0);
	}
	Float32 sgn(Float32 x)
	{
		return (x > 0.0f) ? 1.0f : ((x < 0.0f) ? -1.0f : 0.0f);
	}
}
