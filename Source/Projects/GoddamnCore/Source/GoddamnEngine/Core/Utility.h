// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Templates/Utility.h
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/TypeTraits.h>

GD_NAMESPACE_BEGIN
	
	
	//! @defgroup GDCoreUtilities Utilities <GoddamnEngine/Core/Templates/Utility.h>
	//! @ingroup  GDCoreTemplates
	//! Base template utilities.
	
	//! @{

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Swapping utilities.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Swaps values of two iterators.
	//! @param First First iterator to swap.
	//! @param Second Second iterator to swap.
	//! @{
#if !GD_DOCUMENTATION
	template<typename SignatureType>
	GDINL static void Swap(SignatureType& First, SignatureType& Second);
#endif	// if !GD_DOCUMENTATION
	template<typename IteratorType>
	GDINL static void IteratorSwap(IteratorType const FirstIter, IteratorType const SecondIter)
	{
		Swap(*FirstIter, *SecondIter);
	}
	//! @}

	// ------------------------------------------------------------------------------------------
	//! Swaps value between two objects.
	//! @param First First value to swap.
	//! @param Second Second value to swap.
	//! @note You can overload this function for more efficient swapping operation.
	//! @{
	template<typename SignatureType>
	GDINL static void Swap(SignatureType& First, SignatureType& Second)
	{	
		if (&First != &Second)
		{
			SignatureType Temporary = Move(First);
			First = Move(Second);
			Second = Move(Temporary);
		}
	}
	template<typename SignatureArrayType, SizeTp SignatureArraySize>
	GDINL static void Swap(SignatureArrayType(&First)[SignatureArraySize], SignatureArrayType(&Second)[SignatureArraySize])
	{
		if (&First != &Second)
		{
			SignatureArrayType* const StartSource = First;
			SignatureArrayType* const EndSource = First + SignatureArraySize;
			for (SignatureArrayType* Iter = StartSource; Iter != EndSource; ++Iter)
				IteratorSwap(Iter, Second + (Iter - StartSource));
		}
	}
	//! @}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// R-Value references casting utilities.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Returns RValue reference to specified object.
	//! @param Instance Some object.
	//! @returns RValue reference to specified object.
	template<typename SignatureType> 
	GDINL typename TypeTraits::RemoveReference<SignatureType>::Type&& Move(SignatureType&& Instance)
	{
		return static_cast<typename TypeTraits::RemoveReference<SignatureType>::Type&&>(Instance);
	}
	
	// ------------------------------------------------------------------------------------------
	//! Returns an RValue reference to specified object if it is not an l-value reference. 
	//! If specified object is an l-value reference, the function returns it without modifying it's type.
	//! @param Instance Some object.
	//! @returns RValue reference to specified object if it is not an l-value reference.
	//! @{
	template<typename SignatureType>
	GDINL SignatureType&& Forward(typename TypeTraits::RemoveReference<SignatureType>::Type& Instance)
	{	
		return (static_cast<SignatureType&&>(Instance));
	}
	template<typename SignatureType>
	GDINL SignatureType&& Forward(typename TypeTraits::RemoveReference<SignatureType>::Type&& Instance)
	{	
		static_assert(!TypeTraits::IsLValueReference<SignatureType>::Value, "Invocation of forward function with non-LValue reference type.");
		return static_cast<SignatureType&&>(Instance);
	}
	//! @}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Other utilities.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Returns RValue reference on type without creating any instance.
	//! @returns RValue reference on type without creating any instance.
	template<typename SignatureType>
	GDINT SignatureType&& DeclValue();
	
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Pair structure utilities.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	// ------------------------------------------------------------------------------------------
	//! Stores key-value pair of objects. 
	//! Two pairs are compared by their keys only, this behavior is uncommon to STL-like pairs.
	//! @tparam KeyTypeTp Type of the key element.
	//! @tparam ValueTypeTp Type of the value element.
	template<typename KeyTypeTp, typename ValueTypeTp>
	struct Pair final : public IUnassignable
	{
		using KeyType   = KeyTypeTp;
		using ValueType = ValueTypeTp;

	public:
		KeyType		Key;	//!< First element of the pair.
		ValueType	Value;	//!< Second element of the pair.

	public:

		
		//! Initializes key and value with default values.
		
		GDINL Pair() : Key(), Value() {}

		
		//! Initializes key and value with specified values.
		
		//! @{
		GDINL Pair(KeyType&& Key, ValueType&& Value) : Key(Forward<KeyType>(Key)), Value(Forward<ValueType>(Value)) { }
		GDINL Pair(KeyType const& Key, ValueType const& Value) : Key(Key), Value(Value) { }
		//! @}
	};	// struct Pair

	template<typename KeyType, typename ValueType>
	GDINL static bool operator> (Pair<KeyType, ValueType> const& LHS, Pair<KeyType, ValueType> const& RHS)
	{
		return LHS.Key > RHS.Key;
	}

	template<typename KeyType, typename ValueType>
	GDINL static bool operator< (Pair<KeyType, ValueType> const& LHS, Pair<KeyType, ValueType> const& RHS)
	{
		return LHS.Key < RHS.Key;
	}

	template<typename KeyType, typename ValueType>
	GDINL static bool operator== (Pair<KeyType, ValueType> const& LHS, Pair<KeyType, ValueType> const& RHS)
	{
		return LHS.Key == RHS.Key;
	}

	template<typename KeyType, typename ValueType>
	GDINL static bool operator!= (Pair<KeyType, ValueType> const& LHS, Pair<KeyType, ValueType> const& RHS)
	{
		return LHS.Key != RHS.Key;
	}

	
	// ------------------------------------------------------------------------------------------
	//! Initializes a key-value pair with specified parameters.
	//! @param Key Key of the pair being created.
	//! @param Value Value of the pair being created.
	//! @returns The key-value pair instance.
	//! @{
	template<typename KeyType, typename ValueType>
	GDINL static Pair<KeyType, ValueType> MakePair(KeyType&& Key, ValueType&& Value)
	{
		return Pair<KeyType, ValueType>(Forward<KeyType>(Key), Forward<ValueType>(Value));
	}
	template<typename KeyType, typename ValueType>
	GDINL static Pair<KeyType, ValueType> MakePair(KeyType const& Key, ValueType const& Value)
	{
		return Pair<KeyType, ValueType>(Key, Value);
	}
	//! @}

	
	// ------------------------------------------------------------------------------------------
	//! Initializes a key-value pair with specified parameters in the heap. Instance could be safely deallocated within @ref GD_DELETE macro.
	//! @param Key Key of the pair being created.
	//! @param Value Value of the pair being created.
	//! @returns Pointer to the key-value pair instance.
	//! @{
	template<typename KeyType, typename ValueType>	// Implemented inside Allocator.h - it depends on stuff, declared there.
	GDINL static Pair<KeyType, ValueType>* AllocatePair(KeyType&& Key, ValueType&& Value);
	template<typename KeyType, typename ValueType>
	GDINL static Pair<KeyType, ValueType>* AllocatePair(KeyType const& Key, ValueType const& Value);
	//! @}

	//! @}	// Utilities GoddamnEngine/Core/Templates/Utility.h

GD_NAMESPACE_END
