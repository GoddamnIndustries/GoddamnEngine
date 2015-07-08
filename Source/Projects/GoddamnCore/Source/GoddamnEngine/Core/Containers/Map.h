// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Containers/Map.h
//! Dynamically sized associative container interface.
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/RedBlackTree.h>
#include <GoddamnEngine/Core/Containers/InitializerList.h>

GD_NAMESPACE_BEGIN
	
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// class Map<K, V>
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Dynamically sized associative container that is implemented with Red-Black Trees.
	//! 	   Current implementation has a MASSIVE overhead per each element (33 bytes by Red-Black Tree). 
	//! 	   Red-Black Tree is used for elements access and searching, Lists for elements rapid iteration.
	//! @tparam KeyTypeTp Key type, used for searching.
	//! @tparam ValueTypeTp Type of elements stored in the container.
	template<typename KeyTypeTp, typename ValueTypeTp>
	class Map final : public RedBlackTree<Pair<KeyTypeTp const, ValueTypeTp>>
	{
	public:
		typedef KeyTypeTp											KeyType;
		typedef ValueTypeTp											ValueType;
		typedef Pair<KeyType const, ValueType>						PairType;
		typedef PairType											ElementType;
		typedef RedBlackTree<ElementType>							RedBlackTreeType;
		typedef typename RedBlackTreeType::RedBlackTreeNodeType		RedBlackTreeNodeType;
		typedef typename RedBlackTreeType::Iterator					Iterator;
		typedef typename RedBlackTreeType::ConstIterator			ConstIterator;
		typedef typename RedBlackTreeType::ReverseIterator			ReverseIterator;
		typedef typename RedBlackTreeType::ReverseConstIterator		ReverseConstIterator;

	private:
		GD_CONTAINER_DEFINE_ITERATION_SUPPORT(Map);

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructor and destructor.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Initializes an empty map.
		GDINL Map()
		{
		}

		// ------------------------------------------------------------------------------------------
		//! Initializes map with default C++11's initializer list. You should not use this constructor manually.
		//! @param InitializerList Initializer list passed by the compiler.
		GDINL Map(InitializerList<PairType> const& InitializerList)
		{
			for (auto const& Element : InitializerList)
				this->InsertKeyValue(Element.Key, Element.Value);
		}

		// ------------------------------------------------------------------------------------------
		//! Moves other map here.
		//! @param OtherMap Map would be moved into current object.
		GDINL Map(Map&& OtherMap) : RedBlackTree(Forward<Map>(OtherMap))
		{
		}

		// ------------------------------------------------------------------------------------------
		//! Deinitializes a map.
		GDINL ~Map()
		{
			this->Clear();
		}

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Map manipulation.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Queries for the iterator of the element with specified key.
		//! @param Element Element we are looking for.
		//! @returns Iterator on the element if it was found and End iterator otherwise.
		//! @{
		GDINL ConstIterator QueryIteratorWithKey(KeyType const& Key) const { return ConstIterator(*this, this->QueryNode(&Key)); }
		GDINL Iterator QueryIteratorWithKey(KeyType const& Key)
		{
			return Iterator(*this, this->QueryNode(&Key));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Determines whether the element with specified key exists in the container.
		//! @param Key Key of the element we are looking for.
		//! @returns True if element with specified key exists in the container, false otherwise.
		GDINL bool ContainsElementWithKey(KeyType const& Key) const
		{
			return this->QueryNode(&Key) != nullptr;
		}

		// ------------------------------------------------------------------------------------------
		//! Inserts specified element into collection at desired index.
		//! @param Key Key of the element that is going to be inserted.
		//! @param Value Value of the element that is going to be inserted.
		//! @{
		GDINL void InsertKeyValue(KeyType&& Key, ValueType&& Value = ValueType())
		{
			RedBlackTreeNodeType* const NewNode = this->CreateNode(Forward<KeyType>(Key), Forward<ValueType>(Value));
			this->InsertNode(NewNode);
		}
		GDINL void InsertKeyValue(KeyType const& Key, ValueType const& Value)
		{
			RedBlackTreeNodeType* const NewNode = this->CreateNode(Key, Value);
			this->InsertNode(NewNode);
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Removes existing element from array at specified index.
		//! @param Key Key of the element that is going to be RemoveFromSelfd.
		GDINL void RemoveElementWithKey(KeyType const& Key)
		{
			this->RemovePtr(&Key);
		}

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Overloaded operators.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Moves other map here.
		//! @param OtherMap Other map that would be moved into current object.
		//! @returns This.
		GDINL Map& operator= (Map&& OtherMap)
		{
			RedBlackTreeType::operator= (Forward<Map>(OtherMap));
			return *this;
		}

		// ------------------------------------------------------------------------------------------
		//! Returns reference on value of the element with specified key.
		//! @param Key Key of the element we are looking for.
		//! @returns Reference on some element in the container.
		//! @{
		GDINL ValueType& operator[] (KeyType const& Key) { return const_cast<ValueType&>(const_cast<Map const&>(*this)[Key]); }
		GDINL ValueType const& operator[] (KeyType const& Key) const
		{
			ConstIterator QueriedIterator = this->QueryIteratorWithKey(Key);
			GD_ASSERT(QueriedIterator != this->End(), "Element with specified key does not exist.");
			return QueriedIterator->Value;
		}
		//! @}
	};	// class Map

GD_NAMESPACE_END
