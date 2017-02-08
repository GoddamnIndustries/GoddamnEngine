// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Templates/Utility.h
 * Common utilities.
 */
#pragma once

#include <GoddamnEngine/Include.h>

GD_NAMESPACE_BEGIN
	
	// ------------------------------------------------------------------------------------------
	// R-Value references casting utilities.
	// ------------------------------------------------------------------------------------------

	namespace Utils
	{
		/*!
		 * Returns RValue reference to specified object.
		 * @param instance Some object.
		 */
		template<typename TSingature>
		GDINL auto&& Move(TSingature&& instance)
		{
			return static_cast<TypeTraits::RemoveReference<TSingature>&&>(instance);
		}

		/*!
		 * Returns an RValue reference to specified object if it is not an l-value reference.
		 * If specified object is an l-value reference, the function returns it without modifying it's type.
		 *
		 * @param instance Some object.
		 */
		//! @{
		template<typename TSingature>
		GDINL TSingature&& Forward(TypeTraits::RemoveReference<TSingature>& instance)
		{
			return static_cast<TSingature&&>(instance);
		}
		template<typename TSingature>
		GDINL TSingature&& Forward(TypeTraits::RemoveReference<TSingature>&& instance)
		{
			static_assert(!TypeTraits::IsLValueReference<TSingature>::Value, "Invocation of forward function with non-LValue reference type.");
			return static_cast<TSingature&&>(instance);
		}
		//! @}

	}	// namespace R

	// ------------------------------------------------------------------------------------------
	// Swapping utilities.
	// ------------------------------------------------------------------------------------------

	/*!
	 * Swaps values of two iterators.
	 *
	 * @param lhs First Iterator to swap.
	 * @param rhs Second Iterator to swap.
	 */
	template<typename TIterator>
	GDINL static void IteratorSwap(TIterator const lhsIter, TIterator const rhsIter)
	{
		Swap(*lhsIter, *rhsIter);
	}

	/*!
	 * Swaps value between two objects.
	 * You can overload this function for more efficient swapping operation.
	 *
	 * @param lhs First value to swap.
	 * @param rhs Second value to swap.
	 */
	//! @{
#if !GD_DOCUMENTATION
	template<typename TSingature>
	GDINL static void Swap(TSingature& lhs, TSingature& rhs);
#endif	// if !GD_DOCUMENTATION
	template<typename TSingature>
	GDINL static void Swap(TSingature& lhs, TSingature& rhs)
	{	
		if (&lhs != &rhs)
		{
			auto temp = Utils::Move(lhs);
			lhs = Utils::Move(rhs);
			rhs = Utils::Move(temp);
		}
	}
	template<typename TSingatureArray, SizeTp SignatureArraySize>
	GDINL static void Swap(TSingatureArray(&lhs)[SignatureArraySize], TSingatureArray(&rhs)[SignatureArraySize])
	{
		if (&lhs != &rhs)
		{
			auto const startSource = lhs;
			auto const endSource = lhs + SignatureArraySize;
			for (auto iterator = startSource; iterator != endSource; ++iterator)
			{
				GD::IteratorSwap(iterator, rhs + (iterator - startSource));
			}
		}
	}
	//! @}

	// ------------------------------------------------------------------------------------------
	// Other utilities.
	// ------------------------------------------------------------------------------------------

	/*!
	 *  Returns RValue reference on type without creating any instance.
	 */
	template<typename TSingature>
	GDINT TSingature&& DeclValue() = delete;
	
	// ------------------------------------------------------------------------------------------
	// Pair structure utilities.
	// ------------------------------------------------------------------------------------------
	
	//** ------------------------------------------------------------------------------------------**
	//! Stores key-value pair of objects. 
	//** ------------------------------------------------------------------------------------------**
	template<typename TFirst, typename TSecond>
	struct Pair final 
	{
		using FirstType  = TFirst;
		using SecondType = TSecond;

	public:
		TFirst	First;	//!< First element of the pair.
		TSecond	Second;	//!< Second element of the pair.

	public:
		
		/*!
		 * Initializes pair with default values.
		 */
		GDINL Pair() 
			: First(), Second() 
		{}
		
		/*!
		 * Initializes pair with specified values.
		 */
		//! @{
		GDINL Pair(TFirst&& First, TSecond&& Second) 
			: First(Forward<TFirst>(First)), Second(Forward<TSecond>(Second)) 
		{}
		GDINL Pair(TFirst const& First, TSecond const& Second) 
			: First(First), Second(Second) 
		{}
		//! @}

		// pair<F, S> == pair<K, V>
		template<typename TFirstLHS, typename TSecondLHS, typename TFirstRHS, typename TSecondRHS>
		GDINL friend bool operator== (Pair<TFirstLHS, TSecondLHS> const& lhs, Pair<TFirstRHS, TSecondRHS> const& rhs)
		{
			return lhs.First == rhs.First && lhs.Second == rhs.Second;
		}
		template<typename TFirstLHS, typename TSecondLHS, typename TFirstRHS, typename TSecondRHS>
		GDINL friend bool operator!= (Pair<TFirstLHS, TSecondLHS> const& lhs, Pair<TFirstRHS, TSecondRHS> const& rhs)
		{
			return lhs.First != rhs.First || lhs.Second != rhs.Second;
		}

		// pair<F, S> > pair<K, V>
		template<typename TFirstLHS, typename TSecondLHS, typename TFirstRHS, typename TSecondRHS>
		GDINL friend bool operator> (Pair<TFirstLHS, TSecondLHS> const& lhs, Pair<TFirstRHS, TSecondRHS> const& rhs)
		{
			return lhs.First > rhs.First ? true : (lhs.First < rhs.First ? false : lhs.First > rhs.First);
		}
		template<typename TFirstLHS, typename TSecondLHS, typename TFirstRHS, typename TSecondRHS>
		GDINL friend bool operator< (Pair<TFirstLHS, TSecondLHS> const& lhs, Pair<TFirstRHS, TSecondRHS> const& rhs)
		{
			return lhs.First < rhs.First ? true : (lhs.First > rhs.First ? false : lhs.First > rhs.First);
		}

	};	// struct Pair

	/*!
	 * Initializes a First-Second pair with specified parameters.
	 *
	 * @param first The first of the pair being created.
	 * @param second The second of the pair being created.
	 *
	 * @returns The First-Second pair instance.
	 */
	//! @{
	template<typename TFirst, typename TSecond>
	GDINL static Pair<TFirst, TSecond> MakePair(TFirst&& first, TSecond&& second)
	{
		return Pair<TFirst, TSecond>(Forward<TFirst>(first), Forward<TSecond>(second));
	}
	template<typename TFirst, typename TSecond>
	GDINL static Pair<TFirst, TSecond> MakePair(TFirst const& first, TSecond const& second)
	{
		return Pair<TFirst, TSecond>(first, second);
	}
	//! @}

GD_NAMESPACE_END
