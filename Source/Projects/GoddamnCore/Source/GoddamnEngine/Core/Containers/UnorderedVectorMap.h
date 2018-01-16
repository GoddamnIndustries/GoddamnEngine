// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
* @file GoddamnEngine/Core/Containers/UnorderedVectorMap.h
* Dynamically sized vector-based associative container class.
*/
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/Map.h>
#include <GoddamnEngine/Core/Containers/Vector.h>

GD_NAMESPACE_BEGIN
	
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                                 Map<K, V> class.                                 ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	// **------------------------------------------------------------------------------------------**
	//! Dynamically sized associative container that is implemented with dynamic arrays.
	//! Drop-in replacement for the map class.
	//!
	//! @tparam TKey Key type, used for searching.
	//! @tparam TValue Type of elements stored in the map.
	//! @tparam TAllocator Allocator used by this map.
	// **------------------------------------------------------------------------------------------**
	template<typename TKey, typename TValue, typename TAllocator = DefaultContainerAllocator>
	class UnorderedVectorMap : private Vector<MapPair<TKey, TValue>, TAllocator>, public TNonCopyable
	{
	public:
		using PairType             = MapPair<TKey, TValue>;
		using ElementType          = PairType;
	private:
		using VectorType           = Vector<PairType, TAllocator>;
	public:
		using Iterator             = typename VectorType::Iterator;
		using ConstIterator        = typename VectorType::ConstIterator;
		using ReverseIterator      = typename VectorType::ReverseIterator;
		using ReverseConstIterator = typename VectorType::ReverseConstIterator;

	public:

		// ------------------------------------------------------------------------------------------
		// Constructor and destructor.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Initializes an empty map.
		 */
		GDINL UnorderedVectorMap() = default;

		/*!
		 * Moves other map here.
		 * @param otherMap Map would be moved into current object.
		 */
		GDINL UnorderedVectorMap(UnorderedVectorMap&& otherMap) = default;

		/*!
		 * Initializes map with default C++11's initializer list. You should not use this constructor manually.
		 * @param initializerList Initializer list passed by the compiler.
		 */
		GDINL UnorderedVectorMap(InitializerList<PairType> const& initializerList)
		{
			for (auto const& element : initializerList)
			{
				this->Insert(element.Key, element.Value);
			}
		}

		GDINL ~UnorderedVectorMap()
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
		// Map manipulation.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Returns number of the pairs in the map.
		 */
		GDINL SizeTp GetLength() const
		{
			return this->VectorType::GetLength();
		}

		/*!
		 * Returns true if this map is empty.
		 */
		GDINL bool IsEmpty() const
		{
			return this->VectorType::IsEmpty();
		}

		/*!
		 * Removes all pairs from the map.
		 */
		GDAPI void Clear()
		{
			this->VectorType::Clear();
		}

		/*!
		 * Queries for the iterator of the element with specified key.
		 *
		 * @param key The key of the element we are looking for.
		 * @returns Iterator on the element if it was found and End Iterator otherwise.
		 */
		//! @{
		GDINL ConstIterator FindIterator(TKey const& key) const
		{
			return Algo::FindFirstIf(this->Begin(), this->End(), [&key](PairType const& pair) { return pair.Key == key; });
		}
		GDINL Iterator FindIterator(TKey const& key)
		{
			return Algo::FindFirstIf(this->Begin(), this->End(), [&key](PairType const& pair) { return pair.Key == key; });
		}
		//! @}

		/*!
		 * Queries for the iterator of the element with specified key.
		 *
		 * @param key The element we are looking for.
		 * @returns Iterator on the element if it was found and End Iterator otherwise.
		 */
		//! @{
		GDINL TValue const* Find(TKey const& key) const
		{
			auto const iter = this->FindIterator(key);
			return (iter != this->End()) ? &iter->Value : nullptr;
		}
		GDINL TValue* Find(TKey const& key)
		{
			return const_cast<TValue*>(const_cast<UnorderedVectorMap const*>(this)->Find(key));
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

			this->VectorType::InsertLast(Utils::Move(PairType(Utils::Forward<TKey>(key), Utils::Forward<TValue>(value))));
			return this->GetLast().Value;
		}
		GDINL TValue& Insert(TKey const& key, TValue const& value = TValue())
		{
			GD_DEBUG_VERIFY(!this->Contains(key), "Element with specified key already exists.");

			this->VectorType::InsertLast(PairType(key, value));
			return this->GetLast().Value;
		}
		//! @}

		/*!
		 * Removes existing element from array at specified index.
		 * @param key The key of the element that is going to be removed.
		 */
		GDINL void Erase(TKey const& key)
		{
			auto const foundIter = this->FindIterator(key);
			GD_DEBUG_VERIFY(foundIter != this->End(), "Specified element does not exist.");
			this->EraseAt(foundIter - this->Begin());
		}

	public:

		// ------------------------------------------------------------------------------------------
		// Overloaded operators.
		// ------------------------------------------------------------------------------------------

		GDINL UnorderedVectorMap& operator= (UnorderedVectorMap&& otherMap) = default;

		/*!
		 * Returns reference on value of the element with specified key.
		 *
		 * @param key The key of the element we are looking for.
		 * @returns Reference on some element, or a newly created instance of the value type, that was automatically added to the map.
		 */
		GDINL TValue& operator[] (TKey const& key)
		{
			auto const queriedIterator = this->FindIterator(key);
			if (queriedIterator == this->End())
			{
				return this->Insert(key);
			}
			return queriedIterator->Value;
		}

		/*!
		 * Returns reference on value of the element with specified key.
		 *
		 * @param key The key of the element we are looking for.
		 * @returns Reference on some element.
		 */
		GDINL TValue const& operator[] (TKey const& key) const
		{
			auto const queriedIterator = this->FindIterator(key);
			GD_DEBUG_VERIFY(queriedIterator == this->End(), "Element with specified key does not exist.");
			return queriedIterator->Value;
		}

	};	// class VectorMap

GD_NAMESPACE_END

