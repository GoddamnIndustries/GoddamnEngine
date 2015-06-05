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
#include <GoddamnEngine/Core/Templates/Utility.h>
#include <GoddamnEngine/Core/Containers/Containers.h>

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// RedBlackTreeBaseNode class and RedBlackTreeBase classes.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Defines some flags for base Red-Black Tree node.
	//! @{
	typedef UInt8 RedBlackTreeBaseNodeFlags;
	enum RedBlackTreeBaseNodeFlagsEnum : RedBlackTreeBaseNodeFlags
	{
		GD_REDBLACK_TREE_NODE_FLAGS_NONE			= 0,
		GD_REDBLACK_TREE_NODE_FLAGS_IS_RED			= GD_BIT(0),
		GD_REDBLACK_TREE_NODE_FLAGS_DATA_INLINED	= GD_BIT(1),
	};	// enum RedBlackTreeBaseNodeFlagsEnum : RedBlackTreeBaseNodeFlags
	//! @}

	// ------------------------------------------------------------------------------------------
	//! Defines a single node of the Red-Black Tree.
	class RedBlackTreeBaseNode : public IUncopiable
	{
	private:
		friend class RedBlackTreeBase;

		union
		{
			Handle Element;
			UInt8 InlineMemory[sizeof(Handle)];
		};	// anonymous union
		RedBlackTreeBaseNode* Left = nullptr;
		RedBlackTreeBaseNode* Right = nullptr;
		RedBlackTreeBaseNode* Parent = nullptr;
		RedBlackTreeBaseNodeFlags Flags = GD_REDBLACK_TREE_NODE_FLAGS_NONE;

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructor.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Initializes a tree node.
		//! @param Element The Element data pointer.
		GDINL RedBlackTreeBaseNode(Handle const Element, SizeTp const ElementSize = SizeTpMax)
			: Element(Element)
		{
			GD_NOT_USED(ElementSize);
		}

	private:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Class API.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Returns @c true if this node is red.
		//! @returns @c true if this node is red, false otherwise.
		GDINL bool GetIsRed() const
		{
			return (this->Flags & GD_REDBLACK_TREE_NODE_FLAGS_IS_RED) != GD_REDBLACK_TREE_NODE_FLAGS_NONE;
		}

		// ------------------------------------------------------------------------------------------
		//! Changes the color of the node.
		//! @param IsRed If is @c true, then node color is set to red, is @c false - to black.
		GDINL void SetIsRed(bool const IsRed)
		{
			if (IsRed) this->Flags |= GD_REDBLACK_TREE_NODE_FLAGS_IS_RED;
			else 	   this->Flags &= ~GD_REDBLACK_TREE_NODE_FLAGS_IS_RED;
		}

	public:

		// ------------------------------------------------------------------------------------------
		//! Returns Element data pointer.
		//! @returns Element data pointer.
		//! @{
		GDINL CHandle GetElementBase() const
		{
			return this->Element;
		}
		GDINL Handle GetElementBase()
		{
			return const_cast<Handle>(const_cast<RedBlackTreeBaseNode const*>(this)->GetElementBase());
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns pointer to left sub-tree of this node.
		//! @returns Pointer to left sub-tree of this node.
		//! @{
		GDINL RedBlackTreeBaseNode const* GetLeftBase() const { return this->Left; }
		GDINL RedBlackTreeBaseNode* GetLeftBase()
		{
			return const_cast<RedBlackTreeBaseNode*>(const_cast<RedBlackTreeBaseNode const*>(this)->GetLeftBase());
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns pointer to right sub-tree of this node.
		//! @returns Pointer to right sub-tree of this node.
		//! @{
		GDINL RedBlackTreeBaseNode const* GetRightBase() const
		{
			return this->Left;
		}
		GDINL RedBlackTreeBaseNode* GetRightBase()
		{
			return const_cast<RedBlackTreeBaseNode*>(const_cast<RedBlackTreeBaseNode const*>(this)->GetRightBase());
		}
		//! @}

	};	// class RedBlackTreeNode

	// ------------------------------------------------------------------------------------------
	//! Basic Red-Black Tree data structure. Contains fundamental tree management algorithms.
	//! @see http://en.wikipedia.org/wiki/Red–black_tree
	//! @see http://web.mit.edu/~emin/www.old/source_code/red_black_tree/index.html
	class RedBlackTreeBase : public IUncopiable
	{
	private:
		RedBlackTreeBaseNode* NullNode = nullptr;
		RedBlackTreeBaseNode* RootNode = nullptr;
		SizeTp Length = 0;

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructor and destructor.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Initializes a new Red-Black Tree.
		GDAPI RedBlackTreeBase();

		// ------------------------------------------------------------------------------------------
		//! Moves other Red-Black Tree here.
		//! @param Other Other tree to move here.
		GDAPI RedBlackTreeBase(RedBlackTreeBase&& Other);

		// ------------------------------------------------------------------------------------------
		//! Deinitializes a Red-Black Tree and destroys all internal data.
		GDAPI virtual ~RedBlackTreeBase();

	protected:

#if 0
		// ------------------------------------------------------------------------------------------
		//! Creates a new node.
		//! @param Element Pointer to the element that would be stored in the node.
		//! @returns Created node.
		GDAPI virtual RedBlackTreeBaseNode* CreateNode(Handle const Element) const abstract;
#endif if // 0

		// ------------------------------------------------------------------------------------------
		//! Destroys created tree node.
		//! @param Node Node to be destroyed.
		GDAPI virtual void DestroyNode(RedBlackTreeBaseNode* const Node) const abstract;

		// ------------------------------------------------------------------------------------------
		//! Compares Elements of the nodes. 
		//! @param First First Element.
		//! @param Second Second Element.
		//! @returns Zero, if specified Elements are equal, positive value if first is greater, negative otherwise.
		GDAPI virtual int CompareElements(CHandle const First, CHandle const Second) const abstract;

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Tree iteration.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Returns a node that represents a NULL sentinel of this tree.
		//! @returns A node that represents a NULL sentinel of this tree.
		//! @{
		GDINL RedBlackTreeBaseNode const* GetNullNode() const {	return this->NullNode; }
		GDINL RedBlackTreeBaseNode* GetNullNode()
		{
			return const_cast<RedBlackTreeBaseNode*>(const_cast<RedBlackTreeBase const*>(this)->GetNullNode());
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns the first node of the tree or null.
		//! @returns The first node of the tree or null if no first node exists.
		//! @{
		GDINL RedBlackTreeBaseNode const* GetFirstNode() const { return this->RootNode->Left; }
		GDINL RedBlackTreeBaseNode* GetFirstNode()
		{
			return const_cast<RedBlackTreeBaseNode*>(const_cast<RedBlackTreeBase const*>(this)->GetFirstNode());
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns the first node of the tree or null.
		//! @returns The first node of the tree or null if no first node exists.
		//! @{
		GDAPI RedBlackTreeBaseNode const* GetLastNode() const;
		GDINL RedBlackTreeBaseNode* GetLastNode()
		{
			return const_cast<RedBlackTreeBaseNode*>(const_cast<RedBlackTreeBase const*>(this)->GetLastNode());
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns next node to specified one or null.
		//! @param Node Some node.
		//! @returns Next node to this one or null node if not exists.
		//! @{
		GDAPI RedBlackTreeBaseNode const* GetNextNode(RedBlackTreeBaseNode const* const Node) const;
		GDINL RedBlackTreeBaseNode* GetNextNode(RedBlackTreeBaseNode* const Node)
		{
			return const_cast<RedBlackTreeBaseNode*>(const_cast<RedBlackTreeBase const*>(this)->GetNextNode(Node));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns previous node to specified one or null.
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

		// ------------------------------------------------------------------------------------------
		//! Returns number of nodes that exist in the tree.
		//! @returns Number of nodes that exist in tree.
		GDINL SizeTp GetLength() const
		{
			return this->Length;
		}

		// ------------------------------------------------------------------------------------------
		//! Returns true if this tree is empty.
		//! @returns True if this tree is empty, false otherwise.
		GDINL bool IsEmpty() const
		{
			return this->Length == 0;
		}

	private:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Tree manipulation.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Internally creates a new node.
		//! @param NewNode Reference to new node.
		GDAPI void InternalCreateNode(RedBlackTreeBaseNode*& NewNode);

		// ------------------------------------------------------------------------------------------
		//! Internally destroys a specified node and all is children.
		//! @param Node Node that is going to be destroyed.
		GDAPI void InternalDestroyNode(RedBlackTreeBaseNode* const Node);

		// ------------------------------------------------------------------------------------------
		//! Performs a left rotation of the tree.
		//! @param Node The node to rotate on.
		GDAPI void RotateLeft(RedBlackTreeBaseNode* const Node);

		// ------------------------------------------------------------------------------------------
		//! Performs a right rotation of the tree.
		//! @param Node The node to rotate on.
		GDAPI void RotateRight(RedBlackTreeBaseNode* const Node);

		// ------------------------------------------------------------------------------------------
		//! Fixes the tree and restores it's red-black properties.
		//! @param Node The node to start fixing.
		GDAPI void Repair(RedBlackTreeBaseNode* const Node);

	protected:

		// ------------------------------------------------------------------------------------------
		//! Searches for node with specified element.
		//! @param Element Key we are looking for.
		//! @returns Pointer to node or nullptr if nothing ws found.
		//! @{
		GDAPI RedBlackTreeBaseNode const* QueryNodeBase(CHandle const Element) const;
		GDINL RedBlackTreeBaseNode* QueryNodeBase(CHandle const Element) { return const_cast<RedBlackTreeBaseNode*>(const_cast<RedBlackTreeBase const*>(this)->QueryNodeBase(Element)); }
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Preforms an insertion of the new node with specified key and value.
		//! @param NewNode The node that would be inserted.
		GDAPI void InsertNode(RedBlackTreeBaseNode* const NewNode);

		// ------------------------------------------------------------------------------------------
		//! Deletes the node from the tree.
		//! @param Node Node that would be deleted.
		GDAPI void RemoveNode(RedBlackTreeBaseNode* const Node);

	public:

		// ------------------------------------------------------------------------------------------
		//! Removes all elements from the tree.
		GDAPI void Clear();

	protected:

		// ------------------------------------------------------------------------------------------
		//! Moves other Red-Black Tree here.
		//! @param OtherTree Other Red-Black Tree that would be moved into current object.
		//! @returns this.
		GDAPI RedBlackTreeBase& operator=(RedBlackTreeBase&& OtherTree)
		{
			this->Clear();
			Swap(this->RootNode, OtherTree.RootNode);
			Swap(this->NullNode, OtherTree.NullNode);
			return *this;
		}
	};	// class RedBlackTree

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// RedBlackTreeNode<T> class
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Defines a single node of the Red-Black Tree.
	//! @tparam ElementType Type of objected been stored in the node.
	template<typename ElementTypeTp>
	class RedBlackTreeNode final : public RedBlackTreeBaseNode
	{
		template<typename SomeElementType>
		friend class RedBlackTree;

	public:
		typedef ElementTypeTp ElementType;

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructor.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Initializes a tree node.
		//! @param Element The Element data pointer.
		GDINL explicit RedBlackTreeNode(ElementType* const Element) 
			: RedBlackTreeBaseNode(Element)
		{
		}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Class API.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Returns Element data pointer.
		//! @returns Element data pointer.
		//! @{
		GDINL ElementType const* GetElement() const { return reinterpret_cast<ElementType const*>(this->GetElementBase()); }
		GDINL ElementType* GetElement() { return const_cast<ElementType*>(const_cast<RedBlackTreeNode const*>(this)->GetElement()); }
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns pointer to left sub-tree of this node.
		//! @returns Pointer to left sub-tree of this node.
		//! @{
		GDINL RedBlackTreeNode const* GetLeft() const { return reinterpret_cast<RedBlackTreeNode const*>(this->GetLeftBase()); }
		GDINL RedBlackTreeNode* GetLeft() {	return const_cast<RedBlackTreeNode*>(const_cast<RedBlackTreeNode const*>(this)->GetLeft());	}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns pointer to right sub-tree of this node.
		//! @returns Pointer to right sub-tree of this node.
		//! @{
		GDINL RedBlackTreeNode const* GetRight() const { return reinterpret_cast<RedBlackTreeNode const*>(this->GetRightBase()); }
		GDINL RedBlackTreeNode* GetRight() { return const_cast<RedBlackTreeNode*>(const_cast<RedBlackTreeNode const*>(this)->GetRight()); }
		//! @}
	};	// struct RedBlackTreeNode

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// RedBlackTreeIterator<T> class.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Iterator for Red-Black Tree containers.
	//! @tparam RedBlackTreeContainerTypeTp Type of container.
	template<typename RedBlackTreeContainerTypeTp>
	struct RedBlackTreeIterator final
	{
	public:
		typedef RedBlackTreeContainerTypeTp		ContainerType;
		typedef typename Conditional<TypeTraits::IsConst<ContainerType>::Value, typename ContainerType::ElementType const, typename ContainerType::ElementType>::Type ElementType;
		typedef typename Conditional<TypeTraits::IsConst<ContainerType>::Value, RedBlackTreeNode<ElementType> const, RedBlackTreeNode<ElementType>>::Type RedBlackTreeNodeType;
		typedef typename Conditional<TypeTraits::IsConst<ContainerType>::Value, RedBlackTreeBaseNode const, RedBlackTreeBaseNode>::Type RedBlackTreeBaseNodeType;

	private:
		ContainerType&			Container;
		RedBlackTreeNodeType*	IterNode = nullptr;

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructor.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Initializes iterator that points on the specified node of the tree.
		//! @param Container Container that is going to be iterated.
		//! @param IterNode Start node of the iterator.
		//! @{
		GDINL RedBlackTreeIterator(ContainerType& Container, RedBlackTreeNodeType* const IterNode) 
			: Container(Container), IterNode(IterNode) { }
		GDINL RedBlackTreeIterator(ContainerType& Container, RedBlackTreeBaseNodeType* const IterNode) 
			: Container(Container), IterNode(static_cast<RedBlackTreeNodeType*>(IterNode)) {}
		//! @}

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Overloaded operators.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Returns node associated with this iterator.
		//! @return	Node associated with this iterator.
		GDINL RedBlackTreeNodeType const* GetNode()
		{
			return this->IterNode;
		}

		// ------------------------------------------------------------------------------------------
		//! Assigns this iterator other one.
		//! @param Other Other iterator to assign.
		//! @returns this.
		GDINL RedBlackTreeIterator& operator= (RedBlackTreeIterator const& Other)
		{
			GD_DEBUG_ASSERT(&this->Container == &Other.Container, "Iterators have different base containers.");
			this->IterNode = Other.IterNode;
			return *this;
		}

		// ------------------------------------------------------------------------------------------
		//! Assigns the iterator next node of the tree.
		//! @returns Incremented iterator.
		GDINL RedBlackTreeIterator& operator++ ()
		{
			this->IterNode = static_cast<RedBlackTreeNodeType*>(this->Container.GetNextNode(this->IterNode));
			return *this;
		}

		// ------------------------------------------------------------------------------------------
		//! Assigns the iterator next node of the tree.
		//! @param Unused Unused parameter passed be compiler.
		//! @returns Iterator before incrementing.
		GDINL RedBlackTreeIterator operator++ (int const Unused)
		{
			GD_NOT_USED(Unused);
			RedBlackTreeIterator Copy(*this);
			this->IterNode = static_cast<RedBlackTreeNodeType*>(this->Container.GetNextNode(this->IterNode));
			return Copy;
		}

		// ------------------------------------------------------------------------------------------
		//! Assigns the iterator previous node of the tree.
		//! @returns Decremented iterator.
		GDINL RedBlackTreeIterator& operator-- ()
		{
			this->IterNode = static_cast<RedBlackTreeNodeType*>(this->Container.GetPrevNode(this->IterNode));
			return *this;
		}

		// ------------------------------------------------------------------------------------------
		//! Assigns the iterator previous node of the tree.
		//! @param Unused Unused parameter passed be compiler.
		//! @returns Iterator before decrementing.
		GDINL RedBlackTreeIterator operator-- (int const Unused)
		{
			GD_NOT_USED(Unused);
			RedBlackTreeIterator Copy(*this);
			this->IterNode = static_cast<RedBlackTreeNodeType*>(this->Container.GetPrevNode(this->IterNode));
			return Copy;
		}

		// ------------------------------------------------------------------------------------------
		//! Compares two iterators on equality.
		//! @param Other Other iterator that would be compared.
		//! @returns True if iterators point to the same container at same indices, false otherwise.
		GDINL bool operator== (RedBlackTreeIterator const& Other) const
		{
			return (&this->Container == &Other.Container) && (this->IterNode == Other.IterNode);
		}

		// ------------------------------------------------------------------------------------------
		//! Compares two iterators on inequality.
		//! @param Other Other iterator that would be compared.
		//! @returns False if iterators point to the same container at same indices, true otherwise.
		GDINL bool operator!= (RedBlackTreeIterator const& Other) const
		{
			return (&this->Container != &Other.Container) || (this->IterNode != Other.IterNode);
		}

		// ------------------------------------------------------------------------------------------
		//! Dereferences value of the iterator.
		//! @returns Reference to the element of the node of the tree.
		GDINL ElementType& operator* () const
		{
			return reinterpret_cast<ElementType&>(*this->IterNode->GetElement());
		}

		// ------------------------------------------------------------------------------------------
		//! Dereferences value of the iterator.
		//! @returns Pointer to the element of the node of the tree.
		GDINL ElementType* operator-> () const
		{
			return reinterpret_cast<ElementType*>(this->IterNode->GetElement());
		}
	};	// struct RedBlackTreeIterator

	// ------------------------------------------------------------------------------------------
	//! Templated Red-Black Tree data structure. All management functions overloaded to reloaded to make it type-safe.
	//! @tparam ElementTypeTp Type of objected been stored in the nodes of the tree.
	template<typename ElementTypeTp>
	class RedBlackTree : public RedBlackTreeBase
	{
		template<typename RedBlackTreeContainerType>
		friend struct RedBlackTreeIterator;

	public:
		typedef ElementTypeTp									ElementType;
		typedef RedBlackTreeNode		<ElementTypeTp>			RedBlackTreeNodeType;
		typedef RedBlackTreeIterator	<RedBlackTree>			Iterator;
		typedef RedBlackTreeIterator	<RedBlackTree const>	ConstIterator;
		typedef ReverseContainerIterator<Iterator>				ReverseIterator;
		typedef ReverseContainerIterator<ConstIterator>			ReverseConstIterator;
		GD_CONTAINER_DEFINE_ITERATION_SUPPORT(RedBlackTree);

	protected:

		// ------------------------------------------------------------------------------------------
		//! Initializes a new Red-Black Tree.
		GDINL RedBlackTree() : RedBlackTreeBase() {	}

		// ------------------------------------------------------------------------------------------
		//! Moves other Red-Black Tree here.
		//! @param Other Other tree to move here.
		GDINL RedBlackTree(RedBlackTree&& Other) : RedBlackTreeBase(Forward<RedBlackTreeBase>(Other)) { }

		// ------------------------------------------------------------------------------------------
		//! Deinitializes a Red-Black Tree and destroys all internal data.
		GDINL virtual ~RedBlackTree() { }

	protected:

#if 0
		// ------------------------------------------------------------------------------------------
		//! Creates a new node.
		//! @param Element Pointer to the element that would be stored in the node.
		//! @returns Created node.
		GDAPI virtual RedBlackTreeBaseNode* CreateNode(Handle const Element) const override
		{
			return GD_NEW(RedBlackTreeNodeType, reinterpret_cast<ElementType*>(Element));
		}
#endif	// if 0

		// ------------------------------------------------------------------------------------------
		//! Destroys created tree node.
		//! @param Node Node to be destroyed.
		GDAPI virtual void DestroyNode(RedBlackTreeBaseNode* const Node) const override
		{
			GD_DELETE(static_cast<RedBlackTreeNodeType*>(Node));
		}

		// ------------------------------------------------------------------------------------------
		//! Compares Elements of the nodes by keys. 
		//! @param First First Element.
		//! @param Second Second Element.
		//! @returns Zero, if specified Elements are equal, positive value if first is greater, negative otherwise.
		GDINL virtual int CompareElements(CHandle const First, CHandle const Second) const override
		{
			ElementType const& FirstElem  = *reinterpret_cast<ElementType const*>(First);
			ElementType const& SecondElem = *reinterpret_cast<ElementType const*>(Second);
			if (FirstElem == SecondElem) return 0;
			if (FirstElem >  SecondElem) return 1;
			return -1;
		}

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Tree iteration.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Returns iterator that points to first container element.
		//! @returns Iterator that points to first container element.
		//! @{
		GDINL Iterator Begin() { return Iterator(*this, this->GetFirstNode()); }
		GDINL ConstIterator Begin() const { return ConstIterator(*this, this->GetFirstNode()); }
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns iterator that points to past the end container element.
		//! @returns Iterator that points to past the end container element.
		//! @{
		GDINL Iterator End() { return Iterator(*this, this->GetNullNode()); }
		GDINL ConstIterator End() const { return ConstIterator(*this, this->GetNullNode()); }
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns iterator that points to last container element.
		//! @returns Iterator that points to last container element.
		//! @{
		GDINL ReverseIterator ReverseBegin() { return ReverseIterator(*this, this->GetLastNode()); }
		GDINL ReverseConstIterator ReverseBegin() const { return ReverseConstIterator(*this, this->GetLastNode()); }
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns iterator that points to preceding the first container element.
		//! @returns Iterator that points to preceding the first container element.
		//! @{
		GDINL ReverseIterator ReverseEnd() { return ReverseIterator(*this, this->GetNullNode()); }
		GDINL ReverseConstIterator ReverseEnd() const { return ReverseConstIterator(*this, this->GetNullNode()); }
		//! @}

	private:

		// ------------------------------------------------------------------------------------------
		//! Searches for node with specified element.
		//! @param Element Key we are looking for.
		//! @returns Pointer to node or nullptr if nothing ws found.
		//! @{
	protected:
		GDINL RedBlackTreeNodeType const* QueryNode(CHandle const Element) const { return static_cast<RedBlackTreeNodeType const*>(this->QueryNodeBase(Element)); }
		GDINL RedBlackTreeNodeType* QueryNode(CHandle const Element) { return const_cast<RedBlackTreeNodeType*>(const_cast<RedBlackTree const*>(this)->QueryNode(Element)); }
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Queries for the iterator of the element with specified key.
		//! @param Element Element we are looking for.
		//! @returns Iterator on the element if it was found and End iterator otherwise.
		//! @{
	public:
		GDINL ConstIterator QueryIterator(ElementType const& Element) const { return ConstIterator(*this, this->QueryNode(&Element)); }
		GDINL Iterator QueryIterator(ElementType const& Element) { return Iterator(*this, this->QueryNode(&Element)); }
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Preforms an insertion of the new node with specified key and value.
		//! @param NewNode The node that would be inserted. Should be already created.
		//! @param Element Element that would be inserted.
		//! @{
		GDINL void Insert(ElementType&& Element) { this->InsertNode(GD_NEW(RedBlackTreeNodeType, GD_NEW(ElementType, Forward<ElementType>(Element)))); }
		GDINL void Insert(ElementType const& Element) {	this->InsertNode(GD_NEW(RedBlackTreeNodeType, GD_NEW(ElementType, Element))); }
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Removes existing element from the tree.
		//! @param Element Element that is going to be removed.
		//! @{
		GDINL void Remove(ElementType const& Element) { this->RemovePtr(&Element); }
		GDINL void RemovePtr(CHandle const Element)
		{
			auto QueriedNode = this->QueryNode(Element);
			GD_DEBUG_ASSERT(QueriedNode != nullptr, "Specified element key does not exist.");
			this->RemoveNode(QueriedNode);
		}
		//! @}

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Overloaded operators.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Moves other Red-Black Tree here.
		//! @param OtherTree Other Red-Black Tree that would be moved into current object.
		//! @returns This.
		GDINL RedBlackTree& operator= (RedBlackTree&& OtherTree)
		{
			this->RedBlackTreeBase::operator=(Forward<RedBlackTreeBase>(OtherTree));
			return *this;
		}
	};	// class RedBlackTreeBase

GD_NAMESPACE_END
