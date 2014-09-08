/// ==========================================================================================
/// Vector.inl - Dynamically sized array implementation
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.11.2013 - Created by James Jhuighuy
///		* 24.04.2014 - Rewritten from scratch by James Jhuighuy
/// ==========================================================================================

#if (!defined(GD_CORE_CONTAINERS_VECTOR))
#	error "Attempting to include 'Vector.inl' file. Please, use 'Vector.h' instead.""
#endif	// if (!defined(GD_CORE_CONTAINERS_VECTOR))

#include <GoddamnEngine/Core/Diagnostics/Assertion/Assertion.h>
#include <GoddamnEngine/Core/Math/FastMath/FastMath.h>
#include <GoddamnEngine/Core/Utility.h>
#include <new>			// For placement new

GD_NAMESPACE_BEGIN

	/// ==========================================================================================
	/// StackMemoryProvider class
	/// ==========================================================================================

	// To prevent a lot of copy-pastes in code.
#define GD_STACK_MEMORY_PROVIDER_TEMPLATE() template<typename ElementType, size_t const _Capacity>
#define GD_STACK_MEMORY_PROVIDER_CLASS()    VectorMemoryProviders::StackMemoryProvider<ElementType, _Capacity>

	/// @todo Move StackMemoryProvider implementation here

#undef GD_STACK_MEMORY_PROVIDER_TEMPLATE
#undef GD_STACK_MEMORY_PROVIDER_CLASS

	/// ==========================================================================================
	/// HeapMemoryProvider class
	/// ==========================================================================================

	// To prevent a lot of copy-pastes in code.
#define GD_HEAP_MEMORY_PROVIDER_TEMPLATE() template<typename ElementType>
#define GD_HEAP_MEMORY_PROVIDER_CLASS()    VectorMemoryProviders::HeapMemoryProvider<ElementType>

	/// @todo Move HeapMemoryProvider implementation here

#undef GD_HEAP_MEMORY_PROVIDER_TEMPLATE
#undef GD_HEAP_MEMORY_PROVIDER_CLASS

	/// ==========================================================================================
	/// Vector class
	/// ==========================================================================================

	// To prevent a lot of copy-pastes in code.
#define GD_VECTOR_TEMPLATE() template<typename ElementType, typename MemoryProviderType /*= VectorMemoryProviders::HeapMemoryProvider<ElementType>*/>
#define GD_VECTOR_CLASS()	 Vector<ElementType, MemoryProviderType>

	GD_VECTOR_TEMPLATE()
	inline GD_VECTOR_CLASS()::Vector(size_t const InitialElemntsCount /* = 0 */, size_t const Capacity /* = SIZE_MAX */)
		: MemoryProviderInstance((Capacity != SIZE_MAX) ? Capacity : InitialElemntsCount) 
	{
		this->Count = InitialElemntsCount;
		for (MutableIterator Iterator = this->Begin(); Iterator != this->End(); Iterator += 1) {
			new (&*Iterator) ElementType(/*InitialElement*/);
		}
	}

	GD_VECTOR_TEMPLATE()
	inline GD_VECTOR_CLASS()::Vector(ConstIterator const StartIterator, ConstIterator const EndIterator)
		: MemoryProviderInstance(static_cast<size_t>(EndIterator - StartIterator))
	{
		GD_DEBUG_ASSERT((EndIterator >= StartIterator), "Invalid iterators specified.");
		this->Count = static_cast<size_t>(EndIterator - StartIterator);
		for (ConstIterator Iterator = StartIterator; Iterator != EndIterator; Iterator += 1) {
			new (&*this->Begin() + (Iterator - StartIterator)) ElementType(*Iterator);
		}
	}

	GD_VECTOR_TEMPLATE()
	inline GD_VECTOR_CLASS()::Vector(std::initializer_list<ElementType> const& InitializerList)
		: MemoryProviderInstance(InitializerList.size())
	{
		this->Count = InitializerList.size();
		for (typename std::initializer_list<ElementType>::const_iterator Iterator = InitializerList.begin(); Iterator != InitializerList.end(); Iterator += 1) {
			new (&*this->Begin() + (Iterator - InitializerList.begin())) ElementType(*Iterator);
		}
	}

#if (defined(__cplusplus_cli))
	GD_VECTOR_TEMPLATE()
	inline GD_VECTOR_CLASS()::Vector(array<ElementType>^ CliArray)
		: MemoryProviderInstance(static_cast<size_t>(CliArray->Length))
	{
		this->Count = static_cast<size_t>(CliArray->Length);
		for (ConstIterator Iterator = StartIterator; Iterator != EndIterator; Iterator += 1) {
			new (&*this->Begin() + (Iterator - OtherVector.Begin()) ElementType(CliArray[Iterator - OtherVector.Begin()]);
		}
	}
#endif	// if (defined(__cplusplus_cli))

	GD_VECTOR_TEMPLATE()
	inline GD_VECTOR_CLASS()::Vector(Vector const& OtherVector)
		: MemoryProviderInstance(OtherVector.GetCapacity())
	{
		this->Count = OtherVector.Count;
		for (ConstIterator Iterator = OtherVector.Begin(); Iterator != OtherVector.End(); Iterator += 1) {
			new (&*this->Begin() + (Iterator - OtherVector.Begin())) ElementType(*Iterator);
		}
	}

	GD_VECTOR_TEMPLATE()
	inline GD_VECTOR_CLASS()::Vector(Vector&& OtherVector)
		: MemoryProviderInstance(Move(OtherVector.MemoryProviderInstance))
	{
		this->Count	= OtherVector.Count;
		OtherVector.Count = 0;
	}

	GD_VECTOR_TEMPLATE()
	inline GD_VECTOR_CLASS()::~Vector()
	{
		this->Clear();
	}

	GD_VECTOR_TEMPLATE()
	inline size_t GD_VECTOR_CLASS()::GetSize() const
	{
		return this->Count;
	}

	GD_VECTOR_TEMPLATE()
	inline size_t GD_VECTOR_CLASS()::GetCapacity() const
	{
		return this->MemoryProviderInstance.GetCapacity();
	}

	GD_VECTOR_TEMPLATE()
	inline void GD_VECTOR_CLASS()::Resize(size_t const NewSize)
	{
		if (this->GetSize() != NewSize) {	// We do not need to resize our element with same size.
			if (this->GetSize() < NewSize) {	// Increasing size of container, we need to initialize new elements with template value.
				this->ReserveToSize(NewSize);
				for (MutableIterator Iterator = this->End(); Iterator != (this->Begin() + NewSize); Iterator += 1) {
					new (&*Iterator) ElementType();
				}
			} else {	// Decreasing size of container, we need to destroy last elements there.
				for (MutableIterator Iterator = (this->Begin() + NewSize); Iterator != this->End(); Iterator += 1) {
					(*Iterator).~ElementType();
				}
			}

			this->Count = NewSize;
		}
	}

	GD_VECTOR_TEMPLATE()
	inline void GD_VECTOR_CLASS()::Reserve(size_t const NewCapacity)
	{	// Moving elements into other memory block. Note that elements outside range keep uninitialized.
		if (this->GetCapacity() != NewCapacity)	{
			MemoryProviderType NewMemoryProviderInstance(NewCapacity);
			for (MutableIterator Iterator = this->Begin(); Iterator != (this->Begin() + Min(this->GetSize(), NewCapacity)); Iterator += 1) {
				new (NewMemoryProviderInstance.GetPointer() + (Iterator - this->Begin())) ElementType(Move(*Iterator));
			}
			for (MutableIterator Iterator = this->Begin(); Iterator != this->End(); Iterator += 1) {
				(*Iterator).~ElementType();
			}

			this->MemoryProviderInstance = Move(NewMemoryProviderInstance);
		}
	}

	GD_VECTOR_TEMPLATE()
	inline void GD_VECTOR_CLASS()::ReserveToSize(size_t const NewSize)
	{
		if (this->GetCapacity() < NewSize) {	// Changing capacity to make container not reallocate memory next time.
			size_t const Exponent = /*IntegerMath::Log2(NewSize)*/static_cast<size_t>(log2f(float(NewSize)));
			this->Reserve(size_t(1) << (Exponent + size_t(1)));
		}
	}

	GD_VECTOR_TEMPLATE()
	inline void GD_VECTOR_CLASS()::Emptify()
	{
		this->Resize(0);
	}

	GD_VECTOR_TEMPLATE()
	inline void GD_VECTOR_CLASS()::Clear()
	{
		this->Reserve(0);
	}

	GD_VECTOR_TEMPLATE()
	inline ElementType const& GD_VECTOR_CLASS()::GetElementAt(size_t const Index) const
	{
		GD_ASSERT((Index < this->GetSize()), "Index is out of bounds");
		return *(this->Begin() + Index);
	}

	GD_VECTOR_TEMPLATE()
	inline ElementType& GD_VECTOR_CLASS()::GetElementAt(size_t const Index)
	{
		GD_ASSERT((Index < this->GetSize()), "Index is is out of bounds");
		return *(this->Begin() + Index);
	}

	GD_VECTOR_TEMPLATE()
	inline ElementType const& GD_VECTOR_CLASS()::GetFirstElement() const
	{
		return this->GetElementAt(0);
	}

	GD_VECTOR_TEMPLATE()
	inline ElementType& GD_VECTOR_CLASS()::GetFirstElement()
	{
		return this->GetElementAt(0);
	}

	GD_VECTOR_TEMPLATE()
	inline ElementType const& GD_VECTOR_CLASS()::GetLastElement() const
	{
		return this->GetElementAt(this->GetSize() - 1);
	}

	GD_VECTOR_TEMPLATE()
	inline ElementType& GD_VECTOR_CLASS()::GetLastElement()
	{
		return this->GetElementAt(this->GetSize() - 1);
	}

	GD_VECTOR_TEMPLATE()
	inline void GD_VECTOR_CLASS()::SetElementAt(size_t const Index, ElementType&& Element)
	{
		this->GetElementAt(Index) = Forward<ElementType>(Element);
	}

	GD_VECTOR_TEMPLATE()
	inline void GD_VECTOR_CLASS()::SetElementAt(size_t const Index, ElementType const& Element)
	{
		this->GetElementAt(Index) = Element;
	}

	GD_VECTOR_TEMPLATE()
	inline void GD_VECTOR_CLASS()::InsertElementAt(size_t const Index, ElementType&& Element)
	{
		GD_ASSERT((Index <= this->GetSize()), "Index is out of bounds");
		this->ReserveToSize(this->GetSize() + 1);
		this->Count += 1;
		for (MutableIterator Iterator = this->End() - 1; Iterator != this->Begin() + Index - 1; Iterator -= 1) {
			*(Iterator - 1) = Move(*Iterator);
		}

		new (&*this->Begin() + Index) ElementType(Forward<ElementType>(Element));
	}

	GD_VECTOR_TEMPLATE()
	inline void GD_VECTOR_CLASS()::InsertElementAt(size_t const Index, ElementType const& Element)
	{
		this->InsertElementAt(Index, Move(ElementType(Element)));
	}

	GD_VECTOR_TEMPLATE()
	inline void GD_VECTOR_CLASS()::RemoveElementAt(size_t const Index)
	{
		GD_ASSERT((Index < this->GetSize()), "Index is out of bounds");
		for (MutableIterator Iterator = this->Begin() + Index; Iterator != (this->End() - 1); Iterator += 1) {
			*Iterator = Move(*(Iterator + 1));
		}

		--this->Count;
		(*this->End()).~ElementType();
	}

	GD_VECTOR_TEMPLATE()
	inline void GD_VECTOR_CLASS()::PushLast(ElementType&& NewElement /* = ElementType() */)
	{
		this->ReserveToSize(this->GetSize() + 1);
		this->Count += 1;
		new (&*this->End() - 1) ElementType(Forward<ElementType>(NewElement));
	}

	GD_VECTOR_TEMPLATE()
	inline void GD_VECTOR_CLASS()::PushLast(ElementType const& NewElement /* = ElementType() */)
	{
		this->PushLast(Move(ElementType(NewElement)));
	}

	GD_VECTOR_TEMPLATE()
	inline void GD_VECTOR_CLASS()::PopLast()
	{
		GD_ASSERT((this->GetSize() != 0), "Container size is zero");
		this->Resize(this->GetSize() - 1);
	}

	GD_VECTOR_TEMPLATE() template<typename SearchingPredicateType>
	inline size_t GD_VECTOR_CLASS()::FindFirstElement(SearchingPredicateType const& SearchingPredicate) const
	{
		for (ConstIterator Iterator = this->Begin(); Iterator != this->End(); Iterator += 1) {
			if (SearchingPredicate(*Iterator)) {
				return (Iterator - this->Begin());
			}
		}

		return SIZE_MAX;
	}

	GD_VECTOR_TEMPLATE() template<typename SearchingPredicateType>
	inline size_t GD_VECTOR_CLASS()::FindLastElement(SearchingPredicateType const& SearchingPredicate) const
	{
		for (ReverseConstIterator Iterator = this->ReverseBegin(); Iterator != this->ReverseEnd(); Iterator += 1) {
			if (SearchingPredicate(*Iterator)) {
				return (Iterator - this->ReverseEnd());
			}
		}

		return SIZE_MAX;
	}

	GD_VECTOR_TEMPLATE()
	inline size_t GD_VECTOR_CLASS()::FindFirstElement(ElementType const& Element) const
	{	// Lets use lambda as our predicate
		return this->FindFirstElement([&Element](ElementType const& OtherElement) {
			return (OtherElement == Element);
		});
	}

	GD_VECTOR_TEMPLATE()
	inline size_t GD_VECTOR_CLASS()::FindLastElement(ElementType const& Element) const
	{	// Lets use lambda as our predicate
		return this->FindFirstElement([&Element](ElementType const& OtherElement) {
			return (OtherElement == Element);
		});
	}

	GD_VECTOR_TEMPLATE() template<typename SortingPredicateType>
	inline void GD_VECTOR_CLASS()::Sort(SortingPredicateType const& SortingPredicate, MutableIterator const _Left, MutableIterator const _Right)
	{	// Sorting elements with quick sort implementation, provided here http://anandpandia.blogspot.ru/2012/08/quicksort-algorithm-and-example-in-c.html
		/// @todo Implementation is a little broken if container includes equal values.
		MutableIterator Left = _Left, Right = _Right;
		MutableIterator const Pivot = this->Begin() + (((Left - this->Begin()) + (Right - this->Begin())) / 2);
		while (Left <= Right) {
			while (SortingPredicate((*Left ), (*Pivot))) Left  += 1;
			while (SortingPredicate((*Pivot), (*Right))) Right -= 1;
			if (Left <= Right) {	
				Swap((*Left), (*Right));
				Left  += 1;
				Right -= 1;
			}
		}

		if (_Left <  Right) this->Sort(SortingPredicate, _Left,  Right);
		if ( Left < _Right) this->Sort(SortingPredicate,  Left, _Right);
	}

	GD_VECTOR_TEMPLATE() template<typename SortingPredicateType>
	inline void GD_VECTOR_CLASS()::Sort(SortingPredicateType const& SortingPredicate)
	{	// Sorting all container range.
		this->Sort(SortingPredicate, this->Begin(), (this->End() - 1));
	}

	GD_VECTOR_TEMPLATE()
	inline void GD_VECTOR_CLASS()::Sort()
	{	// Sorting all container range with default predicate.
		this->Sort([](ElementType const& Right, ElementType const& Left) {
			return (Right < Left);
		});
	}

	GD_VECTOR_TEMPLATE()
	inline GD_VECTOR_CLASS()& GD_VECTOR_CLASS()::operator= (Vector&& OtherVector)
	{	
		if ((&OtherVector) != this) {	// Check if we are not assigning container itself.
			this->Clear();
			this->MemoryProviderInstance = Move(OtherVector.MemoryProviderInstance);
			this->Count	= OtherVector.Count;
			OtherVector.Count = 0;
		}

		return (*this);
	}

	GD_VECTOR_TEMPLATE()
	inline GD_VECTOR_CLASS()& GD_VECTOR_CLASS()::operator= (Vector const& OtherVector)
	{	
		if ((&OtherVector) != this) {	// Check if we are not assigning container itself.
			if (this->GetCapacity() >= OtherVector.GetSize()) {	// We have enough place here, so it is optimal to move is here without realloc.
				this->Emptify();
				this->Count = OtherVector.Count;
				for (ConstIterator Iterator = OtherVector.Begin(); Iterator != OtherVector.End(); Iterator += 1) {
					new (this->Begin() + (Iterator - OtherVector.Begin())) ElementType(*Iterator);
				}
			} else {	// Not enough memory. Copying specified vector and moving it here.
				(*this) = Move(Vector(OtherVector));
			}
		}

		return (*this);
	}

	GD_VECTOR_TEMPLATE()
	inline GD_VECTOR_CLASS()& GD_VECTOR_CLASS()::operator= (std::initializer_list<ElementType> const& InitializerList)
	{	
		if (this->GetCapacity() >= InitializerList.size()) {	// We have enough place here, so it is optimal to move is here without realloc.
			this->Emptify();
			this->Count = InitializerList.size();
			for (typename std::initializer_list<ElementType>::const_iterator Iterator = InitializerList.begin(); Iterator != InitializerList.end(); Iterator += 1) {
				new (this->Begin() + (Iterator - InitializerList.begin())) ElementType(*Iterator);
			}
		} else {	// Not enough memory. Assigning itself a container created with specified initializer list.
			(*this) = Move(Vector(InitializerList));
		}

		return (*this);
	}

#if (defined(__cplusplus_cli))
	GD_VECTOR_TEMPLATE()
	inline GD_VECTOR_CLASS()& GD_VECTOR_CLASS()::operator= (array<ElementType>^ CliArray)
	{
		if (this->GetCapacity() >= static_cast<size_t>(CliArray->Length)) {	// We have enough place here, so it is optimal to move is here without realloc.
			this->Emptify();
			this->Count = static_cast<size_t>(CliArray->Length);
			for (ConstIterator Iterator = StartIterator; Iterator != EndIterator; Iterator += 1) {
				new (this->Begin() + (Iterator - OtherVector.Begin())) ElementType(CliArray[Iterator - OtherVector.Begin()]);
			}
		} else {	// Not enough memory. Assigning itself a container created with CLI/C++ Array.
			(*this) = Move(Vector(CliArray));
		}

		return (*this);
	}
#endif	// if (defined(__cplusplus_cli))

	GD_VECTOR_TEMPLATE()
	inline ElementType const& GD_VECTOR_CLASS()::operator[] (size_t const Index) const
	{	// Constant access operator.
		return this->GetElementAt(Index);
	}

	GD_VECTOR_TEMPLATE()
	inline ElementType& GD_VECTOR_CLASS()::operator[] (size_t const Index)
	{	// Mutable access operator.
		return this->GetElementAt(Index);
	}

#	undef GD_VECTOR_TEMPLATE
#	undef GD_VECTOR_CLASS
GD_NAMESPACE_END
