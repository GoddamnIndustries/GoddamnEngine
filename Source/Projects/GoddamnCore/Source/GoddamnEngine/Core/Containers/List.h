/// ==========================================================================================
/// List.h - dynamic list class.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_CONTAINERS_LIST
#define GD_CORE_CONTAINERS_LIST

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/Utility.h>
#include <GoddamnEngine/Core/Containers/Containers.h>

GD_NAMESPACE_BEGIN

	/// @brief Defines a single node of a single-linked list.
	/// @tparam ElementType Container element type.
	template<typename ElementType>
	class LinkedListNode
	{
		template<typename SomeElementType>
		friend class LinkedList;

	private:
		LinkedListNode* Next;
		ElementType Element;

	public:

		/// @brief Initializes a single node of a double-linked container.
		/// @param Element Element that would be stored in this node.
		/// @{
		GDINL LinkedListNode(ElementType&& Element = ElementType())
			: Element(Forward<ElementType>(Element))
		{
		}
		GDINL LinkedListNode(ElementType const& Element)
			: Element(Element)
		{
		}
		/// @}

		/// @brief Returns next node to this.
		/// @returns Next node to this.
		/// @{
		GDINL LinkedListNode const* GetNextNode() const
		{
			return this->Next;
		}
		GDINL LinkedListNode* GetNextNode()
		{
			return const_cast<LinkedListNode*>(const_cast<LinkedListNode const*>(this)->GetNextNode());
		}
		/// @}

		/// @brief Returns Element data reference.
		/// @returns Element data reference.
		/// @{
		GDINL ElementType const& GetElement() const
		{
			return this->Element;
		}
		GDINL ElementType& GetElement()
		{
			return const_cast<ElementType&>(const_cast<LinkedListNode const*>(this)->GetElement());
		}
		/// @}
	
	};	// class DoubleLinkedListNode

	/// @brief Single-linked list iterator type.
	/// @tparam	LinkedListNodeType Single linked list node type.
	template<typename LinkedListNodeType>
	struct LinkedListIterator
	{
	private:
		LinkedListNodeType* IterNode;

	public:
		/// @brief Initializes iterator that points on the specified node of the list.
		/// @param IterNode Some node of the list.
		GDINL explicit LinkedListIterator(LinkedListNodeType* IterNode)
			: IterNode(IterNode)
		{
		}

		/// @brief Assigns the iterator next node of the list.
		/// @returns Incremented iterator.
		GDINL LinkedListIterator& operator++ ()
		{
			this->IterNode = this->IterNode->GetNextNode();
			return *this;
		}

		/// @brief Assigns the iterator next node of the list.
		/// @param Unused Unused parameter passed be compiler.
		/// @returns Iterator before incrementing.
		GDINL LinkedListIterator operator++ (int const Unused)
		{
			GD_NOT_USED(Unused);
			LinkedListIterator Copy(*this);
			this->IterNode = this->IterNode->GetNextNode();
			return Copy;
		}

		/// @brief Compares two iterators on equality.
		/// @param Other Other iterator that would be compared.
		/// @returns True if iterators point to the same container at same indices, false otherwise.
		GDINL bool operator== (LinkedListIterator const& Other) const
		{
			return this->IterNode == Other.IterNode;
		}

		/// @brief Compares two iterators on inequality.
		/// @param Other Other iterator that would be compared.
		/// @returns False if iterators point to the same container at same indices, true otherwise.
		GDINL bool operator!= (LinkedListIterator const& Other) const
		{
			return this->IterNode != Other.IterNode;
		}

		/// @brief Dereferences value of the iterator.
		/// @returns Reference to the element of the node of the list.
		GDINL decltype(DeclValue<LinkedListNodeType>().GetElement()) operator* () const
		{
			return this->IterNode->GetElement();
		}

		/// @brief Dereferences value of the iterator.
		/// @returns Reference to the element of the node of the list.
		GDINL decltype(DeclValue<LinkedListNodeType>().GetElement()) operator-> () const
		{
			return this->IterNode->GetElement();
		}
	};	// class LinkedListIterator

	/// @brief Single-linked template list class.
	/// @tparam ElementType List element type.
	template<typename ElementType>
	class LinkedList final : public IContainer
		, public ContainerIteratableTag
	{
	public:
		typedef LinkedListNode<ElementType> LinkedListNodeType;

		typedef LinkedListIterator<LinkedListNodeType> Iterator;
		typedef LinkedListIterator<LinkedListNodeType const> ConstIterator;
		GD_CONTAINER_CHECK_ITERATORS(LinkedList);

		GD_CONTAINER_DEFINE_ITERATION_SUPPORT(LinkedList);

	private:
		GD_CLASS_UNCOPIABLE(LinkedList);
		LinkedListNodeType* FirstNode = nullptr;
		size_t Length = 0;

	public:

		/// @brief Initializes an empty list.
		GDINL LinkedList()
		{
		}

		/// @brief Moves other vector here.
		/// @param OtherVector Vector would be moved into current object.
		GDINL LinkedList(LinkedList&& Other)
		{
			this->FirstNode = Other.FirstNode;
			this->Length = Other.Length;
			Other.FirstNode = nullptr;
			Other.Length = 0;
		}

		/// @brief Deinitializes all nodes and deallocates memory.
		GDINL ~LinkedList()
		{
			this->Clear();
		}

	public:

		/// @brief Returns iterator that points to first container element.
		/// @returns Iterator that points to first container element.
		/// @{
		GDINL Iterator Begin()
		{
			return Iterator(this->FirstNode);
		}
		GDINL ConstIterator Begin() const
		{
			return ConstIterator(this->FirstNode);
		}
		/// @}

		/// @brief Returns iterator that points to past the end container element.
		/// @returns Iterator that points to past the end container element.
		/// @{
		GDINL Iterator End()
		{
			return Iterator(nullptr);
		}
		GDINL ConstIterator End() const
		{
			return ConstIterator(nullptr);
		}
		/// @}

	public:

		/// @brief Returns pointer on the first node of the list.
		/// @returns Pointer on the first node of the list.
		/// @{
		GDINL LinkedListNodeType const* GetFirstNode() const
		{
			return this->FirstNode;
		}
		GDINL LinkedListNodeType* GetFirstNode()
		{
			return const_cast<LinkedListNodeType*>(const_cast<LinkedList const*>(this)->GetFirstNode());
		}
		/// @}

		/// @brief Returns number of elements that exist in list.
		/// @returns Number of elements that exist in list.
		GDINL size_t GetLength() const
		{
			return this->Length;
		}

		/// @brief Returns true if this list is empty.
		/// @returns True if this list is empty, false otherwise.
		GDINL bool IsEmpty() const
		{
			return this->Length == 0;
		}

		/// @brief Inserts the node to the front of the list.
		/// @param Node Node that would be inserted.
		GDINL void InsertNodeFirst(LinkedListNodeType* const Node)
		{
			GD_DEBUG_ASSERT(Node != nullptr, "Null pointer node specified.");
			if (this->FirstNode == nullptr) {
				this->FirstNode = Node;
			} else {
				Node->Next = this->FirstNode;
				this->FirstNode = Node;
			}

			this->Length += 1;
		}

		/// @brief Inserts the element to the front of the list.
		/// @param Element Element that would be inserted.
		/// @{
		GDINL void InsertFirst(ElementType&& Element = ElementType())
		{
			this->InsertNodeFirst(new LinkedListNodeType(Forward<ElementType>(Element)));
		}
		GDINL void InsertFirst(ElementType const& Element)
		{
			this->InsertNodeFirst(new LinkedListNodeType(Element));
		}
		/// @}

		/// @brief Inserts the node after the specified node.
		/// @param Node Node that would be inserted.
		/// @param After Node after that new one would be inserted. By default, it is before first node.
		GDINL void InsertNodeAfter(LinkedListNodeType* const Node, LinkedListNodeType* const After = nullptr)
		{
			GD_DEBUG_ASSERT(Node != nullptr, "Null pointer node specified.");
			if (After == nullptr) {
				this->InsertNodeFirst(Node);
			} else {
				Node->Next = After->Next;
				After->Next = Node;

				this->Length += 1;
			}
		}

		/// @brief Inserts the element after the specified node.
		/// @param Element Element that would be inserted.
		/// @param After Node after that new one would be inserted. By default, it is last node.
		/// @{
		GDINL void InsertAfter(ElementType&& Element = ElementType(), LinkedListNodeType* const After = nullptr)
		{
			this->InsertNodeAfter(new LinkedListNodeType(Forward<ElementType>(Element), After));
		}
		GDINL void InsertAfter(ElementType const& Element, LinkedListNodeType* const After = nullptr)
		{
			this->InsertNodeAfter(new LinkedListNodeType(Element, After));
		}
		/// @}

		/// @brief Removes the node from the list.
		/// @param Node	Node that would be removed from the list.
		GDINL void RemoveNode(LinkedListNodeType* const Node)
		{
			GD_DEBUG_ASSERT(Node != nullptr, "Null pointer node specified.");

			/// @todo Check if the node is related to list...
			if (this->FirstNode == this->LastNode) {
				delete Node;
				this->FirstNode = nullptr;
				this->Length = 0;
			} else {
				if (this->FirstNode == Node) {
					this->FirstNode = this->FirstNode->Next;
				} else if (this->LastNode == Node) {
					this->LastNode = this->LastNode->Prev;
					this->LastNode->Next = nullptr;
				} else {
					Node->Prev->Next = Node->Next;
				}

				delete Node;
				this->Length -= 1;
			}
		}

		/// @brief Removes first node of the list.
		GDINL void RemoveFirstNode()
		{
			this->RemoveNode(this->FirstNode);
		}

		/// @brief Removes last node of the list.
		GDINL void RemoveLastNode()
		{
			this->RemoveNode(this->LastNode);
		}

		/// @brief Destroys all nodes of the list.
		GDINL void Clear()
		{
			LinkedListNodeType* IterNode = this->FirstNode;
			while (this->FirstNode != nullptr) {
				IterNode = this->FirstNode->Next;
				delete this->FirstNode;
				this->FirstNode = IterNode;
			}

			this->Length = 0;
			this->FirstNode = nullptr;
		}

		/// @brief Moves other list here.
		/// @param OtherList list would be moved into current object.
		/// @returns this.
		GDINL LinkedList& operator= (LinkedList&& OtherList)
		{
			if (&OtherList != this) {
				this->Clear();
				this->FirstNode = OtherList.FirstNode;
				this->Length = OtherList.Length;

				OtherList.FirstNode = nullptr;
				OtherList.Length = 0;
			}

			return *this;
		}
	};	// class LinkedList

	/// @brief Defines a single node of a double-linked container.
	/// @tparam ElementType Container element type.
	template<typename ElementType>
	class DoubleLinkedListNode
	{
		template<typename SomeElementType>
		friend class DoubleLinkedList;

	private:
		GD_CLASS_UNCOPIABLE(DoubleLinkedListNode);
		DoubleLinkedListNode* Next;
		DoubleLinkedListNode* Prev;
		ElementType Element;

	public:

		/// @brief Initializes a single node of a double-linked container.
		/// @param Element Element that would be stored in this node.
		/// @{
		GDINL DoubleLinkedListNode(ElementType&& Element = ElementType())
			: Element(Forward<ElementType>(Element))
		{
		}
		GDINL DoubleLinkedListNode(ElementType const& Element)
			: Element(Element)
		{
		}
		/// @}

		/// @brief Returns next node to this.
		/// @returns Next node to this.
		/// @{
		GDINL DoubleLinkedListNode const* GetNextNode() const
		{
			return this->Next;
		}
		GDINL DoubleLinkedListNode* GetNextNode()
		{
			return const_cast<DoubleLinkedListNode*>(const_cast<DoubleLinkedListNode const*>(this)->GetNextNode());
		}
		/// @}

		/// @brief Returns previous node to this.
		/// @returns Next previous to this.
		/// @{
		GDINL DoubleLinkedListNode const* GetPrevNode() const
		{
			return this->Prev;
		}
		GDINL DoubleLinkedListNode* GetPrevNode()
		{
			return const_cast<DoubleLinkedListNode*>(const_cast<DoubleLinkedListNode const*>(this)->GetPrevNode());
		}
		/// @}

		/// @brief Returns Element data reference.
		/// @returns Element data reference.
		/// @{
		GDINL ElementType const& GetElement() const
		{
			return this->Element;
		}
		GDINL ElementType& GetElement()
		{
			return const_cast<ElementType&>(const_cast<DoubleLinkedListNode const*>(this)->GetElement());
		}
		/// @}
	
	};	// class DoubleLinkedListNode

	/// @brief Double-linked list iterator type.
	/// @tparam	DoubleLinkedListNodeType Double linked list node type.
	template<typename DoubleLinkedListNodeType>
	struct DoubleLinkedListIterator
	{
	private:
		DoubleLinkedListNodeType* IterNode;

	public:
		/// @brief Initializes iterator that points on the specified node of the list.
		/// @param IterNode Some node of the list.
		GDINL explicit DoubleLinkedListIterator(DoubleLinkedListNodeType* IterNode)
			: IterNode(IterNode)
		{
		}

		/// @brief Assigns the iterator next node of the list.
		/// @returns Incremented iterator.
		GDINL DoubleLinkedListIterator& operator++ ()
		{
			this->IterNode = this->IterNode->GetNextNode();
			return *this;
		}

		/// @brief Assigns the iterator next node of the list.
		/// @param Unused Unused parameter passed be compiler.
		/// @returns Iterator before incrementing.
		GDINL DoubleLinkedListIterator operator++ (int const Unused)
		{
			GD_NOT_USED(Unused);
			DoubleLinkedListIterator Copy(*this);
			this->IterNode = this->IterNode->GetNextNode();
			return Copy;
		}

		/// @brief Assigns the iterator previous node of the list.
		/// @returns Decremented iterator.
		GDINL DoubleLinkedListIterator& operator-- ()
		{
			this->IterNode = this->IterNode->GetPrevNode();
			return *this;
		}

		/// @brief Assigns the iterator previous node of the list.
		/// @param Unused Unused parameter passed be compiler.
		/// @returns Iterator before decrementing.
		GDINL DoubleLinkedListIterator operator-- (int const Unused)
		{
			GD_NOT_USED(Unused);
			DoubleLinkedListIterator Copy(*this);
			this->IterNode = this->IterNode->GetPrevNode();
			return Copy;
		}

		/// @brief Compares two iterators on equality.
		/// @param Other Other iterator that would be compared.
		/// @returns True if iterators point to the same container at same indices, false otherwise.
		GDINL bool operator== (DoubleLinkedListIterator const& Other) const
		{
			return this->IterNode == Other.IterNode;
		}

		/// @brief Compares two iterators on inequality.
		/// @param Other Other iterator that would be compared.
		/// @returns False if iterators point to the same container at same indices, true otherwise.
		GDINL bool operator!= (DoubleLinkedListIterator const& Other) const
		{
			return this->IterNode != Other.IterNode;
		}

		/// @brief Dereferences value of the iterator.
		/// @returns Reference to the element of the node of the list.
		GDINL decltype(DeclValue<DoubleLinkedListNodeType>().GetElement()) operator* () const
		{
			return this->IterNode->GetElement();
		}

		/// @brief Dereferences value of the iterator.
		/// @returns Reference to the element of the node of the list.
		GDINL decltype(DeclValue<DoubleLinkedListNodeType>().GetElement()) operator-> () const
		{
			return this->IterNode->GetElement();
		}
	};	// class DoubleLinkedListIterator

	/// @brief Double-linked template list class.
	/// @tparam ElementType Container element type.
	template<typename ElementType>
	class DoubleLinkedList final : public IContainer
		, public ContainerIteratableTag
		, public ContainerReverseIteratableTag
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

		/// @brief Initializes an empty list.
		GDINL DoubleLinkedList()
		{
		}

		/// @brief Moves other vector here.
		/// @param OtherVector Vector would be moved into current object.
		GDINL DoubleLinkedList(DoubleLinkedList&& Other)
		{
			this->FirstNode = Other.FirstNode;
			this->LastNode = Other.LastNode;
			this->Length = Other.Length;
			Other.FirstNode = nullptr;
			Other.LastNode = nullptr;
			Other.Length = 0;
		}

		/// @brief Deinitializes all nodes and deallocates memory.
		GDINL ~DoubleLinkedList()
		{
			this->Clear();
		}

	public:

		/// @brief Returns iterator that points to first container element.
		/// @returns Iterator that points to first container element.
		/// @{
		GDINL Iterator Begin()
		{
			return Iterator(this->FirstNode);
		}
		GDINL ConstIterator Begin() const
		{
			return ConstIterator(this->FirstNode);
		}
		/// @}

		/// @brief Returns iterator that points to past the end container element.
		/// @returns Iterator that points to past the end container element.
		/// @{
		GDINL Iterator End()
		{
			return Iterator(nullptr);
		}
		GDINL ConstIterator End() const
		{
			return ConstIterator(nullptr);
		}
		/// @}

		/// @brief Returns iterator that points to last container element.
		/// @returns Iterator that points to last container element.
		/// @{
		GDINL ReverseIterator ReverseBegin()
		{
			return ReverseIterator(this->LastNode);
		}
		GDINL ReverseConstIterator ReverseBegin() const
		{
			return ReverseConstIterator(this->LastNode);
		}
		/// @}

		/// @brief Returns iterator that points to preceding the first container element.
		/// @returns Iterator that points to preceding the first container element.
		/// @{
		GDINL ReverseIterator ReverseEnd()
		{
			return ReverseIterator(nullptr);
		}
		GDINL ReverseConstIterator ReverseEnd() const
		{
			return ReverseConstIterator(nullptr);
		}
		/// @}

	public:

		/// @brief Returns pointer on the first node of the list.
		/// @returns Pointer on the first node of the list.
		/// @{
		GDINL DoubleLinkedListNodeType const* GetFirstNode() const
		{
			return this->FirstNode;
		}
		GDINL DoubleLinkedListNodeType* GetFirstNode()
		{
			return const_cast<DoubleLinkedListNodeType*>(const_cast<DoubleLinkedList const*>(this)->GetFirstNode());
		}
		/// @}

		/// @brief Returns pointer on the last node of the list.
		/// @returns Pointer on the last node of the list.
		/// @{
		GDINL DoubleLinkedListNodeType const* GetLastNode() const
		{
			return this->LastNode;
		}
		GDINL DoubleLinkedListNodeType* GetLastNode()
		{
			return const_cast<DoubleLinkedListNodeType*>(const_cast<DoubleLinkedList const*>(this)->GetLastNode());
		}
		/// @}

		/// @brief Returns number of elements that exist in list.
		/// @returns Number of elements that exist in list.
		GDINL size_t GetLength() const
		{
			return this->Length;
		}

		/// @brief Returns true if this list is empty.
		/// @returns True if this list is empty, false otherwise.
		GDINL bool IsEmpty() const
		{
			return this->Length == 0;
		}

		/// @brief Inserts the node to the front of the list.
		/// @param Node Node that would be inserted.
		GDINL void InsertNodeFirst(DoubleLinkedListNodeType* const Node)
		{
			GD_DEBUG_ASSERT(Node != nullptr, "Null pointer node specified.");
			if (this->FirstNode == nullptr) {
				this->FirstNode = Node;
				this->LastNode = Node;
			} else {
				Node->Next = this->FirstNode;
				this->FirstNode = Node;
			}

			this->Length += 1;
		}

		/// @brief Inserts the element to the front of the list.
		/// @param Element Element that would be inserted.
		/// @{
		GDINL void InsertFirst(ElementType&& Element = ElementType())
		{
			this->InsertNodeFirst(new DoubleLinkedListNodeType(Forward<ElementType>(Element)));
		}
		GDINL void InsertFirst(ElementType const& Element)
		{
			this->InsertNodeFirst(new DoubleLinkedListNodeType(Element));
		}
		/// @}

		/// @brief Inserts the node to the end of the list.
		/// @param Node Node that would be inserted.
		GDINL void InsertNodeLast(DoubleLinkedListNodeType* const Node)
		{
			GD_DEBUG_ASSERT(Node != nullptr, "Null pointer node specified.");
			if (this->LastNode == nullptr) {
				this->LastNode = Node;
				this->FirstNode = Node;
			} else {
				Node->Prev = this->LastNode;
				this->LastNode = Node;
			}

			this->Length += 1;
		}

		/// @brief Inserts the element to the end of the list.
		/// @param Element Element that would be inserted.
		/// @{
		GDINL void InsertLast(ElementType&& Element = ElementType())
		{
			this->InsertNodeLast(new DoubleLinkedListNodeType(Forward<ElementType>(Element)));
		}
		GDINL void InsertLast(ElementType const& Element)
		{
			this->InsertNodeLast(new DoubleLinkedListNodeType(Element));
		}
		/// @}

		/// @brief Inserts the node after the specified node.
		/// @param Node Node that would be inserted.
		/// @param After Node after that new one would be inserted. By default, it is last node.
		GDINL void InsertNodeAfter(DoubleLinkedListNodeType* const Node, DoubleLinkedListNodeType* const After = nullptr)
		{
			GD_DEBUG_ASSERT(Node != nullptr, "Null pointer node specified.");
			if (After == nullptr) {
				this->InsertNodeLast(Node);
			} else {
				Node->Next = After->Next;
				Node->Prev = After;
				After->Next->Prev = Node;
				After->Next = Node;

				this->Length += 1;
			}
		}

		/// @brief Inserts the element after the specified node.
		/// @param Element Element that would be inserted.
		/// @param After Node after that new one would be inserted. By default, it is last node.
		/// @{
		GDINL void InsertAfter(ElementType&& Element = ElementType(), DoubleLinkedListNodeType* const After = nullptr)
		{
			this->InsertNodeAfter(new DoubleLinkedListNodeType(Forward<ElementType>(Element), After));
		}
		GDINL void InsertAfter(ElementType const& Element, DoubleLinkedListNodeType* const After = nullptr)
		{
			this->InsertNodeAfter(new DoubleLinkedListNodeType(Element, After));
		}
		/// @}

		/// @brief Removes the node from the list.
		/// @param Node	Node that would be removed from the list.
		GDINL void RemoveNode(DoubleLinkedListNodeType* const Node)
		{
			GD_DEBUG_ASSERT(Node != nullptr, "Null pointer node specified.");

			/// @todo Check if the node is related to list...
			if (this->FirstNode == this->LastNode) {
				delete Node;
				this->FirstNode = nullptr;
				this->LastNode = nullptr;
				this->Length = 0;
			} else {
				if (this->FirstNode == Node) {
					this->FirstNode = this->FirstNode->Next;
					this->FirstNode->Prev = nullptr;
				} else if (this->LastNode == Node) {
					this->LastNode = this->LastNode->Prev;
					this->LastNode->Next = nullptr;
				} else {
					Node->Next->Prev = Node->Prev;
					Node->Prev->Next = Node->Next;
				}

				delete Node;
				this->Length -= 1;
			}
		}

		/// @brief Removes first node of the list.
		GDINL void RemoveFirstNode()
		{
			this->RemoveNode(this->FirstNode);
		}

		/// @brief Removes last node of the list.
		GDINL void RemoveLastNode()
		{
			this->RemoveNode(this->LastNode);
		}

		/// @brief Destroys all nodes of the list.
		GDINL void Clear()
		{
			DoubleLinkedListNodeType* IterNode = this->FirstNode;
			while (this->FirstNode != nullptr) {
				IterNode = this->FirstNode->Next;
				delete this->FirstNode;
				this->FirstNode = IterNode;
			}

			this->FirstNode = nullptr;
			this->LastNode = nullptr;
			this->Length = 0;
		}

		/// @brief Moves other list here.
		/// @param OtherList list would be moved into current object.
		/// @returns this.
		GDINL DoubleLinkedList& operator= (DoubleLinkedList&& OtherList)
		{
			if (&OtherList != this) {
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
	};	// class DoubleLinkedList

	/// @brief Simplest-possible single-linked template list class.
	/// @tparam ElementType Container element type.
	template<typename ElementType>
	class List final
	{
	private:
		GD_CLASS_UNCOPIABLE(List);

	};	// class List

GD_NAMESPACE_END

#endif	// ifndef GD_CORE_CONTAINERS_LIST
