// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Containers/Set.h
//! Dynamically sized associative m_Container interface.
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Iterators.h>
#include <GoddamnEngine/Core/Containers/InitializerList.h>
#include <GoddamnEngine/Core/Containers/RedBlackTree/RedBlackTree.h>

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// class Set<T>
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Dynamically sized associative container that is Implemented with Red-Black Trees.
	//! Current Implementation has a MASSIVE overhead per each element (33 bytes by Red-Black Tree). 
	//! Red-Black Tree is used for elements access and searching, Lists for elements rapid iteration.
	template<typename ElementTypeTp>
	class Set final : public RedBlackTree<ElementTypeTp>
	{
	public:
		using ElementType          = ElementTypeTp;
		using RedBlackTreeType     = RedBlackTree<ElementType>;
		using RedBlackTreeNodeType = typename RedBlackTreeType::RedBlackTreeNodeType;
		using Iterator             = typename RedBlackTreeType::Iterator;
		using ConstIterator        = typename RedBlackTreeType::ConstIterator;
		using ReverseIterator      = typename RedBlackTreeType::ReverseIterator;
		using ReverseConstIterator = typename RedBlackTreeType::ReverseConstIterator;

	private:
		GD_CONTAINER_DEFINE_ITERATION_SUPPORT(Set);

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructor and destructor.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Initializes an empty map.
		GDINL Set() 
			: RedBlackTreeType()
		{
		}

		// ------------------------------------------------------------------------------------------
		//! Initializes set with default C++11's initializer list. You should not use this constructor manually.
		//! @param InitializerList Initializer list passed by the compiler.
		GDINL Set(InitializerList<ElementType> const& initializerList) 
			: RedBlackTreeType()
		{
			for (auto const& element : initializerList)
				Insert(element);
		}

		// ------------------------------------------------------------------------------------------
		//! Moves other set here.
		//! @param OtherSet Set would be moved into current object.
		GDINL Set(Set&& otherSet) 
			: RedBlackTreeType(Forward<Set>(otherSet))
		{
		}

		// ------------------------------------------------------------------------------------------
		//! Deinitializes a set.
		GDINL ~Set()
		{
			this->Clear();
		}

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Set manipulation.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Queries for the Iterator of the element with specified Key.
		//! @param element The element we are looking for.
		//! @returns Iterator on the element if it was found and End Iterator otherwise.
		//! @{
		GDINL ConstIterator QueryIterator(ElementType const& element) const
		{
			return ConstIterator(*this, this->_QueryNode(&element));
		}
		GDINL Iterator QueryIterator(ElementType const& element)
		{
			return Iterator(*this, this->_QueryNode(&element));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Determines whether the element with specified Key exists in the m_Container.
		//! @param element The element we are looking for.
		//! @returns True if element with specified Key exists in the m_Container, false otherwise.
		GDINL bool Contains(ElementType const& element) const
		{
			return QueryIterator(element) != this->End();
		}

		// ------------------------------------------------------------------------------------------
		//! Performs an insertion of the new node with specified Key and value.
		//! @param element The element that would be inserted.
		//! @{
		GDINL void Insert(ElementType&& element)
		{
			this->_InsertNode(GD_NEW(RedBlackTreeNodeType, Forward<ElementType>(element)));
		}
		GDINL void Insert(ElementType const& element)
		{
			this->_InsertNode(GD_NEW(RedBlackTreeNodeType, element));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Removes existing element from the tree.
		//! @param element The element that is going to be RemoveFromSelfd.
		//! @{
		GDINL void Erase(ElementType const& element)
		{
			EraseByPtr(&element);
		}
		GDINL void EraseByPtr(ElementType const* const element)
		{
			auto QueriedNode = this->_QueryNode(element);
			GD_DEBUG_ASSERT(QueriedNode != nullptr, "Specified element Key does not exist.");
			this->_RemoveNode(QueriedNode);
		}
		//! @}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Overloaded operators.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		GDINL Set& operator= (Set&& OtherSet)
		{
			RedBlackTreeType::operator= (Forward<Set>(OtherSet));
			return *this;
		}
	};	// class Set

GD_NAMESPACE_END
