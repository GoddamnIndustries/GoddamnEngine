/// ==========================================================================================
/// BoolVector.inl - Dynamic bitset implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 10.05.2014 - Created by James Jhuighuy
/// ==========================================================================================

#if (!defined(GD_CORE_CONTAINERS_BOOL_VECTOR))
#	error "Attempting to include 'BoolVector.inl' file. Please, use 'BoolVector.hh' instead.""
#endif	// if (!defined(GD_CORE_CONTAINERS_VECTOR))

#include <GoddamnEngine/Core/Diagnostics/Assertion/Assertion.hh>
#include <GoddamnEngine/Core/Utility.hh>

GD_NAMESPACE_BEGIN

	/// ==========================================================================================
	/// Vector<bool> class
	/// ==========================================================================================	

#define GD_BVECTOR_TEMPLATE() template<typename MemoryProviderType /*= VectorMemoryProviders::HeapMemoryProvider<bool>*/>
#define GD_BVECTOR_CLASS()    Vector<bool, MemoryProviderType>

	GD_BVECTOR_TEMPLATE()
	GDINL size_t GD_BVECTOR_CLASS()::GetSize() const
	{
		return (self->ByteStorage.GetSize() * GD_BITS_PER_BYTE) + self->BitIndex + 1;
	}

	GD_BVECTOR_TEMPLATE()
	GDINL size_t GD_BVECTOR_CLASS()::GetCapacity() const
	{
		return (self->ByteStorage.GetCapacity() * GD_BITS_PER_BYTE);
	}

	GD_BVECTOR_TEMPLATE()
	GDINL void GD_BVECTOR_CLASS()::Resize(size_t const NewBitsCount, bool const InitialBit /* = bool() */)
	{
		self->ByteStorage.Resize(NewBitsCount / GD_BITS_PER_BYTE);
		self->BitIndex        = (NewBitsCount % GD_BITS_PER_BYTE);
	}

	GD_BVECTOR_TEMPLATE()
	GDINL void GD_BVECTOR_CLASS()::Reserve(size_t const NewCapacity)
	{
		self->ByteStorage.Reserve(NewCapacity / GD_BITS_PER_BYTE);
	}

	GD_BVECTOR_TEMPLATE()
	GDINL void GD_BVECTOR_CLASS()::Emptify()
	{
		self->Resize(0);
	}

	GD_BVECTOR_TEMPLATE()
	GDINL void GD_BVECTOR_CLASS()::Clear()
	{
		self->Reserve(0);
	}

	GD_BVECTOR_TEMPLATE()
	GDINL bool GD_BVECTOR_CLASS()::GetElementAt(size_t const Index) const
	{
		GD_ASSERT((Index < self->GetSize()), "Index is out of bounds");
		return (((self->ByteStorage[Index / GD_BITS_PER_BYTE]) & GD_BIT(self->BitIndex)) != 0);
	}

	GD_BVECTOR_TEMPLATE()
	GDINL bool GD_BVECTOR_CLASS()::GetFirstElement() const
	{
		return self->GetElementAt(0);
	}

	GD_BVECTOR_TEMPLATE()
	GDINL bool GD_BVECTOR_CLASS()::GetLastElement() const
	{
		return self->GetElementAt(self->GetSize() - 1);
	}

	GD_BVECTOR_TEMPLATE()
	GDINL void SetElementAt(size_t const Index, bool const Bit)
	{
		GD_ASSERT((Index < self->GetSize()), "Index is out of bounds");
		self->ByteStorage[Index / GD_BITS_PER_BYTE] |= (Bit ? GD_BIT(self->BitIndex) : 0);
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
		if ((++self->BitIndex) == GD_BITS_PER_BYTE)
		{	// Here comes bytes border.
			self->ByteStorage.PushLast();
			self->BitIndex = 0;
		}

		self->ByteStorage[Index / GD_BITS_PER_BYTE] |= (NewBit ? GD_BIT(self->BitIndex) : 0);
	}

	GD_BVECTOR_TEMPLATE()
	GDINL void GD_BVECTOR_CLASS()::PopLast()
	{
		if ((--self->BitIndex) == SIZE_MAX)
		{	// Here comes bytes border.
			self->ByteStorage.PopLast()
			self->BitIndex = 0;
		}
	}

#undef GD_BVECTOR_TEMPLATE
#undef GD_BVECTOR_CLASS

GD_NAMESPACE_END
