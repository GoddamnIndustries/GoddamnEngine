/// ==========================================================================================
/// Vector.h - Dynamically sized array class.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_CONTAINERS_VECTOR
#define GD_CORE_CONTAINERS_VECTOR

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/Containers.h>
#include <GoddamnEngine/Core/Containers/InitializerList.h>

#include <new>
#include <cstring>

GD_NAMESPACE_BEGIN

	/// @brief Dynamic array implementation.
	template<typename ElementType>
	class Vector : public ContainerIteratableTag, public ContainerReverseIteratableTag, public ContainerPtrIteratableTag, public ContainerPtrReverseIteratableTag
	{
	public:
		typedef IndexedContainerIterator<Vector, ElementType> Iterator;
		typedef IndexedContainerIterator<Vector const, ElementType const> ConstIterator;
		GD_CONTAINER_CHECK_ITERATORS(Vector);

		typedef ReverseContainerIterator<Iterator> ReverseIterator;
		typedef ReverseContainerIterator<ConstIterator> ReverseConstIterator;
		GD_CONTAINER_CHECK_REVERSE_ITERATORS(Vector);

		typedef ElementType* PtrIterator;
		typedef ElementType const* PtrConstIterator;
		GD_CONTAINER_CHECK_PTR_ITERATORS(Vector);

		typedef ReverseContainerIterator<PtrIterator> ReversePtrIterator;
		typedef ReverseContainerIterator<PtrConstIterator> ReversePtrConstIterator;
		GD_CONTAINER_CHECK_REVERSE_PTR_ITERATORS(Vector);

		GD_CONTAINER_DEFINE_PTR_ITERATION_SUPPORT(Vector);

	private:
		ElementType* Memory = nullptr;
		size_t Length = 0;
		size_t Capacity = 0;

	public:

		/// @brief Initializes vector with specified number of allocated and initialized elements.
		/// @param InitialLength Number of elements been initialized.
		/// @param InitialCapacity Initial capacity of vector. 
		GDINL Vector(size_t const InitialLength = 0, size_t const InitialCapacity = SIZE_MAX)
		{
			this->Reserve(InitialCapacity != SIZE_MAX ? InitialCapacity : InitialLength);
			this->Resize(InitialLength);
			Vector::InitializeRange(this->PtrBegin(), this->PtrEnd());
		}

		/// @brief Initializes vector with copy of values of specified iterators. 
		/// @param StartIterator First iterator would be copied.
		/// @param EndIterator Last iterator would be copied.
		GDINL Vector(ConstIterator const StartIterator, ConstIterator const EndIterator)
			: Vector(static_cast<size_t>(EndIterator - StartIterator))
		{
			this->Resize(EndIterator - StartIterator);
			Vector::CopyFromRange(StartIterator, EndIterator, this->PtrBegin());
		}

		/// @brief Initializes vector with default C++11's initializer list. You should not use this constructor manually.
		/// @param InitializerList Initializer list passed by the compiler.
		GDINL Vector(InitializerList<ElementType> const& InitializerList)
		{
			this->Resize(InitializerList.size());
			Vector::CopyFromRange(InitializerList.begin(), InitializerList.end(), this->PtrBegin());
		}

		/// @brief Moves other vector here.
		/// @param OtherVector Vector would be moved into current object.
		GDINL Vector(Vector&& OtherVector)
		{
			this->Resize(OtherVector.Length);
			Vector::CopyFromRange(OtherVector.PtrBegin(), OtherVector.PtrEnd(), this->PtrBegin());
		}

		/// @brief Initializes vector with copy of other vector.
		/// @param OtherVector Vector would be copied.
		GDINL Vector(Vector const& OtherVector)
		{
			this->Memory = OtherVector.Memory;
			this->Length = OtherVector.Length;
			this->Capacity = OtherVector.Capacity;

			OtherVector.Memory = nullptr;
			OtherVector.Length = 0;
			OtherVector.Capacity = 0;
		}

		/// @brief Deinitializes all elements and deallocates memory.
		GDINL ~Vector()
		{
			this->Clear();
		}

	private:

		/// @brief Initializes value of the specified iterator.
		/// @param Iterator Value of this iterator would be initialized.
		GDINL static void InitializeIterator(PtrIterator const Iterator)
		{
			if (!TypeTraits::IsPodType<ElementType>::Value) {
				new (Iterator) ElementType();
			}
		}

		/// @brief Initializes value of the specified iterator.
		/// @param Iterator Value of this iterator would be initialized.
		/// @param Element Initial value of the iterator.
		/// @{
		GDINL static void InitializeIterator(PtrIterator const Iterator, ElementType&& Element)
		{
			new (Iterator) ElementType(Forward<ElementType>(Element));
		}
		GDINL static void InitializeIterator(PtrIterator const Iterator, ElementType const& Element)
		{
			new (Iterator) ElementType(Element);
		}
		/// @}

		/// @brief Initializes all values of the iterators in specified range.
		/// @param StartIterator Start of the iterators range.
		/// @param EndIterator End of the iterators range.
		GDINL static void InitializeRange(PtrIterator const StartIterator, PtrIterator const EndIterator)
		{
			if (!TypeTraits::IsPodType<ElementType>::Value) {
				for (PtrIterator Iterator = StartIterator; Iterator != EndIterator; ++Iterator) {
					Vector::InitializeIterator(Iterator);
				}
			}
		}

		/// @brief Deinitializes value of the specified iterator.
		/// @param Iterator Value of this iterator would be initialized.
		GDINL static void DeinitializeIterator(PtrIterator const Iterator)
		{
			if (!TypeTraits::IsPodType<ElementType>::Value) {
				Iterator->~ElementType();
			}
		}

		/// @brief Deinitializes all values of the iterators in specified range.
		/// @param StartIterator Start of the iterators range.
		/// @param EndIterator End of the iterators range.
		GDINL static void DeinitializeRange(PtrIterator const StartIterator, PtrIterator const EndIterator)
		{
			if (!TypeTraits::IsPodType<ElementType>::Value) {
				for (PtrIterator Iterator = StartIterator; Iterator != EndIterator; ++Iterator) {
					Iterator->~ElementType();
				}
			}
		}

		/// @brief Copies values of the iterators from source range to destination.
		/// @param StartSource Start of the source iterators range.
		/// @param EndSource End of the source iterators range.
		/// @param Destination First destination iterator.
		template<typename SourceIteratorType>
		GDINL static void CopyFromRange(SourceIteratorType const& StartSource, SourceIteratorType const& EndSource, PtrIterator Destination)
		{
			for (SourceIteratorType Iterator = StartSource; Iterator != EndSource; ++Iterator) {
				Vector::InitializeIterator(Destination + (Iterator - StartSource), *Iterator);
			}
		}

		/// @brief Moves values of the iterators from source range to destination.
		/// @param StartSource Start of the source iterators range.
		/// @param EndSource End of the source iterators range.
		/// @param Destination First destination iterator.
		template<typename SourceIteratorType>
		GDINL static void MoveFromRange(SourceIteratorType const& StartSource, SourceIteratorType const& EndSource, PtrIterator Destination)
		{
			for (SourceIteratorType Iterator = StartSource; Iterator != EndSource; ++Iterator) {
				Vector::InitializeIterator(Destination + (Iterator - StartSource), Forward<ElementType>(*Iterator));
			}
		}

		/// @brief Compares this container and some other by a predicate.
		/// @param OtherVector Other container against which we are comparing.
		/// @param Predicate Object with () operator overloaded that takes two elements and compares then somehow.
		template<typename ComparingPredicateType>
		GDINL bool CompareTo(Vector const& OtherVector, ComparingPredicateType const& Predicate)
		{
			size_t const MinLength = Min(this->Length, OtherVector.Length);
			for (size_t Index = 0; Index < MinLength; ++Index) {
				if (!ComparingPredicate(*(this->PtrBegin() + Index), *(OtherVector.PtrBegin() + Index))) {
					return false;
				}
			}

			return true;
		}

	private:

		/// @brief Returns pointer iterator that points to first container element.
		/// @returns Simple pointer iterator that points to first container element.
		/// @{
		GDINL PtrIterator PtrBegin()
		{
			return reinterpret_cast<PtrIterator>(this->Memory);
		}
		GDINL PtrConstIterator PtrBegin() const
		{
			return reinterpret_cast<PtrConstIterator>(this->Memory);
		}
		/// @}

		/// @brief Returns pointer iterator that points to past the end container element.
		/// @returns Simple pointer iterator that points to past the end container element.
		/// @{
		GDINL PtrIterator PtrEnd()
		{
			return this->PtrBegin() + this->Length;
		}
		GDINL PtrConstIterator PtrEnd() const
		{
			return this->PtrBegin() + this->Length;
		}
		/// @}

		/// @brief Returns pointer iterator that points to last container element.
		/// @returns Simple pointer iterator that points to last container element.
		/// @{
		GDINL ReversePtrIterator ReversePtrBegin()
		{
			return ReversePtrIterator(this->PtrEnd() - 1);
		}
		GDINL ReversePtrConstIterator ReversePtrBegin() const
		{
			return ReversePtrConstIterator(this->PtrEnd() - 1);
		}
		/// @}

		/// @brief Returns pointer iterator that points to preceding the first container element.
		/// @returns Simple pointer iterator that points to preceding the first container element.
		/// @{
		GDINL ReversePtrIterator ReversePtrEnd()
		{
			return ReversePtrIterator(this->PtrBegin() - 1);
		}
		GDINL ReversePtrConstIterator ReversePtrEnd() const
		{
			return ReversePtrConstIterator(this->PtrBegin() - 1);
		}
		/// @}

	public:

		/// @brief Returns iterator that points to first container element.
		/// @returns Iterator that points to first container element.
		/// @{
		GDINL Iterator Begin()
		{
			return Iterator(*this);
		}
		GDINL ConstIterator Begin() const
		{
			return ConstIterator(*this);
		}
		/// @}

		/// @brief Returns iterator that points to past the end container element.
		/// @returns Iterator that points to past the end container element.
		/// @{
		GDINL Iterator End()
		{
			return this->Begin() + this->Length;
		}
		GDINL ConstIterator End() const
		{
			return this->Begin() + this->Length;
		}
		/// @}

		/// @brief Returns iterator that points to last container element.
		/// @returns Iterator that points to last container element.
		/// @{
		GDINL ReverseIterator ReverseBegin()
		{
			return ReverseContainerIterator(this->End() - 1);
		}
		GDINL ReverseConstIterator ReverseBegin() const
		{
			return ReverseConstIterator(this->End() - 1);
		}
		/// @}

		/// @brief Returns iterator that points to preceding the first container element.
		/// @returns Iterator that points to preceding the first container element.
		/// @{
		GDINL ReverseIterator ReverseEnd()
		{
			return ReverseContainerIterator(this->Begin() - 1);
		}
		GDINL ReverseConstIterator ReverseEnd() const
		{
			return ReverseConstIterator(this->Begin() - 1);
		}
		/// @}

	public:

		/// @brief Returns number of elements that exist in vector.
		/// @returns Number of elements that exist in vector.
		GDINL size_t GetLength() const
		{
			return this->Length;
		}

		/// @brief Returns number of elements that can be placed into vector without reallocation.
		/// @returns Number of elements that can be placed into vector without reallocation.
		GDINL size_t GetCapacity() const
		{
			return this->Capacity;
		}

		/// @brief Returns true if this container is empty.
		/// @returns True if this container is empty, false otherwise.
		GDINL bool IsEmpty() const
		{
			return (this->Length == 0);
		}

		/// @brief Resizes vector to make it able to contain specified number of elements.
		/// @param NewLength New required length of the container.
		GDINL void Resize(size_t const NewLength)
		{
			if (this->Length != NewLength) {	// We do not need to resize our element with same size.
				if (this->Length < NewLength) {	// Increasing size of container, we need to initialize new elements.
					this->ReserveToLength(NewLength);
					Vector::InitializeRange(this->PtrEnd(), this->PtrBegin() + NewLength);
				} else {	// Decreasing size of container, we need to destroy last elements there.
					Vector::DeinitializeRange(this->PtrBegin() + NewLength, this->PtrEnd());
				}

				this->Length = NewLength;
			}
		}

		/// @brief Reserves memory for vector to make it contain specified number of elements without reallocation when calling Resize/PushBack/Insert method.
		/// @param NewCapacity New required capacity of the container.
		GDINL void Reserve(size_t const NewCapacity)
		{
			// Moving elements into other memory block. Note that elements outside range keep uninitialized.
			if (this->Capacity != NewCapacity)	{
				// Clamping container size to capacity.
				if (NewCapacity < this->Length) {
					this->Resize(NewCapacity);
				}

				ElementType* NewMemory = reinterpret_cast<ElementType*>(Allocator::AllocateMemory(NewCapacity * sizeof(ElementType)));
				Vector::MoveFromRange(this->PtrBegin(), this->PtrEnd(), reinterpret_cast<PtrIterator>(NewMemory));
				Vector::DeinitializeRange(this->PtrBegin(), this->PtrEnd());
				this->Memory = NewMemory;
			}
		}

		/// @brief Reserves memory for vector to make it contain best fitting number of elements to predicted new size. This function incrementally grows capacity in 1.3 times.
		/// @param NewLength New desired length on which best capacity is computed.
		GDINL void ReserveToLength(size_t const NewLength)
		{
			if (NewLength > this->Capacity) {
				this->Reserve(static_cast<size_t>(1.3 * static_cast<double>(Max(this->Capacity, NewLength))));
			}
		}

		/// @brief Shrinks vector's capacity to length.
		GDINL void ShrinkToFit()
		{
			this->Reserve(this->Length);
		}

		/// @brief Destroys all elements in container without memory deallocation.
		GDINL void Emptify()
		{
			this->Resize(0);
		}

		/// @brief Destroys all elements in container with memory deallocation.
		GDINL void Clear()
		{
			this->Reserve(0);
		}

	public:

		/// @brief Returns reference on last element in container.
		/// @returns Reference on last element in the container.
		/// @{
		GDINL ElementType const& GetLastElement() const
		{
			return this->GetElementAt(this->Length - 1);
		}
		GDINL ElementType& GetLastElement()
		{
			return this->GetElementAt(this->Length - 1);
		}
		/// @}

		/// @brief Inserts specified element into collection at desired index.
		/// @param Index At this index new element would be inserted. All existing elements would be shifted to right.
		/// @param Element New element that would be inserted.
		/// @{
		GDINL void InsertElementAt(size_t const Index, ElementType&& Element)
		{
			GD_ASSERT(Index <= this->Length, "Index is out of bounds");
			this->ReserveToLength(this->Length + 1);
			for (PtrIterator Iterator = this->PtrEnd(); Iterator != this->PtrBegin() + Index - 1; --Iterator) {
				*(Iterator - 1) = Move(*Iterator);
			}

			this->Length += 1;
			Vector::InitializeIterator(this->PtrBegin() + Index, Forward<ElementType>(Element));
		}
		GDINL void InsertElementAt(size_t const Index, ElementType const& Element)
		{
			GD_ASSERT(Index <= this->Length, "Index is out of bounds");
			this->ReserveToLength(this->Length + 1);
			for (PtrIterator Iterator = this->PtrEnd(); Iterator != this->PtrBegin() + Index - 1; --Iterator) {
				*(Iterator - 1) = *Iterator;
			}

			this->Length += 1;
			Vector::InitializeIterator(this->PtrBegin() + Index, Element);
		}
		/// @}

		/// @brief Removes existing element from array at specified index.
		/// @param Index Element at this index would be removed. All other elements would be shifted to left.
		GDINL void RemoveElementAt(size_t const Index)
		{
			GD_ASSERT(Index < this->Length, "Index is out of bounds");
			for (PtrIterator Iterator = this->PtrBegin() + Index; Iterator != (this->PtrEnd() - 1); ++Iterator) {
				*Iterator = Move(*(Iterator + 1));
			}

			this->Length -= 1;
			Vector::DeinitializeIterator(this->PtrEnd());
		}

		/// @brief Appends new element to container.
		/// @param NewElement New element that would be inserted into the end of container.
		/// @{
		GDINL void PushLast(ElementType&& NewElement = ElementType())
		{
			this->ReserveToLength(this->Length + 1);
			this->Length += 1;
			Vector::InitializeIterator(this->PtrEnd() - 1, Forward<ElementType>(NewElement));
		}
		GDINL void PushLast(ElementType const& NewElement)
		{
			this->ReserveToLength(this->Length + 1);
			this->Length += 1;
			Vector::InitializeIterator(this->PtrEnd() - 1, NewElement);
		}
		/// @}

		/// @brief Removes last element from container.
		GDINL void PopLast()
		{
			GD_ASSERT((this->Length != 0), "Container size is zero");
			this->Resize(this->Length - 1);
		}

	public:

		/// @brief Searches for first element in container by predicate.
		/// @param Predicate Object with () operator overloaded that returns true if both specified objects are equal.
		/// @returns Index of found element or @c SIZE_MAX if nothing was found.
		template<typename SearchingPredicateType>
		GDINL size_t FindFirstElement(SearchingPredicateType const& Predicate) const
		{
			for (PtrConstIterator Iterator = this->PtrBegin(); Iterator != this->PtrEnd(); ++Iterator) {
				if (Predicate(*Iterator)) {
					return (Iterator - this->PtrBegin());
				}
			}

			return SIZE_MAX;
		}
		
		/// @brief Searches for first element in container by checking equality.
		/// @param Element Object that function would be looking for.
		/// @returns Index of found element or @c SIZE_MAX if nothing was found.
		GDINL size_t FindFirstElement(ElementType const& Element) const
		{
			return this->FindFirstElement([&Element](ElementType const& OtherElement) {
				return (OtherElement == Element);
			});
		}

		/// @brief Searches for last element in container by predicate.
		/// @param Predicate Object with () operator overloaded that returns true if both specified objects are equal.
		/// @returns Index of found element or @c SIZE_MAX if nothing was found.
		template<typename SearchingPredicateType>
		GDINL size_t FindLastElement(SearchingPredicateType const& Predicate) const
		{
			for (PtrConstIterator Iterator = (this->PtrEnd() - 1); Iterator != (this->PtrBegin() - 1); --Iterator) {
				if (Predicate(*Iterator)) {
					return (Iterator - this->ReversePtrEnd());
				}
			}

			return SIZE_MAX;
		}
		
		/// @brief Searches for last element in container by checking equality.
		/// @param Element Object that function would be looking for.
		/// @returns Index of found element or @c SIZE_MAX if nothing was found.
		GDINL size_t FindLastElement(ElementType const& Element) const
		{
			return this->FindFirstElement([&Element](ElementType const& OtherElement) {
				return (OtherElement == Element);
			});
		}

		/// @brief Searches for all elements in container by predicate.
		/// @param Predicate Object with () operator overloaded that returns true if both specified objects are equal.
		/// @returns A container with all elements that were found by the predicate.
		template<typename SearchingPredicateType>
		GDINL Vector FindAllElements(SearchingPredicateType const& Predicate) const
		{
			Vector SearchingResult;
			for (PtrConstIterator Iterator = this->PtrBegin(); Iterator != this->PtrEnd(); ++Iterator) {
				if (Predicate(*Iterator)) {
					SearchingResult.PushLast(*Iterator);
				}
			}

			return SearchingResult;
		}

		/// @brief Sorts all elements in collection by predicate using Quick Sort.
		/// @param Predicate Object with () operator overloaded that returns true that this object should locate upper in collection.
		/// @param Left Left border of sorting.
		/// @param Right Right border of sorting.
		template<typename SortingPredicateType>
		GDINL void Sort(SortingPredicateType const& Predicate, Iterator const _Left, Iterator const _Right)
		{
			/// @todo Implementation is a little broken if container includes equal values.
			Iterator Left = _Left, Right = _Right;
			Iterator const Pivot = this->Begin() + (((Left - this->Begin()) + (Right - this->Begin())) / 2);
			while (Left <= Right) {
				while (Predicate(*Left, *Pivot)) {
					Left += 1;
				}
				while (Predicate(*Pivot, *Right)) {
					Right -= 1;
				}
				if (Left <= Right) {
					Swap(*Left, *Right);
					Left += 1;
					Right -= 1;
				}
			}

			if (_Left < Right) {
				this->Sort(Predicate, _Left, Right);
			}
			if (Left < _Right) {
				this->Sort(Predicate, Left, _Right);
			}
		}

		/// @brief Sorts all elements in collection by predicate using Quick Sort.
		/// @param Predicate Object with () operator overloaded that returns true that this object should locate upper in collection.
		template<typename SortingPredicateType>
		GDINL void Sort(SortingPredicateType const& Predicate)
		{	
			this->Sort(Predicate, this->Begin(), (this->End() - 1));
		}

		/// @brief Sorts all elements in collection using Quick Sort.
		GDINL void Sort()
		{	
			this->Sort([](ElementType const& Right, ElementType const& Left) {
				return (Right < Left);
			});
		}

		/// @brief Moves other vector here.
		/// @param OtherVector Vector would be moved into current object.
		/// @returns Self.
		GDINL Vector& operator= (Vector&& OtherVector)
		{
			if (&OtherVector != this) {
				this->Clear();
				this->Memory = OtherVector.Memory;
				this->Length = OtherVector.Length;
				this->Capacity = OtherVector.Capacity;

				OtherVector.Memory = nullptr;
				OtherVector.Length = 0;
				OtherVector.Capacity = 0;
			}

			return *this;
		}

		/// @brief Assigns vector with copy of other vector.
		/// @param OtherVector Vector would be assigned.
		/// @returns Self.
		GDINL Vector& operator= (Vector const& OtherVector)
		{
			if ((&OtherVector) != this) {
				if (this->Capacity >= OtherVector.Length) {
					this->Emptify();
					this->Length = OtherVector.Length;
					Vector::CopyFromRange(OtherVector.PtrBegin(), OtherVector.PtrEnd(), this->PtrBegin());
				} else {
					*this = Move(Vector(OtherVector));
				}
			}

			return *this;
		}

		/// @brief Assigned vector the default C++11's initializer list. You should not use this constructor manually.
		/// @param InitializerList Initializer list passed by the compiler.
		/// @returns Self.
		GDINL Vector& operator= (InitializerList<ElementType> const& InitializerList)
		{
			if (this->Capacity >= InitializerList.size()) {
				this->Emptify();
				this->Length = InitializerList.size();
				Vector::CopyFromRange(InitializerList.begin(), InitializerList.end(), this->PtrBegin());
			} else {
				*this = Move(Vector(InitializerList));
			}

			return *this;
		}

		/// @brief Returns reference on element at specified index.
		/// @param Index Index of the desired element in vector.
		/// @returns Reference on some element in the container.
		/// @{
		GDINL ElementType const& operator[] (size_t const Index) const
		{
			GD_ASSERT(Index < this->Length, "Index is out of bounds");
			return *(this->PtrBegin() + Index);
		}
		GDINL ElementType& operator[] (size_t const Index)
		{
			return const_cast<ElementType&>(const_cast<Vector const&>(*this)[Index]);
		}
		/// @}

		/// @brief Checks if this vector is same to specified one.
		/// @param OtherVector Other vector to compare to.
		/// @returns True if both vectors have same length and elements.
		GDINL bool operator== (Vector const& OtherVector) const
		{
			if (this->Length == OtherVector.Length) {
				return this->CompareTo([](ElementType const& First, ElementType const& Second) -> bool {
					return (First == Second);
				});
			} else {
				return false;
			}
		}

		/// @brief Checks if this vector is different to specified one.
		/// @param OtherVector Other vector to compare to.
		/// @returns True if both vectors have different length or elements.
		GDINL bool operator!= (Vector const& OtherVector) const
		{
			return this->CompareTo([](ElementType const& First, ElementType const& Second) -> bool {
				return (First != Second);
			});
		}

		/// @brief Checks if this vector is greater to specified one.
		/// @param OtherVector Other vector to compare to.
		/// @returns True if this's of the first different elements in vectors is greater than other's or strings are equal.
		GDINL bool operator> (Vector const& OtherVector) const
		{
			this->CompareTo([](ElementType const& First, ElementType const& Second) -> bool {
				return (First >= Second);
			});
		}

		/// @brief Checks if this vector is greater to specified one.
		/// @param OtherVector Other vector to compare to.
		/// @returns True if this's of the first different elements in vectors is less than other's or strings are equal.
		GDINL bool operator< (Vector const& OtherVector) const
		{
			return this->CompareTo([](ElementType const& First, ElementType const& Second) -> bool {
				return (First <= Second);
			});
		}

		/// @brief Concatenates two vectors.
		/// @param OtherVector Other vector to concatenate with.
		/// @returns Result of the vector concatenation.
		/// @{
		GDINL Vector& operator+= (Vector&& OtherVector)
		{
			this->ReserveToLength(this->Length + OtherVector.Length);
			for (Iterator Iterator = OtherVector.Begin(); Iterator != OtherVector.End(); ++Iterator) {
				this->PushLast(Move(*Iterator));
			}

			return *this;
		}
		GDINL Vector& operator+= (Vector const& OtherVector)
		{
			this->ReserveToLength(this->Length + OtherVector.Length);
			for (Iterator Iterator = OtherVector.Begin(); Iterator != OtherVector.End(); ++Iterator) {
				this->PushLast(*Iterator);
			}

			return *this;
		}
		GDINL Vector operator+ (Vector&& OtherVector) const
		{
			return Vector*this += Forward<Vector>(OtherVector);
		}
		GDINL Vector operator+ (Vector const& OtherVector) const
		{
			return *this + Move(Vector(OtherVector));
		}
		/// @}

	private:

		template<typename SomeElementType>
		GDINL friend void Swap(Vector<SomeElementType>& First, Vector<SomeElementType>& Second);
	};	// class Vector

	/// @brief Swaps two vectors.
	/// @param First The first vector.
	/// @param Second The second vector.
	template<typename SomeElementType>
	GDINL void Swap(Vector<SomeElementType>& First, Vector<SomeElementType>& Second)
	{
		if (&First != &Second) {
			Swap(First.Memory, Second.Memory);
			Swap(First.Length, Second.Length);
			Swap(First.Capacity, Second.Capacity);
		}
	}
	
GD_NAMESPACE_END

#endif	// ifndef GD_CORE_CONTAINERS_VECTOR
