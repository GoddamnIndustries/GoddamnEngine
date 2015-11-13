// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Iterators.h
//! Common algorithms for m_Container Iterator interfaces.
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Utility.h>

// ------------------------------------------------------------------------------------------
//! Adds support of ranged-for iteration to the container. 
//! @param this_type Type of this m_Container. Should contain const and mutable versions of 'Begin' and 'End' methods.
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
	//! Iterator for m_Container that have constant-time (preferably) index access operators.
	//! @tparam ContainerType Type of m_Container.
	template<typename TContainer>
	struct IndexedContainerIterator final
	{
	public:
		using ContainerType = TContainer;
		using ElementType   = typename Conditional<TypeTraits::IsConst<TContainer>::Value
			, typename TContainer::ElementType const
			, typename TContainer::ElementType>::Type;

	private:
		TContainer&	m_Container;
		SizeTp		m_Index = 0;

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructor.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Initializes Iterator that points on the first element of the specified m_Container.
		//! @param container The container that is going to be iterated.
		GDINL explicit IndexedContainerIterator(TContainer& container)
			: m_Container(container), m_Index(0)
		{
		}
			
	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Overloaded operators.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		GDINL IndexedContainerIterator& operator= (IndexedContainerIterator const& other)
		{
			GD_ASSERT(&m_Container == &other.m_Container, "Iterators have different base containers.");
			m_Index = other.m_Index;
			return *this;
		}

		GDINL IndexedContainerIterator& operator++ ()
		{
			++m_Index;
			return *this;
		}
		GDINL IndexedContainerIterator operator++ (int const unused)
		{
			GD_NOT_USED(unused);
			IndexedContainerIterator copy(*this);
			++m_Index;
			return copy;
		}

		GDINL IndexedContainerIterator& operator-- ()
		{
			--m_Index;
			return *this;
		}
		GDINL IndexedContainerIterator operator-- (int const unused)
		{
			GD_NOT_USED(unused);
			IndexedContainerIterator copy(*this);
			--m_Index;
			return copy;
		}

		GDINL IndexedContainerIterator& operator+= (PtrDiffTp const offset)
		{
			m_Index += offset;
			return *this;
		}
		GDINL IndexedContainerIterator operator+ (PtrDiffTp const offset) const
		{
			IndexedContainerIterator copy(*this);
			return copy += offset;
		}

		GDINL IndexedContainerIterator& operator-= (PtrDiffTp const offset)
		{
			m_Index -= offset;
			return *this;
		}
		GDINL IndexedContainerIterator operator- (PtrDiffTp const offset) const
		{
			IndexedContainerIterator copy(*this);
			return copy -= offset;
		}
		GDINL PtrDiffTp operator- (IndexedContainerIterator const& other) const
		{
			return m_Index - other.m_Index;
		}

		GDINL bool operator== (IndexedContainerIterator const& other) const
		{
			return (&m_Container == &other.m_Container) && (m_Index == other.m_Index);
		}
		GDINL bool operator!= (IndexedContainerIterator const& other) const
		{
			return (&m_Container != &other.m_Container) || (m_Index != other.m_Index);
		}

		GDINL ElementType& operator* () const
		{
			return m_Container[m_Index];
		}
		GDINL ElementType* operator-> () const
		{
			return &m_Container[m_Index];
		}
	};	// struct IndexedContainerIterator

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ReverseContainerIterator<T> class.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Adapter that inverses Iterator of a specified type.
	//! @tparam DirectIteratorType Reversible bidirectional Iterator type.
	template<typename TDirectIterator>
	struct ReverseContainerIterator final
	{
	public:
		using DirectIteratorType = TDirectIterator;
		using ContainerType      = typename DirectIteratorType::ContainerType;
		using ElementType        = typename Conditional<TypeTraits::IsPointer<TDirectIterator>::Value
			, typename TypeTraits::RemovePointer<TDirectIterator>::Type
			, typename DirectIteratorType::ElementType>::Type;

	private:
		DirectIteratorType directIterator;

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructor.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Initializes reverse Iterator this direct one.
		//! @param directIterator Iterator that advancing operation would be inverted.
		GDINL explicit ReverseContainerIterator(DirectIteratorType DirectIterator)
			: directIterator(DirectIterator)
		{
		}

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Overloaded operators.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		GDINL ReverseContainerIterator& operator++ ()
		{
			--directIterator;
			return *this;
		}
		GDINL ReverseContainerIterator operator++ (int const unused)
		{
			GD_NOT_USED(unused);
			ReverseContainerIterator copy(*this);
			--directIterator;
			return copy;
		}

		GDINL ReverseContainerIterator& operator-- ()
		{
			++directIterator;
			return *this;
		}
		GDINL ReverseContainerIterator operator-- (int const unused)
		{
			GD_NOT_USED(unused);
			ReverseContainerIterator copy(*this);
			++directIterator;
			return copy;
		}

		GDINL ReverseContainerIterator& operator+= (PtrDiffTp const offset)
		{
			directIterator -= offset;
			return *this;
		}
		GDINL ReverseContainerIterator operator+ (PtrDiffTp const offset) const
		{
			ReverseContainerIterator copy(*this);
			return copy += offset;
		}

		GDINL ReverseContainerIterator& operator-= (PtrDiffTp const offset)
		{
			directIterator += offset;
			return *this;
		}
		GDINL ReverseContainerIterator operator- (PtrDiffTp const offset) const
		{
			ReverseContainerIterator copy(*this);
			return copy -= offset;
		}
		GDINL PtrDiffTp operator- (ReverseContainerIterator const& other) const
		{
			return directIterator - other.directIterator;
		}

		GDINL bool operator== (ReverseContainerIterator const& other) const
		{
			return directIterator == other.directIterator;
		}
		GDINL bool operator!= (ReverseContainerIterator const& other) const
		{
			return directIterator != other.directIterator;
		}

		GDINL ElementType& operator* () const
		{
			return *directIterator;
		}
		GDINL ElementType* operator-> () const
		{
			return directIterator.operator->();
		}
	};	// struct ReverseContainerIterator

	// ------------------------------------------------------------------------------------------
	//! Adapter that reverses direct Iterator functions of specified type.
	//! @tparam ContainerType Direct-treatable m_Container type. 
	template<typename TContainer>
	class ReverseContainerAdapter final
	{
	public:
		using ContainerType        = TContainer;
		using ElementType          = typename ContainerType::ElementType;
		using Iterator             = decltype(DeclValue<ContainerType>().ReverseBegin());
		using ReverseIterator      = decltype(DeclValue<ContainerType>().Begin());
		using ConstIterator        = Iterator;
		using ReverseConstIterator = ReverseIterator;

		GD_CONTAINER_DEFINE_ITERATION_SUPPORT(ReverseContainerAdapter);

	private:
		ContainerType* container;	

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructors.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Initializes an adapter that reverses direct Iterator functions of specified type.
		//! @param m_Container The m_Container that would be indirectly iterated.
		GDINL explicit ReverseContainerAdapter(ContainerType& container)
			: container(&container)
		{
		}

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Iteration API.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		GDINL Iterator Begin()
		{
			return container->ReverseBegin();
		}
		GDINL ConstIterator Begin() const
		{
			return container->ReverseBegin();
		}

		GDINL Iterator End()
		{
			return container->ReverseEnd();
		}
		GDINL ConstIterator End() const
		{
			return container->ReverseEnd();
		}

		GDINL ReverseIterator ReverseBegin()
		{
			return container->Begin();
		}
		GDINL ReverseConstIterator ReverseBegin() const
		{
			return container->Begin();
		}

		GDINL ReverseIterator ReverseEnd()
		{
			return container->End();
		}
		GDINL ReverseConstIterator ReverseEnd() const
		{
			return container->End();
		}
	};	// class ReverseContainerAdapter

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Containers-specific algorithms.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Returns Iterator that points to first m_Container element.
	//! @returns Iterator that points to first m_Container element.
	//! @{
	template<typename TContainer>
	GDINL static auto Begin(TContainer const& container) -> decltype(container.Begin())
	{
		return container.Begin();
	}
	template<typename TContainer>
	GDINL static auto Begin(TContainer& container) -> decltype(container.Begin())
	{
		return container.Begin();
	}
	template<typename TArrayElement, SizeTp ArrayLength>
	GDINL static TArrayElement* Begin(TArrayElement(&Array)[ArrayLength])
	{
		return &Array[0];
	}
	//! @}

	// ------------------------------------------------------------------------------------------
	//! Returns Iterator that points to past the end m_Container element.
	//! @returns Iterator that points to past the end m_Container element.
	//! @{
	template<typename TContainer>
	GDINL static auto End(TContainer const& container) -> decltype(container.End())
	{
		return container.End();
	}
	template<typename TContainer>
	GDINL static auto End(TContainer& container) -> decltype(container.End())
	{
		return container.End();
	}
	template<typename ArrayElementType, SizeTp ArrayLength>
	GDINL static ArrayElementType* End(ArrayElementType(&Array)[ArrayLength])
	{
		return &Array[ArrayLength];
	}
	//! @}

	// ------------------------------------------------------------------------------------------
	//! Returns a version of the specified m_Container with Begin and ReverseBegin, End and ReverseEnd functions reversed.
	//! @param m_Container Some m_Container to invert.
	//! @returns A version of the specified m_Container with Begin and ReverseBegin, End and ReverseEnd functions reversed.
	template<typename TContainer>
	GDINL ReverseContainerAdapter<TContainer> Reverse(TContainer& container)
	{
		return ReverseContainerAdapter<TContainer>(container);
	}

GD_NAMESPACE_END
