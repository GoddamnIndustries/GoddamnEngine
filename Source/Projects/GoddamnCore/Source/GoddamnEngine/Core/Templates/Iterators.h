// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Templates/Iterators.h
 * Common algorithms for container iterator interfaces.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/Utility.h>

/*!
 * Adds support of ranged-for iteration to the container. 
 * @param this_type Type of this container. Should contain const and mutable versions of 'Begin' and 'End' methods.
 */
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

	typedef Allocator DefaultContainerAllocator;
	template<typename>
	using GCContainerAllocator = void;

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                               Iterator traits.                                   ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	template<typename TIteratorType>
	struct IteratorTraits final
	{
		enum { IsForward = 1 };
		enum { IsBidirectional = 1 };
		enum { IsRandomAccess = 1 };
	};	// struct IteratorTraits

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                    IndexedContainerIterator<U, T> class.                         ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	/*!
	 * Iterator for containers that have constant-time (preferably) index access operators.
	 * @tparam TContainer Type of the container.
	 */
	template<typename TContainer>
	struct IndexedContainerIterator final
	{
	public:
		using ContainerType = TContainer;
		using ElementType   = Conditional<TypeTraits::IsConst<TContainer>::Value
			, typename TContainer::ConstElementType
			, typename TContainer::ElementType>;
		using ReferenceType = Conditional<TypeTraits::IsConst<TContainer>::Value
			, typename TContainer::ConstReferenceType
			, typename TContainer::ReferenceType>;

	private:
		TContainer&	m_Container;
		SizeTp		m_Index;

	public:

		/*!
		 * Initializes Iterator that points on the first element of the specified container.
		 * @param container The container that is going to be iterated.
		 */
		GDINL explicit IndexedContainerIterator(TContainer& container)
			: m_Container(container), m_Index(0)
		{
		}
			
	public:

		// iterator = iterator
		GDINL IndexedContainerIterator& operator= (IndexedContainerIterator const& other)
		{
			GD_VERIFY(&m_Container == &other.m_Container, "Iterators have different base containers.");
			m_Index = other.m_Index;
			return *this;
		}

		// iterator++
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

		// iterator--
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

		// iterator + ptrdiff_t
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

		// iterator - ptrdiff_t
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

		// iterator == iterator
		GDINL friend bool operator== (IndexedContainerIterator const& lhs, IndexedContainerIterator const& rhs)
		{
			return (&lhs.m_Container == &rhs.m_Container) && (lhs.m_Index == rhs.m_Index);
		}
		GDINL friend bool operator!= (IndexedContainerIterator const& lhs, IndexedContainerIterator const& rhs)
		{
			return (&lhs.m_Container != &rhs.m_Container) || (lhs.m_Index != rhs.m_Index);
		}

		// iterator->
		GDINL ReferenceType operator* () const
		{
			return m_Container[m_Index];
		}
		GDINL auto operator-> () const
		{
			return &m_Container[m_Index];
		}

	};	// struct IndexedContainerIterator

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                    ReverseContainerIterator<U, T> class.                         ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	/*!
	 * Adapter that inverses Iterator of a specified type.
	 * @tparam TDirectIterator Reversible bidirectional Iterator type.
	 */
	template<typename TDirectIterator>
	struct ReverseContainerIterator final
	{
	public:
		using DirectIteratorType = TDirectIterator;
		using ContainerType      = typename DirectIteratorType::ContainerType;
		using ElementType        = Conditional<TypeTraits::IsPointer<TDirectIterator>::Value, TypeTraits::RemovePointer<TDirectIterator>, typename DirectIteratorType::ElementType>;

	private:
		DirectIteratorType m_DirectIterator;

	public:

		/*!
		 * Initializes reverse Iterator this direct one.
		 * @param directIterator Iterator that advancing operation would be inverted.
		 */
		GDINL explicit ReverseContainerIterator(DirectIteratorType directIterator)
			: m_DirectIterator(directIterator)
		{
		}

	public:

		// reverse_iterator++
		GDINL ReverseContainerIterator& operator++ ()
		{
			--m_DirectIterator;
			return *this;
		}
		GDINL ReverseContainerIterator operator++ (int const unused)
		{
			GD_NOT_USED(unused);
			ReverseContainerIterator copy(*this);
			--m_DirectIterator;
			return copy;
		}

		// reverse_iterator--
		GDINL ReverseContainerIterator& operator-- ()
		{
			++m_DirectIterator;
			return *this;
		}
		GDINL ReverseContainerIterator operator-- (int const unused)
		{
			GD_NOT_USED(unused);
			ReverseContainerIterator copy(*this);
			++m_DirectIterator;
			return copy;
		}

		// reverse_iterator + ptrdiff_t
		GDINL ReverseContainerIterator& operator+= (PtrDiffTp const offset)
		{
			m_DirectIterator -= offset;
			return *this;
		}
		GDINL ReverseContainerIterator operator+ (PtrDiffTp const offset) const
		{
			ReverseContainerIterator copy(*this);
			return copy += offset;
		}

		// reverse_iterator - ptrdiff_t
		GDINL ReverseContainerIterator& operator-= (PtrDiffTp const offset)
		{
			m_DirectIterator += offset;
			return *this;
		}
		GDINL ReverseContainerIterator operator- (PtrDiffTp const offset) const
		{
			ReverseContainerIterator copy(*this);
			return copy -= offset;
		}

		// reverse_iterator - reverse_iterator
		GDINL PtrDiffTp operator- (ReverseContainerIterator const& other) const
		{
			return m_DirectIterator - other.m_DirectIterator;
		}

		// reverse_iterator == reverse_iterator
		GDINL friend bool operator== (ReverseContainerIterator const& lhs, ReverseContainerIterator const& rhs)
		{
			return lhs.m_DirectIterator == rhs.m_DirectIterator;
		}
		GDINL friend bool operator!= (ReverseContainerIterator const& lhs, ReverseContainerIterator const& rhs)
		{
			return lhs.m_DirectIterator != rhs.m_DirectIterator;
		}

		// reverse_iterator->
		GDINL auto& operator* () const
		{
			return *m_DirectIterator;
		}
		GDINL auto operator-> () const
		{
			return m_DirectIterator.operator->();
		}
	};	// struct ReverseContainerIterator

	/*!
	 * Adapter that reverses direct Iterator functions of specified type.
	 * @tparam TContainer Direct-iteratable container type. 
	 */
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

		GD_CONTAINER_DEFINE_ITERATION_SUPPORT(ReverseContainerAdapter)

	private:
		TContainer* m_Container;

	public:

		/*!
		 * Initializes an adapter that reverses direct Iterator functions of specified type.
		 * @param container The container that would be indirectly iterated.
		 */
		GDINL explicit ReverseContainerAdapter(TContainer& container)
			: m_Container(&container)
		{
		}

	public:
		GDINL Iterator Begin()
		{
			return m_Container->ReverseBegin();
		}
		GDINL ConstIterator Begin() const
		{
			return m_Container->ReverseBegin();
		}

		GDINL Iterator End()
		{
			return m_Container->ReverseEnd();
		}
		GDINL ConstIterator End() const
		{
			return m_Container->ReverseEnd();
		}

		GDINL ReverseIterator ReverseBegin()
		{
			return m_Container->Begin();
		}
		GDINL ReverseConstIterator ReverseBegin() const
		{
			return m_Container->Begin();
		}

		GDINL ReverseIterator ReverseEnd()
		{
			return m_Container->End();
		}
		GDINL ReverseConstIterator ReverseEnd() const
		{
			return m_Container->End();
		}

	};	// class ReverseContainerAdapter

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                       Containers-specific algorithms.                            ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	/*!
	 * Returns iterator that points to first container element.
	 */
	//! @{
	template<typename TContainer>
	GDINL static auto Begin(TContainer const& container) 
	{
		return container.Begin();
	}
	template<typename TContainer>
	GDINL static auto Begin(TContainer& container) 
	{
		return container.Begin();
	}
	template<typename TArrayElement, SizeTp TArrayLength>
	GDINL static TArrayElement* Begin(TArrayElement(&array)[TArrayLength])
	{
		return array;
	}
	//! @}

	/*!
	 * Returns Iterator that points to past the end container element.
	 */
	//! @{
	template<typename TContainer>
	GDINL static auto End(TContainer const& container) 
	{
		return container.End();
	}
	template<typename TContainer>
	GDINL static auto End(TContainer& container) 
	{
		return container.End();
	}
	template<typename TArrayElement, SizeTp TArrayLength>
	GDINL static TArrayElement* End(TArrayElement(&array)[TArrayLength])
	{
		return array + TArrayLength;
	}
	//! @}

	/*!
	 * Returns a version of the specified m_Container with Begin and ReverseBegin, End and ReverseEnd functions reversed.
	 * @param container Some container to invert.
	 */
	template<typename TContainer>
	GDINL static auto Reverse(TContainer& container)
	{
		return ReverseContainerAdapter<TContainer>(container);
	}

GD_NAMESPACE_END
