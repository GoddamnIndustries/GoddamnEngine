/// ==========================================================================================
/// Pair.h - pair structure.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_TEMPLATES_PAIR
#define GD_TEMPLATES_PAIR

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/Utility.h>

GD_NAMESPACE_BEGIN

	/// Class key-value objects.
	template<typename KeyType, typename ValueType>
	struct Pair
	{
	public:
		KeyType Key;
		ValueType Value;

		/// @todo Replace this macros with proper code. Possibly unions.
#define First Key
#define Second Value

		GDINL Pair() 
		{
		}

		GDINL Pair(KeyType&& Key, ValueType&& Value)
			: Key(Forward<KeyType>(Key)), Value(Forward<ValueType>(Value))
		{
		}

		GDINL Pair(KeyType const& Key, ValueType const& Value)
			: Key(Key), Value(Value)
		{
		}

	};	// struct Pair

GD_NAMESPACE_END

#endif	// ifndef GD_TEMPLATES_PAIR
