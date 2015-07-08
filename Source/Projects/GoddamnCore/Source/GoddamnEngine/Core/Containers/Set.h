// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Containers/Set.h
//! Dynamically sized associative container interface.
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Iterators.h>
#include <GoddamnEngine/Core/Containers/RedBlackTree.h>
#include <GoddamnEngine/Core/Containers/InitializerList.h>

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// class Set<T>
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Dynamically sized associative container that is implemented with Red-Black Trees.
	//! Current implementation has a MASSIVE overhead per each element (33 bytes by Red-Black Tree). 
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
		GDINL Set() : RedBlackTreeType()
		{
		}

		// ------------------------------------------------------------------------------------------
		//! Initializes set with default C++11's initializer list. You should not use this constructor manually.
		//! @param InitializerList Initializer list passed by the compiler.
		GDINL Set(InitializerList<ElementType> const& InitializerList) : RedBlackTreeType()
		{
			for (auto const& Element : InitializerList)
				this->InsertKeyValue(Element.Key, Element.Value);
		}

		// ------------------------------------------------------------------------------------------
		//! Moves other set here.
		//! @param OtherSet Set would be moved into current object.
		GDINL Set(Set&& OtherSet) : RedBlackTreeType(Forward<Set>(OtherSet))
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
		//! Determines whether the element with specified key exists in the container.
		//! @param Element Element we are looking for.
		//! @returns True if element with specified key exists in the container, false otherwise.
		GDINL bool Contains(ElementType const& Element) const
		{
			return this->QueryIterator(Element) != this->End();
		}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Overloaded operators.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Moves other set here.
		//! @param OtherSet Other set that would be moved into current object.
		//! @returns This.
		GDINL Set& operator= (Set&& OtherSet)
		{
			RedBlackTreeType::operator= (Forward<Set>(OtherSet));
			return *this;
		}
	};	// class Set

GD_NAMESPACE_END
