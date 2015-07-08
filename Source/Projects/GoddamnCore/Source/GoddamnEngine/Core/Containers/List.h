// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Containers/List.h
//! Dynamic list class.
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/Utility.h>
#include <GoddamnEngine/Core/Containers/Iterators.h>
#include "ICCIRef.h"

GD_NAMESPACE_BEGIN

	//! Defines a single node of a single-linked list.
	//! @tparam ElementType Container element type.
	template<typename ElementType>
	class LinkedListNode
	{
		template<typename SomeElementType>
		friend class LinkedList;

	private:
		LinkedListNode* Next;
		ElementType Element;

	public:

		//! Initializes a single node of a double-linked container.
		//! @param Element Element that would be stored in this node.
		//! @{
		GDINL LinkedListNode(ElementType&& Element = ElementType())
			: Element(Forward<ElementType>(Element))
		{
		}
		GDINL LinkedListNode(ElementType const& Element)
			: Element(Element)
		{
		}
		//! @}

		//! Returns next node to this.
		//! @returns Next node to this.
		//! @{
		GDINL LinkedListNode const* GetNextNode() const
		{
			return this->Next;
		}
		GDINL LinkedListNode* GetNextNode()
		{
			return const_cast<LinkedListNode*>(const_cast<LinkedListNode const*>(this)->GetNextNode());
		}
		//! @}

		//! Returns Element data reference.
		//! @returns Element data reference.
		//! @{
		GDINL ElementType const& GetElement() const
		{
			return this->Element;
		}
		GDINL ElementType& GetElement()
		{
			return const_cast<ElementType&>(const_cast<LinkedListNode const*>(this)->GetElement());
		}
		//! @}

	};	// class DoubleLinkedListNode

	//! Single-linked list iterator type.
	//! @tparam	LinkedListNodeType Single linked list node type.
	template<typename LinkedListNodeType>
	struct LinkedListIterator
	{
	private:
		LinkedListNodeType* IterNode;

	public:
		//! Initializes iterator that points on the specified node of the list.
		//! @param IterNode Some node of the list.
		GDINL explicit LinkedListIterator(LinkedListNodeType* IterNode)
			: IterNode(IterNode)
		{
		}

		//! Assigns the iterator next node of the list.
		//! @returns Incremented iterator.
		GDINL LinkedListIterator& operator++ ()
		{
			this->IterNode = this->IterNode->GetNextNode();
			return *this;
		}

		//! Assigns the iterator next node of the list.
		//! @param Unused Unused parameter passed be compiler.
		//! @returns Iterator before incrementing.
		GDINL LinkedListIterator operator++ (int const Unused)
		{
			GD_NOT_USED(Unused);
			LinkedListIterator Copy(*this);
			this->IterNode = this->IterNode->GetNextNode();
			return Copy;
		}

		//! Compares two iterators on equality.
		//! @param Other Other iterator that would be compared.
		//! @returns True if iterators point to the same container at same indices, false otherwise.
		GDINL bool operator== (LinkedListIterator const& Other) const
		{
			return this->IterNode == Other.IterNode;
		}

		//! Compares two iterators on inequality.
		//! @param Other Other iterator that would be compared.
		//! @returns False if iterators point to the same container at same indices, true otherwise.
		GDINL bool operator!= (LinkedListIterator const& Other) const
		{
			return this->IterNode != Other.IterNode;
		}

		//! Dereferences value of the iterator.
		//! @returns Reference to the element of the node of the list.
		GDINL decltype(DeclValue<LinkedListNodeType>().GetElement()) operator* () const
		{
			return this->IterNode->GetElement();
		}

		//! Dereferences value of the iterator.
		//! @returns Reference to the element of the node of the list.
		GDINL decltype(DeclValue<LinkedListNodeType>().GetElement()) operator-> () const
		{
			return this->IterNode->GetElement();
		}
	};	// class LinkedListIterator

	//! Single-linked template list class.
	//! @tparam ElementType List element type.
	template<typename ElementType>
	class LinkedList final : public IContainer, IUncopiable
		, public ContainerIteratableTag
	{
	public:
		typedef LinkedListNode<ElementType> LinkedListNodeType;

		typedef LinkedListIterator<LinkedListNodeType> Iterator;
		typedef LinkedListIterator<LinkedListNodeType const> ConstIterator;
		GD_CONTAINER_CHECK_ITERATORS(LinkedList);

		GD_CONTAINER_DEFINE_ITERATION_SUPPORT(LinkedList);

	private:
		LinkedListNodeType* FirstNode = nullptr;
		SizeTp Length = 0;

	public:

		//! Initializes an empty list.
		GDINL LinkedList()
		{
		}

		//! Moves other vector here.
		//! @param OtherVector Vector would be moved into current object.
		GDINL LinkedList(LinkedList&& Other)
		{
			this->FirstNode = Other.FirstNode;
			this->Length = Other.Length;
			Other.FirstNode = nullptr;
			Other.Length = 0;
		}

		//! Deinitializes all nodes and deallocates memory.
		GDINL ~LinkedList()
		{
			this->Clear();
		}

	public:

		//! Returns iterator that points to first container element.
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

		//! Returns iterator that points to past the end container element.
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

	public:

		//! Returns pointer on the first node of the list.
		//! @returns Pointer on the first node of the list.
		//! @{
		GDINL LinkedListNodeType const* GetFirstNode() const
		{
			return this->FirstNode;
		}
		GDINL LinkedListNodeType* GetFirstNode()
		{
			return const_cast<LinkedListNodeType*>(const_cast<LinkedList const*>(this)->GetFirstNode());
		}
		//! @}

		//! Returns number of elements that exist in list.
		//! @returns Number of elements that exist in list.
		GDINL SizeTp GetLength() const
		{
			return this->Length;
		}

		//! Returns true if this list is empty.
		//! @returns True if this list is empty, false otherwise.
		GDINL bool IsEmpty() const
		{
			return this->Length == 0;
		}

		//! Inserts the node to the front of the list.
		//! @param Node Node that would be inserted.
		GDINL void InsertNodeFirst(LinkedListNodeType* const Node)
		{
			GD_DEBUG_ASSERT(Node != nullptr, "Null pointer node specified.");
			if (this->FirstNode == nullptr)
			{
				this->FirstNode = Node;
			}
			else
			{
				Node->Next = this->FirstNode;
				this->FirstNode = Node;
			}

			this->Length += 1;
		}

		//! Inserts the element to the front of the list.
		//! @param Element Element that would be inserted.
		//! @{
		GDINL void InsertFirst(ElementType&& Element = ElementType())
		{
			this->InsertNodeFirst(GD_NEW(LinkedListNodeType, Forward<ElementType>(Element)));
		}
		GDINL void InsertFirst(ElementType const& Element)
		{
			this->InsertNodeFirst(GD_NEW(LinkedListNodeType, Element));
		}
		//! @}

		//! Inserts the node after the specified node.
		//! @param Node Node that would be inserted.
		//! @param After Node after that new one would be inserted. By default, it is before first node.
		GDINL void InsertNodeAfter(LinkedListNodeType* const Node, LinkedListNodeType* const After = nullptr)
		{
			GD_DEBUG_ASSERT(Node != nullptr, "Null pointer node specified.");
			if (After == nullptr)
			{
				this->InsertNodeFirst(Node);
			}
			else
			{
				Node->Next = After->Next;
				After->Next = Node;

				this->Length += 1;
			}
		}

		//! Inserts the element after the specified node.
		//! @param Element Element that would be inserted.
		//! @param After Node after that new one would be inserted. By default, it is last node.
		//! @{
		GDINL void InsertAfter(ElementType&& Element = ElementType(), LinkedListNodeType* const After = nullptr)
		{
			this->InsertNodeAfter(GD_NEW(LinkedListNodeType, Forward<ElementType>(Element), After));
		}
		GDINL void InsertAfter(ElementType const& Element, LinkedListNodeType* const After = nullptr)
		{
			this->InsertNodeAfter(GD_NEW(LinkedListNodeType, Element, After));
		}
		//! @}

		//! Removes the node from the list.
		//! @param Node	Node that would be RemoveFromSelfd from the list.
		GDINL void RemoveNode(LinkedListNodeType* const Node)
		{
			GD_DEBUG_ASSERT(Node != nullptr, "Null pointer node specified.");

			//! @todo Check if the node is related to list...
			if (this->FirstNode == this->LastNode)
			{
				GD_DELETE(Node);
				this->FirstNode = nullptr;
				this->Length = 0;
			}
			else
			{
				if (this->FirstNode == Node)
				{
					this->FirstNode = this->FirstNode->Next;
				}
				else if (this->LastNode == Node)
				{
					this->LastNode = this->LastNode->Prev;
					this->LastNode->Next = nullptr;
				}
				else
				{
					Node->Prev->Next = Node->Next;
				}

				GD_DELETE(Node);
				this->Length -= 1;
			}
		}

		//! Removes first node of the list.
		GDINL void RemoveFirstNode()
		{
			this->RemoveNode(this->FirstNode);
		}

		//! Removes last node of the list.
		GDINL void RemoveLastNode()
		{
			this->RemoveNode(this->LastNode);
		}

		//! Destroys all nodes of the list.
		GDINL void Clear()
		{
			LinkedListNodeType* IterNode;
			while (this->FirstNode != nullptr)
			{
				IterNode = this->FirstNode->Next;
				GD_DELETE(this->FirstNode);
				this->FirstNode = IterNode;
			}

			this->Length = 0;
			this->FirstNode = nullptr;
		}

		//! Moves other list here.
		//! @param OtherList list would be moved into current object.
		//! @returns this.
		GDINL LinkedList& operator= (LinkedList&& OtherList)
		{
			if (&OtherList != this)
			{
				this->Clear();
				this->FirstNode = OtherList.FirstNode;
				this->Length = OtherList.Length;

				OtherList.FirstNode = nullptr;
				OtherList.Length = 0;
			}

			return *this;
		}
	};	// class LinkedList

	//! Defines a single node of a double-linked container.
	//! @tparam ElementType Container element type.
	template<typename ElementType>
	class DoubleLinkedListNode : public IUncopiable
	{
		template<typename SomeElementType>
		friend class DoubleLinkedList;

	private:
		DoubleLinkedListNode* Next;
		DoubleLinkedListNode* Prev;
		ElementType Element;

	public:

		//! Initializes a single node of a double-linked container.
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

		//! Returns next node to this.
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

		//! Returns previous node to this.
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

		//! Returns Element data reference.
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

	};	// class DoubleLinkedListNode

	//! Double-linked list iterator type.
	//! @tparam	DoubleLinkedListNodeType Double linked list node type.
	template<typename DoubleLinkedListNodeType>
	struct DoubleLinkedListIterator
	{
	private:
		DoubleLinkedListNodeType* IterNode;

	public:
		//! Initializes iterator that points on the specified node of the list.
		//! @param IterNode Some node of the list.
		GDINL explicit DoubleLinkedListIterator(DoubleLinkedListNodeType* IterNode)
			: IterNode(IterNode)
		{
		}

		//! Assigns the iterator next node of the list.
		//! @returns Incremented iterator.
		GDINL DoubleLinkedListIterator& operator++ ()
		{
			this->IterNode = this->IterNode->GetNextNode();
			return *this;
		}

		//! Assigns the iterator next node of the list.
		//! @param Unused Unused parameter passed be compiler.
		//! @returns Iterator before incrementing.
		GDINL DoubleLinkedListIterator operator++ (int const Unused)
		{
			GD_NOT_USED(Unused);
			DoubleLinkedListIterator Copy(*this);
			this->IterNode = this->IterNode->GetNextNode();
			return Copy;
		}

		//! Assigns the iterator previous node of the list.
		//! @returns Decremented iterator.
		GDINL DoubleLinkedListIterator& operator-- ()
		{
			this->IterNode = this->IterNode->GetPrevNode();
			return *this;
		}

		//! Assigns the iterator previous node of the list.
		//! @param Unused Unused parameter passed be compiler.
		//! @returns Iterator before decrementing.
		GDINL DoubleLinkedListIterator operator-- (int const Unused)
		{
			GD_NOT_USED(Unused);
			DoubleLinkedListIterator Copy(*this);
			this->IterNode = this->IterNode->GetPrevNode();
			return Copy;
		}

		//! Compares two iterators on equality.
		//! @param Other Other iterator that would be compared.
		//! @returns True if iterators point to the same container at same indices, false otherwise.
		GDINL bool operator== (DoubleLinkedListIterator const& Other) const
		{
			return this->IterNode == Other.IterNode;
		}

		//! Compares two iterators on inequality.
		//! @param Other Other iterator that would be compared.
		//! @returns False if iterators point to the same container at same indices, true otherwise.
		GDINL bool operator!= (DoubleLinkedListIterator const& Other) const
		{
			return this->IterNode != Other.IterNode;
		}

		//! Dereferences value of the iterator.
		//! @returns Reference to the element of the node of the list.
		GDINL decltype(DeclValue<DoubleLinkedListNodeType>().GetElement()) operator* () const
		{
			return this->IterNode->GetElement();
		}

		//! Dereferences value of the iterator.
		//! @returns Reference to the element of the node of the list.
		GDINL decltype(DeclValue<DoubleLinkedListNodeType>().GetElement()) operator-> () const
		{
			return this->IterNode->GetElement();
		}
	};	// class DoubleLinkedListIterator

	//! Double-linked template list class.
	//! @tparam ElementType Container element type.
	template<typename ElementType>
	class DoubleLinkedList final : public IContainer, IUncopiable
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
		DoubleLinkedListNodeType* FirstNode = nullptr;
		DoubleLinkedListNodeType* LastNode = nullptr;
		SizeTp Length = 0;

	public:

		//! Initializes an empty list.
		GDINL DoubleLinkedList()
		{
		}

		//! Moves other vector here.
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

		//! Deinitializes all nodes and deallocates memory.
		GDINL ~DoubleLinkedList()
		{
			this->Clear();
		}

	public:

		//! Returns iterator that points to first container element.
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

		//! Returns iterator that points to past the end container element.
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

		//! Returns iterator that points to last container element.
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

		//! Returns iterator that points to preceding the first container element.
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

		//! Returns pointer on the first node of the list.
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

		//! Returns pointer on the last node of the list.
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

		//! Returns number of elements that exist in list.
		//! @returns Number of elements that exist in list.
		GDINL SizeTp GetLength() const
		{
			return this->Length;
		}

		//! Returns true if this list is empty.
		//! @returns True if this list is empty, false otherwise.
		GDINL bool IsEmpty() const
		{
			return this->Length == 0;
		}

		//! Inserts the node to the front of the list.
		//! @param Node Node that would be inserted.
		GDINL void InsertNodeFirst(DoubleLinkedListNodeType* const Node)
		{
			GD_DEBUG_ASSERT(Node != nullptr, "Null pointer node specified.");
			if (this->FirstNode == nullptr)
			{
				this->FirstNode = Node;
				this->LastNode = Node;
			}
			else
			{
				Node->Next = this->FirstNode;
				this->FirstNode = Node;
			}

			this->Length += 1;
		}

		//! Inserts the element to the front of the list.
		//! @param Element Element that would be inserted.
		//! @{
		GDINL void InsertFirst(ElementType&& Element = ElementType())
		{
			this->InsertNodeFirst(GD_NEW(DoubleLinkedListNodeType, Forward<ElementType>(Element)));
		}
		GDINL void InsertFirst(ElementType const& Element)
		{
			this->InsertNodeFirst(GD_NEW(DoubleLinkedListNodeType, Element));
		}
		//! @}

		//! Inserts the node to the end of the list.
		//! @param Node Node that would be inserted.
		GDINL void InsertNodeLast(DoubleLinkedListNodeType* const Node)
		{
			GD_DEBUG_ASSERT(Node != nullptr, "Null pointer node specified.");
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

		//! Inserts the element to the end of the list.
		//! @param Element Element that would be inserted.
		//! @{
		GDINL void InsertLast(ElementType&& Element = ElementType())
		{
			this->InsertNodeLast(GD_NEW(DoubleLinkedListNodeType, Forward<ElementType>(Element)));
		}
		GDINL void InsertLast(ElementType const& Element)
		{
			this->InsertNodeLast(GD_NEW(DoubleLinkedListNodeType, Element));
		}
		//! @}

		//! Inserts the node after the specified node.
		//! @param Node Node that would be inserted.
		//! @param After Node after that new one would be inserted. By default, it is last node.
		GDINL void InsertNodeAfter(DoubleLinkedListNodeType* const Node, DoubleLinkedListNodeType* const After = nullptr)
		{
			GD_DEBUG_ASSERT(Node != nullptr, "Null pointer node specified.");
			if (After == nullptr)
			{
				this->InsertNodeLast(Node);
			}
			else
			{
				Node->Next = After->Next;
				Node->Prev = After;
				After->Next->Prev = Node;
				After->Next = Node;

				this->Length += 1;
			}
		}

		//! Inserts the element after the specified node.
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

		//! Removes the node from the list.
		//! @param Node	Node that would be RemoveFromSelfd from the list.
		GDINL void RemoveNode(DoubleLinkedListNodeType* const Node)
		{
			GD_DEBUG_ASSERT(Node != nullptr, "Null pointer node specified.");

			//! @todo Check if the node is related to list...
			if (this->FirstNode == this->LastNode)
			{
				GD_DELETE(Node);
				this->FirstNode = nullptr;
				this->LastNode = nullptr;
				this->Length = 0;
			}
			else
			{
				if (this->FirstNode == Node)
				{
					this->FirstNode = this->FirstNode->Next;
					this->FirstNode->Prev = nullptr;
				}
				else if (this->LastNode == Node)
				{
					this->LastNode = this->LastNode->Prev;
					this->LastNode->Next = nullptr;
				}
				else
				{
					Node->Next->Prev = Node->Prev;
					Node->Prev->Next = Node->Next;
				}

				GD_DELETE(Node);
				this->Length -= 1;
			}
		}

		//! Removes first node of the list.
		GDINL void RemoveFirstNode()
		{
			this->RemoveNode(this->FirstNode);
		}

		//! Removes last node of the list.
		GDINL void RemoveLastNode()
		{
			this->RemoveNode(this->LastNode);
		}

		//! Destroys all nodes of the list.
		GDINL void Clear()
		{
			DoubleLinkedListNodeType* IterNode;
			while (this->FirstNode != nullptr)
			{
				IterNode = this->FirstNode->Next;
				GD_DELETE(this->FirstNode);
				this->FirstNode = IterNode;
			}

			this->FirstNode = nullptr;
			this->LastNode = nullptr;
			this->Length = 0;
		}

		//! Moves other list here.
		//! @param OtherList list would be moved into current object.
		//! @returns this.
		GDINL DoubleLinkedList& operator= (DoubleLinkedList&& OtherList)
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
	};	// class DoubleLinkedList

	//! Simplest-possible single-linked template list class.
	//! @tparam ElementType Container element type.
	template<typename ElementType>
	class List final : IUncopiable
	{
	};	// class List

GD_NAMESPACE_END
