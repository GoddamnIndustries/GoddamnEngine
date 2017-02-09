// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Containers/List.h
 * Linked list class.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/Utility.h>
#include <GoddamnEngine/Core/Templates/Iterators.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******            LinkedList, LinkedListIterator & LinkedListNode classes.              ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	// **------------------------------------------------------------------------------------------**
	//! Defines a single node of a single-linked list.
	//! @tparam TElement Container element type.
	// **------------------------------------------------------------------------------------------**
	template<typename TElement>
	class LinkedListNode
	{
		template<typename, typename>
		friend class LinkedList;

	private:
		LinkedListNode* m_Next;
		TElement m_Element;

	public:

		/*!
		 * Initializes a single node of a single-linked container.
		 * @param arguments Constructor arguments of the data that would be emplaced inside node.
		 */
		template<typename... TArguments>
		GDINL explicit LinkedListNode(LinkedListNode* const next, TArguments&&... arguments)
			: m_Next(next), m_Element(Utils::Forward<TElement&&>(arguments)...)
		{}

	public:

		/*!
		 * Returns Element data reference.
		 */
		//! @{
		GDINL TElement const& GetElement() const
		{
			return m_Element;
		}
		GDINL TElement& GetElement()
		{
			return const_cast<TElement&>(const_cast<LinkedListNode const*>(this)->GetElement());
		}
		//! @}

		/*!
		 * Returns next node to this.
		 */
		//! @{
		GDINL LinkedListNode const* GetNextNode() const
		{
			return m_Next;
		}
		GDINL LinkedListNode* GetNextNode()
		{
			return const_cast<LinkedListNode*>(const_cast<LinkedListNode const*>(this)->GetNextNode());
		}
		//! @}

	}; // class LinkedListNode

	// **------------------------------------------------------------------------------------------**
	//! Single-linked list iterator type.
	//! @tparam	TLinkedListNode Single linked list node type.
	// **------------------------------------------------------------------------------------------**
	template <typename TLinkedListNode>
	struct LinkedListIterator
	{
	private:
		TLinkedListNode* m_IterNode;

	public:

		/*!
		 * Initializes iterator that points on the specified node of the list.
		 * @param iterNode Some node of the list.
		 */
		GDINL explicit LinkedListIterator(TLinkedListNode* iterNode)
			: m_IterNode(iterNode)
		{}

	public:

		// iterator++
		GDINL LinkedListIterator& operator++()
		{
			m_IterNode = m_IterNode->GetNextNode();
			return *this;
		}
		GDINL LinkedListIterator operator++(int const unused)
		{
			GD_NOT_USED(unused);
			auto copy(*this);
			m_IterNode = m_IterNode->GetNextNode();
			return copy;
		}

		// iterator + size_t
		GDINL LinkedListIterator& operator+= (SizeTp const offset)
		{
			for (PtrDiffTp cnt = 0; cnt < offset; ++cnt)
			{
				++(*this);
			}
			return *this;
		}
		GDINL LinkedListIterator operator+ (SizeTp const offset) const
		{
			auto copy(*this);
			return copy += offset;
		}

		// iterator == iterator
		GDINL bool operator==(LinkedListIterator const& other) const
		{
			return m_IterNode == other.m_IterNode;
		}
		GDINL bool operator!=(LinkedListIterator const& other) const
		{
			return m_IterNode != other.m_IterNode;
		}

		// *iterator
		GDINL auto operator*() const
		{
			return m_IterNode->GetElement();
		}
		GDINL auto operator->() const
		{
			return m_IterNode->GetElement();
		}

	}; // class LinkedListIterator

	// **------------------------------------------------------------------------------------------**
	//! Single-linked template list class.
	//! @tparam ElementType List element type.
	// **------------------------------------------------------------------------------------------**
	template<typename TElement, typename TAllocator = HeapAllocator>
	class LinkedList final : public TNonCopyable, public TAllocator
	{
	public:
		using Element            = TElement;
		using Allocator          = TAllocator;
		using LinkedListNodeType = LinkedListNode<TElement>;
		using Iterator           = LinkedListIterator<LinkedListNodeType>;
		using ConstIterator      = LinkedListIterator<LinkedListNodeType const>;

		GD_CONTAINER_DEFINE_ITERATION_SUPPORT(LinkedList)

	private:
		LinkedListNodeType* m_FirstNode;
		SizeTp m_Length;

	public:

		// ------------------------------------------------------------------------------------------
		// Constructors and destructor.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Initializes an empty list.
		 */
		GDINL LinkedList()
			: m_FirstNode(nullptr), m_Length(0)
		{}

		/*!
		 * Moves other list here.
		 * @param other The other list to move here.
		 */
		GDINL LinkedList(LinkedList&& other) noexcept
			: m_FirstNode(other.m_FirstNode), m_Length(other.m_Length)
		{
			other.m_FirstNode = nullptr;
			other.m_Length = 0;
		}

		GDINL ~LinkedList()
		{
			this->Clear();
		}

	public:

		// ------------------------------------------------------------------------------------------
		// Iteration API.
		// ------------------------------------------------------------------------------------------

		GDINL Iterator Begin()
		{
			return Iterator(m_FirstNode);
		}
		GDINL ConstIterator Begin() const
		{
			return ConstIterator(m_FirstNode);
		}

		GDINL Iterator End()
		{
			return Iterator(nullptr);
		}
		GDINL ConstIterator End() const
		{
			return ConstIterator(nullptr);
		}

	public:

		// ------------------------------------------------------------------------------------------
		// Dynamic size management.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Returns number of elements that exist in list.
		 */
		GDINL SizeTp GetLength() const
		{
			return m_Length;
		}

		/*!
		 * Returns true if this list is empty.
		 */
		GDINL bool IsEmpty() const
		{
			return m_Length == 0;
		}

		/*!
		 * Resizes list to make it able to contain specified number of elements.
		 * @param newLength New required length of the list.
		 */
		GDINL void Resize(UInt32 const newLength)
		{
			GD_NOT_IMPLEMENTED();
		}

		/*!
		 * Destroys all elements in list with memory deallocation.
		 */
		GDINL void Clear()
		{
			while (m_FirstNode != nullptr)
			{
				auto const remNode = m_FirstNode;
				m_FirstNode = m_FirstNode->GetNextNode();

				remNode->~LinkedListNodeType();
				GD_FREE(remNode);
			}
			m_Length = 0;
			m_FirstNode = nullptr;
		}

	public:

		// ------------------------------------------------------------------------------------------
		// Dynamic elements access.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Returns reference on first element in the list.
		 */
		//! @{
		GDINL TElement const& GetFirst() const
		{
			GD_DEBUG_VERIFY(m_FirstNode != nullptr, "Index is out of bounds");
			return m_FirstNode->GetElement();
		}
		GDINL TElement& GetFirst()
		{
			return const_cast<TElement&>(const_cast<LinkedList const*>(this)->GetLast());
		}
		//! @}

		/*!
		 * Adds a new specified element to the beginning of the list.
		 * @param element The element that would be inserted.
		 */
		//! @{
		GDINL TElement& InsertFirst(TElement&& element)
		{
			auto const newNode = GD_MALLOC_T(LinkedListNodeType);
			new (newNode) LinkedListNodeType(m_FirstNode, Utils::Forward<TElement>(element));

			m_FirstNode = newNode;
			return newNode->GetElement();
		}
		GDINL TElement& InsertFirst(TElement const& element)
		{
			auto const newNode = GD_MALLOC_T(LinkedListNodeType);
			new (newNode) LinkedListNodeType(m_FirstNode, element);

			m_FirstNode = newNode;
			return newNode->GetElement();
		}
		//! @}

		//! @brief Inserts the node after the specified node.
		//! @param Node Node that would be inserted.
		//! @param After Node after that new one would be inserted. By default, it is before first node.
		GDINL void InsertNodeAfter(LinkedListNodeType* const Node, LinkedListNodeType* const After = nullptr)
		{
			GD_DEBUG_VERIFY(Node != nullptr, "Null pointer node specified.");
			if (After == nullptr)
			{
				this->InsertNodeFirst(Node);
			}
			else
			{
				Node->m_Next = After->m_Next;
				After->m_Next = Node;

				this->m_Length += 1;
			}
		}

		//! @brief Inserts the element after the specified node.
		//! @param Element Element that would be inserted.
		//! @param After Node after that new one would be inserted. By default, it is last node.
		//! @{
		GDINL void InsertAfter(TElement&& Element = TElement(), LinkedListNodeType* const After = nullptr)
		{
			this->InsertNodeAfter(new LinkedListNodeType(Forward<TElement>(Element), After));
		}
		GDINL void InsertAfter(TElement const& Element, LinkedListNodeType* const After = nullptr)
		{
			this->InsertNodeAfter(new LinkedListNodeType(Element, After));
		}
		//! @}

		//! @brief Removes the node from the list.
		//! @param Node	Node that would be removed from the list.
		GDINL void RemoveNode(LinkedListNodeType* const Node)
		{
			GD_DEBUG_VERIFY(Node != nullptr, "Null pointer node specified.");

			//! @todo Check if the node is related to list...
			if (this->m_FirstNode == this->LastNode)
			{
				delete Node;
				this->m_FirstNode = nullptr;
				this->m_Length = 0;
			}
			else
			{
				if (this->m_FirstNode == Node)
				{
					this->m_FirstNode = this->m_FirstNode->m_Next;
				}
				else
				{
					Node->Prev->m_Next = Node->m_Next;
				}

				delete Node;
				this->m_Length -= 1;
			}
		}

		//! @brief Removes first node of the list.
		GDINL void RemoveFirstNode()
		{
			this->RemoveNode(this->m_FirstNode);
		}

		//! @brief Removes last node of the list.
		GDINL void RemoveLastNode()
		{
			this->RemoveNode(this->LastNode);
		}

	public:

		// ------------------------------------------------------------------------------------------
		// Overloaded operators.
		// ------------------------------------------------------------------------------------------

		GDINL LinkedList& operator=(LinkedList&& other) noexcept
		{
			if (&other != this)
			{
				Clear();
				m_FirstNode = other.m_FirstNode;
				m_Length = other.m_Length;
				other.m_FirstNode = nullptr;
				other.m_Length = 0;
			}
			return *this;
		}

	}; // class LinkedList

	template<typename TElement, SizeTp TCapacity = InlineAllocatorCapacityNormal>
	using TempLinkedList = LinkedList<TElement, InlineFixedSizedPoolAllocator<sizeof LinkedListNode<TElement>, InlineAllocatorCapacityNormal>>;
	template<typename TElement>
	using TempTinyLinkedList = TempLinkedList<TElement, InlineAllocatorCapacityTiny>;
	template<typename TElement>
	using TempHugeLinkedList = TempLinkedList<TElement, InlineAllocatorCapacityHuge>;

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******    DoubleLinkedList, DoubleLinkedListIterator & DoubleLinkedListNode classes.    ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

#if 0
	//! @brief Defines a single node of a double-linked container.
	//! @tparam ElementType Container element type.
	template <typename ElementType>
	class DoubleLinkedListNode
	{
		template <typename SomeElementType>
		friend class DoubleLinkedList;

	private:
		GD_CLASS_UNCOPIABLE(DoubleLinkedListNode);
		DoubleLinkedListNode* Next;
		DoubleLinkedListNode* Prev;
		ElementType Element;

	public:
		//! @brief Initializes a single node of a double-linked container.
		//! @param Element Element that would be stored in this node.
		//! @{
		GDINL DoubleLinkedListNode(ElementType&& Element = ElementType())
			: Element(Forward<ElementType>(Element))
		{
		}
		GDINL DoubleLinkedListNode(ElementType const& Element)
			: Element(Element)
		{
		}
		//! @}

		//! @brief Returns next node to this.
		//! @returns Next node to this.
		//! @{
		GDINL DoubleLinkedListNode const* GetNextNode() const
		{
			return this->Next;
		}
		GDINL DoubleLinkedListNode* GetNextNode()
		{
			return const_cast<DoubleLinkedListNode*>(const_cast<DoubleLinkedListNode const*>(this)->GetNextNode());
		}
		//! @}

		//! @brief Returns previous node to this.
		//! @returns Next previous to this.
		//! @{
		GDINL DoubleLinkedListNode const* GetPrevNode() const
		{
			return this->Prev;
		}
		GDINL DoubleLinkedListNode* GetPrevNode()
		{
			return const_cast<DoubleLinkedListNode*>(const_cast<DoubleLinkedListNode const*>(this)->GetPrevNode());
		}
		//! @}

		//! @brief Returns Element data reference.
		//! @returns Element data reference.
		//! @{
		GDINL ElementType const& GetElement() const
		{
			return this->Element;
		}
		GDINL ElementType& GetElement()
		{
			return const_cast<ElementType&>(const_cast<DoubleLinkedListNode const*>(this)->GetElement());
		}
		//! @}

	}; // class DoubleLinkedListNode

	//! @brief Double-linked list iterator type.
	//! @tparam	DoubleLinkedListNodeType Double linked list node type.
	template <typename DoubleLinkedListNodeType>
	struct DoubleLinkedListIterator
	{
	private:
		DoubleLinkedListNodeType* IterNode;

	public:
		//! @brief Initializes iterator that points on the specified node of the list.
		//! @param IterNode Some node of the list.
		GDINL explicit DoubleLinkedListIterator(DoubleLinkedListNodeType* IterNode)
			: IterNode(IterNode)
		{
		}

		//! @brief Assigns the iterator next node of the list.
		//! @returns Incremented iterator.
		GDINL DoubleLinkedListIterator& operator++()
		{
			this->IterNode = this->IterNode->GetNextNode();
			return *this;
		}

		//! @brief Assigns the iterator next node of the list.
		//! @param Unused Unused parameter passed be compiler.
		//! @returns Iterator before incrementing.
		GDINL DoubleLinkedListIterator operator++(int const Unused)
		{
			GD_NOT_USED(Unused);
			DoubleLinkedListIterator Copy(*this);
			this->IterNode = this->IterNode->GetNextNode();
			return Copy;
		}

		//! @brief Assigns the iterator previous node of the list.
		//! @returns Decremented iterator.
		GDINL DoubleLinkedListIterator& operator--()
		{
			this->IterNode = this->IterNode->GetPrevNode();
			return *this;
		}

		//! @brief Assigns the iterator previous node of the list.
		//! @param Unused Unused parameter passed be compiler.
		//! @returns Iterator before decrementing.
		GDINL DoubleLinkedListIterator operator--(int const Unused)
		{
			GD_NOT_USED(Unused);
			DoubleLinkedListIterator Copy(*this);
			this->IterNode = this->IterNode->GetPrevNode();
			return Copy;
		}

		//! @brief Compares two iterators on equality.
		//! @param Other Other iterator that would be compared.
		//! @returns True if iterators point to the same container at same indices, false otherwise.
		GDINL bool operator==(DoubleLinkedListIterator const& Other) const
		{
			return this->IterNode == Other.IterNode;
		}

		//! @brief Compares two iterators on inequality.
		//! @param Other Other iterator that would be compared.
		//! @returns False if iterators point to the same container at same indices, true otherwise.
		GDINL bool operator!=(DoubleLinkedListIterator const& Other) const
		{
			return this->IterNode != Other.IterNode;
		}

		//! @brief Dereferences value of the iterator.
		//! @returns Reference to the element of the node of the list.
		GDINL decltype(DeclValue<DoubleLinkedListNodeType>().GetElement()) operator*() const
		{
			return this->IterNode->GetElement();
		}

		//! @brief Dereferences value of the iterator.
		//! @returns Reference to the element of the node of the list.
		GDINL decltype(DeclValue<DoubleLinkedListNodeType>().GetElement()) operator->() const
		{
			return this->IterNode->GetElement();
		}
	}; // class DoubleLinkedListIterator

	//! @brief Double-linked template list class.
	//! @tparam ElementType Container element type.
	template <typename ElementType>
	class DoubleLinkedList final : public IContainer, public ContainerIteratableTag, public ContainerReverseIteratableTag
	{
	public:
		typedef DoubleLinkedListNode<ElementType> DoubleLinkedListNodeType;

		typedef DoubleLinkedListIterator<DoubleLinkedListNodeType> Iterator;
		typedef DoubleLinkedListIterator<DoubleLinkedListNodeType const> ConstIterator;
		GD_CONTAINER_CHECK_ITERATORS(DoubleLinkedList);

		typedef ReverseContainerIterator<Iterator> ReverseIterator;
		typedef ReverseContainerIterator<ConstIterator> ReverseConstIterator;
		GD_CONTAINER_CHECK_REVERSE_ITERATORS(DoubleLinkedList);

		GD_CONTAINER_DEFINE_ITERATION_SUPPORT(DoubleLinkedList);

	private:
		GD_CLASS_UNCOPIABLE(DoubleLinkedList);
		DoubleLinkedListNodeType* FirstNode = nullptr;
		DoubleLinkedListNodeType* LastNode = nullptr;
		size_t Length = 0;

	public:
		//! @brief Initializes an empty list.
		GDINL DoubleLinkedList()
		{
		}

		//! @brief Moves other vector here.
		//! @param OtherVector Vector would be moved into current object.
		GDINL DoubleLinkedList(DoubleLinkedList&& Other)
		{
			this->FirstNode = Other.FirstNode;
			this->LastNode = Other.LastNode;
			this->Length = Other.Length;
			Other.FirstNode = nullptr;
			Other.LastNode = nullptr;
			Other.Length = 0;
		}

		//! @brief Deinitializes all nodes and deallocates memory.
		GDINL ~DoubleLinkedList()
		{
			this->Clear();
		}

	public:
		//! @brief Returns iterator that points to first container element.
		//! @returns Iterator that points to first container element.
		//! @{
		GDINL Iterator Begin()
		{
			return Iterator(this->FirstNode);
		}
		GDINL ConstIterator Begin() const
		{
			return ConstIterator(this->FirstNode);
		}
		//! @}

		//! @brief Returns iterator that points to past the end container element.
		//! @returns Iterator that points to past the end container element.
		//! @{
		GDINL Iterator End()
		{
			return Iterator(nullptr);
		}
		GDINL ConstIterator End() const
		{
			return ConstIterator(nullptr);
		}
		//! @}

		//! @brief Returns iterator that points to last container element.
		//! @returns Iterator that points to last container element.
		//! @{
		GDINL ReverseIterator ReverseBegin()
		{
			return ReverseIterator(this->LastNode);
		}
		GDINL ReverseConstIterator ReverseBegin() const
		{
			return ReverseConstIterator(this->LastNode);
		}
		//! @}

		//! @brief Returns iterator that points to preceding the first container element.
		//! @returns Iterator that points to preceding the first container element.
		//! @{
		GDINL ReverseIterator ReverseEnd()
		{
			return ReverseIterator(nullptr);
		}
		GDINL ReverseConstIterator ReverseEnd() const
		{
			return ReverseConstIterator(nullptr);
		}
		//! @}

	public:
		//! @brief Returns pointer on the first node of the list.
		//! @returns Pointer on the first node of the list.
		//! @{
		GDINL DoubleLinkedListNodeType const* GetFirstNode() const
		{
			return this->FirstNode;
		}
		GDINL DoubleLinkedListNodeType* GetFirstNode()
		{
			return const_cast<DoubleLinkedListNodeType*>(const_cast<DoubleLinkedList const*>(this)->GetFirstNode());
		}
		//! @}

		//! @brief Returns pointer on the last node of the list.
		//! @returns Pointer on the last node of the list.
		//! @{
		GDINL DoubleLinkedListNodeType const* GetLastNode() const
		{
			return this->LastNode;
		}
		GDINL DoubleLinkedListNodeType* GetLastNode()
		{
			return const_cast<DoubleLinkedListNodeType*>(const_cast<DoubleLinkedList const*>(this)->GetLastNode());
		}
		//! @}

		//! @brief Returns number of elements that exist in list.
		//! @returns Number of elements that exist in list.
		GDINL size_t GetLength() const
		{
			return this->Length;
		}

		//! @brief Returns true if this list is empty.
		//! @returns True if this list is empty, false otherwise.
		GDINL bool IsEmpty() const
		{
			return this->Length == 0;
		}

		//! @brief Inserts the node to the front of the list.
		//! @param Node Node that would be inserted.
		GDINL void InsertNodeFirst(DoubleLinkedListNodeType* const Node)
		{
			GD_DEBUG_VERIFY(Node != nullptr, "Null pointer node specified.");
			if (this->FirstNode == nullptr)
			{
				this->FirstNode = Node;
				this->LastNode = Node;
			}
			else
			{
				Node->m_Next = this->FirstNode;
				this->FirstNode = Node;
			}

			this->Length += 1;
		}

		//! @brief Inserts the element to the front of the list.
		//! @param Element Element that would be inserted.
		//! @{
		GDINL void InsertFirst(ElementType&& Element = ElementType())
		{
			this->InsertNodeFirst(new DoubleLinkedListNodeType(Forward<ElementType>(Element)));
		}
		GDINL void InsertFirst(ElementType const& Element)
		{
			this->InsertNodeFirst(new DoubleLinkedListNodeType(Element));
		}
		//! @}

		//! @brief Inserts the node to the end of the list.
		//! @param Node Node that would be inserted.
		GDINL void InsertNodeLast(DoubleLinkedListNodeType* const Node)
		{
			GD_DEBUG_VERIFY(Node != nullptr, "Null pointer node specified.");
			if (this->LastNode == nullptr)
			{
				this->LastNode = Node;
				this->FirstNode = Node;
			}
			else
			{
				Node->Prev = this->LastNode;
				this->LastNode = Node;
			}

			this->Length += 1;
		}

		//! @brief Inserts the element to the end of the list.
		//! @param Element Element that would be inserted.
		//! @{
		GDINL void InsertLast(ElementType&& Element = ElementType())
		{
			this->InsertNodeLast(new DoubleLinkedListNodeType(Forward<ElementType>(Element)));
		}
		GDINL void InsertLast(ElementType const& Element)
		{
			this->InsertNodeLast(new DoubleLinkedListNodeType(Element));
		}
		//! @}

		//! @brief Inserts the node after the specified node.
		//! @param Node Node that would be inserted.
		//! @param After Node after that new one would be inserted. By default, it is last node.
		GDINL void InsertNodeAfter(DoubleLinkedListNodeType* const Node, DoubleLinkedListNodeType* const After = nullptr)
		{
			GD_DEBUG_VERIFY(Node != nullptr, "Null pointer node specified.");
			if (After == nullptr)
			{
				this->InsertNodeLast(Node);
			}
			else
			{
				Node->m_Next = After->m_Next;
				Node->Prev = After;
				After->m_Next->Prev = Node;
				After->m_Next = Node;

				this->Length += 1;
			}
		}

		//! @brief Inserts the element after the specified node.
		//! @param Element Element that would be inserted.
		//! @param After Node after that new one would be inserted. By default, it is last node.
		//! @{
		GDINL void InsertAfter(ElementType&& Element = ElementType(), DoubleLinkedListNodeType* const After = nullptr)
		{
			this->InsertNodeAfter(new DoubleLinkedListNodeType(Forward<ElementType>(Element), After));
		}
		GDINL void InsertAfter(ElementType const& Element, DoubleLinkedListNodeType* const After = nullptr)
		{
			this->InsertNodeAfter(new DoubleLinkedListNodeType(Element, After));
		}
		//! @}

		//! @brief Removes the node from the list.
		//! @param Node	Node that would be removed from the list.
		GDINL void RemoveNode(DoubleLinkedListNodeType* const Node)
		{
			GD_DEBUG_VERIFY(Node != nullptr, "Null pointer node specified.");

			//! @todo Check if the node is related to list...
			if (this->FirstNode == this->LastNode)
			{
				delete Node;
				this->FirstNode = nullptr;
				this->LastNode = nullptr;
				this->Length = 0;
			}
			else
			{
				if (this->FirstNode == Node)
				{
					this->FirstNode = this->FirstNode->m_Next;
					this->FirstNode->Prev = nullptr;
				}
				else if (this->LastNode == Node)
				{
					this->LastNode = this->LastNode->Prev;
					this->LastNode->m_Next = nullptr;
				}
				else
				{
					Node->m_Next->Prev = Node->Prev;
					Node->Prev->m_Next = Node->m_Next;
				}

				delete Node;
				this->Length -= 1;
			}
		}

		//! @brief Removes first node of the list.
		GDINL void RemoveFirstNode()
		{
			this->RemoveNode(this->FirstNode);
		}

		//! @brief Removes last node of the list.
		GDINL void RemoveLastNode()
		{
			this->RemoveNode(this->LastNode);
		}

		//! @brief Destroys all nodes of the list.
		GDINL void Clear()
		{
			DoubleLinkedListNodeType* IterNode = this->FirstNode;
			while (this->FirstNode != nullptr)
			{
				IterNode = this->FirstNode->m_Next;
				delete this->FirstNode;
				this->FirstNode = IterNode;
			}

			this->FirstNode = nullptr;
			this->LastNode = nullptr;
			this->Length = 0;
		}

		//! @brief Moves other list here.
		//! @param OtherList list would be moved into current object.
		//! @returns this.
		GDINL DoubleLinkedList& operator=(DoubleLinkedList&& OtherList)
		{
			if (&OtherList != this)
			{
				this->Clear();
				this->FirstNode = OtherList.FirstNode;
				this->LastNode = OtherList.LastNode;
				this->Length = OtherList.Length;

				OtherList.FirstNode = nullptr;
				OtherList.LastNode = nullptr;
				OtherList.Length = 0;
			}

			return *this;
		}
	}; // class DoubleLinkedList
#endif

GD_NAMESPACE_END
