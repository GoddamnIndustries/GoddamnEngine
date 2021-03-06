// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Templates/Algorithm.h
//! Template algorithms definitions. 
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/Utility.h>
#include <GoddamnEngine/Core/Templates/Iterators.h>

GD_NAMESPACE_BEGIN
	
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Some default predicates.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	/*!
	 * Default predicate for 'equal' operation.
	 */
	template<typename TValue>
	struct TEqual final
	{
		GDINL bool operator()(TValue const& lhs, TValue const& rhs) const { return lhs == rhs; }
	};	// struct TEqual final

	/*!
	 * Default predicate for 'not equal' operation.
	 */
	template<typename TValue>
	struct TNotEqual final
	{
		GDINL bool operator()(TValue const& lhs, TValue const& rhs) const { return lhs != rhs; }
	};	// struct TNotEqual final

	/*!
	 * Default predicate for 'less' operation.
	 */
	template<typename TValue>
	struct TLess final
	{
		GDINL bool operator()(TValue const& lhs, TValue const& rhs) const { return lhs < rhs; }
	};	// struct LessTp final

	/*!
	 * Default predicate for 'less equal' operation.
	 */
	template<typename TValue>
	struct TLessEqual final
	{
		GDINL bool operator()(TValue const& lhs, TValue const& rhs) const { return lhs <= rhs; }
	};	// struct LessTp final

	/*!
	 * Default predicate for 'greater' operation.
	 */
	template<typename TValue>
	struct TGreater final
	{
		GDINL bool operator()(TValue const& lhs, TValue const& rhs) const { return lhs > rhs; }
	};	// struct GreaterTp final

	/*!
	 * Default predicate for 'greater equal' operation.
	 */
	template<typename TValue>
	struct TGreaterEqual final
	{
		GDINL bool operator()(TValue const& lhs, TValue const& rhs) const { return lhs >= rhs; }
	};	// struct TGreaterEqual final

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Min & max algorithms.
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    // ------------------------------------------------------------------------------------------
    //! Returns a minimal object of the two specified.
    //! @param lhs First comparand.
    //! @param lhs rhs comparand.
    //! @returns Minimal of two comparands.
    //! @{
    template<typename TComparand>
    GDINL TComparand const& Min(TComparand const& lhs, TComparand const& rhs)
    {
        return (lhs < rhs) ? lhs : rhs;
    }
    template<typename TComparand>
    GDINL TComparand& Min(TComparand& lhs, TComparand& rhs)
    {
        return (lhs < rhs) ? lhs : rhs;
    }
    //! @}

    // ------------------------------------------------------------------------------------------
    //! Returns a maximum object of the two specified.
    //! @param lhs First comparand.
    //! @param rhs Second comparand.
    //! @returns Maximum of two comparands.
    //! @{
    template<typename TComparand>
    GDINL TComparand& Max(TComparand& lhs, TComparand& rhs)
    {
        return (lhs > rhs) ? lhs : rhs;
    }
    template<typename TComparand>
    GDINL TComparand const& Max(TComparand const& lhs, TComparand const& rhs)
    {
        return (lhs > rhs) ? lhs : rhs;
    }
    //! @}

	namespace Algo
	{

		// ------------------------------------------------------------------------------------------
		// Iterators and ranges initialization algorithms.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Initializes value of the specified Iterator.
		 * @param iterator Value of this Iterator would be initialized.
		 */
		template<typename TIterator>
		GDINL static void InitializeIterator(TIterator const iterator)
		{
			using TIterElement = TypeTraits::RemoveReference<decltype(*iterator)>;
			new (&*iterator) TIterElement();
		}

		/*!
		 * Initializes value of the specified Iterator.
		 *
		 * @param iterator Value of this Iterator would be initialized.
		 * @param element Initial value of the Iterator.
		 */
		//! @{
		template<typename TIterator, typename TElement>
		GDINL static void InitializeIterator(TIterator const iterator, TElement&& element)
		{
			using TIterElement = TypeTraits::RemoveReference<decltype(*iterator)>;
			new (&*iterator) TIterElement(Utils::Forward<TElement>(element));
		}
		template<typename TIterator, typename TElement>
		GDINL static void InitializeIterator(TIterator const iterator, TElement const& element)
		{
			using TIterElement = TypeTraits::RemoveReference<decltype(*iterator)>;
			new (&*iterator) TIterElement(element);
		}
		//! @}

		/*!
		 * Initializes all values of the iterators in specified range.
		 *
		 * @param startIterator Start of the iterators range.
		 * @param endIterator End of the iterators range.
		 */
		template<typename TForwardIterator
			, typename = typename EnableIf<IteratorTraits<TForwardIterator>::IsForward>::Type
			> 
		GDINL static void InitializeRange(TForwardIterator const startIterator, TForwardIterator const endIterator)
		{
			using TIterElement = TypeTraits::RemoveReference<decltype(*startIterator)>;
			for (auto iterator = startIterator; iterator != endIterator; ++iterator)
			{
				Algo::InitializeIterator(iterator);
			}
		}

		/*!
		 * Deinitializes value of the specified Iterator.
		 * @param iterator Value of this Iterator would be deinitialized.
		 */
		template<typename TIterator>
		GDINL static void DeinitializeIterator(TIterator const iterator)
		{
			using TIterElement = TypeTraits::RemoveReference<decltype(*iterator)>;
			(*iterator).~TIterElement();
		}

		/*!
		 * Deinitializes all values of the iterators in specified range.
		 *
		 * @param startIterator Start of the iterators range.
		 * @param endIterator End of the iterators range.
		 */
		template<typename TForwardIterator
			, typename = typename EnableIf<IteratorTraits<TForwardIterator>::IsForward>::Type
			>
		GDINL static void DeinitializeRange(TForwardIterator const startIterator, TForwardIterator const endIterator)
		{
			using TIterElement = TypeTraits::RemoveReference<decltype(*startIterator)>;
			for (auto iterator = startIterator; iterator != endIterator; ++iterator)
			{
				Algo::DeinitializeIterator(iterator);
			}
		}

		/*!
		 * Copies values of the iterators from source range to destination.
		 *
		 * @param startSource Start of the source iterators range.
		 * @param endSource End of the source iterators range.
		 * @param destination lhs destination Iterator.
		 */
		template<typename TSourceForwardIterator, typename TDestinationForwardIterator
			, typename = typename EnableIf<IteratorTraits<TSourceForwardIterator>::IsForward>::Type
			, typename = typename EnableIf<IteratorTraits<TDestinationForwardIterator>::IsForward>::Type
			>
		GDINL static void CopyRange(TSourceForwardIterator const& startSource
			, TSourceForwardIterator const& endSource, TDestinationForwardIterator destination)
		{
			for (auto iterator = startSource; iterator != endSource; ++iterator)
			{
				Algo::InitializeIterator(destination + (iterator - startSource), *iterator);
			}
		}

		/*!
		 * Moves values of the iterators from source range to destination.
		 *
		 * @param startSource Start of the source iterators range.
		 * @param endSource End of the source iterators range.
		 * @param destination lhs destination Iterator.
		 */
		template<typename TSourceForwardIterator, typename TDestinationForwardIterator
			, typename = typename EnableIf<IteratorTraits<TSourceForwardIterator>::IsForward>::Type
			, typename = typename EnableIf<IteratorTraits<TDestinationForwardIterator>::IsForward>::Type
			>
		GDINL static void MoveRange(TSourceForwardIterator const& startSource
			, TSourceForwardIterator const& endSource, TDestinationForwardIterator destination)
		{
			using TIterElement = TypeTraits::RemoveReference<decltype(*destination)>;
			for (auto iterator = startSource; iterator != endSource; ++iterator)
			{
				Algo::InitializeIterator(destination + (iterator - startSource), Utils::Move(*iterator));
			}
		}

		// ------------------------------------------------------------------------------------------
		// Searching algorithms.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Linearly searches through all iterators for one that matches the specified predicate.
		 *
		 * @param startSource Start of the iterators range.
		 * @param endSource End of the iterators range.
		 * @param predicate The unary predicate instance.
		 *
		 * @returns Iterator found that matched specified predicate or the end Iterator.
		 */
		//! @{
		template<typename TForwardIterator, typename TPredicate
			, typename = typename EnableIf<IteratorTraits<TForwardIterator>::IsForward>::Type
			>
		GDINL static TForwardIterator FindFirstIf(TForwardIterator const startIterator, TForwardIterator const endIterator, TPredicate const& predicate)
		{
			for (auto iterator = startIterator; iterator != endIterator; ++iterator)
			{
				if (predicate(*iterator))
				{
					return iterator;
				}
			}
			return endIterator;
		}
		template<typename TForwardIterator, typename TElement>
		GDINL static TForwardIterator FindFirst(TForwardIterator const startIterator, TForwardIterator const endIterator, TElement const& predicate)
		{
			return Algo::FindFirstIf(startIterator, endIterator, [&predicate](TElement const& Value) { return Value == predicate; });
		}
		//! @}

		/*!
		 * Linearly searches through the container for the first element that matches the specified predicate.
		 *
		 * @param container The container we are looking in.
		 * @param predicate The unary predicate instance.
		 *
		 * @returns Iterator found that matched specified predicate or the end Iterator.
		 */
		//! @{
		template<typename TContainer, typename TPredicate>
		GDINL static auto FindFirstIf(TContainer const& container, TPredicate const& predicate) 
		{
			return Algo::FindFirstIf(Begin(container), End(container), predicate);
		}
		template<typename TContainer, typename TPredicate>
		GDINL static auto FindFirstIf(TContainer& container, TPredicate const& predicate) 
		{
			return Algo::FindFirstIf(Begin(container), End(container), predicate);
		}
		template<typename TContainer, typename TElement>
		GDINL static auto FindFirst(TContainer const& container, TElement const& predicate)
		{
			return Algo::FindFirst(Begin(container), End(container), predicate);
		}
		template<typename TContainer, typename TElement>
		GDINL static auto FindFirst(TContainer& container, TElement const& predicate)
		{
			return Algo::FindFirst(Begin(container), End(container), predicate);
		}
		//! @}

		/*!
		 * Linearly searches through all iterators (in reverse order) for one that matches the specified predicate.
		 *
		 * @param startSource Start of the iterators range.
		 * @param endSource End of the iterators range.
		 * @param predicate The unary predicate instance.
		 *
		 * @returns Last iterator found that matched specified predicate or the end Iterator.
		 */
		//! @{
		template<typename TBidirectionalIterator, typename TPredicate
			, typename = typename EnableIf<IteratorTraits<TBidirectionalIterator>::IsBidirectional>::Type
			>
		GDINL static TBidirectionalIterator FindLastIf(TBidirectionalIterator const startIterator, TBidirectionalIterator const endIterator, TPredicate const& predicate)
		{
			auto const reverseStartIterator = endIterator - 1;
			auto const reverseEndIterator = startIterator - 1;
			for (auto iterator = reverseStartIterator; iterator != reverseEndIterator; --iterator)
			{
				if (predicate(*iterator))
				{
					return iterator;
				}
			}
			return endIterator;
		}
		template<typename TBidirectionalIterator, typename TElement>
		GDINL static TBidirectionalIterator FindLast(TBidirectionalIterator const startIterator, TBidirectionalIterator const endIterator, TElement const& predicate)
		{
			return FindLastIf(startIterator, endIterator, [&predicate](TElement const& Value) { return Value == predicate; });
		}
		//! @}

		/*!
		 * Linearly searches through all iterators (in reverse order) for one that matches the specified predicate.
		 *
		 * @param container The container we are looking in.
		 * @param predicate The unary predicate instance.
		 *
		 * @returns Last iterator found that matched specified predicate or the end Iterator.
		 */
		//! @{
		template<typename TContainer, typename TPredicate>
		GDINL static auto FindLastIf(TContainer const& container, TPredicate const& predicate) -> decltype(Begin(container))
		{
			return FindLastIf(Begin(container), End(container), predicate);
		}
		template<typename TContainer, typename TPredicate>
		GDINL static auto FindLastIf(TContainer& container, TPredicate const& predicate) -> decltype(Begin(container))
		{
			return FindLastIf(Begin(container), End(container), predicate);
		}
		template<typename TContainer, typename TElement>
		GDINL static auto FindLast(TContainer const& container, TElement const& predicate) -> decltype(Begin(container))
		{
			return FindLast(Begin(container), End(container), predicate);
		}
		template<typename TContainer, typename TElement>
		GDINL static auto FindLast(TContainer& container, TElement const& predicate) -> decltype(Begin(container))
		{
			return FindLast(Begin(container), End(container), predicate);
		}
		//! @}

		/*!
		 * Searches through all iterators for one that matches the specified predicate via binary searching algorithm.
		 * The range must me sorted in ascending order. See @ref UnstableSort method.
		 *
		 * @param startSource Start of the iterators range.
		 * @param endSource End of the iterators range.
		 * @param predicate Element we are searching for.
		 *
		 * @returns Iterator found that matched specified predicate or the end Iterator.
		 */
		template<typename TRandomAccessIterator, typename TElement
			, typename = typename EnableIf<IteratorTraits<TRandomAccessIterator>::IsRandomAccess>::Type
			>
		GDINL static TRandomAccessIterator BinaryFind(TRandomAccessIterator const startIterator, TRandomAccessIterator const endIterator, TElement const& predicate)
		{
			for (auto leftIterator = startIterator, rightIterator = (endIterator - 1); leftIterator != rightIterator; )
			{
				auto const iteratorsDistance = (rightIterator - leftIterator);
				auto const pivotIterator = leftIterator + iteratorsDistance / 2;
				auto const& pivotElement = *pivotIterator;
				if (pivotElement == predicate)
				{
					return pivotIterator;
				}

				((pivotElement < predicate) ? leftIterator : rightIterator) = pivotIterator;
			}
			return endIterator;
		}

		// ------------------------------------------------------------------------------------------
		//! Searches through the m_Container for one that matches the specified predicate via binary searching algorithm.
		//! @param startIterator Start of the iterators range.
		//! @param endIterator End of the iterators range.
		//! @param predicate element we are searching for.
		//! @returns lhs Iterator found that matched specified predicate or the end Iterator.
		//! @{
		template<typename TContainer, typename TElement>
		GDINL static auto BinaryFind(TContainer const& container, TElement const& predicate) -> decltype(Begin(container))
		{
			return BinaryFind(Begin(container), End(container), predicate);
		}
		template<typename TContainer, typename TElement>
		GDINL static auto BinaryFind(TContainer& container, TElement const& predicate) -> decltype(Begin(container))
		{
			return BinaryFind(Begin(container), End(container), predicate);
		}
		//! @}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Sorting algorithms.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		// Sort two iterators by predicate.
		template<typename TIterator, typename TLessPredicate>
		GDINL static void _PairSort(TIterator const FirstIterator, TIterator const SecondIterator, TLessPredicate const& lessPredicate)
		{
			if (lessPredicate(*SecondIterator, *FirstIterator))
				IteratorSwap(FirstIterator, SecondIterator);
		}

		// ------------------------------------------------------------------------------------------
		// https://en.wikipedia.org/wiki/Insertion_sort
		template<typename TRandomAccessIterator, typename TLessPredicate
	#if !GD_DOCUMENTATION
			, typename = typename EnableIf<IteratorTraits<TRandomAccessIterator>::IsRandomAccess>::Type
	#endif	// if !GD_DOCUMENTATION
		>
		GDINL static void _InsertionSort(TRandomAccessIterator const startIterator, TRandomAccessIterator const endIterator, TLessPredicate const& lessPredicate)
		{
			if (endIterator != startIterator)
			{
				TRandomAccessIterator const reverseEndIterator = startIterator - 1;
				for (TRandomAccessIterator iterator = startIterator + 1; iterator != endIterator; ++iterator)
				{
					for (TRandomAccessIterator sortedIterator = iterator; (sortedIterator != reverseEndIterator)
						&& lessPredicate(*sortedIterator, *(sortedIterator - 1)); --sortedIterator)
						IteratorSwap(sortedIterator, sortedIterator - 1);
				}
			}
		}

		// ------------------------------------------------------------------------------------------
		// https://en.wikipedia.org/wiki/Quicksort
		template<typename TRandomAccessIterator, typename TLessPredicate
	#if !GD_DOCUMENTATION
			, typename = typename EnableIf<IteratorTraits<TRandomAccessIterator>::IsRandomAccess>::Type
	#endif	// if !GD_DOCUMENTATION
		>
		GDINL static void _QuickSort(TRandomAccessIterator const startIterator, TRandomAccessIterator const endIterator, TLessPredicate const& lessPredicate)
		{
			if (startIterator != endIterator)
			{
				TRandomAccessIterator leftIterator = startIterator;
				TRandomAccessIterator rightIterator = endIterator - 1;
				if (leftIterator != rightIterator)
				{
					TRandomAccessIterator const pivotIterator = leftIterator++;
					while (leftIterator != rightIterator)
					{
						if (lessPredicate(*leftIterator, *pivotIterator))
						{
							++leftIterator;
						}
						else
						{
							while ((leftIterator != rightIterator) && lessPredicate(*pivotIterator, *rightIterator))
								--rightIterator;
							IteratorSwap(leftIterator, rightIterator);
						}
					}
					IteratorSwap(pivotIterator, leftIterator - 1);
					_QuickSort(startIterator, leftIterator, lessPredicate);
					_QuickSort(rightIterator, endIterator, lessPredicate);
				}
			}
		}

		// ------------------------------------------------------------------------------------------
		// https://en.wikipedia.org/wiki/Smoothsort
		template<typename TRandomAccessIterator, typename TLessPredicate
	#if !GD_DOCUMENTATION
			, typename = typename EnableIf<IteratorTraits<TRandomAccessIterator>::IsRandomAccess>::Type
	#endif	// if !GD_DOCUMENTATION
		>
		GDINL static void _SmoothSort(TRandomAccessIterator const startIterator, TRandomAccessIterator const endIterator, TLessPredicate const& lessPredicate)
		{
			//! @todo Implement smooth sorting.
			_QuickSort(startIterator, endIterator, lessPredicate);
		}

		// ------------------------------------------------------------------------------------------
		// https://en.wikipedia.org/wiki/Merge_sort
		template<typename TRandomAccessIterator, typename TLessPredicate
	#if !GD_DOCUMENTATION
			, typename = typename EnableIf<IteratorTraits<TRandomAccessIterator>::IsRandomAccess>::Type
	#endif	// if !GD_DOCUMENTATION
		>
		GDINL static void _MergeSort(TRandomAccessIterator const startIterator, TRandomAccessIterator const endIterator, TLessPredicate const& lessPredicate)
		{
			//! @todo Implement merge sorting.
			_InsertionSort(startIterator, endIterator, lessPredicate);
		}

		// ------------------------------------------------------------------------------------------
		//! Performs fast unstable (in case of >= 2 elements) sorting of Iterator range. 
		//! @param startIterator Start of the iterators range.
		//! @param endIterator End of the iterators range.
		//! @param lessPredicate Binary predicate that determines, whether m_Left expression is less than m_Right.
		//! @{
		template<typename TRandomAccessIterator, typename TLessPredicate
	#if !GD_DOCUMENTATION
			, typename = typename EnableIf<IteratorTraits<TRandomAccessIterator>::IsRandomAccess>::Type
	#endif	// if !GD_DOCUMENTATION
		>
		GDINL static void UnstableSort(TRandomAccessIterator const startIterator, TRandomAccessIterator const endIterator, TLessPredicate const& lessPredicate)
		{
			SizeTp const iteratorsDistance = endIterator - startIterator;
			if (iteratorsDistance > 1)
			{
				if (iteratorsDistance != 2)
				{
					SizeTp static const maxInsertionSortDistance = 8;
					if (iteratorsDistance > maxInsertionSortDistance)
					{
						SizeTp static const maxQuickSortDistance = 150;
						iteratorsDistance <= maxQuickSortDistance ? _QuickSort(startIterator, endIterator, lessPredicate) : _SmoothSort(startIterator, endIterator, lessPredicate);
					}
					else
					{
						_InsertionSort(startIterator, endIterator, lessPredicate);
					}
				}
				else
				{
					_PairSort(startIterator, endIterator - 1, lessPredicate);
				}
			}
		}
		template<typename TRandomAccessIterator>
		GDINL static void UnstableSort(TRandomAccessIterator const startIterator, TRandomAccessIterator const endIterator)
		{
			typedef typename TypeTraits::RemoveReference<decltype(*startIterator)>::Type ElementType;
			UnstableSort(startIterator, endIterator, TLess<ElementType>());
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Performs fast unstable (in case of >= 2 elements) sorting of the m_Container. 
		//! @param m_Container The m_Container to be sorted.
		template<typename TContainer>
		GDINL static void UnstableSort(TContainer& container)
		{
			UnstableSort(Begin(container), End(container));
		}

		// ------------------------------------------------------------------------------------------
		//! Performs fast stable sorting of Iterator range. Stable sorting is slower than unstable one.
		//! @param startIterator Start of the iterators range.
		//! @param endIterator End of the iterators range.
		//! @param lessPredicate Binary predicate that determines, whether m_Left expression is less than m_Right.
		//! @{
		template<typename TRandomAccessIterator, typename TLessPredicate
	#if !GD_DOCUMENTATION
			, typename = typename EnableIf<IteratorTraits<TRandomAccessIterator>::IsRandomAccess>::Type
	#endif	// if !GD_DOCUMENTATION
		>
		GDINL static void StableSort(TRandomAccessIterator const startIterator, TRandomAccessIterator const endIterator
			, TLessPredicate const& lessPredicate)
		{
			SizeTp const iteratorsDistance = endIterator - startIterator;
			if (iteratorsDistance > 1)
			{
				if (iteratorsDistance != 2)
				{
					SizeTp static const maxInsertionSortDistance = 5;
					maxInsertionSortDistance >= iteratorsDistance ? _InsertionSort(startIterator, endIterator, lessPredicate) : _MergeSort(startIterator, endIterator, lessPredicate);
				}
				else
				{
					_PairSort(startIterator, endIterator - 1, lessPredicate);
				}
			}
		}
		template<typename TRandomAccessIterator>
		GDINL static void StableSort(TRandomAccessIterator const startIterator, TRandomAccessIterator const endIterator)
		{
			typedef typename TypeTraits::RemoveReference<decltype(*startIterator)>::Type ElementType;
			StableSort(startIterator, endIterator, TLess<ElementType>());
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Performs fast stable sorting of the m_Container. Stable sorting is slower than unstable one.
		//! @param container The m_Container to be sorted.
		template<typename TContainer>
		GDINL static void StableSort(TContainer& container)
		{
			StableSort(Begin(container), End(container));
		}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// other algorithms.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Compares this container and some other by a predicate.
		//! @param otherVector Other container against which we are comparing.
		//! @param predicate Object with () operator overloaded that takes two elements and compares then somehow.
		template<typename TContainerLHS, typename TContainerRHS, typename TBinaryPredicate>
		GDINL bool LexicographicalCompare(TContainerLHS const& lhs, TContainerRHS const& rhs, TBinaryPredicate const& predicate)
		{
			SizeTp const minLength = Min(lhs.GetLength(), rhs.GetLength());
			for (SizeTp cnt = 0; cnt < minLength; ++cnt)
			{
				if (!predicate(*(lhs.Begin() + cnt), *(rhs.Begin() + cnt)))
				{
					return false;
				}
			}
			return true;
		}

		template<class InputIt1, class InputIt2>
		bool LexicographicalLess(InputIt1 first1, InputIt1 last1,
			InputIt2 first2, InputIt2 last2)
		{
			for (; (first1 != last1) && (first2 != last2); ++first1, (void)++first2) {
				if (*first1 < *first2) return true;
				if (*first2 < *first1) return false;
			}
			return (first1 == last1) && (first2 != last2);
		}
		template<typename TContainerLHS, typename TContainerRHS>
		GDINL bool LexicographicalLess(TContainerLHS const& lhs, TContainerRHS const& rhs)
		{
			return LexicographicalLess(lhs.Begin(), lhs.End(), rhs.Begin(), rhs.End());
		}

	}	// namespace Algo

	typedef Int32 HashValue;

	//! Represents hash code that can not be IMPLcilty casted to integer type. 
	struct HashCode final
	{
	private:
		HashValue Value = 0;

	public:

		GDINL HashCode() {}

		//! Initializes hash code with precomputed integer value.
		GDINL explicit HashCode(HashValue const HashValue) : Value(HashValue) {}

		//! Returns integer representation of this hash code.
		GDINL HashValue GetValue() const { return Value; }

		//! Compares to hash code.
		GDINL bool operator== (HashCode const& HashCode) const { return (Value == HashCode.Value); }
		GDINL bool operator!= (HashCode const& HashCode) const { return (Value != HashCode.Value); }
		GDINL bool operator>  (HashCode const& HashCode) const { return (Value >  HashCode.Value); }
		GDINL bool operator>= (HashCode const& HashCode) const { return (Value >= HashCode.Value); }
		GDINL bool operator<  (HashCode const& HashCode) const { return (Value <  HashCode.Value); }
		GDINL bool operator<= (HashCode const& HashCode) const { return (Value <= HashCode.Value); }
	};	// struct HashCode

GD_NAMESPACE_END
