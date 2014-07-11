//////////////////////////////////////////////////////////////////////////
/// StringStream.inl - String stream implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 18.06.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

GD_NAMESPACE_BEGIN

	/// ==========================================================================================
	/// BaseStringInputStream class.
	/// Specifies read-only stream that provides reading from reference on data.
	/// ==========================================================================================

	/// ------------------------------------------------------------------------------------------
	/// Class implementation API:
	/// ------------------------------------------------------------------------------------------

	/// Initializes StringInputStream using pointer to some memory.
	/// @param SomeMemory       Pointer to input stream initial memory.
	/// @param SomeMemoryLength Length of specified data memory length.
	template<typename CharacterType>
	GDINL BaseStringInputStream<CharacterType>::BaseStringInputStream(CharacterType const* SomeMemory, size_t const SomeMemoryLength)
		: DataReference(reinterpret_cast<UInt8 const*>(SomeMemory))
		, DataReferenceLength(sizeof(CharacterType)*  SomeMemoryLength)
	{
		GD_DEBUG_ASSERT(self->DataReference != nullptr, "Nullptr data reference.");
	}

	/// Initializes StringInputStream using some String class instance.
	/// @param SomeString InputStream data.
	template<typename CharacterType>
	GDINL BaseStringInputStream<CharacterType>::BaseStringInputStream(BaseString<CharacterType> const& SomeString)
		: BaseStringInputStream(reinterpret_cast<UInt8 const*>(SomeString.CStr())
		, sizeof(CharacterType) * SomeString.GetSize())
	{
	}

	/// Initializes StringInputStream using some StringBuilder class instance.
	/// @param SomeStringBuilder InputStream data.
	template<typename CharacterType>
	GDINL BaseStringInputStream<CharacterType>::BaseStringInputStream(BaseStringBuilder<CharacterType> const& SomeStringBuilder)
		: BaseStringInputStream(reinterpret_cast<UInt8 const*>(SomeStringBuilder.GetPointer())
		, sizeof(CharacterType) * SomeStringBuilder.GetSize())
	{
	}

	/// Initializes StringInputStream using some Vector of characters class instance.
	/// @param SomeVector InputStream data.
	template<typename CharacterType>
	GDINL BaseStringInputStream<CharacterType>::BaseStringInputStream(Vector<CharacterType> const& SomeVector)
		: BaseStringInputStream(reinterpret_cast<UInt8 const*>(&SomeVector.GetFirstElement())
		, sizeof(CharacterType) * SomeVector.GetSize())
	{
	}

	template<typename CharacterType>
	GDINL BaseStringInputStream<CharacterType>::~BaseStringInputStream()
	{
		self->Close();
	}

	/// @see BaseStream::GetPosition()
	template<typename CharacterType>
	GDINL size_t BaseStringInputStream<CharacterType>::GetPosition() const
	{
		GD_DEBUG_ASSERT(self->DataReference != nullptr, "Nullptr data reference.");
		return self->DataReferencePosition;
	}

	/// @see BaseStream::GetSize()
	template<typename CharacterType>
	GDINL size_t BaseStringInputStream<CharacterType>::GetSize() const
	{
		GD_DEBUG_ASSERT(self->DataReference != nullptr, "Nullptr data reference.");
		return self->DataReferenceLength;
	}

	/// @see InputStream::Close()
	template<typename CharacterType>
	GDINL void BaseStringInputStream<CharacterType>::Close()
	{
		GD_DEBUG_ASSERT(self->DataReference != nullptr, "Nullptr data reference.");
		self->DataReference = nullptr;
		self->DataReferenceLength = SIZE_MAX;
		self->DataReferencePosition = SIZE_MAX;
	}

	/// @see InputStream::Seek(ptrdiff_t const Offset, SeekOrigin const Origin = SeekOrigin::Current)
	template<typename CharacterType>
	GDINL void BaseStringInputStream<CharacterType>::Seek(ptrdiff_t const Offset, SeekOrigin const Origin /* = SeekOrigin::Current */)
	{
		GD_DEBUG_ASSERT(self->DataReference != nullptr, "Nullptr data reference.");
		switch (Origin)
		{
		case SeekOrigin::Begin:
			self->DataReferencePosition = Offset;
			break;
		case SeekOrigin::Current:
			self->DataReferencePosition += Offset;
			break;
		case SeekOrigin::End:
			self->DataReferencePosition = self->DataReferenceLength + Offset;
			break;
		}

		if (self->DataReferencePosition >= self->DataReferenceLength)
			throw IOException("Failed to seek inside data reference (position is out of bounds).");
	}

	/// @see InputStream::Read()
	template<typename CharacterType>
	GDINL UInt8 BaseStringInputStream<CharacterType>::Read()
	{
		GD_DEBUG_ASSERT(self->DataReference != nullptr, "Nullptr data reference.");
		if (self->DataReferencePosition >= self->DataReferenceLength)
			throw IOException("Failed to read from data reference (position is out of bounds).");

		UInt8 Result = (*self->DataReference);
		++self->DataReferencePosition;
	}

	/// @see InputStream::Read(UInt8* const Array, size_t const Count, size_t const Length)
	template<typename CharacterType>
	GDINL void BaseStringInputStream<CharacterType>::Read(UInt8* const Array, size_t const Count, size_t const Length)
	{
		GD_DEBUG_ASSERT(self->DataReference != nullptr, "Nullptr data reference.");
		if (self->DataReferencePosition + (Count * Length) >= self->DataReferenceLength)
			throw IOException("Failed to read from data reference (position is out of bounds).");

		::memcpy(Array, self->DataReference, Count * Length);
		self->DataReferencePosition += (Count * Length);
	}

	/// ==========================================================================================
	/// BaseStringOutputStream class.
	/// Specifies write-only stream that provides writing to reference on data (BaseStringBuilder).
	/// ==========================================================================================

	/// ------------------------------------------------------------------------------------------
	/// Class implementation API:
	/// ------------------------------------------------------------------------------------------

	/// Initializes new string output stream that points to some string builder.
	/// @param Builder Builder in which output would be stored.
	template<typename CharacterType>
	GDINL BaseStringOutputStream<CharacterType>::BaseStringOutputStream(BaseStringBuilder<CharacterType>& Builder)
		: Builder(&Builder)
	{
	}

	template<typename CharacterType>
	GDINL BaseStringOutputStream<CharacterType>::~BaseStringOutputStream()
	{ 
		self->Close(); 
	}

	/// @see BaseStream::GetPosition()
	template<typename CharacterType>
	GDINL size_t BaseStringOutputStream<CharacterType>::GetPosition() const
	{	// Since we anly writing, our position is equal to decremented size.
		return (self->GetSize() - 1);
	}

	/// @see BaseStream::GetSize()
	template<typename CharacterType>
	GDINL size_t BaseStringOutputStream<CharacterType>::GetSize() const
	{
		return self->Builder->GetSize();
	}

	/// @see OutputStream::Close()
	template<typename CharacterType>
	GDINL void BaseStringOutputStream<CharacterType>::Close()
	{
		self->Builder = nullptr;
	}

	/// @see OutputStream::Flush()
	template<typename CharacterType>
	GDINL void BaseStringOutputStream<CharacterType>::Flush()
	{	// Enerything is already flushed.
	}

	/// @see OutputStream::Write(UInt8 const Byte)
	template<typename CharacterType>
	GDINL void BaseStringOutputStream<CharacterType>::Write(UInt8 const Byte)
	{
		static_assert(sizeof(Byte) == sizeof(CharacterType), "'BaseStringOutputStream<T>::Write' error: T should size of byte");
		self->Builder->Append(*reinterpret_cast<CharacterType const*>(&Byte));
	}

	/// @see OutputStream::Write(UInt8 const* const Array, size_t const Count, size_t const Length)
	template<typename CharacterType>
	GDINL void BaseStringOutputStream<CharacterType>::Write(UInt8 const* const Array, size_t const Count, size_t const Length)
	{
		GD_DEBUG_ASSERT((Length % sizeof(CharacterType)) == 0, "Length of single element is not multiple of character type.");
		self->Builder->Append(reinterpret_cast<CharacterType const*>(Array), Count * Length);
	}

GD_NAMESPACE_END
