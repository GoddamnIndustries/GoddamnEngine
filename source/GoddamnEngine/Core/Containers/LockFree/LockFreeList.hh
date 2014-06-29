//////////////////////////////////////////////////////////////////////////
/// LockFreeList.hh - Lock-free lst interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 21.05.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_CORE_CONTAINERS_LOCKFREE
#define GD_CORE_CONTAINERS_LOCKFREE

#if 0

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/Containers/Containers.hh>
#include <GoddamnEngine/Core/Containers/Pointer/SharedPtr.hh>
#include <GoddamnEngine/Core/Threading/Atomic/Atomic.hh>

GD_NAMESPACE_BEGIN

	template<typename ElementType>
	class LockFreeList final
	{
	private:
		struct Node;
		Node* Head, Tail;

	public:
		template<typename Tag>
		struct Iterator final
		{
			typedef ElementType ThisElementType;
			typedef typename Conditional<Tag::IsConst, ElementType const*, ElementType*>::Type ThisPtrType;
			typedef typename Conditional<Tag::IsConst, ElementType const&, ElementType&>::Type ThisRefType;

		private:
			ThisPtrType Pointer = nullptr;

		public:
			GDINL  Iterator(ThisPtrType const  Pointer);
			GDINL  Iterator(Iterator    const& Iterator);
			GDINL ~Iterator();

			/// Increases/decreases iterator.
			GDINL Iterator& operator++ (int const);
			GDINL Iterator& operator++ (         );
			GDINL Iterator& operator-- (int const);
			GDINL Iterator& operator-- (         );

			/// Increases/decreases iterator on specified value.
			inline Iterator& operator+= (ptrdiff_t const Offset);
			inline Iterator& operator-= (ptrdiff_t const Offset);
			inline Iterator  operator+  (ptrdiff_t const Offset) const;
			inline Iterator  operator-  (ptrdiff_t const Offset) const;

			/// Computes difference between iterators.
			inline ptrdiff_t operator- (Iterator           const& Iterator) const;
			inline ptrdiff_t operator- (ElementType const* const  Pointer ) const;

			/// Compares iterators.
			GDINL bool operator== (Iterator    const&       Other  ) const;
			GDINL bool operator!= (Iterator    const&       Other  ) const; { return (self->Pointer != Other.Pointer); }
			GDINL bool operator== (ElementType const* const Pointer) const { return (self->Pointer == Pointer); }
			GDINL bool operator!= (ElementType const* const Pointer) const { return (self->Pointer != Pointer); }

			/// Assigns this iterator other value.
			GDINL Iterator& operator= (ThisPtrType const  Pointer ) { self->Pointer =           Pointer; return (*self); }
			GDINL Iterator& operator= (Iterator    const& Iterator) { self->Pointer = Iterator->Pointer; return (*self); }

			/// (De)referensing iterator.
			GDINL ThisRefType operator*  () const { return (*self->Pointer); }
			GDINL ThisPtrType operator-> () const { return (self->Pointer); }
		};	// struct Iterator

		/// Iterator type this container uses.
		typedef Iterator<ContainerDetails::IteratorTagConst  >   ConstIterator;
		typedef Iterator<ContainerDetails::IteratorTagMutable> MutableIterator;
		
		/// Reverse iterator type this container uses.
		typedef ContainerDetails::ReverseIterator<MutableIterator> ReverseMutableIterator;
		typedef ContainerDetails::ReverseIterator<ConstIterator  > ReverseConstIterator;

		typedef MemoryProviderType ThisMemoryProviderType;
		typedef ElementType ThisElementType;

		SharedPtr<ElementType> PopLastElement();
	};	// class LockFreeList

	handle* get_hazard_pointer_for_current_thread();

	template<typename ElementType>
	SharedPtr<ElementType> LockFreeList<ElementType>::PopLastElement()
	{
		handle* HazardPointer = get_hazard_pointer_for_current_thread();
		Node* OldHead = self->Head;
		do
		{
			Node* Temp = nullptr;
			do
			{
				Temp = OldHead;
				AtomicsIntrinsics::InterlockedExchangePtr(HazardPointer, OldHead)
				OldHead = head.load();
			} while (old_head != temp);
		} while (old_head && !head.compare_exchange_strong(old_head, old_head->next));
		
		hp.store(nullptr);
		std::shared_ptr<T> res;
		if (old_head)
		{
			res.swap(old_head->data);
			if (outstanding_hazard_pointers_for(old_head))
			{
				reclaim_later(old_head);
			}
			else
			{
				delete old_head;
			}
			delete_nodes_with_no_hazards();
		}
		return res;
	}


GD_NAMESPACE_END

#endif
#endif
