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
#include <cstring>

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
			std::memcpy(&this->Memory[0], &OtherStackMemoryProvider.Memory[0], sizeof(this->Memory));
			std::memset(&OtherStackMemoryProvider.Memory[0], 0, sizeof(OtherStackMemoryProvider.Memory));
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

	/// ------------------------------------------------------------------------------------------
	/// Private methods.
	/// ------------------------------------------------------------------------------------------

	template<typename ElementType, typename MemoryProviderType> template<typename OtherSourceIteratorType, typename OtherDestinationIteratorType>
	inline void Vector<ElementType, MemoryProviderType>::CopyFromRange(OtherSourceIteratorType const& StartSourceIterator, OtherSourceIteratorType const& EndSourceIterator,
		OtherDestinationIteratorType const& StartDestinationIterator)
	{
		for (OtherSourceIteratorType Iterator = StartSourceIterator; Iterator != EndSourceIterator; ++Iterator) {
			new (StartDestinationIterator + (Iterator - StartSourceIterator)) ElementType(*Iterator);
		}
	}

	template<typename ElementType, typename MemoryProviderType> template<typename OtherSourceIteratorType, typename OtherDestinationIteratorType>
	inline void Vector<ElementType, MemoryProviderType>::MoveFromRange(OtherSourceIteratorType const& StartSourceIterator, OtherSourceIteratorType const& EndSourceIterator,
		OtherDestinationIteratorType const& StartDestinationIterator)
	{
		for (OtherSourceIteratorType Iterator = StartSourceIterator; Iterator != EndSourceIterator; ++Iterator) {
			new (StartDestinationIterator + (Iterator - StartSourceIterator)) ElementType(Forward<ElementType>(*Iterator));
		}
	}

	template<typename ElementType, typename MemoryProviderType>
	inline void Vector<ElementType, MemoryProviderType>::InitializeRange(PtrMutableIterator const StartIterator, PtrMutableIterator const EndIterator)
	{
		for (PtrMutableIterator Iterator = StartIterator; Iterator != EndIterator; ++Iterator) {
			new (Iterator) ElementType();
		}
	}

	template<typename ElementType, typename MemoryProviderType>
	inline void Vector<ElementType, MemoryProviderType>::DeinitializeRange(PtrMutableIterator const StartIterator, PtrMutableIterator const EndIterator)
	{
		for (PtrMutableIterator Iterator = StartIterator; Iterator != EndIterator; ++Iterator) {
			Iterator->~ElementType();
		}
	}

	template<typename ElementType, typename MemoryProviderType> template<typename ComparasionPredicateType>
	inline bool Vector<ElementType, MemoryProviderType>::CompareTo(Vector const& OtherVector, ComparasionPredicateType const& ComparasionPredicate)
	{
		size_t const MinCount = Min(this->Length, OtherVector.GetLength());
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

	template<typename ElementType, typename MemoryProviderType>
	inline Vector<ElementType, MemoryProviderType>::Vector(size_t const InitialElemntsCount /* = 0 */, size_t const Capacity /* = SIZE_MAX */)
		: MemoryProviderInstance(((Capacity != SIZE_MAX) ? Capacity : InitialElemntsCount) * sizeof(ElementType))
	{
		this->Length = InitialElemntsCount;
		Vector::InitializeRange(this->PtrBegin(), this->PtrEnd());
	}

	template<typename ElementType, typename MemoryProviderType>
	inline Vector<ElementType, MemoryProviderType>::Vector(ConstIterator const StartIterator, ConstIterator const EndIterator)
		: MemoryProviderInstance(static_cast<size_t>(EndIterator - StartIterator))
	{
		GD_DEBUG_ASSERT((EndIterator >= StartIterator), "Invalid iterators specified.");
		this->Length = static_cast<size_t>(EndIterator - StartIterator);
		Vector::CopyFromRange(StartIterator, EndIterator, this->PtrBegin());
	}

	template<typename ElementType, typename MemoryProviderType>
	inline Vector<ElementType, MemoryProviderType>::Vector(InitializerList<ElementType> const& InitializerList)
		: MemoryProviderInstance(InitializerList.size())
	{
		this->Length = InitializerList.size();
		Vector::CopyFromRange(InitializerList.begin(), InitializerList.end(), this->PtrBegin());
	}

#if (defined(__cplusplus_cli))
	template<typename ElementType, typename MemoryProviderType>
	inline Vector<ElementType, MemoryProviderType>::Vector(array<ElementType>^ CliArray)
		: MemoryProviderInstance(static_cast<size_t>(CliArray->Length))
	{
		this->Length = static_cast<size_t>(CliArray->Length);
		pin_ptr<ElementType> CliArrayPtr = &CliArray[0];
		PtrMutableIterator PtrCliArrayIteratorStart = CliArrayPtr;
		Vector::CopyFromRange(PtrCliArrayIteratorStart, PtrCliArrayIteratorStart + this->Length, this->PtrBegin());
	}
#endif	// if (defined(__cplusplus_cli))

	template<typename ElementType, typename MemoryProviderType>
	inline Vector<ElementType, MemoryProviderType>::Vector(Vector const& OtherVector)
		: MemoryProviderInstance(OtherVector.GetCapacity())
	{
		this->Length = OtherVector.Count;
		Vector::CopyFromRange(OtherVector.PtrBegin(), OtherVector.PtrEnd(), this->PtrBegin());
	}

	template<typename ElementType, typename MemoryProviderType>
	inline Vector<ElementType, MemoryProviderType>::Vector(Vector&& OtherVector)
		: MemoryProviderInstance(Move(OtherVector.MemoryProviderInstance))
	{
		this->Length = OtherVector.Count;
		OtherVector.Count = 0;
	}

	template<typename ElementType, typename MemoryProviderType>
	GDINL Vector<ElementType, MemoryProviderType>::~Vector()
	{
		this->Clear();
	}

	/// ------------------------------------------------------------------------------------------
	/// Dynamic size management.
	/// ------------------------------------------------------------------------------------------

	template<typename ElementType, typename MemoryProviderType>
	GDINL size_t Vector<ElementType, MemoryProviderType>::GetLength() const
	{
		return this->Length;
	}

	template<typename ElementType, typename MemoryProviderType>
	GDINL size_t Vector<ElementType, MemoryProviderType>::GetCapacity() const
	{
		return this->MemoryProviderInstance.GetCapacity();
	}

	template<typename ElementType, typename MemoryProviderType>
	inline void Vector<ElementType, MemoryProviderType>::Resize(size_t const NewSize)
	{
		if (this->Length != NewSize) {	// We do not need to resize our element with same size.
			if (this->Length < NewSize) {	// Increasing size of container, we need to initialize new elements with template value.
				this->ReserveToSize(NewSize);
				Vector::InitializeRange(this->PtrEnd(), this->PtrBegin() + NewSize);
			} else {	// Decreasing size of container, we need to destroy last elements there.
				Vector::DeinitializeRange(this->PtrBegin() + NewSize, this->PtrEnd());
			}

			this->Length = NewSize;
		}
	}

	template<typename ElementType, typename MemoryProviderType>
	inline void Vector<ElementType, MemoryProviderType>::Reserve(size_t const NewCapacity)
	{
		// Moving elements into other memory block. Note that elements outside range keep uninitialized.
		if (MemoryProviderType::MaxCapacity == VectorMemoryProviders::FlexibleMaxCapacity) {
			if (this->GetCapacity() != NewCapacity)	{
				// Clamping container size to capacity.
				if (NewCapacity < this->Length) {
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

	template<typename ElementType, typename MemoryProviderType>
	inline void Vector<ElementType, MemoryProviderType>::ReserveToSize(size_t const NewSize)
	{
		if (MemoryProviderType::MaxCapacity == VectorMemoryProviders::FlexibleMaxCapacity) {
			if (NewSize > this->GetCapacity()) {
				this->Reserve(Max(this->GetCapacity(), NewSize) * 13 / 10);
			}
		}
	}

	template<typename ElementType, typename MemoryProviderType>
	GDINL void Vector<ElementType, MemoryProviderType>::ShrinkToFit()
	{
		this->Reserve(this->Length);
	}

	template<typename ElementType, typename MemoryProviderType>
	GDINL void Vector<ElementType, MemoryProviderType>::Emptify()
	{
		this->Resize(0);
	}

	template<typename ElementType, typename MemoryProviderType>
	GDINL void Vector<ElementType, MemoryProviderType>::Clear()
	{
		this->Reserve(0);
	}

	/// ------------------------------------------------------------------------------------------
	/// Elements access.
	/// ------------------------------------------------------------------------------------------

	template<typename ElementType, typename MemoryProviderType>
	inline ElementType const& Vector<ElementType, MemoryProviderType>::GetElementAt(size_t const Index) const
	{
		GD_ASSERT(Index < this->Length, "Index is out of bounds");
		return (*(this->PtrBegin() + Index));
	}

	template<typename ElementType, typename MemoryProviderType>
	inline ElementType& Vector<ElementType, MemoryProviderType>::GetElementAt(size_t const Index)
	{
		GD_ASSERT(Index < this->Length, "Index is is out of bounds");
		return (*(this->PtrBegin() + Index));
	}

	template<typename ElementType, typename MemoryProviderType>
	GDINL ElementType const& Vector<ElementType, MemoryProviderType>::GetFirstElement() const
	{
		return this->GetElementAt(0);
	}

	template<typename ElementType, typename MemoryProviderType>
	GDINL ElementType& Vector<ElementType, MemoryProviderType>::GetFirstElement()
	{
		return this->GetElementAt(0);
	}

	template<typename ElementType, typename MemoryProviderType>
	GDINL ElementType const& Vector<ElementType, MemoryProviderType>::GetLastElement() const
	{
		return this->GetElementAt(this->Length - 1);
	}

	template<typename ElementType, typename MemoryProviderType>
	GDINL ElementType& Vector<ElementType, MemoryProviderType>::GetLastElement()
	{
		return this->GetElementAt(this->Length - 1);
	}

	template<typename ElementType, typename MemoryProviderType>
	inline void Vector<ElementType, MemoryProviderType>::SetElementAt(size_t const Index, ElementType&& Element)
	{
		ElementType& ExistingElement = this->GetElementAt(Index);
		ExistingElement.~ElementType();
		new (&ExistingElement) ElementType(Forward<ElementType>(Element));
	}

	template<typename ElementType, typename MemoryProviderType>
	inline void Vector<ElementType, MemoryProviderType>::SetElementAt(size_t const Index, ElementType const& Element)
	{
		this->SetElementAt(Move(ElementType(Element)));
	}

	/// ------------------------------------------------------------------------------------------
	/// Elements insertion and removal. 
	/// ------------------------------------------------------------------------------------------

	template<typename ElementType, typename MemoryProviderType>
	inline void Vector<ElementType, MemoryProviderType>::InsertElementAt(size_t const Index, ElementType&& Element)
	{
		GD_ASSERT(Index <= this->Length, "Index is out of bounds");
		this->ReserveToSize(this->Length + 1);
		this->Length += 1;
		for (PtrMutableIterator Iterator = this->PtrEnd() - 1; Iterator != this->PtrBegin() + Index - 1; --Iterator) {
			*(Iterator - 1) = Move(*Iterator);
		}

		new (this->PtrBegin() + Index) ElementType(Forward<ElementType>(Element));
	}

	template<typename ElementType, typename MemoryProviderType>
	inline void Vector<ElementType, MemoryProviderType>::InsertElementAt(size_t const Index, ElementType const& Element)
	{
		this->InsertElementAt(Index, Move(ElementType(Element)));
	}

	template<typename ElementType, typename MemoryProviderType>
	inline void Vector<ElementType, MemoryProviderType>::RemoveElementAt(size_t const Index)
	{
		GD_ASSERT(Index < this->Length, "Index is out of bounds");
		for (PtrMutableIterator Iterator = this->PtrBegin() + Index; Iterator != (this->PtrEnd() - 1); ++Iterator) {
			*Iterator = Move(*(Iterator + 1));
		}

		this->Length -= 1;
		this->PtrEnd()->~ElementType();
	}

	template<typename ElementType, typename MemoryProviderType>
	inline void Vector<ElementType, MemoryProviderType>::PushLast(ElementType&& NewElement /* = ElementType() */)
	{
		this->ReserveToSize(this->Length + 1);
		new (this->PtrEnd()) ElementType(Forward<ElementType>(NewElement));
		this->Length += 1;
	}

	template<typename ElementType, typename MemoryProviderType>
	inline void Vector<ElementType, MemoryProviderType>::PushLast(ElementType const& NewElement)
	{
		this->PushLast(Move(ElementType(NewElement)));
	}

	template<typename ElementType, typename MemoryProviderType>
	inline void Vector<ElementType, MemoryProviderType>::PopLast()
	{
		GD_ASSERT((this->Length != 0), "Container size is zero");
		this->Resize(this->Length - 1);
	}

	/// ------------------------------------------------------------------------------------------
	/// Searching of elements.
	/// ------------------------------------------------------------------------------------------

	template<typename ElementType, typename MemoryProviderType> template<typename SearchingPredicateType>
	inline size_t Vector<ElementType, MemoryProviderType>::FindFirstElement(SearchingPredicateType const& SearchingPredicate) const
	{
		for (PtrConstIterator Iterator = this->PtrBegin(); Iterator != this->PtrEnd(); ++Iterator) {
			if (SearchingPredicate(*Iterator)) {
				return (Iterator - this->PtrBegin());
			}
		}

		return SIZE_MAX;
	}

	template<typename ElementType, typename MemoryProviderType> template<typename SearchingPredicateType>
	inline size_t Vector<ElementType, MemoryProviderType>::FindLastElement(SearchingPredicateType const& SearchingPredicate) const
	{
		for (PtrConstIterator Iterator = (this->PtrEnd() - 1); Iterator != (this->PtrBegin() - 1); --Iterator) {
			if (SearchingPredicate(*Iterator)) {
				return (Iterator - this->ReversePtrEnd());
			}
		}

		return SIZE_MAX;
	}

	template<typename ElementType, typename MemoryProviderType>
	inline size_t Vector<ElementType, MemoryProviderType>::FindFirstElement(ElementType const& Element) const
	{
		return this->FindFirstElement([&Element](ElementType const& OtherElement) {
			return (OtherElement == Element);
		});
	}

	template<typename ElementType, typename MemoryProviderType>
	inline size_t Vector<ElementType, MemoryProviderType>::FindLastElement(ElementType const& Element) const
	{
		return this->FindFirstElement([&Element](ElementType const& OtherElement) {
			return (OtherElement == Element);
		});
	}

	template<typename ElementType, typename MemoryProviderType> template<typename SearchingPredicateType>
	inline Vector<ElementType, MemoryProviderType> Vector<ElementType, MemoryProviderType>::FindAllElements(SearchingPredicateType const& SearchingPredicate) const
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

	template<typename ElementType, typename MemoryProviderType> template<typename SortingPredicateType>
	inline void Vector<ElementType, MemoryProviderType>::Sort(SortingPredicateType const& SortingPredicate, MutableIterator const _Left, MutableIterator const _Right)
	{	// Sorting elements with quick sort implementation, provided here http://anandpandia.blogspot.ru/2012/08/quicksort-algorithm-and-example-in-c.html
		/// @todo Implementation is a little broken if container includes equal values.
		MutableIterator Left = _Left, Right = _Right;
		MutableIterator const Pivot = this->Begin() + (((Left - this->Begin()) + (Right - this->Begin())) / 2);
		while (Left <= Right) {
			while (SortingPredicate((*Left), (*Pivot))) Left += 1;
			while (SortingPredicate((*Pivot), (*Right))) Right -= 1;
			if (Left <= Right) {
				Swap((*Left), (*Right));
				Left += 1;
				Right -= 1;
			}
		}

		if (_Left < Right) this->Sort(SortingPredicate, _Left, Right);
		if (Left < _Right) this->Sort(SortingPredicate, Left, _Right);
	}

	template<typename ElementType, typename MemoryProviderType> template<typename SortingPredicateType>
	inline void Vector<ElementType, MemoryProviderType>::Sort(SortingPredicateType const& SortingPredicate)
	{	// Sorting all container range.
		this->Sort(SortingPredicate, this->Begin(), (this->End() - 1));
	}

	template<typename ElementType, typename MemoryProviderType>
	inline void Vector<ElementType, MemoryProviderType>::Sort()
	{	// Sorting all container range with default predicate.
		this->Sort([](ElementType const& Right, ElementType const& Left) {
			return (Right < Left);
		});
	}

	/// ------------------------------------------------------------------------------------------
	/// Assignment operators.
	/// ------------------------------------------------------------------------------------------

	template<typename ElementType, typename MemoryProviderType>
	inline Vector<ElementType, MemoryProviderType>& Vector<ElementType, MemoryProviderType>::operator= (Vector&& OtherVector)
	{
		if ((&OtherVector) != this) {	// Check if we are not assigning container itself.
			this->Clear();
			this->MemoryProviderInstance = Move(OtherVector.MemoryProviderInstance);
			this->Length = OtherVector.Count;
			OtherVector.Count = 0;
		}

		return (*this);
	}

	template<typename ElementType, typename MemoryProviderType>
	inline Vector<ElementType, MemoryProviderType>& Vector<ElementType, MemoryProviderType>::operator= (Vector const& OtherVector)
	{
		if ((&OtherVector) != this) {	// Check if we are not assigning container itself.
			if (this->GetCapacity() >= OtherVector.GetLength()) {	// We have enough place here, so it is optimal to move is here without realloc.
				this->Emptify();
				this->Length = OtherVector.Count;
				Vector::CopyFromRange(OtherVector.PtrBegin(), OtherVector.PtrEnd(), this->PtrBegin());
			} else {	// Not enough memory. Copying specified vector and moving it here.
				(*this) = Move(Vector(OtherVector));
			}
		}

		return (*this);
	}

	template<typename ElementType, typename MemoryProviderType>
	inline Vector<ElementType, MemoryProviderType>& Vector<ElementType, MemoryProviderType>::operator= (InitializerList<ElementType> const& InitializerList)
	{
		if (this->GetCapacity() >= InitializerList.size()) {	// We have enough place here, so it is optimal to move is here without realloc.
			this->Emptify();
			this->Length = InitializerList.size();
			Vector::CopyFromRange(InitializerList.begin(), InitializerList.end(), this->PtrBegin());
		} else {	// Not enough memory. Assigning itself a container created with specified initializer list.
			(*this) = Move(Vector(InitializerList));
		}

		return (*this);
	}

#if (defined(__cplusplus_cli))
	template<typename ElementType, typename MemoryProviderType>
	inline Vector<ElementType, MemoryProviderType>& Vector<ElementType, MemoryProviderType>::operator= (array<ElementType>^ CliArray)
	{
		if (this->GetCapacity() >= static_cast<size_t>(CliArray->Length)) {	// We have enough place here, so it is optimal to move is here without realloc.
			this->Emptify();
			this->Length = static_cast<size_t>(CliArray->Length);
			pin_ptr<ElementType> CliArrayPtr = &CliArray[0];
			PtrMutableIterator PtrCliArrayIteratorStart = CliArrayPtr;
			Vector::CopyFromRange(PtrCliArrayIteratorStart, PtrCliArrayIteratorStart + this->Length, this->PtrBegin());
		} else {	// Not enough memory. Assigning itself a container created with CLI/C++ Array.
			(*this) = Move(Vector(CliArray));
		}

		return (*this);
	}
#endif	// if (defined(__cplusplus_cli))

	/// ------------------------------------------------------------------------------------------
	/// Comparison operators.
	/// ------------------------------------------------------------------------------------------

	template<typename ElementType, typename MemoryProviderType>
	inline bool Vector<ElementType, MemoryProviderType>::operator== (Vector const& OtherVector) const
	{	// Vector with different sizes can't be equal. 
		if (this->Length == OtherVector.GetLength()) {
			return this->CompareTo([](ElementType const& First, ElementType const& Second) -> bool {
				return (First == Second);
			});
		} else {
			return false;
		}
	}

	template<typename ElementType, typename MemoryProviderType>
	inline bool Vector<ElementType, MemoryProviderType>::operator!= (Vector const& OtherVector) const
	{	// Vector with same sizes can't be non-equal. 
		if (this->Length != OtherVector.GetLength()) {
			return this->CompareTo([](ElementType const& First, ElementType const& Second) -> bool {
				return (First != Second);
			});
		} else {
			return true;
		}
	}

	template<typename ElementType, typename MemoryProviderType>
	inline bool Vector<ElementType, MemoryProviderType>::operator< (Vector const& OtherVector) const
	{
		return this->CompareTo([](ElementType const& First, ElementType const& Second) -> bool {
			return (First < Second);
		});
	}

	template<typename ElementType, typename MemoryProviderType>
	inline bool Vector<ElementType, MemoryProviderType>::operator>(Vector const& OtherVector) const
	{
		return this->CompareTo([](ElementType const& First, ElementType const& Second) -> bool {
			return (First > Second);
		});
	}

	template<typename ElementType, typename MemoryProviderType>
	inline bool Vector<ElementType, MemoryProviderType>::operator<= (Vector const& OtherVector) const
	{
		return this->CompareTo([](ElementType const& First, ElementType const& Second) -> bool {
			return (First <= Second);
		});
	}

	template<typename ElementType, typename MemoryProviderType>
	inline bool Vector<ElementType, MemoryProviderType>::operator>= (Vector const& OtherVector) const
	{
		return this->CompareTo([](ElementType const& First, ElementType const& Second) -> bool {
			return (First >= Second);
		});
	}

	/// ------------------------------------------------------------------------------------------
	/// Appending operators.
	/// ------------------------------------------------------------------------------------------

	template<typename ElementType, typename MemoryProviderType>
	inline Vector<ElementType, MemoryProviderType>& Vector<ElementType, MemoryProviderType>::operator+= (Vector&& OtherVector)
	{
		this->ReserveToSize(this->Length + OtherVector.GetLength());
		for (MutableIterator Iterator = OtherVector.Begin(); Iterator != OtherVector.End(); ++Iterator) {
			this->PushLast(Move(*Iterator));
		}

		return (*this);
	}

	template<typename ElementType, typename MemoryProviderType>
	inline Vector<ElementType, MemoryProviderType>& Vector<ElementType, MemoryProviderType>::operator+= (Vector const& OtherVector)
	{
		return ((*this) += Move(Vector(OtherVector)));
	}

	template<typename ElementType, typename MemoryProviderType>
	inline Vector<ElementType, MemoryProviderType> Vector<ElementType, MemoryProviderType>::operator+ (Vector&& OtherVector) const
	{
		return (Vector(*this) += Forward<Vector>(OtherVector));
	}

	template<typename ElementType, typename MemoryProviderType>
	inline Vector<ElementType, MemoryProviderType> Vector<ElementType, MemoryProviderType>::operator+ (Vector const& OtherVector) const
	{
		return ((*this) + Move(Vector(OtherVector)));
	}

	/// ------------------------------------------------------------------------------------------
	/// Access operators.
	/// ------------------------------------------------------------------------------------------

	template<typename ElementType, typename MemoryProviderType>
	GDINL ElementType const& Vector<ElementType, MemoryProviderType>::operator[] (size_t const Index) const
	{
		return this->GetElementAt(Index);
	}

	template<typename ElementType, typename MemoryProviderType>
	GDINL ElementType& Vector<ElementType, MemoryProviderType>::operator[] (size_t const Index)
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
			Swap(First.Length, Second.Length);
		}
	}

GD_NAMESPACE_END
