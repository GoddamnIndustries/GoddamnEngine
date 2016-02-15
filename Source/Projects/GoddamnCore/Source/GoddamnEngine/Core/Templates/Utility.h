// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Utility.h
#pragma once

#include <GoddamnEngine/Include.h>

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
	template<typename TFirst, typename TSecond>
	struct Pair final 
	{
		using FirstType   = TFirst;
		using SecondType = TSecond;

	public:
		TFirst	First;	//!< First element of the pair.
		TSecond	Second;	//!< Second element of the pair.

	public:
		
		// ------------------------------------------------------------------------------------------
		//! Initializes First and Second with default Seconds.
		GDINL Pair() : First(), Second() {}
		
		// ------------------------------------------------------------------------------------------
		//! Initializes First and Second with specified Seconds.
		//! @{
		GDINL Pair(TFirst&& First, TSecond&& Second) : First(Forward<TFirst>(First)), Second(Forward<TSecond>(Second)) { }
		GDINL Pair(TFirst const& First, TSecond const& Second) : First(First), Second(Second) { }
		//! @}
	};	// struct Pair

	template<typename TFirst, typename TSecond>
	GDINL static bool operator> (Pair<TFirst, TSecond> const& lhs, Pair<TFirst, TSecond> const& rhs)
	{
	//	return lhs.First > rhs.First;
		assert(0);
	}

	template<typename TFirst, typename TSecond>
	GDINL static bool operator< (Pair<TFirst, TSecond> const& lhs, Pair<TFirst, TSecond> const& rhs)
	{
		assert(0);
	//	return lhs.First < rhs.First;
	}

	template<typename TFirst, typename TSecond>
	GDINL static bool operator== (Pair<TFirst, TSecond> const& lhs, Pair<TFirst, TSecond> const& rhs)
	{
		return lhs.First == rhs.First && lhs.Second == rhs.Second;
	}

	template<typename TFirst, typename TSecond>
	GDINL static bool operator!= (Pair<TFirst, TSecond> const& lhs, Pair<TFirst, TSecond> const& rhs)
	{
		return lhs.First != rhs.First || lhs.Second != rhs.Second;
	}
	
	// ------------------------------------------------------------------------------------------
	//! Initializes a First-Second pair with specified parameters.
	//! @param First First of the pair being created.
	//! @param Second Second of the pair being created.
	//! @returns The First-Second pair instance.
	//! @{
	template<typename TFirst, typename TSecond>
	GDINL static Pair<TFirst, TSecond> MakePair(TFirst&& First, TSecond&& Second)
	{
		return Pair<TFirst, TSecond>(Forward<TFirst>(First), Forward<TSecond>(Second));
	}
	template<typename TFirst, typename TSecond>
	GDINL static Pair<TFirst, TSecond> MakePair(TFirst const& First, TSecond const& Second)
	{
		return Pair<TFirst, TSecond>(First, Second);
	}
	//! @}
	
	// ------------------------------------------------------------------------------------------
	//! Initializes a First-Second pair with specified parameters in the heap. instance could be safely deallocated within @ref GD_DELETE macro.
	//! @param First First of the pair being created.
	//! @param Second Second of the pair being created.
	//! @returns pointer to the First-Second pair instance.
	//! @{
	template<typename TFirst, typename TSecond>	// Implemented inside Allocator.h - it depends on stuff, declared there.
	GDINL static Pair<TFirst, TSecond>* AllocatePair(TFirst&& First, TSecond&& Second);
	template<typename TFirst, typename TSecond>
	GDINL static Pair<TFirst, TSecond>* AllocatePair(TFirst const& First, TSecond const& Second);
	//! @}

	//! @}	// Utilities GoddamnEngine/Core/Utility.h

GD_NAMESPACE_END
