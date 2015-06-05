// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Templates/Tuple.h
//! Template tuple class.

#pragma once

#include <GoddamnEngine/Include.h>

GD_NAMESPACE_BEGIN

	template<SizeTp const TupleIndex, typename CurrentTupleType, typename... RestTupleTypes>
	struct TupleElementHelper;

	template<SizeTp const TupleIndex, typename CurrentTupleType, typename... RestTupleTypes>
	struct TupleElementHelper<TupleIndex, CurrentTupleType, RestTupleTypes...> : public TupleElementHelper<TupleIndex - 1, RestTupleTypes...>
	{
		static_assert((sizeof...(RestTupleTypes) + 1) >= TupleIndex, "Specified index is out of range.");
	};	// struct TupleElementHelper<TupleIndex, CurrentTupleType, RestTupleTypes>

	template<typename CurrentTupleType, typename... RestTupleTypes>
	struct TupleElementHelper<0, CurrentTupleType, RestTupleTypes...>
	{
		typedef CurrentTupleType Type;
	};	// struct TupleElementHelper<TupleIndex, CurrentTupleType, RestTupleTypes>
	
	//! Objects that can retrieve a type with a specified index from the tuple.
	//! @tparam TupleIndex Index of the desired element.
	//! @tparam TupleTypes Types stored in a tuple.
	template<SizeTp const TupleIndex, typename... TupleTypes>
	struct TupleElement : public TupleElementHelper<TupleIndex, TupleTypes...>
	{
	};	// struct TupleElement

GD_NAMESPACE_END
