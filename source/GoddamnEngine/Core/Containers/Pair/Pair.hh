//////////////////////////////////////////////////////////////////////////
/// Pair.hh - pair structure class interface.
/// Copyright (C) $(GD_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.06.2013 - Created by James Jhuighuy
///		* 05.05.2014 - Rewritten from scratch by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_CORE_CONTAINERS_PAIR
#define GD_CORE_CONTAINERS_PAIR

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/Utility.hh>

GD_NAMESPACE_BEGIN

	/// Class key-value objects
	template<typename KeyType, typename ValueType>
	struct Pair final
	{
	public:
		typedef KeyType   ThisKeyType;		///< Type of this pair`s key
		typedef ValueType ValueTypeType;	///< Type of this pair`s values

		KeyType   Key;		///< Key element in Key-Value pair 
		ValueType Value;	///< Value element in Key-Value pair
#define First  Key
#define Second Value

		GDINL Pair() { }
		GDINL Pair(Pair const& OtherPair);
		GDINL Pair(Pair     && OtherPair);
		GDINL Pair(KeyType const& Key, ValueType const& Value);
		GDINL Pair(KeyType     && Key, ValueType     && Value);

		GDINL Pair& operator=(Pair const& OtherPair);
		GDINL Pair& operator=(Pair     && OtherPair);

	private:
        GDINL friend void Swap(Pair& First, Pair& Second)
		{	using GD Swap;
			if (&First != &Second)
			{
				Swap(First.Key,   Second.Key);
				Swap(First.Value, Second.Value);
			}
		}
	};	// struct Pair

GD_NAMESPACE_END

#include <GoddamnEngine/Core/Containers/Pair/Pair.inl>

#endif
