#if !defined(GD_CORE_CONTAINERS_STRING)
#	pragma error("Attempting to include 'String.inl' file. Please, use 'String.h' instead")
#endif

GD_NAMESPACE_BEGIN

	GDINL String::String(const Vector<CharAnsi>& vector)
	{
		self->HeapMemory = (Char*)malloc(1 * sizeof(Char));
		self->HeapSize = 1;

		*self = String(&vector[0]);
	}

	GDINL String::String(const Vector<CharUtf16>& vector)
	{
		self->HeapMemory = (Char*)malloc(1 * sizeof(Char));
		self->HeapSize = 1;

		*self = String(&vector[0]);
	}

	GDINL String::String(const Vector<CharUtf32>& vector)
	{
		self->HeapMemory = (Char*)malloc(1 * sizeof(Char));
		self->HeapSize = 1;

		*self = String(&vector[0]);
	}

	GDINL size_t String::GetSize() const
	{ 
		return self->HeapSize; 
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
		const Char HeapMemory[] = { character, '\0' }; 
		return self->Find(&HeapMemory[0]);
	}

	GDINL size_t String::ReverseFind(const Char character) const
	{ 
		const Char HeapMemory[] = { character, '\0' }; 
		return self->ReverseFind(&HeapMemory[0]);
	}

	GDINL String String::Split(const size_t index)
	{
		const String HeapMemory = self->GetSubstring(0, index);
		*self = self->GetSubstring(index);

		return (HeapMemory.GetSize() != 0) ? HeapMemory : *self;
	}

	GDINL void String::PushLast(const Char character)
	{ 
		self->Resize(self->GetSize() + 1); 
		(*self)[self->GetSize() - 1] = character; 
	}

	GDINL void String::PopLast()
	{ 
		GD_ASSERT(self->GetSize() != 0, 
			"'String::PopLast' error: Attempt to Pop last element from zero-sized HeapMemory");

		self->Resize(self->GetSize() - 1); 
	}

	GDINL const Char& String::operator[] (const size_t index) const
	{ 
		/*GD_ASSERT(index < self->GetSize(), 
			"'String::operator[]' error: Index out of bounds.");*/

		return self->HeapMemory[index]; 
	}

	GDINL Char& String::operator[] (const size_t index)
	{ 
		/*GD_ASSERT(index < self->GetSize(), 
			"'String::operator[]' error: Index out of bounds.");*/

		return self->HeapMemory[index]; 
	}

	GDINL String String::operator+ (const Char* HeapMemory) const
	{ 
		return *self + String(HeapMemory); 
	}

	GDINL String& String::operator+= (const String& HeapMemory)
	{ 
		return (*self = (*self + HeapMemory)); 
	}

	GDINL String& String::operator+= (const Char* HeapMemory)
	{ 
		return (*self = (*self + String(HeapMemory))); 
	}

	GDINL bool String::operator==(const HashCode& hash) const
	{ 
		return self->GetHashCode() == hash; 
	}

	GDINL bool String::operator!= (const String& HeapMemory) const
	{ 
		return !(*self == HeapMemory);
	}

GD_NAMESPACE_END
