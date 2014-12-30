/// ==========================================================================================
/// Pair.h - pair structure class interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.06.2013 - Created by James Jhuighuy
///		* 05.05.2014 - Rewritten from scratch by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_CONTAINERS_PAIR
#define GD_CORE_CONTAINERS_PAIR

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Utility.h>

GD_NAMESPACE_BEGIN

	/// Class key-value objects.
	template<typename KeyType, typename ValueType>
	struct Pair
	{
	public:
		typedef KeyType   ThisKeyType;		///< Type of this pair`s key
		typedef ValueType ValueTypeType;	///< Type of this pair`s values
		typedef typename Conditional<TypeTraits::IsPodType<KeyType>::Value && (sizeof(KeyType) <= sizeof(size_t)), KeyType const, KeyType const&>::Type KeyTypeConstRef;
		typedef typename Conditional<TypeTraits::IsPodType<ValueType>::Value && (sizeof(ValueType) <= sizeof(size_t)), ValueType const, ValueType const&>::Type ValueTypeConstRef;

		KeyType   Key;		///< Key element in Key-Value pair 
		ValueType Value;	///< Value element in Key-Value pair
#define First  Key
#define Second Value

		GDINL Pair() { }
		GDINL Pair(Pair const& OtherPair);
		GDINL Pair(Pair  && OtherPair);
		GDINL Pair(KeyType const& Key, ValueType const& Value);
		GDINL Pair(KeyType  && Key, ValueType  && Value);

		GDINL Pair& operator=(Pair const& OtherPair);
		GDINL Pair& operator=(Pair  && OtherPair);

		GDINL void SetValue(ValueType   && Value);
		GDINL void SetValue(ValueTypeConstRef Value);

	private:
  GDINL friend void Swap(Pair& First, Pair& Second)
		{	using GD::Swap;
			if (&First != &Second)
			{
				Swap(First.Key,   Second.Key);
				Swap(First.Value, Second.Value);
			}
		}
	};	// struct Pair

GD_NAMESPACE_END

#if (!defined(GD_CORE_CONTAINERS_PAIR))
#	error "Attempting to include 'Pair.inl' file. Please, use 'Pair.h' instead."
#endif	// if (!defined(GD_CORE_CONTAINERS_PAIR))

#include <GoddamnEngine/Core/Utility.h>

GD_NAMESPACE_BEGIN

// To prevent a lot of copy-pastes in code.
#define GD_PAIR_TEMPLATE() template<class KeyType, class ValueType>
#define GD_PAIR_CLASS() Pair<KeyType, ValueType>

GD_PAIR_TEMPLATE()
GDINL GD_PAIR_CLASS()::Pair(Pair const& OtherPair)
: Key(OtherPair.Key), Value(OtherPair.Value)
{
}

GD_PAIR_TEMPLATE()
GDINL GD_PAIR_CLASS()::Pair(Pair&& OtherPair)
: Key(Move(OtherPair.Key)), Value(Move(OtherPair.Value))
{
}

GD_PAIR_TEMPLATE()
GDINL GD_PAIR_CLASS()::Pair(KeyType const& Key, ValueType const& Value)
: Key(Key), Value(Value)
{
}

GD_PAIR_TEMPLATE()
GDINL GD_PAIR_CLASS()::Pair(KeyType&& Key, ValueType&& Value)
: Key(Forward<KeyType>(Key)), Value(Forward<ValueType>(Value))
{
}

GD_PAIR_TEMPLATE()
GDINL GD_PAIR_CLASS()& GD_PAIR_CLASS()::operator= (Pair const& OtherPair)
{
	if ((&OtherPair) != this)
	{
		this->Key = OtherPair.Key;
		this->Value = OtherPair.Value;
	}

	return (*this);
}

GD_PAIR_TEMPLATE()
GDINL GD_PAIR_CLASS()& GD_PAIR_CLASS()::operator= (Pair&& OtherPair)
{
	if ((&OtherPair) != this)
	{
		this->Key = Move(OtherPair.Key);
		this->Value = Move(OtherPair.Value);
	}

	return (*this);
}

#	undef GD_PAIR_TEMPLATE
#	undef GD_PAIR_CLASS
GD_NAMESPACE_END

#endif
