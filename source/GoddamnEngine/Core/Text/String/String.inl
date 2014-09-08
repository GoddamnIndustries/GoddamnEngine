#if !defined(GD_CORE_CONTAINERS_STRING)
#	pragma error("Attempting to include 'String.inl' file. Please, use 'String.h' instead")
#endif

GD_NAMESPACE_BEGIN

	GDINL String::String(const Vector<CharAnsi>& vector)
	{
		this->HeapMemory = (Char*)malloc(1 * sizeof(Char));
		this->HeapSize = 1;

		*this = String(&vector[0]);
	}

	GDINL String::String(const Vector<CharUtf16>& vector)
	{
		this->HeapMemory = (Char*)malloc(1 * sizeof(Char));
		this->HeapSize = 1;

		*this = String(&vector[0]);
	}

	GDINL String::String(const Vector<CharUtf32>& vector)
	{
		this->HeapMemory = (Char*)malloc(1 * sizeof(Char));
		this->HeapSize = 1;

		*this = String(&vector[0]);
	}

	GDINL size_t String::GetSize() const
	{ 
		return this->HeapSize; 
	}

	GDINL String String::Trimm() const
	{ 
		return this->TrimmLeft().TrimmRight(); 
	}

	GDINL String String::GetSubstring(const size_t from) const
	{ 
		return this->GetSubstring(from, this->GetSize()); 
	}

	GDINL size_t String::Find(const Char character) const
	{ 
		const Char HeapMemory[] = { character, '\0' }; 
		return this->Find(&HeapMemory[0]);
	}

	GDINL size_t String::ReverseFind(const Char character) const
	{ 
		const Char HeapMemory[] = { character, '\0' }; 
		return this->ReverseFind(&HeapMemory[0]);
	}

	GDINL String String::Split(const size_t index)
	{
		const String HeapMemory = this->GetSubstring(0, index);
		*this = this->GetSubstring(index);

		return (HeapMemory.GetSize() != 0) ? HeapMemory : *this;
	}

	GDINL void String::PushLast(const Char character)
	{ 
		this->Resize(this->GetSize() + 1); 
		(*this)[this->GetSize() - 1] = character; 
	}

	GDINL void String::PopLast()
	{ 
		GD_ASSERT(this->GetSize() != 0, 
			"'String::PopLast' error: Attempt to Pop last element from zero-sized HeapMemory");

		this->Resize(this->GetSize() - 1); 
	}

	GDINL const Char& String::operator[] (const size_t index) const
	{ 
		/*GD_ASSERT(index < this->GetSize(), 
			"'String::operator[]' error: Index out of bounds.");*/

		return this->HeapMemory[index]; 
	}

	GDINL Char& String::operator[] (const size_t index)
	{ 
		/*GD_ASSERT(index < this->GetSize(), 
			"'String::operator[]' error: Index out of bounds.");*/

		return this->HeapMemory[index]; 
	}

	GDINL String String::operator+ (const Char* HeapMemory) const
	{ 
		return *this + String(HeapMemory); 
	}

	GDINL String& String::operator+= (const String& HeapMemory)
	{ 
		return (*this = (*this + HeapMemory)); 
	}

	GDINL String& String::operator+= (const Char* HeapMemory)
	{ 
		return (*this = (*this + String(HeapMemory))); 
	}

	GDINL bool String::operator==(const HashCode& hash) const
	{ 
		return this->GetHashCode() == hash; 
	}

	GDINL bool String::operator!= (const String& HeapMemory) const
	{ 
		return !(*this == HeapMemory);
	}

GD_NAMESPACE_END
