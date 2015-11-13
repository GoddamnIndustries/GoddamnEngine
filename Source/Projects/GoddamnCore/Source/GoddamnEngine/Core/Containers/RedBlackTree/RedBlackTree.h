// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Containers/RedBlackTree/RedBlackTree.h
//! Red-Black tree data structure interface.
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Utility.h>
#include <GoddamnEngine/Core/Iterators.h>

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// RedBlackTreeBase & RedBlackTreeBaseNode classes.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Defines a single node of the Red-Black Tree.
	struct RedBlackTreeBaseNode : public IUncopiable
	{
	private:
		friend class RedBlackTreeBase;

	private:
		RedBlackTreeBaseNode*	m_Left   = nullptr;
		RedBlackTreeBaseNode*	m_Right	 = nullptr;
		RedBlackTreeBaseNode*	m_Parent = nullptr;
		bool					m_IsRed  = false;
#if GD_PLATFORM_WINDOWS && !GD_RELEASE
		bool                    m_IsNull = false;
#endif	// if GD_PLATFORM_WINDOWS && !GD_RELEASE
		Byte					m_DataUntyped[1];

	public:

		// ------------------------------------------------------------------------------------------
		//! Returns element data pointer.
		//! @returns element data pointer.
		//! @{
		GDINL CHandle GetDataUntyped() const
		{
			return &m_DataUntyped[0];
		}
		GDINL Handle GetDataUntyped()
		{
			return const_cast<Handle>(const_cast<RedBlackTreeBaseNode const*>(this)->GetDataUntyped());
		}
		//! @}
	};	// struct RedBlackTreeBaseNode

	// ------------------------------------------------------------------------------------------
	//! Basic Red-Black Tree data structure. Contains fundamental tree management algorithms.
	//! @see http://en.wikipedia.org/wiki/m_IsRed–black_tree
	//! @see http://web.mit.edu/~emin/www.old/source_code/red_black_tree/m_Index.html
	class RedBlackTreeBase : public IUncopiable
	{
	private:
		RedBlackTreeBaseNode* m_NullNode = nullptr;
		RedBlackTreeBaseNode* m_RootNode = nullptr;
		SizeTp                m_Length = 0;

	protected:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructor and destructor.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Initializes a new Red-Black Tree.
		GDAPI RedBlackTreeBase();

		// ------------------------------------------------------------------------------------------
		//! Moves other Red-Black Tree here.
		//! @param other The other tree to move here.
		GDAPI RedBlackTreeBase(RedBlackTreeBase&& other);

		// ------------------------------------------------------------------------------------------
		//! Deinitializes a Red-Black Tree and destroys all internal data.
		GDAPI virtual ~RedBlackTreeBase();

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Tree iteration.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Returns number of nodes that exist in the tree.
		//! @returns Number of nodes that exist in tree.
		GDINL SizeTp GetLength() const
		{
			return m_Length;
		}

		// ------------------------------------------------------------------------------------------
		//! Returns true if this tree is empty.
		//! @returns True if this tree is empty, false otherwise.
		GDINL bool IsEmpty() const
		{
			return m_Length == 0;
		}

		// ------------------------------------------------------------------------------------------
		//! Removes all elements from the tree.
		GDAPI void Clear();

	protected:

		// ------------------------------------------------------------------------------------------
		//! Returns a node that represents a null sentinel of this tree.
		//! @returns A node that represents a null sentinel of this tree.
		//! @{
		GDINL RedBlackTreeBaseNode const* _GetNullNode() const
		{
			return m_NullNode;
		}
		GDINL RedBlackTreeBaseNode* _GetNullNode()
		{
			return const_cast<RedBlackTreeBaseNode*>(const_cast<RedBlackTreeBase const*>(this)->_GetNullNode());
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns the first node of the tree or null.
		//! @returns The first node of the tree or null if no first node exists.
		//! @{
		GDAPI RedBlackTreeBaseNode const* _GetFirstNode() const;
		GDINL RedBlackTreeBaseNode* _GetFirstNode()
		{
			return const_cast<RedBlackTreeBaseNode*>(const_cast<RedBlackTreeBase const*>(this)->_GetFirstNode());
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns the first node of the tree or null.
		//! @returns The first node of the tree or null if no first node exists.
		//! @{
		GDAPI RedBlackTreeBaseNode const* _GetLastNode() const;
		GDINL RedBlackTreeBaseNode* _GetLastNode()
		{
			return const_cast<RedBlackTreeBaseNode*>(const_cast<RedBlackTreeBase const*>(this)->_GetLastNode());
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns next node to specified one or null.
		//! @param node Some node.
		//! @returns Next node to this one or null node if not exists.
		//! @{
		GDAPI RedBlackTreeBaseNode const* _GetNextNode(RedBlackTreeBaseNode const* const node) const;
		GDINL RedBlackTreeBaseNode* _GetNextNode(RedBlackTreeBaseNode* const node)
		{
			return const_cast<RedBlackTreeBaseNode*>(const_cast<RedBlackTreeBase const*>(this)->_GetNextNode(node));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns previous node to specified one or null.
		//! @param node Some node.
		//! @returns Previous node to this one or null node if not exists.
		//! @{
		GDAPI RedBlackTreeBaseNode const* _GetPrevNode(RedBlackTreeBaseNode const* const node) const;
		GDINL RedBlackTreeBaseNode* _GetPrevNode(RedBlackTreeBaseNode* const node)
		{
			return const_cast<RedBlackTreeBaseNode*>(const_cast<RedBlackTreeBase const*>(this)->_GetPrevNode(node));
		}
		//! @}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Tree manipulation.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	private:

		// ------------------------------------------------------------------------------------------
		//! Internally creates a new node.
		//! @param newNode Reference to new node.
		GDAPI void _InternalCreateNode(RedBlackTreeBaseNode*& newNode);

		// ------------------------------------------------------------------------------------------
		//! Internally destroys a specified node and all is children.
		//! @param node The node that is going to be destroyed.
		GDAPI void _InternalDestroyNode(RedBlackTreeBaseNode* const node);

		// ------------------------------------------------------------------------------------------
		//! Performs a m_Left rotation of the tree.
		//! @param node The node to rotate on.
		GDAPI void _InternalRotateLeft(RedBlackTreeBaseNode* const node);

		// ------------------------------------------------------------------------------------------
		//! Performs a m_Right rotation of the tree.
		//! @param node The node to rotate on.
		GDAPI void _InternalRotateRight(RedBlackTreeBaseNode* const node);

		// ------------------------------------------------------------------------------------------
		//! Fixes the tree and restores it's Red-Black properties.
		//! @param node The node to start fixing.
		GDAPI void _InternalRepair(RedBlackTreeBaseNode* const node);

	protected:

		// ------------------------------------------------------------------------------------------
		//! Destroys created tree node.
		//! @param node The node to be destroyed.
		GDAPI virtual void _DestroyNode(RedBlackTreeBaseNode* const node) const GD_PURE_VIRTUAL;

		// ------------------------------------------------------------------------------------------
		//! Compares Elements of the nodes. 
		//! @param lhs First element.
		//! @param rhs Second element.
		//! @returns Zero, if specified Elements are equal, positive value if first is greater, negative otherwise.
		GDAPI virtual int _CompareElements(CHandle const lhs, CHandle const rhs) const GD_PURE_VIRTUAL;

	protected:

		// ------------------------------------------------------------------------------------------
		//! Searches for node with specified element.
		//! @param element Key we are looking for.
		//! @returns pointer to node or nullptr if nothing was found.
		//! @{
		GDAPI RedBlackTreeBaseNode const* _QueryNode(CHandle const element) const;
		GDINL RedBlackTreeBaseNode* _QueryNode(CHandle const element)
		{
			return const_cast<RedBlackTreeBaseNode*>(const_cast<RedBlackTreeBase const*>(this)->_QueryNode(element));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Performs an insertion of the new node with specified Key and value.
		//! @param newNode The node that would be inserted.
		GDAPI void _InsertNode(RedBlackTreeBaseNode* const newNode);

		// ------------------------------------------------------------------------------------------
		//! Deletes the node from the tree.
		//! @param node The node that would be deleted.
		GDAPI void _RemoveNode(RedBlackTreeBaseNode* const node);

	protected:

		// ------------------------------------------------------------------------------------------
		//! Moves other Red-Black Tree here.
		//! @param OtherTree other Red-Black Tree that would be moved into current object.
		//! @returns this.
		GDAPI RedBlackTreeBase& operator=(RedBlackTreeBase&& OtherTree)
		{
			Clear();
			Swap(m_RootNode, OtherTree.m_RootNode);
			Swap(m_NullNode, OtherTree.m_NullNode);
			return *this;
		}
	};	// class RedBlackTreeBase

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// RedBlackTree, RedBlackTreeIterator & RedBlackTreeNode classes.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Defines a single node of the Red-Black Tree.
	//! @tparam ElementType Type of objected been stored in the node.
	//! @warning Do not create/destroy instances of this class using default allocation operators. 
	template<typename TElement>
	struct RedBlackTreeNode final : public RedBlackTreeBaseNode
	{
		template<typename SomeElementType> 
		friend class RedBlackTree;
		friend class Allocator;
		typedef TElement ElementType;

	private:

		// ------------------------------------------------------------------------------------------
		//! Initializes the node and the element inside a node. 
		//! @param Arguments Constructor arguments of the data that would be emplaced inside node.
		template<typename... TArguments>
		GDINL explicit RedBlackTreeNode(TArguments&&... arguments)
		{
			new (GetData()) ElementType(Forward<TArguments>(arguments)...);
		}

		// ------------------------------------------------------------------------------------------
		//! Initializes the node and the element inside a node.
		GDINL ~RedBlackTreeNode()
		{
			GetData()->~ElementType();
		}

	public:

		// ------------------------------------------------------------------------------------------
		//! Returns element data pointer.
		//! @returns element data pointer.
		//! @{
		GDINL ElementType const* GetData() const
		{
			return reinterpret_cast<ElementType const*>(GetDataUntyped());
		}
		GDINL ElementType* GetData()
		{
			return const_cast<ElementType*>(const_cast<RedBlackTreeNode const*>(this)->GetData());
		}
		//! @}
	};	// struct RedBlackTreeNode

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// RedBlackTreeIterator<T> class.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Iterator for Red-Black Tree containers.
	//! @tparam TTreeContainer Type of m_Container.
	template<typename TTreeContainer>
	struct RedBlackTreeIterator final
	{
	public:
		using ContainerType            = TTreeContainer;
		using ElementType              = typename Conditional<TypeTraits::IsConst<ContainerType>::Value, typename ContainerType::ElementType const, typename ContainerType::ElementType>::Type;
		using RedBlackTreeNodeType     = typename Conditional<TypeTraits::IsConst<ContainerType>::Value, RedBlackTreeNode<ElementType> const, RedBlackTreeNode<ElementType>>::Type;
		using RedBlackTreeBaseNodeType = typename Conditional<TypeTraits::IsConst<ContainerType>::Value, RedBlackTreeBaseNode          const, RedBlackTreeBaseNode         >::Type;

	private:
		ContainerType&			m_Container;
		RedBlackTreeNodeType*	m_IterNode = nullptr;

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructor.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Initializes Iterator that points on the specified node of the tree.
		//! @param m_Container The m_Container that is going to be iterated.
		//! @param m_IterNode Start node of the Iterator.
		GDINL RedBlackTreeIterator(ContainerType& container, RedBlackTreeBaseNodeType* const iterNode)
			: m_Container(container), m_IterNode(static_cast<RedBlackTreeNodeType*>(iterNode))
		{
		}

	public:

		// ------------------------------------------------------------------------------------------
		//! Returns node associated with this Iterator.
		//! @return	node associated with this Iterator.
		GDINL RedBlackTreeNodeType const* GetNode()
		{
			return m_IterNode;
		}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Overloaded operators.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		GDINL RedBlackTreeIterator& operator= (RedBlackTreeIterator const& other)
		{
			GD_DEBUG_ASSERT(&m_Container == &other.m_Container, "Iterators have different base containers.");
			m_IterNode = other.m_IterNode;
			return *this;
		}

		GDINL RedBlackTreeIterator& operator++ ()
		{
			m_IterNode = static_cast<RedBlackTreeNodeType*>(m_Container._GetNextNode(m_IterNode));
			return *this;
		}
		GDINL RedBlackTreeIterator operator++ (int const unused)
		{
			GD_NOT_USED(unused);
			RedBlackTreeIterator copy(*this);
			m_IterNode = static_cast<RedBlackTreeNodeType*>(m_Container._GetNextNode(m_IterNode));
			return copy;
		}

		GDINL RedBlackTreeIterator& operator-- ()
		{
			m_IterNode = static_cast<RedBlackTreeNodeType*>(m_Container._GetPrevNode(m_IterNode));
			return *this;
		}
		GDINL RedBlackTreeIterator operator-- (int const unused)
		{
			GD_NOT_USED(unused);
			RedBlackTreeIterator copy(*this);
			m_IterNode = static_cast<RedBlackTreeNodeType*>(m_Container._GetPrevNode(m_IterNode));
			return copy;
		}

		GDINL RedBlackTreeIterator& operator+= (PtrDiffTp const offset)
		{
			if (offset > 0)
			{
				for (PtrDiffTp cnt = 0; cnt < offset; ++cnt)
					++(*this);
			}
			else
			{
				for (PtrDiffTp cnt = 0; cnt < -offset; ++cnt)
					--(*this);
			}
			return *this;
		}
		GDINL RedBlackTreeIterator operator+ (PtrDiffTp const offset) const
		{
			RedBlackTreeIterator copy(*this);
			return copy += offset;
		}

		GDINL RedBlackTreeIterator& operator-= (PtrDiffTp const offset)
		{
			return *this += (-offset);
		}
		GDINL RedBlackTreeIterator operator- (PtrDiffTp const offset) const
		{
			RedBlackTreeIterator copy(*this);
			return copy -= offset;
		}

		GDINL bool operator== (RedBlackTreeIterator const& other) const
		{
			return (&m_Container == &other.m_Container) && (m_IterNode == other.m_IterNode);
		}
		GDINL bool operator!= (RedBlackTreeIterator const& other) const
		{
			return (&m_Container != &other.m_Container) || (m_IterNode != other.m_IterNode);
		}

		GDINL ElementType& operator* () const
		{
			return reinterpret_cast<ElementType&>(*m_IterNode->GetData());
		}
		GDINL ElementType* operator-> () const
		{
			return reinterpret_cast<ElementType*>(m_IterNode->GetData());
		}
	};	// struct RedBlackTreeIterator

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// RedBlackTreeBase RedBlackTree<T> classes.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Templated Red-Black Tree data structure. All management functions overloaded to reloaded to make it type-safe.
	//! @tparam TElement Type of objected been stored in the nodes of the tree.
	template<typename TElement>
	class RedBlackTree : public RedBlackTreeBase
	{
		template<typename RedBlackTreeTContainer>
		friend struct RedBlackTreeIterator;

	public:
		using ElementType          = TElement;
		using RedBlackTreeNodeType = RedBlackTreeNode<TElement>;
		using Iterator             = RedBlackTreeIterator<RedBlackTree>;
		using ConstIterator        = RedBlackTreeIterator<RedBlackTree const>;
		using ReverseIterator      = ReverseContainerIterator<Iterator>;
		using ReverseConstIterator = ReverseContainerIterator<ConstIterator>;
	
		GD_CONTAINER_DEFINE_ITERATION_SUPPORT(RedBlackTree);

	protected:

		// ------------------------------------------------------------------------------------------
		//! Initializes a new Red-Black Tree.
		GDINL RedBlackTree() 
			: RedBlackTreeBase()
		{
		}

		// ------------------------------------------------------------------------------------------
		//! Moves other Red-Black Tree here.
		//! @param other The other tree to move here.
		GDINL RedBlackTree(RedBlackTree&& other) 
			: RedBlackTreeBase(Forward<RedBlackTreeBase>(other))
		{
		}

		// ------------------------------------------------------------------------------------------
		//! Deinitializes a Red-Black Tree and destroys all internal data.
		GDINL virtual ~RedBlackTree()
		{
		}

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Tree iteration.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		GDINL Iterator Begin()
		{
			return Iterator(*this, _GetFirstNode());
		}
		GDINL ConstIterator Begin() const
		{
			return ConstIterator(*this, _GetFirstNode());
		}

		GDINL Iterator End()
		{
			return Iterator(*this, _GetNullNode());
		}
		GDINL ConstIterator End() const
		{
			return ConstIterator(*this, _GetNullNode());
		}

		GDINL ReverseIterator ReverseBegin()
		{
			return ReverseIterator(*this, _GetLastNode());
		}
		GDINL ReverseConstIterator ReverseBegin() const
		{
			return ReverseConstIterator(*this, _GetLastNode());
		}

		GDINL ReverseIterator ReverseEnd()
		{
			return ReverseIterator(*this, _GetNullNode());
		}
		GDINL ReverseConstIterator ReverseEnd() const
		{
			return ReverseConstIterator(*this, _GetNullNode());
		}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Tree manipulation.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	public:

		// ------------------------------------------------------------------------------------------
		//! Returns reference to the first element of the map.
		//! @returns Reference to the first element of the map.
		//! @{
		GDINL ElementType const& GetFirst() const
		{
			GD_DEBUG_ASSERT(!IsEmpty(), "Red-black tree is empty.");
			return *static_cast<RedBlackTreeNodeType const*>(_GetFirstNode())->GetData();
		}
		GDINL ElementType& GetFirst()
		{
			return const_cast<ElementType&>(const_cast<RedBlackTree const*>(this)->GetFirst());
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns reference to the last element of the map.
		//! @returns Reference to the last element of the map.
		//! @{
		GDINL ElementType const& GetLast() const
		{
			GD_DEBUG_ASSERT(!IsEmpty(), "Red-black tree is empty.");
			return *static_cast<RedBlackTreeNodeType const*>(_GetLastNode())->GetData();
		}
		GDINL ElementType& GetLast()
		{
			return const_cast<ElementType&>(const_cast<RedBlackTree const*>(this)->GetLast());
		}
		//! @}

	protected:

		// ------------------------------------------------------------------------------------------
		//! Creates a new node.
		//! @param element pointer to the element that would be stored in the node.
		//! @returns Created node.
		//! @{
		template<typename... TArguments>
		GDINT static RedBlackTreeNodeType* CreateNode(TArguments&&... Arguments)
		{
			RedBlackTreeNodeType* const AllocatedNode = static_cast<RedBlackTreeNodeType*>(
				GD_MALLOC(sizeof(RedBlackTreeNodeType) + sizeof(ElementType) - sizeof(Byte)));
			new (AllocatedNode) RedBlackTreeNodeType(Forward<TArguments>(Arguments)...);
			return AllocatedNode;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Destroys created tree node.
		//! @param node The node to be destroyed.
		GDINT virtual void _DestroyNode(RedBlackTreeBaseNode* const node) const override
		{
			static_cast<RedBlackTreeNodeType*>(node)->~RedBlackTreeNodeType();
			GD_DEALLOC(node);
		}

		// ------------------------------------------------------------------------------------------
		//! Compares Elements of the nodes by keys. 
		//! @param lhs First element.
		//! @param rhs Second element.
		//! @returns Zero, if specified Elements are equal, positive value if first is greater, negative otherwise.
		GDINL virtual int _CompareElements(CHandle const lhs, CHandle const rhs) const override
		{
			ElementType const& FirstElem  = *reinterpret_cast<ElementType const*>(lhs);
			ElementType const& SecondElem = *reinterpret_cast<ElementType const*>(rhs);
			if (FirstElem == SecondElem) return 0;
			if (FirstElem >  SecondElem) return 1;
			return -1;
		}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Overloaded operators.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		GDINL RedBlackTree& operator= (RedBlackTree&& otherTree)
		{
			RedBlackTreeBase::operator=(Forward<RedBlackTreeBase>(otherTree));
			return *this;
		}
	};	// class RedBlackTreeBase

GD_NAMESPACE_END
