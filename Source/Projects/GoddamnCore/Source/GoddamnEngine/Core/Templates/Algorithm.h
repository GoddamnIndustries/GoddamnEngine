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
#include <GoddamnEngine/Core/Templates/Utility.h>
#include <GoddamnEngine/Core/Templates/TypeTraits.h>

GD_NAMESPACE_BEGIN

	//! Returns a minimal object of the two specified.
	//! @param First First comparand.
	//! @param Second Second comparand.
	//! @tparam ComparandType Type of the objects. Should contain the '<' operator.
	//! @returns Minimal of two comparands.
	//! @{
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
	//! @}

	//! Returns a maximum object of the two specified.
	//! @param First First comparand.
	//! @param Second Second comparand.
	//! @tparam ComparandType Type of the objects. Should contain the '>' operator.
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

GD_NAMESPACE_END
