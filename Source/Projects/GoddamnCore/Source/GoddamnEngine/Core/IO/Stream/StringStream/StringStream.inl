//! ==========================================================================================
//! StringStream.inl - String stream implementation.
//! Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
//! 
//! @author James Jhuighuy
//! ==========================================================================================

GD_NAMESPACE_BEGIN

	//! ==========================================================================================
	//! BaseStringInputStream class.
	//! ==========================================================================================

	//! ------------------------------------------------------------------------------------------
	//! Constructors/Destructor.
	//! ------------------------------------------------------------------------------------------

	template<typename CharacterType>
	GDINL BaseStringInputStream<CharacterType>::BaseStringInputStream(CharacterType const* SomeMemory, SizeTp const SomeMemoryLength)
		: DataReference(reinterpret_cast<UInt8 const*>(SomeMemory))
		, DataReferenceLength(sizeof(CharacterType)*  SomeMemoryLength)
	{
		GD_DEBUG_ASSERT(DataReference != nullptr, "Nullptr data reference.");
	}

	/*template<typename CharacterType>
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
	}*/

	template<typename CharacterType>
	GDINL BaseStringInputStream<CharacterType>::~BaseStringInputStream()
	{
		Close();
	}

	//! ------------------------------------------------------------------------------------------
	//! Class API.
	//! ------------------------------------------------------------------------------------------

	template<typename CharacterType>
	GDINL SizeTp BaseStringInputStream<CharacterType>::GetPosition() const
	{
		GD_DEBUG_ASSERT(DataReference != nullptr, "Nullptr data reference.");
		return DataReferencePosition;
	}

	template<typename CharacterType>
	GDINL SizeTp BaseStringInputStream<CharacterType>::GetLength() const
	{
		GD_DEBUG_ASSERT(DataReference != nullptr, "Nullptr data reference.");
		return DataReferenceLength;
	}

	template<typename CharacterType>
	GDINL void BaseStringInputStream<CharacterType>::Close()
	{
		GD_DEBUG_ASSERT(DataReference != nullptr, "Nullptr data reference.");
		DataReference = nullptr;
		DataReferenceLength = SizeTpMax;
		DataReferencePosition = SizeTpMax;
	}

	template<typename CharacterType>
	GDINL void BaseStringInputStream<CharacterType>::Seek(ptrdiff_t const offset, SeekOrigin const origin /* = SeekOrigin::Current */)
	{
		GD_DEBUG_ASSERT(DataReference != nullptr, "Nullptr data reference.");
		switch (origin) {
			case SeekOrigin::Begin: {
				DataReferencePosition = offset;
			} break;
			case SeekOrigin::Current: {
				DataReferencePosition += offset;
			} break;
			case SeekOrigin::End: {
				DataReferencePosition = DataReferenceLength + offset;
			} break;
		}

		if (DataReferencePosition >= DataReferenceLength) {
			throw IOException("Failed to seek inside data reference (position is out of bounds).");
		}
	}

	template<typename CharacterType>
	GDINL UInt8 BaseStringInputStream<CharacterType>::Read()
	{
		GD_DEBUG_ASSERT(DataReference != nullptr, "Nullptr data reference.");
		if (DataReferencePosition >= DataReferenceLength) {
			throw IOException("Failed to read from data reference (position is out of bounds).");
		}

		UInt8 Result = (*(DataReference + DataReferencePosition));
		DataReferencePosition += 1;
		return Result;
	}

	template<typename CharacterType>
	GDINL void BaseStringInputStream<CharacterType>::Read(Handle const Array, SizeTp const count, SizeTp const length)
	{
		GD_DEBUG_ASSERT(DataReference != nullptr, "Nullptr data reference.");
		if (DataReferencePosition + (count * length) > DataReferenceLength) {
			throw IOException("Failed to read from data reference (position is out of bounds).");
		}

		std::memcpy(Array, DataReference, count * length);
		DataReferencePosition += (count * length);
	}

	//! ==========================================================================================
	//! BaseStringOutputStream class.
	//! ==========================================================================================

	//! ------------------------------------------------------------------------------------------
	//! Construtor/Destructor.
	//! ------------------------------------------------------------------------------------------

	template<typename CharacterType>
	GDINL BaseStringOutputStream<CharacterType>::BaseStringOutputStream(BaseStringBuilder<CharacterType>& Builder)
		: Builder(&Builder)
	{
	}

	template<typename CharacterType>
	GDINL BaseStringOutputStream<CharacterType>::~BaseStringOutputStream()
	{ 
		Close(); 
	}

	//! ------------------------------------------------------------------------------------------
	//! Class API:
	//! ------------------------------------------------------------------------------------------

	template<typename CharacterType>
	GDINL SizeTp BaseStringOutputStream<CharacterType>::GetPosition() const
	{	// Since we anly writing, our position is equal to decremented size.
		return (GetLength() - 1);
	}

	template<typename CharacterType>
	GDINL SizeTp BaseStringOutputStream<CharacterType>::GetLength() const
	{
		return Builder->GetLength();
	}

	template<typename CharacterType>
	GDINL void BaseStringOutputStream<CharacterType>::Close()
	{
		Builder = nullptr;
	}

	template<typename CharacterType>
	GDINL void BaseStringOutputStream<CharacterType>::Flush()
	{	// Enerything is already flushed.
	}

	template<typename CharacterType>
	GDINL void BaseStringOutputStream<CharacterType>::Write(UInt8 const Byte)
	{
		static_assert(sizeof(Byte) == sizeof(CharacterType), "'BaseStringOutputStream<T>::Write' error: T should size of byte");
		Builder->Append(*reinterpret_cast<CharacterType const*>(&Byte));
	}

	template<typename CharacterType>
	GDINL void BaseStringOutputStream<CharacterType>::Write(CHandle const Array, SizeTp const count, SizeTp const length)
	{
		GD_DEBUG_ASSERT((length % sizeof(CharacterType)) == 0, "m_Length of single element is not multiple of character type.");
		Builder->Append(reinterpret_cast<CharacterType const*>(Array), count * length);
	}

GD_NAMESPACE_END
