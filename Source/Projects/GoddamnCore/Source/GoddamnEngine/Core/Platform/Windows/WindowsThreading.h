// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Platform/Windows/WindowsThreading.h
//! @note This file should be never directly included, please consider using <GoddamnEngine/Platform/Threading.h> instead.
//! File contains Windows Atomics implementation.
#pragma once
#if !defined(GD_INSIDE_THREADING_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Platform/Threading.h> instead.
#endif	// if !defined(GD_INSIDE_THREADING_H)

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Platform/Platform.h>
#include <GoddamnEngine/Core/Platform/Microsoft/MicrosoftThreading.h>

GD_NAMESPACE_BEGIN

	//! @brief Windows Critical Section class.
	typedef MicrosoftCriticalSection WindowsCriticalSection;

	//! @brief Represents definitions for Critical Section.
	typedef WindowsCriticalSection CriticalSection;

GD_NAMESPACE_END
