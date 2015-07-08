// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Templates/Algorithm.h
//! Template algorithms definitions. 
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Utility.h>
#include <GoddamnEngine/Core/Iterators.h>
#include <GoddamnEngine/Core/TypeTraits.h>

GD_NAMESPACE_BEGIN
	
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Some default predicates.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Default predicate for 'equal' operation.
	template<typename ValueType>
	struct EqualTp final
	{
		GDINL bool operator()(ValueType const& LHS, ValueType const& RHS) const { return LHS == RHS; }
	};	// struct EqualTp final

	// ------------------------------------------------------------------------------------------
	//! Default predicate for 'not equal' operation.
	template<typename ValueType>
	struct NotEqualTp final
	{
		GDINL bool operator()(ValueType const& LHS, ValueType const& RHS) const { return LHS != RHS; }
	};	// struct NotEqualTp final

	// ------------------------------------------------------------------------------------------
	//! Default predicate for 'greater' operation.
	template<typename ValueType>
	struct LessTp final
	{
		GDINL bool operator()(ValueType const& LHS, ValueType const& RHS) const { return LHS < RHS; }
	};	// struct LessTp final

	// ------------------------------------------------------------------------------------------
	//! Default predicate for 'less equal' operation.
	template<typename ValueType>
	struct LessEqualTp final
	{
		GDINL bool operator()(ValueType const& LHS, ValueType const& RHS) const { return LHS <= RHS; }
	};	// struct LessTp final

	// ------------------------------------------------------------------------------------------
	//! Default predicate for 'greater' operation.
	template<typename ValueType>
	struct GreaterTp final
	{
		GDINL bool operator()(ValueType const& LHS, ValueType const& RHS) const { return LHS > RHS; }
	};	// struct GreaterTp final

	// ------------------------------------------------------------------------------------------
	//! Default predicate for 'less equal' operation.
	template<typename ValueType>
	struct GreaterEqualTp final
	{
		GDINL bool operator()(ValueType const& LHS, ValueType const& RHS) const { return LHS >= RHS; }
	};	// struct GreaterEqualTp final

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Iterators and ranges initialization algorithms.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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
	template<typename ForwardIteratorType
#if !GD_DOCUMENTATION
		, typename = typename EnableIf<IteratorTraits<ForwardIteratorType>::IsForward>::Type
#endif	// if !GD_DOCUMENTATION
	> 
	GDINL static void InitializeRange(ForwardIteratorType const StartIterator, ForwardIteratorType const EndIterator)
	{
		typedef typename TypeTraits::RemoveReference<decltype(*StartIterator)>::Type ElementType;
		if (!TypeTraits::IsPOD<ElementType>::Value)
			for (ForwardIteratorType Iterator = StartIterator; Iterator != EndIterator; ++Iterator)
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
	template<typename ForwardIteratorType
#if !GD_DOCUMENTATION
		, typename = typename EnableIf<IteratorTraits<ForwardIteratorType>::IsForward>::Type
#endif	// if !GD_DOCUMENTATION
	>
	GDINL static void DeinitializeRange(ForwardIteratorType const StartIterator, ForwardIteratorType const EndIterator)
	{
		typedef typename TypeTraits::RemoveReference<decltype(*StartIterator)>::Type ElementType;
		if (!TypeTraits::IsPOD<ElementType>::Value)
			for (ForwardIteratorType Iterator = StartIterator; Iterator != EndIterator; ++Iterator)
				DeinitializeIterator(Iterator);
	}

	// ------------------------------------------------------------------------------------------
	//! Copies values of the iterators from source range to destination.
	//! @param StartSource Start of the source iterators range.
	//! @param EndSource End of the source iterators range.
	//! @param Destination First destination iterator.
	template<typename SourceForwardIteratorType, typename DestinationForwardIteratorType
#if !GD_DOCUMENTATION
		, typename = typename EnableIf<IteratorTraits<SourceForwardIteratorType>::IsForward>::Type
		, typename = typename EnableIf<IteratorTraits<DestinationForwardIteratorType>::IsForward>::Type
#endif	// if !GD_DOCUMENTATION
	>
	GDINL static DestinationForwardIteratorType CopyRange(SourceForwardIteratorType const& StartSource
		, SourceForwardIteratorType const& EndSource, DestinationForwardIteratorType Destination)
	{
		SourceForwardIteratorType Iterator = StartSource;
		for (; Iterator != EndSource; ++Iterator)
			InitializeIterator(Destination + (Iterator - StartSource), *Iterator);
		return Iterator;
	}

	// ------------------------------------------------------------------------------------------
	//! Moves values of the iterators from source range to destination.
	//! @param StartSource Start of the source iterators range.
	//! @param EndSource End of the source iterators range.
	//! @param Destination First destination iterator.
	template<typename SourceForwardIteratorType, typename DestinationForwardIteratorType
#if !GD_DOCUMENTATION
		, typename = typename EnableIf<IteratorTraits<SourceForwardIteratorType>::IsForward>::Type
		, typename = typename EnableIf<IteratorTraits<DestinationForwardIteratorType>::IsForward>::Type
#endif	// if !GD_DOCUMENTATION
	>
	GDINL static DestinationForwardIteratorType MoveRange(SourceForwardIteratorType const& StartSource
		, SourceForwardIteratorType const& EndSource, DestinationForwardIteratorType Destination)
	{
		typedef typename TypeTraits::RemoveReference<decltype(*Destination)>::Type ElementType;
		SourceForwardIteratorType Iterator = StartSource;
		for (; Iterator != EndSource; ++Iterator)
			InitializeIterator(Destination + (Iterator - StartSource), Forward<ElementType>(*Iterator));
		return Iterator;
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Searching algorithms.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Linearly searches through all iterators for one that matches the specified predicate.
	//! @param StartIterator Start of the iterators range.
	//! @param EndIterator End of the iterators range.
	//! @param Predicate The unary predicate instance.
	//! @returns First iterator found that matched specified predicate or the end iterator.
	//! @{
	template<typename ForwardIteratorType, typename PredicateType
#if !GD_DOCUMENTATION
		, typename = typename EnableIf<IteratorTraits<ForwardIteratorType>::IsForward>::Type
#endif	// if !GD_DOCUMENTATION
	>
	GDINL static ForwardIteratorType FindFirstIf(ForwardIteratorType const StartIterator, ForwardIteratorType const EndIterator, PredicateType const& Predicate)
	{
		for (ForwardIteratorType Iterator = StartIterator; Iterator != EndIterator; ++Iterator)
			if (Predicate(*Iterator))
				return Iterator;
		return EndIterator;
	}
	template<typename ForwardIteratorType, typename ElementType>
	GDINL static ForwardIteratorType FindFirst(ForwardIteratorType const StartIterator, ForwardIteratorType const EndIterator, ElementType const& Predicate)
	{
		return FindFirstIf(StartIterator, EndIterator
			, [&Predicate](ElementType const& Value) { return Value == Predicate; });
	}
	//! @}

	// ------------------------------------------------------------------------------------------
	//! Linearly searches through the container for the first element that matches the specified predicate.
	//! @param StartIterator Start of the iterators range.
	//! @param EndIterator End of the iterators range.
	//! @param Predicate The unary predicate instance.
	//! @returns First iterator found that matched specified predicate or the end iterator.
	//! @{
	template<typename ContainerType, typename PredicateType>
	GDINL static auto FindFirstIf(ContainerType const& Container, PredicateType const& Predicate) -> decltype(Begin(Container))
	{
		return FindFirstIf(Begin(Container), End(Container), Predicate);
	}
	template<typename ContainerType, typename PredicateType>
	GDINL static auto FindFirstIf(ContainerType& Container, PredicateType const& Predicate) -> decltype(Begin(Container))
	{
		return FindFirstIf(Begin(Container), End(Container), Predicate);
	}
	template<typename ContainerType, typename ElementType>
	GDINL static auto FindFirst(ContainerType const& Container, ElementType const& Predicate) -> decltype(Begin(Container))
	{
		return FindFirst(Begin(Container), End(Container), Predicate);
	}
	template<typename ContainerType, typename ElementType>
	GDINL static auto FindFirst(ContainerType& Container, ElementType const& Predicate) -> decltype(Begin(Container))
	{
		return FindFirst(Begin(Container), End(Container), Predicate);
	}
	//! @}

	// ------------------------------------------------------------------------------------------
	//! Linearly searches through all iterators (in reverse order) for one that matches the specified predicate.
	//! @param StartIterator Start of the iterators range.
	//! @param EndIterator End of the iterators range.
	//! @param Predicate The unary predicate instance.
	//! @returns Last iterator found that matched specified predicate or the end iterator.
	//! @{
	template<typename BidirectionalIteratorType, typename PredicateType
#if !GD_DOCUMENTATION
		, typename = typename EnableIf<IteratorTraits<BidirectionalIteratorType>::IsBidirectional>::Type
#endif	// if !GD_DOCUMENTATION
	>
	GDINL static BidirectionalIteratorType FindLastIf(BidirectionalIteratorType const StartIterator, BidirectionalIteratorType const EndIterator, PredicateType const& Predicate)
	{
		BidirectionalIteratorType const ReverseStartIterator = EndIterator - 1;
		BidirectionalIteratorType const ReverseEndIterator = StartIterator - 1;
		for (BidirectionalIteratorType Iterator = ReverseStartIterator; Iterator != ReverseEndIterator; --Iterator)
			if (Predicate(*Iterator))
				return Iterator;
		return EndIterator;
	}
	template<typename BidirectionalIteratorType, typename ElementType>
	GDINL static BidirectionalIteratorType FindLast(BidirectionalIteratorType const StartIterator, BidirectionalIteratorType const EndIterator
		, ElementType const& Predicate)
	{
		return FindLastIf(StartIterator, EndIterator
			, [&Predicate](ElementType const& Value) { return Value == Predicate; });
	}
	//! @}

	// ------------------------------------------------------------------------------------------
	//! Linearly searches through the container for the last element that matches the specified predicate.
	//! @param StartIterator Start of the iterators range.
	//! @param EndIterator End of the iterators range.
	//! @param Predicate The unary predicate instance.
	//! @returns First iterator found that matched specified predicate or the end iterator.
	//! @{
	template<typename ContainerType, typename PredicateType>
	GDINL static auto FindLastIf(ContainerType const& Container, PredicateType const& Predicate) -> decltype(Begin(Container))
	{
		return FindLastIf(Begin(Container), End(Container), Predicate);
	}
	template<typename ContainerType, typename PredicateType>
	GDINL static auto FindLastIf(ContainerType& Container, PredicateType const& Predicate) -> decltype(Begin(Container))
	{
		return FindLastIf(Begin(Container), End(Container), Predicate);
	}
	template<typename ContainerType, typename ElementType>
	GDINL static auto FindLast(ContainerType const& Container, ElementType const& Predicate) -> decltype(Begin(Container))
	{
		return FindLast(Begin(Container), End(Container), Predicate);
	}
	template<typename ContainerType, typename ElementType>
	GDINL static auto FindLast(ContainerType& Container, ElementType const& Predicate) -> decltype(Begin(Container))
	{
		return FindLast(Begin(Container), End(Container), Predicate);
	}
	//! @}

	// ------------------------------------------------------------------------------------------
	//! Searches through all iterators for one that matches the specified predicate via binary searching algorithm.
	//! The range must me sorted in ascending order. See @ref UnstableSort method.
	//! @param StartIterator Start of the iterators range.
	//! @param EndIterator End of the iterators range.
	//! @param Predicate Element we are searching for.
	//! @returns First iterator found that matched specified predicate or the end iterator.
	template<typename RandomAccessIteratorType, typename ElementType
#if !GD_DOCUMENTATION
		, typename = typename EnableIf<IteratorTraits<RandomAccessIteratorType>::IsRandomAccess>::Type
#endif	// if !GD_DOCUMENTATION
	>
	GDINL static RandomAccessIteratorType BinaryFind(RandomAccessIteratorType const StartIterator, RandomAccessIteratorType const EndIterator, ElementType const& Predicate)
	{
		for (RandomAccessIteratorType LeftIterator = StartIterator, RightIterator = (EndIterator - 1); LeftIterator != RightIterator; )
		{
			SizeTp const IteratorsDistance = (RightIterator - LeftIterator);
			RandomAccessIteratorType const PivotIterator = LeftIterator + IteratorsDistance / 2;
			ElementType const& PivotElement = *PivotIterator;
			if (PivotElement == Predicate)
				return PivotIterator;
			((PivotElement < Predicate) ? LeftIterator : RightIterator) = PivotIterator;
		}
		return EndIterator;
	}

	// ------------------------------------------------------------------------------------------
	//! Searches through the container for one that matches the specified predicate via binary searching algorithm.
	//! @param StartIterator Start of the iterators range.
	//! @param EndIterator End of the iterators range.
	//! @param Predicate Element we are searching for.
	//! @returns First iterator found that matched specified predicate or the end iterator.
	//! @{
	template<typename ContainerType, typename ElementType>
	GDINL static auto BinaryFind(ContainerType const& Container, ElementType const& Predicate) -> decltype(Begin(Container))
	{
		return BinaryFind(Begin(Container), End(Container), Predicate);
	}
	template<typename ContainerType, typename ElementType>
	GDINL static auto BinaryFind(ContainerType& Container, ElementType const& Predicate) -> decltype(Begin(Container))
	{
		return BinaryFind(Begin(Container), End(Container), Predicate);
	}
	//! @}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Sorting algorithms.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	// Sort two iterators by predicate.
	template<typename IteratorType, typename LessPredicateType>
	GDINL static void _PairSort(IteratorType const FirstIterator, IteratorType const SecondIterator, LessPredicateType const& LessPredicate)
	{
		if (LessPredicate(*SecondIterator, *FirstIterator))
			IteratorSwap(FirstIterator, SecondIterator);
	}

	// ------------------------------------------------------------------------------------------
	// https://en.wikipedia.org/wiki/Insertion_sort
	template<typename RandomAccessIteratorType, typename LessPredicateType
#if !GD_DOCUMENTATION
		, typename = typename EnableIf<IteratorTraits<RandomAccessIteratorType>::IsRandomAccess>::Type
#endif	// if !GD_DOCUMENTATION
	>
	GDINL static void _InsertionSort(RandomAccessIteratorType const StartIterator, RandomAccessIteratorType const EndIterator, LessPredicateType const& LessPredicate)
	{
		if (EndIterator != StartIterator)
		{
			RandomAccessIteratorType const ReverseEndIterator = StartIterator - 1;
			for (RandomAccessIteratorType Iterator = StartIterator + 1; Iterator != EndIterator; ++Iterator)
			{
				for (RandomAccessIteratorType SortedIterator = Iterator; (SortedIterator != ReverseEndIterator)
					&& LessPredicate(*SortedIterator, *(SortedIterator - 1)); --SortedIterator)
					IteratorSwap(SortedIterator, SortedIterator - 1);
			}
		}
	}

	// ------------------------------------------------------------------------------------------
	// https://en.wikipedia.org/wiki/Quicksort
	template<typename RandomAccessIteratorType, typename LessPredicateType
#if !GD_DOCUMENTATION
		, typename = typename EnableIf<IteratorTraits<RandomAccessIteratorType>::IsRandomAccess>::Type
#endif	// if !GD_DOCUMENTATION
	>
	GDINL static void _QuickSort(RandomAccessIteratorType const StartIterator, RandomAccessIteratorType const EndIterator, LessPredicateType const& LessPredicate)
	{
		if (StartIterator != EndIterator)
		{
			RandomAccessIteratorType LeftIterator = StartIterator;
			RandomAccessIteratorType RightIterator = EndIterator - 1;
			if (LeftIterator != RightIterator)
			{
				RandomAccessIteratorType const PivotIterator = LeftIterator++;
				while (LeftIterator != RightIterator)
				{
					if (LessPredicate(*LeftIterator, *PivotIterator))
					{
						++LeftIterator;
					}
					else
					{
						while ((LeftIterator != RightIterator) && LessPredicate(*PivotIterator, *RightIterator))
							--RightIterator;
						IteratorSwap(LeftIterator, RightIterator);
					}
				}
				IteratorSwap(PivotIterator, LeftIterator - 1);
				_QuickSort(StartIterator, LeftIterator, LessPredicate);
				_QuickSort(RightIterator, EndIterator, LessPredicate);
			}
		}
	}

	// ------------------------------------------------------------------------------------------
	// https://en.wikipedia.org/wiki/Smoothsort
	template<typename RandomAccessIteratorType, typename LessPredicateType
#if !GD_DOCUMENTATION
		, typename = typename EnableIf<IteratorTraits<RandomAccessIteratorType>::IsRandomAccess>::Type
#endif	// if !GD_DOCUMENTATION
	>
	GDINL static void _SmoothSort(RandomAccessIteratorType const StartIterator, RandomAccessIteratorType const EndIterator, LessPredicateType const& LessPredicate)
	{
		//! @todo Implement smooth sorting.
		_QuickSort(StartIterator, EndIterator, LessPredicate);
	}

	// ------------------------------------------------------------------------------------------
	// https://en.wikipedia.org/wiki/Merge_sort
	template<typename RandomAccessIteratorType, typename LessPredicateType
#if !GD_DOCUMENTATION
		, typename = typename EnableIf<IteratorTraits<RandomAccessIteratorType>::IsRandomAccess>::Type
#endif	// if !GD_DOCUMENTATION
	>
	GDINL static void _MergeSort(RandomAccessIteratorType const StartIterator, RandomAccessIteratorType const EndIterator, LessPredicateType const& LessPredicate)
	{
		//! @todo Implement merge sorting.
		_InsertionSort(StartIterator, EndIterator, LessPredicate);
	}

	// ------------------------------------------------------------------------------------------
	//! Performs fast unstable (in case of >= 2 elements) sorting of iterator range. 
	//! @param StartIterator Start of the iterators range.
	//! @param EndIterator End of the iterators range.
	//! @param LessPredicate Binary predicate that determines, whether left expression is less than right.
	//! @{
	template<typename RandomAccessIteratorType, typename LessPredicateType
#if !GD_DOCUMENTATION
		, typename = typename EnableIf<IteratorTraits<RandomAccessIteratorType>::IsRandomAccess>::Type
#endif	// if !GD_DOCUMENTATION
	>
	GDINL static void UnstableSort(RandomAccessIteratorType const StartIterator, RandomAccessIteratorType const EndIterator, LessPredicateType const& LessPredicate)
	{
		SizeTp const IteratorsDistance = EndIterator - StartIterator;
		if (IteratorsDistance > 1)
		{
			if (IteratorsDistance != 2)
			{
				SizeTp static const MaxInsertionSortDistance = 8;
				if (IteratorsDistance > MaxInsertionSortDistance)
				{
					SizeTp static const MaxQuickSortDistance = 150;
					if (IteratorsDistance <= MaxQuickSortDistance)
						 _QuickSort(StartIterator, EndIterator, LessPredicate);
					else _SmoothSort(StartIterator, EndIterator, LessPredicate);
				}
				else
					_InsertionSort(StartIterator, EndIterator, LessPredicate);
			}
			else
				_PairSort(StartIterator, EndIterator - 1, LessPredicate);
		}
	}
	template<typename RandomAccessIteratorType>
	GDINL static void UnstableSort(RandomAccessIteratorType const StartIterator, RandomAccessIteratorType const EndIterator)
	{
		typedef typename TypeTraits::RemoveReference<decltype(*StartIterator)>::Type ElementType;
		UnstableSort(StartIterator, EndIterator, LessTp<ElementType>());
	}
	//! @}

	// ------------------------------------------------------------------------------------------
	//! Performs fast unstable (in case of >= 2 elements) sorting of the container. 
	//! @param Container Container to be sorted.
	template<typename ContainerType>
	GDINL static void UnstableSort(ContainerType& Container)
	{
		UnstableSort(Begin(Container), End(Container));
	}

	// ------------------------------------------------------------------------------------------
	//! Performs fast stable sorting of iterator range. Stable sorting is slower than unstable one.
	//! @param StartIterator Start of the iterators range.
	//! @param EndIterator End of the iterators range.
	//! @param LessPredicate Binary predicate that determines, whether left expression is less than right.
	//! @{
	template<typename RandomAccessIteratorType, typename LessPredicateType
#if !GD_DOCUMENTATION
		, typename = typename EnableIf<IteratorTraits<RandomAccessIteratorType>::IsRandomAccess>::Type
#endif	// if !GD_DOCUMENTATION
	>
	GDINL static void StableSort(RandomAccessIteratorType const StartIterator, RandomAccessIteratorType const EndIterator
		, LessPredicateType const& LessPredicate)
	{
		SizeTp const IteratorsDistance = EndIterator - StartIterator;
		if (IteratorsDistance > 1)
		{
			if (IteratorsDistance != 2)
			{
				SizeTp static const MaxInsertionSortDistance = 5;
				if (MaxInsertionSortDistance >= IteratorsDistance)
					 _InsertionSort(StartIterator, EndIterator, LessPredicate);
				else _MergeSort(StartIterator, EndIterator, LessPredicate);
			}
			else
				_PairSort(StartIterator, EndIterator - 1, LessPredicate);
		}
	}
	template<typename RandomAccessIteratorType>
	GDINL static void StableSort(RandomAccessIteratorType const StartIterator, RandomAccessIteratorType const EndIterator)
	{
		typedef typename TypeTraits::RemoveReference<decltype(*StartIterator)>::Type ElementType;
		StableSort(StartIterator, EndIterator, LessTp<ElementType>());
	}
	//! @}

	// ------------------------------------------------------------------------------------------
	//! Performs fast stable sorting of the container. Stable sorting is slower than unstable one.
	//! @param Container Container to be sorted.
	template<typename ContainerType>
	GDINL static void StableSort(ContainerType& Container)
	{
		StableSort(Begin(Container), End(Container));
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Other algorithms.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Compares this container and some other by a predicate.
	//! @param OtherVector Other container against which we are comparing.
	//! @param Predicate Object with () operator overloaded that takes two elements and compares then somehow.
	template<typename ContainerType, typename BinaryPredicateType>
	GDINL bool LexicographicalCompare(ContainerType const& First, ContainerType const& Second, BinaryPredicateType const& Predicate)
	{
		SizeTp const MinLength = Min(First.GetLength(), Second.GetLength());
		for (SizeTp Index = 0; Index < MinLength; ++Index)
			if (!Predicate(*(First.Begin() + Index), *(Second.Begin() + Index)))
				return false;
		return true;
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Min & max algorithms.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Returns a minimal object of the two specified.
	//! @param First First comparand.
	//! @param Second Second comparand.
	//! @returns Minimal of two comparands.
	//! @{
	template<typename ComparandType>
	GDINL ComparandType const& Min(ComparandType const& First, ComparandType const& Second)
	{
		return (First < Second) ? First : Second;
	}
	template<typename ComparandType>
	GDINL ComparandType& Min(ComparandType& First, ComparandType& Second)
	{
		return (First < Second) ? First : Second;
	}
	//! @}

	// ------------------------------------------------------------------------------------------
	//! Returns a maximum object of the two specified.
	//! @param First First comparand.
	//! @param Second Second comparand.
	//! @returns Maximum of two comparands.
	//! @{
	template<typename ComparandType>
	GDINL ComparandType& Max(ComparandType& First, ComparandType& Second)
	{
		return (First > Second) ? First : Second;
	}
	template<typename ComparandType>
	GDINL ComparandType const& Max(ComparandType const& First, ComparandType const& Second)
	{
		return (First > Second) ? First : Second;
	}
	//! @}

	typedef Int32 HashValueType;

	//! Represents hash summ that can not be implcilty casted to integer type. 
	struct HashCode final
	{
	private:
		HashValueType Value = 0;

	public:
		//! Initializes hash summ with precomputed integer value.
		GDINL explicit HashCode(HashValueType const HashValue = 0) : Value(HashValue) {}

		//! Returns integer representation of this hash summ.
		GDINL HashValueType GetValue() const { return this->Value; }

		//! Compares to hash summs.
		GDINL bool operator== (HashCode const& HashCode) const { return (this->Value == HashCode.Value); }
		GDINL bool operator!= (HashCode const& HashCode) const { return (this->Value != HashCode.Value); }
		GDINL bool operator>  (HashCode const& HashCode) const { return (this->Value >  HashCode.Value); }
		GDINL bool operator>= (HashCode const& HashCode) const { return (this->Value >= HashCode.Value); }
		GDINL bool operator<  (HashCode const& HashCode) const { return (this->Value <  HashCode.Value); }
		GDINL bool operator<= (HashCode const& HashCode) const { return (this->Value <= HashCode.Value); }
	};	// struct HashCode

GD_NAMESPACE_END
