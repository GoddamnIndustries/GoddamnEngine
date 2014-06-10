#if !defined(GD_CORE_CONTAINERS_STRING)
#	pragma error("Attempting to include 'String.inl' file. Please, use 'String.h' instead")
#endif

GD_NAMESPACE_BEGIN

	GDINL String::String(const Vector<CharAnsi>& vector)
	{
		self->string = (Char*)malloc(1 * sizeof(Char));
		self->size = 1;

		*self = String(&vector[0]);
	}

	GDINL String::String(const Vector<CharUtf16>& vector)
	{
		self->string = (Char*)malloc(1 * sizeof(Char));
		self->size = 1;

		*self = String(&vector[0]);
	}

	GDINL String::String(const Vector<CharUtf32>& vector)
	{
		self->string = (Char*)malloc(1 * sizeof(Char));
		self->size = 1;

		*self = String(&vector[0]);
	}

	GDINL size_t String::GetSize() const
	{ 
		return self->size; 
	}

	GDINL String String::Trimm() const
	{ 
		return self->TrimmLeft().TrimmRight(); 
	}

	GDINL String String::GetSubstring(const size_t from) const
	{ 
		return self->GetSubstring(from, self->GetSize()); 
	}

	GDINL size_t String::Find(const Char character) const
	{ 
		const Char string[] = { character, '\0' }; 
		return self->Find(&string[0]);
	}

	GDINL size_t String::ReverseFind(const Char character) const
	{ 
		const Char string[] = { character, '\0' }; 
		return self->ReverseFind(&string[0]);
	}

	GDINL String String::Split(const size_t index)
	{
		const String string = self->GetSubstring(0, index);
		*self = self->GetSubstring(index);

		return (string.GetSize() != 0) ? string : *self;
	}

	GDINL void String::PushLast(const Char character)
	{ 
		self->Resize(self->GetSize() + 1); 
		(*self)[self->GetSize() - 1] = character; 
	}

	GDINL void String::PopLast()
	{ 
		GD_ASSERT(self->GetSize() != 0, 
			"'String::PopLast' error: Attempt to Pop last element from zero-sized string");

		self->Resize(self->GetSize() - 1); 
	}

	GDINL const Char& String::operator[] (const size_t index) const
	{ 
		/*GD_ASSERT(index < self->GetSize(), 
			"'String::operator[]' error: Index out of bounds.");*/

		return self->string[index]; 
	}

	GDINL Char& String::operator[] (const size_t index)
	{ 
		/*GD_ASSERT(index < self->GetSize(), 
			"'String::operator[]' error: Index out of bounds.");*/

		return self->string[index]; 
	}

	GDINL String String::operator+ (const Char* string) const
	{ 
		return *self + String(string); 
	}

	GDINL String& String::operator+= (const String& string)
	{ 
		return (*self = (*self + string)); 
	}

	GDINL String& String::operator+= (const Char* string)
	{ 
		return (*self = (*self + String(string))); 
	}

	GDINL bool String::operator==(const HashSumm& hash) const
	{ 
		return self->GetHashSumm() == hash; 
	}

	GDINL bool String::operator!= (const String& string) const
	{ 
		return !(*self == string);
	}

GD_NAMESPACE_END
