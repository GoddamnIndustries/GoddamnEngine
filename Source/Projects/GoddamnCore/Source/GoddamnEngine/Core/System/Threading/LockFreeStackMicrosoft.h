// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/System/Threading/LockFreeStackMicrosoft.h
 * File contains Microsoft Lock-Free stack implementation.
 */
#pragma once
#if !defined(GD_INSIDE_LOCKFREESTACK_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Core/System/Threading/LockFreeStack.h> instead.
#endif	// if !defined(GD_INSIDE_LOCKFREESTACK_H)

#include <Windows.h>
#include <GoddamnEngine/Core/Algorithm.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Microsoft Lock-Free stack class.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	template<typename TElement>
	class LockFreeStackMicrosoft : public LockFreeStackGeneric<TElement>
	{
	private:
		PSLIST_HEADER m_StackHeader;
		struct Node
		{
			SLIST_ENTRY m_StackEntry;
			TElement    m_Element;
		};	// struct Node

	public:

		/*!
		 * Initializes an empty lock-free stack.
		 */
		GDINL LockFreeStackMicrosoft()
		{
			m_StackHeader = static_cast<PSLIST_HEADER>(GD_MALLOC_ALIGNED(sizeof SLIST_HEADER, MEMORY_ALLOCATION_ALIGNMENT));
			InitializeSListHead(m_StackHeader);
		}

		GDINL ~LockFreeStackMicrosoft()
		{
			Clear();
			GD_DEALLOC(m_StackHeader);
		}

	public:

		/*!
		 * Appends new element to the lock-free stack.
		 * @param newElement New element that would be inserted into the end of container.
		 */
		//! @{
		GDINL void PushBack(TElement&& newElement = TElement())
		{
			auto const node = static_cast<Node*>(GD_MALLOC_ALIGNED(sizeof Node, MEMORY_ALLOCATION_ALIGNMENT));
			InitializeIterator(&node->m_Element, Forward<TElement>(newElement));
			InterlockedPushEntrySList(m_StackHeader, &node->m_StackEntry);
		}
		GDINL void PushBack(TElement const& newElement)
		{
			auto const node = static_cast<Node*>(GD_MALLOC_ALIGNED(sizeof Node, MEMORY_ALLOCATION_ALIGNMENT));
			InitializeIterator(&node->m_Element, newElement);
			InterlockedPushEntrySList(m_StackHeader, &node->m_StackEntry);
		}
		//! @}

		/*!
		 * Removes last element from the lock-free stack.
		 *
		 * @param outElement Reference for the output.
		 * @returns False if stack is empty.
		 */
		GDINL bool PopBack(TElement& outElement)
		{
			auto const node = reinterpret_cast<Node*>(InterlockedPopEntrySList(m_StackHeader));
			if (node != nullptr)
			{
				outElement = node->m_Element;
				DeinitializeIterator(&node->m_Element);
				GD_DEALLOC(node);
				return true;
			}
			return false;
		}

		/*!
		 * Destroys all elements in the stack.
		 */
		// ReSharper disable once CppMemberFunctionMayBeConst
		GDINL void Clear()
		{
			for (;;)
			{
				auto const node = reinterpret_cast<Node*>(InterlockedPopEntrySList(m_StackHeader));
				if (node == nullptr)
				{
					break;
				}
				DeinitializeIterator(&node->m_Element);
			}
		}

	};	// class LockFreeStackMicrosoft

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Cross-platform Lock-Free stack class.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	template<typename TElement>
	using LockFreeStack = LockFreeStackMicrosoft<TElement>;

GD_NAMESPACE_END
