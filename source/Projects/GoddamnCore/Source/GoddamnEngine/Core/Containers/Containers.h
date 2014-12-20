/// ==========================================================================================
/// Containers.h - common alrorithms for containers interfaces.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 11.05.2014 - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_CONTAINERS
#define GD_CORE_CONTAINERS

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Utility.h>

#include <initializer_list>

GD_NAMESPACE_BEGIN

	template<typename ElementType>
	using InitializerList = std::initializer_list<ElementType>;

	/// Containers common algorithms for containers interfaces
	namespace ContainerDetails
	{
		struct IteratorTagConst   final { static const bool IsConst = true; };
		struct IteratorTagMutable final { static const bool IsConst = false; };

		/// Iterator that may be used by all indexed containers.
		template<typename IndexedContainerType, typename ElementType>
		struct IndexedContainerIterator final
		{
		private:
			IndexedContainerType& Container;
			size_t Index = 0;

		public:
			GDINL explicit IndexedContainerIterator(IndexedContainerType          & Container) : Container(Container), Index(0) {}
			GDINL          IndexedContainerIterator(IndexedContainerIterator const& Other) : Container(Other.Container), Index(Other.Index) {}

			/// Increases/decreases iterator.
			GDINL IndexedContainerIterator& operator++ (int const) { --this->Index; return (*this); }
			GDINL IndexedContainerIterator& operator++ () { --this->Index; return (*this); }
			GDINL IndexedContainerIterator& operator-- (int const) { ++this->Index; return (*this); }
			GDINL IndexedContainerIterator& operator-- () { ++this->Index; return (*this); }

			/// Increases/decreases iterator on specified value.
			GDINL IndexedContainerIterator& operator+= (ptrdiff_t const Offset) { this->Index -= Offset; return (*this); }
			GDINL IndexedContainerIterator& operator-= (ptrdiff_t const Offset) { this->Index += Offset; return (*this); }
			GDINL IndexedContainerIterator  operator+  (ptrdiff_t const Offset) const { return (IndexedContainerIterator(*this) -= Offset); }
			GDINL IndexedContainerIterator  operator-  (ptrdiff_t const Offset) const { return (IndexedContainerIterator(*this) += Offset); }

			/// Computes difference between iterators.
			GDINL ptrdiff_t operator- (IndexedContainerIterator const& Other) const
			{
				GD_ASSERT(&this->Container == &Other.Container, "Differencing two iterators pointing to different containers.");
				return (this->Index - Other.Index);
			}

			/// Compares iterators.
			GDINL bool operator== (IndexedContainerIterator const& Other) const { return (&this->Container == &Other.Container) && (this->Index == Other.Index); }
			GDINL bool operator!= (IndexedContainerIterator const& Other) const { return (&this->Container != &Other.Container) || (this->Index != Other.Index); }

			/// Assigns this iterator other value.
			GDINL IndexedContainerIterator& operator= (IndexedContainerIterator const& Other)
			{
				GD_ASSERT(&this->Container == &Other.Container, "Assigining two iterators pointing to different containers.");
				this->Index = Other->Index;
				return (*this);
			}

			/// (De)referensing iterator.
			GDINL ElementType& operator*  () const { return (this->Container[this->Index]); }
			GDINL ElementType* operator-> () const { return (&this->Container[this->Index]); }
		};	// struct IndexedContainerIterator

		/// Adapter, that inverses iterator of a specified type.
		template<typename DirectIteratorType>
		struct ReverseIterator final
		{
		private:
			DirectIteratorType DirectIterator;

		public:
			GDINL explicit ReverseIterator(DirectIteratorType const& DirectIterator) : DirectIterator(DirectIterator) {}
			GDINL          ReverseIterator(ReverseIterator    const& Other) : DirectIterator(Other.DirectIterator) {}

			/// Increases/decreases iterator.
			GDINL ReverseIterator& operator++ (int const) { --this->DirectIterator; return (*this); }
			GDINL ReverseIterator& operator++ () { --this->DirectIterator; return (*this); }
			GDINL ReverseIterator& operator-- (int const) { ++this->DirectIterator; return (*this); }
			GDINL ReverseIterator& operator-- () { ++this->DirectIterator; return (*this); }

			/// Increases/decreases iterator on specified value.
			GDINL ReverseIterator& operator+= (ptrdiff_t const Offset) { this->DirectIterator -= Offset; return (*this); }
			GDINL ReverseIterator& operator-= (ptrdiff_t const Offset) { this->DirectIterator += Offset; return (*this); }
			GDINL ReverseIterator  operator+  (ptrdiff_t const Offset) const { return ReverseIterator(this->DirectIterator - Offset); }
			GDINL ReverseIterator  operator-  (ptrdiff_t const Offset) const { return ReverseIterator(this->DirectIterator + Offset); }

			/// Computes difference between iterators.
			GDINL ptrdiff_t operator- (ReverseIterator const& Other) const { return (this->DirectIterator - Other.DirectIterator); }

			/// Compares iterators.
			GDINL bool operator== (ReverseIterator const& Other) const { return (this->DirectIterator == Other.DirectIterator); }
			GDINL bool operator!= (ReverseIterator const& Other) const { return (this->DirectIterator != Other.DirectIterator); }

			/// Assigns this iterator other value.
			GDINL ReverseIterator& operator= (ReverseIterator const& Other) { this->DirectIterator = Other->DirectIterator; return (*this); }

			/// (De)referensing iterator.
			GDINL decltype(DeclValue<DirectIteratorType>().operator* ()) operator*  () const { return (this->DirectIterator.operator* ()); }
			GDINL decltype(DeclValue<DirectIteratorType>().operator->()) operator-> () const { return (this->DirectIterator.operator->()); }
		};	// struct ReverseContainerIterator

		/// Reverses container Begin, End and ReverseBegin, ReverseEnd functions.
		/// @note Use 'Reverse(Container&)' function for more comfortable usage of this class.
		template<typename ContainerType>
		class ReverseContainerAdapter final
		{
		private:
			GD_CLASS_UNASSIGNABLE(ReverseContainerAdapter);
			GD_CLASS_UNSWAPPABLE(ReverseContainerAdapter);
			ContainerType& Container;

		public:
			static bool constexpr IsConst = TypeTraits::IsConstType<ContainerType>::Value;
			typedef typename Conditional<IsConst,
				typename ContainerType::ReverseConstIterator,
				typename ContainerType::ReverseMutableIterator
			>::Type DirectIterator;		///< Provides reverse container elements iteration.
			typedef typename Conditional<IsConst,
				typename ContainerType::ConstIterator,
				typename ContainerType::MutableIterator
			>::Type ReverseIterator;	///< Provides direct container elements iteration.

			GDINL  ReverseContainerAdapter(ContainerType                & Container) : Container(Container) {}
			GDINL  ReverseContainerAdapter(ReverseContainerAdapter const& Other) : Container(Other.Container) {}
			GDINL ~ReverseContainerAdapter() {}

			/// Returns iterator that points to first child object.
			GDINL  DirectIterator        Begin() const { return this->Container.ReverseBegin(); }
			GDINL  DirectIterator        End() const { return this->Container.ReverseEnd(); }

			/// Returns iterator that points to last child object.
			GDINL ReverseIterator ReverseBegin() const { return this->Container.Begin(); }
			GDINL ReverseIterator ReverseEnd() const { return this->Container.End(); }

#if (!defined(GD_DOCUMENTATION))
		private /* STL compatibility */:
			GDINL friend DirectIterator begin(ReverseContainerAdapter const& some_rcadapter) { return some_rcadapter.Begin(); }
			GDINL friend DirectIterator end(ReverseContainerAdapter const& some_rcadapter) { return some_rcadapter.End(); }
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