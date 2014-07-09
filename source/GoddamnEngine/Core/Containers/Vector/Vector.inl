/// ==========================================================================================
/// Vector.inl - Dynamically sized array implementation
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.11.2013 - Created by James Jhuighuy
///		* 24.04.2014 - Rewritten from scratch by James Jhuighuy
/// ==========================================================================================

#if (!defined(GD_CORE_CONTAINERS_VECTOR))
#	error "Attempting to include 'Vector.inl' file. Please, use 'Vector.hh' instead.""
#endif	// if (!defined(GD_CORE_CONTAINERS_VECTOR))

#include <GoddamnEngine/Core/Diagnostics/Assertion/Assertion.hh>
#include <GoddamnEngine/Core/Math/FastMath/FastMath.hh>
#include <GoddamnEngine/Core/Utility.hh>
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
	inline GD_VECTOR_CLASS()::Vector(size_t const InitialElemntsCount /* = 0 */, ElementType const& InitialElement /* = ElementType() */, size_t const Capacity /* = -1 */)
		: MemoryProviderInstance((Capacity != -1) ? Capacity : InitialElemntsCount) 
	{
		self->Count = InitialElemntsCount;
		for (MutableIterator Iterator = self->Begin(); Iterator != self->End(); Iterator += 1)
			new (&*Iterator) ElementType(InitialElement);
	}

	GD_VECTOR_TEMPLATE()
	inline GD_VECTOR_CLASS()::Vector(ConstIterator const StartIterator, ConstIterator const EndIterator)
		: MemoryProviderInstance(static_cast<size_t>(EndIterator - StartIterator))
	{
		GD_DEBUG_ASSERT((EndIterator >= StartIterator), "Invalid iterators specified.");
		self->Count = static_cast<size_t>(EndIterator - StartIterator);
		for (ConstIterator Iterator = StartIterator; Iterator != EndIterator; Iterator += 1)
			new (&*self->Begin() + (Iterator - StartIterator)) ElementType(*Iterator);
	}

	GD_VECTOR_TEMPLATE()
	inline GD_VECTOR_CLASS()::Vector(std::initializer_list<ElementType> const& InitializerList)
		: MemoryProviderInstance(InitializerList.size())
	{
		self->Count = InitializerList.size();
		for (typename std::initializer_list<ElementType>::const_iterator Iterator = InitializerList.begin(); Iterator != InitializerList.end(); Iterator += 1)
			new (&*self->Begin() + (Iterator - InitializerList.begin())) ElementType(*Iterator);
	}

#if (defined(__cplusplus_cli))
	GD_VECTOR_TEMPLATE()
	inline GD_VECTOR_CLASS()::Vector(array<ElementType>^ CliArray)
		: MemoryProviderInstance(static_cast<size_t>(CliArray->Length))
	{
		self->Count = static_cast<size_t>(CliArray->Length);
		for (ConstIterator Iterator = StartIterator; Iterator != EndIterator; Iterator += 1)
			new (&*self->Begin() + (Iterator - OtherVector.Begin()) ElementType(CliArray[Iterator - OtherVector.Begin()]);
	}
#endif	// if (defined(__cplusplus_cli))

	GD_VECTOR_TEMPLATE()
	inline GD_VECTOR_CLASS()::Vector(Vector const& OtherVector)
		: MemoryProviderInstance(OtherVector.GetCapacity())
	{
		self->Count = OtherVector.Count;
		for (ConstIterator Iterator = OtherVector.Begin(); Iterator != OtherVector.End(); Iterator += 1)
			new (&*self->Begin() + (Iterator - OtherVector.Begin())) ElementType(*Iterator);
	}

	GD_VECTOR_TEMPLATE()
	inline GD_VECTOR_CLASS()::Vector(Vector&& OtherVector)
		: MemoryProviderInstance(Move(OtherVector.MemoryProviderInstance))
	{
		self->Count	= OtherVector.Count;
		OtherVector.Count = 0;
	}

	GD_VECTOR_TEMPLATE()
	inline GD_VECTOR_CLASS()::~Vector()
	{
		self->Clear();
	}

	GD_VECTOR_TEMPLATE()
	inline size_t GD_VECTOR_CLASS()::GetSize() const
	{
		return self->Count;
	}

	GD_VECTOR_TEMPLATE()
	inline size_t GD_VECTOR_CLASS()::GetCapacity() const
	{
		return self->MemoryProviderInstance.GetCapacity();
	}

	GD_VECTOR_TEMPLATE()
	inline void GD_VECTOR_CLASS()::Resize(size_t const NewSize, ElementType const& InitialElement /* = ElementType() */)
	{
		if (self->GetSize() != NewSize)
		{	// We do not need to resize our element with same size.
			if (self->GetSize() < NewSize)
			{	// Increasing size of container, we need to initialize new elements with template value.
				self->ReserveToSize(NewSize);
				for (MutableIterator Iterator = self->End(); Iterator != (self->Begin() + NewSize); Iterator += 1)
					new (&*Iterator) ElementType(InitialElement);
			}
			else
			{	// Decreasing size of container, we need to destroy last elements there.
				for (MutableIterator Iterator = (self->Begin() + NewSize); Iterator != self->End(); Iterator += 1)
					(*Iterator).~ElementType();
			}

			self->Count = NewSize;
		}
	}

	GD_VECTOR_TEMPLATE()
	inline void GD_VECTOR_CLASS()::Reserve(size_t const NewCapacity)
	{	// Moving elements into other memory block. Note that elements outside range keep uninitialized.
		if (self->GetCapacity() != NewCapacity)
		{
			MemoryProviderType NewMemoryProviderInstance(NewCapacity);
			for (MutableIterator Iterator = self->Begin(); Iterator != (self->Begin() + Min(self->GetSize(), NewCapacity)); Iterator += 1)
				new (NewMemoryProviderInstance.GetPointer() + (Iterator - self->Begin())) ElementType(Move(*Iterator));
			for (MutableIterator Iterator = self->Begin(); Iterator !=  self->End(); Iterator += 1)
				(*Iterator).~ElementType();

			self->MemoryProviderInstance = Move(NewMemoryProviderInstance);
		}
	}

	GD_VECTOR_TEMPLATE()
	inline void GD_VECTOR_CLASS()::ReserveToSize(size_t const NewSize)
	{
		if (self->GetCapacity() < NewSize)
		{	// Changing capacity to make container not reallocate memory next time.
			size_t const Exponent = /*IntegerMath::Log2(NewSize)*/static_cast<size_t>(log2f(float(NewSize)));
			self->Reserve(size_t(1) << (Exponent + size_t(1)));
		}
	}

	GD_VECTOR_TEMPLATE()
	inline void GD_VECTOR_CLASS()::Emptify()
	{
		self->Resize(0);
	}

	GD_VECTOR_TEMPLATE()
	inline void GD_VECTOR_CLASS()::Clear()
	{
		self->Reserve(0);
	}

	GD_VECTOR_TEMPLATE()
	inline ElementType const& GD_VECTOR_CLASS()::GetElementAt(size_t const Index) const
	{
		GD_ASSERT((Index < self->GetSize()), "Index is out of bounds");
		return *(self->Begin() + Index);
	}

	GD_VECTOR_TEMPLATE()
	inline ElementType& GD_VECTOR_CLASS()::GetElementAt(size_t const Index)
	{
		GD_ASSERT((Index < self->GetSize()), "Index is is out of bounds");
		return *(self->Begin() + Index);
	}

	GD_VECTOR_TEMPLATE()
	inline ElementType const& GD_VECTOR_CLASS()::GetFirstElement() const
	{
		return self->GetElementAt(0);
	}

	GD_VECTOR_TEMPLATE()
	inline ElementType& GD_VECTOR_CLASS()::GetFirstElement()
	{
		return self->GetElementAt(0);
	}

	GD_VECTOR_TEMPLATE()
	inline ElementType const& GD_VECTOR_CLASS()::GetLastElement() const
	{
		return self->GetElementAt(self->GetSize() - 1);
	}

	GD_VECTOR_TEMPLATE()
	inline ElementType& GD_VECTOR_CLASS()::GetLastElement()
	{
		return self->GetElementAt(self->GetSize() - 1);
	}

	GD_VECTOR_TEMPLATE()
	inline void GD_VECTOR_CLASS()::SetElementAt(size_t const Index, ElementType&& Element)
	{
		self->GetElementAt(Index) = Forward<ElementType>(Element);
	}

	GD_VECTOR_TEMPLATE()
	inline void GD_VECTOR_CLASS()::SetElementAt(size_t const Index, ElementType const& Element)
	{
		self->GetElementAt(Index) = Element;
	}

	GD_VECTOR_TEMPLATE()
	inline void GD_VECTOR_CLASS()::InsertElementAt(size_t const Index, ElementType&& Element)
	{
		GD_ASSERT((Index <= self->GetSize()), "Index is out of bounds");
		self->ReserveToSize(self->GetSize() + 1);
		self->Count += 1;
		for (MutableIterator Iterator = self->End() - 1; Iterator != self->Begin() + Index - 1; Iterator -= 1)
			*(Iterator - 1) = Move(*Iterator);

		new (&*self->Begin() + Index) ElementType(Forward<ElementType>(Element));
	}

	GD_VECTOR_TEMPLATE()
	inline void GD_VECTOR_CLASS()::InsertElementAt(size_t const Index, ElementType const& Element)
	{
		self->InsertElementAt(Index, Move(ElementType(Element)));
	}

	GD_VECTOR_TEMPLATE()
	inline void GD_VECTOR_CLASS()::RemoveElementAt(size_t const Index)
	{
		GD_ASSERT((Index < self->GetSize()), "Index is out of bounds");
		for (MutableIterator Iterator = self->Begin() + Index; Iterator != (self->End() - 1); Iterator += 1)
			*Iterator = Move(*(Iterator + 1));

		  self->Count -= 1;
		(*self->End()).~ElementType();
	}

	GD_VECTOR_TEMPLATE()
	inline void GD_VECTOR_CLASS()::PushLast(ElementType&& NewElement /* = ElementType() */)
	{
		self->ReserveToSize(self->GetSize() + 1);
		self->Count += 1;
		new (&*self->End() - 1) ElementType(Forward<ElementType>(NewElement));
	}

	GD_VECTOR_TEMPLATE()
	inline void GD_VECTOR_CLASS()::PushLast(ElementType const& NewElement /* = ElementType() */)
	{
		self->PushLast(Move(ElementType(NewElement)));
	}

	GD_VECTOR_TEMPLATE()
	inline void GD_VECTOR_CLASS()::PopLast()
	{
		GD_ASSERT((self->GetSize() != 0), "Container size is zero");
		self->Resize(self->GetSize() - 1);
	}

	GD_VECTOR_TEMPLATE() template<typename SearchingPredicateType>
	inline size_t GD_VECTOR_CLASS()::FindFirstElement(SearchingPredicateType const& SearchingPredicate) const
	{
		for (ConstIterator Iterator = self->Begin(); Iterator != self->End(); Iterator += 1)
		{
			if (SearchingPredicate(*Iterator))
				return (Iterator - self->Begin());
		}

		return SIZE_MAX;
	}

	GD_VECTOR_TEMPLATE() template<typename SearchingPredicateType>
	inline size_t GD_VECTOR_CLASS()::FindLastElement(SearchingPredicateType const& SearchingPredicate) const
	{
		for (ReverseConstIterator Iterator = self->ReverseBegin(); Iterator != self->ReverseEnd(); Iterator += 1)
		{
			if (SearchingPredicate(*Iterator))
				return (Iterator - self->ReverseEnd());
		}

		return SIZE_MAX;
	}

	GD_VECTOR_TEMPLATE()
	inline size_t GD_VECTOR_CLASS()::FindFirstElement(ElementType const& Element) const
	{	// Lets use lambda as our predicate
		return self->FindFirstElement([&Element](ElementType const& OtherElement){
			return (OtherElement == Element);
		});
	}

	GD_VECTOR_TEMPLATE()
	inline size_t GD_VECTOR_CLASS()::FindLastElement(ElementType const& Element) const
	{	// Lets use lambda as our predicate
		return self->FindFirstElement([&Element](ElementType const& OtherElement){
			return (OtherElement == Element);
		});
	}

	GD_VECTOR_TEMPLATE() template<typename SortingPredicateType>
	inline void GD_VECTOR_CLASS()::Sort(SortingPredicateType const& SortingPredicate, MutableIterator const _Left, MutableIterator const _Right)
	{	// Sorting elements with quick sort implementation, provided here http://anandpandia.blogspot.ru/2012/08/quicksort-algorithm-and-example-in-c.html
		/// @todo Implementation is a little broken if container includes equal values.
		MutableIterator Left = _Left, Right = _Right;
		MutableIterator const Pivot = self->Begin() + (((Left - self->Begin()) + (Right - self->Begin())) / 2);
		while (Left <= Right)
		{
			while (SortingPredicate((*Left ), (*Pivot))) Left  += 1;
			while (SortingPredicate((*Pivot), (*Right))) Right -= 1;
			if (Left <= Right)
			{	
				Swap((*Left), (*Right));
				Left  += 1;
				Right -= 1;
			}
		}

		if (_Left <  Right) self->Sort(SortingPredicate, _Left,  Right);
		if ( Left < _Right) self->Sort(SortingPredicate,  Left, _Right);
	}

	GD_VECTOR_TEMPLATE() template<typename SortingPredicateType>
	inline void GD_VECTOR_CLASS()::Sort(SortingPredicateType const& SortingPredicate)
	{	// Sorting all container range.
		self->Sort(SortingPredicate, self->Begin(), (self->End() - 1));
	}

	GD_VECTOR_TEMPLATE()
	inline void GD_VECTOR_CLASS()::Sort()
	{	// Sorting all container range with default predicate.
		self->Sort([](ElementType const& Right, ElementType const& Left) {
			return (Right < Left);
		});
	}

	GD_VECTOR_TEMPLATE()
	inline GD_VECTOR_CLASS()& GD_VECTOR_CLASS()::operator= (Vector&& OtherVector)
	{	
		if ((&OtherVector) != self)
		{	// Check if we are not assigning container itself.
			self->Clear();
			self->MemoryProviderInstance = Move(OtherVector.MemoryProviderInstance);
			self->Count	= OtherVector.Count;
			OtherVector.Count = 0;
		}

		return (*self);
	}

	GD_VECTOR_TEMPLATE()
	inline GD_VECTOR_CLASS()& GD_VECTOR_CLASS()::operator= (Vector const& OtherVector)
	{	
		if ((&OtherVector) != self)
		{	// Check if we are not assigning container itself.
			if (self->GetCapacity() >= OtherVector.GetSize())
			{	// We have enough place here, so it is optimal to move is here without realloc.
				self->Emptify();
				self->Count = OtherVector.Count;
				for (ConstIterator Iterator = OtherVector.Begin(); Iterator != OtherVector.End(); Iterator += 1)
					new (self->Begin() + (Iterator - OtherVector.Begin())) ElementType(*Iterator);
			}
			else
			{	// Not enough memory. Copying specified vector and moving it here.
				(*self) = Move(Vector(OtherVector));
			}
		}

		return (*self);
	}

	GD_VECTOR_TEMPLATE()
	inline GD_VECTOR_CLASS()& GD_VECTOR_CLASS()::operator= (std::initializer_list<ElementType> const& InitializerList)
	{	
		if (self->GetCapacity() >= InitializerList.size())
		{	// We have enough place here, so it is optimal to move is here without realloc.
			self->Emptify();
			self->Count = InitializerList.size();
			for (typename std::initializer_list<ElementType>::const_iterator Iterator = InitializerList.begin(); Iterator != InitializerList.end(); Iterator += 1)
				new (self->Begin() + (Iterator - InitializerList.begin())) ElementType(*Iterator);
		}
		else
		{	// Not enough memory. Assigning itself a container created with specified initializer list.
			(*self) = Move(Vector(InitializerList));
		}

		return (*self);
	}

#if (defined(__cplusplus_cli))
	GD_VECTOR_TEMPLATE()
	inline GD_VECTOR_CLASS()& GD_VECTOR_CLASS()::operator= (array<ElementType>^ CliArray)
	{
		if (self->GetCapacity() >= static_cast<size_t>(CliArray->Length))
		{	// We have enough place here, so it is optimal to move is here without realloc.
			self->Emptify();
			self->Count = static_cast<size_t>(CliArray->Length);
			for (ConstIterator Iterator = StartIterator; Iterator != EndIterator; Iterator += 1)
				new (self->Begin() + (Iterator - OtherVector.Begin())) ElementType(CliArray[Iterator - OtherVector.Begin()]);
		}
		else
		{	// Not enough memory. Assigning itself a container created with CLI/C++ Array.
			(*self) = Move(Vector(CliArray));
		}

		return (*self);
	}
#endif	// if (defined(__cplusplus_cli))

	GD_VECTOR_TEMPLATE()
	inline ElementType const& GD_VECTOR_CLASS()::operator[] (size_t const Index) const
	{	// Constant access operator.
		return self->GetElementAt(Index);
	}

	GD_VECTOR_TEMPLATE()
	inline ElementType& GD_VECTOR_CLASS()::operator[] (size_t const Index)
	{	// Mutable access operator.
		return self->GetElementAt(Index);
	}

#	undef GD_VECTOR_TEMPLATE
#	undef GD_VECTOR_CLASS
GD_NAMESPACE_END
