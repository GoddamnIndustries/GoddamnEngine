// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Containers/InitializerList.h
//! Initializer list class.
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/Containers.h>

#include <initializer_list>	// for std::initializer_list

GD_NAMESPACE_BEGIN

	//! @brief Workaround of standart initializer list with Goddamn-Like naming conversion.
	//! @todo Possibly write a wrapper container class with Goddamn-Like methods naming.
	template<typename ElementType>
	using InitializerList = std::initializer_list<ElementType>;

GD_NAMESPACE_END
