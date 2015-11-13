// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Utility.h
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/TypeTraits.h>

GD_NAMESPACE_BEGIN
	
	
	//! @defgroup GDCoreUtilities Utilities <GoddamnEngine/Core/Utility.h>
	//! @ingroup  GDCoreTemplates
	//! Base template utilities.
	
	//! @{

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Swapping utilities.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Swaps values of two iterators.
	//! @param lhs First Iterator to swap.
	//! @param rhs Second Iterator to swap.
	//! @{
#if !GD_DOCUMENTATION
	template<typename TSingature>
	GDINL static void Swap(TSingature& lhs, TSingature& rhs);
#endif	// if !GD_DOCUMENTATION
	template<typename TIterator>
	GDINL static void IteratorSwap(TIterator const lhsIter, TIterator const rhsIter)
	{
		Swap(*lhsIter, *rhsIter);
	}
	//! @}

	// ------------------------------------------------------------------------------------------
	//! Swaps value between two objects.
	//! @param lhs First value to swap.
	//! @param rhs Second value to swap.
	//! @note You can overload this function for more efficient swapping operation.
	//! @{
	template<typename TSingature>
	GDINL static void Swap(TSingature& lhs, TSingature& rhs)
	{	
		if (&lhs != &rhs)
		{
			TSingature temp = Move(lhs);
			lhs = Move(rhs);
			rhs = Move(temp);
		}
	}
	template<typename TSingatureArray, SizeTp SignatureArraySize>
	GDINL static void Swap(TSingatureArray(&lhs)[SignatureArraySize], TSingatureArray(&rhs)[SignatureArraySize])
	{
		if (&lhs != &rhs)
		{
			TSingatureArray* const startSource = lhs;
			TSingatureArray* const endSource = lhs + SignatureArraySize;
			for (TSingatureArray* iterator = startSource; iterator != endSource; ++iterator)
				IteratorSwap(iterator, rhs + (iterator - startSource));
		}
	}
	//! @}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// R-Value references casting utilities.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Returns RValue reference to specified object.
	//! @param instance Some object.
	//! @returns RValue reference to specified object.
	template<typename TSingature> 
	GDINL typename TypeTraits::RemoveReference<TSingature>::Type&& Move(TSingature&& instance)
	{
		return static_cast<typename TypeTraits::RemoveReference<TSingature>::Type&&>(instance);
	}
	
	// ------------------------------------------------------------------------------------------
	//! Returns an RValue reference to specified object if it is not an l-value reference. 
	//! If specified object is an l-value reference, the function returns it without modifying it's type.
	//! @param instance Some object.
	//! @returns RValue reference to specified object if it is not an l-value reference.
	//! @{
	template<typename TSingature>
	GDINL TSingature&& Forward(typename TypeTraits::RemoveReference<TSingature>::Type& instance)
	{	
		return (static_cast<TSingature&&>(instance));
	}
	template<typename TSingature>
	GDINL TSingature&& Forward(typename TypeTraits::RemoveReference<TSingature>::Type&& instance)
	{	
		static_assert(!TypeTraits::IsLValueReference<TSingature>::Value, "Invocation of forward function with non-LValue reference type.");
		return static_cast<TSingature&&>(instance);
	}
	//! @}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// other utilities.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Returns RValue reference on type without creating any instance.
	//! @returns RValue reference on type without creating any instance.
	template<typename TSingature>
	GDINT TSingature&& DeclValue()
	{
		return{};
	}
	
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Pair structure utilities.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	// ------------------------------------------------------------------------------------------
	//! Stores Key-value pair of objects. 
	//! Two pairs are compared by their keys only, this behavior is uncommon to STL-like pairs.
	//! @tparam KeyType Type of the Key element.
	//! @tparam ValueType Type of the value element.
	template<typename TKey, typename TValue>
	struct Pair final //: public IUnassignable
	{
		using KeyType   = TKey;
		using ValueType = TValue;

	public:
		TKey	Key;	//!< First element of the pair.
		TValue	Value;	//!< Second element of the pair.

	public:
		
		// ------------------------------------------------------------------------------------------
		//! Initializes Key and value with default values.
		GDINL Pair() : Key(), Value() {}
		
		// ------------------------------------------------------------------------------------------
		//! Initializes Key and value with specified values.
		//! @{
		GDINL Pair(TKey&& key, TValue&& value) : Key(Forward<TKey>(key)), Value(Forward<TValue>(value)) { }
		GDINL Pair(TKey const& key, TValue const& value) : Key(key), Value(value) { }
		//! @}
	};	// struct Pair

	template<typename TKey, typename TValue>
	GDINL static bool operator> (Pair<TKey, TValue> const& lhs, Pair<TKey, TValue> const& rhs)
	{
		return lhs.Key > rhs.Key;
	}

	template<typename TKey, typename TValue>
	GDINL static bool operator< (Pair<TKey, TValue> const& lhs, Pair<TKey, TValue> const& rhs)
	{
		return lhs.Key < rhs.Key;
	}

	template<typename TKey, typename TValue>
	GDINL static bool operator== (Pair<TKey, TValue> const& lhs, Pair<TKey, TValue> const& rhs)
	{
		return lhs.Key == rhs.Key;
	}

	template<typename TKey, typename TValue>
	GDINL static bool operator!= (Pair<TKey, TValue> const& lhs, Pair<TKey, TValue> const& rhs)
	{
		return lhs.Key != rhs.Key;
	}
	
	// ------------------------------------------------------------------------------------------
	//! Initializes a Key-value pair with specified parameters.
	//! @param Key Key of the pair being created.
	//! @param Value Value of the pair being created.
	//! @returns The Key-value pair instance.
	//! @{
	template<typename TKey, typename TValue>
	GDINL static Pair<TKey, TValue> MakePair(TKey&& key, TValue&& value)
	{
		return Pair<TKey, TValue>(Forward<TKey>(key), Forward<TValue>(value));
	}
	template<typename TKey, typename TValue>
	GDINL static Pair<TKey, TValue> MakePair(TKey const& key, TValue const& value)
	{
		return Pair<TKey, TValue>(key, value);
	}
	//! @}
	
	// ------------------------------------------------------------------------------------------
	//! Initializes a Key-value pair with specified parameters in the heap. instance could be safely deallocated within @ref GD_DELETE macro.
	//! @param Key Key of the pair being created.
	//! @param Value Value of the pair being created.
	//! @returns pointer to the Key-value pair instance.
	//! @{
	template<typename TKey, typename TValue>	// Implemented inside Allocator.h - it depends on stuff, declared there.
	GDINL static Pair<TKey, TValue>* AllocatePair(TKey&& key, TValue&& value);
	template<typename TKey, typename TValue>
	GDINL static Pair<TKey, TValue>* AllocatePair(TKey const& key, TValue const& value);
	//! @}

	//! @}	// Utilities GoddamnEngine/Core/Utility.h

GD_NAMESPACE_END
