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

	/// ------------------------------------------------------------------------------------------
	/// Constructors/Destructor.
	/// ------------------------------------------------------------------------------------------

	template<size_t const Capacity>
	inline VectorMemoryProviders::StackMemoryProvider<Capacity>::StackMemoryProvider(size_t const TheCapacity)
	{ 
		GD_ASSERT((TheCapacity <= Capacity), "Stack memory provider size is out of range.");
	}

	/// ------------------------------------------------------------------------------------------
	/// Operators.
	/// ------------------------------------------------------------------------------------------

	template<size_t const Capacity>
	inline VectorMemoryProviders::StackMemoryProvider<Capacity>& VectorMemoryProviders::StackMemoryProvider<Capacity>::operator= (StackMemoryProvider&& OtherStackMemoryProvider)
	{
		if ((&OtherStackMemoryProvider) != this) {
			// This operator is quiet dangerous because it does not moves object into new places but just copies memory.
			// And do we actually need to fill with zeros all moved memory? So leave it be how it is.
			this->~StackMemoryProvider();
			std::memcpy(&this->Memory[0], &OtherStackMemoryProvider.Memory[0],    sizeof(this->Memory));
			std::memset(                  &OtherStackMemoryProvider.Memory[0], 0, sizeof(OtherStackMemoryProvider.Memory));
		}

		return (*this);
	}

	/// ------------------------------------------------------------------------------------------
	/// Swap function.
	/// ------------------------------------------------------------------------------------------

	template<size_t const Capacity>
	inline void Swap(VectorMemoryProviders::StackMemoryProvider<Capacity>& First, VectorMemoryProviders::StackMemoryProvider<Capacity>& Second)
	{
		if (&First != &Second) {
			for (size_t BlockIndex = 0; BlockIndex < MemoryBlockSize; ++BlockIndex) {
				Swap(First.Memory[BlockIndex], Second.Memory[BlockIndex]);
			}
		}
	}

	/// ==========================================================================================
	/// HeapMemoryProvider class
	/// ==========================================================================================

	/// ------------------------------------------------------------------------------------------
	/// Constructors/Destructor.
	/// ------------------------------------------------------------------------------------------

	inline VectorMemoryProviders::HeapMemoryProvider::HeapMemoryProvider(size_t const Capacity)
		: Capacity(Capacity)
		, Memory(Allocator::AllocateMemory(Capacity))
	{
	}

	inline VectorMemoryProviders::HeapMemoryProvider::HeapMemoryProvider(HeapMemoryProvider&& OtherHeapMemoryProvider)
		: Capacity(OtherHeapMemoryProvider.Capacity)
		, Memory(OtherHeapMemoryProvider.Memory)
	{
		OtherHeapMemoryProvider.Memory = nullptr;
		OtherHeapMemoryProvider.Capacity = 0;
	}

	inline VectorMemoryProviders::HeapMemoryProvider::~HeapMemoryProvider()
	{
		Allocator::DeallocateMemory(static_cast<handle>(this->Memory));
		this->Memory = nullptr;
	}

	/// ------------------------------------------------------------------------------------------
	/// Operators.
	/// ------------------------------------------------------------------------------------------

	inline VectorMemoryProviders::HeapMemoryProvider& VectorMemoryProviders::HeapMemoryProvider::operator= (HeapMemoryProvider&& OtherHeapMemoryProvider)
	{
		if ((&OtherHeapMemoryProvider) != this) {
			this->~HeapMemoryProvider();
			this->Capacity = OtherHeapMemoryProvider.Capacity;
			this->Memory = OtherHeapMemoryProvider.Memory;
			OtherHeapMemoryProvider.Capacity = 0;
			OtherHeapMemoryProvider.Memory = nullptr;
		}

		return (*this);
	}

	/// ------------------------------------------------------------------------------------------
	/// Swap function.
	/// ------------------------------------------------------------------------------------------

	inline void Swap(VectorMemoryProviders::HeapMemoryProvider& First, VectorMemoryProviders::HeapMemoryProvider& Second)
	{
		if (&First != &Second) {
			Swap(First.Memory, Second.Memory);
			Swap(First.Capacity, Second.Capacity);
		}
	}

	/// ==========================================================================================
	/// Vector class
	/// ==========================================================================================

	// To prevent a lot of copy-pastes in code.
#define GD_VECTOR_TEMPLATE template<typename ElementType, typename MemoryProviderType /*= VectorMemoryProviders::HeapMemoryProvider*/>
#define GD_VECTOR_CLASS	   Vector<ElementType, MemoryProviderType>

	/// ------------------------------------------------------------------------------------------
	/// Private methods.
	/// ------------------------------------------------------------------------------------------

	GD_VECTOR_TEMPLATE template<typename OtherSourceIteratorType, typename OtherDestinationIteratorType>
	inline void GD_VECTOR_CLASS::CopyFromRange(OtherSourceIteratorType const& StartSourceIterator, OtherSourceIteratorType const& EndSourceIterator, OtherDestinationIteratorType const& StartDestinationIterator)
	{
		for (OtherSourceIteratorType Iterator = StartSourceIterator; Iterator != EndSourceIterator; ++Iterator) {
			new (StartDestinationIterator + (Iterator - StartSourceIterator)) ElementType(*Iterator);
		}
	}

	GD_VECTOR_TEMPLATE template<typename OtherSourceIteratorType, typename OtherDestinationIteratorType>
	inline void GD_VECTOR_CLASS::MoveFromRange(OtherSourceIteratorType const& StartSourceIterator, OtherSourceIteratorType const& EndSourceIterator, OtherDestinationIteratorType const& StartDestinationIterator)
	{
		for (OtherSourceIteratorType Iterator = StartSourceIterator; Iterator != EndSourceIterator; ++Iterator) {
			new (StartDestinationIterator + (Iterator - StartSourceIterator)) ElementType(Forward<ElementType>(*Iterator));
		}
	}

	GD_VECTOR_TEMPLATE
	inline void GD_VECTOR_CLASS::InitializeRange(PtrMutableIterator const StartIterator, PtrMutableIterator const EndIterator)
	{
		for (PtrMutableIterator Iterator = StartIterator; Iterator != EndIterator; ++Iterator) {
			new (Iterator) ElementType();
		}
	}

	GD_VECTOR_TEMPLATE
	inline void GD_VECTOR_CLASS::DeinitializeRange(PtrMutableIterator const StartIterator, PtrMutableIterator const EndIterator)
	{
		for (PtrMutableIterator Iterator = StartIterator; Iterator != EndIterator; ++Iterator) {
			Iterator->~ElementType();
		}
	}

	GD_VECTOR_TEMPLATE template<typename ComparasionPredicateType>
	inline bool GD_VECTOR_CLASS::CompareTo(Vector const& OtherVector, ComparasionPredicateType const& ComparasionPredicate)
	{
		size_t MinCount = Min(this->Count, OtherVector.GetSize());
		for (size_t Index = 0; Index < MinCount; ++Index) {
			if (!ComparasionPredicate(*(this->PtrBegin() + Index), *(OtherVector.PtrBegin() + Index))) {
				return false;
			}
		}

		return true;
	}

	/// ------------------------------------------------------------------------------------------
	/// Constructors and destructor.
	/// ------------------------------------------------------------------------------------------

	GD_VECTOR_TEMPLATE
	inline GD_VECTOR_CLASS::Vector(size_t const InitialElemntsCount /* = 0 */, size_t const Capacity /* = SIZE_MAX */)
		: MemoryProviderInstance(((Capacity != SIZE_MAX) ? Capacity : InitialElemntsCount) * sizeof(ElementType)) 
	{
		this->Count = InitialElemntsCount;
		Vector::InitializeRange(this->PtrBegin(), this->PtrEnd());
	}

	GD_VECTOR_TEMPLATE
	inline GD_VECTOR_CLASS::Vector(ConstIterator const StartIterator, ConstIterator const EndIterator)
		: MemoryProviderInstance(static_cast<size_t>(EndIterator - StartIterator))
	{
		GD_DEBUG_ASSERT((EndIterator >= StartIterator), "Invalid iterators specified.");
		this->Count = static_cast<size_t>(EndIterator - StartIterator);
		Vector::CopyFromRange(StartIterator, EndIterator, this->PtrBegin());
	}

	GD_VECTOR_TEMPLATE
	inline GD_VECTOR_CLASS::Vector(std::initializer_list<ElementType> const& InitializerList)
		: MemoryProviderInstance(InitializerList.size())
	{
		this->Count = InitializerList.size();
		Vector::CopyFromRange(InitializerList.begin(), InitializerList.end(), this->PtrBegin());
	}

#if (defined(__cplusplus_cli))
	GD_VECTOR_TEMPLATE
	inline GD_VECTOR_CLASS::Vector(array<ElementType>^ CliArray)
		: MemoryProviderInstance(static_cast<size_t>(CliArray->Length))
	{
		this->Count = static_cast<size_t>(CliArray->Length);
		pin_ptr<ElementType> CliArrayPtr = &CliArray[0];
		PtrMutableIterator PtrCliArrayIteratorStart = CliArrayPtr;
		Vector::CopyFromRange(PtrCliArrayIteratorStart, PtrCliArrayIteratorStart + this->Count, this->PtrBegin());
	}
#endif	// if (defined(__cplusplus_cli))

	GD_VECTOR_TEMPLATE
	inline GD_VECTOR_CLASS::Vector(Vector const& OtherVector)
		: MemoryProviderInstance(OtherVector.GetCapacity())
	{
		this->Count = OtherVector.Count;
		Vector::CopyFromRange(OtherVector.PtrBegin(), OtherVector.PtrEnd(), this->PtrBegin());
	}

	GD_VECTOR_TEMPLATE
	inline GD_VECTOR_CLASS::Vector(Vector&& OtherVector)
		: MemoryProviderInstance(Move(OtherVector.MemoryProviderInstance))
	{
		this->Count	= OtherVector.Count;
		OtherVector.Count = 0;
	}

	GD_VECTOR_TEMPLATE
	inline GD_VECTOR_CLASS::~Vector()
	{
		this->Clear();
	}

	/// ------------------------------------------------------------------------------------------
	/// Dynamic size management.
	/// ------------------------------------------------------------------------------------------

	GD_VECTOR_TEMPLATE
	inline size_t GD_VECTOR_CLASS::GetSize() const
	{
		return this->Count;
	}

	GD_VECTOR_TEMPLATE
	inline size_t GD_VECTOR_CLASS::GetCapacity() const
	{
		return this->MemoryProviderInstance.GetCapacity();
	}

	GD_VECTOR_TEMPLATE
	inline void GD_VECTOR_CLASS::Resize(size_t const NewSize)
	{
		if (this->Count != NewSize) {	// We do not need to resize our element with same size.
			if (this->Count < NewSize) {	// Increasing size of container, we need to initialize new elements with template value.
				this->ReserveToSize(NewSize);
				Vector::InitializeRange(this->PtrEnd(), this->PtrBegin() + NewSize);
			} else {	// Decreasing size of container, we need to destroy last elements there.
				Vector::DeinitializeRange(this->PtrBegin() + NewSize, this->PtrEnd());
			}

			this->Count = NewSize;
		}
	}

	GD_VECTOR_TEMPLATE
	inline void GD_VECTOR_CLASS::Reserve(size_t const NewCapacity)
	{	
		// Moving elements into other memory block. Note that elements outside range keep uninitialized.
		if (MemoryProviderType::MaxCapacity == VectorMemoryProviders::FlexibleMaxCapacity) {
			if (this->GetCapacity() != NewCapacity)	{
				// Clamping contatiner size to capacity.
				if (NewCapacity < this->Count) {
					this->Resize(NewCapacity);
				}

				MemoryProviderType NewMemoryProviderInstance(NewCapacity * sizeof(ElementType));
				Vector::MoveFromRange(this->PtrBegin(), this->PtrEnd(), reinterpret_cast<PtrMutableIterator>(NewMemoryProviderInstance.GetPointer()));
				Vector::DeinitializeRange(this->PtrBegin(), this->PtrEnd());
				this->MemoryProviderInstance = Move(NewMemoryProviderInstance);
			}
		} else {
			GD_ASSERT(NewCapacity <= MemoryProviderType::MaxCapacity, "Unable to increase capacity. Max value reached.");
		}
	}

	GD_VECTOR_TEMPLATE
	inline void GD_VECTOR_CLASS::ReserveToSize(size_t const NewSize)
	{
		if (MemoryProviderType::MaxCapacity == VectorMemoryProviders::FlexibleMaxCapacity) {
			if (NewSize > this->GetCapacity()) {
				this->Reserve(Max(this->GetCapacity(), NewSize) * 13 / 10);
			}
		}
	}

	GD_VECTOR_TEMPLATE
	inline void GD_VECTOR_CLASS::ShrinkToFit()
	{
		this->Reserve(this->Count);
	}

	GD_VECTOR_TEMPLATE
	inline void GD_VECTOR_CLASS::Emptify()
	{
		this->Resize(0);
	}

	GD_VECTOR_TEMPLATE
	inline void GD_VECTOR_CLASS::Clear()
	{
		this->Reserve(0);
	}

	/// ------------------------------------------------------------------------------------------
	/// Elements access.
	/// ------------------------------------------------------------------------------------------

	GD_VECTOR_TEMPLATE
	inline ElementType const& GD_VECTOR_CLASS::GetElementAt(size_t const Index) const
	{
		GD_ASSERT(Index < this->Count, "Index is out of bounds");
		return (*(this->PtrBegin() + Index));
	}

	GD_VECTOR_TEMPLATE
	inline ElementType& GD_VECTOR_CLASS::GetElementAt(size_t const Index)
	{
		GD_ASSERT(Index < this->Count, "Index is is out of bounds");
		return (*(this->PtrBegin() + Index));
	}

	GD_VECTOR_TEMPLATE
	inline ElementType const& GD_VECTOR_CLASS::GetFirstElement() const
	{
		return this->GetElementAt(0);
	}

	GD_VECTOR_TEMPLATE
	inline ElementType& GD_VECTOR_CLASS::GetFirstElement()
	{
		return this->GetElementAt(0);
	}

	GD_VECTOR_TEMPLATE
	inline ElementType const& GD_VECTOR_CLASS::GetLastElement() const
	{
		return this->GetElementAt(this->Count - 1);
	}

	GD_VECTOR_TEMPLATE
	inline ElementType& GD_VECTOR_CLASS::GetLastElement()
	{
		return this->GetElementAt(this->Count - 1);
	}

	GD_VECTOR_TEMPLATE
	inline void GD_VECTOR_CLASS::SetElementAt(size_t const Index, ElementType&& Element)
	{
		ElementType& ExistingElement = this->GetElementAt(Index);
		ExistingElement.~ElementType();
		new (&ExistingElement) ElementType(Forward<ElementType>(Element));
	}

	GD_VECTOR_TEMPLATE
	inline void GD_VECTOR_CLASS::SetElementAt(size_t const Index, ElementType const& Element)
	{
		this->SetElementAt(Move(ElementType(Element)));
	}

	/// ------------------------------------------------------------------------------------------
	/// Elements insertation and removal. 
	/// ------------------------------------------------------------------------------------------

	GD_VECTOR_TEMPLATE
	inline void GD_VECTOR_CLASS::InsertElementAt(size_t const Index, ElementType&& Element)
	{
		GD_ASSERT(Index <= this->Count, "Index is out of bounds");
		this->ReserveToSize(this->Count + 1);
		this->Count += 1;
		for (PtrMutableIterator Iterator = this->PtrEnd() - 1; Iterator != this->PtrBegin() + Index - 1; --Iterator) {
			*(Iterator - 1) = Move(*Iterator);
		}

		new (this->PtrBegin() + Index) ElementType(Forward<ElementType>(Element));
	}

	GD_VECTOR_TEMPLATE
	inline void GD_VECTOR_CLASS::InsertElementAt(size_t const Index, ElementType const& Element)
	{
		this->InsertElementAt(Index, Move(ElementType(Element)));
	}

	GD_VECTOR_TEMPLATE
	inline void GD_VECTOR_CLASS::RemoveElementAt(size_t const Index)
	{
		GD_ASSERT(Index < this->Count, "Index is out of bounds");
		for (PtrMutableIterator Iterator = this->PtrBegin() + Index; Iterator != (this->PtrEnd() - 1); ++Iterator) {
			*Iterator = Move(*(Iterator + 1));
		}

		this->Count -= 1;
		this->PtrEnd()->~ElementType();
	}

	GD_VECTOR_TEMPLATE
	inline void GD_VECTOR_CLASS::PushLast(ElementType&& NewElement /* = ElementType() */)
	{
		this->ReserveToSize(this->Count + 1);
		new (this->PtrEnd()) ElementType(Forward<ElementType>(NewElement));
		this->Count += 1;
	}

	GD_VECTOR_TEMPLATE
	inline void GD_VECTOR_CLASS::PushLast(ElementType const& NewElement)
	{
		this->PushLast(Move(ElementType(NewElement)));
	}

	GD_VECTOR_TEMPLATE
	inline void GD_VECTOR_CLASS::PopLast()
	{
		GD_ASSERT((this->Count != 0), "Container size is zero");
		this->Resize(this->Count - 1);
	}

	/// ------------------------------------------------------------------------------------------
	/// Searching of elements.
	/// ------------------------------------------------------------------------------------------

	GD_VECTOR_TEMPLATE template<typename SearchingPredicateType>
	inline size_t GD_VECTOR_CLASS::FindFirstElement(SearchingPredicateType const& SearchingPredicate) const
	{
		for (PtrConstIterator Iterator = this->PtrBegin(); Iterator != this->PtrEnd(); ++Iterator) {
			if (SearchingPredicate(*Iterator)) {
				return (Iterator - this->PtrBegin());
			}
		}

		return SIZE_MAX;
	}

	GD_VECTOR_TEMPLATE template<typename SearchingPredicateType>
	inline size_t GD_VECTOR_CLASS::FindLastElement(SearchingPredicateType const& SearchingPredicate) const
	{
		for (ReversePtrConstIterator Iterator = this->ReversePtrBegin(); Iterator != this->ReversePtrEnd(); ++Iterator) {
			if (SearchingPredicate(*Iterator)) {
				return (Iterator - this->ReversePtrEnd());
			}
		}

		return SIZE_MAX;
	}

	GD_VECTOR_TEMPLATE
	inline size_t GD_VECTOR_CLASS::FindFirstElement(ElementType const& Element) const
	{	
		return this->FindFirstElement([&Element](ElementType const& OtherElement) {
			return (OtherElement == Element);
		});
	}

	GD_VECTOR_TEMPLATE
	inline size_t GD_VECTOR_CLASS::FindLastElement(ElementType const& Element) const
	{	
		return this->FindFirstElement([&Element](ElementType const& OtherElement) {
			return (OtherElement == Element);
		});
	}

	GD_VECTOR_TEMPLATE template<typename SearchingPredicateType>
	inline GD_VECTOR_CLASS GD_VECTOR_CLASS::FindAllElements(SearchingPredicateType const& SearchingPredicate) const
	{
		ThisVectorType SearchingResult;
		for (PtrConstIterator Iterator = this->PtrBegin(); Iterator != this->PtrEnd(); ++Iterator) {
			if (SearchingPredicate(*Iterator)) {
				SearchingResult.PushLast(*Iterator);
			}
		}

		return SearchingResult;
	}

	/// ------------------------------------------------------------------------------------------
	/// Sorting elements.
	/// ------------------------------------------------------------------------------------------

	GD_VECTOR_TEMPLATE template<typename SortingPredicateType>
	inline void GD_VECTOR_CLASS::Sort(SortingPredicateType const& SortingPredicate, MutableIterator const _Left, MutableIterator const _Right)
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

	GD_VECTOR_TEMPLATE template<typename SortingPredicateType>
	inline void GD_VECTOR_CLASS::Sort(SortingPredicateType const& SortingPredicate)
	{	// Sorting all container range.
		this->Sort(SortingPredicate, this->Begin(), (this->End() - 1));
	}

	GD_VECTOR_TEMPLATE
	inline void GD_VECTOR_CLASS::Sort()
	{	// Sorting all container range with default predicate.
		this->Sort([](ElementType const& Right, ElementType const& Left) {
			return (Right < Left);
		});
	}

	/// ------------------------------------------------------------------------------------------
	/// Assigment operators.
	/// ------------------------------------------------------------------------------------------

	GD_VECTOR_TEMPLATE
	inline GD_VECTOR_CLASS& GD_VECTOR_CLASS::operator= (Vector&& OtherVector)
	{	
		if ((&OtherVector) != this) {	// Check if we are not assigning container itself.
			this->Clear();
			this->MemoryProviderInstance = Move(OtherVector.MemoryProviderInstance);
			this->Count	= OtherVector.Count;
			OtherVector.Count = 0;
		}

		return (*this);
	}

	GD_VECTOR_TEMPLATE
	inline GD_VECTOR_CLASS& GD_VECTOR_CLASS::operator= (Vector const& OtherVector)
	{	
		if ((&OtherVector) != this) {	// Check if we are not assigning container itself.
			if (this->GetCapacity() >= OtherVector.GetSize()) {	// We have enough place here, so it is optimal to move is here without realloc.
				this->Emptify();
				this->Count = OtherVector.Count;
				Vector::CopyFromRange(OtherVector.PtrBegin(), OtherVector.PtrEnd(), this->PtrBegin());
			} else {	// Not enough memory. Copying specified vector and moving it here.
				(*this) = Move(Vector(OtherVector));
			}
		}

		return (*this);
	}

	GD_VECTOR_TEMPLATE
	inline GD_VECTOR_CLASS& GD_VECTOR_CLASS::operator= (std::initializer_list<ElementType> const& InitializerList)
	{	
		if (this->GetCapacity() >= InitializerList.size()) {	// We have enough place here, so it is optimal to move is here without realloc.
			this->Emptify();
			this->Count = InitializerList.size();
			Vector::CopyFromRange(InitializerList.begin(), InitializerList.end(), this->PtrBegin());
		} else {	// Not enough memory. Assigning itself a container created with specified initializer list.
			(*this) = Move(Vector(InitializerList));
		}

		return (*this);
	}

#if (defined(__cplusplus_cli))
	GD_VECTOR_TEMPLATE
	inline GD_VECTOR_CLASS& GD_VECTOR_CLASS::operator= (array<ElementType>^ CliArray)
	{
		if (this->GetCapacity() >= static_cast<size_t>(CliArray->Length)) {	// We have enough place here, so it is optimal to move is here without realloc.
			this->Emptify();
			this->Count = static_cast<size_t>(CliArray->Length);
			pin_ptr<ElementType> CliArrayPtr = &CliArray[0];
			PtrMutableIterator PtrCliArrayIteratorStart = CliArrayPtr;
			Vector::CopyFromRange(PtrCliArrayIteratorStart, PtrCliArrayIteratorStart + this->Count, this->PtrBegin());
		} else {	// Not enough memory. Assigning itself a container created with CLI/C++ Array.
			(*this) = Move(Vector(CliArray));
		}

		return (*this);
	}
#endif	// if (defined(__cplusplus_cli))

	/// ------------------------------------------------------------------------------------------
	/// Comparasion operators.
	/// ------------------------------------------------------------------------------------------

	GD_VECTOR_TEMPLATE
	inline bool GD_VECTOR_CLASS::operator== (Vector const& OtherVector) const
	{	// Vector with different sizes can't be equal. 
		if (this->Count == OtherVector.GetSize()) {
			return this->CompareTo([](ElementType const& First, ElementType const& Second) -> bool {
				return (First == Second);
			});
		} else {
			return false;
		}
	}

	GD_VECTOR_TEMPLATE
	inline bool GD_VECTOR_CLASS::operator!= (Vector const& OtherVector) const
	{	// Vector with same sizes can't be non-equal. 
		if (this->Count != OtherVector.GetSize()) {
			return this->CompareTo([](ElementType const& First, ElementType const& Second) -> bool {
				return (First != Second);
			});
		} else {
			return true;
		}
	}

	GD_VECTOR_TEMPLATE
	inline bool GD_VECTOR_CLASS::operator< (Vector const& OtherVector) const
	{
		return this->CompareTo([](ElementType const& First, ElementType const& Second) -> bool {
			return (First < Second);
		});
	}

	GD_VECTOR_TEMPLATE
	inline bool GD_VECTOR_CLASS::operator> (Vector const& OtherVector) const
	{
		return this->CompareTo([](ElementType const& First, ElementType const& Second) -> bool {
			return (First > Second);
		});
	}

	GD_VECTOR_TEMPLATE
	inline bool GD_VECTOR_CLASS::operator<= (Vector const& OtherVector) const
	{
		return this->CompareTo([](ElementType const& First, ElementType const& Second) -> bool {
			return (First <= Second);
		});
	}

	GD_VECTOR_TEMPLATE
	inline bool GD_VECTOR_CLASS::operator>= (Vector const& OtherVector) const
	{
		return this->CompareTo([](ElementType const& First, ElementType const& Second) -> bool {
			return (First >= Second);
		});
	}

	/// ------------------------------------------------------------------------------------------
	/// Appending operators.
	/// ------------------------------------------------------------------------------------------

	GD_VECTOR_TEMPLATE
	inline GD_VECTOR_CLASS& GD_VECTOR_CLASS::operator+= (Vector&& OtherVector)
	{
		this->ReserveToSize(this->Count + OtherVector.GetSize());
		for (MutableIterator Iterator = OtherVector.Begin(); Iterator != OtherVector.End(); ++Iterator) {
			this->PushLast(Move(*Iterator));
		}

		return (*this);
	}

	GD_VECTOR_TEMPLATE
	inline GD_VECTOR_CLASS& GD_VECTOR_CLASS::operator+= (Vector const& OtherVector)
	{
		return ((*this) += Move(Vector(OtherVector)));
	}

	GD_VECTOR_TEMPLATE
	inline GD_VECTOR_CLASS GD_VECTOR_CLASS::operator+ (Vector&& OtherVector) const
	{
		return (Vector(*this) += Forward<Vector>(OtherVector));
	}
	
	GD_VECTOR_TEMPLATE
	inline GD_VECTOR_CLASS GD_VECTOR_CLASS::operator+(Vector const& OtherVector) const
	{
		return ((*this) + Move(Vector(OtherVector)));
	}


	/// ------------------------------------------------------------------------------------------
	/// Access operators.
	/// ------------------------------------------------------------------------------------------

	GD_VECTOR_TEMPLATE
	inline ElementType const& GD_VECTOR_CLASS::operator[] (size_t const Index) const
	{	
		return this->GetElementAt(Index);
	}

	GD_VECTOR_TEMPLATE
	inline ElementType& GD_VECTOR_CLASS::operator[] (size_t const Index)
	{	
		return this->GetElementAt(Index);
	}

	/// ------------------------------------------------------------------------------------------
	/// Swap function.
	/// ------------------------------------------------------------------------------------------

	template<typename ElementType, typename MemoryProviderType>
	inline void Swap(Vector<ElementType, MemoryProviderType>& First, Vector<ElementType, MemoryProviderType>& Second)
	{
		if (&First != &Second) {
			Swap(First.MemoryProviderInstance, Second.MemoryProviderInstance);
			Swap(First.Count, Second.Count);
		}
	}

#undef GD_VECTOR_TEMPLATE
#undef GD_VECTOR_CLASS

GD_NAMESPACE_END
