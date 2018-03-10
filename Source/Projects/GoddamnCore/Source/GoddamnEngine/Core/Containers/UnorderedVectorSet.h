// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Containers/UnorderedVectorSet.h
 * Dynamically sized vector-based set class.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/Vector.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                               VectorSet<T> class.                                ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	// **------------------------------------------------------------------------------------------**
	//! Dynamically sized set that is implemented with dynamic arrays.
	//! Drop-in replacement for the set class, use in case of small sets.
	//!
	//! @tparam TElement Container element type.
	//! @tparam TAllocator Allocator used by this set.
	// **------------------------------------------------------------------------------------------**
	template<typename TElement, typename TAllocator = DefaultContainerAllocator>
	class UnorderedVectorSet : private Vector<TElement, TAllocator>, public TNonCopyable  // NOLINT
	{
	private:
		using VectorType           = Vector<TElement, TAllocator>;
	public:
		using AllocatorType        = TAllocator;
		using ElementType          = TElement;
		using Iterator             = typename VectorType::Iterator;
		using ConstIterator        = typename VectorType::ConstIterator;
		using ReverseIterator      = typename VectorType::ReverseIterator;
		using ReverseConstIterator = typename VectorType::ReverseConstIterator;

		GD_CONTAINER_DEFINE_ITERATION_SUPPORT(UnorderedVectorSet)

	public:

		// ------------------------------------------------------------------------------------------
		// Constructor and destructor.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Initializes an empty set.
		 */
		GDINL UnorderedVectorSet() = default;

		/*!
		 * Moves other set here.
		 * @param otherSet Set would be moved into current object.
		 */
		GDINL UnorderedVectorSet(UnorderedVectorSet&& otherSet) noexcept = default;

		/*!
		 * Initializes set with default C++11's initializer list. You should not use this constructor manually.
		 * @param initializerList Initializer list passed by the compiler.
		 */
		GDINL UnorderedVectorSet(InitializerList<TElement> const& initializerList)
		{
			for (auto const& element : initializerList)
			{
				this->Insert(element);
			}
		}

		GDINL ~UnorderedVectorSet()
		{
			this->Clear();
		}

	public:

		// ------------------------------------------------------------------------------------------
		// Iteration API.
		// ------------------------------------------------------------------------------------------

		GDINL Iterator Begin()
		{
			return this->VectorType::Begin();
		}
		GDINL ConstIterator Begin() const
		{
			return this->VectorType::Begin();
		}

		GDINL Iterator End()
		{
			return this->VectorType::End();
		}
		GDINL ConstIterator End() const
		{
			return this->VectorType::End();
		}

		GDINL ReverseIterator ReverseBegin()
		{
			return this->VectorType::ReverseBegin();
		}
		GDINL ReverseConstIterator ReverseBegin() const
		{
			return this->VectorType::ReverseBegin();
		}

		GDINL ReverseIterator ReverseEnd()
		{
			return this->VectorType::ReverseEnd();
		}
		GDINL ReverseConstIterator ReverseEnd() const
		{
			return this->VectorType::ReverseEnd();
		}

	public:

		// ------------------------------------------------------------------------------------------
		// Set manipulation.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Returns number of the elements in the set.
		 */
		GDINL SizeTp GetLength() const
		{
			return this->VectorType::GetLength();
		}

		/*!
		 * Returns true if this set is empty.
		 */
		GDINL bool IsEmpty() const
		{
			return this->VectorType::IsEmpty();
		}

		/*!
		 * Removes all elements from the set.
		 */
		GDAPI void Clear()
		{
			this->VectorType::Clear();
		}

		/*!
		 * Queries for the iterator of the element with specified Key.
		 *
		 * @param element The element we are looking for.
		 * @returns Iterator on the element if it was found and End Iterator otherwise.
		 */
		//! @{
		GDINL ConstIterator FindIterator(TElement const& element) const
		{
			return Algo::FindFirst(this->Begin(), this->End(), element);
		}
		GDINL Iterator FindIterator(TElement const& element)
		{
			return Algo::FindFirst(this->Begin(), this->End(), element);
		}
		//! @}

		/*!
		 * Queries for the iterator of the element with specified key.
		 *
		 * @param element The element we are looking for.
		 * @returns Pointer to the element if it was found and null pointer otherwise.
		 */
		//! @{
		GDINL TElement const* Find(TElement const& element) const
		{
			auto const iterator = this->FindIterator(element);
			return iterator != this->End() ? &*iterator : nullptr;
		}
		GDINL TElement* Find(TElement const& element)
		{
			return const_cast<TElement*>(const_cast<UnorderedVectorSet const*>(this)->Find(element));
		}
		//! @}

		/*!
		 * Determines whether the specified element exists in the set.
		 * @param element The element we are looking for.
		 */
		GDINL bool Contains(TElement const& element) const
		{
			return this->FindIterator(element) != this->End();
		}

		/*!
		 * Adds a new specified element to the set.
		 * @param element The element that would be inserted.
		 */
		//! @{
		GDINL TElement& Insert(TElement&& element)
		{
			GD_ASSERT(!this->Contains(element), "Specified element already exists.");

			this->VectorType::InsertLast(Utils::Forward<TElement>(element));
			return this->GetLast();
		}
		GDINL TElement& Insert(TElement const& element)
		{
			GD_ASSERT(!this->Contains(element), "Specified element already exists.");

			this->VectorType::InsertLast(element);
			return this->GetLast();
		}
		//! @}

		/*!
		 * Removes existing element from the set.
		 * @param element The element that is going to be removed.
		 */
		GDINL void Erase(TElement const& element)
		{
			auto const foundIndex = this->FindFirst(element);
			GD_ASSERT(foundIndex != SizeTpMax, "Specified element does not exist.");
			this->EraseAt(foundIndex);
		}

		// ------------------------------------------------------------------------------------------
		// Overloaded operators.
		// ------------------------------------------------------------------------------------------

		GDINL UnorderedVectorSet& operator= (UnorderedVectorSet&& otherSet) = default;

	};	// class UnorderedVectorSet 

GD_NAMESPACE_END
