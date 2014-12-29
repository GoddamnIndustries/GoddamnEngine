/// ==========================================================================================
/// RedBlackTree.h - Red Black tree data structure interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_CONTAINERS_MAP_RED_BLACK_TREE
#define GD_CORE_CONTAINERS_MAP_RED_BLACK_TREE

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/Containers.h>
#include <GoddamnEngine/Core/Containers/Pair/Pair.h>

GD_NAMESPACE_BEGIN

	template<typename KeyType, typename ValueType>
	class RedBlackTree
	{
	public:
		typedef KeyType ThisKeyType;
		typedef ValueType ThisValueType;
		typedef Pair<KeyType, ValueType> ThisPairType;
		typedef typename ThisPairType::KeyTypeConstRef KeyTypeConstRef;
		typedef typename ThisPairType::ValueTypeConstRef ValueTypeConstRef;
		typedef RedBlackTree<ThisKeyType, ThisValueType> ThisRedBlackTreeType;

	private:

		/// Defines color of the Red-Black tree node.
		enum class NodeColor : UInt8
		{
			Red,
			Black
		};	// enum class NodeColor

		/// Defines a single node of the Red-black tree.
		struct Node 
		{
			Node* Left = nullptr;
			Node* Right = nullptr;
			Node* Parent = nullptr;
			NodeColor Color;
			ThisPairType* Data = nullptr;
		};	// struct Node

		template<typename TreeType, typename TreePairType>
		struct TreeBaseIterator
		{
		private:
			friend class RedBlackTree;
			typedef typename TreePairType::Node TreeTypeNode;
			TreeType& Container;
			TreeTypeNode* IterNode;

		public:
			/// @brief					Initializes a base Red-Black tree iterator for some tree.
			/// @param[in] Container	Tree that is going to be iterated.
			/// @param[in] StartNode	First iteration node.
			GDINL						TreeBaseIterator(TreeType& Container, TreeTypeNode* const StartNode);

			/// @brief					Initializes a copy of some iterator.
			/// @param[in] Other		Tree that is going to be iterated.
			GDINL						TreeBaseIterator(TreeBaseIterator const& Other);

			GDINL TreeBaseIterator&		operator= (TreeBaseIterator const& Other);

			GDINL TreeBaseIterator&		operator++();
			GDINL TreeBaseIterator 		operator++(int const);

			GDINL TreeBaseIterator& 	operator--();
			GDINL TreeBaseIterator  	operator--(int const);

			GDINL bool 					operator== (TreeBaseIterator const& Other) const;
			GDINL bool					operator!= (TreeBaseIterator const& Other) const;

			GDINL TreePairType*			operator-> () const;
			GDINL TreePairType&			operator*  () const;
		};	// struct BaseIterator

		size_t Length = 0;
		Node* RootNode;
		Node* NullNode;

	public:
		typedef TreeBaseIterator<ThisRedBlackTreeType, ThisPairType> Iterator;
		typedef TreeBaseIterator<ThisRedBlackTreeType const, ThisPairType const> ConstIterator;

		/// ------------------------------------------------------------------------------------------
		/// @name Constructors & Destructor.
		/// @{
		/// ------------------------------------------------------------------------------------------

	protected:

		/// @brief						Initializes a new red-black tree.
		inline							RedBlackTree();

		/// @brief						Moves other red-black tree here.
		/// @param[in] Other			Other tree to move here.
		inline							RedBlackTree(RedBlackTree&& Other);

		/// @brief						Initializes a copy of a specified tree.
		/// @param[in] Other			Other tree to copy.
		inline							RedBlackTree(RedBlackTree const& Other);

		/// @brief						Deinitializes a red-black tree and destroys all internal data.
		inline							~RedBlackTree();

		/// @}

		/// ------------------------------------------------------------------------------------------
		/// @name Basic Red-black tree algorithms.
		/// @{
		/// ------------------------------------------------------------------------------------------

	private:

		/// @brief						Creates a new node.
		/// @param[out] NewNode			Reference to new node.
		inline void						CreateNode(Node*& NewNode);

		/// @brief						Copies existing node.
		/// @param[in] TheNode			Node to copy.
		/// @param[in] NullNode			Null node of the tree we are copying from.
		/// @param[out] OutNode			Reference to output of copies node.
		inline void						CopyNode(Node* const TheNode, Node* const NullNode, Node*& OutNode);

		/// @brief						Destroys a specified node and all is children.
		/// @param[in] TheNode			Node that is going to be destroyed.
		inline void						DestroyNode(Node* const TheNode);

		/// @brief						Returns next node to specified one or null.
		/// @param[in] TheNode			Some node.
		/// @returns					Next node to this one or null node if not exists.
		inline Node const*				NextNode(Node const* const TheNode) const;
		inline Node      *				NextNode(Node      * const TheNode);

		/// @brief						Returns previous node to specified one or null.
		/// @param[in] TheNode			Some node.
		/// @returns					Previous node to this one or null node if not exists.
		inline Node const*				PrevNode(Node const* const TheNode) const;
		inline Node      *				PrevNode(Node      * const TheNode);

		/// @brief						Performs a left rotation of the tree.
		/// @param[in] TheNode			The node to rotate on.
		inline void						RotateLeft(Node* const TheNode);

		/// @brief						Performs a right rotation of the tree.
		/// @param[in] TheNode			The node to rotate on.
		inline void						RotateRight(Node* const TheNode);

		/// @brief						Preforms an insertion of the new node with specified key and value.
		/// @param[in] Pair				Initial key/value pair of the array.
		inline void						Insert(ThisPairType* const Pair);

		/// @brief						Fixes the tree and restores it's red-black properties.
		/// @param[in] TheNode			The node to start fixing.
		inline void						Repair(Node* const TheNode);

		/// @brief						Deletes the node from the tree.
		/// @param[in] TheNode			Node that would be deleted.
		inline void						Delete(Node* const TheNode);

		/// @brief						Searches for node with specified key.
		/// @param[in] Key				Key we are looking for.
		inline Node const*				Query(KeyTypeConstRef Key) const;
		inline Node      *				Query(KeyTypeConstRef Key);

		/// @}

		/// ------------------------------------------------------------------------------------------
		/// @name Red-Black tree iteration.
		/// @{
		/// ------------------------------------------------------------------------------------------

		/// @brief						Returns iterator that points to first container element.
		/// @returns 					Iterator that points to first container element.
		GDINL Iterator					Begin();
		GDINL ConstIterator				Begin() const;

		/// @brief  					Returns iterator that points to past the end container element.
		/// @returns 					Iterator that points to past the end container element.
		GDINL Iterator					End();
		GDINL ConstIterator				End() const;

		/// ------------------------------------------------------------------------------------------
		/// @name Dynamic size management.
		/// @{
		/// ------------------------------------------------------------------------------------------

		/// @brief   					Returns number of elements that exist in the container.
		/// @returns					Number of elements that exist in container.
		GDINL size_t					GetLength() const;

		/// @brief   					Returns true if this container is empty.
		/// @returns					True if this container is empty, false otherwise.
		GDINL bool						IsEmpty() const;

		/// @brief   					Destroys all elements in container with memory deallocation.
		inline void						Clear();

		/// @}

		/// ------------------------------------------------------------------------------------------
		/// @name Elements access.
		/// @{
		/// ------------------------------------------------------------------------------------------

		/// @brief						Searches for iterator with a key.
		/// @param[in] Key				Key we are looking for.
		/// @returns					Iterator of the tree we with specified key or End if nothing was found.
		GDINL ConstIterator				FindIteratorWithKey(KeyTypeConstRef Key) const;
		GDINL Iterator					FindIteratorWithKey(KeyTypeConstRef Key);

		/// @brief  					Returns reference on value of the element with specified key.
		/// @param[in] Key				Key we are looking for.
		/// @returns   					Reference on some element in the container.
		inline ValueType const&			GetValueWithKey(KeyTypeConstRef Key) const;
		inline ValueType	  &			GetValueWithKey(KeyTypeConstRef Key);

		/// @brief						Assigns new element into place in container with specified index.
		/// @param[in] Key				Key we are looking for.
		/// @param[in] Value			Value that would be assigned.
		inline void						SetValueWithKey(KeyTypeConstRef Key, ValueType      && Value);
		inline void						SetValueWithKey(KeyTypeConstRef Key, ValueTypeConstRef Value);

		/// @brief						Inserts specified element into collection at desired index.
		/// @param[in] Key				Key of the element that is going to be inserted.
		/// @param[in] Value			Value of the element that is going to be inserted.
		inline void						InsertKeyValue(KeyType      && Key, ValueType      && Value);
		inline void						InsertKeyValue(KeyTypeConstRef Key, ValueTypeConstRef Value);

		/// @brief						Removes existing element from array at specified index.
		/// @param[in] Key				Key of the element that is going to be removed.
		inline void						RemoveElementAt(KeyTypeConstRef Key);

		/// @}

		/// ------------------------------------------------------------------------------------------
		/// @name Operators.
		/// @{
		/// ------------------------------------------------------------------------------------------

		inline RedBlackTree&			operator= (RedBlackTree     && OtherVector);
		inline RedBlackTree& 			operator= (RedBlackTree const& OtherVector);

		GDINL ValueType const&			operator[] (KeyTypeConstRef Key) const;
		GDINL ValueType      & 			operator[] (KeyTypeConstRef Key);

		/// @}

	private:
		GDINL friend Iterator			begin(RedBlackTree      & some_red_black_tree) { return some_red_black_tree.Begin(); }
		GDINL friend ConstIterator		begin(RedBlackTree const& some_red_black_tree) { return some_red_black_tree.Begin(); }
		GDINL friend Iterator			end  (RedBlackTree      & some_red_black_tree) { return some_red_black_tree.End(); }
		GDINL friend ConstIterator		end  (RedBlackTree const& some_red_black_tree) { return some_red_black_tree.End(); }
	};	// class RedBlackTree

GD_NAMESPACE_END

#include <GoddamnEngine/Core/Containers/Map/RedBlackTree/RedBlackTree.inl>

#endif	// ifndef GD_CORE_CONTAINERS_MAP_RED_BLACK_TREE
