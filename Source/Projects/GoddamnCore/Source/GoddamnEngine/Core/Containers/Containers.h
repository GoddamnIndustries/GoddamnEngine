/// ==========================================================================================
/// Containers.h - common algorithms for containers interfaces.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_CONTAINERS
#define GD_CORE_CONTAINERS

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/Utility.h>

#include <new>

/// Adds support of ranged-for iteration to the container. 
/// @param this_type Type of this container. Should contain const and mutable versions of 'Begin' and 'End' methods.
/// @note If this container has 'PtrBegin' and 'PtrEnd' methods, preferably to use @c 'GD_CONTAINER_DEFINE_PTR_ITERATION_SUPPORT' macro intead.
#define GD_CONTAINER_DEFINE_ITERATION_SUPPORT(this_type) \
	GDINL friend Iterator begin(this_type& some_container) \
	{ \
		return some_container.Begin(); \
	} \
	GDINL friend ConstIterator begin(this_type const& some_container) \
	{ \
		return some_container.Begin(); \
	} \
	GDINL friend Iterator end(this_type& some_container) \
	{ \
		return some_container.End(); \
	} \
	GDINL friend ConstIterator end(this_type const& some_container) \
	{ \
		return some_container.End(); \
	} \

/// Adds support of ranged-for iteration to the container. 
/// @param this_type Type of this container. Should contain const and mutable versions of 'PtrBegin' and 'PtrEnd' methods.
#define GD_CONTAINER_DEFINE_PTR_ITERATION_SUPPORT(this_type) \
	GDINL friend PtrIterator begin(this_type& some_container) \
	{ \
		return some_container.PtrBegin(); \
	} \
	GDINL friend PtrConstIterator begin(this_type const& some_container) \
	{ \
		return some_container.PtrBegin(); \
	} \
	GDINL friend PtrIterator end(this_type& some_container) \
	{ \
		return some_container.PtrEnd(); \
	} \
	GDINL friend PtrConstIterator end(this_type const& some_container) \
	{ \
		return some_container.PtrEnd(); \
	} \

GD_NAMESPACE_BEGIN

	/// @brief Type that cannot be used.
	struct UnusableType
	{
		GDINT UnusableType();
		GDINT ~UnusableType();
	};	// struct UnusableType

	/// @brief Struct that is used to determine if iterator has correct signature for direct iterations.
	struct DirectIteratorTag
	{
		typedef UnusableType Iterator;

		GDINL Iterator operator++ ();
		GDINL Iterator operator++ (int const Unused);
	};	// struct DirectIteratorTag

	/// @brief Struct that is used to determine if iterator has correct signature for reverse iterations.
	struct ReverseIteratorTag
	{
		typedef UnusableType Iterator;

		GDINL Iterator operator-- ();
		GDINL Iterator operator-- (int const Unused);
	};	// struct ReverseIteratorTag

	/// @brief Checks if direct iterator has valid signature.
	/// @param ThisType Iterator type.
#define GD_ITERATOR_CHECK_DIRECT(ThisType) \
	static_assert(TypeTraits::IsBase<DirectIteratorTag, ThisType>::Value, "'GD_ITERATOR_CHECK_DIRECT' is supported only for 'DirectIteratorTag'-derived types"); \
	/** @todo Add checks for valid operator++. */ \

	/// @brief Checks if reverse iterator has valid signature.
	/// @param ThisType Iterator type.
#define GD_ITERATOR_CHECK_REVERSE(ThisType) \
	static_assert(TypeTraits::IsBase<ReverseIteratorTag, ThisType>::Value, "'GD_ITERATOR_CHECK_REVERSE' is supported only for 'ReverseIteratorTag'-derived types"); \
	/** @todo Add checks for valid operator++. */ \

	/// @brief Struct that is used to determine if container has correct signature for iterations.
	struct ContainerIteratableTag
	{
		typedef UnusableType Iterator;
		typedef UnusableType ConstIterator;

		GDINL Iterator Begin();
		GDINL ConstIterator Begin() const;
		GDINL Iterator End();
		GDINL ConstIterator End() const;
	};	// struct Container

	/// @brief Struct that is used to determine if container has correct signature for reverse iterations.
	struct ContainerReverseIteratableTag
	{
		typedef UnusableType ReverseIterator;
		typedef UnusableType ReverseConstIterator;

		GDINL ReverseIterator ReverseBegin();
		GDINL ReverseConstIterator ReverseBegin() const;
		GDINL ReverseIterator ReverseEnd();
		GDINL ReverseConstIterator ReverseEnd() const;
	};	// struct ContainerReverseIteratableTag

	/// @brief Struct that is used to determine if container has correct signature for pointer iterations.
	struct ContainerPtrIteratableTag
	{
		typedef UnusableType PtrIterator;
		typedef UnusableType PtrConstIterator;

		GDINL PtrIterator PtrBegin();
		GDINL PtrConstIterator PtrBegin() const;
		GDINL PtrIterator PtrEnd();
		GDINL PtrConstIterator PtrEnd() const;
	};	// struct ContainerPtrIteratableTag

	/// @brief Struct that is used to determine if container has correct signature for pointer reverse iterations.
 	struct ContainerPtrReverseIteratableTag
	{
		typedef UnusableType ReversePtrIterator;
		typedef UnusableType ReversePtrConstIterator;

		GDINL ReversePtrIterator ReversePtrBegin();
		GDINL ReversePtrConstIterator ReversePtrBegin() const;
		GDINL ReversePtrIterator ReversePtrEnd();
		GDINL ReversePtrConstIterator ReversePtrEnd() const;
	};	// struct ContainerPtrReverseIteratableTag

	/// @brief Checks if this container has a valid signature for iterations.
	/// @param ThisType Container type.
#define GD_CONTAINER_CHECK_ITERATORS(ThisType) \
	static_assert(TypeTraits::IsBase<ContainerIteratableTag, ThisType>::Value, "'GD_CONTAINER_CHECK_ITERATOR' is supported only for 'ContainerIteratableTag'-derived types"); \
	static_assert(!TypeTraits::IsSame<Iterator, UnusableType>::Value, "'Iterator' type is not defined."); \
	static_assert(!TypeTraits::IsSame<ConstIterator, UnusableType>::Value, "'ConstIterator' type is not defined."); \
	/** @todo Add checks for valid Begin/End methods. */ \

	/// @brief Checks if this container has a valid signature for reverse iterations.
	/// @param ThisType Container type.
#define GD_CONTAINER_CHECK_REVERSE_ITERATORS(ThisType) \
	static_assert(TypeTraits::IsBase<ContainerReverseIteratableTag, ThisType>::Value, "'GD_CONTAINER_CHECK_REVERSE_ITERATORS' is supported only for 'ContainerReverseIteratableTag'-derived types"); \
	static_assert(!TypeTraits::IsSame<ReverseIterator, UnusableType>::Value, "'ReverseIterator' type is not defined."); \
	static_assert(!TypeTraits::IsSame<ReverseConstIterator, UnusableType>::Value, "'ReverseConstIterator' type is not defined."); \
	/** @todo Add checks for valid ReverseBegin/ReverseEnd methods. */ \

	/// @brief Checks if this container has a valid signature for per-pointer iterations.
	/// @param ThisType Container type.
#define GD_CONTAINER_CHECK_PTR_ITERATORS(ThisType) \
	static_assert(TypeTraits::IsBase<ContainerPtrIteratableTag, ThisType>::Value, "'GD_CONTAINER_CHECK_PTR_ITERATORS' is supported only for 'ContainerPtrIteratableTag'-derived types"); \
	static_assert(!TypeTraits::IsSame<PtrIterator, UnusableType>::Value, "'PtrIterator' type is not defined."); \
	static_assert(!TypeTraits::IsSame<PtrConstIterator, UnusableType>::Value, "'PtrConstIterator' type is not defined."); \
	/** @todo Add checks for valid PtrBegin/PtrEnd methods. */ \

	/// @brief Checks if this container has a valid signature for per-pointer reverse iterations.
	/// @param ThisType Container type.
#define GD_CONTAINER_CHECK_REVERSE_PTR_ITERATORS(ThisType) \
	static_assert(TypeTraits::IsBase<ContainerPtrReverseIteratableTag, ThisType>::Value, "'GD_CONTAINER_CHECK_REVERSE_PTR_ITERATORS' is supported only for 'ContainerPtrReverseIteratableTag'-derived types"); \
	static_assert(!TypeTraits::IsSame<ReversePtrIterator, UnusableType>::Value, "'ReversePtrIterator' type is not defined."); \
	static_assert(!TypeTraits::IsSame<ReversePtrConstIterator, UnusableType>::Value, "'ReversePtrConstIterator' type is not defined."); \
	/** @todo Add checks for valid ReverseBegin/ReverseEnd methods. */ \

	/// @brief Iterator for container that have constant-time (preferably) index access operators.
	/// @tparam IndexedContainerType Type of container.
	/// @tparam IndexedContainerElementType Type of container element type.
	template<typename IndexedContainerType, typename IndexedContainerElementType>
	struct IndexedContainerIterator final : public DirectIteratorTag, public ReverseIteratorTag
	{
		GD_ITERATOR_CHECK_DIRECT(IndexedContainerIterator);
		GD_ITERATOR_CHECK_REVERSE(IndexedContainerIterator);

	private:
		IndexedContainerType& Container;
		size_t Index = 0;

	public:

		/// @brief Initializes iterator that points on the first element of the specified container.
		/// @param Container Container that is going to be iterated.
		GDINL explicit IndexedContainerIterator(IndexedContainerType& Container)
			: Container(Container), Index(0)
		{
		}

		/// @brief Assigns this iterator other one.
		/// @param Other Other iterator to assign.
		/// @returns this.
		GDINL IndexedContainerIterator& operator= (IndexedContainerIterator const& Other)
		{
			GD_ASSERT(&this->Container == &Other.Container, "Iterators have different base containers.");
			this->Index = Other.Index;
			return *this;
		}

		/// @brief Assigns the iterator next element of the indexed container.
		/// @returns Incremented iterator.
		GDINL IndexedContainerIterator& operator++ ()
		{
			this->Index++;
			return *this;
		}

		/// @brief Assigns the iterator next element of the indexed container.
		/// @param Unused Unused parameter passed be compiler.
		/// @returns Iterator before incrementing.
		GDINL IndexedContainerIterator operator++ (int const Unused)
		{
			GD_NOT_USED(Unused);
			IndexedContainerIterator Copy(*this);
			this->Index++;
			return Copy;
		}

		/// @brief Assigns the iterator previous element of the indexed container.
		/// @returns Decremented iterator.
		GDINL IndexedContainerIterator& operator-- ()
		{
			this->Index--;
			return *this;
		}

		/// @brief Assigns the iterator previous element of the indexed container.
		/// @param Unused Unused parameter passed be compiler.
		/// @returns Iterator before decrementing.
		GDINL IndexedContainerIterator operator-- (int const Unused)
		{
			GD_NOT_USED(Unused);
			IndexedContainerIterator Copy(*this);
			this->Index--;
			return Copy;
		}

		/// @brief Positively advances iterator on specified value.
		/// @param Offset Advanced from current position.
		/// @returns Positively advanced iterator.
		/// @{
		GDINL IndexedContainerIterator& operator+= (ptrdiff_t const Offset)
		{
			this->Index += Offset;
			return *this;
		}
		GDINL IndexedContainerIterator operator+ (ptrdiff_t const Offset) const
		{
			IndexedContainerIterator Copy(*this);
			return Copy += Offset;
		}
		/// @}

		/// @brief Negatively advances iterator on specified value.
		/// @param Offset Advanced from current position.
		/// @returns Negatively advanced iterator.
		/// @{
		GDINL IndexedContainerIterator& operator-= (ptrdiff_t const Offset)
		{
			this->Index -= Offset;
			return *this;
		}
		GDINL IndexedContainerIterator operator- (ptrdiff_t const Offset) const
		{
			IndexedContainerIterator Copy(*this);
			return Copy -= Offset;
		}
		/// @}

		/// @brief Returns distance between this iterator and specified one.
		/// @param Other Some iterator to compute distance with.
		/// @returns Distance between this iterator and specified one. 
		GDINL ptrdiff_t operator- (IndexedContainerIterator const& Other) const
		{
			return this->Index - Other.Index;
		}

		/// @brief Compares two iterators on equality.
		/// @param Other Other iterator that would be compared.
		/// @returns True if iterators point to the same container at same indices, false otherwise.
		GDINL bool operator== (IndexedContainerIterator const& Other) const
		{
			return (&this->Container == &Other.Container) && (this->Index == Other.Index);
		}

		/// @brief Compares two iterators on inequality.
		/// @param Other Other iterator that would be compared.
		/// @returns False if iterators point to the same container at same indices, true otherwise.
		GDINL bool operator!= (IndexedContainerIterator const& Other) const
		{
			return (&this->Container != &Other.Container) || (this->Index != Other.Index);
		}

		/// @brief Dereferences value of the iterator.
		/// @returns Reference to the element of the container at iterator index.
		GDINL IndexedContainerElementType& operator* () const
		{
			return this->Container[this->Index];
		}

		/// @brief Dereferences value of the iterator.
		/// @returns Pointer to the element of the container at iterator index.
		GDINL IndexedContainerElementType* operator-> () const
		{
			return &this->Container[this->Index];
		}
	};	// struct IndexedContainerIterator

	/// @brief Adapter that inverses iterator of a specified type.
	/// @tparam DirectIteratorType Reversable bidirectional iterator type.
	template<typename DirectIteratorType>
	struct ReverseContainerIterator final
	{
	private:
		DirectIteratorType DirectIterator;

	public:

		/// @brief Initializes reverse iterator this direct one.
		/// @param DirectIterator Iterator that advancing operation would be inverted.
		GDINL explicit ReverseContainerIterator(DirectIteratorType DirectIterator)
			: DirectIterator(DirectIterator)
		{
		}

		/// @brief Assigns the iterator previous element of the container.
		/// @returns Incremented iterator.
		GDINL ReverseContainerIterator& operator++ ()
		{
			this->DirectIterator--;
			return *this;
		}

		/// @brief Assigns the iterator previous element of the container.
		/// @param Unused Unused parameter passed be compiler.
		/// @returns Iterator before incrementing.
		GDINL ReverseContainerIterator operator++ (int const Unused)
		{
			GD_NOT_USED(Unused);
			ReverseContainerIterator Copy(*this);
			this->DirectIterator--;
			return Copy;
		}

		/// @brief Assigns the iterator next element of the container.
		/// @returns Decremented iterator.
		GDINL ReverseContainerIterator& operator-- ()
		{
			this->DirectIterator++;
			return *this;
		}

		/// @brief Assigns the iterator next element of the container.
		/// @param Unused Unused parameter passed be compiler.
		/// @returns Iterator before decrementing.
		GDINL ReverseContainerIterator operator-- (int const Unused)
		{
			GD_NOT_USED(Unused);
			ReverseContainerIterator Copy(*this);
			this->DirectIterator++;
			return Copy;
		}

		/// @brief Negatively advances iterator on specified value.
		/// @param Offset Advanced from current position.
		/// @returns Negatively advanced iterator.
		/// @{
		GDINL ReverseContainerIterator& operator+= (ptrdiff_t const Offset)
		{
			this->DirectIterator -= Offset;
			return *this;
		}
		GDINL ReverseContainerIterator operator+ (ptrdiff_t const Offset) const
		{
			ReverseContainerIterator Copy(*this);
			return Copy += Offset;
		}
		/// @}

		/// @brief Positively advances iterator on specified value.
		/// @param Offset Advanced from current position.
		/// @returns Positively advanced iterator.
		/// @{
		GDINL ReverseContainerIterator& operator-= (ptrdiff_t const Offset)
		{
			this->DirectIterator += Offset;
			return *this;
		}
		GDINL ReverseContainerIterator operator- (ptrdiff_t const Offset) const
		{
			ReverseContainerIterator Copy(*this);
			return Copy -= Offset;
		}
		/// @}

		/// @brief Returns distance between this iterator and specified one.
		/// @param Other Some iterator to compute distance with.
		/// @returns Distance between this iterator and specified one. 
		GDINL ptrdiff_t operator- (ReverseContainerIterator const& Other) const
		{
			return this->DirectIterator - Other.DirectIterator;
		}

		/// @brief Compares two iterators on equality.
		/// @param Other Other iterator that would be compared.
		/// @returns True if direct iterators are same.
		GDINL bool operator== (ReverseContainerIterator const& Other) const
		{
			return this->DirectIterator == Other.DirectIterator;
		}

		/// @brief Compares two iterators on inequality.
		/// @param Other Other iterator that would be compared.
		/// @returns True if direct iterators are different.
		GDINL bool operator!= (ReverseContainerIterator const& Other) const
		{
			return this->DirectIterator != Other.DirectIterator;
		}

		/// @brief Dereferences value of the iterator.
		/// @returns Reference to the element of the container at iterator index.
		GDINL decltype(DeclValue<DirectIteratorType>().operator*()) operator* () const
		{
			return this->DirectIterator.operator*();
		}

		/// @brief Dereferences value of the iterator.
		/// @returns Pointer to the element of the container at iterator index.
		GDINL decltype(DeclValue<DirectIteratorType>().operator->()) operator-> () const
		{
			return this->DirectIterator.operator->();
		}
	};	// struct ReverseContainerIterator

	/// @brief Adapter that reverses direct iterator functions of specified type.
	/// @tparam ContainerType Direct-iteratable container type. 
	template<typename ContainerType>
	class ReverseContainerAdapter final
	{
	private:
		GD_CLASS_UNASSIGNABLE(ReverseContainerAdapter);
		ContainerType& Container;

	public:

		/// @brief Initializes an adapter that reverses direct iterator functions of specified type.
		/// @param Container Container that would be indirectly iterated.
		GDINL ReverseContainerAdapter(ContainerType& Container) 
			: Container(Container)
		{
		}

		/// @brief Initializes an adapter as copy of the other adapter.
		/// @param Other Other adapter that would be copied.
		GDINL ReverseContainerAdapter(ReverseContainerAdapter const& Other)
			: Container(Other.Container)
		{
		}

		/// @brief Returns iterator that points to first container element.
		/// @returns Iterator that points to first container element.
		GDINL auto Begin() const ->
			typename EnableIf<TypeTraits::IsBase<ContainerReverseIteratableTag, ContainerType>::Value, decltype(DeclValue<ContainerType>().ReverseBegin())>::Type
		{
			return this->Container.ReverseBegin(); 
		}

		/// @brief Returns iterator that points to past the end container element.
		/// @returns Iterator that points to past the end container element.
		GDINL auto End() const -> 
			typename EnableIf<TypeTraits::IsBase<ContainerReverseIteratableTag, ContainerType>::Value, decltype(DeclValue<ContainerType>().ReverseEnd())>::Type
		{ 
			return this->Container.ReverseEnd(); 
		}

		/// @brief Returns iterator that points to last container element.
		/// @returns Iterator that points to last container element.
		GDINL auto ReverseBegin() const ->
			typename EnableIf<TypeTraits::IsBase<ContainerIteratableTag, ContainerType>::Value, decltype(DeclValue<ContainerType>().Begin())>::Type
		{
			return this->Container.Begin();
		}

		/// @brief Returns iterator that points to preceding the first container element.
		/// @returns Iterator that points to preceding the first container element.
		GDINL auto ReverseEnd() const ->
			typename EnableIf<TypeTraits::IsBase<ContainerIteratableTag, ContainerType>::Value, decltype(DeclValue<ContainerType>().End())>::Type
		{
			return this->Container.End();
		}

	private:
		GDINL friend auto begin(ReverseContainerAdapter const& container) ->
			typename EnableIf<TypeTraits::IsBase<ContainerReverseIteratableTag, ContainerType>::Value, decltype(DeclValue<ContainerType>().ReverseBegin())>::Type
		{
			return container.Begin();
		}

		GDINL friend auto end(ReverseContainerAdapter const& container) ->
			typename EnableIf<TypeTraits::IsBase<ContainerReverseIteratableTag, ContainerType>::Value, decltype(DeclValue<ContainerType>().ReverseBegin())>::Type
		{
			return container.End();
		}

	};	// class ReverseContainerAdapter

	/// @brief Returns a version of the specified container with Begin and ReverseBegin, End and ReverseEnd functions reversed.
	/// @param Container Some container to invert.
	/// @returns A version of the specified container with Begin and ReverseBegin, End and ReverseEnd functions reversed.
	template<typename ContainerType>
	GDINL ReverseContainerAdapter<ContainerType> Reverse(ContainerType& Container)
	{	
		return ReverseContainerAdapter<ContainerType>(Container);
	}

	/// @brief Initializes value of the specified iterator.
	/// @param Iterator Value of this iterator would be initialized.
	template<typename IteratorType>
	GDINL static void InitializeIterator(IteratorType const Iterator)
	{
		typedef typename TypeTraits::RemoveReference<decltype(*Iterator)>::Type ElementType;
		if (!TypeTraits::IsPOD<ElementType>::Value) {
			new (Iterator) ElementType();
		}
	}

	/// @brief Initializes value of the specified iterator.
	/// @param Iterator Value of this iterator would be initialized.
	/// @param Element Initial value of the iterator.
	/// @{
	template<typename IteratorType, typename ElementType>
	GDINL static void InitializeIterator(IteratorType const Iterator, ElementType&& Element)
	{
		new (Iterator) ElementType(Forward<ElementType>(Element));
	}
	template<typename IteratorType, typename ElementType>
	GDINL static void InitializeIterator(IteratorType const Iterator, ElementType const& Element)
	{
		new (Iterator) ElementType(Element);
	}
	/// @}

	/// @brief Initializes all values of the iterators in specified range.
	/// @param StartIterator Start of the iterators range.
	/// @param EndIterator End of the iterators range.
	template<typename IteratorType>
	GDINL static void InitializeRange(IteratorType const StartIterator, IteratorType const EndIterator)
	{
		typedef typename TypeTraits::RemoveReference<decltype(*StartIterator)>::Type ElementType;
		if (!TypeTraits::IsPOD<ElementType>::Value) {
			for (IteratorType Iterator = StartIterator; Iterator != EndIterator; ++Iterator) {
				InitializeIterator(Iterator);
			}
		}
	}

	/// @brief Deinitializes value of the specified iterator.
	/// @param Iterator Value of this iterator would be initialized.
	template<typename IteratorType>
	GDINL static void DeinitializeIterator(IteratorType const Iterator)
	{
		typedef typename TypeTraits::RemoveReference<decltype(*Iterator)>::Type ElementType;
		if (!TypeTraits::IsPOD<ElementType>::Value) {
			Iterator->~ElementType();
		}
	}

	/// @brief Deinitializes all values of the iterators in specified range.
	/// @param StartIterator Start of the iterators range.
	/// @param EndIterator End of the iterators range.
	template<typename IteratorType>
	GDINL static void DeinitializeRange(IteratorType const StartIterator, IteratorType const EndIterator)
	{
		typedef typename TypeTraits::RemoveReference<decltype(*StartIterator)>::Type ElementType;
		if (!TypeTraits::IsPOD<ElementType>::Value) {
			for (IteratorType Iterator = StartIterator; Iterator != EndIterator; ++Iterator) {
				DeinitializeIterator(Iterator);
			}
		}
	}

	/// @brief Copies values of the iterators from source range to destination.
	/// @param StartSource Start of the source iterators range.
	/// @param EndSource End of the source iterators range.
	/// @param Destination First destination iterator.
	template<typename SourceIteratorType, typename DestinationIteratorType>
	GDINL static void CopyFromRange(SourceIteratorType const& StartSource, SourceIteratorType const& EndSource, DestinationIteratorType Destination)
	{
		for (SourceIteratorType Iterator = StartSource; Iterator != EndSource; ++Iterator) {
			InitializeIterator(Destination + (Iterator - StartSource), *Iterator);
		}
	}

	/// @brief Moves values of the iterators from source range to destination.
	/// @param StartSource Start of the source iterators range.
	/// @param EndSource End of the source iterators range.
	/// @param Destination First destination iterator.
	template<typename SourceIteratorType, typename DestinationIteratorType>
	GDINL static void MoveFromRange(SourceIteratorType const& StartSource, SourceIteratorType const& EndSource, DestinationIteratorType Destination)
	{
		typedef typename TypeTraits::RemoveReference<decltype(*Destination)>::Type ElementType;
		for (SourceIteratorType Iterator = StartSource; Iterator != EndSource; ++Iterator) {
			InitializeIterator(Destination + (Iterator - StartSource), Forward<ElementType>(*Iterator));
		}
	}

	/// @brief Compares this container and some other by a predicate.
	/// @param OtherVector Other container against which we are comparing.
	/// @param Predicate Object with () operator overloaded that takes two elements and compares then somehow.
	template<typename ContainerType, typename ComparingPredicateType>
	GDINL bool CompareTo(ContainerType const& First, ContainerType const& Second, ComparingPredicateType const& Predicate)
	{
		size_t const MinLength = Min(First.Length, Second.Length);
		for (size_t Index = 0; Index < MinLength; ++Index) {
			if (!Predicate(*(First.PtrBegin() + Index), *(Second.PtrBegin() + Index))) {
				return false;
			}
		}

		return true;
	}

GD_NAMESPACE_END

#endif	// ifndef GD_CORE_CONTAINERS
