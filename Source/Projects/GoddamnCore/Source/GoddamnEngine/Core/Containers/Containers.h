// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Containers/Containers.h
//! Common algorithms for containers interfaces.
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/Utility.h>

// ------------------------------------------------------------------------------------------
//! Adds support of ranged-for iteration to the container. 
//! @param this_type Type of this container. Should contain const and mutable versions of 'Begin' and 'End' methods.
//! @note If this container has 'PtrBegin' and 'PtrEnd' methods, preferably to use @c 'GD_CONTAINER_DEFINE_PTR_ITERATION_SUPPORT' macro instead.
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

// ------------------------------------------------------------------------------------------
//! Adds support of ranged-for iteration to the container. 
//! @param this_type Type of this container. Should contain const and mutable versions of 'PtrBegin' and 'PtrEnd' methods.
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

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// IndexedContainerIterator<U, T> class.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Iterator for container that have constant-time (preferably) index access operators.
	//! @tparam ContainerType Type of container.
	template<typename ContainerTypeTp>
	struct IndexedContainerIterator final
	{
	public:
		typedef ContainerTypeTp																																			ContainerType;
		typedef typename Conditional<TypeTraits::IsConst<ContainerType>::Value, typename ContainerType::ElementType const, typename ContainerType::ElementType>::Type	ElementType;

	private:
		ContainerType&	Container;
		SizeTp			Index = 0;

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructor.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Initializes iterator that points on the first element of the specified container.
		//! @param Container Container that is going to be iterated.
		GDINL explicit IndexedContainerIterator(ContainerType& Container)
			: Container(Container), Index(0)
		{
		}

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Overloaded operators.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Assigns this iterator other one.
		//! @param Other Other iterator to assign.
		//! @returns this.
		GDINL IndexedContainerIterator& operator= (IndexedContainerIterator const& Other)
		{
			GD_ASSERT(&this->Container == &Other.Container, "Iterators have different base containers.");
			this->Index = Other.Index;
			return *this;
		}

		// ------------------------------------------------------------------------------------------
		//! Assigns the iterator next element of the indexed container.
		//! @returns Incremented iterator.
		GDINL IndexedContainerIterator& operator++ ()
		{
			if (*this != this->Container.End())
				++this->Index;
			return *this;
		}

		// ------------------------------------------------------------------------------------------
		//! Assigns the iterator next element of the indexed container.
		//! @param Unused Unused parameter passed be compiler.
		//! @returns Iterator before incrementing.
		GDINL IndexedContainerIterator operator++ (int const Unused)
		{
			GD_NOT_USED(Unused);
			if (*this != this->Container.End())
			{
				IndexedContainerIterator Copy(*this);
				++this->Index;
				return Copy;
			}
			return *this;
		}

		// ------------------------------------------------------------------------------------------
		//! Assigns the iterator previous element of the indexed container.
		//! @returns Decremented iterator.
		GDINL IndexedContainerIterator& operator-- ()
		{
			if (this->Index != SizeTpMax)
				--this->Index;
			return *this;
		}

		// ------------------------------------------------------------------------------------------
		//! Assigns the iterator previous element of the indexed container.
		//! @param Unused Unused parameter passed be compiler.
		//! @returns Iterator before decrementing.
		GDINL IndexedContainerIterator operator-- (int const Unused)
		{
			GD_NOT_USED(Unused);
			if (*this != this->Container.Begin() - 1)
			{
				IndexedContainerIterator Copy(*this);
				--this->Index;
				return Copy;
			}
			return *this;
		}

		// ------------------------------------------------------------------------------------------
		//! Positively advances iterator on specified value.
		//! @param Offset Advanced from current position.
		//! @returns Positively advanced iterator.
		//! @{
		GDINL IndexedContainerIterator& operator+= (PtrDiffTp const Offset)
		{
			if ((this->Index += Offset) > this->Container.GetLength())
				return *this = this->Container.End();
			return *this;
		}
		GDINL IndexedContainerIterator operator+ (PtrDiffTp const Offset) const
		{
			IndexedContainerIterator Copy(*this);
			return Copy += Offset;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Negatively advances iterator on specified value.
		//! @param Offset Advanced from current position.
		//! @returns Negatively advanced iterator.
		//! @{
		GDINL IndexedContainerIterator& operator-= (PtrDiffTp const Offset)
		{
			if ((this->Index -= Offset) < -1)
				return (*this = (this->Container.Begin() - 1));
			return *this;
		}
		GDINL IndexedContainerIterator operator- (PtrDiffTp const Offset) const
		{
			IndexedContainerIterator Copy(*this);
			return Copy -= Offset;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns distance between this iterator and specified one.
		//! @param Other Some iterator to compute distance with.
		//! @returns Distance between this iterator and specified one. 
		GDINL PtrDiffTp operator- (IndexedContainerIterator const& Other) const
		{
			return this->Index - Other.Index;
		}

		// ------------------------------------------------------------------------------------------
		//! Compares two iterators on equality.
		//! @param Other Other iterator that would be compared.
		//! @returns True if iterators point to the same container at same indices, false otherwise.
		GDINL bool operator== (IndexedContainerIterator const& Other) const
		{
			return (&this->Container == &Other.Container) && (this->Index == Other.Index);
		}

		// ------------------------------------------------------------------------------------------
		//! Compares two iterators on inequality.
		//! @param Other Other iterator that would be compared.
		//! @returns False if iterators point to the same container at same indices, true otherwise.
		GDINL bool operator!= (IndexedContainerIterator const& Other) const
		{
			return (&this->Container != &Other.Container) || (this->Index != Other.Index);
		}

		// ------------------------------------------------------------------------------------------
		//! Dereferences value of the iterator.
		//! @returns Reference to the element of the container at iterator index.
		GDINL ElementType& operator* () const
		{
			return this->Container[this->Index];
		}

		// ------------------------------------------------------------------------------------------
		//! Dereferences value of the iterator.
		//! @returns Pointer to the element of the container at iterator index.
		GDINL ElementType* operator-> () const
		{
			return &this->Container[this->Index];
		}
	};	// struct IndexedContainerIterator

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ReverseContainerIterator<T> class.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Adapter that inverses iterator of a specified type.
	//! @tparam DirectIteratorType Reversible bidirectional iterator type.
	template<typename DirectIteratorTypeTp>
	struct ReverseContainerIterator final
	{
	public:
		typedef DirectIteratorTypeTp						DirectIteratorType;
		typedef typename DirectIteratorType::ContainerType	ContainerType;
		typedef typename Conditional<TypeTraits::IsPointer<DirectIteratorTypeTp>::Value, typename TypeTraits::RemovePointer<DirectIteratorTypeTp>::Type, typename DirectIteratorType::ElementType>::Type ElementType;
	//	typedef typename DirectIteratorType::ElementType	ElementType;

	private:
		DirectIteratorType DirectIterator;

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructor.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Initializes reverse iterator this direct one.
		//! @param DirectIterator Iterator that advancing operation would be inverted.
		GDINL explicit ReverseContainerIterator(DirectIteratorType DirectIterator)
			: DirectIterator(DirectIterator)
		{
		}

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Overloaded operators.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Assigns the iterator previous element of the container.
		//! @returns Incremented iterator.
		GDINL ReverseContainerIterator& operator++ ()
		{
			--this->DirectIterator;
			return *this;
		}

		// ------------------------------------------------------------------------------------------
		//! Assigns the iterator previous element of the container.
		//! @param Unused Unused parameter passed be compiler.
		//! @returns Iterator before incrementing.
		GDINL ReverseContainerIterator operator++ (int const Unused)
		{
			GD_NOT_USED(Unused);
			ReverseContainerIterator Copy(*this);
			--this->DirectIterator;
			return Copy;
		}

		// ------------------------------------------------------------------------------------------
		//! Assigns the iterator next element of the container.
		//! @returns Decremented iterator.
		GDINL ReverseContainerIterator& operator-- ()
		{
			++this->DirectIterator;
			return *this;
		}

		// ------------------------------------------------------------------------------------------
		//! Assigns the iterator next element of the container.
		//! @param Unused Unused parameter passed be compiler.
		//! @returns Iterator before decrementing.
		GDINL ReverseContainerIterator operator-- (int const Unused)
		{
			GD_NOT_USED(Unused);
			ReverseContainerIterator Copy(*this);
			++this->DirectIterator;
			return Copy;
		}

		// ------------------------------------------------------------------------------------------
		//! Negatively advances iterator on specified value.
		//! @param Offset Advanced from current position.
		//! @returns Negatively advanced iterator.
		//! @{
		GDINL ReverseContainerIterator& operator+= (PtrDiffTp const Offset)
		{
			this->DirectIterator -= Offset;
			return *this;
		}
		GDINL ReverseContainerIterator operator+ (PtrDiffTp const Offset) const
		{
			ReverseContainerIterator Copy(*this);
			return Copy += Offset;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Positively advances iterator on specified value.
		//! @param Offset Advanced from current position.
		//! @returns Positively advanced iterator.
		//! @{
		GDINL ReverseContainerIterator& operator-= (PtrDiffTp const Offset)
		{
			this->DirectIterator += Offset;
			return *this;
		}
		GDINL ReverseContainerIterator operator- (PtrDiffTp const Offset) const
		{
			ReverseContainerIterator Copy(*this);
			return Copy -= Offset;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns distance between this iterator and specified one.
		//! @param Other Some iterator to compute distance with.
		//! @returns Distance between this iterator and specified one. 
		GDINL PtrDiffTp operator- (ReverseContainerIterator const& Other) const
		{
			return this->DirectIterator - Other.DirectIterator;
		}

		// ------------------------------------------------------------------------------------------
		//! Compares two iterators on equality.
		//! @param Other Other iterator that would be compared.
		//! @returns True if direct iterators are same.
		GDINL bool operator== (ReverseContainerIterator const& Other) const
		{
			return this->DirectIterator == Other.DirectIterator;
		}

		// ------------------------------------------------------------------------------------------
		//! Compares two iterators on inequality.
		//! @param Other Other iterator that would be compared.
		//! @returns True if direct iterators are different.
		GDINL bool operator!= (ReverseContainerIterator const& Other) const
		{
			return this->DirectIterator != Other.DirectIterator;
		}

		// ------------------------------------------------------------------------------------------
		//! Dereferences value of the iterator.
		//! @returns Reference to the element of the container at iterator index.
		GDINL ElementType& operator* () const
		{
			return this->DirectIterator.operator*();
		}

		// ------------------------------------------------------------------------------------------
		//! Dereferences value of the iterator.
		//! @returns Pointer to the element of the container at iterator index.
		GDINL ElementType* operator-> () const
		{
			return this->DirectIterator.operator->();
		}
	};	// struct ReverseContainerIterator

	// ------------------------------------------------------------------------------------------
	//! Adapter that reverses direct iterator functions of specified type.
	//! @tparam ContainerType Direct-iteratable container type. 
	template<typename ContainerTypeTp>
	class ReverseContainerAdapter final
	{
	public:
		typedef ContainerTypeTp										ContainerType;
		typedef typename ContainerType::ElementType					ElementType;
		typedef decltype(DeclValue<ContainerType>().ReverseBegin())	Iterator;
		typedef decltype(DeclValue<ContainerType>().Begin())		ReverseIterator;
		typedef	Iterator											ConstIterator;
		typedef ReverseIterator										ReverseConstIterator;

		GD_CONTAINER_DEFINE_ITERATION_SUPPORT(ReverseContainerAdapter);

	private:
		ContainerType* Container;

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructors.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Initializes an adapter that reverses direct iterator functions of specified type.
		//! @param Container Container that would be indirectly iterated.
		GDINL ReverseContainerAdapter(ContainerType& Container)
			: Container(&Container)
		{
		}

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Iteration API.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Returns iterator that points to first container element.
		//! @returns Iterator that points to first container element.
		//! @{
		GDINL Iterator Begin() { return this->Container->ReverseBegin(); }
		GDINL ConstIterator Begin() const { return this->Container->ReverseBegin(); }
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns iterator that points to past the end container element.
		//! @returns Iterator that points to past the end container element.
		//! @{
		GDINL Iterator End() { return this->Container->ReverseEnd(); }
		GDINL ConstIterator End() const { return this->Container->ReverseEnd(); }
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns iterator that points to last container element.
		//! @returns Iterator that points to last container element.
		//! @{
		GDINL ReverseIterator ReverseBegin() { return this->Container->Begin(); }
		GDINL ReverseConstIterator ReverseBegin() const { return this->Container->Begin(); }
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns iterator that points to preceding the first container element.
		//! @returns Iterator that points to preceding the first container element.
		//! @{
		GDINL ReverseIterator ReverseEnd() { return this->Container->End(); }
		GDINL ReverseConstIterator ReverseEnd() const { return this->Container->End(); }
		//! @}
	};	// class ReverseContainerAdapter

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Containers-specific algorithms.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Returns a version of the specified container with Begin and ReverseBegin, End and ReverseEnd functions reversed.
	//! @param Container Some container to invert.
	//! @returns A version of the specified container with Begin and ReverseBegin, End and ReverseEnd functions reversed.
	template<typename ContainerType>
	GDINL ReverseContainerAdapter<ContainerType> Reverse(ContainerType& Container)
	{	
		return ReverseContainerAdapter<ContainerType>(Container);
	}

	// ------------------------------------------------------------------------------------------
	//! Initializes value of the specified iterator.
	//! @param Iterator Value of this iterator would be initialized.
	template<typename IteratorType>
	GDINL static void InitializeIterator(IteratorType const Iterator)
	{
		typedef typename TypeTraits::RemoveReference<decltype(*Iterator)>::Type ElementType;
		if (!TypeTraits::IsPOD<ElementType>::Value) 
			new (Iterator) ElementType();
	}

	// ------------------------------------------------------------------------------------------
	//! Initializes value of the specified iterator.
	//! @param Iterator Value of this iterator would be initialized.
	//! @param Element Initial value of the iterator.
	//! @{
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
	//! @}

	// ------------------------------------------------------------------------------------------
	//! Initializes all values of the iterators in specified range.
	//! @param StartIterator Start of the iterators range.
	//! @param EndIterator End of the iterators range.
	template<typename IteratorType>
	GDINL static void InitializeRange(IteratorType const StartIterator, IteratorType const EndIterator)
	{
		typedef typename TypeTraits::RemoveReference<decltype(*StartIterator)>::Type ElementType;
		if (!TypeTraits::IsPOD<ElementType>::Value) 
			for (IteratorType Iterator = StartIterator; Iterator != EndIterator; ++Iterator) 
				InitializeIterator(Iterator);
	}

	// ------------------------------------------------------------------------------------------
	//! Deinitializes value of the specified iterator.
	//! @param Iterator Value of this iterator would be initialized.
	template<typename IteratorType>
	GDINL static void DeinitializeIterator(IteratorType const Iterator)
	{
		typedef typename TypeTraits::RemoveReference<decltype(*Iterator)>::Type ElementType;
		if (!TypeTraits::IsPOD<ElementType>::Value) 
			Iterator->~ElementType();
	}

	// ------------------------------------------------------------------------------------------
	//! Deinitializes all values of the iterators in specified range.
	//! @param StartIterator Start of the iterators range.
	//! @param EndIterator End of the iterators range.
	template<typename IteratorType>
	GDINL static void DeinitializeRange(IteratorType const StartIterator, IteratorType const EndIterator)
	{
		typedef typename TypeTraits::RemoveReference<decltype(*StartIterator)>::Type ElementType;
		if (!TypeTraits::IsPOD<ElementType>::Value) 
			for (IteratorType Iterator = StartIterator; Iterator != EndIterator; ++Iterator) 
				DeinitializeIterator(Iterator);
	}

	// ------------------------------------------------------------------------------------------
	//! Copies values of the iterators from source range to destination.
	//! @param StartSource Start of the source iterators range.
	//! @param EndSource End of the source iterators range.
	//! @param Destination First destination iterator.
	template<typename SourceIteratorType, typename DestinationIteratorType>
	GDINL static void CopyFromRange(SourceIteratorType const& StartSource, SourceIteratorType const& EndSource, DestinationIteratorType Destination)
	{
		for (SourceIteratorType Iterator = StartSource; Iterator != EndSource; ++Iterator) 
			InitializeIterator(Destination + (Iterator - StartSource), *Iterator);
	}

	// ------------------------------------------------------------------------------------------
	//! Moves values of the iterators from source range to destination.
	//! @param StartSource Start of the source iterators range.
	//! @param EndSource End of the source iterators range.
	//! @param Destination First destination iterator.
	template<typename SourceIteratorType, typename DestinationIteratorType>
	GDINL static void MoveFromRange(SourceIteratorType const& StartSource, SourceIteratorType const& EndSource, DestinationIteratorType Destination)
	{
		typedef typename TypeTraits::RemoveReference<decltype(*Destination)>::Type ElementType;
		for (SourceIteratorType Iterator = StartSource; Iterator != EndSource; ++Iterator) 
			InitializeIterator(Destination + (Iterator - StartSource), Forward<ElementType>(*Iterator));
	}

	// ------------------------------------------------------------------------------------------
	//! Compares this container and some other by a predicate.
	//! @param OtherVector Other container against which we are comparing.
	//! @param Predicate Object with () operator overloaded that takes two elements and compares then somehow.
	template<typename ContainerType, typename ComparingPredicateType>
	GDINL bool CompareTo(ContainerType const& First, ContainerType const& Second, ComparingPredicateType const& Predicate)
	{
		SizeTp const MinLength = Min(First.GetLength(), Second.GetLength());
		for (SizeTp Index = 0; Index < MinLength; ++Index) 
			if (!Predicate(*(First.Begin() + Index), *(Second.Begin() + Index))) 
				return false;
		return true;
	}

GD_NAMESPACE_END
