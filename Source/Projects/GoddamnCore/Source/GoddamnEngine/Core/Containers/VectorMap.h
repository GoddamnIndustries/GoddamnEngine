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
#include <GoddamnEngine/Core/Containers/Map.h>
#include <GoddamnEngine/Core/Containers/Vector.h>

GD_NAMESPACE_BEGIN
	
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                                 Map<K, V> class.                                 ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	// **------------------------------------------------------------------------------------------**
	//! Dynamically sized associative map that is implemented with dynamic arrays.
	//! Drop-in replacement for the map class.
	//!
	//! @tparam TKey Key type, used for searching.
	//! @tparam TValue Type of elements stored in the map.
	// **------------------------------------------------------------------------------------------**
	template<typename TKey, typename TValue, typename TAllocator = DefaultContainerAllocator>
	class VectorMap : public Vector<MapPair<TKey, TValue>>, public IIteratable<Map<TKey, TValue, TAllocator>>, public TNonCopyable
	{
	public:
		using PairType             = MapPair<TKey, TValue>;
		using ElementType          = PairType;
		using VectorType           = Vector<PairType, TAllocator>;
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
		GDINL VectorMap() = default;

		/*!
		 * Moves other map here.
		 * @param OtherMap Map would be moved into current object.
		 */
		GDINL VectorMap(VectorMap&& otherMap) = default;

		/*!
		 * Initializes map with default C++11's initializer list. You should not use this constructor manually.
		 * @param initializerList Initializer list passed by the compiler.
		 */
		GDINL VectorMap(InitializerList<PairType> const& initializerList)
		{
			for (auto const& element : initializerList)
			{
				this->Insert(element.Key, element.Value);
			}
		}

		GDINL ~VectorMap()
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
		 * @param element The element we are looking for.
		 * @returns Iterator on the element if it was found and End Iterator otherwise.
		 */
		//! @{
		GDINL ConstIterator FindIterator(TKey const& key) const
		{
			return GD::FindFirstIf(this->Begin(), this->End()
				, [&key](PairType const& pair) { return pair.Key == key; });
		}
		GDINL Iterator FindIterator(TKey const& key)
		{
			return GD::FindFirstIf(this->Begin(), this->End()
				, [&key](PairType const& pair) { return pair.Key == key; });
		}
		//! @}

		/*!
		 * Queries for the Iterator of the element with specified key.
		 *
		 * @param key The element we are looking for.
		 * @returns Iterator on the element if it was found and End Iterator otherwise.
		 */
		//! @{
		GDINL TValue const* Find(TKey const& key) const
		{
			auto const iter = FindIterator(key);
			return (iter != this->End()) ? &iter->Value : nullptr;
		}
		GDINL TValue* Find(TKey const& key)
		{
			return const_cast<TValue*>(const_cast<VectorMap const*>(this)->Find(key));
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
			GD_DEBUG_ASSERT(!this->Contains(key), "Element with specified key already exists.");
			VectorType::InsertLast(Move(PairType(Forward<TKey>(key), Forward<TValue>(value))));
			return this->GetLast().Value;
		}
		GDINL TValue& Insert(TKey const& key, TValue const& value)
		{
			GD_DEBUG_ASSERT(!this->Contains(key), "Element with specified key already exists.");
			VectorType::InsertLast(PairType(key, value));
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
			GD_DEBUG_ASSERT(foundIter != this->End(), "Specified element does not exist.");
			this->EraseAt(foundIter - this->Begin());
		}

	public:

		// ------------------------------------------------------------------------------------------
		// Overloaded operators.
		// ------------------------------------------------------------------------------------------

		GDINL VectorMap& operator= (VectorMap&& otherMap) = default;

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
				return const_cast<VectorMap*>(this)->Insert(Move(TKey(key)), Move(TValue()));
			}
			return queriedIterator->Value;
		}
		GDINL TValue& operator[] (TKey const& key)
		{
			return const_cast<TValue&>(const_cast<VectorMap const&>(*this)[key]);
		}
		//! @}
	};	// class VectorMap

	template<typename TKey, typename TValue>
	using GCVectorMap = VectorMap<TKey, TValue, GCContainerAllocator<Pair<TKey, TValue>>>;

GD_NAMESPACE_END

