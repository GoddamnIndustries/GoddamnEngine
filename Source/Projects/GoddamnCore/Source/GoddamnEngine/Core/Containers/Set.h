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
#include <GoddamnEngine/Core/Templates/Iterators.h>
#include <GoddamnEngine/Core/Containers/InitializerList.h>
#include <GoddamnEngine/Core/Containers/RedBlackTree/RedBlackTree.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                                   Set<T> class.                                  ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	// **------------------------------------------------------------------------------------------**
	//! Dynamically sized associative set that is implemented with Red-Black Trees.
	//! Red-Black Tree is used for elements access and searching.
	//!
	//! @tparam TElement Container element type.
	// **------------------------------------------------------------------------------------------**
	template<typename TElement, typename TAllocator = DefaultContainerAllocator>
	class Set : public RedBlackTree<TElement>, public IIteratable<Set<TElement, TAllocator>>
	{
	public:
		using ElementType          = TElement;
		using RedBlackTreeType     = RedBlackTree<TElement>;
		using RedBlackTreeNodeType = typename RedBlackTreeType::RedBlackTreeNodeType;
		using Iterator             = typename RedBlackTreeType::Iterator;
		using ConstIterator        = typename RedBlackTreeType::ConstIterator;
		using ReverseIterator      = typename RedBlackTreeType::ReverseIterator;
		using ReverseConstIterator = typename RedBlackTreeType::ReverseConstIterator;

	public:

		// ------------------------------------------------------------------------------------------
		// Constructor and destructor.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Initializes an empty set.
		 */
		GDINL Set() = default;

		/*!
		* Moves other set here.
		* @param otherSet Set would be moved into current object.
		*/
		GDINL Set(Set&& otherSet) = default;

		/*!
		 * Initializes set with default C++11's initializer list. You should not use this constructor manually.
		 * @param initializerList Initializer list passed by the compiler.
		 */
		GDINL Set(InitializerList<TElement> const& initializerList)
		{
			for (auto const& element : initializerList)
			{
				this->Insert(element);
			}
		}

		GDINL ~Set()
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
			return ConstIterator(*this, this->FindNodeBase(&element));
		}
		GDINL Iterator FindIterator(TElement const& element)
		{
			return Iterator(*this, this->FindNodeBase(&element));
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
			return const_cast<TElement*>(const_cast<Set const*>(this)->Find(element));
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
			
			auto const newNode = this->InternalCreateNode(Utils::Forward<TElement>(element));
			this->InsertNodeBase(newNode);
			return *newNode->GetData();
		}
		GDINL TElement& Insert(TElement const& element)
		{
			GD_DEBUG_VERIFY(!this->Contains(element), "Specified element already exists.");
			
			auto const newNode = this->InternalCreateNode(element);
			this->InsertNodeBase(newNode);
			return *newNode->GetData();
		}
		//! @}

		/*!
		 * Removes existing element from the set.
		 * @param element The element that is going to be removed.
		 */
		GDINL void Erase(TElement const& element)
		{
			auto const foundNode = this->FindNodeBase(&element);
			GD_DEBUG_VERIFY(foundNode != this->GetNullNodeBase(), "Specified element does not exist.");
			this->RemoveNodeBase(foundNode);
		}

		// ------------------------------------------------------------------------------------------
		// Overloaded operators.
		// ------------------------------------------------------------------------------------------

		GDINL Set& operator= (Set&& otherSet) = default;

	};	// class Set

GD_NAMESPACE_END
