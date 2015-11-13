// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Containers/Map.h
//! Dynamically sized associative m_Container interface.
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/InitializerList.h>
#include <GoddamnEngine/Core/Containers/RedBlackTree/RedBlackTree.h>

GD_NAMESPACE_BEGIN
	
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// class Map<K, V>
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Dynamically sized associative container that is Implemented with Red-Black Trees.
	//! Current Implementation has a MASSIVE overhead per each element (33 bytes by Red-Black Tree). 
	//! Red-Black Tree is used for elements access and searching, Lists for elements rapid iteration.
	//! @tparam TKey key type, used for searching.
	//! @tparam TValue Type of elements stored in the m_Container.
	template<typename TKey, typename TValue>
	class Map final : public RedBlackTree<Pair<TKey, TValue>>
	{
	public:
		using KeyType              = TKey;
		using ValueType            = TValue;
		using PairType             = Pair<KeyType, ValueType>;
		using ElementType          = PairType;
		using RedBlackTreeType     = RedBlackTree<ElementType>;
		using RedBlackTreeNodeType = typename RedBlackTreeType::RedBlackTreeNodeType;
		using Iterator             = typename RedBlackTreeType::Iterator;
		using ConstIterator        = typename RedBlackTreeType::ConstIterator;
		using ReverseIterator      = typename RedBlackTreeType::ReverseIterator;
		using ReverseConstIterator = typename RedBlackTreeType::ReverseConstIterator;

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
			for (auto const& element : InitializerList)
				Insert(element.Key, element.Value);
		}

		// ------------------------------------------------------------------------------------------
		//! Moves other map here.
		//! @param OtherMap Map would be moved into current object.
		GDINL Map(Map&& OtherMap) 
			: RedBlackTree(Forward<Map>(OtherMap))
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
		//! Queries for the Iterator of the element with specified key.
		//! @param element The element we are looking for.
		//! @returns Iterator on the element if it was found and End Iterator otherwise.
		//! @{
		GDINL ConstIterator QueryIterator(KeyType const& key) const
		{
			return ConstIterator(*this, this->_QueryNode(&key));
		}
		GDINL Iterator QueryIterator(KeyType const& key)
		{
			return Iterator(*this, this->_QueryNode(&key));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Queries for the Iterator of the element with specified key.
		//! @param key The element we are looking for.
		//! @returns Iterator on the element if it was found and End Iterator otherwise.
		//! @{
		GDINL ValueType const* Query(KeyType const& key) const
		{
			auto const node = static_cast<RedBlackTreeNodeType*>(_QueryNode(&key));
			return (node != this->_GetNullNode()) ? &node->GetData()->value : nullptr;
		}
		GDINL ValueType* Query(KeyType const& key)
		{
			return const_cast<ValueType*>(const_cast<Map const*>(this)->Query(key));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Determines whether the element with specified key exists in the m_Container.
		//! @param key The key of the element we are looking for.
		//! @returns True if element with specified key exists in the m_Container, false otherwise.
		GDINL bool Contains(KeyType const& key) const
		{
			return this->_QueryNode(&key) != this->_GetNullNode();
		}

		// ------------------------------------------------------------------------------------------
		//! Inserts specified element into collection at desired m_Index.
		//! @param key The key of the element that is going to be inserted.
		//! @param value The value of the element that is going to be inserted.
		//! @{
		GDINL void Insert(KeyType&& key, ValueType&& value = ValueType())
		{
			RedBlackTreeNodeType* const newNode = CreateNode(Forward<KeyType>(key), Forward<ValueType>(value));
			this->_InsertNode(newNode);
		}
		GDINL void Insert(KeyType const& key, ValueType const& value)
		{
			RedBlackTreeNodeType* const newNode = CreateNode(key, value);
			this->_InsertNode(newNode);
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Removes existing element from array at specified index.
		//! @param key The key of the element that is going to be RemoveFromSelfd.
		GDINL void Erase(KeyType const& key)
		{
			this->Erase(&key);
		}

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Overloaded operators.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		GDINL Map& operator= (Map&& OtherMap)
		{
			RedBlackTreeType::operator= (Forward<Map>(OtherMap));
			return *this;
		}

		// ------------------------------------------------------------------------------------------
		//! Returns reference on value of the element with specified key.
		//! @param key The key of the element we are looking for.
		//! @returns Reference on some element in the m_Container.
		//! @{
		GDINL ValueType const& operator[] (KeyType const& key) const
		{
			ConstIterator QueriedIterator = QueryIterator(key);
			GD_ASSERT(QueriedIterator != this->End(), "element with specified key does not exist.");
			return QueriedIterator->Value;
		}
		GDINL ValueType& operator[] (KeyType const& key)
		{
			return const_cast<ValueType&>(const_cast<Map const&>(*this)[key]);
		}
		//! @}
	};	// class Map

GD_NAMESPACE_END
