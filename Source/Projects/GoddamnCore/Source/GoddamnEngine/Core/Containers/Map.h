// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Containers/Map.h
 * Dynamically sized associative map class.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/InitializerList.h>
#include <GoddamnEngine/Core/Containers/RedBlackTree/RedBlackTree.h>

GD_NAMESPACE_BEGIN
	
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                               MapPair<K, V> class.                               ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	// **------------------------------------------------------------------------------------------**
	//! Stores Key-value pair of objects. 
	//! All comparing operator compare keys only. 
	//! 
	//! @tparam KeyType Type of the Key element.
	//! @tparam ValueType Type of the value element.
	// **------------------------------------------------------------------------------------------**
	template<typename TKey, typename TValue>
	struct MapPair final 
	{
	public:
		TKey	Key;	//!< First element of the pair.
		TValue	Value;	//!< Second element of the pair.

	public:
		GDINL MapPair() 
			: Key(), Value()
		{
		}
		
		GDINL MapPair(TKey&& key, TValue&& value) 
			: Key(Forward<TKey>(key)), Value(Forward<TValue>(value))
		{
		}

		GDINL MapPair(TKey const& key, TValue const& value) 
			: Key(key), Value(value)
		{
		}

	public:
		GDINL friend bool operator> (MapPair<TKey, TValue> const& lhs, MapPair<TKey, TValue> const& rhs)
		{
			return lhs.Key > rhs.Key;
		}
		GDINL friend bool operator< (MapPair<TKey, TValue> const& lhs, MapPair<TKey, TValue> const& rhs)
		{
			return lhs.Key < rhs.Key;
		}

		GDINL friend bool operator== (MapPair<TKey, TValue> const& lhs, MapPair<TKey, TValue> const& rhs)
		{
			return lhs.Key == rhs.Key;
		}
		GDINL friend bool operator!= (MapPair<TKey, TValue> const& lhs, MapPair<TKey, TValue> const& rhs)
		{
			return lhs.Key != rhs.Key;
		}

	};	// struct MapPair

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                                 Map<K, V> class.                                 ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	// **------------------------------------------------------------------------------------------**
	//! Dynamically sized associative map that is implemented with Red-Black Trees.
	//! Red-Black Tree is used for elements access and searching.
	//!
	//! @tparam TKey Key type, used for searching.
	//! @tparam TValue Type of elements stored in the map.
	// **------------------------------------------------------------------------------------------**
	template<typename TKey, typename TValue, typename TAllocator = DefaultContainerAllocator>
	class Map : public RedBlackTree<MapPair<TKey, TValue>>, public IIteratable<Map<TKey, TValue, TAllocator>>
	{
	public:
		using PairType             = MapPair<TKey, TValue>;
		using ElementType          = PairType;
		using RedBlackTreeType     = RedBlackTree<PairType>;
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
		 * Initializes an empty map.
		 */
		GDINL Map() = default;

		/*!
		 * Moves other map here.
		 * @param otherMap Map would be moved into current object.
		 */
		GDINL Map(Map&& otherMap) = default;

		/*!
		 * Initializes map with default C++11's initializer list. You should not use this constructor manually.
		 * @param initializerList Initializer list passed by the compiler.
		 */
		GDINL Map(InitializerList<PairType> const& initializerList)
		{
			for (auto const& element : initializerList)
			{
				this->Insert(element.Key, element.Value);
			}
		}

		GDINL ~Map()
		{
			this->Clear();
		}

	public:

		// ------------------------------------------------------------------------------------------
		// Map manipulation.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Queries for the iterator of the element with specified key.
		 *
		 * @param key The key of the element we are looking for.
		 * @returns Iterator on the element if it was found and End Iterator otherwise.
		 */
		//! @{
		GDINL ConstIterator FindIterator(TKey const& key) const
		{
			return ConstIterator(*this, this->FindNodeBase(&key));
		}
		GDINL Iterator FindIterator(TKey const& key)
		{
			return Iterator(*this, this->FindNodeBase(&key));
		}
		//! @}

		/*!
		 * Queries for the Iterator of the element with specified key.
		 *
		 * @param key The element we are looking for.
		 * @returns Pointer to the element if it was found and null pointer otherwise.
		 */
		//! @{
		GDINL TValue const* Find(TKey const& key) const
		{
			auto const iterator = this->FindIterator(key);
			return iterator != this->End() ? &iterator->Value : nullptr;
		}
		GDINL TValue* Find(TKey const& key)
		{
			return const_cast<TValue*>(const_cast<Map const*>(this)->Find(key));
		}
		//! @}

		/*!
		 * Determines whether the element with specified key exists in the map.
		 *
		 * @param key The key of the element we are looking for.
		 * @returns True if element with specified key exists in the map, false otherwise.
		 */
		GDINL bool Contains(TKey const& key) const
		{
			return this->Find(key) != nullptr;
		}

		/*!
		 * Inserts the specified key-value pair into the map.
		 *
		 * @param key The key of the element that is going to be inserted.
		 * @param value The value of the element that is going to be inserted.
		 */
		//! @{
		GDINL TValue& Insert(TKey&& key, TValue&& value = TValue())
		{
			GD_DEBUG_VERIFY(!this->Contains(key), "Element with specified key already exists.");

			RedBlackTreeNodeType* const newNode = this->InternalCreateNode(Utils::Forward<TKey>(key), Utils::Forward<TValue>(value));
			this->InsertNodeBase(newNode);
			return newNode->GetData()->Value;
		}
		GDINL TValue& Insert(TKey const& key, TValue const& value = TValue())
		{
			GD_DEBUG_VERIFY(!this->Contains(key), "Element with specified key already exists.");

			RedBlackTreeNodeType* const newNode = this->InternalCreateNode(key, value);
			this->InsertNodeBase(newNode);
			return newNode->GetData()->Value;
		}
		//! @}

		/*!
		 * Removes existing element from array at specified index.
		 * @param key The key of the element that is going to be removed.
		 */
		GDINL void Erase(TKey const& key)
		{
			auto const foundNode = this->FindNodeBase(&key);
			GD_DEBUG_VERIFY(foundNode != this->GetNullNodeBase(), "Element with specified key does not exist.");
			this->RemoveNodeBase(foundNode);
		}

	public:

		// ------------------------------------------------------------------------------------------
		// Overloaded operators.
		// ------------------------------------------------------------------------------------------

		GDINL Map& operator= (Map&& otherMap) = default;

		/*!
		 * Returns reference on value of the element with specified key.
		 *
		 * @param key The key of the element we are looking for.
		 * @returns Reference on some element, or a newly created instance of the value type, that was automatically added to the map.
		 */
		//! @{
		GDINL TValue const& operator[] (TKey const& key) const
		{
			auto const queriedIterator = this->FindIterator(key);
			if (queriedIterator == this->End())
			{
				return const_cast<Map*>(this)->Insert(key);
			}
			return queriedIterator->Value;
		}
		GDINL TValue& operator[] (TKey const& key)
		{
			return const_cast<TValue&>(const_cast<Map const&>(*this)[key]);
		}
		//! @}

	};	// class VectorMap

GD_NAMESPACE_END
