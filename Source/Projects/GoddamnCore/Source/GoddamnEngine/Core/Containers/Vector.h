// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Containers/Vector.h
//! Dynamically sized array class.
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/Algorithm.h>
#include <GoddamnEngine/Core/Containers/Containers.h>
#include <GoddamnEngine/Core/Containers/InitializerList.h>


GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Vector<T> class.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Dynamic array implementation.
	//! @tparam ElementTypeTp Container element type.
	template<typename ElementTypeTp>
	class Vector final 
	{
	public:
		typedef ElementTypeTp								ElementType;
		typedef IndexedContainerIterator<Vector>			Iterator;
		typedef IndexedContainerIterator<Vector const>		ConstIterator;
		typedef ReverseContainerIterator<Iterator>			ReverseIterator;
		typedef ReverseContainerIterator<ConstIterator>		ReverseConstIterator;
		typedef ElementType*								PtrIterator;
		typedef ElementType const*							PtrConstIterator;
		typedef ReverseContainerIterator<PtrIterator>		ReversePtrIterator;
		typedef ReverseContainerIterator<PtrConstIterator>	ReversePtrConstIterator;

		GD_CONTAINER_DEFINE_PTR_ITERATION_SUPPORT(Vector);

	private:
		Handle Memory	= nullptr;	// layout of variables should be saved.
		SizeTp Length	= 0;
		SizeTp Capacity	= 0;

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructors and destructor.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Initializes vector with specified number of allocated and initialized elements.
		//! @param InitialLength Number of elements been initialized.
		//! @param InitialCapacity Initial capacity of vector. 
		GDINL explicit Vector(SizeTp const InitialLength = 0, SizeTp const InitialCapacity = SizeTpMax)
		{
			this->Reserve(InitialCapacity != SizeTpMax ? InitialCapacity : InitialLength);
			this->Resize(InitialLength);
			InitializeRange(this->PtrBegin(), this->PtrEnd());
		}

		// ------------------------------------------------------------------------------------------
		//! Initializes vector with copy of values of specified iterators. 
		//! @param StartIterator First iterator would be copied.
		//! @param EndIterator Last iterator would be copied.
		GDINL Vector(ConstIterator const StartIterator, ConstIterator const EndIterator)
		{
			this->Resize(EndIterator - StartIterator);
			CopyFromRange(StartIterator, EndIterator, this->PtrBegin());
		}

		// ------------------------------------------------------------------------------------------
		//! Initializes vector with default C++11's initializer list. You should not use this constructor manually.
		//! @param InitializerList Initializer list passed by the compiler.
		GDINL Vector(InitializerList<ElementType> const& InitializerList)
		{
			this->Resize(InitializerList.size());
			CopyFromRange(InitializerList.begin(), InitializerList.end(), this->PtrBegin());
		}

		// ------------------------------------------------------------------------------------------
		//! Moves other vector here.
		//! @param OtherVector Vector would be moved into current object.
		GDINL Vector(Vector&& OtherVector)
		{
			this->Memory = OtherVector.Memory;
			this->Length = OtherVector.Length;
			this->Capacity = OtherVector.Capacity;

			OtherVector.Memory = nullptr;
			OtherVector.Length = 0;
			OtherVector.Capacity = 0;
		}

		// ------------------------------------------------------------------------------------------
		//! Initializes vector with copy of other vector.
		//! @param OtherVector Vector would be copied.
		GDINL Vector(Vector const& OtherVector)
		{
			this->Resize(OtherVector.Length);
			CopyFromRange(OtherVector.PtrBegin(), OtherVector.PtrEnd(), this->PtrBegin());
		}

		// ------------------------------------------------------------------------------------------
		//! Deinitializes all elements and deallocates memory.
		GDINL ~Vector()
		{
			this->Clear();
		}

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Iteration API.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Iterator that points to first container element.
		//! @returns Iterator that points to first container element.
		//! @{
	public:
		GDINL Iterator Begin() { return Iterator(*this); }
		GDINL ConstIterator Begin() const { return ConstIterator(*this); }
	private:
		GDINL PtrIterator PtrBegin() { return reinterpret_cast<PtrIterator>(this->Memory); }
		GDINL PtrConstIterator PtrBegin() const { return reinterpret_cast<PtrConstIterator>(this->Memory); }
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns iterator that points to past the end container element.
		//! @returns Iterator that points to past the end container element.
		//! @{
	public:
		GDINL Iterator End() { return this->Begin() + this->Length; }
		GDINL ConstIterator End() const { return this->Begin() + this->Length; }
	private:
		GDINL PtrIterator PtrEnd() { return this->PtrBegin() + this->Length; }
		GDINL PtrConstIterator PtrEnd() const { return this->PtrBegin() + this->Length; }
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns iterator that points to last container element.
		//! @returns Iterator that points to last container element.
		//! @{
	public:
		GDINL ReverseIterator ReverseBegin() { return ReverseIterator(this->End() - 1); }
		GDINL ReverseConstIterator ReverseBegin() const { return ReverseConstIterator(this->End() - 1); }
	private:
		GDINL ReversePtrIterator ReversePtrBegin() { return ReversePtrIterator(this->PtrEnd() - 1); }
		GDINL ReversePtrConstIterator ReversePtrBegin() const { return ReversePtrConstIterator(this->PtrEnd() - 1);	}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns iterator that points to preceding the first container element.
		//! @returns Iterator that points to preceding the first container element.
		//! @{
	public:
		GDINL ReverseIterator ReverseEnd() { return ReverseIterator(this->Begin() - 1);	}
		GDINL ReverseConstIterator ReverseEnd() const { return ReverseConstIterator(this->Begin() - 1);	}
	private:
		GDINL ReversePtrIterator ReversePtrEnd() { return ReversePtrIterator(this->PtrBegin() - 1); }
		GDINL ReversePtrConstIterator ReversePtrEnd() const	{ return ReversePtrConstIterator(this->PtrBegin() - 1);	}
		//! @}

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Dynamic size management.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Returns number of elements that exist in vector.
		//! @returns Number of elements that exist in vector.
		GDINL SizeTp GetLength() const
		{
			return this->Length;
		}

		// ------------------------------------------------------------------------------------------
		//! Returns number of elements that can be placed into vector without reallocation.
		//! @returns Number of elements that can be placed into vector without reallocation.
		GDINL SizeTp GetCapacity() const
		{
			return this->Capacity;
		}

		// ------------------------------------------------------------------------------------------
		//! Returns true if this container is empty.
		//! @returns True if this container is empty, false otherwise.
		GDINL bool IsEmpty() const
		{
			return this->Length == 0;
		}

		// ------------------------------------------------------------------------------------------
		//! Resizes vector to make it able to contain specified number of elements.
		//! @param NewLength New required length of the container.
		GDINL void Resize(SizeTp const NewLength)
		{
			if (this->Length != NewLength)
			{	// We do not need to resize our element with same size.
				if (this->Length < NewLength)
				{	// Increasing size of container, we need to initialize new elements.
					this->ReserveToLength(NewLength);
					InitializeRange(this->PtrEnd(), this->PtrBegin() + NewLength);
				}
				else
				{	// Decreasing size of container, we need to destroy last elements there.
					DeinitializeRange(this->PtrBegin() + NewLength, this->PtrEnd());
				}
				this->Length = NewLength;
			}
		}

		// ------------------------------------------------------------------------------------------
		//! Reserves memory for vector to make it contain specified number of elements without 
		//! reallocation when calling Resize/PushBack/Insert method.
		//! @param NewCapacity New required capacity of the container.
		GDINL void Reserve(SizeTp const NewCapacity)
		{
			// Moving elements into other memory block. Note that elements outside range keep uninitialized.
			if (this->Capacity != NewCapacity)
			{
				// Clamping container size to capacity.
				if (NewCapacity < this->Length)
					this->Resize(NewCapacity);
				ElementType* NewMemory = reinterpret_cast<ElementType*>(GD_MALLOC(NewCapacity * sizeof(ElementType)));
				MoveFromRange(this->PtrBegin(), this->PtrEnd(), reinterpret_cast<PtrIterator>(NewMemory));
				DeinitializeRange(this->PtrBegin(), this->PtrEnd());
				this->Memory = NewMemory;
				this->Capacity = NewCapacity;
			}
		}

		// ------------------------------------------------------------------------------------------
		//! Reserves memory for vector to make it contain best fitting number of elements 
		//! to predicted new size. This function incrementally grows capacity in 1.3 times.
		//! @param NewLength New desired length on which best capacity is computed.
		GDINL void ReserveToLength(SizeTp const NewLength)
		{
			if (NewLength > this->Capacity)
				this->Reserve(static_cast<SizeTp>(1.3 * static_cast<double>(Max(this->Capacity, NewLength))));
		}

		// ------------------------------------------------------------------------------------------
		//! Shrinks vector's capacity to length.
		GDINL void ShrinkToFit()
		{
			this->Reserve(this->Length);
		}

		// ------------------------------------------------------------------------------------------
		//! Destroys all elements in container without memory deallocation.
		GDINL void Emptify()
		{
			this->Resize(0);
		}

		// ------------------------------------------------------------------------------------------
		//! Destroys all elements in container with memory deallocation.
		GDINL void Clear()
		{
			this->Reserve(0);
		}

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Dynamic elements access.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Returns reference on last element in container.
		//! @returns Reference on last element in the container.
		//! @{
		GDINL ElementType const& GetLastElement() const
		{
			return (*this)[this->Length - 1];
		}
		GDINL ElementType& GetLastElement()
		{
			return (*this)[this->Length - 1];
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Inserts specified element into collection at desired index.
		//! @param Index At this index new element would be inserted. All existing elements would be shifted to right.
		//! @param Element New element that would be inserted.
		//! @{
		GDINL void InsertElementAt(SizeTp const Index, ElementType&& Element)
		{
			GD_DEBUG_ASSERT(Index <= this->Length, "Index is out of bounds");
			this->ReserveToLength(this->Length + 1);
			for (PtrIterator Iterator = this->PtrEnd(); Iterator != this->PtrBegin() + Index - 1; --Iterator)
				*(Iterator - 1) = Move(*Iterator);
			this->Length += 1;
			InitializeIterator(this->PtrBegin() + Index, Forward<ElementType>(Element));
		}
		GDINL void InsertElementAt(SizeTp const Index, ElementType const& Element)
		{
			GD_DEBUG_ASSERT(Index <= this->Length, "Index is out of bounds");
			this->ReserveToLength(this->Length + 1);
			for (PtrIterator Iterator = this->PtrEnd(); Iterator != this->PtrBegin() + Index - 1; --Iterator)
				*(Iterator - 1) = *Iterator;
			this->Length += 1;
			InitializeIterator(this->PtrBegin() + Index, Element);
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Removes existing element from array at specified index.
		//! @param Index Element at this index would be removed. All other elements would be shifted to left.
		GDINL void RemoveElementAt(SizeTp const Index)
		{
			GD_ASSERT(Index < this->Length, "Index is out of bounds");
			for (PtrIterator Iterator = this->PtrBegin() + Index; Iterator != (this->PtrEnd() - 1); ++Iterator)
				*Iterator = Move(*(Iterator + 1));
			this->Length -= 1;
			DeinitializeIterator(this->PtrEnd());
		}

		// ------------------------------------------------------------------------------------------
		//! Appends new element to container.
		//! @param NewElement New element that would be inserted into the end of container.
		//! @{
		GDINL void InsertLast(ElementType&& NewElement = ElementType())
		{
			this->ReserveToLength(this->Length + 1);
			this->Length += 1;
			InitializeIterator(this->PtrEnd() - 1, Forward<ElementType>(NewElement));
		}
		GDINL void InsertLast(ElementType const& NewElement)
		{
			this->ReserveToLength(this->Length + 1);
			this->Length += 1;
			InitializeIterator(this->PtrEnd() - 1, NewElement);
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Removes last element from container.
		GDINL void RemoveLast()
		{
			GD_DEBUG_ASSERT((this->Length != 0), "Container size is zero");
			this->Resize(this->Length - 1);
		}

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Searching for elements and sorting. 
		//! @todo Move this code to Algorithms.h and make if abstract for all containers.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Searches for first element in container by predicate.
		//! @param Predicate Object with () operator overloaded that returns true if both specified objects are equal.
		//! @returns Index of found element or @c SizeTpMax if nothing was found.
		template<typename SearchingPredicateType>
		GDINL SizeTp FindFirstElement(SearchingPredicateType const& Predicate) const
		{
			for (PtrConstIterator Iterator = this->PtrBegin(); Iterator != this->PtrEnd(); ++Iterator)
				if (Predicate(*Iterator))
					return (Iterator - this->PtrBegin());
			return SizeTpMax;
		}

		// ------------------------------------------------------------------------------------------
		//! Searches for first element in container by checking equality.
		//! @param Element Object that function would be looking for.
		//! @returns Index of found element or @c SizeTpMax if nothing was found.
		GDINL SizeTp FindFirstElement(ElementType const& Element) const
		{
			return this->FindFirstElement([&Element](ElementType const& OtherElement)
			{
				return (OtherElement == Element);
			});
		}

		// ------------------------------------------------------------------------------------------
		//! Searches for last element in container by predicate.
		//! @param Predicate Object with () operator overloaded that returns true if both specified objects are equal.
		//! @returns Index of found element or @c SizeTpMax if nothing was found.
		template<typename SearchingPredicateType>
		GDINL SizeTp FindLastElement(SearchingPredicateType const& Predicate) const
		{
			for (PtrConstIterator Iterator = (this->PtrEnd() - 1); Iterator != (this->PtrBegin() - 1); --Iterator)
				if (Predicate(*Iterator))
					return (Iterator - this->ReversePtrEnd());
			return SizeTpMax;
		}

		// ------------------------------------------------------------------------------------------
		//! Searches for last element in container by checking equality.
		//! @param Element Object that function would be looking for.
		//! @returns Index of found element or @c SizeTpMax if nothing was found.
		GDINL SizeTp FindLastElement(ElementType const& Element) const
		{
			return this->FindFirstElement([&Element](ElementType const& OtherElement)
			{
				return (OtherElement == Element);
			});
		}

		// ------------------------------------------------------------------------------------------
		//! Searches for all elements in container by predicate.
		//! @param Predicate Object with () operator overloaded that returns true if both specified objects are equal.
		//! @returns A container with all elements that were found by the predicate.
		template<typename SearchingPredicateType>
		GDINL Vector FindAllElements(SearchingPredicateType const& Predicate) const
		{
			Vector SearchingResult;
			for (PtrConstIterator Iterator = this->PtrBegin(); Iterator != this->PtrEnd(); ++Iterator)
				if (Predicate(*Iterator))
					SearchingResult.InsertLast(*Iterator);
			return SearchingResult;
		}

		// ------------------------------------------------------------------------------------------
		//! Sorts all elements in collection by predicate using Quick Sort.
		//! @param Predicate Object with () operator overloaded that returns true that this object should locate upper in collection.
		//! @param Left Left border of sorting.
		//! @param Right Right border of sorting.
		template<typename SortingPredicateType>
		GDINL void Sort(SortingPredicateType const& Predicate, Iterator const _Left, Iterator const _Right)
		{
			//! @todo Implementation is a little broken if container includes equal values.
			Iterator Left = _Left, Right = _Right;
			Iterator const Pivot = this->Begin() + (((Left - this->Begin()) + (Right - this->Begin())) / 2);
			while (Left <= Right)
			{
				while (Predicate(*Left, *Pivot))
				{
					Left += 1;
				}
				while (Predicate(*Pivot, *Right))
				{
					Right -= 1;
				}
				if (Left <= Right)
				{
					Swap(*Left, *Right);
					Left += 1;
					Right -= 1;
				}
			}

			if (_Left < Right)
			{
				this->Sort(Predicate, _Left, Right);
			}
			if (Left < _Right)
			{
				this->Sort(Predicate, Left, _Right);
			}
		}

		// ------------------------------------------------------------------------------------------
		//! Sorts all elements in collection by predicate using Quick Sort.
		//! @param Predicate Object with () operator overloaded that returns true that this object should locate upper in collection.
		template<typename SortingPredicateType>
		GDINL void Sort(SortingPredicateType const& Predicate)
		{
			this->Sort(Predicate, this->Begin(), (this->End() - 1));
		}

		// ------------------------------------------------------------------------------------------
		//! Sorts all elements in collection using Quick Sort.
		GDINL void Sort()
		{
			this->Sort([](ElementType const& Right, ElementType const& Left)
			{
				return (Right < Left);
			});
		}

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Overloaded operators.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Moves other vector here.
		//! @param OtherVector Vector would be moved into current object.
		//! @returns this.
		GDINL Vector& operator= (Vector&& OtherVector)
		{
			if (&OtherVector != this)
			{
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

		// ------------------------------------------------------------------------------------------
		//! Assigns vector with copy of other vector.
		//! @param OtherVector Vector would be assigned.
		//! @returns this.
		GDINL Vector& operator= (Vector const& OtherVector)
		{
			if ((&OtherVector) != this)
			{
				if (this->Capacity >= OtherVector.Length)
				{
					this->Emptify();
					this->Length = OtherVector.Length;
					CopyFromRange(OtherVector.PtrBegin(), OtherVector.PtrEnd(), this->PtrBegin());
				}
				else
				{
					*this = Move(Vector(OtherVector));
				}
			}
			return *this;
		}

		// ------------------------------------------------------------------------------------------
		//! Assigned vector the default C++11's initializer list. You should not use this constructor manually.
		//! @param InitializerList Initializer list passed by the compiler.
		//! @returns this.
		GDINL Vector& operator= (InitializerList<ElementType> const& InitializerList)
		{
			if (this->Capacity >= InitializerList.size())
			{
				this->Emptify();
				this->Length = InitializerList.size();
				CopyFromRange(InitializerList.begin(), InitializerList.end(), this->PtrBegin());
			}
			else
			{
				*this = Move(Vector(InitializerList));
			}
			return *this;
		}

		// ------------------------------------------------------------------------------------------
		//! Returns reference on element at specified index.
		//! @param Index Index of the desired element in vector.
		//! @returns Reference on some element in the container.
		//! @{
		GDINL ElementType& operator[] (SizeTp const Index) { return const_cast<ElementType&>(const_cast<Vector const&>(*this)[Index]); }
		GDINL ElementType const& operator[] (SizeTp const Index) const
		{
			GD_ASSERT(Index < this->Length, "Index is out of bounds");
			return *(this->PtrBegin() + Index);
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Checks if this vector is same to specified one.
		//! @param OtherVector Other vector to compare to.
		//! @returns True if both vectors have same length and elements.
		GDINL bool operator== (Vector const& OtherVector) const
		{
			if (this->Length == OtherVector.Length)
				return CompareTo(*this, OtherVector, [](ElementType const& First, ElementType const& Second) -> bool
				{
					return (First == Second);
				});
			return false;
		}

		// ------------------------------------------------------------------------------------------
		//! Checks if this vector is different to specified one.
		//! @param OtherVector Other vector to compare to.
		//! @returns True if both vectors have different length or elements.
		GDINL bool operator!= (Vector const& OtherVector) const
		{
			return CompareTo(*this, OtherVector, [](ElementType const& First, ElementType const& Second) -> bool
			{
				return (First != Second);
			});
		}

		// ------------------------------------------------------------------------------------------
		//! Checks if this vector is greater to specified one.
		//! @param OtherVector Other vector to compare to.
		//! @returns True if this's of the first different elements in vectors is greater than other's or strings are equal.
		GDINL bool operator> (Vector const& OtherVector) const
		{
			return CompareTo(*this, OtherVector, [](ElementType const& First, ElementType const& Second) -> bool
			{
				return (First >= Second);
			});
		}

		// ------------------------------------------------------------------------------------------
		//! Checks if this vector is greater to specified one.
		//! @param OtherVector Other vector to compare to.
		//! @returns True if this's of the first different elements in vectors is less than other's or strings are equal.
		GDINL bool operator< (Vector const& OtherVector) const
		{
			return CompareTo(*this, OtherVector, [](ElementType const& First, ElementType const& Second) -> bool
			{
				return (First <= Second);
			});
		}

		// ------------------------------------------------------------------------------------------
		//! Concatenates two vectors.
		//! @param OtherVector Other vector to concatenate with.
		//! @returns Result of the vector concatenation.
		//! @{
		GDINL Vector& operator+= (Vector&& OtherVector)
		{
			this->ReserveToLength(this->Length + OtherVector.Length);
			for (Iterator Iterator = OtherVector.Begin(); Iterator != OtherVector.End(); ++Iterator)
				this->InsertLast(Move(*Iterator));
			return *this;
		}
		GDINL Vector& operator+= (Vector const& OtherVector)
		{
			this->ReserveToLength(this->Length + OtherVector.Length);
			for (Iterator Iterator = OtherVector.Begin(); Iterator != OtherVector.End(); ++Iterator)
				this->InsertLast(*Iterator);
			return *this;
		}
		GDINL Vector operator+ (Vector&& OtherVector) const
		{
			return Vector(*this) += Forward<Vector>(OtherVector);
		}
		GDINL Vector operator+ (Vector const& OtherVector) const
		{
			return Vector(*this) + Move(Vector(OtherVector));
		}
		//! @}

	private:

		template<typename SomeElementType>
		GDINL friend void Swap(Vector<SomeElementType>& First, Vector<SomeElementType>& Second);
	};	// class Vector

	// ------------------------------------------------------------------------------------------
	//! Swaps two vectors.
	//! @param First The first vector.
	//! @param Second The second vector.
	template<typename SomeElementType>
	GDINL void Swap(Vector<SomeElementType>& First, Vector<SomeElementType>& Second)
	{
		if (&First != &Second)
		{
			Swap(First.Memory, Second.Memory);
			Swap(First.Length, Second.Length);
			Swap(First.Capacity, Second.Capacity);
		}
	}

GD_NAMESPACE_END
