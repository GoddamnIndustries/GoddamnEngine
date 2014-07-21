/// ==========================================================================================
/// LockFreeStack.h - lock free stack FIFO containter generic interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 26.06.2014 - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_CONTAINERS_LOCKFREE_LOCKFREE_STACK
#define GD_CORE_CONTAINERS_LOCKFREE_LOCKFREE_STACK

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/Pointer/SharedPtr.h>
#include <memory>
#include <atomic>

/// Define this constant to use non-generic lock free stack impelementation on windows platform.
/// Native WinAPI implementation is bit faster than generic one. 
// #define GD_LOCKFREE_STACK_USE_WINAPI_IMPL 1

#if (defined(GD_LOCKFREE_STACK_USE_WINAPI_IMPL) && !(defined(GD_PLATFORM_WINDOWS) || defined(GD_PLATFORM_WINRT) || defined(GD_PLATFORM_WP8)) || defined(GD_DOCUMENTATION))
# 	 undef   GD_LOCKFREE_STACK_USE_WINAPI_IMPL	// Undefining this constant because custom impl. is not supported there.
#endif	// if (defined(GD_LOCKFREE_STACK_USE_WINAPI_IMPL) && !(defined(GD_PLATFORM_WINDOWS) || defined(GD_PLATFORM_WINRT) || defined(GD_PLATFORM_WP8)) || defined(GD_DOCUMENTATION))

GD_NAMESPACE_BEGIN

	template<typename ElementType>
	class LockFreeStack final
	{
	private:
#if (defined(GD_LOCKFREE_STACK_USE_WINAPI_IMPL))
#else	// if (defined(GD_LOCKFREE_STACK_USE_WINAPI_IMPL))
		struct Node;
		struct CountedNodePtr final
		{
			Int32 ExternalCount = 0;
			Node* Pointer = nullptr;
		};	// struct CountedNodePtr

		struct Node final
		{
			SharedPtr<ElementType> Data;
			Atomic<Int32>          InternalCount;
			CountedNodePtr Next;
			
			GDINL Node(ElementType const& Data)
				: Data(SharedPtr<ElementType>(Data))
				, InternalCount(0)
			{
			}
		};	// struct Node

		std::atomic<CountedNodePtr> Head;

		inline void IncreaseHeadCount(CountedNodePtr& OldCounter)
		{
			CountedNodePtr NewCounter;
			do
			{
				++(NewCounter = OldCounter).ExternalCount;
			} while (!self->Head.compare_exchange_strong(OldCounter, NewCounter, std::memory_order_acquire, std::memory_order_relaxed));
			OldCounter.ExternalCount = NewCounter.ExternalCount;
		}

	public:
		GDINL  LockFreeStack() = default;
		GDINL ~LockFreeStack() { while (self->Pop() != nullptr); }

		inline void Push(ElementType const& Data)
		{
			CountedNodePtr NewNode;
			NewNode.Pointer = new Node(Data);
			NewNode.ExternalCount = 1;
			NewNode.Pointer->Next = self->Head.load(std::memory_order_relaxed);
			while (!self->Head.compare_exchange_weak(NewNode.Pointer->Next, NewNode, std::memory_order_release, std::memory_order_relaxed));
		}

		inline SharedPtr<ElementType> Pop()
		{
			CountedNodePtr OldNode = self->Head.load(std::memory_order_relaxed);
			for (;;)
			{
				IncreaseHeadCount(OldNode);
				Node* const Pointer = OldNode.Pointer;
				if (!Pointer)
					return SharedPtr<ElementType>();
				
				if (self->Head.compare_exchange_strong(OldNode, Pointer->Next, std::memory_order_relaxed))
				{
					SharedPtr<ElementType> Result(Pointer->Data);
					Int32 const CountIncrease = OldNode.ExternalCount - 2;
					if (Pointer->InternalCount.fetch_add(CountIncrease, std::memory_order_release) == -CountIncrease)
						delete Pointer;

					return Result;
				}
				else if (Pointer->InternalCount.fetch_add(-1, std::memory_order_relaxed) == 1)
				{
					Pointer->InternalCount.load(std::memory_order_acquire);
					delete Pointer;
					return SharedPtr<ElementType>();
				}
			}
		}
#endif	// if (defined(GD_LOCKFREE_STACK_USE_WINAPI_IMPL))
	};	// class LockFreeStack

GD_NAMESPACE_END

#endif
