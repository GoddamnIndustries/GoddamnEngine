// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Containers/InitializerList.h
 * Initializer list class.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <initializer_list>	// for std::initializer_list

GD_NAMESPACE_BEGIN

	// **------------------------------------------------------------------------------------------**
	//! Workaround of standard initializer list with Goddamn-Like naming conversion.
	// **------------------------------------------------------------------------------------------**
	template<typename TElement>
	using InitializerList = ::std::initializer_list<TElement>;

GD_NAMESPACE_END
