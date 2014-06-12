//////////////////////////////////////////////////////////////////////////
/// Object.h: basic object interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.12.2013 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_CORE_OBJECT
#define GD_CORE_OBJECT

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/Reflection/Reflectable.hh>
#include <GoddamnEngine/Core/Reflection/TypeInformation/TypeInformation.hh>
#include <GoddamnEngine/Core/Containers/LockFreeList/LockFreeList.hh>
#include <GoddamnEngine/Core/Threading/Atomic/Atomic.hh>

GD_NAMESPACE_BEGIN

	typedef UInt64 ObjectRefenceCount;
	typedef UInt8  ObjectTreeLockingFlags;

	/// Basic Object class.
	class Object : public IReflectable/*: public Lockable*/
	{
	public:
		template<typename Tag, typename ObjectType = Object>
		struct Iterator final
		{
			typedef ObjectType ThisElementType;
			typedef typename Conditional<Tag::IsConst, ObjectType const*, ObjectType*>::Type ThisPtrType;

		private:
			ThisPtrType Pointer;

		public:
			GDINL  Iterator(ThisPtrType const  Pointer ) : Pointer(         Pointer) { }
			GDINL  Iterator(Iterator    const& Iterator) : Pointer(Iterator.Pointer) { }
			GDINL ~Iterator() { }
			
			/// Compares iterators.
			GDINL bool operator== (Iterator   const&       Other  ) const { return (self->Pointer == Other.Pointer); }
			GDINL bool operator!= (Iterator   const&       Other  ) const { return (self->Pointer != Other.Pointer); }
			GDINL bool operator== (ObjectType const* const Pointer) const { return (self->Pointer == Pointer); }
			GDINL bool operator!= (ObjectType const* const Pointer) const { return (self->Pointer != Pointer); }

			/// Assigns this iterator other value.
			GDINL Iterator& operator= (ThisPtrType const  Pointer ) { self->Pointer =           Pointer; return (*self); }
			GDINL Iterator& operator= (Iterator    const& Iterator) { self->Pointer = Iterator->Pointer; return (*self); }

			/// (De)referensing iterator.
			GDINL ThisPtrType operator*  () const { return (self->Pointer); }
			GDINL ThisPtrType operator-> () const { return (self->Pointer); }

			/// Increases/decreases iterator on specified value.
			GDINL Iterator  operator+  (ptrdiff_t const Offset) const { return (Iterator(self->Pointer) += Offset); }
			GDINL Iterator  operator-  (ptrdiff_t const Offset) const { return (Iterator(self->Pointer) -= Offset); }

			/// Increases iterator.
			GDINL Iterator& operator++ (int const) { return ++(*self); }
			GDINL Iterator& operator++ (         )
			{
				GD_DEBUG_ASSERT(self->Pointer != nullptr, "nullptr iterator increment.");
				self->Pointer = object_cast<ThisPtrType>(self->Pointer->GetNextSiblingObject());
				return (*self);
			}

			/// Decreases iterator.
			GDINL Iterator& operator-- (int const) { return --(*self); }
			GDINL Iterator& operator-- (         )
			{
				GD_DEBUG_ASSERT(self->Pointer != nullptr, "nullptr iterator decrement.");
				self->Pointer = object_cast<ThisPtrType>(self->Pointer->GetPreviousSiblingObject());
				return (*self);
			}
		};	// struct Iterator

		template<typename ObjectType> using        MutableIterator =        Iterator<ContainerDetails::IteratorTagMutable, ObjectType>;
		template<typename ObjectType> using          ConstIterator =        Iterator<ContainerDetails::IteratorTagConst,   ObjectType>;
		template<typename ObjectType> using ReverseMutableIterator = MutableIterator<                                      ObjectType>;
		template<typename ObjectType> using   ReverseConstIterator =   ConstIterator<                                      ObjectType>;

		enum ObjectTreeLockingFlagsEnumeration : ObjectTreeLockingFlags
		{
			TreeLockingFlagsCheckValidity				= 1 << 0,	///< Do check object tree validity
			TreeLockingFlagsAutomaticReferenceCounting	= 1 << 1,	///< Do automatically add/remove reference when attaching / detaching
			TreeLockingFlagsDisableDetaching			= 1 << 2,	///< This object supports detaching from parent only one time 
			TreeLockingFlagsDisableAttachingToObject	= 1 << 3,	///< This object supports attaching to parent only one time 
			TreeLockingFlagsDisableAttachingObjects		= 1 << 4,	///< This object does not supports attaching objects to it
			TreeLockingFlagsDisableSiblingSwapping		= 1 << 5,	///< This object does not supports swapping with sibling objects
			TreeLockingFlagsDisableSiblingMoving		= 1 << 6,	///< This object does not supports moving to sibling objects

			TreeLockingFlagsDefaultObject				= (TreeLockingFlagsCheckValidity | TreeLockingFlagsAutomaticReferenceCounting),
			TreeLockingFlagsDefaultHRIObject			= (TreeLockingFlagsDefaultObject),
			TreeLockingFlagsDefaultComponent			= (TreeLockingFlagsDefaultObject | TreeLockingFlagsDisableDetaching | TreeLockingFlagsDisableAttachingToObject | TreeLockingFlagsDisableAttachingObjects),
			TreeLockingFlagsDefaultGameObject			= (TreeLockingFlagsDefaultComponent & (~TreeLockingFlagsDisableAttachingObjects)),
			TreeLockingFlagsDefaultResource				= ((TreeLockingFlagsDefaultObject | TreeLockingFlagsDisableDetaching | TreeLockingFlagsDisableAttachingToObject) & (~TreeLockingFlagsAutomaticReferenceCounting)),

			TreeLockingFlagsNone						= 0,		///< None 
			TreeLockingFlagsAll							= 0xFF		///< All
		};	// enum ObjectTreeLockingFlags

	private:
		GD_TYPEINFORMATION_DEFINITION(Object, NoBaseClass, GDAPI);
		GD_CLASS_UNASSIGNABLE(Object);
		GD_CLASS_UNSWAPPABLE (Object);
		GD_CLASS_UNCOPIABLE  (Object);

		mutable ObjectTreeLockingFlags TreeLockingFlags = TreeLockingFlagsDefaultObject;
		mutable ObjectRefenceCount RefenceCount = 1;
		Object* ParentObject = nullptr;
		Object* NextSiblingObject = nullptr;
		Object* PreviousSiblingObject = nullptr;
		Object* FirstChildObject = nullptr;
		Object* LastChildObject = nullptr;
		UInt64 Guid = 0;

	protected:
		
		/// Initializes new object with specified flags for moving in object tree
		///	and reference counter values 1
		/// @param TreeLockingFlags	Flags for moving in object tree. Nothing locked by default
		/// @param ParentObject		Parent object that created this one
		GDAPI explicit Object(ObjectTreeLockingFlags const TreeLockingFlags = TreeLockingFlagsDefaultObject, Object* const ParentObject = nullptr);

		/// Destroys this object and decrements reference counter in
		///	all child object that are attached to this 
		GDAPI virtual ~Object();

	public:

		/// @name Reference Counting
		///	Objects in GoddamnEngine are garbage-collectible.
		///	GC is implemented using reference counting: When object is initialized it`s reference count is 1. When it is required and is busy counter is
		///	is incremented, when finished - is decremented. When counter becomes zero, objects destroys itself.
		/// @note Reference counting can be automated using RefPtr class.
		///	@code
		///		// Manual counting:
		///		MyObject* const object = ...;
		///		object->AddReference();
		///		object->DoSomething();
		///		DoSomething2(object);
		///		object->RemoveReference();
		///	@endcode
		/// @{

		/// Increments reference counter.
		/// @returns Current reference counter`s value.
		GDAPI ObjectRefenceCount AddReference() const;
		/// Decrements reference counter.
		/// @returns Current reference counter`s value.
		GDAPI ObjectRefenceCount RemoveReference() const;

		/// @}

		/// @name Object tree management
		///	Each object may contain list of object that it creates.
		///	All this object are going to be counter-decremented on Objects deletion.
		///	@code
		///		MyObjectType* const myObject = ...;
		///     
		///		for (MyOtherObjectType* 
		///			 myChildObject = (MyOtherObjectType*)myObject->GetLastChildObject(); myChildObject != nullptr
		///			 myChildObject = (MyOtherObjectType*)myChildObject->GetPreviousSiblingObject())
		///		{	// Manual iteration:
		///			myChildObject->DoSomething(...);
		///		}
		///		
		///		for (auto const myChildObject : IterateObject<MyOtherObjectType>(myChildObject))
		///		{	// Automated iteration:
		///			myChildObject->DoSomething(...);
		///		}
		///	@endcode
		/// @{

		/// Returns flags Describing flags for object edit-ability in object tree.
		GDINL ObjectTreeLockingFlags GetTreeLockingFlags() const { return self->TreeLockingFlags; }
		
		template<typename ObjectType> GDINL        MutableIterator<ObjectType>        Begin()       { return        MutableIterator<ObjectType>(object_cast<ObjectType      *>(self->GetFirstChildObject())); }
		template<typename ObjectType> GDINL          ConstIterator<ObjectType>        Begin() const { return          ConstIterator<ObjectType>(object_cast<ObjectType const*>(self->GetFirstChildObject())); }
		template<typename ObjectType> GDINL        MutableIterator<ObjectType>        End  ()       { return        MutableIterator<ObjectType>(nullptr); }
		template<typename ObjectType> GDINL          ConstIterator<ObjectType>        End  () const { return          ConstIterator<ObjectType>(nullptr); }
		template<typename ObjectType> GDINL ReverseMutableIterator<ObjectType> ReverseBegin()       { return ReverseMutableIterator<ObjectType>(object_cast<ObjectType      *>(self->GetFirstChildObject())); }
		template<typename ObjectType> GDINL   ReverseConstIterator<ObjectType> ReverseBegin() const { return   ReverseConstIterator<ObjectType>(object_cast<ObjectType const*>(self->GetFirstChildObject())); }
		template<typename ObjectType> GDINL ReverseMutableIterator<ObjectType> ReverseEnd  ()       { return ReverseMutableIterator<ObjectType>(nullptr); }
		template<typename ObjectType> GDINL   ReverseConstIterator<ObjectType> ReverseEnd  () const { return   ReverseConstIterator<ObjectType>(nullptr); }

		/// Returns next object in list of parent`s child objects.
		/// @returns Pointer to object or nullptr if is last child.
		GDINL Object const* GetNextSiblingObject    () const { return self->NextSiblingObject; }
		GDINL Object      * GetNextSiblingObject    ()       { return self->NextSiblingObject; }
		
		/// Returns previous object in list of parent`s child objects.
		/// @returns Pointer to object or nullptr is is first child.
		GDINL Object const* GetPreviousSiblingObject() const { return self->PreviousSiblingObject; }
		GDINL Object      * GetPreviousSiblingObject()       { return self->PreviousSiblingObject; }
		
		/// Returns first object attached to this.
		/// @returns Pointer to object or nullptr if no child existing.
		GDINL Object const* GetFirstChildObject     () const { return self->FirstChildObject; }
		GDINL Object      * GetFirstChildObject     ()       { return self->FirstChildObject; }
		
		/// Returns last object attached to this.
		/// @returns Pointer to object or nullptr if no child existing.
		GDINL Object const* GetLastChildObject      () const { return self->LastChildObject; }
		GDINL Object      * GetLastChildObject      ()       { return self->LastChildObject; }
		
		/// Returns object this object is attached to.
		///	For game objects returns scene this game object is attached to, for components this functions returns owner game object, for other objects read the specifications.
		/// @returns Pointer to object or nullptr if is not attached to any objects.
		GDINL Object const* GetParentObject         () const { return self->ParentObject; }
		GDINL Object      * GetParentObject         ()       { return self->ParentObject; }

		/// @}

		/// @name Child object searching functions.
		/// @{

		/// Searches for first object with specified searching function.
		/// @returns Pointer to first found child object.
		template<typename ObjectType, typename SearchingPredicateType>
		GDINL ObjectType const* SearchForFirstChildObject(SearchingPredicateType const& Predicate) const;
		template<typename ObjectType, typename SearchingPredicateType>
		GDINL ObjectType      * SearchForFirstChildObject(SearchingPredicateType const& Predicate);
		
		/// Searches for first object with specified searching function.
		/// @returns Pointer to last found child object.
		template<typename ObjectType, typename SearchingPredicateType>
		GDINL ObjectType const* SearchForLastChildObject(SearchingPredicateType const& Predicate) const;
		template<typename ObjectType, typename SearchingPredicateType>
		GDINL ObjectType      * SearchForLastChildObject(SearchingPredicateType const& Predicate);

		/// @}
		
		/// @name Child object management.
		/// @{

	private:
		GDAPI void DetachFromParentObject(ObjectTreeLockingFlags const CustomTreeLockingFlags);
		GDAPI void AttachToObject        (Object* const ParentObject,  ObjectTreeLockingFlags const CustomTreeLockingFlags);
		GDAPI void SwapToSiblingObject   (Object* const SiblingObject, ObjectTreeLockingFlags const CustomTreeLockingFlags);
		GDAPI void MoveToSiblingObject   (Object* const SiblingObject, ObjectTreeLockingFlags const CustomTreeLockingFlags);
		
	public:
		/// Removes this object from parent object`s list.
		GDINL void DetachFromParentObject(                           ) { self->DetachFromParentObject(            self->GetTreeLockingFlags()); }
		
		/// Attaches object to specified parent.
		GDINL void AttachToObject        (Object* const ParentObject ) { self->AttachToObject(ParentObject,       self->GetTreeLockingFlags()); }
		
		/// Swaps object`s places is parent's list to.
		/// @param SiblingObject Sibling object to swap with.
		GDINL void SwapToSiblingObject   (Object* const SiblingObject) { self->SwapToSiblingObject(SiblingObject, self->GetTreeLockingFlags());}
		
		/// Moves object from it's old place in list next to specified object.
		/// @param SiblingObject Sibling object to move with.
		/// @note Moving is supported only for objects that are at the end of child list.
		GDINL void MoveToSiblingObject   (Object* const SiblingObject) { self->MoveToSiblingObject(SiblingObject, self->GetTreeLockingFlags());}

		/// @}

#if (defined(GD_DEBUG) || defined(GD_DOCUMENTATION))
		/// Prints object tree to debug output
		GDAPI void DumpTreeData(_Out_ String& DumpedTreeData) const;
#else
		/// Dummy
		GDINL void DumpTreeData(_Out_ String& DumpedTreeData) const { }
#endif
	};	// class Object


	/// Proveds iteration methods for some object.
	/// This adapter if fully compatiable with Reverse function.
	template<typename ObjectType, typename Tag>
	class ObjectIterationAdapter final
	{
		static_assert(TypeTraits::IsBaseType<Object, ObjectType>::Value, "'ObjectType' should be derived from 'Object'");
	public /*Inner types*/:
		/// Provides iteration of constant sibling objects.
		typedef typename Conditional<Tag::IsConst, Object const*, Object*>::Type ThisPtrType;
		typedef typename Conditional<Tag::IsConst, 
			typename Object::  ConstIterator<ObjectType>,
			typename Object::MutableIterator<ObjectType>
		>::Type Iterator;
		typedef typename Conditional<Tag::IsConst,
			typename Object::  ReverseConstIterator<ObjectType>,
			typename Object::ReverseMutableIterator<ObjectType>
		>::Type ReverseIterator;

	private:
		GD_CLASS_UNASSIGNABLE(ObjectIterationAdapter);
		GD_CLASS_UNSWAPPABLE (ObjectIterationAdapter);
		ThisPtrType TheBaseObject;

	public /*Class API*/:
		GDINL explicit ObjectIterationAdapter(ThisPtrType            const  TheBaseObject) : TheBaseObject(      TheBaseObject) { }
		GDINL          ObjectIterationAdapter(ObjectIterationAdapter const& Other        ) : TheBaseObject(Other.TheBaseObject) { }
		GDINL         ~ObjectIterationAdapter(                                           ) { }

		/// Returns iterator that points to first child object.
		GDINL Iterator               Begin() const { return self->TheBaseObject->Begin<ObjectType>(); }
		GDINL Iterator               End  () const { return self->TheBaseObject->End  <ObjectType>(); }

		/// Returns iterator that points to last child object.
		GDINL ReverseIterator ReverseBegin() const { return self->TheBaseObject->ReverseBegin<ObjectType>(); }
		GDINL ReverseIterator ReverseEnd  () const { return self->TheBaseObject->ReverseEnd  <ObjectType>(); }

#if (!defined(GD_DOCUMENTATION)) 
	private /* STL compatibility */:
		GDINL friend Iterator begin(ObjectIterationAdapter const& some_oiadapter) { return some_oiadapter.Begin(); }
		GDINL friend Iterator end  (ObjectIterationAdapter const& some_oiadapter) { return some_oiadapter.End(); }
#endif	// if (!defined(GD_DOCUMENTATION)) 
	};	// class ObjectIterationAdapter

	/// Returns version of this object that can be used in ranged for loops.
	template<typename ObjectType>
	ObjectIterationAdapter<ObjectType, ContainerDetails::IteratorTagConst> IterateChildObjects(Object const* const TheObject)
	{	// Returning object wrapped with iteration adapter.
		return ObjectIterationAdapter<ObjectType, ContainerDetails::IteratorTagConst>(TheObject);
	}
	template<typename ObjectType>
	ObjectIterationAdapter<ObjectType, ContainerDetails::IteratorTagMutable> IterateChildObjects(Object* const TheObject)
	{	// Returning object wrapped with iteration adapter.
		return ObjectIterationAdapter<ObjectType, ContainerDetails::IteratorTagMutable>(TheObject);
	}

	/// Increases specified object`s reference count if it exists.
	GDINL void SafeObtain(Object const* const TheObject)
	{	// Checking if specified object is not null.
		if (TheObject != nullptr) TheObject->AddReference();
	}

	/// Decreases specified object`s reference count if it exists.
	GDINL void SafeRelease(Object const* const TheObject)
	{	// Checking if specified object is not null.
		if (TheObject != nullptr) TheObject->RemoveReference();
	}

	typedef Object HRIObject;

GD_NAMESPACE_END

#include <GoddamnEngine/Core/Object/Object.inl>

#endif
