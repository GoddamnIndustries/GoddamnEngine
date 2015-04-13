// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Platform/Windows/WindowsDateTime.h
//! @note This file should be never directly included, please consider using <GoddamnEngine/Core/Platform/DateTime.h> instead.
//! Windows date/time definitions.
#pragma once
#if !defined(GD_INSIDE_DATETIME_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Core/Platform/DateTime.hh> instead.
#endif	// if !defined(GD_INSIDE_DATETIME_H)

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Platform/Platform.h>
#include <GoddamnEngine/Core/Platform/Microsoft/MicrosoftDateTime.h>

GD_NAMESPACE_BEGIN

	//! @brief Defines a Windows date/time manager class.
	typedef MicrosoftDateTime WindowsDateTime;

	//! @brief Defines a date/time manager class.
	typedef WindowsDateTime DateTime;

GD_NAMESPACE_END
