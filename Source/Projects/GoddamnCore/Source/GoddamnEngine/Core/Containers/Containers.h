/// ==========================================================================================
/// Containers.h - common algorithms for containers interfaces.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_CONTAINERS
#define GD_CORE_CONTAINERS

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Utility.h>

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
		UnusableType();
		~UnusableType();
	};	// struct UnusableType

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
	static_assert(TypeTraits::IsBaseType<ContainerIteratableTag, ThisType>::Value, "'GD_CONTAINER_CHECK_ITERATOR' Is supported only for 'ContainerIteratableTag' types"); \
	static_assert(!TypeTraits::IsSameType<Iterator, UnusableType>::Value, "'Iterator' type is not defined."); \
	static_assert(!TypeTraits::IsSameType<ConstIterator, UnusableType>::Value, "'ConstIterator' type is not defined."); \
	/** @todo Add checks for valid Begin/End methods. */ \

#define GD_CONTAINER_CHECK_REVERSE_ITERATORS(ThisType) \
	/** @todo Implement checks here.. */ \

#define GD_CONTAINER_CHECK_REVERSE_ITERATORS(ThisType) \
	/** @todo Implement checks here.. */ \

#define GD_CONTAINER_CHECK_PTR_ITERATORS(ThisType) \
	/** @todo Implement checks here.. */ \

#define GD_CONTAINER_CHECK_REVERSE_PTR_ITERATORS(ThisType) \
	/** @todo Implement checks here.. */ \

	/// @brief Iterator for container that have constant-time (preferably) index access operators.
	/// @tparam IndexedContainerType Type of container.
	/// @tparam IndexedContainerElementType Type of container element type.
	template<typename IndexedContainerType, typename IndexedContainerElementType>
	struct IndexedContainerIterator final
	{
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
		/// @returns Self.
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
		GDINL IndexedContainerIterator operator-- (int const)
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
		GD_CLASS_UNSWAPPABLE(ReverseContainerAdapter);
		ContainerType& Container;

	public:

		/// @brief Initializes an adapter that reverses direct iterator functions of specified type.
		/// @param Container Container that would be indirectly iterated.
		GDINL ReverseContainerAdapter(ContainerType& Container) 
			: Container(Container)
		{
		}

		/// @brief Returns iterator that points to first container element.
		/// @returns Iterator that points to first container element.
		GDINL auto Begin() const ->
			typename EnableIf<TypeTraits::IsBaseType<ContainerReverseIteratableTag, ContainerType>::Value, decltype(DeclValue<ContainerType>().ReverseBegin())>::Type
		{
			return this->Container.ReverseBegin(); 
		}

		/// @brief Returns iterator that points to past the end container element.
		/// @returns Iterator that points to past the end container element.
		GDINL auto End() const -> 
			typename EnableIf<TypeTraits::IsBaseType<ContainerReverseIteratableTag, ContainerType>::Value, decltype(DeclValue<ContainerType>().ReverseEnd())>::Type
		{ 
			return this->Container.ReverseEnd(); 
		}

		/// @brief Returns iterator that points to last container element.
		/// @returns Iterator that points to last container element.
		GDINL auto ReverseBegin() const ->
			typename EnableIf<TypeTraits::IsBaseType<ContainerIteratableTag, ContainerType>::Value, decltype(DeclValue<ContainerType>().Begin())>::Type
		{
			return this->Container.Begin();
		}

		/// @brief Returns iterator that points to preceding the first container element.
		/// @returns Iterator that points to preceding the first container element.
		GDINL auto ReverseEnd() const ->
			typename EnableIf<TypeTraits::IsBaseType<ContainerIteratableTag, ContainerType>::Value, decltype(DeclValue<ContainerType>().End())>::Type
		{
			return this->Container.End();
		}

	private:
		GDINL friend auto begin(ReverseContainerAdapter const& container) ->
			typename EnableIf<TypeTraits::IsBaseType<ContainerReverseIteratableTag, ContainerType>::Value, decltype(DeclValue<ContainerType>().ReverseBegin())>::Type
		{
			return container.Begin();
		}

		GDINL friend auto end(ReverseContainerAdapter const& container) ->
			typename EnableIf<TypeTraits::IsBaseType<ContainerReverseIteratableTag, ContainerType>::Value, decltype(DeclValue<ContainerType>().ReverseBegin())>::Type
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
		return ReverseContainerAdapter<typename TypeTraits::IsReference<ContainerType>::Type>(Container);
	}

GD_NAMESPACE_END

#endif	// ifndef GD_CORE_CONTAINERS
