// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Containers/Array.h
 * Statically sized array class.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/Algorithm.h>
#include <GoddamnEngine/Core/Templates/Iterators.h>
#include <GoddamnEngine/Core/Containers/InitializerList.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                                Array<T, S> class.                                ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	// **------------------------------------------------------------------------------------------**
	//! Fixed-size array implementation.
	//! @tparam TElement Vector element type.
	//! @tparam TLength Fixed array size.
	// **------------------------------------------------------------------------------------------**
	template<typename TElement, SizeTp TLength>
	class Array
	{
	public:
		SizeTp static const Length = TLength;
		using ElementType          = TElement;
		using ConstElementType     = TElement const;
		using ReferenceType        = TElement&;
		using ConstReferenceType   = TElement const&;
		using Iterator             = IndexedContainerIterator<Array>;
		using ConstIterator        = IndexedContainerIterator<Array const>;
		using ReverseIterator      = ReverseContainerIterator<Iterator>;
		using ReverseConstIterator = ReverseContainerIterator<ConstIterator>;

		GD_CONTAINER_DEFINE_ITERATION_SUPPORT(Array)

	private:
		TElement m_Data[TLength];

	public:

		// ------------------------------------------------------------------------------------------
		// Constructors and destructor.
		// ------------------------------------------------------------------------------------------

	public:

		// ------------------------------------------------------------------------------------------
		// Iteration API.
		// ------------------------------------------------------------------------------------------

		GDINL Iterator Begin()
		{
			return Iterator(*this);
		}
		GDINL ConstIterator Begin() const
		{
			return ConstIterator(*this);
		}

		GDINL Iterator End()
		{
			return Begin() + Length;
		}
		GDINL ConstIterator End() const
		{
			return Begin() + Length;
		}

		GDINL ReverseIterator ReverseBegin()
		{
			return ReverseIterator(End() - 1);
		}
		GDINL ReverseConstIterator ReverseBegin() const
		{
			return ReverseConstIterator(End() - 1);
		}

		GDINL ReverseIterator ReverseEnd()
		{
			return ReverseIterator(Begin() - 1);
		}
		GDINL ReverseConstIterator ReverseEnd() const
		{
			return ReverseConstIterator(Begin() - 1);
		}

	public:

		// ------------------------------------------------------------------------------------------
		// Fixed size management.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Returns pointer to the data stored in vector.
		 */
		//! @{
		GDINL TElement const* GetData() const
		{
			return m_Data;
		}
		GDINL TElement* GetData()
		{
			return m_Data;
		}
		//! @}

		/*!
		 * Returns number of elements that exist in vector.
		 */
		GDINL static constexpr SizeTp GetLength()
		{
			return Length;
		}

	};	// class Array

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                                Array<T, 0> class.                                ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	// **------------------------------------------------------------------------------------------**
	//! Zero-size array implementation.
	//! @tparam TElement Vector element type.
	// **------------------------------------------------------------------------------------------**
	template<typename TElement>
	class Array<TElement, 0>
	{
	public:
		SizeTp static const Length = 0;
		using ElementType          = TElement;
		using ConstElementType     = TElement const;
		using ReferenceType        = TElement&;
		using ConstReferenceType   = TElement const&;
		using Iterator             = IndexedContainerIterator<Array>;
		using ConstIterator        = IndexedContainerIterator<Array const>;
		using ReverseIterator      = ReverseContainerIterator<Iterator>;
		using ReverseConstIterator = ReverseContainerIterator<ConstIterator>;

		GD_CONTAINER_DEFINE_ITERATION_SUPPORT(Array)

	public:

		// ------------------------------------------------------------------------------------------
		// Iteration API.
		// ------------------------------------------------------------------------------------------

		GDINL Iterator Begin()
		{
			return Iterator(*this);
		}
		GDINL ConstIterator Begin() const
		{
			return ConstIterator(*this);
		}

		GDINL Iterator End()
		{
			return Begin() + Length;
		}
		GDINL ConstIterator End() const
		{
			return Begin() + Length;
		}

		GDINL ReverseIterator ReverseBegin()
		{
			return ReverseIterator(End() - 1);
		}
		GDINL ReverseConstIterator ReverseBegin() const
		{
			return ReverseConstIterator(End() - 1);
		}

		GDINL ReverseIterator ReverseEnd()
		{
			return ReverseIterator(Begin() - 1);
		}
		GDINL ReverseConstIterator ReverseEnd() const
		{
			return ReverseConstIterator(Begin() - 1);
		}

	public:

		// ------------------------------------------------------------------------------------------
		// Fixed size management.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Returns pointer to the data stored in vector.
		 */
		//! @{
		GDINL TElement const* GetData() const
		{
			GD_DEBUG_VERIFY_FALSE("Zero-sized Array data access.");
		}
		GDINL TElement* GetData()
		{
			GD_DEBUG_VERIFY_FALSE("Zero-sized Array data access.");
		}
		//! @}

		/*!
		 * Returns number of elements that exist in vector.
		 */
		GDINL static constexpr SizeTp GetLength()
		{
			return Length;
		}

	};	// class Array

GD_NAMESPACE_END
