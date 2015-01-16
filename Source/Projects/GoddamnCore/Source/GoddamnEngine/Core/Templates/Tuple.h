/// ==========================================================================================
/// Tuple.h - template tuple class.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_TEMPLATES_TUPLE
#define GD_CORE_TEMPLATES_TUPLE

#include <GoddamnEngine/Include.h>

GD_NAMESPACE_BEGIN

	template<size_t const TupleIndex, typename CurrentTupleType, typename... RestTupleTypes>
	struct TupleElementHelper;

	template<size_t const TupleIndex, typename CurrentTupleType, typename... RestTupleTypes>
	struct TupleElementHelper<TupleIndex, CurrentTupleType, RestTupleTypes...> : public TupleElementHelper<TupleIndex - 1, RestTupleTypes...>
	{
		static_assert((sizeof...(RestTupleTypes) + 1) >= TupleIndex, "Specified index is out of range.");
	};	// struct TupleElementHelper<TupleIndex, CurrentTupleType, RestTupleTypes>

	template<typename CurrentTupleType, typename... RestTupleTypes>
	struct TupleElementHelper<0, CurrentTupleType, RestTupleTypes...>
	{
		typedef CurrentTupleType Type;
	};	// struct TupleElementHelper<TupleIndex, CurrentTupleType, RestTupleTypes>
	
	/// @brief Objects that can retrieve a type with a specified index from the tuple.
	/// @tparam TupleIndex Index of the desired element.
	/// @tparam TupleTypes Types stored in a tuple.
	template<size_t const TupleIndex, typename... TupleTypes>
	struct TupleElement : public TupleElementHelper<TupleIndex, TupleTypes...>
	{
	};	// struct TupleElement

GD_NAMESPACE_END

#endif
