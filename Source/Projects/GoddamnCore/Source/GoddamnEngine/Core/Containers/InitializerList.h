/// ==========================================================================================
/// InitializerList.h - Initializer list class.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_CONTAINERS_INITIALIZER_LIST
#define GD_CORE_CONTAINERS_INITIALIZER_LIST

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/Containers.h>

#include <initializer_list>	// for std::initializer_list

GD_NAMESPACE_BEGIN

	/// @brief Workaround of standart initializer list with Goddamn-Like naming conversion.
	/// @todo Possibly write a wrapper container class with Goddamn-Like methods naming.
	template<typename ElementType>
	using InitializerList = std::initializer_list<ElementType>;

GD_NAMESPACE_END

#endif	// ifndef GD_CORE_CONTAINERS_INITIALIZER_LIST
