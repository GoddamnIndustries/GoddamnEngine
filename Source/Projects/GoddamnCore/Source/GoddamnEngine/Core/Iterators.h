// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Templates/Iterators.h
//! Common algorithms for container iterator interfaces.
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Utility.h>

// ------------------------------------------------------------------------------------------
//! Adds support of ranged-for iteration to the container. 
//! @param this_type Type of this container. Should contain const and mutable versions of 'Begin' and 'End' methods.
//! @note If this container has '_PtrBegin' and '_PtrEnd' methods, preferably to use @c 'GD_CONTAINER_DEFINE_PTR_ITERATION_SUPPORT' macro instead.
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
//! @param this_type Type of this container. Should contain const and mutable versions of '_PtrBegin' and '_PtrEnd' methods.
#define GD_CONTAINER_DEFINE_PTR_ITERATION_SUPPORT(this_type) \
	GDINL friend PtrIterator begin(this_type& some_container) \
	{ \
		return some_container._PtrBegin(); \
	} \
	GDINL friend PtrConstIterator begin(this_type const& some_container) \
	{ \
		return some_container._PtrBegin(); \
	} \
	GDINL friend PtrIterator end(this_type& some_container) \
	{ \
		return some_container._PtrEnd(); \
	} \
	GDINL friend PtrConstIterator end(this_type const& some_container) \
	{ \
		return some_container._PtrEnd(); \
	} \

GD_NAMESPACE_BEGIN
	
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Iterator traits.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	template<typename IteratorType>
	struct IteratorTraits final
	{
		enum { IsForward = 1 };
		enum { IsBidirectional = 1 };
		enum { IsRandomAccess = 1 };
	};	// struct IteratorTraits

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
		using ContainerType = ContainerTypeTp;
		using ElementType = typename Conditional<TypeTraits::IsConst<ContainerType>::Value
			, typename ContainerType::ElementType const
			, typename ContainerType::ElementType>::Type;

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
			IndexedContainerIterator Copy(*this);
			++this->Index;
			return Copy;
		}

		// ------------------------------------------------------------------------------------------
		//! Assigns the iterator previous element of the indexed container.
		//! @returns Decremented iterator.
		GDINL IndexedContainerIterator& operator-- ()
		{
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
			IndexedContainerIterator Copy(*this);
			--this->Index;
			return Copy;
		}

		// ------------------------------------------------------------------------------------------
		//! Positively advances iterator on specified value.
		//! @param Offset Advanced from current position.
		//! @returns Positively advanced iterator.
		//! @{
		GDINL IndexedContainerIterator& operator+= (PtrDiffTp const Offset)
		{
			this->Index += Offset;
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
			this->Index -= Offset;
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
		using DirectIteratorType = DirectIteratorTypeTp;
		using ContainerType      = typename DirectIteratorType::ContainerType;
		using ElementType        = typename Conditional<TypeTraits::IsPointer<DirectIteratorTypeTp>::Value
			, typename TypeTraits::RemovePointer<DirectIteratorTypeTp>::Type
			, typename DirectIteratorType::ElementType>::Type;

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
	//! @tparam ContainerType Direct-treatable container type. 
	template<typename ContainerTypeTp>
	class ReverseContainerAdapter final
	{
	public:
		using ContainerType        = ContainerTypeTp;
		using ElementType          = typename ContainerType::ElementType;
		using Iterator             = decltype(DeclValue<ContainerType>().ReverseBegin());
		using ReverseIterator      = decltype(DeclValue<ContainerType>().Begin());
		using ConstIterator        = Iterator;
		using ReverseConstIterator = ReverseIterator;

		GD_CONTAINER_DEFINE_ITERATION_SUPPORT(ReverseContainerAdapter);

	private:
		ContainerType* Container;	// should be stored by pointer to make this adapter copiable and assignable.

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructors.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Initializes an adapter that reverses direct iterator functions of specified type.
		//! @param Container Container that would be indirectly iterated.
		GDINL explicit ReverseContainerAdapter(ContainerType& Container)
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
	//! Returns iterator that points to first container element.
	//! @returns Iterator that points to first container element.
	//! @{
	template<typename ContainerType>
	GDINL static auto Begin(ContainerType const& Container) -> decltype(Container.Begin())
	{
		return Container.Begin();
	}
	template<typename ContainerType>
	GDINL static auto Begin(ContainerType& Container) -> decltype(Container.Begin())
	{
		return Container.Begin();
	}
	template<typename ArrayElementType, SizeTp ArrayLength>
	GDINL static ArrayElementType* Begin(ArrayElementType(&Array)[ArrayLength])
	{
		return &Array[0];
	}
	//! @}

	// ------------------------------------------------------------------------------------------
	//! Returns iterator that points to past the end container element.
	//! @returns Iterator that points to past the end container element.
	//! @{
	template<typename ContainerType>
	GDINL static auto End(ContainerType const& Container) -> decltype(Container.End())
	{
		return Container.End();
	}
	template<typename ContainerType>
	GDINL static auto End(ContainerType& Container) -> decltype(Container.End())
	{
		return Container.End();
	}
	template<typename ArrayElementType, SizeTp ArrayLength>
	GDINL static ArrayElementType* End(ArrayElementType(&Array)[ArrayLength])
	{
		return &Array[ArrayLength];
	}
	//! @}

	// ------------------------------------------------------------------------------------------
	//! Returns a version of the specified container with Begin and ReverseBegin, End and ReverseEnd functions reversed.
	//! @param Container Some container to invert.
	//! @returns A version of the specified container with Begin and ReverseBegin, End and ReverseEnd functions reversed.
	template<typename ContainerType>
	GDINL ReverseContainerAdapter<ContainerType> Reverse(ContainerType& Container)
	{
		return ReverseContainerAdapter<ContainerType>(Container);
	}

GD_NAMESPACE_END
