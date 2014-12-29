/// ==========================================================================================
/// RedBlackTree.h - Red Black tree data structure interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_CONTAINERS_MAP_RED_BLACK_TREE
#define GD_CORE_CONTAINERS_MAP_RED_BLACK_TREE

#include <GoddamnEngine/Include.h>

GD_NAMESPACE_BEGIN

	/// Defines color of the Red-Black Tree node.
	enum class RedBlackTreeNodeColor : UInt8
	{
		Red,
		Black
	};	// enum class RedBlackTreeNodeColor

	/// Defines a single node of the Red-Black Tree.
	struct RedBlackTreeNode
	{
	private:
		friend class RedBlackTree;
		RedBlackTreeNode* Left = nullptr;
		RedBlackTreeNode* Right = nullptr;
		RedBlackTreeNode* Parent = nullptr;
		RedBlackTreeNodeColor Color = RedBlackTreeNodeColor::Red;
	};	// struct RedBlackTreeNode

	/// Red-Black Tree data structure. Contains fundamental tree management algorithms.
	/// @see http://en.wikipedia.org/wiki/Red–black_tree
	/// @see http://web.mit.edu/~emin/www.old/source_code/red_black_tree/index.html
	class RedBlackTree
	{
	private:
		RedBlackTreeNode static* NullNode;
		RedBlackTreeNode* RootNode = nullptr;
		size_t Length = 0;

	protected:

		/// @brief Initializes a new Red-Black Tree.
		GDAPI RedBlackTree();

		/// @brief Moves other Red-Black Tree here.
		/// @param[in] Other Other tree to move here.
		GDAPI RedBlackTree(RedBlackTree&& Other);

		/// @brief Deinitializes a Red-Black Tree and destroys all internal data.
		GDAPI ~RedBlackTree();

		/// @brief Extracts a comparand object from the specified node.
		/// @param[in] TheNode The node from which comparand would be extracted.
		/// @returns Comparand object of the node.
		GDAPI virtual chandle ExtractNodeComparand(RedBlackTreeNode const* const TheNode) const abstract;

		/// @brief Compares comparands of the nodes. 
		/// @param[in] First First comparand.
		/// @param[in] Second Second comparand.
		/// @returns Zero, if specified comparands are equal, positive value if first is greater, negative otherwise.
		GDAPI virtual int CompareComparands(chandle const First, chandle const Second) const abstract;

	protected:

		/// @brief Returns a node that represents a NULL sentinel of this tree.
		/// @returns A node that represents a NULL sentinel of this tree.
		GDINL RedBlackTreeNode static const* GetNullNode();

		/// @brief Returns the first node of the tree or null.
		/// @returns The first node of the tree or null if no first node exists.
		GDINL RedBlackTreeNode const* GetFirstNode() const;
		GDINL RedBlackTreeNode* GetFirstNode();

		/// @brief Returns the first node of the tree or null.
		/// @returns The first node of the tree or null if no first node exists.
		GDAPI RedBlackTreeNode const* GetLastNode() const;
		GDINL RedBlackTreeNode* GetLastNode();

		/// @brief Returns next node to specified one or null.
		/// @param[in] TheNode Some node.
		/// @returns Next node to this one or null node if not exists.
		GDAPI RedBlackTreeNode const* GetNextNode(RedBlackTreeNode const* const TheNode) const;
		GDINL RedBlackTreeNode* GetNextNode(RedBlackTreeNode* const TheNode);

		/// @brief Returns previous node to specified one or null.
		/// @param[in] TheNode Some node.
		/// @returns Previous node to this one or null node if not exists.
		GDAPI RedBlackTreeNode const* GetPrevNode(RedBlackTreeNode const* const TheNode) const;
		GDINL RedBlackTreeNode* GetPrevNode(RedBlackTreeNode* const TheNode);

	public:

		/// @brief Returns number of nodes that exist in the tree.
		/// @returns Number of nodes that exist in tree.
		GDINL size_t GetLength() const;

		/// @brief Returns true if this tree is empty.
		/// @returns True if this tree is empty, false otherwise.
		GDINL bool IsEmpty() const;

	private:

		/// @brief Internally creates a new node.
		/// @param[out] NewNode Reference to new node.
		GDAPI void InternalCreateNode(RedBlackTreeNode*& NewNode);

		/// @brief Internally destroys a specified node and all is children.
		/// @param[in] TheNode Node that is going to be destroyed.
		GDAPI void InternalDestroyNode(RedBlackTreeNode* const TheNode);

		/// @brief Performs a left rotation of the tree.
		/// @param[in] TheNode The node to rotate on.
		GDAPI void RotateLeft(RedBlackTreeNode* const TheNode);

		/// @brief Performs a right rotation of the tree.
		/// @param[in] TheNode The node to rotate on.
		GDAPI void RotateRight(RedBlackTreeNode* const TheNode);

		/// @brief Fixes the tree and restores it's red-black properties.
		/// @param[in] TheNode The node to start fixing.
		GDAPI void Repair(RedBlackTreeNode* const TheNode);

	protected:

		/// @brief Preforms an insertion of the new node with specified key and value.
		/// @param[in] NewNode The node that would be inserted. Should be already created.
		GDAPI void Insert(RedBlackTreeNode* const NewNode);

		/// @brief Deletes the node from the tree.
		/// @param[in] TheNode Node that would be deleted. Should be manually deleted.
		GDAPI void Delete(RedBlackTreeNode* const TheNode);

		/// @brief Searches for node with specified key.
		/// @param[in] Key Key we are looking for.
		GDAPI RedBlackTreeNode const* Query(handle const Comparand) const;
		GDINL RedBlackTreeNode* Query(handle const Comparand);

		/// @brief Destroys all elements in container with memory deallocation.
		GDAPI void Clear();

	};	// class RedBlackTree

GD_NAMESPACE_END

#include <GoddamnEngine/Core/Containers/Map/RedBlackTree/RedBlackTree.inl>

#endif	// ifndef GD_CORE_CONTAINERS_MAP_RED_BLACK_TREE
