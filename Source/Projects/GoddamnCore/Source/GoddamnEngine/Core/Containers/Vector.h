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
	//! Dynamic array Implementation.
	//! @tparam TElement m_Container element type.
	template<typename TElement>
	class Vector final
	{
	public:
		using ElementType          = TElement;
		using Iterator             = IndexedContainerIterator<Vector>;
		using ConstIterator        = IndexedContainerIterator<Vector const>;
		using ReverseIterator      = ReverseContainerIterator<Iterator>;
		using ReverseConstIterator = ReverseContainerIterator<ConstIterator>;

		GD_CONTAINER_DEFINE_ITERATION_SUPPORT(Vector);

	private:
		ElementType* m_Memory   = nullptr;
		SizeTp       m_Length   = 0;
		SizeTp       m_Capacity = 0;

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructors and destructor.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Initializes vector with specified number of allocated and initialized elements.
		//! @param initialLength Number of elements been initialized.
		//! @param initialCapacity Initial m_Capacity of vector. 
		GDINL explicit Vector(SizeTp const initialLength = 0, SizeTp const initialCapacity = SizeTpMax)
		{
			Reserve(initialCapacity != SizeTpMax ? initialCapacity : initialLength);
			m_Length = initialLength;
			InitializeRange(m_Memory, m_Memory + m_Length);
		}

		// ------------------------------------------------------------------------------------------
		//! Initializes vector with copy of values of specified iterators. 
		//! @param startIterator lhs Iterator would be copied.
		//! @param endIterator Last Iterator would be copied.
		template<typename TForwardIterator
#if !GD_DOCUMENTATION
			, typename = typename EnableIf<IteratorTraits<TForwardIterator>::IsForward>::Type
#endif	// if !GD_DOCUMENTATION
		>
		GDINL Vector(TForwardIterator const startIterator, TForwardIterator const endIterator)
		{
			Resize(endIterator - startIterator);
			CopyRange(startIterator, endIterator, m_Memory);
		}

		// ------------------------------------------------------------------------------------------
		//! Initializes vector with default C++11's initializer list. You should not use this constructor manually.
		//! @param initializerList Initializer list passed by the compiler.
		GDINL Vector(InitializerList<ElementType> const& initializerList)
		{
			Resize(initializerList.size());
			CopyRange(initializerList.begin(), initializerList.end(), Begin());
		}

		// ------------------------------------------------------------------------------------------
		//! Moves other vector here.
		//! @param otherVector Vector would be moved into current object.
		GDINL Vector(Vector&& otherVector)
		{
			m_Memory = otherVector.m_Memory;
			m_Length = otherVector.m_Length;
			m_Capacity = otherVector.m_Capacity;

			otherVector.m_Memory = nullptr;
			otherVector.m_Length = 0;
			otherVector.m_Capacity = 0;
		}

		// ------------------------------------------------------------------------------------------
		//! Initializes vector with copy of other vector.
		//! @param otherVector Vector would be copied.
		GDINL Vector(Vector const& otherVector)
		{
			Resize(otherVector.m_Length);
			CopyRange(otherVector.m_Memory, otherVector.m_Memory + otherVector.m_Length, m_Memory);
		}

		// ------------------------------------------------------------------------------------------
		//! Deinitializes all elements and deallocates m_Memory.
		GDINL ~Vector()
		{
			Clear();
		}

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Iteration API.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		GDINL Iterator Begin()
		{
			return Iterator(*this);
		}
		GDINL ConstIterator Begin() const
		{
			return ConstIterator(*this);
		}
		
		GDINL Iterator End()
		{
			return Begin() + m_Length;
		}
		GDINL ConstIterator End() const
		{
			return Begin() + m_Length;
		}

		GDINL ReverseIterator ReverseBegin()
		{
			return ReverseIterator(End() - 1);
		}
		GDINL ReverseConstIterator ReverseBegin() const
		{
			return ReverseConstIterator(End() - 1);
		}

		GDINL ReverseIterator ReverseEnd()
		{
			return ReverseIterator(Begin() - 1);
		}
		GDINL ReverseConstIterator ReverseEnd() const
		{
			return ReverseConstIterator(Begin() - 1);
		}

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Dynamic size management.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Returns pointer to the data stored in vector.
		//! @returns pointer to the data stored in vector.
		//! @{
		GDINL ElementType const* GetData() const
		{
			return m_Memory;
		}
		GDINL ElementType* GetData()
		{
			return m_Memory;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns number of elements that exist in vector.
		//! @returns Number of elements that exist in vector.
		GDINL SizeTp GetLength() const
		{
			return m_Length;
		}

		// ------------------------------------------------------------------------------------------
		//! Returns number of elements that can be placed into vector without reallocation.
		//! @returns Number of elements that can be placed into vector without reallocation.
		GDINL SizeTp GetCapacity() const
		{
			return m_Capacity;
		}

		// ------------------------------------------------------------------------------------------
		//! Returns true if this m_Container is empty.
		//! @returns True if this m_Container is empty, false otherwise.
		GDINL bool IsEmpty() const
		{
			return m_Length == 0;
		}

		// ------------------------------------------------------------------------------------------
		//! Resizes vector to make it able to contain specified number of elements.
		//! @param newLength New required m_Length of the m_Container.
		GDINL void Resize(SizeTp const newLength)
		{
			if (m_Length != newLength)
			{	
				if (m_Length < newLength)
				{	
					ReserveToLength(newLength);
					InitializeRange(m_Memory + m_Length, m_Memory + newLength);
				}
				else
				{	
					DeinitializeRange(m_Memory + newLength, m_Memory + m_Length);
				}
				m_Length = newLength;
			}
		}

		// ------------------------------------------------------------------------------------------
		//! Reserves m_Memory for vector to make it contain specified number of elements without 
		//! reallocation when calling Resize/Insert/InsertLast method.
		//! @param newCapacity New required m_Capacity of the m_Container.
		GDINL void Reserve(SizeTp const newCapacity)
		{
			// Note that elements outside range keep uninitialized.
			if (m_Capacity != newCapacity)
			{
				if (newCapacity < m_Length)
					Resize(newCapacity);
				ElementType* const newMemory = reinterpret_cast<ElementType*>(GD_MALLOC(newCapacity * sizeof(ElementType)));
				MoveRange(m_Memory, m_Memory + m_Length, newMemory);
				DeinitializeRange(m_Memory, m_Memory + m_Length);
				m_Memory = newMemory;
				m_Capacity = newCapacity;
			}
		}

		// ------------------------------------------------------------------------------------------
		//! Reserves m_Memory for vector to make it contain best fitting number of elements 
		//! to predicted new size. This function incrementally grows m_Capacity in 1.3 times.
		//! @param newLength New desired m_Length on which best m_Capacity is computed.
		GDINL void ReserveToLength(SizeTp const newLength)
		{
			if (newLength > m_Capacity)
			{
				SizeTp const newCapacity = 13 * (Max(m_Capacity, newLength) + 1) / 10;
				Reserve(newCapacity);
			}
		}

		// ------------------------------------------------------------------------------------------
		//! Shrinks vector's m_Capacity to m_Length.
		GDINL void ShrinkToFit()
		{
			Reserve(m_Length);
		}

		// ------------------------------------------------------------------------------------------
		//! Destroys all elements in m_Container without m_Memory deallocation.
		GDINL void Emptify()
		{
			Resize(0);
		}

		// ------------------------------------------------------------------------------------------
		//! Destroys all elements in m_Container with m_Memory deallocation.
		GDINL void Clear()
		{
			Reserve(0);
		}

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Dynamic elements access.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Returns reference on last element in m_Container.
		//! @returns Reference on last element in the m_Container.
		//! @{
		GDINL ElementType const& GetLast() const
		{
			return (*this)[m_Length - 1];
		}
		GDINL ElementType& GetLast()
		{
			return (*this)[m_Length - 1];
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Inserts specified element into collection at desired m_Index.
		//! @param m_Index At this m_Index new element would be inserted. All existing elements would be shifted to m_Right.
		//! @param element New element that would be inserted.
		//! @returns m_Index at which new element was inserted, or the value of @ref m_Index param.
		//! @{
		GDINL SizeTp InsertAt(SizeTp const index, ElementType&& element)
		{
			GD_DEBUG_ASSERT(index <= m_Length, "m_Index is out of bounds");
			ReserveToLength(m_Length + 1);
			for (Iterator iterator = End(); iterator != Begin() + index - 1; --iterator)
				*iterator = Move(*(iterator - 1));
			m_Length += 1;
			InitializeIterator(m_Memory + index, Forward<ElementType>(element));
			return index;
		}
		GDINL SizeTp InsertAt(SizeTp const index, ElementType const& element)
		{
			GD_DEBUG_ASSERT(index <= m_Length, "m_Index is out of bounds");
			ReserveToLength(m_Length + 1);
			for (Iterator iterator = End(); iterator != Begin() + index - 1; --iterator)
				*iterator = *(iterator - 1);
			m_Length += 1;
			InitializeIterator(m_Memory + index, element);
			return index;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Appends new element to m_Container.
		//! @param NewElement New element that would be inserted into the end of m_Container.
		//! @returns m_Index at which new element was inserted.
		//! @{
		GDINL SizeTp InsertLast(ElementType&& NewElement = ElementType())
		{
			ReserveToLength(m_Length + 1);
			m_Length += 1;
			InitializeIterator(End() - 1, Forward<ElementType>(NewElement));
			return m_Length - 1;
		}
		GDINL SizeTp InsertLast(ElementType const& NewElement)
		{
			ReserveToLength(m_Length + 1);
			m_Length += 1;
			InitializeIterator(End() - 1, NewElement);
			return m_Length - 1;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Removes existing element from array at specified m_Index.
		//! @param m_Index element at this m_Index would be removed. All other elements would be shifted to m_Left.
		GDINL void EraseAt(SizeTp const Index)
		{
			GD_ASSERT(Index < m_Length, "m_Index is out of bounds");
			for (Iterator iterator = Begin() + Index; iterator != (End() - 1); ++iterator)
				*iterator = Move(*(iterator + 1));
			m_Length -= 1;
			DeinitializeIterator(End());
		}

		// ------------------------------------------------------------------------------------------
		//! Removes last element from m_Container.
		GDINL void EraseLast()
		{
			GD_DEBUG_ASSERT(m_Length != 0, "m_Container size is zero");
			Resize(m_Length - 1);
		}

		// ------------------------------------------------------------------------------------------
		//! Searches for first element in m_Container by checking equality.
		//! @param element Object that function would be looking for.
		//! @returns m_Index of found element or @c SizeTpMax if nothing was found.
		GDINL SizeTp FindFirst(ElementType const& element) const
		{
			for (SizeTp iterator = 0; iterator < m_Length; ++iterator)
			{
				if ((*this)[iterator] == element)
					return iterator;
			}
			return SizeTpMax;
		}

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Overloaded operators.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		GDINL Vector& operator= (Vector&& otherVector)
		{
			if (&otherVector != this)
			{
				Clear();
				m_Memory = otherVector.m_Memory;
				m_Length = otherVector.m_Length;
				m_Capacity = otherVector.m_Capacity;

				otherVector.m_Memory = nullptr;
				otherVector.m_Length = 0;
				otherVector.m_Capacity = 0;
			}
			return *this;
		}
		GDINL Vector& operator= (Vector const& otherVector)
		{
			if ((&otherVector) != this)
			{
				if (m_Capacity >= otherVector.m_Length)
				{
					Emptify();
					m_Length = otherVector.m_Length;
					CopyRange(otherVector.m_Memory, otherVector.m_Memory + otherVector.m_Length, m_Memory);
				}
				else
				{
					*this = Move(Vector(otherVector));
				}
			}
			return *this;
		}
		GDINL Vector& operator= (InitializerList<ElementType> const& InitializerList)
		{
			if (m_Capacity >= InitializerList.size())
			{
				Emptify();
				m_Length = InitializerList.size();
				CopyRange(InitializerList.begin(), InitializerList.end(), m_Memory);
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
			GD_ASSERT(Index < m_Length, "m_Index is out of bounds");
			return *(GetData() + Index);
		}
	};	// class Vector

	template<typename TElement>
	GDINL static bool operator== (Vector<TElement> const& lhs, Vector<TElement> const& rhs)
	{
		if (lhs.GetLength() == rhs.GetLength())
			return LexicographicalCompare(lhs, rhs, EqualTp<TElement>());
		return false;
	}

	template<typename TElement>
	GDINL static bool operator!= (Vector<TElement> const& lhs, Vector<TElement> const& rhs)
	{
		if (lhs.GetLength() == rhs.GetLength())
			return LexicographicalCompare(lhs, rhs, NotEqualTp<TElement>());
		return true;
	}

	template<typename TElement>
	GDINL static bool operator> (Vector<TElement> const& lhs, Vector<TElement> const& rhs)
	{
		return LexicographicalCompare(lhs, rhs, GreaterEqualTp<TElement>());
	}

	template<typename TElement>
	GDINL static bool operator< (Vector<TElement> const& lhs, Vector<TElement> const& rhs)
	{
		return LexicographicalCompare(lhs, rhs, GreaterEqualTp<TElement>());
	}

	template<typename TElement>
	GDINL static Vector<TElement>& operator+= (Vector<TElement>& lhs, Vector<TElement>&& rhs)
	{
		typedef typename Vector<TElement>::Iterator IteratorType;
		lhs.ReserveToLength(lhs.GetLength() + rhs.GetLength());
		for (IteratorType iterator = rhs.Begin(); iterator != rhs.End(); ++iterator)
			lhs.InsertLast(Move(*iterator));
		rhs.Clear();
		return lhs;
	}

	template<typename TElement>
	GDINL static Vector<TElement>& operator+= (Vector<TElement>& lhs, Vector<TElement> const& rhs)
	{
		typedef typename Vector<TElement>::Iterator IteratorType;
		lhs.ReserveToLength(lhs.GetLength() + rhs.GetLength());
		for (IteratorType iterator = rhs.Begin(); iterator != rhs.End(); ++iterator)
			lhs.InsertLast(*iterator);
		return lhs;
	}

	template<typename TElement>
	GDINL static Vector<TElement> operator+ (Vector<TElement> const& lhs, Vector<TElement>&& rhs)
	{
		return Vector<TElement>(lhs) += Forward<Vector<TElement>>(rhs);
	}

	template<typename TElement>
	GDINL static Vector<TElement> operator+ (Vector<TElement> const& lhs, Vector<TElement> const& rhs)
	{
		return Vector<TElement>(lhs) + Move(Vector<TElement>(rhs));
	}

	template<typename TElement>
	GDINL static Vector<TElement>& operator+= (Vector<TElement>& lhs, TElement&& rhs)
	{
		lhs.InsertLast(Forward<TElement>(rhs));
		return lhs;
	}

	template<typename TElement>
	GDINL static Vector<TElement>& operator+= (Vector<TElement>& lhs, TElement const& rhs)
	{
		lhs.InsertLast(rhs);
		return lhs;
	}
	
	template<typename TElement>
	GDINL static Vector<TElement> operator+ (Vector<TElement> const& lhs, TElement&& rhs)
	{
		return Vector<TElement>(lhs) += Forward<TElement>(rhs);
	}
	
	template<typename TElement>
	GDINL static Vector<TElement> operator+ (Vector<TElement> const& lhs, TElement const& rhs)
	{
		return Vector<TElement>(lhs) += rhs;
	}

	template<typename TSomeElement>
	GDINL void Swap(Vector<TSomeElement>& lhs, Vector<TSomeElement>& rhs)
	{
		if (&lhs != &rhs)
		{
			Swap(lhs.m_Memory, rhs.m_Memory);
			Swap(lhs.m_Length, rhs.m_Length);
			Swap(lhs.capacity, rhs.capacity);
		}
	}

GD_NAMESPACE_END
