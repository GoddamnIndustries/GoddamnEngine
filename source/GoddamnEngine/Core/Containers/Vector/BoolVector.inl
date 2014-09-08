/// ==========================================================================================
/// BoolVector.inl - Dynamic bitset implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 10.05.2014 - Created by James Jhuighuy
/// ==========================================================================================

#if (!defined(GD_CORE_CONTAINERS_BOOL_VECTOR))
#	error "Attempting to include 'BoolVector.inl' file. Please, use 'BoolVector.h' instead.""
#endif	// if (!defined(GD_CORE_CONTAINERS_VECTOR))

#include <GoddamnEngine/Core/Diagnostics/Assertion/Assertion.h>
#include <GoddamnEngine/Core/Utility.h>

GD_NAMESPACE_BEGIN

	/// ==========================================================================================
	/// Vector<bool> class
	/// ==========================================================================================	

#define GD_BVECTOR_TEMPLATE() template<typename MemoryProviderType /*= VectorMemoryProviders::HeapMemoryProvider<bool>*/>
#define GD_BVECTOR_CLASS()    Vector<bool, MemoryProviderType>

	GD_BVECTOR_TEMPLATE()
	GDINL size_t GD_BVECTOR_CLASS()::GetSize() const
	{
		return (this->ByteStorage.GetSize() * GD_BITS_PER_BYTE) + this->BitIndex + 1;
	}

	GD_BVECTOR_TEMPLATE()
	GDINL size_t GD_BVECTOR_CLASS()::GetCapacity() const
	{
		return (this->ByteStorage.GetCapacity() * GD_BITS_PER_BYTE);
	}

	GD_BVECTOR_TEMPLATE()
	GDINL void GD_BVECTOR_CLASS()::Resize(size_t const NewBitsCount, bool const InitialBit /* = bool() */)
	{
		this->ByteStorage.Resize(NewBitsCount / GD_BITS_PER_BYTE);
		this->BitIndex        = (NewBitsCount % GD_BITS_PER_BYTE);
	}

	GD_BVECTOR_TEMPLATE()
	GDINL void GD_BVECTOR_CLASS()::Reserve(size_t const NewCapacity)
	{
		this->ByteStorage.Reserve(NewCapacity / GD_BITS_PER_BYTE);
	}

	GD_BVECTOR_TEMPLATE()
	GDINL void GD_BVECTOR_CLASS()::Emptify()
	{
		this->Resize(0);
	}

	GD_BVECTOR_TEMPLATE()
	GDINL void GD_BVECTOR_CLASS()::Clear()
	{
		this->Reserve(0);
	}

	GD_BVECTOR_TEMPLATE()
	GDINL bool GD_BVECTOR_CLASS()::GetElementAt(size_t const Index) const
	{
		GD_ASSERT((Index < this->GetSize()), "Index is out of bounds");
		return (((this->ByteStorage[Index / GD_BITS_PER_BYTE]) & GD_BIT(this->BitIndex)) != 0);
	}

	GD_BVECTOR_TEMPLATE()
	GDINL bool GD_BVECTOR_CLASS()::GetFirstElement() const
	{
		return this->GetElementAt(0);
	}

	GD_BVECTOR_TEMPLATE()
	GDINL bool GD_BVECTOR_CLASS()::GetLastElement() const
	{
		return this->GetElementAt(this->GetSize() - 1);
	}

	GD_BVECTOR_TEMPLATE()
	GDINL void SetElementAt(size_t const Index, bool const Bit)
	{
		GD_ASSERT((Index < this->GetSize()), "Index is out of bounds");
		this->ByteStorage[Index / GD_BITS_PER_BYTE] |= (Bit ? GD_BIT(this->BitIndex) : 0);
	}

	GD_BVECTOR_TEMPLATE()
	GDINL void GD_BVECTOR_CLASS()::InsertElementAt(size_t const Index, bool const Bit)
	{
		GD_NOT_IMPLEMENTED();
	}

	GD_BVECTOR_TEMPLATE()
	GDINL void GD_BVECTOR_CLASS()::RemoveElementAt(size_t const Index)
	{
		GD_NOT_IMPLEMENTED();
	}

	GD_BVECTOR_TEMPLATE()
	GDINL void GD_BVECTOR_CLASS()::PushLast(bool NewBit /* = bool() */)
	{
		if ((++this->BitIndex) == GD_BITS_PER_BYTE)
		{	// Here comes bytes border.
			this->ByteStorage.PushLast();
			this->BitIndex = 0;
		}

		this->ByteStorage[Index / GD_BITS_PER_BYTE] |= (NewBit ? GD_BIT(this->BitIndex) : 0);
	}

	GD_BVECTOR_TEMPLATE()
	GDINL void GD_BVECTOR_CLASS()::PopLast()
	{
		if ((--this->BitIndex) == SIZE_MAX)
		{	// Here comes bytes border.
			this->ByteStorage.PopLast()
			this->BitIndex = 0;
		}
	}

#undef GD_BVECTOR_TEMPLATE
#undef GD_BVECTOR_CLASS

GD_NAMESPACE_END
