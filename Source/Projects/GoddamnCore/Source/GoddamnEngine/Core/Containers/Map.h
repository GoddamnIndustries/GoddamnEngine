/// ==========================================================================================
/// Map.h - Dynamically sized associative container interface.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_CONTAINERS_MAP
#define GD_CORE_CONTAINERS_MAP

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/Pair.h>
#include <GoddamnEngine/Core/Miscellaneous/Hash.h>
#include <GoddamnEngine/Core/Containers/Containers.h>
#include <GoddamnEngine/Core/Containers/InitializerList.h>
#include <GoddamnEngine/Core/Containers/RedBlackTree/RedBlackTree.h>

GD_NAMESPACE_BEGIN

	/// @brief Dynamically sized associative container that is implemented with Red-Black Trees.
	/// 	   Current implementation has a MASSIVE overhead per each element (33 bytes by Red-Black Tree). 
	/// 	   Red-Black Tree is used for elements access and searching, Lists for elements rapid iteration.
	/// @tparam KeyType Key type, used for searching.
	/// @tparam ValueType Type of elements stored in the container.
	template<typename KeyType, typename ValueType>
	class Map final : public IContainer, public RedBlackTree<Pair<KeyType const, ValueType>>
		, public ContainerIteratableTag
		, public ContainerReverseIteratableTag
	{
	public:
		typedef Pair<KeyType const, ValueType> PairType;
		typedef RedBlackTree<PairType> RedBlackTreeType;
		typedef RedBlackTreeNode<PairType> RedBlackTreeNodeType;

		typedef RedBlackTreeIterator<Map, RedBlackTreeNodeType> Iterator;
		typedef RedBlackTreeIterator<Map const, RedBlackTreeNode<PairType> const> ConstIterator;
		GD_CONTAINER_CHECK_ITERATORS(Map);

		typedef ReverseContainerIterator<Iterator> ReverseIterator;
		typedef ReverseContainerIterator<ConstIterator> ReverseConstIterator;
		GD_CONTAINER_CHECK_REVERSE_ITERATORS(Map);

		GD_CONTAINER_DEFINE_ITERATION_SUPPORT(Map);

	public:

		/// @brief Initializes an empty map.
		GDINL Map()
			: RedBlackTreeType()
		{
		}

		/// @brief Initializes map with default C++11's initializer list. You should not use this constructor manually.
		/// @param InitializerList Initializer list passed by the compiler.
		GDINL Map(InitializerList<PairType> const& InitializerList)
			: RedBlackTreeType()
		{
			for (auto const& Element : InitializerList) {
				this->InsertKeyValue(Element.Key, Element.Value);
			}
		}

		/// @brief Moves other vector here.
		/// @param OtherVector Vector would be moved into current object.
		GDINL Map(Map&& OtherVector)
			: RedBlackTreeType(Forward<Map>(OtherVector))
		{
		}

	private:

		/// @brief Compares Elements of the nodes by keys. 
		/// @param First First Element.
		/// @param Second Second Element.
		/// @returns Zero, if specified Elements are equal, positive value if first is greater, negative otherwise.
		GDINL virtual int CompareElements(chandle const First, chandle const Second) const final override
		{
			KeyType const& FirstKey = reinterpret_cast<PairType const*>(First)->Key;
			KeyType const& SecondKey = reinterpret_cast<PairType const*>(Second)->Key;
			if (FirstKey == SecondKey) {
				return 0;
			} else if (FirstKey > SecondKey) {
				return 1;
			} else {
				return -1;
			}
		}

	public:

		/// @brief Returns iterator that points to first container element.
		/// @returns Iterator that points to first container element.
		/// @{
		GDINL Iterator Begin()
		{
			return Iterator(*this, this->RedBlackTreeType::GetFirstNode());
		}
		GDINL ConstIterator Begin() const
		{
			return ConstIterator(*this, this->RedBlackTreeType::GetFirstNode());
		}
		/// @}

		/// @brief Returns iterator that points to past the end container element.
		/// @returns Iterator that points to past the end container element.
		/// @{
		GDINL Iterator End()
		{
			return Iterator(*this, this->RedBlackTreeType::GetNullNode());
		}
		GDINL ConstIterator End() const
		{
			return ConstIterator(*this, this->RedBlackTreeType::GetNullNode());
		}
		/// @}

		/// @brief Returns iterator that points to last container element.
		/// @returns Iterator that points to last container element.
		/// @{
		GDINL ReverseIterator ReverseBegin()
		{
			return ReverseIterator(*this, this->RedBlackTreeType::GetLastNode());
		}
		GDINL ReverseConstIterator ReverseBegin() const
		{
			return ReverseConstIterator(*this, this->RedBlackTreeType::GetLastNode());
		}
		/// @}

		/// @brief Returns iterator that points to preceding the first container element.
		/// @returns Iterator that points to preceding the first container element.
		/// @{
		GDINL ReverseIterator ReverseEnd()
		{
			return ReverseIterator(*this, this->RedBlackTreeType::GetNullNode());
		}
		GDINL ReverseConstIterator ReverseEnd() const
		{
			return ReverseConstIterator(*this, this->RedBlackTreeType::GetNullNode());
		}
		/// @}
	
	public:

		/// @brief Queries for the iterator of the element with specified key.
		/// @param Key Key of the element we are looking for.
		/// @returns Iterator on the element if it was found and End iterator otherwise.
		/// @{
		GDINL ConstIterator QueryIterator(KeyType const& Key) const
		{
			return ConstIterator(*this, this->RedBlackTreeType::Query(&Key));
		}
		GDINL Iterator QueryIterator(KeyType const& Key)
		{
			return Iterator(*this, this->RedBlackTreeType::Query(&Key));
		}
		/// @}

		/// @brief Determines whether the element with specified key exists in the container.
		/// @param Key Key of the element we are looking for.
		/// @returns True if element with specified key exists in the container, false otherwise.
		GDINL bool Contains(KeyType const& Key) const
		{
			return this->QueryIterator(Key) != this->End();
		}

		/// @brief Inserts specified element into collection at desired index.
		/// @param Key Key of the element that is going to be inserted.
		/// @param Value Value of the element that is going to be inserted.
		/// @{
		GDINL void InsertKeyValue(KeyType&& Key, ValueType&& Value = ValueType())
		{
			RedBlackTreeNodeType* const NewNode = new RedBlackTreeNodeType(new PairType(Forward<KeyType>(Key), Forward<ValueType>(Value)));
			this->RedBlackTreeType::Insert(NewNode);
		}
		GDINL void InsertKeyValue(KeyType const& Key, ValueType const& Value)
		{
			RedBlackTreeNodeType* const NewNode = new RedBlackTreeNodeType(new PairType(Key, Value));
			this->RedBlackTreeType::Insert(NewNode);
		}
		/// @}

		/// @brief Removes existing element from array at specified index.
		/// @param Key Key of the element that is going to be removed.
		GDINL void RemoveElementWithKey(KeyType const& Key)
		{
			Iterator QueriedIterator = this->QueryIterator(Key);
			GD_ASSERT(QueriedIterator != this->End(), "Element with specified key does not exist.");
			
			auto const QueriedNode = const_cast<RedBlackTreeNodeType*>(QueriedIterator.GetNode());
			this->RedBlackTreeType::Delete(QueriedNode);
			delete QueriedNode->GetElement();
			delete QueriedNode;
		}

	public:

		/// @brief Moves other map here.
		/// @param OtherMap Other map that would be moved into current object.
		/// @returns this.
		GDINL Map& operator= (Map&& OtherMap)
		{
			RedBlackTreeType::operator= (Forward<Map>(OtherMap));
			return *this;
		}

		/// @brief Returns reference on value of the element with specified key.
		/// @param Key Key of the element we are looking for.
		/// @returns Reference on some element in the container.
		/// @{
		GDINL ValueType const& operator[] (KeyType const& Key) const
		{
			ConstIterator QueriedIterator = this->QueryIterator(Key);
			GD_ASSERT(QueriedIterator != this->End(), "Element with specified key does not exist.");
			return QueriedIterator->Value;
		}
		GDINL ValueType& operator[] (KeyType const& Key)
		{
			return const_cast<ValueType&>(const_cast<Map const&>(*this)[Key]);
		}
		/// @}
	};	// class Map

	/// @brief Map with hash codes used as keys.
	template<typename ValueType>
	using HashMap = Map<HashCode, ValueType>;

GD_NAMESPACE_END

#endif	// ifndef GD_CORE_CONTAINERS_MAP
