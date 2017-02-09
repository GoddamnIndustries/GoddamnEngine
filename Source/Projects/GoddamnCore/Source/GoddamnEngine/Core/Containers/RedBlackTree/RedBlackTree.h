// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Containers/RedBlackTree/RedBlackTree.h
 * Red-Black tree data structure interface.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/Utility.h>
#include <GoddamnEngine/Core/Templates/Iterators.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                 RedBlackTreeBase & RedBlackTreeBaseNode classes.                 ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	// **------------------------------------------------------------------------------------------**
	//! Defines a single node of the Red-Black Tree.
	// **------------------------------------------------------------------------------------------**
	struct RedBlackTreeBaseNode : public TNonCopyable
	{
		friend class RedBlackTreeBase;

	private:
		RedBlackTreeBaseNode* m_Left   = nullptr;
		RedBlackTreeBaseNode* m_Right  = nullptr;
		RedBlackTreeBaseNode* m_Parent = nullptr;
		bool				  m_IsRed  = false;
#if GD_PLATFORM_WINDOWS && !GD_RELEASE
		bool                  m_IsNull = false;
#endif	// if GD_PLATFORM_WINDOWS && !GD_RELEASE
		Byte				  m_DataUntyped[1];

	public:

		/*!
		 * Returns element data pointer.
		 */
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

		/*!
		 * Returns true if this node is null.
		 */
		GDINL bool IsNull() const
		{
			return m_IsNull;
		}

	};	// struct RedBlackTreeBaseNode

	// **------------------------------------------------------------------------------------------**
	//! Basic Red-Black Tree data structure. Contains fundamental tree management algorithms.
	//! 
	//! @see https://en.wikipedia.org/wiki/Red–black_tree
	//! @see http://web.mit.edu/~emin/www.old/source_code/red_black_tree/index.html
	// **------------------------------------------------------------------------------------------**
	class RedBlackTreeBase : public TNonCopyable
	{
	private:
		RedBlackTreeBaseNode* m_NullNode = nullptr;
		RedBlackTreeBaseNode* m_RootNode = nullptr;
		SizeTp                m_Length = 0;

	protected:

		// ------------------------------------------------------------------------------------------
		// Constructor and destructor.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Initializes a new Red-Black Tree.
		 */
		GDAPI RedBlackTreeBase();

		/*!
		 * Moves other Red-Black Tree here.
		 * @param other The other tree to move here.
		 */
		GDAPI RedBlackTreeBase(RedBlackTreeBase&& other) noexcept;

		/*!
		 * Deinitializes a Red-Black Tree and destroys all internal data.
		 */
		GDAPI virtual ~RedBlackTreeBase();

	public:

		// ------------------------------------------------------------------------------------------
		// Tree iteration.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Returns number of nodes that exist in the tree.
		 */
		GDINL SizeTp GetLength() const
		{
			return m_Length;
		}

		/*!
		 * Returns true if this tree is empty.
		 */
		GDINL bool IsEmpty() const
		{
			return m_Length == 0;
		}

		/*!
		 * Removes all elements from the tree.
		 */
		GDAPI void Clear();

	protected:

		/*!
		 * Returns a node that represents a null sentinel of this tree.
		 */
		//! @{
		GDINL RedBlackTreeBaseNode const* GetNullNodeBase() const
		{
			return m_NullNode;
		}
		GDINL RedBlackTreeBaseNode* GetNullNodeBase()
		{
			return const_cast<RedBlackTreeBaseNode*>(const_cast<RedBlackTreeBase const*>(this)->GetNullNodeBase());
		}
		//! @}

		/*!
		 * Returns first node of the tree or null.
		 */
		//! @{
		GDAPI RedBlackTreeBaseNode const* GetFirstNodeBase() const;
		GDINL RedBlackTreeBaseNode* GetFirstNodeBase()
		{
			return const_cast<RedBlackTreeBaseNode*>(const_cast<RedBlackTreeBase const*>(this)->GetFirstNodeBase());
		}
		//! @}

		/*!
		 * Returns last node of the tree or null.
		 */
		//! @{
		GDAPI RedBlackTreeBaseNode const* GetLastNodeBase() const;
		GDINL RedBlackTreeBaseNode* GetLastNodeBase()
		{
			return const_cast<RedBlackTreeBaseNode*>(const_cast<RedBlackTreeBase const*>(this)->GetLastNodeBase());
		}
		//! @}

		/*!
		 * Returns next node to specified one or null.
		 *
		 * @param node Some node.
		 * @returns Next node to this one or null node if not exists.
		 */
		//! @{
		GDAPI RedBlackTreeBaseNode const* GetNextNodeBase(RedBlackTreeBaseNode const* const node) const;	// ReSharper disable once CppMemberFunctionMayBeConst
		GDINL RedBlackTreeBaseNode* GetNextNodeBase(RedBlackTreeBaseNode* const node)	
		{
			return const_cast<RedBlackTreeBaseNode*>(const_cast<RedBlackTreeBase const*>(this)->GetNextNodeBase(node));
		}
		//! @}

		/*!
		 * Returns previous node to specified one or null.
		 *
		 * @param node Some node.
		 * @returns Previous node to this one or null node if not exists.
		 */
		//! @{
		GDAPI RedBlackTreeBaseNode const* GetPrevNodeBase(RedBlackTreeBaseNode const* const node) const;	// ReSharper disable once CppMemberFunctionMayBeConst
		GDINL RedBlackTreeBaseNode* GetPrevNodeBase(RedBlackTreeBaseNode* const node)	
		{
			return const_cast<RedBlackTreeBaseNode*>(const_cast<RedBlackTreeBase const*>(this)->GetPrevNodeBase(node));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// Tree manipulation.
		// ------------------------------------------------------------------------------------------

	private:

		/*!
		 * Internally creates a new node.
		 * @param newNode Reference to new node.
		 */
		GDAPI void InternalCreateNodeBase(RedBlackTreeBaseNode*& newNode);

		/*!
		 * Internally destroys a specified node and all is children.
		 * @param node The node that is going to be destroyed.
		 */
		GDAPI void InternalDestroyNode(RedBlackTreeBaseNode* const node);

		/*!
		 * Performs a left rotation of the tree.
		 * @param node The node to rotate on.
		 */
		GDAPI void InternalRotateLeft(RedBlackTreeBaseNode* const node);

		/*!
		 * Performs a right rotation of the tree.
		 * @param node The node to rotate on.
		 */
		GDAPI void InternalRotateRight(RedBlackTreeBaseNode* const node);

		/*!
		 * Fixes the tree and restores it's Red-Black properties.
		 * @param node The node to start fixing.
		 */
		GDAPI void InternalRepair(RedBlackTreeBaseNode* const node);

	protected:

		/*!
		 * Destroys created tree node.
		 * @param node The node to be destroyed.
		 */
		GDAPI virtual void OnDestroyNode(RedBlackTreeBaseNode* const node) const GD_PURE_VIRTUAL;

		/*!
		 * Compares Elements of the nodes. 
		 *
		 * @param lhs First element.
		 * @param rhs Second element.
		 *
		 * @returns Zero, if specified elements are equal, positive value if first is greater, negative otherwise.
		 */
		GDAPI virtual int OnCompareElements(CHandle const lhs, CHandle const rhs) const GD_PURE_VIRTUAL;

	protected:

		/*!
		 * Searches for node with specified element.
		 *
		 * @param element Key we are looking for.
		 * @returns pointer to node or nullptr if nothing was found.
		 */
		//! @{
		GDAPI RedBlackTreeBaseNode const* FindNodeBase(CHandle const element) const;
		GDINL RedBlackTreeBaseNode* FindNodeBase(CHandle const element)
		{
			return const_cast<RedBlackTreeBaseNode*>(const_cast<RedBlackTreeBase const*>(this)->FindNodeBase(element));
		}
		//! @}

		/*!
		 * Performs an insertion of the new node with specified Key and value.
		 * @param newNode The node that would be inserted.
		 */
		GDAPI void InsertNodeBase(RedBlackTreeBaseNode* const newNode);

		/*!
		 * Deletes the node from the tree.
		 * @param node The node that would be deleted.
		 */
		GDAPI void RemoveNodeBase(RedBlackTreeBaseNode* const node);

	protected:
		GDINL RedBlackTreeBase& operator=(RedBlackTreeBase&& OtherTree) noexcept
		{
			Clear();
			Swap(m_RootNode, OtherTree.m_RootNode);
			Swap(m_NullNode, OtherTree.m_NullNode);
			return *this;
		}
	};	// class RedBlackTreeBase

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******         RedBlackTree, RedBlackTreeIterator & RedBlackTreeNode classes.           ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	// **------------------------------------------------------------------------------------------**
	//! Defines a single node of the Red-Black Tree.
	//! @tparam TElement Type of objected been stored in the node.
	//! @warning Do not create/destroy instances of this class using default allocation operators. 
	// **------------------------------------------------------------------------------------------**
	template<typename TElement>
	struct RedBlackTreeNode final : public RedBlackTreeBaseNode
	{
		template<typename, typename> 
		friend class RedBlackTree;
		using ElementType = TElement;

	private:

		/*!
		 * Initializes the node and the element inside a node. 
		 * @param arguments Constructor arguments of the data that would be emplaced inside node.
		 */
		template<typename... TArguments>
		GDINL explicit RedBlackTreeNode(TArguments&&... arguments)
		{
			new (GetData()) TElement(Utils::Forward<TArguments>(arguments)...);
		}

		GDINL ~RedBlackTreeNode()
		{
			GetData()->~TElement();
		}

	public:

		/*!
		 * Returns element data pointer.
		 */
		//! @{
		GDINL TElement const* GetData() const
		{
			return reinterpret_cast<TElement const*>(GetDataUntyped());
		}
		GDINL TElement* GetData()
		{
			return const_cast<TElement*>(const_cast<RedBlackTreeNode const*>(this)->GetData());
		}
		//! @}

	};	// struct RedBlackTreeNode

	// **------------------------------------------------------------------------------------------**
	//! Iterator for Red-Black Tree-based containers.
	//! @tparam TTreeContainer Type of container.
	// **------------------------------------------------------------------------------------------**
	template<typename TTreeContainer>
	struct RedBlackTreeIterator final
	{
	public:
		using ContainerType            = TTreeContainer;
		using ElementType              = Conditional<TypeTraits::IsConst<ContainerType>::Value, typename ContainerType::ElementType const, typename ContainerType::ElementType>;
		using RedBlackTreeNodeType     = Conditional<TypeTraits::IsConst<ContainerType>::Value, RedBlackTreeNode<ElementType> const, RedBlackTreeNode<ElementType>>;
		using RedBlackTreeBaseNodeType = Conditional<TypeTraits::IsConst<ContainerType>::Value, RedBlackTreeBaseNode          const, RedBlackTreeBaseNode         >;

	private:
		ContainerType&		  m_Container;
		RedBlackTreeNodeType* m_IterNode = nullptr;

	public:

		/*!
		 * Initializes Iterator that points on the specified node of the tree.
		 *
		 * @param container The container that is going to be iterated.
		 * @param iterNode Start node of the Iterator.
		 */
		GDINL RedBlackTreeIterator(ContainerType& container, RedBlackTreeBaseNodeType* const iterNode)
			: m_Container(container), m_IterNode(static_cast<RedBlackTreeNodeType*>(iterNode))
		{
		}

	public:

		/*!
		 * Returns node associated with this Iterator.
		 */
		GDINL RedBlackTreeNodeType const* GetNode()
		{
			return m_IterNode;
		}

		// ------------------------------------------------------------------------------------------
		// Overloaded operators.
		// ------------------------------------------------------------------------------------------

		GDINL RedBlackTreeIterator& operator= (RedBlackTreeIterator const& other)
		{
			GD_DEBUG_VERIFY(&m_Container == &other.m_Container, "Iterators have different base containers.");
			m_IterNode = other.m_IterNode;
			return *this;
		}

		// iterator++
		GDINL RedBlackTreeIterator& operator++ ()
		{
			m_IterNode = static_cast<RedBlackTreeNodeType*>(m_Container.GetNextNodeBase(m_IterNode));
			return *this;
		}
		GDINL RedBlackTreeIterator operator++ (int const unused)
		{
			GD_NOT_USED(unused);
			auto copy(*this);
			m_IterNode = static_cast<RedBlackTreeNodeType*>(m_Container.GetNextNodeBase(m_IterNode));
			return copy;
		}

		// iterator--
		GDINL RedBlackTreeIterator& operator-- ()
		{
			m_IterNode = static_cast<RedBlackTreeNodeType*>(m_Container.GetPrevNodeBase(m_IterNode));
			return *this;
		}
		GDINL RedBlackTreeIterator operator-- (int const unused)
		{
			GD_NOT_USED(unused);
			auto copy(*this);
			m_IterNode = static_cast<RedBlackTreeNodeType*>(m_Container.GetPrevNodeBase(m_IterNode));
			return copy;
		}

		// iterator + ptrdiff_t
		GDINL RedBlackTreeIterator& operator+= (PtrDiffTp const offset)
		{
			if (offset > 0)
			{
				for (PtrDiffTp cnt = 0; cnt < offset; ++cnt)
				{
					++(*this);
				}
			}
			else
			{
				for (PtrDiffTp cnt = 0; cnt < -offset; ++cnt)
				{
					--(*this);
				}
			}
			return *this;
		}
		GDINL RedBlackTreeIterator operator+ (PtrDiffTp const offset) const
		{
			auto copy(*this);
			return copy += offset;
		}

		// iterator - ptrdiff_t
		GDINL RedBlackTreeIterator& operator-= (PtrDiffTp const offset)
		{
			return *this += -offset;
		}
		GDINL RedBlackTreeIterator operator- (PtrDiffTp const offset) const
		{
			auto copy(*this);
			return copy -= offset;
		}

		// iterator == iterator
		GDINL bool operator== (RedBlackTreeIterator const& other) const
		{
			return (&m_Container == &other.m_Container) && (m_IterNode == other.m_IterNode);
		}
		GDINL bool operator!= (RedBlackTreeIterator const& other) const
		{
			return (&m_Container != &other.m_Container) || (m_IterNode != other.m_IterNode);
		}

		// *iterator
		GDINL ElementType& operator* () const
		{
			GD_DEBUG_VERIFY(!m_IterNode->IsNull(), "Null node pointer.");
			return reinterpret_cast<ElementType&>(*m_IterNode->GetData());
		}
		GDINL ElementType* operator-> () const
		{
			return reinterpret_cast<ElementType*>(m_IterNode->GetData());
		}

	};	// struct RedBlackTreeIterator

	// **------------------------------------------------------------------------------------------**
	//! Templated Red-Black Tree data structure. All management functions overloaded to reloaded to 
	//! make it type-safe.
	//! @tparam TElement Type of objected been stored in the nodes of the tree.
	// **------------------------------------------------------------------------------------------**
	template<typename TElement, typename TAllocator = DefaultContainerAllocator>
	class RedBlackTree : public RedBlackTreeBase, public IIteratable<RedBlackTree<TElement, TAllocator>>
	{
		template<typename>
		friend struct RedBlackTreeIterator;

	public:
		using ElementType          = TElement;
		using RedBlackTreeNodeType = RedBlackTreeNode<TElement>;
		using Iterator             = RedBlackTreeIterator<RedBlackTree>;
		using ConstIterator        = RedBlackTreeIterator<RedBlackTree const>;
		using ReverseIterator      = ReverseContainerIterator<Iterator>;
		using ReverseConstIterator = ReverseContainerIterator<ConstIterator>;

		GD_CONTAINER_DEFINE_ITERATION_SUPPORT(RedBlackTree)

	protected:

		/*!
		 * Initializes a new Red-Black Tree.
		 */
		GDINL RedBlackTree() = default;

		/*!
		 * Moves other Red-Black Tree here.
		 * @param other The other tree to move here.
		 */
		GDINL RedBlackTree(RedBlackTree&& other) = default;

		GDINL virtual ~RedBlackTree()
		{
			Clear();
		}

	public:

		// ------------------------------------------------------------------------------------------
		// Tree iteration.
		// ------------------------------------------------------------------------------------------

		GDINL Iterator Begin()
		{
			return Iterator(*this, GetFirstNodeBase());
		}
		GDINL ConstIterator Begin() const
		{
			return ConstIterator(*this, GetFirstNodeBase());
		}

		GDINL Iterator End()
		{
			return Iterator(*this, GetNullNodeBase());
		}
		GDINL ConstIterator End() const
		{
			return ConstIterator(*this, GetNullNodeBase());
		}

		GDINL ReverseIterator ReverseBegin()
		{
			return ReverseIterator(*this, GetLastNodeBase());
		}
		GDINL ReverseConstIterator ReverseBegin() const
		{
			return ReverseConstIterator(*this, GetLastNodeBase());
		}

		GDINL ReverseIterator ReverseEnd()
		{
			return ReverseIterator(*this, GetNullNodeBase());
		}
		GDINL ReverseConstIterator ReverseEnd() const
		{
			return ReverseConstIterator(*this, GetNullNodeBase());
		}

		// ------------------------------------------------------------------------------------------
		// Tree manipulation.
		// ------------------------------------------------------------------------------------------

	public:

		/*!
		 * Returns reference to the first element of the map.
		 */
		//! @{
		GDINL TElement const& GetFirst() const
		{
			GD_DEBUG_VERIFY(!IsEmpty(), "Red-black tree is empty.");
			return *static_cast<RedBlackTreeNodeType const*>(GetFirstNodeBase())->GetData();
		}
		GDINL TElement& GetFirst()
		{
			return const_cast<TElement&>(const_cast<RedBlackTree const*>(this)->GetFirst());
		}
		//! @}

		/*!
		 * Returns reference to the last element of the map.
		 */
		//! @{
		GDINL TElement const& GetLast() const
		{
			GD_DEBUG_VERIFY(!IsEmpty(), "Red-black tree is empty.");
			return *static_cast<RedBlackTreeNodeType const*>(GetLastNodeBase())->GetData();
		}
		GDINL TElement& GetLast()
		{
			return const_cast<TElement&>(const_cast<RedBlackTree const*>(this)->GetLast());
		}
		//! @}

	protected:

		/*!
		 * Creates a new node.
		 *
		 * @param element pointer to the element that would be stored in the node.
		 * @returns Created node.
		 */
		template<typename... TArguments>
		GDINT static RedBlackTreeNodeType* InternalCreateNode(TArguments&&... Arguments)
		{
			auto const allocatedNode = static_cast<RedBlackTreeNodeType*>(GD_MALLOC(sizeof(RedBlackTreeNodeType) + sizeof(TElement) - 1));
			new (allocatedNode) RedBlackTreeNodeType(Utils::Forward<TArguments>(Arguments)...);
			return allocatedNode;
		}

		/*!
		 * Destroys created tree node.
		 * @param node The node to be destroyed.
		 */
		GDINT virtual void OnDestroyNode(RedBlackTreeBaseNode* const node) const override final
		{
			static_cast<RedBlackTreeNodeType*>(node)->~RedBlackTreeNodeType();
			GD_FREE(node);
		}

		/*!
		 * Compares Elements of the nodes by keys. 
		 *
		 * @param lhs First element.
		 * @param rhs Second element.
		 *
		 * @returns Zero, if specified elements are equal, positive value if first is greater, negative otherwise.
		 */
		GDINL virtual int OnCompareElements(CHandle const lhs, CHandle const rhs) const override final
		{
			auto const& lhsElem = *reinterpret_cast<TElement const*>(lhs);
			auto const& rhsElem = *reinterpret_cast<TElement const*>(rhs);
			return lhsElem == rhsElem ? 0 : (lhsElem > rhsElem ? 1 : -1);
		}

		// ------------------------------------------------------------------------------------------
		// Overloaded operators.
		// ------------------------------------------------------------------------------------------

	public:
		GDINL RedBlackTree& operator= (RedBlackTree&& otherTree) = default;

	};	// class RedBlackTreeBase

GD_NAMESPACE_END
