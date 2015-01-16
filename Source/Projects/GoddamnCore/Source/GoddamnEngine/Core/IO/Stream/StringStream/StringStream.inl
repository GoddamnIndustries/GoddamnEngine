/// ==========================================================================================
/// StringStream.inl - String stream implementation.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// 
/// @author James Jhuighuy
/// ==========================================================================================

GD_NAMESPACE_BEGIN

	/// ==========================================================================================
	/// BaseStringInputStream class.
	/// ==========================================================================================

	/// ------------------------------------------------------------------------------------------
	/// Constructors/Destructor.
	/// ------------------------------------------------------------------------------------------

	template<typename CharacterType>
	GDINL BaseStringInputStream<CharacterType>::BaseStringInputStream(CharacterType const* SomeMemory, size_t const SomeMemoryLength)
		: DataReference(reinterpret_cast<UInt8 const*>(SomeMemory))
		, DataReferenceLength(sizeof(CharacterType)*  SomeMemoryLength)
	{
		GD_DEBUG_ASSERT(this->DataReference != nullptr, "Nullptr data reference.");
	}

	template<typename CharacterType>
	GDINL BaseStringInputStream<CharacterType>::BaseStringInputStream(BaseString<CharacterType> const& SomeString)
		: BaseStringInputStream(reinterpret_cast<CharacterType const*>(SomeString.CStr())
		, sizeof(CharacterType) * SomeString.GetLength())
	{
	}

	template<typename CharacterType>
	GDINL BaseStringInputStream<CharacterType>::BaseStringInputStream(BaseStringBuilder<CharacterType> const& SomeStringBuilder)
		: BaseStringInputStream(reinterpret_cast<CharacterType const*>(SomeStringBuilder.CStr())
		, sizeof(CharacterType) * SomeStringBuilder.GetLength())
	{
	}

	template<typename CharacterType>
	GDINL BaseStringInputStream<CharacterType>::BaseStringInputStream(Vector<CharacterType> const& SomeVector)
		: BaseStringInputStream(reinterpret_cast<CharacterType const*>(&SomeVector.GetFirstElement())
		, sizeof(CharacterType) * SomeVector.GetLength())
	{
	}

	template<typename CharacterType>
	GDINL BaseStringInputStream<CharacterType>::~BaseStringInputStream()
	{
		this->Close();
	}

	/// ------------------------------------------------------------------------------------------
	/// Class API.
	/// ------------------------------------------------------------------------------------------

	template<typename CharacterType>
	GDINL size_t BaseStringInputStream<CharacterType>::GetPosition() const
	{
		GD_DEBUG_ASSERT(this->DataReference != nullptr, "Nullptr data reference.");
		return this->DataReferencePosition;
	}

	template<typename CharacterType>
	GDINL size_t BaseStringInputStream<CharacterType>::GetLength() const
	{
		GD_DEBUG_ASSERT(this->DataReference != nullptr, "Nullptr data reference.");
		return this->DataReferenceLength;
	}

	template<typename CharacterType>
	GDINL void BaseStringInputStream<CharacterType>::Close()
	{
		GD_DEBUG_ASSERT(this->DataReference != nullptr, "Nullptr data reference.");
		this->DataReference = nullptr;
		this->DataReferenceLength = SIZE_MAX;
		this->DataReferencePosition = SIZE_MAX;
	}

	template<typename CharacterType>
	GDINL void BaseStringInputStream<CharacterType>::Seek(ptrdiff_t const Offset, SeekOrigin const Origin /* = SeekOrigin::Current */)
	{
		GD_DEBUG_ASSERT(this->DataReference != nullptr, "Nullptr data reference.");
		switch (Origin) {
			case SeekOrigin::Begin: {
				this->DataReferencePosition = Offset;
			} break;
			case SeekOrigin::Current: {
				this->DataReferencePosition += Offset;
			} break;
			case SeekOrigin::End: {
				this->DataReferencePosition = this->DataReferenceLength + Offset;
			} break;
		}

		if (this->DataReferencePosition >= this->DataReferenceLength) {
			throw IOException("Failed to seek inside data reference (position is out of bounds).");
		}
	}

	template<typename CharacterType>
	GDINL UInt8 BaseStringInputStream<CharacterType>::Read()
	{
		GD_DEBUG_ASSERT(this->DataReference != nullptr, "Nullptr data reference.");
		if (this->DataReferencePosition >= this->DataReferenceLength) {
			throw IOException("Failed to read from data reference (position is out of bounds).");
		}

		UInt8 Result = (*(this->DataReference + this->DataReferencePosition));
		this->DataReferencePosition += 1;
		return Result;
	}

	template<typename CharacterType>
	GDINL void BaseStringInputStream<CharacterType>::Read(handle const Array, size_t const Count, size_t const Length)
	{
		GD_DEBUG_ASSERT(this->DataReference != nullptr, "Nullptr data reference.");
		if (this->DataReferencePosition + (Count * Length) > this->DataReferenceLength) {
			throw IOException("Failed to read from data reference (position is out of bounds).");
		}

		std::memcpy(Array, this->DataReference, Count * Length);
		this->DataReferencePosition += (Count * Length);
	}

	/// ==========================================================================================
	/// BaseStringOutputStream class.
	/// ==========================================================================================

	/// ------------------------------------------------------------------------------------------
	/// Construtor/Destructor.
	/// ------------------------------------------------------------------------------------------

	template<typename CharacterType>
	GDINL BaseStringOutputStream<CharacterType>::BaseStringOutputStream(BaseStringBuilder<CharacterType>& Builder)
		: Builder(&Builder)
	{
	}

	template<typename CharacterType>
	GDINL BaseStringOutputStream<CharacterType>::~BaseStringOutputStream()
	{ 
		this->Close(); 
	}

	/// ------------------------------------------------------------------------------------------
	/// Class API:
	/// ------------------------------------------------------------------------------------------

	template<typename CharacterType>
	GDINL size_t BaseStringOutputStream<CharacterType>::GetPosition() const
	{	// Since we anly writing, our position is equal to decremented size.
		return (this->GetLength() - 1);
	}

	template<typename CharacterType>
	GDINL size_t BaseStringOutputStream<CharacterType>::GetLength() const
	{
		return this->Builder->GetLength();
	}

	template<typename CharacterType>
	GDINL void BaseStringOutputStream<CharacterType>::Close()
	{
		this->Builder = nullptr;
	}

	template<typename CharacterType>
	GDINL void BaseStringOutputStream<CharacterType>::Flush()
	{	// Enerything is already flushed.
	}

	template<typename CharacterType>
	GDINL void BaseStringOutputStream<CharacterType>::Write(UInt8 const Byte)
	{
		static_assert(sizeof(Byte) == sizeof(CharacterType), "'BaseStringOutputStream<T>::Write' error: T should size of byte");
		this->Builder->Append(*reinterpret_cast<CharacterType const*>(&Byte));
	}

	template<typename CharacterType>
	GDINL void BaseStringOutputStream<CharacterType>::Write(chandle const Array, size_t const Count, size_t const Length)
	{
		GD_DEBUG_ASSERT((Length % sizeof(CharacterType)) == 0, "Length of single element is not multiple of character type.");
		this->Builder->Append(reinterpret_cast<CharacterType const*>(Array), Count * Length);
	}

GD_NAMESPACE_END
