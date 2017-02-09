// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Containers/Set.h
 * Dynamically sized associative set class.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/Vector.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                               VectorSet<T> class.                                ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	// **------------------------------------------------------------------------------------------**
	//! Dynamically sized associative set that is implemented with dynamic arrays.
	//! Drop-in replacement for the set class.
	//!
	//! @tparam TElement Container element type.
	// **------------------------------------------------------------------------------------------**
	template<typename TElement, typename TAllocator = DefaultContainerAllocator>
	class UnorderedVectorSet : public Vector<TElement, TAllocator>
		, public IIteratable<UnorderedVectorSet<TElement, TAllocator>>, public TNonCopyable
	{
	public:
		using ElementType          = TElement;
		using VectorType           = Vector<TElement, TAllocator>;
		using Iterator             = typename VectorType::Iterator;
		using ConstIterator        = typename VectorType::ConstIterator;
		using ReverseIterator      = typename VectorType::ReverseIterator;
		using ReverseConstIterator = typename VectorType::ReverseConstIterator;

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
		// Set manipulation.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Queries for the Iterator of the element with specified Key.
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
		 * Queries for the Iterator of the element with specified key.
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
			GD_DEBUG_VERIFY(!this->Contains(element), "Specified element already exists.");
			this->VectorType::InsertLast(Utils::Forward<TElement>(element));
			return this->GetLast();
		}
		GDINL TElement& Insert(TElement const& element)
		{
			GD_DEBUG_VERIFY(!this->Contains(element), "Specified element already exists.");
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
			GD_DEBUG_VERIFY(foundIndex != SizeTpMax, "Specified element does not exist.");
			this->EraseAt(foundIndex);
		}

		// ------------------------------------------------------------------------------------------
		// Overloaded operators.
		// ------------------------------------------------------------------------------------------

		GDINL UnorderedVectorSet& operator= (UnorderedVectorSet&& otherSet) = default;

	};	// class VectorSet 

	template<typename TElement>
	using GCVectorSet = UnorderedVectorSet<TElement, GCContainerAllocator<TElement>>;

GD_NAMESPACE_END
