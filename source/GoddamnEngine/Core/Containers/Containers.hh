/// ==========================================================================================
/// Containers.hh - common alrorithms for containers interfaces.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 11.05.2014 - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_CONTAINERS
#define GD_CORE_CONTAINERS

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/Utility.hh>

GD_NAMESPACE_BEGIN
	
	namespace ContainerDetails
	{
		struct IteratorTagConst   final { static const bool IsConst = true ; };
		struct IteratorTagMutable final { static const bool IsConst = false; };

		/// Constant reverse iterator type this container uses.
		template<typename IteratorType>
		struct ReverseIterator final
		{
		public:
			typedef typename IteratorType::ThisElementType ThisElementType;
			typedef typename IteratorType::ThisPtrType ThisPtrType;
			typedef typename IteratorType::ThisRefType ThisRefType;

		private:
			IteratorType Iterator;

		public:
			GDINL ReverseIterator(ThisPtrType     const  Pointer ) : Iterator(IteratorType(Pointer)) { }
			GDINL ReverseIterator(IteratorType    const& Iterator) : Iterator(      Iterator       ) { }
			GDINL ReverseIterator(ReverseIterator const& Other   ) : Iterator(Other.Iterator       ) { }

			/// Increases/decreases iterator.
			GDINL ReverseIterator& operator++ (int const) { --self->Iterator; return (*self); }
			GDINL ReverseIterator& operator++ (         ) { --self->Iterator; return (*self); }
			GDINL ReverseIterator& operator-- (int const) { ++self->Iterator; return (*self); }
			GDINL ReverseIterator& operator-- (         ) { ++self->Iterator; return (*self); }

			/// Increases/decreases iterator on specified value.
			GDINL ReverseIterator& operator+= (ptrdiff_t const Offset)	     { self->Iterator -= Offset; return (*self); }
			GDINL ReverseIterator& operator-= (ptrdiff_t const Offset)       { self->Iterator += Offset; return (*self); }
			GDINL ReverseIterator  operator+  (ptrdiff_t const Offset) const { return ReverseContainerIterator(self->Iterator - Offset); }
			GDINL ReverseIterator  operator-  (ptrdiff_t const Offset) const { return ReverseContainerIterator(self->Iterator + Offset); }

			/// Computes difference between iterators.
			GDINL ptrdiff_t operator- (ThisPtrType     const  Pointer ) const { return (self->Iterator - Pointer); }
			GDINL ptrdiff_t operator- (IteratorType    const& Iterator) const { return (self->Iterator - Iterator); }
			GDINL ptrdiff_t operator- (ReverseIterator const& Other   ) const { return (self->Iterator - Other.Iterator); }

			/// Compares iterators.
			GDINL bool operator== (ThisPtrType     const  Pointer ) const { return (self->Iterator == Pointer); }
			GDINL bool operator== (IteratorType    const  Iterator) const { return (self->Iterator == Iterator); }
			GDINL bool operator== (ReverseIterator const& Other   ) const { return (self->Iterator == Other.Iterator); }
			GDINL bool operator!= (ThisPtrType     const  Pointer ) const { return (self->Iterator != Pointer); }
			GDINL bool operator!= (IteratorType    const  Iterator) const { return (self->Iterator != Iterator); }
			GDINL bool operator!= (ReverseIterator const& Other   ) const { return (self->Iterator != Other.Iterator); }

			/// Assigns this iterator other value.
			GDINL ReverseIterator& operator= (ThisPtrType     const  Pointer ) { self->Iterator = Pointer;         return (*self); }
			GDINL ReverseIterator& operator= (IteratorType    const  Iterator) { self->Iterator = Iterator;        return (*self); }
			GDINL ReverseIterator& operator= (ReverseIterator const& Other   ) { self->Iterator = Other->Iterator; return (*self); }

			/// (De)referensing iterator.
			GDINL ThisRefType operator*  () const { return (self->Iterator.operator* ()); }
			GDINL ThisPtrType operator-> () const { return (self->Iterator.operator->()); }
		};	// struct ReverseContainerIterator

		/// Reverses container Begin, End and ReverseBegin, ReverseEnd functions.
		/// @note Use 'Reverse(Container&)' function for more comfortable usage of this class.
		template<typename ContainerType>
		class ReverseContainerAdapter final
		{
		private:
			GD_CLASS_UNASSIGNABLE(ReverseContainerAdapter);
			GD_CLASS_UNSWAPPABLE (ReverseContainerAdapter);
			ContainerType& Container;

		public:
			static bool constexpr IsConst = TypeTraits::IsConstType<ContainerType>::Value;
			typedef typename Conditional<IsConst,
				typename ContainerType::  ReverseConstIterator,
				typename ContainerType::ReverseMutableIterator
			>::Type DirectIterator;		///< Provides reverse container elements iteration.
			typedef typename Conditional<IsConst,
				typename ContainerType::  ConstIterator,
				typename ContainerType::MutableIterator
			>::Type ReverseIterator;	///< Provides direct container elements iteration.

			GDINL  ReverseContainerAdapter(ContainerType                & Container) : Container(      Container) { }
			GDINL  ReverseContainerAdapter(ReverseContainerAdapter const& Other    ) : Container(Other.Container) { }
			GDINL ~ReverseContainerAdapter(                                        ) { }

			/// Returns iterator that points to first child object.
			GDINL  DirectIterator        Begin() const { return self->Container.ReverseBegin(); }
			GDINL  DirectIterator        End  () const { return self->Container.ReverseEnd(); }
		
			/// Returns iterator that points to last child object.
			GDINL ReverseIterator ReverseBegin() const { return self->Container.Begin(); }
			GDINL ReverseIterator ReverseEnd  () const { return self->Container.End  (); }

#if (!defined(GD_DOCUMENTATION))
		private /* STL compatibility */:	
			GDINL friend DirectIterator begin(ReverseContainerAdapter const& some_rcadapter) { return some_rcadapter.Begin(); }
			GDINL friend DirectIterator end  (ReverseContainerAdapter const& some_rcadapter) { return some_rcadapter.End(); }
#endif		// if (!defined(GD_DOCUMENTATION))
		};	// class ReverseContainerAdapter
	}	// namespace ContainerDetails

	/// Returns a version of this container with Begin and End, ReverseBegin and ReverseEnd functions reversed.
	template<typename ContainerType>
	GDINL ContainerDetails::ReverseContainerAdapter<ContainerType> Reverse(ContainerType& Container)
	{	// Returning container wrapped with reverse adapter.
		return ContainerDetails::ReverseContainerAdapter<ContainerType>(Container);
	}

GD_NAMESPACE_END

#endif