#include <GoddamnEngine/Core/Containers/String/String.hh>
#include <GoddamnEngine/Core/Containers/String/StringEx.hh>
#include <GoddamnEngine/Core/Diagnostics/Assertion/Assertion.hh>
#include <GoddamnEngine/Core/Allocator/Allocator.hh>

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <wchar.h>
#include <locale>

GD_NAMESPACE_BEGIN
	
	//////////////////////////////////////////////////////////////////////////
	// Supporting helpers
	//////////////////////////////////////////////////////////////////////////

	template<class T>
	GDINL void ArithmeticalToString(String& string, const Char* mode, const T t)
	{
		Char VertexBufferObject[256] = { 0 };

		memset(&VertexBufferObject[0], 0, 256 * sizeof(Char));
		sprintf(&VertexBufferObject[0], mode, t);

		string = String(&VertexBufferObject[0]);
	}

	//////////////////////////////////////////////////////////////////////////
	// Constructors / Destructor
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Formatting constructors
	//////////////////////////////////////////////////////////////////////////

	String String::Format(const String& format, va_list list)
	{
		String result;

		enum class StateMachine
		{
			ReadingPlainData,
			ReadingFormat
		} state = StateMachine::ReadingPlainData;

		for (size_t cnt = 0; cnt < format.GetSize(); cnt += 1)
		{
			const Char character = format[cnt];
			switch (state)
			{
			case StateMachine::ReadingPlainData: 
				{
					if (character == '%')
					{
						state = StateMachine::ReadingFormat;
						continue;
					}

					result.PushLast(character);
				} break;
			case StateMachine::ReadingFormat:
				{
					switch (character)
					{
                    //case '^':	{ result += String(va_arg(list, String const*)); } break;
					case 's':	{ result += String(va_arg(list, CharAnsi*)); } break;
					case 'w':	{ result += String(va_arg(list, CharUtf16*)); } break;
					case 'f':	{ result += String(va_arg(list, float)); } break;
					case 'd':	{ result += String(va_arg(list, int)); } break;
					case 'x':	{ result += String(va_arg(list, handle)); } break;
					case 'c':	{ result += String(va_arg(list, CharAnsi)); } break;
					case 'u':	{ result += String(va_arg(list, CharUtf16)); } break;
					case 'U':	{ result += String(va_arg(list, CharUtf32)); } break;
					case '%':	{ result.PushLast('%'); } break;
                    default:	{ GD_ASSERT_FALSE("'String::Format' error: Unknown character type after '%%'"); } break;
					}

					state = StateMachine::ReadingPlainData;
				} break;
			}
		}

		result[result.GetSize()] = '\0';
		return result;
	}

	String String::Format(const CharAnsi* format, ...)
	{
		va_list list; va_start(list, format);
		return String::Format(String(format), list);
	}

	String String::Format(const CharUtf16* format, ...)
	{
		va_list list; va_start(list, format);
		return String::Format(String(format), list);
	}

	String String::Format(const CharUtf32* format, ...)
	{
		va_list list; va_start(list, format); 
		return String::Format(String(format), list);
	}

	//////////////////////////////////////////////////////////////////////////
	// BASE-64 encoding and decoding
	//////////////////////////////////////////////////////////////////////////

	//void String::EncodeBase64(const Vector<byte>& data, String& output)
	//{
	//	/*const auto IsBase64Character = [](byte b) { return (isalnum(b) || (b == '+') || (b == '/')); };
	//	const String base64Characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	//	size_t i = 0;
	//	size_t j = 0;
	//	byte char3[3] = { '\0' };
	//	byte char4[4] = { '\0' };

	//	output.Resize(0);
	//	const byte* dataPointer = &data[0];
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

	//void String::DecodeBase64(Vector<byte>& output, const String& base64)
	//{
	//}

	//////////////////////////////////////////////////////////////////////////
	// Constructors from Char arrays
	//////////////////////////////////////////////////////////////////////////

	String::String(const nullptr_t _nullptr, const size_t size, Char const fillWith)
	{
		GD_UNUSED(_nullptr);

		self->size		= size;
		self->string	= (Char*)Allocator::AllocateMemory((self->GetSize() + 1) * sizeof(Char));

		memset(self->string, (int)fillWith, (self->GetSize() + 1) * sizeof(Char));
	}

	String::String(const CharAnsi* string)
	{
		GD_ASSERT((string != nullptr), "Nullptr memory specified");

		self->size		= strlen(string);
		self->string	= (Char*)Allocator::AllocateMemory((self->GetSize() + 1) * sizeof(Char));

		memcpy(self->string, string, (self->GetSize() + 1) * sizeof(Char));
	}

	String::String(const CharUtf16* string)
	{
		GD_ASSERT((string != nullptr), "Nullptr memory specified");
		
		self->size = wcslen(string);
		self->string	= (Char*)Allocator::AllocateMemory((self->GetSize() + 1) * sizeof(Char));

		wcstombs(self->string, string, (self->GetSize() + 1) * sizeof(Char));
	}

	String::String(const CharUtf32* string)
	{
		GD_ASSERT((string != nullptr), "Nullptr memory specified");
		
		Char VertexBufferObject[256] = { 0 };
		for (self->size = 0; string[self->size] != (CharUtf32)0; self->size += 1)
		{
			VertexBufferObject[self->GetSize()] = (Char)string[self->GetSize()];
		}

		self->string = (Char*)Allocator::AllocateMemory((self->GetSize() + 1) * sizeof(Char));
		memcpy(self->string, &VertexBufferObject[0], (self->GetSize() + 1) * sizeof(Char));
	}

	String::String(const String& string)
	{
		self->size		= string.GetSize();
		self->string	= (Char*)Allocator::AllocateMemory((self->GetSize() + 1) * sizeof(Char));

		memcpy(self->string, &string[0], (self->GetSize() + 1) * sizeof(Char));
	}

	//////////////////////////////////////////////////////////////////////////
	// Constructors from Arithmetical types
	//////////////////////////////////////////////////////////////////////////

	String::String(const float value)
	{
		self->string = (Char*)Allocator::AllocateMemory(1 * sizeof(Char));
		ArithmeticalToString(*self, "%f", value);
	}

	String::String(const int value)
	{
		self->string = (Char*)Allocator::AllocateMemory(1 * sizeof(Char));
		ArithmeticalToString(*self, "%d", value);
	}

	String::String(const handle value)
	{
		self->string = (Char*)Allocator::AllocateMemory(1 * sizeof(Char));
		ArithmeticalToString(*self, "%x", value);
	}

	String::String(const CharAnsi value)
	{
		self->size		= 1;
		self->string	= (Char*)Allocator::AllocateMemory(2 * sizeof(Char));
		self->string[0] = value;
		self->string[1] = '\0';
	}

	String::String(const CharUtf16 value)
	{
		self->size		= 1;
		self->string	= (Char*)Allocator::AllocateMemory(2 * sizeof(Char));
		self->string[1] = '\0';

		wctomb(&self->string[0], value);
	}

	String::String(const CharUtf32 value)
	{
		self->size		= 1;
		self->string	= (Char*)Allocator::AllocateMemory(2 * sizeof(Char));
		self->string[1] = '\0';

		wctomb(&self->string[0], (CharUtf16)value);
	}

	//////////////////////////////////////////////////////////////////////////
	// Destructor
	//////////////////////////////////////////////////////////////////////////

	String::~String()
	{
		Allocator::DeallocateMemory(self->string);

		self->string = nullptr;
		self->size = 0;
	}


	//////////////////////////////////////////////////////////////////////////
	// String's size
	//////////////////////////////////////////////////////////////////////////

	void String::Resize(const size_t size)
	{
		self->string = (Char*)Allocator::ReallocateMemory(self->string, ((self->size = size) + 1) * sizeof(Char));
		self->string[size] = '\0';
	}

	//////////////////////////////////////////////////////////////////////////
	// String Trimming
	//////////////////////////////////////////////////////////////////////////

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

	//////////////////////////////////////////////////////////////////////////
	// Substring
	//////////////////////////////////////////////////////////////////////////

	String String::GetSubstring(const size_t from, const size_t to) const 
	{
		String string(nullptr, to - from);
		memcpy(&string[0], &(*self)[from], (to - from) * sizeof(Char));

		return string;
	}

	//////////////////////////////////////////////////////////////////////////
	// String Find
	//////////////////////////////////////////////////////////////////////////

	size_t String::Find(const String& string) const
	{
		const Char* location = strstr(self->string, &string[0]);

		return (location 
			? (size_t)(location - self->string) 
			: -1
		);
	}

	size_t String::ReverseFind(const String& string) const
	{
		String copy(*self);
		String reverse = _strrev(&copy[0]);

		size_t const foundIndex = reverse.Find(string);
		return (foundIndex != -1 ? (self->GetSize() - foundIndex) : -1);
	}

	//////////////////////////////////////////////////////////////////////////
	// String Split
	//////////////////////////////////////////////////////////////////////////

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

	//////////////////////////////////////////////////////////////////////////
	// String Hash
	//////////////////////////////////////////////////////////////////////////

	HashSumm String::GetHashSumm() const
	{
		HashSumm::HashValueType computed = 0;
		for (size_t cnt = 0; cnt < self->GetSize(); cnt += 1) 
		{
			computed = 65599 * computed + (*self)[cnt];
		}
		
		return HashSumm((computed >> 16) ^ computed);
	}

	//////////////////////////////////////////////////////////////////////////
	// Converters
	//////////////////////////////////////////////////////////////////////////

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
		for (Char* character = output.string; *character != '\0'; character += sizeof(Char))
		{
			*character = (Char)toupper(*character);
		}

		return output;
	}

	//////////////////////////////////////////////////////////////////////////
	// Operators
	//////////////////////////////////////////////////////////////////////////

	String& String::operator= (const String& string)
	{
		self->Resize(string.GetSize());

		memcpy(&(*self)[0], &string[0], self->GetSize());

		return *self;
	}

	bool String::operator== (const String& string) const
	{ 
		return strcmp(&(*self)[0], &string[0]) == 0; 
	}

	String String::operator+ (const String& string) const
	{
		String result(nullptr, self->GetSize() + string.GetSize());

		memcpy(&result[0], (&self->string[0]), self->GetSize() * sizeof(Char));
		memcpy(&result[self->GetSize()], &string[0], (string.GetSize() + 1) * sizeof(Char));

		return result;
	}
	
GD_NAMESPACE_END
