#include <GoddamnEngine/Core/Text/String/String.h>
#if 0
#include <GoddamnEngine/Core/Text/String/StringEx.h>
#include <GoddamnEngine/Core/Diagnostics/Assertion/Assertion.h>
#include <GoddamnEngine/Core/Allocator/Allocator.h>

#include <stdarg.h>
#include <memory.h>
#include <stdio.h>
#include <wchar.h>
#include <locale>

GD_NAMESPACE_BEGIN
	
	/// ==========================================================================================
	// Supporting helpers
	/// ==========================================================================================

	template<class T>
	GDINL void ArithmeticalToString(String& HeapMemory, const Char* mode, const T t)
	{
		Char VertexBufferObject[256] = { 0 };

		memset(&VertexBufferObject[0], 0, 256 * sizeof(Char));
		sprintf(&VertexBufferObject[0], mode, t);

		HeapMemory = String(&VertexBufferObject[0]);
	}

	/// ==========================================================================================
	// Constructors / Destructor
	/// ==========================================================================================

	/// ==========================================================================================
	// Formatting constructors
	/// ==========================================================================================

	String String::FormatVa(Str const Format, va_list const List)
	{
		Char Buffer0[1024];
		memset(&Buffer0[0], 0, sizeof(Buffer0));
		int const Result0 = vsnprintf(&Buffer0[0], GD_ARRAY_SIZE(Buffer0), Format, List);
		///**/ if (Result0 < 0)
		//{
		//	GD_ASSERT_FALSE("String formatting failed due format error.");
		//	return String();
		//}
		/*else*/ if ((Result0 > 0) && (Result0 < GD_ARRAY_SIZE(Buffer0)))
			return String(&Buffer0[0]);

		Char Buffer1[2048];
		memset(&Buffer1[0], 0, sizeof(Buffer1));
		int const Result1 = vsnprintf(&Buffer1[0], GD_ARRAY_SIZE(Buffer1), Format, List);
		///**/ if (Result1 < 0)
		//{
		//	GD_ASSERT_FALSE("String formatting failed due format error.");
		//	return String();
		//}
		/*else*/ if ((Result1 > 0) && (Result1 < GD_ARRAY_SIZE(Buffer1)))
			return String(&Buffer1[0]);

		GD_ASSERT_FALSE("String formatting failed buffers error.");
		return String();
	}

	String String::Format(const CharAnsi* format, ...)
	{
		va_list list; va_start(list, format);
		return Forward<String>(String::FormatVa(format, list));
	}

	/*String&& String::Format(const CharUtf16* format, ...)
	{
		va_list list; va_start(list, format);
		return String::FormatVa(format, list);
	}

	String&& String::Format(const CharUtf32* format, ...)
	{
		va_list list; va_start(list, format); 
		return String::FormatVa(format, list);
	}*/

	/// ==========================================================================================
	// BASE-64 encoding and decoding
	/// ==========================================================================================

	//void String::EncodeBase64(const Vector<UInt8>& data, String& output)
	//{
	//	/*const auto IsBase64Character = [](UInt8 b) { return (isalnum(b) || (b == '+') || (b == '/')); };
	//	const String base64Characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	//	size_t i = 0;
	//	size_t j = 0;
	//	UInt8 char3[3] = { '\0' };
	//	UInt8 char4[4] = { '\0' };

	//	output.Resize(0);
	//	const UInt8* dataPointer = &data[0];
	//	for (GDInt64 cnt = (GDInt64)data.GetSize(); cnt >= 0; cnt -= 1)
	//	{
	//	char3[i++] = *(dataPointer++);
	//	if (i == 3)
	//	{
	//	char4[0] = (char3[0] & 0xFC) >> 2;
	//	char4[1] = ((char3[0] & 0x03) << 4) + ((char3[1] & 0xF0) >> 4);
	//	char4[2] = ((char3[1] & 0x0F) << 2) + ((char3[2] & 0xC0) >> 6);
	//	char4[3] = char3[2] & 0x3F;

	//	for (i = 0; i < 4; i += 1)
	//	{
	//	output.PushLast(base64Characters[char4[i]]);
	//	}

	//	i = 0;
	//	}
	//	}

	//	if (i != 0)
	//	{
	//	for (j = i; j < 3; j += 1)
	//	{
	//	char3[j] = '\0';
	//	}

	//	char4[0] = (char3[0] & 0xFC) >> 2;
	//	char4[1] = ((char3[0] & 0x03) << 4) + ((char3[1] & 0xF0) >> 4);
	//	char4[2] = ((char3[1] & 0x0F) << 2) + ((char3[2] & 0xC0) >> 6);
	//	char4[3] = char3[2] & 0x3F;

	//	for (j = 0; (j < i + 1); j += 1)
	//	{
	//	output.PushLast(base64Characters[char4[j]]);
	//	}

	//	while ((i++ < 3))
	//	{
	//	output.PushLast('=');
	//	}
	//	}*/
	//}

	//void String::DecodeBase64(Vector<UInt8>& output, const String& base64)
	//{
	//}

	/// ==========================================================================================
	// Constructors from Char arrays
	/// ==========================================================================================

	String::String(const nullptr_t _nullptr, const size_t HeapSize, Char const fillWith)
	{
		GD_UNUSED(_nullptr);

		self->HeapSize		= HeapSize;
		self->HeapMemory	= (Char*)Allocator::AllocateMemory((self->GetSize() + 1) * sizeof(Char));

		memset(self->HeapMemory, (int)fillWith, (self->GetSize()) * sizeof(Char));
		*self->End() = '\0';
	}

	String::String(const CharAnsi* HeapMemory)
	{
		GD_ASSERT((HeapMemory != nullptr), "Nullptr memory specified");

		self->HeapSize		= strlen(HeapMemory);
		self->HeapMemory	= (Char*)Allocator::AllocateMemory((self->GetSize() + 1) * sizeof(Char));

		memcpy(self->HeapMemory, HeapMemory, (self->GetSize() + 1) * sizeof(Char));
	}

	String::String(const CharUtf16* HeapMemory)
	{
		GD_ASSERT((HeapMemory != nullptr), "Nullptr memory specified");
		
		self->HeapSize = wcslen(HeapMemory);
		self->HeapMemory	= (Char*)Allocator::AllocateMemory((self->GetSize() + 1) * sizeof(Char));

		wcstombs(self->HeapMemory, HeapMemory, (self->GetSize() + 1) * sizeof(Char));
	}

	String::String(const CharUtf32* HeapMemory)
	{
		GD_ASSERT((HeapMemory != nullptr), "Nullptr memory specified");
		
		Char VertexBufferObject[256] = { 0 };
		for (self->HeapSize = 0; HeapMemory[self->HeapSize] != (CharUtf32)0; self->HeapSize += 1)
		{
			VertexBufferObject[self->GetSize()] = (Char)HeapMemory[self->GetSize()];
		}

		self->HeapMemory = (Char*)Allocator::AllocateMemory((self->GetSize() + 1) * sizeof(Char));
		memcpy(self->HeapMemory, &VertexBufferObject[0], (self->GetSize() + 1) * sizeof(Char));
	}

	String::String(const String& HeapMemory)
	{
		self->HeapSize		= HeapMemory.GetSize();
		self->HeapMemory	= (Char*)Allocator::AllocateMemory((self->GetSize() + 1) * sizeof(Char));

		memcpy(self->HeapMemory, &HeapMemory[0], (self->GetSize() + 1) * sizeof(Char));
	}

	/// ==========================================================================================
	// Constructors from Arithmetical types
	/// ==========================================================================================

	String::String(const float value)
	{
		self->HeapMemory = (Char*)Allocator::AllocateMemory(1 * sizeof(Char));
		ArithmeticalToString(*self, "%f", value);
	}

	String::String(const int value)
	{
		self->HeapMemory = (Char*)Allocator::AllocateMemory(1 * sizeof(Char));
		ArithmeticalToString(*self, "%d", value);
	}

	String::String(const handle value)
	{
		self->HeapMemory = (Char*)Allocator::AllocateMemory(1 * sizeof(Char));
		ArithmeticalToString(*self, "%x", value);
	}

	String::String(const CharAnsi value)
	{
		self->HeapSize		= 1;
		self->HeapMemory	= (Char*)Allocator::AllocateMemory(2 * sizeof(Char));
		self->HeapMemory[0] = value;
		self->HeapMemory[1] = '\0';
	}

	String::String(const CharUtf16 value)
	{
		self->HeapSize		= 1;
		self->HeapMemory	= (Char*)Allocator::AllocateMemory(2 * sizeof(Char));
		self->HeapMemory[1] = '\0';

		wctomb(&self->HeapMemory[0], value);
	}

	String::String(const CharUtf32 value)
	{
		self->HeapSize		= 1;
		self->HeapMemory	= (Char*)Allocator::AllocateMemory(2 * sizeof(Char));
		self->HeapMemory[1] = '\0';

		wctomb(&self->HeapMemory[0], (CharUtf16)value);
	}

	/// ==========================================================================================
	// Destructor
	/// ==========================================================================================

	String::~String()
	{
		Allocator::DeallocateMemory(self->HeapMemory);

		self->HeapMemory = nullptr;
		self->HeapSize = 0;
	}


	/// ==========================================================================================
	// String's HeapSize
	/// ==========================================================================================

	void String::Resize(const size_t HeapSize)
	{
		self->HeapMemory = (Char*)Allocator::ReallocateMemory(self->HeapMemory, ((self->HeapSize = HeapSize) + 1) * sizeof(Char));
		self->HeapMemory[HeapSize] = '\0';
	}

	/// ==========================================================================================
	// String Trimming
	/// ==========================================================================================

	String String::TrimmLeft() const
	{
		static const String spaces = " \t\r\n";

		size_t index = 0;
		for (; (index < self->GetSize() && (spaces.Find((*self)[index]) != -1)); index += 1);

		return self->GetSubstring(index);
	}

	String String::TrimmRight() const
	{
		static const String spaces = " \t\r\n";

		size_t index = self->GetSize() - 1;
		for (; (index != -1 && (spaces.Find((*self)[index]) != -1)); index -= 1);

		return self->GetSubstring(0, index + 1);
	}

	/// ==========================================================================================
	// Substring
	/// ==========================================================================================

	String String::GetSubstring(const size_t from, const size_t to) const 
	{
		String HeapMemory(nullptr, to - from);
		memcpy(&HeapMemory[0], &(*self)[from], (to - from) * sizeof(Char));

		return HeapMemory;
	}

	/// ==========================================================================================
	// String Find
	/// ==========================================================================================

	size_t String::Find(const String& HeapMemory) const
	{
		const Char* location = strstr(self->HeapMemory, &HeapMemory[0]);

		return (location 
			? (size_t)(location - self->HeapMemory) 
			: -1
		);
	}

	size_t String::ReverseFind(const String& HeapMemory) const
	{
		String copy(*self);
		String reverse = _strrev(&copy[0]);

		size_t const foundIndex = reverse.Find(HeapMemory);
		return (foundIndex != -1 ? (self->GetSize() - foundIndex) : -1);
	}

	/// ==========================================================================================
	// String Split
	/// ==========================================================================================

	Vector<String> String::Split(const Char c, const bool include) const
	{
		String s(*self), VertexBufferObject;
		Vector<String> result;
		size_t index = 0;
		
		while ((index = s.Find(c)) != -1)
		{
			VertexBufferObject = s.GetSubstring(0, index);
			result.PushLast(VertexBufferObject);
			if (include)
				result.PushLast(String(c));

			s = s.GetSubstring(index + 1);
		}

		result.PushLast(s);

		return result;
	}

	/// ==========================================================================================
	// String Hash
	/// ==========================================================================================

	HashSumm String::GetHashSumm() const
	{
		HashSumm::HashValueType computed = 0;
		for (size_t cnt = 0; cnt < self->GetSize(); cnt += 1) 
		{
			computed = 65599 * computed + (*self)[cnt];
		}
		
		return HashSumm((computed >> 16) ^ computed);
	}

	/// ==========================================================================================
	// Converters
	/// ==========================================================================================

	float String::ToFloat() const
	{
		float result = 0.0f;
		#pragma warning(suppress: 6031)
		sscanf(&(*self)[0], "%f", &result);

		return result;
	}

	int String::ToInt() const
	{
		int result = 0;
		#pragma warning(suppress: 6031)
		sscanf(&(*self)[0], "%d", &result);

		return result;
	}

	String String::ToUpper() const
	{
		String output(*self);
		for (Char* character = output.HeapMemory; *character != '\0'; character += sizeof(Char))
		{
			*character = (Char)toupper(*character);
		}

		return output;
	}

	String String::ToLower() const
	{
		String output(*self);
		for (Char* character = output.HeapMemory; *character != '\0'; character += sizeof(Char))
		{
			*character = (Char)tolower(*character);
		}

		return output;
	}

	/// ==========================================================================================
	// Operators
	/// ==========================================================================================

	String& String::operator= (const String& HeapMemory)
	{
		self->Resize(HeapMemory.GetSize());

		memcpy(&(*self)[0], &HeapMemory[0], self->GetSize());

		return *self;
	}

	bool String::operator== (const String& HeapMemory) const
	{ 
		return strcmp(&(*self)[0], &HeapMemory[0]) == 0; 
	}

	String String::operator+ (const String& HeapMemory) const
	{
		String result(nullptr, self->GetSize() + HeapMemory.GetSize());

		memcpy(&result[0], (&self->HeapMemory[0]), self->GetSize() * sizeof(Char));
		memcpy(&result[self->GetSize()], &HeapMemory[0], (HeapMemory.GetSize() + 1) * sizeof(Char));

		return result;
	}
	
GD_NAMESPACE_END

#endif 0
