/// ==========================================================================================
/// LockFreeList.h - Lock-free list class.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_CONTAINERS_LOCKFREE_LOCKFREE
#define GD_CORE_CONTAINERS_LOCKFREE_LOCKFREE

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/Utility.h>
#include <GoddamnEngine/Core/Containers/Containers.h>

#if (defined(GD_PLATFORM_WINDOWS))

#include <Windows.h>

GD_NAMESPACE_BEGIN

	/// @brief Defines a single node of a single-linked list.
	/// @note We are using Windows default SList data structure. It seams to be faster than more generic implementations.
	class LockFreeListBaseNode
	{
	private:
		GD_CLASS_UNASSIGNABLE(LockFreeListBaseNode);
		GD_CLASS_UNCOPIABLE(LockFreeListBaseNode);

		friend class LockFreeListBase;

		union {
			static_assert((static_cast<PSLIST_ENTRY*>(nullptr) - &static_cast<PSLIST_ENTRY>(nullptr)->Next) == 0, "'SLIST_ENTRY' internal data structure modified, please update this code.");
			LockFreeListBaseNode* Next;
			SLIST_ENTRY Entry;
		};	// anonymous union
		handle Element;

	protected:

		/// @brief Node aligned allocation/deallocation operators.
		/// @todo This should be rewritten: used _aligned_malloc/_aligned_free but not engine's allocator.
		/// @{
		GDINL void* operator new(size_t const Size)
		{
			if (Size != 0) {
				void* const Memory = ::_aligned_malloc(Size, MEMORY_ALLOCATION_ALIGNMENT);
				GD_ASSERT(Memory != nullptr, "Failed to allocate memory");
				return Memory;
			} else {
				return nullptr;
			}
		}
		GDINL void operator delete(void* const Memory)
		{
			if (Memory != nullptr) {
				::_aligned_free(Memory);
			}
		}
		/// @}

	public:
		/// @brief Initializes a tree node.
		/// @param Element The Element data pointer.
		GDINL LockFreeListBaseNode(handle const Element)
			: Element(Element)
		{
			this->Next = nullptr;
		}

		/// @brief Returns Element data pointer.
		/// @returns Element data pointer.
		/// @{
		GDINL chandle GetElement() const
		{
			return this->Element;
		}
		GDINL handle GetElement()
		{
			return const_cast<handle>(const_cast<LockFreeListBaseNode const*>(this)->GetElement());
		}
		/// @}

	};	// class LockFreeListBase

	/// @brief Single-linked lock-free list.
	/// @note We are using Windows default SList data structure. It seams to be faster than more generic implementations.
	/// @see http://msdn.microsoft.com/en-us/library/windows/desktop/ms686962(v=vs.85).aspx
	class LockFreeListBase
	{
	private:
		GD_CLASS_UNCOPIABLE(LockFreeListBase);

		PSLIST_HEADER Header = nullptr;
		LockFreeListBaseNode* FirstNode = nullptr;

	public:
		/// @brief Initializes a new Lock-Free List.
		GDINL LockFreeListBase()
		{
			this->Header = reinterpret_cast<PSLIST_HEADER>(::_aligned_malloc(sizeof(SLIST_HEADER), MEMORY_ALLOCATION_ALIGNMENT));
			GD_ASSERT(this->Header != nullptr, "Failed to allocate Lock-free list header.");
			::InitializeSListHead(this->Header);
		}

		/// @brief Moves other Lock-Free List here.
		/// @param Other Other list to move here.
		GDINL LockFreeListBase(LockFreeListBase&& Other)
		{
			GD_NOT_USED(Other);
			GD_NOT_IMPLEMENTED();
		}

		/// @brief Deinitializes a Lock-Free List and destroys all internal data.
		GDINL ~LockFreeListBase()
		{
			this->Clear();
			::_aligned_free(this->Header);
		}

	public:

		/// @brief Returns pointer on the first node of the list.
		/// @returns Pointer on the first node of the list.
		/// @{
		GDINL LockFreeListBaseNode const* GetFirstNode() const
		{
			//return reinterpret_cast<LockFreeListBaseNode const*>(::RtlFirstEntrySList(this->Header));
			MemoryBarrier();
			return this->FirstNode;
		}

		GDINL LockFreeListBaseNode* GetFirstNode()
		{
			return const_cast<LockFreeListBaseNode*>(const_cast<LockFreeListBase const*>(this)->GetFirstNode());
		}
		/// @}


		/// @brief Returns number of elements that exist in list.
		/// @returns Number of elements that exist in list.
		GDINL size_t GetLength() const
		{
			return static_cast<size_t>(::QueryDepthSList(this->Header));
		}

		/// @brief Returns true if this list is empty.
		/// @returns True if this list is empty, false otherwise.
		GDINL bool IsEmpty() const
		{
			return ::QueryDepthSList(this->Header) == 0;
		}

		/// @brief Inserts the node to the front of the list.
		/// @param Node Node that would be inserted.
		GDINL void InsertNodeFirst(LockFreeListBaseNode* const Node)
		{
			::MemoryBarrier();
			::InterlockedCompareExchangePointer(reinterpret_cast<PVOID volatile*>(this->FirstNode), Node, nullptr);
			::InterlockedPushEntrySList(this->Header, &Node->Entry);
		}
		
		/// @brief Removes last node of the list.
		GDINL void RemoveLastNode()
		{
			::InterlockedPopEntrySList(this->Header);
		}

		/// @brief Destroys all nodes of the list.
		GDINL void Clear()
		{
			::InterlockedFlushSList(this->Header);
		}

		/// @brief Moves other list here.
		/// @param OtherList list would be moved into current object.
		/// @returns this.
		GDINL LockFreeListBase& operator= (LockFreeListBase&& OtherList)
		{
			GD_NOT_USED(OtherList);
			GD_NOT_IMPLEMENTED();
		}
	};	// class LockFreeListBase

GD_NAMESPACE_END

#else	// if (defined(GD_PLATFORM_WINDOWS))

/// @todo Provide generic implementation of the LockFreeList.
#	error LockFreeList is not implemented for this platform.

#endif	// if (defined(GD_PLATFORM_WINDOWS))

GD_NAMESPACE_BEGIN

	

GD_NAMESPACE_END

#endif	// ifndef GD_CORE_CONTAINERS_LOCKFREE_LOCKFREE
