/// ==========================================================================================
/// RedBlackTree.h - Red Black tree data structure interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_CONTAINERS_RED_BLACK_TREE
#define GD_CORE_CONTAINERS_RED_BLACK_TREE

#include <GoddamnEngine/Include.h>

GD_NAMESPACE_BEGIN

	/// @brief Defines color of the Red-Black Tree node.
	enum class RedBlackTreeNodeColor : UInt8
	{
		Red,
		Black
	};	// enum class RedBlackTreeNodeColor

	/// @brief Defines a single node of the Red-Black Tree.
	class RedBlackTreeBaseNode
	{
	private:
		GD_CLASS_UNASSIGNABLE(RedBlackTreeBaseNode);
		GD_CLASS_UNCOPIABLE(RedBlackTreeBaseNode);

		template<typename ComparandType>
		friend class RedBlackTreeNode;
		friend class RedBlackTree;

		chandle const Comparand;
		RedBlackTreeBaseNode* Left = nullptr;
		RedBlackTreeBaseNode* Right = nullptr;
		RedBlackTreeBaseNode* Parent = nullptr;
		RedBlackTreeNodeColor Color = RedBlackTreeNodeColor::Red;

		GDINL RedBlackTreeBaseNode(chandle const Comparand)
			: Comparand(Comparand)
		{
		}
	};	// class RedBlackTreeNode

	/// @brief Red-Black Tree data structure. Contains fundamental tree management algorithms.
	/// @see http://en.wikipedia.org/wiki/Red–black_tree
	/// @see http://web.mit.edu/~emin/www.old/source_code/red_black_tree/index.html
	class RedBlackTree
	{
	private:
		GD_CLASS_UNCOPIABLE(RedBlackTree);

		RedBlackTreeBaseNode static* NullNode;
		RedBlackTreeBaseNode* RootNode = nullptr;
		size_t Length = 0;

	protected:

		/// @brief Initializes a new Red-Black Tree.
		GDAPI RedBlackTree();

		/// @brief Moves other Red-Black Tree here.
		/// @param Other Other tree to move here.
		GDAPI RedBlackTree(RedBlackTree&& Other);

		/// @brief Deinitializes a Red-Black Tree and destroys all internal data.
		GDAPI ~RedBlackTree();

		/// @brief Compares comparands of the nodes. 
		/// @param First First comparand.
		/// @param Second Second comparand.
		/// @returns Zero, if specified comparands are equal, positive value if first is greater, negative otherwise.
		GDAPI virtual int CompareComparands(chandle const First, chandle const Second) const abstract;

	protected:

		/// @brief Returns a node that represents a NULL sentinel of this tree.
		/// @returns A node that represents a NULL sentinel of this tree.
		GDINL RedBlackTreeBaseNode const* GetNullNode() const
		{
			return RedBlackTree::NullNode;
		}

		/// @brief Returns the first node of the tree or null.
		/// @returns The first node of the tree or null if no first node exists.
		/// @{
		GDINL RedBlackTreeBaseNode const* GetFirstNode() const
		{
			return this->RootNode->Left;
		}
		GDINL RedBlackTreeBaseNode* GetFirstNode()
		{
			return this->RootNode->Left;
		}
		/// @}

		/// @brief Returns the first node of the tree or null.
		/// @returns The first node of the tree or null if no first node exists.
		/// @{
		GDAPI RedBlackTreeBaseNode const* GetLastNode() const;
		GDINL RedBlackTreeBaseNode* GetLastNode()
		{
			return const_cast<RedBlackTreeBaseNode*>(const_cast<RedBlackTree const*>(this)->GetLastNode());
		}
		/// @}

		/// @brief Returns next node to specified one or null.
		/// @param TheNode Some node.
		/// @returns Next node to this one or null node if not exists.
		/// @{
		GDAPI RedBlackTreeBaseNode const* GetNextNode(RedBlackTreeBaseNode const* const TheNode) const;
		GDINL RedBlackTreeBaseNode* GetNextNode(RedBlackTreeBaseNode* const TheNode)
		{
			return const_cast<RedBlackTreeBaseNode*>(const_cast<RedBlackTree const*>(this)->GetNextNode(TheNode));
		}
		/// @}

		/// @brief Returns previous node to specified one or null.
		/// @param TheNode Some node.
		/// @returns Previous node to this one or null node if not exists.
		/// @{
		GDAPI RedBlackTreeBaseNode const* GetPrevNode(RedBlackTreeBaseNode const* const TheNode) const;
		GDINL RedBlackTreeBaseNode* GetPrevNode(RedBlackTreeBaseNode* const TheNode)
		{
			return const_cast<RedBlackTreeBaseNode*>(const_cast<RedBlackTree const*>(this)->GetPrevNode(TheNode));
		}
		/// @}

	public:

		/// @brief Returns number of nodes that exist in the tree.
		/// @returns Number of nodes that exist in tree.
		GDINL size_t GetLength() const
		{
			return this->Length;
		}

		/// @brief Returns true if this tree is empty.
		/// @returns True if this tree is empty, false otherwise.
		GDINL bool IsEmpty() const
		{
			return this->Length == 0;
		}

	private:

		/// @brief Internally creates a new node.
		/// @param NewNode Reference to new node.
		GDAPI void InternalCreateNode(RedBlackTreeBaseNode*& NewNode);

		/// @brief Internally destroys a specified node and all is children.
		/// @param TheNode Node that is going to be destroyed.
		GDAPI void InternalDestroyNode(RedBlackTreeBaseNode* const TheNode);

		/// @brief Performs a left rotation of the tree.
		/// @param TheNode The node to rotate on.
		GDAPI void RotateLeft(RedBlackTreeBaseNode* const TheNode);

		/// @brief Performs a right rotation of the tree.
		/// @param TheNode The node to rotate on.
		GDAPI void RotateRight(RedBlackTreeBaseNode* const TheNode);

		/// @brief Fixes the tree and restores it's red-black properties.
		/// @param TheNode The node to start fixing.
		GDAPI void Repair(RedBlackTreeBaseNode* const TheNode);

	protected:

		/// @brief Preforms an insertion of the new node with specified key and value.
		/// @param NewNode The node that would be inserted. Should be already created.
		GDAPI void Insert(RedBlackTreeBaseNode* const NewNode);

		/// @brief Deletes the node from the tree.
		/// @param TheNode Node that would be deleted. Should be manually deleted.
		GDAPI void Delete(RedBlackTreeBaseNode* const TheNode);

		/// @brief Searches for node with specified key.
		/// @param Key Key we are looking for.
		/// @{
		GDAPI RedBlackTreeBaseNode const* Query(handle const Comparand) const;
		GDINL RedBlackTreeBaseNode* Query(handle const Comparand)
		{
			return const_cast<RedBlackTreeBaseNode*>(const_cast<RedBlackTree const*>(this)->Query(Comparand));
		}
		/// @}

		/// @brief Destroys all elements in container with memory deallocation.
		GDAPI void Clear();

	public:

		/// @brief Moves other Red-Black Tree here.
		/// @param OtherTree Other Red-Black Tree that would be moved into current object.
		/// @returns Self.
		GDAPI RedBlackTree& operator= (RedBlackTree&& OtherTree);
	};	// class RedBlackTree

	/// @brief Defines a single node of the Red-Black Tree.
	/// @tparam ComparandType Type of objected been stored in the node.
	template<typename ComparandType>
	class RedBlackTreeNode final : public RedBlackTreeBaseNode
	{
	public:

		/// @brief Initializes a tree node.
		/// @param Comparand The comparand data pointer.
		GDINL RedBlackTreeNode(ComparandType const* const Comparand)
			: RedBlackTreeBaseNode(Comparand)
		{
		}

		/// @brief Returns comparand data pointer.
		/// @returns Comparand data pointer.
		GDINL ComparandType const* GetComparand() const
		{
			return reinterpret_cast<ComparandType const*>(this->Comparand);
		}
	};	// struct RedBlackTreeNode

	/// @brief Iterator for Red-Black Tree containers.
	/// @tparam RedBlackTreeContainerType Type of container.
	/// @tparam RedBlackTreeNodeType Type of container element type.
	template<typename RedBlackTreeContainerType, typename RedBlackTreeNodeType>
	struct RedBlackTreeIterator final
	{
	private:
		typedef typename TypeTraits::RemovePointer<decltype(DeclValue<RedBlackTreeNodeType>().GetComparand())>::Type RedBlackTreeComparandType;
		RedBlackTreeContainerType& Container;
		RedBlackTreeNodeType const* IterNode = nullptr;

	public:

		/// @brief Initializes iterator that points on the first element of the specified container.
		/// @param Container Container that is going to be iterated.
		/// @param IterNode Start node of the iterator.
		GDINL explicit RedBlackTreeIterator(RedBlackTreeContainerType& Container, RedBlackTreeNodeType const* const IterNode)
			: Container(Container), IterNode(IterNode)
		{
		}

		/// @brief Returns node associated with this iterator.
		/// @return	Node associated with this iterator.
		GDINL RedBlackTreeNodeType const* GetNode()
		{
			return this->IterNode;
		}

		/// @brief Assigns the iterator next element of the indexed container.
		/// @returns Incremented iterator.
		GDINL RedBlackTreeIterator& operator++ ()
		{
			this->IterNode = this->Container->GetNextNode(this->IterNode);
			return *this;
		}

		/// @brief Assigns the iterator next element of the indexed container.
		/// @param Unused Unused parameter passed be compiler.
		/// @returns Iterator before incrementing.
		GDINL RedBlackTreeIterator operator++ (int const Unused)
		{
			GD_NOT_USED(Unused);
			RedBlackTreeIterator Copy(*this);
			this->IterNode = this->Container->GetNextNode(this->IterNode);
			return Copy;
		}

		/// @brief Assigns the iterator previous element of the indexed container.
		/// @returns Decremented iterator.
		GDINL RedBlackTreeIterator& operator-- ()
		{
			this->IterNode = this->Container->GetPrevNode(this->IterNode);
			return *this;
		}

		/// @brief Assigns the iterator previous element of the indexed container.
		/// @param Unused Unused parameter passed be compiler.
		/// @returns Iterator before decrementing.
		GDINL RedBlackTreeIterator operator-- (int const)
		{
			GD_NOT_USED(Unused);
			RedBlackTreeIterator Copy(*this);
			this->IterNode = this->Container->GetPrevNode(this->IterNode);
			return Copy;
		}

		/// @brief Compares two iterators on equality.
		/// @param Other Other iterator that would be compared.
		/// @returns True if iterators point to the same container at same indices, false otherwise.
		GDINL bool operator== (RedBlackTreeIterator const& Other) const
		{
			return (&this->Container == &Other.Container) && (this->IterNode == Other.IterNode);
		}

		/// @brief Compares two iterators on inequality.
		/// @param Other Other iterator that would be compared.
		/// @returns False if iterators point to the same container at same indices, true otherwise.
		GDINL bool operator!= (RedBlackTreeIterator const& Other) const
		{
			return (&this->Container != &Other.Container) || (this->IterNode != Other.IterNode);
		}

		/// @brief Dereferences value of the iterator.
		/// @returns Reference to the element of the container at iterator index.
		GDINL RedBlackTreeComparandType& operator* () const
		{
			return *this->IterNode->GetComparand();
		}

		/// @brief Dereferences value of the iterator.
		/// @returns Pointer to the element of the container at iterator index.
		GDINL RedBlackTreeComparandType* operator-> () const
		{
			return this->IterNode->GetComparand();
		}
	};	// struct RedBlackTreeIterator

GD_NAMESPACE_END

#endif	// ifndef GD_CORE_CONTAINERS_RED_BLACK_TREE
