/// ==========================================================================================
/// Algorithm.h - Template algorithms definitions. 
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_TEMPLATE_ALGORITH
#define GD_CORE_TEMPLATE_ALGORITH

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/Utility.h>
#include <GoddamnEngine/Core/Templates/TypeTraits.h>

GD_NAMESPACE_BEGIN

	/// @brief Returns a minimal object of the two specified.
	/// @param First First comparand.
	/// @param Second Second comparand.
	/// @tparam ComparandType Type of the objects. Should contain the '<' operator.
	/// @returns Minimal of two comparands.
	/// @{
	template<typename ComparandType>
	GDINL ComparandType& Min(ComparandType& First, ComparandType& Second)
	{
		return (First < Second) ? First : Second;
	}
	template<typename ComparandType>
	GDINL ComparandType const& Min(ComparandType const& First, ComparandType const& Second)
	{
		return (First < Second) ? First : Second;
	}
	/// @}

	/// @brief Returns a maximum object of the two specified.
	/// @param First First comparand.
	/// @param Second Second comparand.
	/// @tparam ComparandType Type of the objects. Should contain the '>' operator.
	/// @returns Maximum of two comparands.
	/// @{
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
	/// @}

GD_NAMESPACE_END

#endif	// ifndef GD_CORE_TEMPLATE_ALGORITH
