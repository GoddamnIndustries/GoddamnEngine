//////////////////////////////////////////////////////////////////////////
/// LockFreeList.h - Lock-free lst interface.
/// Copyright (C) $(GD_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 21.05.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_CORE_CONTAINERS_LOCKFREE
#define GD_CORE_CONTAINERS_LOCKFREE

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/Containers.h>
#include <GoddamnEngine/Core/Threading/Atomic/Atomic.h>

#include <atomic>

GD_NAMESPACE_BEGIN

	template<typename LockFreeListNodeType> using AtomicPtr = std::atomic<LockFreeListNodeType*>;
	template<typename LockFreeListNodeType> class LockFreeList;

	/// Lock free list abstract node.
	/// Each impemented node should be derived from this class.
	template<typename NodeType>
	struct LockFreeListNode
	{
	private:
		GD_CLASS_UNASSIGNABLE(LockFreeListNode);
		GD_CLASS_UNCOPIABLE  (LockFreeListNode);
		friend class LockFreeList<NodeType>;
		NodeType*     NextNode = nullptr;
		NodeType* PreviousNode = nullptr;

	protected:
		GDINL explicit LockFreeListNode() { }
		GDINL virtual ~LockFreeListNode() { }

	public:
		GDINL NodeType const* GetNextSiblingNode    () const { return self->NextNode; }
		GDINL NodeType      * GetNextSiblingNode    ()       { return self->NextNode; }
		GDINL NodeType const* GetPreviousSiblingNode() const { return self->PreviousNode; }
		GDINL NodeType      * GetPreviousSiblingNode()       { return self->PreviousNode; }
	};	// struct LockFreeListNode

	template<typename NodeType>
	class LockFreeList
	{
	public:
		typedef LockFreeListNode<NodeType> LockFreeListNodeType;

	private:
		AtomicPtr<LockFreeListNodeType> HeadNode;
		AtomicPtr<LockFreeListNodeType> TailNode;
	};	// class LockFreeList

GD_NAMESPACE_END

#endif
