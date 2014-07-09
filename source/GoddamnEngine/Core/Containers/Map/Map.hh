/// ==========================================================================================
/// Map.hh - Dynamically sized associative vectors interfacs.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.06.2013 - Created by James Jhuighuy
///		* 05.05.2014 - Rewritten from scratch by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_CONTAINERS_MAP
#define GD_CORE_CONTAINERS_MAP

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/Containers/Pair/Pair.hh>
#include <GoddamnEngine/Core/Containers/Vector/Vector.hh>

GD_NAMESPACE_BEGIN

	/// Dynamically sized associative vector
	template<typename KeyType, typename ValueType, typename MemoryProviderType = VectorMemoryProviders::HeapMemoryProvider<Pair<KeyType, ValueType>>>
	class UnorderedMap final : public Vector<Pair<KeyType, ValueType>, MemoryProviderType>
	{
	public /*Type definitions*/:
        typedef Vector<Pair<KeyType, ValueType>, MemoryProviderType> ThisBaseType;
        typedef typename ThisBaseType::ConstIterator ConstIterator;
        typedef typename ThisBaseType::MutableIterator MutableIterator;
		typedef KeyType ThisKeyType;
		typedef ValueType ThisValueType;
		typedef Pair<KeyType, ValueType> ThisPairType;
		typedef UnorderedMap ThisUnorderedMapType;

	public /*Class API*/:
		GDINL UnorderedMap(size_t const InitialElemntsCount = 0, ThisPairType const& InitialPair = ThisPairType(), size_t const Capacity = -1);
		GDINL UnorderedMap(ConstIterator const StartIterator, ConstIterator const EndIterator);
		GDINL UnorderedMap(std::initializer_list<ThisPairType> const& InitializerList);
		GDINL UnorderedMap(UnorderedMap     && OtherMap);
		GDINL UnorderedMap(UnorderedMap const& OtherMap);
#if (defined(__cplusplus_cli))
		GDINL UnorderedMap(array<ThisPairType>^ CliArray);
#endif	// if (defined(__cplusplus_cli))

		/// Adds new pair to end of list.
		GDINL void PushLast(KeyType const& Key, ValueType const& Value);

		/// Searches for first element in container by checking equality.
		/// @returns Index of found element or @c SIZE_MAX if nothing was found.
		GDINL size_t FindFirstElement(KeyType const& Key) const;

		/// Searches for last element in container by checking equality.
		/// @returns Index of found element or @c SIZE_MAX if nothing was found.
		GDINL size_t FindLastElement(KeyType const& Key) const;

	public /*Operators*/:
		GDINL UnorderedMap& operator= (UnorderedMap     && OtherMap);
		GDINL UnorderedMap& operator= (UnorderedMap const& OtherMap);
		GDINL UnorderedMap& operator= (std::initializer_list<ThisPairType> const& InitializerList);
#if (defined(__cplusplus_cli))
		GDINL Vector& operator= (array<ThisPairType>^ CliArray);
#endif	// if (defined(__cplusplus_cli))

		GDINL ValueType const& operator[] (KeyType const& Key) const;
		GDINL ValueType      & operator[] (KeyType const& Key);

	private:
        GDINL friend void Swap(UnorderedMap& First, UnorderedMap& Second)
		{	using GD Swap;	/// @todo Does not compiles without this line. Do not know the reason.
            using ThisVectorType = typename UnorderedMap::ThisVectorType;
			Swap(static_cast<ThisVectorType&>(First), static_cast<ThisVectorType&>(Second));
		}
	};	// class UnorderedMap;

	template<typename KeyType, typename ValueType>
	using HeapUnorderedMap = UnorderedMap<KeyType, ValueType>;

	template< typename KeyType, typename ValueType, size_t const Capacity>
	using StackUnorderedMap = UnorderedMap<KeyType, ValueType, VectorMemoryProviders::StackMemoryProvider<Pair<KeyType, ValueType>, Capacity>>;

#if 0
	template<typename KeyType, typename ValueType, typename MemoryProviderType = VectorMemoryProviders::HeapMemoryProvider<Pair<KeyType, ValueType>>>
	class Map final : public Vector<Pair<KeyType, ValueType>, MemoryProviderType>
	{
	public /*Type definitions*/:
		typedef KeyType ThisKeyType;
		typedef ValueType ThisValueType;
		typedef Pair<KeyType, ValueType> ThisPairType;
		typedef Map ThisUnorderedMapType;

	public /*Class API*/:
		GDINL Map(size_t const InitialElemntsCount = 0, ThisPairType const& InitialPair = ThisPairType(), size_t const Capacity = -1);
		GDINL Map(ConstIterator const StartIterator, ConstIterator const EndIterator);
		GDINL Map(std::initializer_list<ThisPairType> const& InitializerList);
		GDINL Map(Map     && OtherMap);
		GDINL Map(Map const& OtherMap);
#if (defined(__cplusplus_cli))
		GDINL Map(array<ThisPairType>^ CliArray);
#endif	// if (defined(__cplusplus_cli))

		GDINL Map& operator= (Map     && OtherMap);
		GDINL Map& operator= (Map const& OtherMap);
		GDINL Map& operator= (std::initializer_list<ThisPairType> const& InitializerList);
#if (defined(__cplusplus_cli))
		GDINL Vector& operator= (array<ThisPairType>^ CliArray);
#endif	// if (defined(__cplusplus_cli))

		GDINL ValueType const& operator[] (KeyType const& Key) const;
		GDINL ValueType      & operator[] (KeyType const& Key);

	public:
		GDINL friend void Swap(_Inout_ Map& First,
			_Inout_ Map& Second)
		{
			using GD Swap;	/// @todo Does not compiles without this line. Do not know the reason.
			Swap(static_cast<ThisVectorType&>(First), static_cast<ThisVectorType&>(Second));
		}
	};	// class Map;
#else
#	define Map UnorderedMap
#endif

GD_NAMESPACE_END

#include <GoddamnEngine/Core/Containers/Map/Map.inl>

#endif
