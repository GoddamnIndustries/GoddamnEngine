/// ==========================================================================================
/// Map.h - Dynamically sized associative container interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_CONTAINERS_MAP
#define GD_CORE_CONTAINERS_MAP

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/Containers.h>
#include <GoddamnEngine/Core/Containers/Pair.h>
#include <GoddamnEngine/Core/Containers/RedBlackTree/RedBlackTree.h>

GD_NAMESPACE_BEGIN

	/// @brief Dynamically sized associative container that is implemented with Red-Black Trees.
	/// 	   Current implementation has a MASSIVE overhead per each element (33 bytes by Red-Black Tree). 
	/// 	   Red-Black Tree is used for elements access and searching, Lists for elements rapid iteration.
	template<typename KeyType, typename ValueType>
	class Map : public RedBlackTree, public ContainerIteratableTag, public ContainerReverseIteratableTag
	{
	private:
		typedef Pair<KeyType, ValueType> PairType;
		typedef RedBlackTreeNode<PairType> RedBlackTreeNodeType;
		typedef typename PairType::KeyType const& KeyType const&;
		typedef typename PairType::ValueType const& ValueType const&;

		typedef RedBlackTreeIterator<Map, RedBlackTreeNodeType> Iterator;
		typedef RedBlackTreeIterator<Map const, RedBlackTreeNode<PairType const>> ConstIterator;
		GD_CONTAINER_CHECK_ITERATORS(Map);

		typedef ReverseContainerIterator<Iterator> ReverseIterator;
		typedef ReverseContainerIterator<ConstIterator> ReverseConstIterator;
		GD_CONTAINER_CHECK_REVERSE_ITERATORS(Map);

		GD_CONTAINER_DEFINE_PTR_ITERATION_SUPPORT(Map);

	public:

		/// @brief Initializes map with default C++11's initializer list. You should not use this constructor manually.
		/// @param InitializerList Initializer list passed by the compiler.
		GDINL Map(InitializerList<PairType> const& InitializerList)
			: RedBlackTree()
		{
			for (auto const& Element : InitializerList) {
				this->Insert(Element.Key, Element.Value);
			}
		}

		/// @brief Moves other vector here.
		/// @param OtherVector Vector would be moved into current object.
		GDINL Map(Map&& OtherVector)
			: RedBlackTree(Forward<Map>(OtherVector))
		{
		}

	private:

		/// @brief Compares comparands of the nodes by keys. 
		/// @param First First comparand.
		/// @param Second Second comparand.
		/// @returns Zero, if specified comparands are equal, positive value if first is greater, negative otherwise.
		GDINL virtual int CompareComparands(chandle const First, chandle const Second) const final override
		{
			KeyType const* FirstKey = reinterpret_cast<PairType const*>(First)->Key;
			KeyType const* SecondKey = reinterpret_cast<PairType const*>(Second)->Key;
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
			return Iterator(*this, this->GetFirstNode());
		}
		GDINL ConstIterator Begin() const
		{
			return ConstIterator(*this, this->GetFirstNode());
		}
		/// @}

		/// @brief Returns iterator that points to past the end container element.
		/// @returns Iterator that points to past the end container element.
		/// @{
		GDINL Iterator End()
		{
			return Iterator(*this, this->GetNullNode());
		}
		GDINL ConstIterator End() const
		{
			return Iterator(*this, this->GetNullNode());
		}
		/// @}

		/// @brief Returns iterator that points to last container element.
		/// @returns Iterator that points to last container element.
		/// @{
		GDINL ReverseIterator ReverseBegin()
		{
			return ReverseIterator(*this, this->GetLastNode());
		}
		GDINL ReverseConstIterator ReverseBegin() const
		{
			return ReverseConstIterator(*this, this->GetLastNode());
		}
		/// @}

		/// @brief Returns iterator that points to preceding the first container element.
		/// @returns Iterator that points to preceding the first container element.
		/// @{
		GDINL ReverseIterator ReverseEnd()
		{
			return Iterator(*this, this->GetNullNode());
		}
		GDINL ReverseConstIterator ReverseEnd() const
		{
			return Iterator(*this, this->GetNullNode());
		}
		/// @}
	
	public:

		/// @brief Queries for the iterator of the element with specified key.
		/// @param Key Key of the element we are looking for.
		/// @returns Iterator on the element if it was found and End iterator otherwise.
		/// @{
		GDINL ConstIterator QueryIterator(KeyType const& Key) const
		{
			return ConstIterator(*this, this->Query(&Key));
		}
		GDINL Iterator QueryIterator(KeyType const& Key)
		{
			return Iterator(*this, this->Query(&Key));
		}
		/// @}

		/// @brief Inserts specified element into collection at desired index.
		/// @param Key Key of the element that is going to be inserted.
		/// @param Value Value of the element that is going to be inserted.
		/// @{
		GDINL void InsertKeyValue(KeyType&& Key, ValueType&& Value = ValueType())
		{
			RedBlackTreeNodeType* const NewNode = new RedBlackTreeNodeType(new PairType(Forward<KeyType>(Key), Forward<ValueType>(Value)));
			this->Insert(NewNode);
		}
		GDINL void InsertKeyValue(KeyType const& Key, ValueType const& Value)
		{
			RedBlackTreeNodeType* const NewNode = new RedBlackTreeNodeType(new PairType(Key, Value));
			this->Insert(NewNode);
		}
		/// @}

		/// @brief Removes existing element from array at specified index.
		/// @param Key Key of the element that is going to be removed.
		GDINL void RemoveElementAt(KeyType const& Key)
		{
			Iterator QueriedIterator = this->QueryIterator(Key);
			GD_ASSERT(QueriedIterator != this->End(), "Element with specified key does not exist.");
			
			RedBlackTreeNodeType const* QueriedNode = QueriedIterator.GetNode();
			this->Delete(QueriedNode);
			delete QueriedNode->GetComparand();
			delete QueriedNode;
		}

	public:

		/// @brief Moves other map here.
		/// @param OtherMap Other map that would be moved into current object.
		/// @returns Self.
		GDINL Map& operator= (Map&& OtherMap)
		{
			RedBlackTree::operator= (Forward<Map>(OtherMap));
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

GD_NAMESPACE_END

#endif	// ifndef GD_CORE_CONTAINERS_MAP
