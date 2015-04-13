// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Containers/RedBlackTree/RedBlackTree.h
//! Red Black tree data structure interface.
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/Containers.h>
#include <GoddamnEngine/Core/Templates/Utility.h>

GD_NAMESPACE_BEGIN

	//! @brief Defines a single node of the Red-Black Tree.
	class RedBlackTreeBaseNode : IUncopiable
	{
	private:
		friend class RedBlackTreeBase;

		Handle const Element;
		RedBlackTreeBaseNode* Left = nullptr;
		RedBlackTreeBaseNode* Right = nullptr;
		RedBlackTreeBaseNode* Parent = nullptr;
		bool IsRed;

	public:
		//! @brief Initializes a tree node.
		//! @param Element The Element data pointer.
		GDINL RedBlackTreeBaseNode(Handle const Element)
			: Element(Element)
		{
		}

		//! @brief Returns Element data pointer.
		//! @returns Element data pointer.
		//! @{
		GDINL CHandle GetElement() const
		{
			return this->Element;
		}
		GDINL Handle GetElement()
		{
			return const_cast<Handle>(const_cast<RedBlackTreeBaseNode const*>(this)->GetElement());
		}
		//! @}

		//! @brief Returns pointer to left sub-tree of this node.
		//! @returns Pointer to left sub-tree of this node.
		//! @{
		GDINL RedBlackTreeBaseNode const* GetLeft() const
		{
			return this->Left;
		}
		GDINL RedBlackTreeBaseNode* GetLeft()
		{
			return const_cast<RedBlackTreeBaseNode*>(const_cast<RedBlackTreeBaseNode const*>(this)->GetLeft());
		}
		//! @}

		//! @brief Returns pointer to right sub-tree of this node.
		//! @returns Pointer to right sub-tree of this node.
		//! @{
		GDINL RedBlackTreeBaseNode const* GetRight() const
		{
			return this->Left;
		}
		GDINL RedBlackTreeBaseNode* GetRight()
		{
			return const_cast<RedBlackTreeBaseNode*>(const_cast<RedBlackTreeBaseNode const*>(this)->GetRight());
		}
		//! @}

	};	// class RedBlackTreeNode

	//! @brief Basic Red-Black Tree data structure. Contains fundamental tree management algorithms.
	//! @see http://en.wikipedia.org/wiki/Red–black_tree
	//! @see http://web.mit.edu/~emin/www.old/source_code/red_black_tree/index.html
	class RedBlackTreeBase : IUncopiable
	{
	private:
		RedBlackTreeBaseNode* NullNode = nullptr;
		RedBlackTreeBaseNode* RootNode = nullptr;
		SizeTp Length = 0;

	public:
		//! @brief Initializes a new Red-Black Tree.
		GDAPI RedBlackTreeBase();

		//! @brief Moves other Red-Black Tree here.
		//! @param Other Other tree to move here.
		GDAPI RedBlackTreeBase(RedBlackTreeBase&& Other);

		//! @brief Deinitializes a Red-Black Tree and destroys all internal data.
		GDAPI virtual ~RedBlackTreeBase();

	protected:

		//! @brief Compares Elements of the nodes. 
		//! @param First First Element.
		//! @param Second Second Element.
		//! @returns Zero, if specified Elements are equal, positive value if first is greater, negative otherwise.
		GDAPI virtual int CompareElements(CHandle const First, CHandle const Second) const abstract;

	public:

		//! @brief Returns a node that represents a NULL sentinel of this tree.
		//! @returns A node that represents a NULL sentinel of this tree.
		//! @{
		GDINL RedBlackTreeBaseNode const* GetNullNode() const
		{
			return this->NullNode;
		}
		GDINL RedBlackTreeBaseNode* GetNullNode()
		{
			return const_cast<RedBlackTreeBaseNode*>(const_cast<RedBlackTreeBase const*>(this)->GetNullNode());
		}
		//! @}

		//! @brief Returns the first node of the tree or null.
		//! @returns The first node of the tree or null if no first node exists.
		//! @{
		GDINL RedBlackTreeBaseNode const* GetFirstNode() const
		{
			return this->RootNode->Left;
		}
		GDINL RedBlackTreeBaseNode* GetFirstNode()
		{
			return const_cast<RedBlackTreeBaseNode*>(const_cast<RedBlackTreeBase const*>(this)->GetFirstNode());
		}
		//! @}

		//! @brief Returns the first node of the tree or null.
		//! @returns The first node of the tree or null if no first node exists.
		//! @{
		GDAPI RedBlackTreeBaseNode const* GetLastNode() const;
		GDINL RedBlackTreeBaseNode* GetLastNode()
		{
			return const_cast<RedBlackTreeBaseNode*>(const_cast<RedBlackTreeBase const*>(this)->GetLastNode());
		}
		//! @}

		//! @brief Returns next node to specified one or null.
		//! @param Node Some node.
		//! @returns Next node to this one or null node if not exists.
		//! @{
		GDAPI RedBlackTreeBaseNode const* GetNextNode(RedBlackTreeBaseNode const* const Node) const;
		GDINL RedBlackTreeBaseNode* GetNextNode(RedBlackTreeBaseNode* const Node)
		{
			return const_cast<RedBlackTreeBaseNode*>(const_cast<RedBlackTreeBase const*>(this)->GetNextNode(Node));
		}
		//! @}

		//! @brief Returns previous node to specified one or null.
		//! @param Node Some node.
		//! @returns Previous node to this one or null node if not exists.
		//! @{
		GDAPI RedBlackTreeBaseNode const* GetPrevNode(RedBlackTreeBaseNode const* const Node) const;
		GDINL RedBlackTreeBaseNode* GetPrevNode(RedBlackTreeBaseNode* const Node)
		{
			return const_cast<RedBlackTreeBaseNode*>(const_cast<RedBlackTreeBase const*>(this)->GetPrevNode(Node));
		}
		//! @}

	public:

		//! @brief Returns number of nodes that exist in the tree.
		//! @returns Number of nodes that exist in tree.
		GDINL SizeTp GetLength() const
		{
			return this->Length;
		}

		//! @brief Returns true if this tree is empty.
		//! @returns True if this tree is empty, false otherwise.
		GDINL bool IsEmpty() const
		{
			return this->Length == 0;
		}

	private:

		//! @brief Internally creates a new node.
		//! @param NewNode Reference to new node.
		GDAPI void InternalCreateNode(RedBlackTreeBaseNode*& NewNode);

#if 0
		//! @brief Internally destroys a specified node and all is children.
		//! @param Node Node that is going to be destroyed.
		GDAPI void InternalDestroyNode(RedBlackTreeBaseNode* const Node);
#endif	// if 0

		//! @brief Performs a left rotation of the tree.
		//! @param Node The node to rotate on.
		GDAPI void RotateLeft(RedBlackTreeBaseNode* const Node);

		//! @brief Performs a right rotation of the tree.
		//! @param Node The node to rotate on.
		GDAPI void RotateRight(RedBlackTreeBaseNode* const Node);

		//! @brief Fixes the tree and restores it's red-black properties.
		//! @param Node The node to start fixing.
		GDAPI void Repair(RedBlackTreeBaseNode* const Node);

	public:

		//! @brief Preforms an insertion of the new node with specified key and value.
		//! @param NewNode The node that would be inserted. Should be already created.
		GDAPI void Insert(RedBlackTreeBaseNode* const NewNode);

		//! @brief Deletes the node from the tree.
		//! @param Node Node that would be deleted. Should be manually deleted.
		GDAPI void Delete(RedBlackTreeBaseNode* const Node);

		//! @brief Searches for node with specified key.
		//! @param Key Key we are looking for.
		//! @{
		GDAPI RedBlackTreeBaseNode const* Query(CHandle const Element) const;
		GDINL RedBlackTreeBaseNode* Query(CHandle const Element)
		{
			return const_cast<RedBlackTreeBaseNode*>(const_cast<RedBlackTreeBase const*>(this)->Query(Element));
		}
		//! @}

		//! @brief Removes all elements from the tree. All nodes should be manully deleted.
		GDAPI void Clear();

	public:

		//! @brief Moves other Red-Black Tree here.
		//! @param OtherTree Other Red-Black Tree that would be moved into current object.
		//! @returns this.
		GDAPI RedBlackTreeBase& operator= (RedBlackTreeBase&& OtherTree);
	};	// class RedBlackTree

	//! @brief Defines a single node of the Red-Black Tree.
	//! @tparam ElementType Type of objected been stored in the node.
	template<typename ElementType>
	class RedBlackTreeNode final : private RedBlackTreeBaseNode
	{
		template<typename SomeElementType>
		friend class RedBlackTree;

	public:

		//! @brief Initializes a tree node.
		//! @param Element The Element data pointer.
		GDINL RedBlackTreeNode(ElementType* const Element)
			: RedBlackTreeBaseNode(Element)
		{
		}

		//! @brief Returns Element data pointer.
		//! @returns Element data pointer.
		//! @{
		GDINL ElementType const* GetElement() const
		{
			return reinterpret_cast<ElementType const*>(this->RedBlackTreeBaseNode::GetElement());
		}
		GDINL ElementType* GetElement()
		{
			return const_cast<ElementType*>(const_cast<RedBlackTreeNode const*>(this)->GetElement());
		}
		//! @}

		//! @brief Returns pointer to left sub-tree of this node.
		//! @returns Pointer to left sub-tree of this node.
		//! @{
		GDINL RedBlackTreeNode const* GetLeft() const
		{
			return reinterpret_cast<RedBlackTreeNode const*>(this->RedBlackTreeBaseNode::GetLeft());
		}
		GDINL RedBlackTreeNode* GetLeft()
		{
			return const_cast<RedBlackTreeNode*>(const_cast<RedBlackTreeNode const*>(this)->GetLeft());
		}
		//! @}

		//! @brief Returns pointer to right sub-tree of this node.
		//! @returns Pointer to right sub-tree of this node.
		//! @{
		GDINL RedBlackTreeNode const* GetRight() const
		{
			return reinterpret_cast<RedBlackTreeNode const*>(this->RedBlackTreeBaseNode::GetRight());
		}
		GDINL RedBlackTreeNode* GetRight()
		{
			return const_cast<RedBlackTreeNode*>(const_cast<RedBlackTreeNode const*>(this)->GetRight());
		}
		//! @}

	};	// struct RedBlackTreeNode

	//! @brief Iterator for Red-Black Tree containers.
	//! @tparam RedBlackTreeContainerType Type of container.
	//! @tparam RedBlackTreeNodeType Type of container element type.
	template<typename RedBlackTreeContainerType, typename RedBlackTreeNodeType>
	struct RedBlackTreeIterator final
	{
	private:
		RedBlackTreeContainerType& Container;
		RedBlackTreeNodeType* IterNode = nullptr;

	public:

		//! @brief Initializes iterator that points on the specified node of the tree.
		//! @param Container Container that is going to be iterated.
		//! @param IterNode Start node of the iterator.
		GDINL explicit RedBlackTreeIterator(RedBlackTreeContainerType& Container, RedBlackTreeNodeType* const IterNode)
			: Container(Container), IterNode(IterNode)
		{
		}

		//! @brief Returns node associated with this iterator.
		//! @return	Node associated with this iterator.
		GDINL RedBlackTreeNodeType const* GetNode()
		{
			return this->IterNode;
		}

		//! @brief Assigns this iterator other one.
		//! @param Other Other iterator to assign.
		//! @returns this.
		GDINL RedBlackTreeIterator& operator= (RedBlackTreeIterator const& Other)
		{
			GD_ASSERT(&this->Container == &Other.Container, "Iterators have different base containers.");
			this->IterNode = Other.IterNode;
			return *this;
		}

		//! @brief Assigns the iterator next node of the tree.
		//! @returns Incremented iterator.
		GDINL RedBlackTreeIterator& operator++ ()
		{
			this->IterNode = this->Container->GetNextNode(this->IterNode);
			return *this;
		}

		//! @brief Assigns the iterator next node of the tree.
		//! @param Unused Unused parameter passed be compiler.
		//! @returns Iterator before incrementing.
		GDINL RedBlackTreeIterator operator++ (int const Unused)
		{
			GD_NOT_USED(Unused);
			RedBlackTreeIterator Copy(*this);
			this->IterNode = this->Container->GetNextNode(this->IterNode);
			return Copy;
		}

		//! @brief Assigns the iterator previous node of the tree.
		//! @returns Decremented iterator.
		GDINL RedBlackTreeIterator& operator-- ()
		{
			this->IterNode = this->Container->GetPrevNode(this->IterNode);
			return *this;
		}

		//! @brief Assigns the iterator previous node of the tree.
		//! @param Unused Unused parameter passed be compiler.
		//! @returns Iterator before decrementing.
		GDINL RedBlackTreeIterator operator-- (int const Unused)
		{
			GD_NOT_USED(Unused);
			RedBlackTreeIterator Copy(*this);
			this->IterNode = this->Container->GetPrevNode(this->IterNode);
			return Copy;
		}

		//! @brief Compares two iterators on equality.
		//! @param Other Other iterator that would be compared.
		//! @returns True if iterators point to the same container at same indices, false otherwise.
		GDINL bool operator== (RedBlackTreeIterator const& Other) const
		{
			return (&this->Container == &Other.Container) && (this->IterNode == Other.IterNode);
		}

		//! @brief Compares two iterators on inequality.
		//! @param Other Other iterator that would be compared.
		//! @returns False if iterators point to the same container at same indices, true otherwise.
		GDINL bool operator!= (RedBlackTreeIterator const& Other) const
		{
			return (&this->Container != &Other.Container) || (this->IterNode != Other.IterNode);
		}

		//! @brief Dereferences value of the iterator.
		//! @returns Reference to the element of the node of the tree.
		GDINL decltype(*DeclValue<RedBlackTreeNodeType>().GetElement()) operator* () const
		{
			return *this->IterNode->GetElement();
		}

		//! @brief Dereferences value of the iterator.
		//! @returns Pointer to the element of the node of the tree.
		GDINL decltype(DeclValue<RedBlackTreeNodeType>().GetElement()) operator-> () const
		{
			return this->IterNode->GetElement();
		}
	};	// struct RedBlackTreeIterator

	//! @brief Templatized Red-Black Tree data structure. All management functions overloaded to reloaded to make it type-safe.
	//! @tparam ElementType Type of objected been stored in the nodes of the tree.
	template<typename ElementType>
	class RedBlackTree : public IContainer, private RedBlackTreeBase
	{
	protected:
		typedef RedBlackTreeNode<ElementType> RedBlackTreeNodeType;

		//! @brief Initializes a new Red-Black Tree.
		GDINL RedBlackTree()
			: RedBlackTreeBase()
		{
		}

		//! @brief Moves other Red-Black Tree here.
		//! @param Other Other tree to move here.
		GDINL RedBlackTree(RedBlackTree&& Other)
			: RedBlackTreeBase(Forward<RedBlackTreeBase>(Other))
		{
		}

		//! @brief Deinitializes a Red-Black Tree and destroys all internal data.
		GDINL ~RedBlackTree()
		{
		}

	protected:

		//! @brief Returns a node that represents a NULL sentinel of this tree.
		//! @returns A node that represents a NULL sentinel of this tree.
		//! @{
		GDINL RedBlackTreeNodeType const* GetNullNode() const
		{
			return static_cast<RedBlackTreeNodeType const*>(this->RedBlackTreeBase::GetNullNode());
		}
		GDINL RedBlackTreeNodeType* GetNullNode()
		{
			return const_cast<RedBlackTreeNodeType*>(const_cast<RedBlackTree const*>(this)->GetNullNode());
		}
		//! @}

		//! @brief Returns the first node of the tree or null.
		//! @returns The first node of the tree or null if no first node exists.
		//! @{
		GDINL RedBlackTreeNodeType const* GetFirstNode() const
		{
			return static_cast<RedBlackTreeNodeType const*>(this->RedBlackTreeBase::GetFirstNode());
		}
		GDINL RedBlackTreeNodeType* GetFirstNode()
		{
			return const_cast<RedBlackTreeNodeType*>(const_cast<RedBlackTree const*>(this)->GetFirstNode());
		}
		//! @}

		//! @brief Returns the first node of the tree or null.
		//! @returns The first node of the tree or null if no first node exists.
		//! @{
		GDINL RedBlackTreeNodeType const* GetLastNode() const
		{
			return static_cast<RedBlackTreeNodeType const*>(this->RedBlackTreeBase::GetLastNode());
		}
		GDINL RedBlackTreeNodeType* GetLastNode()
		{
			return const_cast<RedBlackTreeNodeType*>(const_cast<RedBlackTree const*>(this)->GetLastNode());
		}
		//! @}

		//! @brief Returns next node to specified one or null.
		//! @param Node Some node.
		//! @returns Next node to this one or null node if not exists.
		//! @{
		GDINL RedBlackTreeNodeType const* GetNextNode(RedBlackTreeNodeType const* const Node) const
		{
			return static_cast<RedBlackTreeNodeType const*>(this->RedBlackTreeBase::GetNextNode());
		}
		GDINL RedBlackTreeNodeType* GetNextNode(RedBlackTreeNodeType* const Node)
		{
			return const_cast<RedBlackTreeNodeType*>(const_cast<RedBlackTree const*>(this)->GetNextNode(Node));
		}
		//! @}

		//! @brief Returns previous node to specified one or null.
		//! @param Node Some node.
		//! @returns Previous node to this one or null node if not exists.
		//! @{
		GDINL RedBlackTreeNodeType const* GetPrevNode(RedBlackTreeNodeType const* const Node) const
		{
			return static_cast<RedBlackTreeNodeType const*>(this->RedBlackTreeBase::GetPrevNode());
		}
		GDINL RedBlackTreeNodeType* GetPrevNode(RedBlackTreeNodeType* const Node)
		{
			return const_cast<RedBlackTreeNodeType*>(const_cast<RedBlackTree const*>(this)->GetPrevNode(Node));
		}
		//! @}

	public:

		//! @brief Returns base Red-Black Tree representation of this object.
		//! @returns Base Red-Black Tree representation of this object.
		//! @{
		GDINL RedBlackTreeBase const* ToBaseTree() const
		{
			return static_cast<RedBlackTreeBase const*>(this);
		}
		GDINL RedBlackTreeBase* ToBaseTree()
		{
			return const_cast<RedBlackTreeBase*>(const_cast<RedBlackTree const*>(this)->ToBaseTree());
		}
		//! @}

		//! @brief Returns number of nodes that exist in the tree.
		//! @returns Number of nodes that exist in tree.
		GDINL SizeTp GetLength() const
		{
			return this->RedBlackTreeBase::GetLength();
		}

		//! @brief Returns true if this tree is empty.
		//! @returns True if this tree is empty, false otherwise.
		GDINL bool IsEmpty() const
		{
			return this->RedBlackTreeBase::IsEmpty();
		}

	protected:

		//! @brief Preforms an insertion of the new node with specified key and value.
		//! @param NewNode The node that would be inserted. Should be already created.
		GDINL void Insert(RedBlackTreeNodeType* const NewNode)
		{
			this->RedBlackTreeBase::Insert(static_cast<RedBlackTreeBaseNode*>(NewNode));
		}

		//! @brief Deletes the node from the tree.
		//! @param Node Node that would be deleted. Should be manually deleted.
		GDINL void Delete(RedBlackTreeNodeType* const Node)
		{
			this->RedBlackTreeBase::Delete(static_cast<RedBlackTreeBaseNode*>(Node));
		}

		//! @brief Searches for node with specified key.
		//! @param Key Key we are looking for.
		//! @{
		GDINL RedBlackTreeNodeType const* Query(CHandle const Element) const
		{
			return static_cast<RedBlackTreeNodeType const*>(this->RedBlackTreeBase::Query(Element));
		}
		GDINL RedBlackTreeNodeType* Query(CHandle const Element)
		{
			return const_cast<RedBlackTreeNodeType*>(const_cast<RedBlackTree const*>(this)->Query(Element));
		}
		//! @}

		//! @brief Removes all elements from the tree. All nodes should be manully deleted.
		GDINL void Clear()
		{
			this->RedBlackTreeBase::Clear();
		}

	public:

		//! @brief Moves other Red-Black Tree here.
		//! @param OtherTree Other Red-Black Tree that would be moved into current object.
		//! @returns this.
		GDINL RedBlackTree& operator= (RedBlackTree&& OtherTree)
		{
			this->RedBlackTreeBase::operator=(Forward<RedBlackTreeBase>(OtherTree));
			return *this;
		}
	};	// class RedBlackTreeBase

GD_NAMESPACE_END
