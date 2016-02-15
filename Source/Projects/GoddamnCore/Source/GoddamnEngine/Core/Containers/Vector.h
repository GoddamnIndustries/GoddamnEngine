// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Containers/Vector.h
 * Dynamically sized array class.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Algorithm.h>
#include <GoddamnEngine/Core/Templates/Iterators.h>
#include <GoddamnEngine/Core/Containers/InitializerList.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                                 Vector<T> class.                                 ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	// **------------------------------------------------------------------------------------------**
	//! Dynamic array implementation.
	//! @tparam TElement Container element type.
	// **------------------------------------------------------------------------------------------**
	template<typename TElement, typename TAllocator = DefaultContainerAllocator>
	class Vector : public IIteratable<Vector<TElement, TAllocator>>
	{
	public:
		using ElementType          = TElement;
		using Iterator             = IndexedContainerIterator<Vector>;
		using ConstIterator        = IndexedContainerIterator<Vector const>;
		using ReverseIterator      = ReverseContainerIterator<Iterator>;
		using ReverseConstIterator = ReverseContainerIterator<ConstIterator>;

		GD_CONTAINER_DEFINE_ITERATION_SUPPORT(Vector);

	private:
		TElement* m_Memory   = nullptr;
		UInt32    m_Length   = 0;
		UInt32    m_Capacity = 0;

	public:

		// ------------------------------------------------------------------------------------------
		// Constructors and destructor.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Initializes vector with specified number of allocated and initialized elements.
		 *
		 * @param initialLength Number of elements been initialized.
		 * @param initialCapacity Initial capacity of vector. 
		 */
		GDINL explicit Vector(UInt32 const initialLength = 0, UInt32 const initialCapacity = UInt32Max)
		{
			Reserve(initialCapacity != UInt32Max ? initialCapacity : initialLength);
			m_Length = initialLength;
			InitializeRange(m_Memory, m_Memory + m_Length);
		}

		/*!
		 * Initializes vector with copy of values of specified iterators. 
		 *
		 * @param startIterator lhs Iterator would be copied.
		 * @param endIterator Last Iterator would be copied.
		 */
		template<typename TForwardIterator
#if !GD_DOCUMENTATION
			, typename = typename EnableIf<IteratorTraits<TForwardIterator>::IsForward>::Type
#endif	// if !GD_DOCUMENTATION
		>
		GDINL Vector(TForwardIterator const startIterator, TForwardIterator const endIterator)
		{
			this->Resize(static_cast<UInt32>(endIterator - startIterator));
			CopyRange(startIterator, endIterator, m_Memory);
		}

		/*!
		 * Initializes vector with default C++11's initializer list. You should not use this constructor manually.
		 * @param initializerList Initializer list passed by the compiler.
		 */
		GDINL Vector(InitializerList<TElement> const& initializerList)
			: Vector(initializerList.begin(), initializerList.end())
		{
		}
		
		/*!
		 * Initializes vector with copy of other vector.
		 * @param otherVector Vector would be copied.
		 */
		//! @{
		GDINL Vector(Vector const& otherVector)
			: Vector(otherVector.Begin(), otherVector.End())
		{
		}
		template<typename TOtherAllocator>
		GDINL implicit Vector(Vector<TOtherAllocator> const& otherVector)
			: Vector(otherVector.Begin(), otherVector.End())
		{
		}
		//! @}

		/*!
		 * Moves other vector here.
		 * @param otherVector Vector would be moved into current object.
		 */
		GDINL Vector(Vector&& otherVector)
		{
			m_Memory = otherVector.m_Memory;
			m_Length = otherVector.m_Length;
			m_Capacity = otherVector.m_Capacity;

			otherVector.m_Memory = nullptr;
			otherVector.m_Length = 0;
			otherVector.m_Capacity = 0;
		}

		GDINL ~Vector()
		{
			Clear();
		}

	public:

		// ------------------------------------------------------------------------------------------
		// Iteration API.
		// ------------------------------------------------------------------------------------------

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

		// ------------------------------------------------------------------------------------------
		// Dynamic size management.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Returns pointer to the data stored in vector.
		 */
		//! @{
		GDINL TElement const* GetData() const
		{
			return m_Memory;
		}
		GDINL TElement* GetData()
		{
			return m_Memory;
		}
		//! @}

		/*!
		 * Returns number of elements that exist in vector.
		 */
		GDINL UInt32 GetLength() const
		{
			return m_Length;
		}

		/*!
		 * Returns number of elements that can be placed into vector without reallocation.
		 */
		GDINL UInt32 GetCapacity() const
		{
			return m_Capacity;
		}

		/*!
		 * Returns true if this m_Container is empty.
		 */
		GDINL bool IsEmpty() const
		{
			return m_Length == 0;
		}

		/*!
		 * Resizes vector to make it able to contain specified number of elements.
		 * @param newLength New required length of the vector.
		 */
		GDINL void Resize(UInt32 const newLength)
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

		/*!
		 * Reserves memory for vector to make it contain specified number of elements without 
		 * reallocation when calling Resize/Insert/InsertLast method.
		 *
		 * @param newCapacity New required capacity of the vector.
		 */
		GDINL void Reserve(UInt32 const newCapacity)
		{
			// Note that elements outside range keep uninitialized.
			if (m_Capacity != newCapacity)
			{
				if (newCapacity < m_Length)
				{
					Resize(newCapacity);
				}
				TElement* const newMemory = reinterpret_cast<TElement*>(GD_MALLOC(newCapacity * sizeof(TElement)));
				MoveRange(m_Memory, m_Memory + m_Length, newMemory);
				DeinitializeRange(m_Memory, m_Memory + m_Length);
				GD_DEALLOC(m_Memory);
				m_Memory = newMemory;
				m_Capacity = newCapacity;
			}
		}

		/*!
		 * Reserves m_Memory for vector to make it contain best fitting number of elements 
		 * to predicted new size. This function incrementally grows capacity in 1.3 times.
		 *
		 * @param newLength New desired length.
		 */
		GDINL void ReserveToLength(UInt32 const newLength)
		{
			if (newLength > m_Capacity)
			{
				auto const newCapacity = 13 * newLength / 10;
				Reserve(newCapacity);
			}
		}

		/*!
		 * Shrinks vector's capacity to the specified length.
		 */
		GDINL void ShrinkToFit()
		{
			Reserve(m_Length);
		}

		/*!
		 * Destroys all elements in the vector without memory deallocation.
		 */
		GDINL void Emptify()
		{
			Resize(0);
		}

		/*!
		 * Destroys all elements in vector with memory deallocation.
		 */
		GDINL void Clear()
		{
			Reserve(0);
		}

	public:

		// ------------------------------------------------------------------------------------------
		// Dynamic elements access.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Returns reference on last element in m_Container.
		 * @returns Reference on last element in the m_Container.
		 */
		//! @{
		GDINL TElement const& GetLast() const
		{
			return (*this)[m_Length - 1];
		}
		GDINL TElement& GetLast()
		{
			return (*this)[m_Length - 1];
		}
		//! @}

		/*!
		 * Inserts specified element into collection at desired m_Index.
		 *
		 * @param index At this index new element would be inserted. All existing elements would be shifted to m_Right.
		 * @param element New element that would be inserted.
		 *
		 * @returns Index at which new element was inserted, or the value of @ref index parameter.
		 */
		//! @{
		GDINL SizeTp InsertAt(SizeTp const index, TElement&& element)
		{
			GD_DEBUG_ASSERT(index <= m_Length, "m_Index is out of bounds");
			ReserveToLength(m_Length + 1);
			for (Iterator iterator = End(); iterator != Begin() + index - 1; --iterator)
			{
				*iterator = Move(*(iterator - 1));
			}
			m_Length += 1;
			InitializeIterator(m_Memory + index, Forward<TElement>(element));
			return index;
		}
		GDINL SizeTp InsertAt(SizeTp const index, TElement const& element)
		{
			GD_DEBUG_ASSERT(index <= m_Length, "m_Index is out of bounds");
			ReserveToLength(m_Length + 1);
			for (Iterator iterator = End(); iterator != Begin() + index - 1; --iterator)
			{
				*iterator = *(iterator - 1);
			}
			m_Length += 1;
			InitializeIterator(m_Memory + index, element);
			return index;
		}
		//! @}

		/*!
		 * Appends new element to the vector.
		 *
		 * @param newElement New element that would be inserted into the end of container.
		 * @returns Index at which new element was inserted.
		 */
		//! @{
		GDINL SizeTp InsertLast(TElement&& newElement = TElement())
		{
			ReserveToLength(m_Length + 1);
			m_Length += 1;
			InitializeIterator(End() - 1, Forward<TElement>(newElement));
			return m_Length - 1;
		}
		GDINL SizeTp InsertLast(TElement const& newElement)
		{
			ReserveToLength(m_Length + 1);
			m_Length += 1;
			InitializeIterator(End() - 1, newElement);
			return m_Length - 1;
		}
		//! @}

		/*!
		 * Removes existing element from array at specified index.
		 * @param index Element at this index would be removed. All other elements would be shifted to left.
		 */
		GDINL void EraseAt(SizeTp const index)
		{
			GD_ASSERT(index < m_Length, "m_Index is out of bounds");
			for (Iterator iterator = Begin() + index; iterator != (End() - 1); ++iterator)
			{
				*iterator = Move(*(iterator + 1));
			}
			DeinitializeIterator(End() - 1);
			m_Length -= 1;
		}

		/*!
		 * Removes last element from vector.
		 */
		GDINL void EraseLast()
		{
			GD_DEBUG_ASSERT(m_Length != 0, "vector size is zero");
			Resize(m_Length - 1);
		}

		/*!
		 * Searches for first element in vector by checking equality.
		 *
		 * @param element Object that function would be looking for.
		 * @returns Index of found element or @c SizeTpMax if nothing was found.
		 */
		GDINL SizeTp FindFirst(TElement const& element) const
		{
			for (SizeTp iterator = 0; iterator < m_Length; ++iterator)
			{
				if ((*this)[iterator] == element)
					return iterator;
			}
			return SizeTpMax;
		}

	public:

		// ------------------------------------------------------------------------------------------
		// Overloaded operators.
		// ------------------------------------------------------------------------------------------

		// Vector = something
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
					this->~Vector();
					new(this) Vector(otherVector);
				}
			}
			return *this;
		}
		template<typename TOtherAllocator>
		GDINL Vector& operator= (Vector<TOtherAllocator> const& otherVector)
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
					this->~Vector();
					new(this) Vector(otherVector);
				}
			}
			return *this;
		}
		GDINL Vector& operator= (InitializerList<TElement> const& initializerList)
		{
			if (m_Capacity >= initializerList.size())
			{
				Emptify();
				m_Length = initializerList.size();
				CopyRange(initializerList.begin(), initializerList.end(), m_Memory);
			}
			else
			{
				this->~Vector();
				new(this) Vector(initializerList);
			}
			return *this;
		}

		// Vector[]
		GDINL TElement const& operator[] (SizeTp const index) const
		{
			GD_DEBUG_ASSERT(index < m_Length, "Index is out of bounds");
			return GetData()[index];
		}
		GDINL TElement& operator[] (SizeTp const index)
		{
			return const_cast<TElement&>(const_cast<Vector const&>(*this)[index]);
		}

		// Vector == Vector
		template<typename TOtherAllocator>
		GDINL friend static bool operator== (Vector<TElement> const& lhs, Vector<TElement, TOtherAllocator> const& rhs)
		{
			return lhs.GetLength() == rhs.GetLength() ? LexicographicalCompare(lhs, rhs, EqualTp<TElement>()) : false;
		}
		template<typename TOtherAllocator>
		GDINL friend static bool operator!= (Vector<TElement> const& lhs, Vector<TElement, TOtherAllocator> const& rhs)
		{
			return lhs.GetLength() == rhs.GetLength() ? LexicographicalCompare(lhs, rhs, NotEqualTp<TElement>()) : true;
		}

		// Vector > Vector
		template<typename TOtherAllocator>
		GDINL friend static bool operator> (Vector<TElement> const& lhs, Vector<TElement, TOtherAllocator> const& rhs)
		{
			return LexicographicalCompare(lhs, rhs, GreaterEqualTp<TElement>());
		}
		template<typename TOtherAllocator>
		GDINL friend static bool operator< (Vector<TElement> const& lhs, Vector<TElement, TOtherAllocator> const& rhs)
		{
			return LexicographicalCompare(lhs, rhs, LessEqualTp<TElement>());
		}

		// Vector += Vector
		template<typename TOtherAllocator>
		GDINL friend static Vector<TElement>& operator+= (Vector<TElement>& lhs, Vector<TElement, TOtherAllocator>&& rhs)
		{
			typedef typename Vector<TElement>::Iterator IteratorType;
			lhs.ReserveToLength(lhs.GetLength() + rhs.GetLength());
			for (IteratorType iterator = rhs.Begin(); iterator != rhs.End(); ++iterator)
			{
				lhs.InsertLast(Move(*iterator));
			}
			rhs.Clear();
			return lhs;
		}
		template<typename TOtherAllocator>
		GDINL friend static Vector<TElement>& operator+= (Vector<TElement>& lhs, Vector<TElement, TOtherAllocator> const& rhs)
		{
			typedef typename Vector<TElement>::Iterator IteratorType;
			lhs.ReserveToLength(lhs.GetLength() + rhs.GetLength());
			for (IteratorType iterator = rhs.Begin(); iterator != rhs.End(); ++iterator)
			{
				lhs.InsertLast(*iterator);
			}
			return lhs;
		}

		// Vector += Element
		GDINL friend static Vector<TElement>& operator+= (Vector<TElement>& lhs, TElement&& rhs)
		{
			lhs.InsertLast(Forward<TElement>(rhs));
			return lhs;
		}
		GDINL friend static Vector<TElement>& operator+= (Vector<TElement>& lhs, TElement const& rhs)
		{
			lhs.InsertLast(rhs);
			return lhs;
		}

		// Vector + Vector
		template<typename TOtherAllocator>
		GDINL friend static Vector<TElement> operator+ (Vector<TElement> const& lhs, Vector<TElement, TOtherAllocator>&& rhs)
		{
			return Vector<TElement>(lhs) += Forward<Vector<TElement>>(rhs);
		}
		template<typename TOtherAllocator>
		GDINL friend static Vector<TElement> operator+ (Vector<TElement> const& lhs, Vector<TElement, TOtherAllocator> const& rhs)
		{
			return Vector<TElement>(lhs) + Move(Vector<TElement>(rhs));
		}

		// Vector + Element
		GDINL friend static Vector<TElement> operator+ (Vector<TElement> const& lhs, TElement&& rhs)
		{
			return Vector<TElement>(lhs) += Forward<TElement>(rhs);
		}
		GDINL friend static Vector<TElement> operator+ (Vector<TElement> const& lhs, TElement const& rhs)
		{
			return Vector<TElement>(lhs) += rhs;
		}

	};	// class Vector

	/*!
	 * Dynamic stack implementation.
	 */
	template<typename TElement>
	using Stack = Vector<TElement>;

GD_NAMESPACE_END
