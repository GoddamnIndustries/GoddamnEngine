//////////////////////////////////////////////////////////////////////////
/// Pair.h - pair structure class implementation.
/// Copyright (C) $(GD_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.06.2013 - Created by James Jhuighuy
///		* 05.05.2014 - Rewritten from scratch by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#if (!defined(GD_CORE_CONTAINERS_PAIR))
#	error "Attempting to include 'Pair.inl' file. Please, use 'Pair.h' instead."
#endif	// if (!defined(GD_CORE_CONTAINERS_PAIR))

#include <GoddamnEngine/Core/Utility.h>

GD_NAMESPACE_BEGIN

// To prevent a lot of copy-pastes in code.
#define GD_PAIR_TEMPLATE() template<class KeyType, class ValueType>
#define GD_PAIR_CLASS()    Pair<KeyType, ValueType>

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
		if ((&OtherPair) != self)
		{
			self->Key   = OtherPair.Key;
			self->Value = OtherPair.Value;
		}

		return (*self);
	}

	GD_PAIR_TEMPLATE()
	GDINL GD_PAIR_CLASS()& GD_PAIR_CLASS()::operator= (Pair&& OtherPair)
	{ 
		if ((&OtherPair) != self)
		{
			self->Key   = Move(OtherPair.Key);
			self->Value = Move(OtherPair.Value);
		}

		return (*self);
	}

#	undef GD_PAIR_TEMPLATE
#	undef GD_PAIR_CLASS
GD_NAMESPACE_END
