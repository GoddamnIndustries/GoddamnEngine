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
#include <GoddamnEngine/Core/Templates/Algorithm.h>
#include <GoddamnEngine/Core/Templates/Iterators.h>
#include <GoddamnEngine/Core/Containers/InitializerList.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                                 Vector<T> class.                                 ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	// **------------------------------------------------------------------------------------------**
	//! Dynamic array implementation.
	//! @tparam TElement Vector element type.
	//! @tparam TAllocator Allocator used by this vector.
	// **------------------------------------------------------------------------------------------**
	template<typename TElement, typename TAllocator = HeapAllocator>
	class Vector : private TAllocator
	{
	public:
		using AllocatorType        = TAllocator;
		using ElementType          = TElement;
		using ConstElementType     = TElement const;
		using ReferenceType        = TElement&;
		using ConstReferenceType   = TElement const&;
		using Iterator             = IndexedContainerIterator<Vector>;
		using ConstIterator        = IndexedContainerIterator<Vector const>;
		using ReverseIterator      = ReverseContainerIterator<Iterator>;
		using ReverseConstIterator = ReverseContainerIterator<ConstIterator>;

		GD_CONTAINER_DEFINE_ITERATION_SUPPORT(Vector)

	private:
		TElement* m_Memory;
		SizeTp    m_Length;
		SizeTp    m_Capacity;

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
		//! @{
		GDINL explicit Vector()
			: m_Memory(nullptr), m_Length(0), m_Capacity(0)
		{}
		GDINL explicit Vector(SizeTp const initialLength)
			: m_Memory(nullptr), m_Length(0), m_Capacity(0)
		{
			Resize(initialLength);
		}
		GDINL explicit Vector(SizeTp const initialLength, SizeTp const initialCapacity)
			: m_Memory(nullptr), m_Length(0), m_Capacity(0)
		{
			Reserve(initialCapacity);
			Resize(initialLength);
		}
		//! @}

		/*!
		 * Initializes vector with copy of values of specified iterators. 
		 *
		 * @param startIterator lhs Iterator would be copied.
		 * @param endIterator Last Iterator would be copied.
		 */
		template<typename TForwardIterator, typename = typename EnableIf<IteratorTraits<TForwardIterator>::IsForward>::Type>
		GDINL Vector(TForwardIterator const startIterator, TForwardIterator const endIterator)
			: m_Memory(nullptr), m_Length(0), m_Capacity(0)
		{
			Resize(static_cast<SizeTp>(endIterator - startIterator));
			Algo::CopyRange(startIterator, endIterator, Begin());
		}

		/*!
		 * Initializes vector with default C++11's initializer list. You should not use this constructor manually.
		 * @param initializerList Initializer list passed by the compiler.
		 */
		GDINL Vector(InitializerList<TElement> const& initializerList)
			: Vector(initializerList.begin(), initializerList.end())
		{}
		
		/*!
		 * Initializes vector with copy of other vector.
		 * @param otherVector Vector would be copied.
		 */
		//! @{
		GDINL Vector(Vector const& otherVector)
			: Vector(otherVector.Begin(), otherVector.End())
		{}
		template<typename TOtherAllocator>
		GDINL implicit Vector(Vector<TOtherAllocator> const& otherVector)
			: Vector(otherVector.Begin(), otherVector.End())
		{}
		//! @}

		/*!
		 * Moves other vector here.
		 * @param otherVector Vector would be moved into current object.
		 */
		//! @{
		GDINL Vector(Vector&& otherVector) noexcept
			: m_Memory(otherVector.m_Memory), m_Length(otherVector.m_Length), m_Capacity(otherVector.m_Capacity)
		{
			otherVector.m_Memory = nullptr;
			otherVector.m_Length = 0;
			otherVector.m_Capacity = 0;
		}
		template<typename TOtherAllocator>
		GDINL implicit Vector(Vector<TOtherAllocator>&& otherVector)
			: m_Memory(nullptr), m_Length(0), m_Capacity(0)
		{
			this->Resize(otherVector.m_Length);
			Algo::MoveRange(otherVector.Begin(), otherVector.End(), Begin());
			otherVector.Clear();
		}
		//! @}

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
		GDINL SizeTp GetLength() const
		{
			return m_Length;
		}

		/*!
		 * Returns number of elements that can be placed into vector without reallocation.
		 */
		GDINL SizeTp GetCapacity() const
		{
			return m_Capacity;
		}

		/*!
		 * Returns true if this vector is empty.
		 */
		GDINL bool IsEmpty() const
		{
			return m_Length == 0;
		}

		/*!
		 * Resizes vector to make it able to contain specified number of elements.
		 * @param newLength New required length of the vector.
		 */
		GDINL void Resize(SizeTp const newLength)
		{
			if (m_Length != newLength)
			{	
				if (m_Length < newLength)
				{	
					ReserveToLength(newLength);
					Algo::InitializeRange(m_Memory + m_Length, m_Memory + newLength);
				}
				else
				{	
					Algo::DeinitializeRange(m_Memory + newLength, m_Memory + m_Length);
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
		GDINL void Reserve(SizeTp const newCapacity)
		{
			if (m_Capacity != newCapacity)
			{
				if (newCapacity < m_Length)
				{
					Resize(newCapacity);
				}
				
				auto const newMemory = GD_MALLOC_ARRAY_T(TElement, newCapacity);
				Algo::MoveRange(m_Memory, m_Memory + m_Length, newMemory);
				Algo::DeinitializeRange(m_Memory, m_Memory + m_Length);
				GD_FREE(m_Memory);
				
				m_Memory = newMemory;
				m_Capacity = newCapacity;
			}
		}

		/*!
		 * Reserves memory for vector to make it contain best fitting number of elements 
		 * to predicted new size. This function incrementally grows capacity in 1.3 times.
		 *
		 * @param newLength New desired length.
		 */
		GDINL void ReserveToLength(SizeTp const newLength)
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
		 * Returns reference on last element in the vector.
		 */
		//! @{
		GDINL TElement const& GetLast() const
		{
			return (*this)[m_Length - 1];
		}
		GDINL TElement& GetLast()
		{
			return const_cast<TElement&>(const_cast<Vector const*>(this)->GetLast());
		}
		//! @}

		/*!
		 * Inserts specified element into collection at desired index.
		 *
		 * @param index At this index new element would be inserted. All existing elements would be shifted to right.
		 * @param element New element that would be inserted.
		 *
		 * @returns Index at which new element was inserted, or the value of @ref index parameter.
		 */
		//! @{
		GDINL SizeTp InsertAt(SizeTp const index, TElement&& element = TElement())
		{
			GD_DEBUG_VERIFY(index <= m_Length, "Index is out of bounds");

			ReserveToLength(m_Length + 1);
			m_Length += 1;
			for (auto iterator = End() - 1; iterator != Begin() + index; --iterator)
			{
				*iterator = Utils::Move(*(iterator - 1));
			}
			/*for (auto iterator = ReverseBegin(); iterator != ReverseEnd() - index + 1; ++iterator)
			{
				*iterator = Utils::Move(*(iterator + 1));
			}*/
			//Algo::MoveRange(ReverseBegin(), ReverseEnd() - index, ReverseBegin() + 1);
			Algo::InitializeIterator(Begin() + index, Utils::Forward<TElement>(element));
			return index;
		}
		GDINL SizeTp InsertAt(SizeTp const index, TElement const& element)
		{
			GD_DEBUG_VERIFY(index <= m_Length, "Index is out of bounds");

			ReserveToLength(m_Length + 1);
			m_Length += 1;
			for (auto iterator = End() - 1; iterator != Begin() + index - 1; --iterator)
			{
				*iterator = Utils::Move(*(iterator - 1));
			}
			//Algo::MoveRange(ReverseBegin(), ReverseEnd() - index, ReverseBegin() + 1);
			Algo::InitializeIterator(Begin() + index, element);
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
			Algo::InitializeIterator(End() - 1, Utils::Forward<TElement>(newElement));
			return m_Length - 1;
		}
		GDINL SizeTp InsertLast(TElement const& newElement)
		{
			ReserveToLength(m_Length + 1);
			m_Length += 1;
			Algo::InitializeIterator(End() - 1, newElement);
			return m_Length - 1;
		}
		//! @}

		/*!
		 * Removes existing element from array at specified index.
		 *
		 * @param index Element at this index would be removed. All other elements would be shifted to left.
		 * @param amount An amount of elements to remove.
		 */
		GDINL void EraseAt(SizeTp const index, SizeTp const amount = 1)
		{
			GD_DEBUG_VERIFY(index + amount <= m_Length, "Index is out of bounds");
			for (auto iterator = Begin() + index; iterator != (End() - amount); ++iterator)
			{
				*iterator = Utils::Move(*(iterator + amount));
			}
			Algo::DeinitializeRange(End() - amount, End());
			m_Length -= amount;
		}

		/*!
		 * Removes last element from vector.
		 */
		GDINL void EraseLast()
		{
			GD_DEBUG_VERIFY(m_Length != 0, "vector size is zero");
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
			for (SizeTp cnt = 0; cnt < m_Length; ++cnt)
			{
				if ((*this)[cnt] == element)
				{
					return cnt;
				}
			}
			return SizeTpMax;
		}

	public:

		// ------------------------------------------------------------------------------------------
		// Overloaded operators.
		// ------------------------------------------------------------------------------------------

		// vector = something
		GDINL Vector& operator= (Vector&& otherVector) noexcept
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
			if (&otherVector != this)
			{
				if (m_Capacity >= otherVector.m_Length)
				{
					Emptify();
					m_Length = otherVector.m_Length;
					Algo::CopyRange(otherVector.m_Memory, otherVector.m_Memory + otherVector.m_Length, m_Memory);
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
			if (&otherVector != this)
			{
				if (m_Capacity >= otherVector.m_Length)
				{
					Emptify();
					m_Length = otherVector.m_Length;
					Algo::CopyRange(otherVector.m_Memory, otherVector.m_Memory + otherVector.m_Length, m_Memory);
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
				Algo::CopyRange(initializerList.begin(), initializerList.end(), m_Memory);
			}
			else
			{
				this->~Vector();
				new(this) Vector(initializerList);
			}
			return *this;
		}

		// vector[]
		GDINL TElement const& operator[] (SizeTp const index) const
		{
			GD_DEBUG_VERIFY(index < m_Length, "Index is out of bounds");
			return GetData()[index];
		}
		GDINL TElement& operator[] (SizeTp const index)
		{
			return const_cast<TElement&>(const_cast<Vector const&>(*this)[index]);
		}

		// vector == vector
		template<typename TOtherAllocator>
		GDINL friend bool operator== (Vector const& lhs, Vector<TElement, TOtherAllocator> const& rhs)
		{
			return lhs.m_Length == rhs.m_Length && Algo::LexicographicalCompare(lhs, rhs, TEqual<TElement>());
		}
		template<typename TOtherAllocator>
		GDINL friend bool operator!= (Vector const& lhs, Vector<TElement, TOtherAllocator> const& rhs)
		{
			return lhs.m_Length == rhs.m_Length || Algo::LexicographicalCompare(lhs, rhs, TNotEqual<TElement>());
		}

		// vector > vector
		template<typename TOtherAllocator>
		GDINL friend bool operator< (Vector const& lhs, Vector<TElement, TOtherAllocator> const& rhs)
		{
			return Algo::LexicographicalLess(lhs, rhs);
		}
		template<typename TOtherAllocator>
		GDINL friend bool operator> (Vector const& lhs, Vector<TElement, TOtherAllocator> const& rhs)
		{
			return rhs < lhs;
		}

		// vector >= vector
		template<typename TOtherAllocator>
		GDINL friend bool operator<= (Vector const& lhs, Vector<TElement, TOtherAllocator> const& rhs)
		{
			return !(lhs > rhs);
		}
		template<typename TOtherAllocator>
		GDINL friend bool operator>= (Vector const& lhs, Vector<TElement, TOtherAllocator> const& rhs)
		{
			return !(lhs < rhs);
		}

		// vector += vector
		template<typename TOtherAllocator>
		GDINL friend Vector& operator+= (Vector& lhs, Vector<TElement, TOtherAllocator>&& rhs)
		{
			lhs.ReserveToLength(lhs.m_Length + rhs.m_Length);
			for (auto iterator = rhs.Begin(); iterator != rhs.End(); ++iterator)
			{
				lhs.InsertLast(Move(*iterator));
			}
			rhs.Clear();
			return lhs;
		}
		template<typename TOtherAllocator>
		GDINL friend Vector& operator+= (Vector& lhs, Vector<TElement, TOtherAllocator> const& rhs)
		{
			lhs.ReserveToLength(lhs.m_Length + rhs.m_Length);
			for (auto iterator = rhs.Begin(); iterator != rhs.End(); ++iterator)
			{
				lhs.InsertLast(*iterator);
			}
			return lhs;
		}

		// vector += Element
		GDINL friend Vector& operator+= (Vector& lhs, TElement&& rhs)
		{
			lhs.InsertLast(Forward<TElement>(rhs));
			return lhs;
		}
		GDINL friend Vector& operator+= (Vector& lhs, TElement const& rhs)
		{
			lhs.InsertLast(rhs);
			return lhs;
		}

		// vector + vector
		template<typename TOtherAllocator>
		GDINL friend Vector operator+ (Vector const& lhs, Vector<TElement, TOtherAllocator>&& rhs)
		{
			return Vector(lhs) += Forward<Vector<TElement, TOtherAllocator>>(rhs);
		}
		template<typename TOtherAllocator>
		GDINL friend Vector operator+ (Vector const& lhs, Vector<TElement, TOtherAllocator> const& rhs)
		{
			return Vector(lhs) + rhs;
		}

	};	// class Vector

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                               Vector<bool> class.                                ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	// **------------------------------------------------------------------------------------------**
	//! Dynamic bitset optimized implementation.
	// **------------------------------------------------------------------------------------------**
	template<typename TAllocator>
	class Vector<bool, TAllocator> : private TAllocator
	{
	public:
		using Word = UInt64;

		class ConstReferenceType final
		{
		private:
			Word const& m_WordRef;
			Word        m_WordMask;

		public:
			GDINL ConstReferenceType(Word const& wordRef, Word const wordIndex)
				: m_WordRef(wordRef), m_WordMask(GD_BIT(wordIndex))
			{}

		public:

			// (bit)const_bit_reference
			GDINL implicit operator bool() const
			{
				return (m_WordRef & m_WordMask) != 0;
			}

		};	// class ConstReferenceType

		class ReferenceType final
		{
		private:
			Word& m_WordRef;
			Word  m_WordMask;

		public:
			GDINL ReferenceType(Word& wordRef, Word const wordIndex)
				: m_WordRef(wordRef), m_WordMask(GD_BIT(wordIndex))
			{}

		public:

			// (bit)bit_reference
			GDINL implicit operator bool() const
			{
				return (m_WordRef & m_WordMask) != 0;
			}

			// (const_bit_reference)bit_reference
			GDINL implicit operator ConstReferenceType() const
			{
				return union_cast<ConstReferenceType>(*this);
			}

			// bit_reference = ...
			GDINL ReferenceType& operator= (bool const bit)
			{
				if (bit)
				{
					m_WordRef |= m_WordMask;
				}
				else
				{
					m_WordRef &= ~m_WordMask;
				}
				return *this;
			}
			GDINL ReferenceType& operator= (ReferenceType const& ref)
			{
				return *this = static_cast<bool>(ref);
			}
			GDINL ReferenceType& operator= (ConstReferenceType const& cref)
			{
				return *this = static_cast<bool>(cref);
			}
			
		};	// class ReferenceType

		using AllocatorType        = TAllocator;
		using ElementType          = ReferenceType;
		using ConstElementType     = ConstReferenceType;
		using Iterator             = IndexedContainerIterator<Vector>;
		using ConstIterator        = IndexedContainerIterator<Vector const>;
		using ReverseIterator      = ReverseContainerIterator<Iterator>;
		using ReverseConstIterator = ReverseContainerIterator<ConstIterator>;

		GD_CONTAINER_DEFINE_ITERATION_SUPPORT(Vector)

	private:
		auto static constexpr s_BitPerWord = sizeof(Word) * 8;
		GDINL static Word ToWord(SizeTp const bitsCapacity)
		{
			return bitsCapacity / s_BitPerWord + (bitsCapacity % s_BitPerWord != 0);
		}

	private:
		UInt64* m_Memory;
		SizeTp  m_Length;
		SizeTp  m_Capacity;

	public:

		// ------------------------------------------------------------------------------------------
		// Constructors and destructor.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Initializes vector with specified number of allocated and initialized bits.
		 *
		 * @param initialLength Number of bits been initialized.
		 * @param initialCapacity Initial capacity of vector. 
		 */
		//! @{
		GDINL explicit Vector()
			: m_Memory(nullptr), m_Length(0), m_Capacity(0)
		{}
		GDINL explicit Vector(SizeTp const initialLength)
			: m_Memory(nullptr), m_Length(0), m_Capacity(0)
		{
			Resize(initialLength);
		}
		GDINL explicit Vector(SizeTp const initialLength, SizeTp const initialCapacity)
			: m_Memory(nullptr), m_Length(0), m_Capacity(0)
		{
			Reserve(initialCapacity);
			Resize(initialLength);
		}
		//! @}

		/*!
		 * Initializes vector with copy of values of specified iterators. 
		 *
		 * @param startIterator lhs Iterator would be copied.
		 * @param endIterator Last Iterator would be copied.
		 */
		template<typename TForwardIterator, typename = typename EnableIf<IteratorTraits<TForwardIterator>::IsForward>::Type>
		GDINL Vector(TForwardIterator const startIterator, TForwardIterator const endIterator)
			: m_Memory(nullptr), m_Length(0), m_Capacity(0)
		{
			ReserveToLength(static_cast<SizeTp>(endIterator - startIterator));
			for (auto iterator = startIterator; iterator != endIterator; ++iterator)
			{
				this->InsertLast(*iterator != 0);
			}
		}

		/*!
		 * Initializes vector with default C++11's initializer list. You should not use this constructor manually.
		 * @param initializerList Initializer list passed by the compiler.
		 */
		GDINL Vector(InitializerList<bool> const& initializerList)
			: Vector(initializerList.begin(), initializerList.end())
		{}
		
		/*!
		 * Initializes vector with copy of other vector.
		 * @param otherVector Vector would be copied.
		 */
		//! @{
		GDINL Vector(Vector const& otherVector)
			: m_Length(otherVector.m_Length), m_Capacity(otherVector.m_Capacity)
		{
			auto const wordCapacity = ToWord(m_Capacity);
			m_Memory = CMemory::TMemcpy(GD_MALLOC_ARRAY_T(Word, wordCapacity), otherVector.m_Memory, wordCapacity);
		}
		template<typename TOtherAllocator>
		GDINL implicit Vector(Vector<TOtherAllocator> const& otherVector)
			: m_Length(otherVector.m_Length), m_Capacity(otherVector.m_Capacity)
		{
			auto const wordCapacity = ToWord(m_Capacity);
			m_Memory = CMemory::TMemcpy(GD_MALLOC_ARRAY_T(Word, wordCapacity), otherVector.m_Memory, wordCapacity);
		}
		//! @}

		/*!
		 * Moves other vector here.
		 * @param otherVector Vector would be moved into current object.
		 */
		GDINL Vector(Vector&& otherVector) noexcept
			: m_Memory(otherVector.m_Memory), m_Length(otherVector.m_Length), m_Capacity(otherVector.m_Capacity)
		{
			otherVector.m_Memory = nullptr;
			otherVector.m_Length = 0;
			otherVector.m_Capacity = 0;
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
		GDINL Word const* GetData() const
		{
			return m_Memory;
		}
		GDINL Word* GetData()
		{
			return m_Memory;
		}
		//! @}

		/*!
		 * Returns number of elements that exist in vector.
		 */
		GDINL SizeTp GetLength() const
		{
			return m_Length;
		}

		/*!
		 * Returns number of elements that can be placed into vector without reallocation.
		 */
		GDINL SizeTp GetCapacity() const
		{
			return m_Capacity;
		}

		/*!
		 * Returns true if this vector is empty.
		 */
		GDINL bool IsEmpty() const
		{
			return m_Length == 0;
		}

		/*!
		 * Resizes vector to make it able to contain specified number of bits.
		 * @param newLength New required length of the vector.
		 */
		GDINL void Resize(SizeTp const newLength)
		{
			if (m_Length != newLength)
			{	
				if (m_Length > newLength)
				{
					// Cleaning up all bits from the word, that would become last active.
					auto const lastWordIndex = newLength / s_BitPerWord;
					m_Memory[lastWordIndex] &= ~(~Word(0) << newLength % s_BitPerWord);

					auto const totalWordsToClean = ToWord(m_Length - newLength);
					if (totalWordsToClean > 1)
					{	// Cleaning up all the rest words that succeed the last word.
						CMemory::TMemset(m_Memory + lastWordIndex + 1, Byte(0), totalWordsToClean - 1);
					}
				}
				else
				{	
					ReserveToLength(newLength);
				}
				m_Length = newLength;
			}
		}

		/*!
		 * Reserves memory for vector to make it contain specified number of bits without 
		 * reallocation when calling Resize/Insert/InsertLast method.
		 *
		 * @param newCapacity New required capacity of the vector.
		 */
		GDINL void Reserve(SizeTp const newCapacity)
		{
			if (m_Capacity != newCapacity)
			{
				auto const wordCapacity = ToWord(m_Capacity);
				auto const newWordCapacity = ToWord(newCapacity);
				if (newWordCapacity != wordCapacity)
				{
					auto const newMemory = GD_MALLOC_ARRAY_T(Word, newWordCapacity);
					if (newWordCapacity > wordCapacity)
					{	// Pre-cleaning up all new memory and copying bits from old one.
						CMemory::TMemcpy(newMemory, m_Memory, wordCapacity);
						CMemory::TMemset(newMemory + wordCapacity, Byte(0), newWordCapacity - wordCapacity);
					}
					else
					{	// Just copying bits from old one.
						CMemory::TMemcpy(newMemory, m_Memory, newWordCapacity);
					}
					GD_FREE(m_Memory);
					m_Memory = newMemory;
				}
				m_Capacity = newCapacity;
			}
		}

		/*!
		 * Reserves memory for vector to make it contain best fitting number of bits 
		 * to predicted new size. This function incrementally grows capacity in 1.3 times.
		 *
		 * @param newLength New desired length.
		 */
		GDINL void ReserveToLength(SizeTp const newLength)
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

		// ------------------------------------------------------------------------------------------
		// Dynamic elements access.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Returns reference on last bit in the vector.
		 */
		//! @{
		GDINL ConstReferenceType GetLast() const
		{
			return (*this)[m_Length - 1];
		}
		GDINL ReferenceType GetLast()
		{
			return (*this)[m_Length - 1];
		}
		//! @}

		/*!
		 * Inserts specified bit into collection at desired index.
		 *
		 * @param index At this index new element would be inserted. All existing elements would be shifted.
		 * @param element New element that would be inserted.
		 *
		 * @returns Index at which new element was inserted, or the value of @ref index parameter.
		 */
		GDINL SizeTp InsertAt(SizeTp const index, bool const element = bool())
		{
			GD_DEBUG_VERIFY(index <= m_Length, "Index is out of bounds");
			ReserveToLength(m_Length + 1);
			m_Length += 1;
			GD_STUBBED(Suboptimal bitwise shift);
			{
				for (auto iterator = End() - 1; iterator != Begin() + index - 1; --iterator)
				{
					*iterator = *(iterator - 1);
				}
			}
			(*this)[index] = element;
			return index;
		}

		/*!
		 * Appends new bit to the vector.
		 *
		 * @param newElement New bit that would be inserted into the end of container.
		 * @returns Index at which new bit was inserted.
		 */
		GDINL SizeTp InsertLast(bool newElement = bool())
		{
			Resize(m_Length + 1);
			GetLast() = newElement;
			return m_Length - 1;
		}

		/*!
		 * Removes existing element from array at specified index.
		 *
		 * @param index Element at this index would be removed. All other elements would be shifted to left.
		 * @param amount An amount of elements to remove.
		 */
		GDINL void EraseAt(SizeTp const index, SizeTp const amount = 1)
		{
			GD_DEBUG_VERIFY(index + amount <= m_Length, "Index is out of bounds");
			GD_STUBBED(Suboptimal bitwise shift);
			{
				for (auto iterator = Begin() + index; iterator != (End() - amount); ++iterator)
				{
					*iterator = *(iterator + amount);
				}
				Resize(m_Length - amount);
			}
		}

		/*!
		 * Removes last bit from vector.
		 */
		GDINL void EraseLast()
		{
			GD_DEBUG_VERIFY(m_Length != 0, "vector size is zero");
			Resize(m_Length - 1); 
		}

	public:

		// ------------------------------------------------------------------------------------------
		// Overloaded operators.
		// ------------------------------------------------------------------------------------------

		// vector = something
		GDINL Vector& operator= (Vector&& otherVector) noexcept
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
			if (&otherVector != this)
			{
				if (m_Capacity >= otherVector.m_Length)
				{
					this->Resize(otherVector.m_Length);
					CMemory::TMemcpy(m_Memory, otherVector.m_Memory, ToWord(otherVector.m_Length));
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
			if (&otherVector != this)
			{
				if (m_Capacity >= otherVector.m_Length)
				{
					this->Resize(otherVector.m_Length);
					CMemory::TMemcpy(m_Memory, otherVector.m_Memory, ToWord(otherVector.m_Length));
				}
				else
				{
					this->~Vector();
					new(this) Vector(otherVector);
				}
			}
			return *this;
		}
		GDINL Vector& operator= (InitializerList<bool> const& initializerList)
		{
			if (m_Capacity >= initializerList.size())
			{
				this->Resize(initializerList.size());
				Algo::CopyRange(initializerList.begin(), initializerList.end(), Begin());
			}
			else
			{
				this->~Vector();
				new(this) Vector(initializerList);
			}
			return *this;
		}

		// vector[]
		GDINL ConstReferenceType operator[] (SizeTp const index) const
		{
			GD_DEBUG_VERIFY(index < m_Length, "Index is out of bounds");
			return{ m_Memory[index / s_BitPerWord], index % s_BitPerWord };
		}
		GDINL ReferenceType operator[] (SizeTp const index)
		{
			GD_DEBUG_VERIFY(index < m_Length, "Index is out of bounds");
			return{ m_Memory[index / s_BitPerWord], index % s_BitPerWord };
		}

		// vector == vector
		template<typename TOtherAllocator>
		GDINL friend bool operator== (Vector const& lhs, Vector<bool, TOtherAllocator> const& rhs)
		{
			return lhs.m_Length == rhs.m_Length && CMemory::TMemcmp(lhs.m_Memory, rhs.m_Memory, ToWord(lhs.m_Length)) == 0;
		}
		template<typename TOtherAllocator>
		GDINL friend bool operator!= (Vector const& lhs, Vector<bool, TOtherAllocator> const& rhs)
		{
			return lhs.m_Length != rhs.m_Length || CMemory::TMemcmp(lhs.m_Memory, rhs.m_Memory, ToWord(lhs.m_Length)) != 0;
		}

		// vector > vector
		template<typename TOtherAllocator>
		GDINL friend bool operator< (Vector const& lhs, Vector<bool, TOtherAllocator> const& rhs)
		{
			return CMemory::TMemcmp(lhs.m_Memory, rhs.m_Memory, ToWord(Min(lhs.m_Length, rhs.m_Length))) < 0;
		}
		template<typename TOtherAllocator>
		GDINL friend bool operator> (Vector const& lhs, Vector<bool, TOtherAllocator> const& rhs)
		{
			return rhs < lhs;
		}

		// vector >= vector
		template<typename TOtherAllocator>
		GDINL friend bool operator<= (Vector const& lhs, Vector<bool, TOtherAllocator> const& rhs)
		{
			return !(lhs > rhs);
		}
		template<typename TOtherAllocator>
		GDINL friend bool operator>= (Vector const& lhs, Vector<bool, TOtherAllocator> const& rhs)
		{
			return !(lhs < rhs);
		}

		// vector += vector
		template<typename TOtherAllocator>
		GDINL friend Vector& operator+= (Vector& lhs, Vector<bool, TOtherAllocator>&& rhs)
		{
			lhs.ReserveToLength(lhs.m_Length + rhs.m_Length);
			for (auto iterator = rhs.Begin(); iterator != rhs.End(); ++iterator)
			{
				lhs.InsertLast(Move(*iterator));
			}
			rhs.Clear();
			return lhs;
		}
		template<typename TOtherAllocator>
		GDINL friend Vector& operator+= (Vector& lhs, Vector<bool, TOtherAllocator> const& rhs)
		{
			lhs.ReserveToLength(lhs.m_Length + rhs.m_Length);
			for (auto iterator = rhs.Begin(); iterator != rhs.End(); ++iterator)
			{
				lhs.InsertLast(*iterator);
			}
			return lhs;
		}

		// vector += bool
		GDINL friend Vector& operator+= (Vector& lhs, bool const rhs)
		{
			lhs.InsertLast(rhs);
			return lhs;
		}

		// vector + vector
		template<typename TOtherAllocator>
		GDINL friend Vector operator+ (Vector const& lhs, Vector<bool, TOtherAllocator>&& rhs)
		{
			return Vector(lhs) += Utils::Forward<Vector<bool, TOtherAllocator>>(rhs);
		}
		template<typename TOtherAllocator>
		GDINL friend Vector operator+ (Vector const& lhs, Vector<bool, TOtherAllocator> const& rhs)
		{
			return Vector(lhs) + rhs;
		}

	};	// class Vector<bool>

	/*!
	 * Temporary vector implementation.
	 */
	template<typename TElement, SizeTp TCapacity = InlineAllocatorCapacityNormal>
	using TempVector = Vector<TElement, InlineDoubleEndedStackAllocator<InlineAllocatorCapacityNormal>>;
	template<typename TElement>
	using TempTinyVector = TempVector<TElement, InlineAllocatorCapacityTiny>;
	template<typename TElement>
	using TempHugeVector = TempVector<TElement, InlineAllocatorCapacityHuge>;

	/*!
	 * Dynamic stack implementation.
	 */
	template<typename TElement>
	using Stack = Vector<TElement>;
	template<typename TElement, SizeTp TCapacity = InlineAllocatorCapacityNormal>
	using TempStack = Vector<TElement, InlineStackAllocator<InlineAllocatorCapacityNormal>>;
	template<typename TElement>
	using TempTinyStack = TempStack<TElement, InlineAllocatorCapacityTiny>;
	template<typename TElement>
	using TempHugeStack = TempStack<TElement, InlineAllocatorCapacityHuge>;

GD_NAMESPACE_END
