// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Templates/Pair.h
//! Pair structure.
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/Utility.h>

GD_NAMESPACE_BEGIN

	//! Class key-value objects.
	template<typename KeyType, typename ValueType>
	struct Pair : IUncopiable
	{
	public:
		KeyType Key;
		ValueType Value;

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

		GDINL bool operator== (Pair const& Other) const
		{
			if (this->Key == Other.Key)
			//	if (this->Value == Other.Value)
					return true;
			return false;
		}

		GDINL bool operator!= (Pair const& Other) const
		{
			if (this->Key != Other.Key)
				if (this->Value != Other.Value)
					return true;
			return false;
		}

		GDINL bool operator> (Pair const& Other) const
		{
			return this->Key > Other.Key;
		}
		GDINL bool operator< (Pair const& Other) const
		{
			return this->Key < Other.Key;
		}
	};	// struct Pair

GD_NAMESPACE_END
