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
#include <GoddamnEngine/Core/Algorithm.h>
#include <GoddamnEngine/Core/Iterators.h>
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
		using ElementType             = ElementTypeTp;
		using PtrIterator             = ElementType*;
		using PtrConstIterator        = ElementType const*;
		using Iterator                = IndexedContainerIterator<Vector>;
		using ConstIterator           = IndexedContainerIterator<Vector const>;
		using ReverseIterator         = ReverseContainerIterator<Iterator>;
		using ReverseConstIterator    = ReverseContainerIterator<ConstIterator>;
		using ReversePtrIterator      = ReverseContainerIterator<PtrIterator>;
		using ReversePtrConstIterator = ReverseContainerIterator<PtrConstIterator>;

		GD_CONTAINER_DEFINE_PTR_ITERATION_SUPPORT(Vector);

	private:
		template<typename SomeElementType>
		GDINL friend void Swap(Vector<SomeElementType>& First, Vector<SomeElementType>& Second);

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
			this->Length = InitialLength;
			InitializeRange(this->_PtrBegin(), this->_PtrEnd());
		}

		// ------------------------------------------------------------------------------------------
		//! Initializes vector with copy of values of specified iterators. 
		//! @param StartIterator First iterator would be copied.
		//! @param EndIterator Last iterator would be copied.
		GDINL Vector(ConstIterator const StartIterator, ConstIterator const EndIterator)
		{
			this->Resize(EndIterator - StartIterator);
			CopyRange(StartIterator, EndIterator, this->_PtrBegin());
		}

		// ------------------------------------------------------------------------------------------
		//! Initializes vector with default C++11's initializer list. You should not use this constructor manually.
		//! @param InitializerList Initializer list passed by the compiler.
		GDINL Vector(InitializerList<ElementType> const& InitializerList)
		{
			this->Resize(InitializerList.size());
			CopyRange(InitializerList.begin(), InitializerList.end(), this->_PtrBegin());
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
			CopyRange(OtherVector._PtrBegin(), OtherVector._PtrEnd(), this->_PtrBegin());
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
		//! Returns iterator that points to first container element.
		//! @returns Iterator that points to first container element.
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

		
		// ------------------------------------------------------------------------------------------
		//! Returns iterator that points to past the end container element.
		//! @returns Iterator that points to past the end container element.
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

		// ------------------------------------------------------------------------------------------
		//! Returns iterator that points to last container element.
		//! @returns Iterator that points to last container element.
		/// @{
		GDINL ReverseIterator ReverseBegin()
		{
			return ReverseIterator(this->End() - 1);
		}
		GDINL ReverseConstIterator ReverseBegin() const
		{
			return ReverseConstIterator(this->End() - 1);
		}
		/// @}

		// ------------------------------------------------------------------------------------------
		//! Returns iterator that points to preceding the first container element.
		//! @returns Iterator that points to preceding the first container element.
		/// @{
		GDINL ReverseIterator ReverseEnd()
		{
			return ReverseIterator(this->Begin() - 1);
		}
		GDINL ReverseConstIterator ReverseEnd() const
		{
			return ReverseConstIterator(this->Begin() - 1);
		}
		/// @}

	private:

		GDINL PtrIterator _PtrBegin()
		{
			return PtrIterator(this->GetData());
		}
		GDINL PtrConstIterator _PtrBegin() const
		{
			return PtrConstIterator(this->GetData());
		}
		
		GDINL PtrIterator _PtrEnd()
		{
			return this->_PtrBegin() + this->Length;
		}
		GDINL PtrConstIterator _PtrEnd() const
		{
			return this->_PtrBegin() + this->Length;
		}
		
		GDINL ReversePtrIterator _ReversePtrBegin()
		{
			return ReversePtrIterator(this->_PtrEnd() - 1);
		}
		GDINL ReversePtrConstIterator _ReversePtrBegin() const
		{
			return ReversePtrConstIterator(this->_PtrEnd() - 1);
		}
		
		GDINL ReversePtrIterator _ReversePtrEnd()
		{
			return ReversePtrIterator(this->_PtrBegin() - 1);
		}
		GDINL ReversePtrConstIterator _ReversePtrEnd() const
		{
			return ReversePtrConstIterator(this->_PtrBegin() - 1);
		}

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Dynamic size management.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Returns pointer to the data stored in vector.
		//! @returns pointer to the data stored in vector.
		/// @{
		GDINL ElementType const* GetData() const
		{
			return reinterpret_cast<ElementType const*>(this->Memory);
		}
		GDINL ElementType* GetData()
		{
			return reinterpret_cast<ElementType*>(this->Memory);
		}
		/// @}

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
			{	
				if (this->Length < NewLength)
				{	
					this->ReserveToLength(NewLength);
					InitializeRange(this->_PtrEnd(), this->_PtrBegin() + NewLength);
				}
				else
				{	
					DeinitializeRange(this->_PtrBegin() + NewLength, this->_PtrEnd());
				}
				this->Length = NewLength;
			}
		}

		// ------------------------------------------------------------------------------------------
		//! Reserves memory for vector to make it contain specified number of elements without 
		//! reallocation when calling Resize/Insert/InsertLast method.
		//! @param NewCapacity New required capacity of the container.
		GDINL void Reserve(SizeTp const NewCapacity)
		{
			// Note that elements outside range keep uninitialized.
			if (this->Capacity != NewCapacity)
			{
				if (NewCapacity < this->Length)
					this->Resize(NewCapacity);
				ElementType* const NewMemory = reinterpret_cast<ElementType*>(GD_MALLOC(NewCapacity * sizeof(ElementType)));
				MoveRange(this->_PtrBegin(), this->_PtrEnd(), reinterpret_cast<PtrIterator>(NewMemory));
				DeinitializeRange(this->_PtrBegin(), this->_PtrEnd());
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
			{
				SizeTp const NewCapacity = 13 * (Max(this->Capacity, NewLength) + 1) / 10;
				this->Reserve(NewCapacity);
			}
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
		/// @{
		GDINL ElementType const& GetLastElement() const
		{
			return (*this)[this->Length - 1];
		}
		GDINL ElementType& GetLastElement()
		{
			return (*this)[this->Length - 1];
		}
		/// @}

		// ------------------------------------------------------------------------------------------
		//! Inserts specified element into collection at desired index.
		//! @param Index At this index new element would be inserted. All existing elements would be shifted to right.
		//! @param Element New element that would be inserted.
		//! @returns Index at which new element was inserted, or the value of @ref Index param.
		/// @{
		GDINL SizeTp InsertElementAt(SizeTp const Index, ElementType&& Element)
		{
			GD_DEBUG_ASSERT(Index <= this->Length, "Index is out of bounds");
			this->ReserveToLength(this->Length + 1);
			for (PtrIterator Iterator = this->_PtrEnd(); Iterator != this->_PtrBegin() + Index - 1; --Iterator)
				*(Iterator - 1) = Move(*Iterator);
			this->Length += 1;
			InitializeIterator(this->_PtrBegin() + Index, Forward<ElementType>(Element));
			return Index;
		}
		GDINL SizeTp InsertElementAt(SizeTp const Index, ElementType const& Element)
		{
			GD_DEBUG_ASSERT(Index <= this->Length, "Index is out of bounds");
			this->ReserveToLength(this->Length + 1);
			for (PtrIterator Iterator = this->_PtrEnd(); Iterator != this->_PtrBegin() + Index - 1; --Iterator)
				*(Iterator - 1) = *Iterator;
			this->Length += 1;
			InitializeIterator(this->_PtrBegin() + Index, Element);
			return Index;
		}
		/// @}

		// ------------------------------------------------------------------------------------------
		//! Appends new element to container.
		//! @param NewElement New element that would be inserted into the end of container.
		//! @returns Index at which new element was inserted.
		/// @{
		GDINL SizeTp InsertLast(ElementType&& NewElement = ElementType())
		{
			this->ReserveToLength(this->Length + 1);
			this->Length += 1;
			InitializeIterator(this->_PtrEnd() - 1, Forward<ElementType>(NewElement));
			return this->Length - 1;
		}
		GDINL SizeTp InsertLast(ElementType const& NewElement)
		{
			this->ReserveToLength(this->Length + 1);
			this->Length += 1;
			InitializeIterator(this->_PtrEnd() - 1, NewElement);
			return this->Length - 1;
		}
		/// @}

		// ------------------------------------------------------------------------------------------
		//! Removes existing element from array at specified index.
		//! @param Index Element at this index would be removed. All other elements would be shifted to left.
		GDINL void RemoveElementAt(SizeTp const Index)
		{
			GD_ASSERT(Index < this->Length, "Index is out of bounds");
			for (PtrIterator Iterator = this->_PtrBegin() + Index; Iterator != (this->_PtrEnd() - 1); ++Iterator)
				*Iterator = Move(*(Iterator + 1));
			this->Length -= 1;
			DeinitializeIterator(this->_PtrEnd());
		}

		// ------------------------------------------------------------------------------------------
		//! Removes last element from container.
		GDINL void RemoveLast()
		{
			GD_DEBUG_ASSERT((this->Length != 0), "Container size is zero");
			this->Resize(this->Length - 1);
		}

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Overloaded operators.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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
		GDINL Vector& operator= (Vector const& OtherVector)
		{
			if ((&OtherVector) != this)
			{
				if (this->Capacity >= OtherVector.Length)
				{
					this->Emptify();
					this->Length = OtherVector.Length;
					CopyRange(OtherVector._PtrBegin(), OtherVector._PtrEnd(), this->_PtrBegin());
				}
				else
				{
					*this = Move(Vector(OtherVector));
				}
			}
			return *this;
		}
		GDINL Vector& operator= (InitializerList<ElementType> const& InitializerList)
		{
			if (this->Capacity >= InitializerList.GetLength())
			{
				this->Emptify();
				this->Length = InitializerList.GetLength();
				CopyRange(InitializerList.begin(), InitializerList.end(), this->_PtrBegin());
			}
			else
			{
				*this = Move(Vector(InitializerList));
			}
			return *this;
		}

		GDINL ElementType& operator[] (SizeTp const Index)
		{
			return const_cast<ElementType&>(const_cast<Vector const&>(*this)[Index]);
		}
		GDINL ElementType const& operator[] (SizeTp const Index) const
		{
			GD_ASSERT(Index < this->Length, "Index is out of bounds");
			return *(this->GetData() + Index);
		}
	};	// class Vector

	template<typename ElementType>
	GDINL static bool operator== (Vector<ElementType> const& LHS, Vector<ElementType> const& RHS)
	{
		if (LHS.GetLength() == RHS.GetLength())
			return LexicographicalCompare(LHS, RHS, EqualTp<ElementType>());
		return false;
	}

	template<typename ElementType>
	GDINL static bool operator!= (Vector<ElementType> const& LHS, Vector<ElementType> const& RHS)
	{
		if (LHS.GetLength() == RHS.GetLength())
			return LexicographicalCompare(LHS, RHS, NotEqualTp<ElementType>());
		return true;
	}

	template<typename ElementType>
	GDINL static bool operator> (Vector<ElementType> const& LHS, Vector<ElementType> const& RHS)
	{
		return LexicographicalCompare(LHS, RHS, GreaterEqualTp<ElementType>());
	}

	template<typename ElementType>
	GDINL static bool operator< (Vector<ElementType> const& LHS, Vector<ElementType> const& RHS)
	{
		return LexicographicalCompare(LHS, RHS, GreaterEqualTp<ElementType>());
	}

	template<typename ElementType>
	GDINL static Vector<ElementType>& operator+= (Vector<ElementType>& LHS, Vector<ElementType>&& RHS)
	{
		typedef typename Vector<ElementType>::Iterator IteratorType;
		LHS.ReserveToLength(LHS.GetLength() + RHS.GetLength());
		for (IteratorType Iterator = RHS.Begin(); Iterator != RHS.End(); ++Iterator)
			LHS.InsertLast(Move(*Iterator));
		RHS.Clear();
		return LHS;
	}

	template<typename ElementType>
	GDINL static Vector<ElementType>& operator+= (Vector<ElementType>& LHS, Vector<ElementType> const& RHS)
	{
		typedef typename Vector<ElementType>::Iterator IteratorType;
		LHS.ReserveToLength(LHS.GetLength() + RHS.GetLength());
		for (IteratorType Iterator = RHS.Begin(); Iterator != RHS.End(); ++Iterator)
			LHS.InsertLast(*Iterator);
		return LHS;
	}

	template<typename ElementType>
	GDINL static Vector<ElementType> operator+ (Vector<ElementType> const& LHS, Vector<ElementType>&& RHS)
	{
		return Vector<ElementType>(LHS) += Forward<Vector<ElementType>>(RHS);
	}

	template<typename ElementType>
	GDINL static Vector<ElementType> operator+ (Vector<ElementType> const& LHS, Vector<ElementType> const& RHS)
	{
		return Vector<ElementType>(LHS) + Move(Vector<ElementType>(RHS));
	}

	template<typename ElementType>
	GDINL static Vector<ElementType>& operator+= (Vector<ElementType>& LHS, ElementType&& RHS)
	{
		LHS.InsertLast(Forward<ElementType>(RHS));
		return LHS;
	}

	template<typename ElementType>
	GDINL static Vector<ElementType>& operator+= (Vector<ElementType>& LHS, ElementType const& RHS)
	{
		LHS.InsertLast(RHS);
		return LHS;
	}
	
	template<typename ElementType>
	GDINL static Vector<ElementType> operator+ (Vector<ElementType> const& LHS, ElementType&& RHS)
	{
		return Vector<ElementType>(LHS) += Forward<ElementType>(RHS);
	}
	
	template<typename ElementType>
	GDINL static Vector<ElementType> operator+ (Vector<ElementType> const& LHS, ElementType const& RHS)
	{
		return Vector<ElementType>(LHS) += RHS;
	}

	template<typename SomeElementType>
	GDINL void Swap(Vector<SomeElementType>& First, Vector<SomeElementType>& Second)
	{
		if (&First != &Second)
		{
			Swap(First.Memory, Second.Memory);
			Swap(First._Length, Second._Length);
			Swap(First.Capacity, Second.Capacity);
		}
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// DVectorRef<T> class.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Defines a simple reference on @ref Vector class that can be safely passed to D by value.
	//! @tparam ElementTypeTp Element type of the desired vector.
	template<typename ElementTypeTp>
	class DVectorRef final
	{
		using ElementType = ElementTypeTp;

	private:
		ElementType const* _DataReference;
		SizeTp             _DataLength;

	public:

		// ------------------------------------------------------------------------------------------
		//! Initializes a vector reference.
		//! @param TheVector The vector to reference.
		GDINL explicit DVectorRef(Vector<ElementType> const& TheVector)
			: _DataReference(TheVector.GetData())
			, _DataLength(TheVector.GetLength())
		{
		}
	};	// class DVectorRef<ElementTypeTp>

	// ------------------------------------------------------------------------------------------
	//! Initializes a vector reference.
	//! @param TheVector The vector to reference.
	//! @returns Created D reference.
	template<typename ElementType>
	GDINL static DVectorRef<ElementType> MakeDVectorRef(Vector<ElementType> const& TheVector)
	{
		return DVectorRef<ElementType>(TheVector);
	}

GD_NAMESPACE_END
