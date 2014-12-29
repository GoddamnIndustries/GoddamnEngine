/// ==========================================================================================
/// RedBlackTree.cpp - Red Black tree data structure inline implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// ==========================================================================================

#if (!defined(GD_CORE_CONTAINERS_MAP_RED_BLACK_TREE))
#	error Attempting to include 'RedBlackTree.inl' file. Please, use 'RedBlackTree.h' instead.
#endif	// if (!defined(GD_CORE_CONTAINERS_MAP_RED_BLACK_TREE))

GD_NAMESPACE_BEGIN

	/// ------------------------------------------------------------------------------------------
	/// Tree iteration.
	/// ------------------------------------------------------------------------------------------

	GDINL RedBlackTreeNode const* RedBlackTree::GetNullNode()
	{
		return RedBlackTree::NullNode;
	}

	GDINL RedBlackTreeNode const* RedBlackTree::GetFirstNode() const
	{
		return this->RootNode->Left;
	}

	GDINL RedBlackTreeNode*	RedBlackTree::GetFirstNode()
	{
		return const_cast<RedBlackTreeNode*>(const_cast<RedBlackTree const*>(this)->GetFirstNode());
	}

	GDINL RedBlackTreeNode*	RedBlackTree::GetLastNode()
	{
		return const_cast<RedBlackTreeNode*>(const_cast<RedBlackTree const*>(this)->GetLastNode());
	}

	GDINL RedBlackTreeNode* RedBlackTree::GetNextNode(RedBlackTreeNode* const TheNode)
	{
		return const_cast<RedBlackTreeNode*>(const_cast<RedBlackTree const*>(this)->GetNextNode(TheNode));
	}

	GDINL RedBlackTreeNode* RedBlackTree::GetPrevNode(RedBlackTreeNode* const TheNode)
	{
		return const_cast<RedBlackTreeNode*>(const_cast<RedBlackTree const*>(this)->GetPrevNode(TheNode));
	}

	GDINL size_t RedBlackTree::GetLength() const
	{
		return this->Length;
	}

	GDINL bool RedBlackTree::IsEmpty() const
	{
		return (this->Length == 0);
	}

	/// ------------------------------------------------------------------------------------------
	/// Tree modification.
	/// ------------------------------------------------------------------------------------------

	GDINL RedBlackTreeNode* RedBlackTree::Query(handle const Comparand)
	{
		return const_cast<RedBlackTreeNode*>(const_cast<RedBlackTree const*>(this)->Query(Comparand));
	}

GD_NAMESPACE_END
