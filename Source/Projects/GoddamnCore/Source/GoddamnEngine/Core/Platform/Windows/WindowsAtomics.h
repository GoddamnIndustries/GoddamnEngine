// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Platform/Windows/WindowsAtomics.h
//! @note This file should be never directly included, please consider using <GoddamnEngine/Platform/Atomics.h> instead.
//! File contains Windows Atomics implementation.
#pragma once
#if !defined(GD_INSIDE_ATOMICS_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Platform/Atomics.h> instead.
#endif	// if !defined(GD_INSIDE_ATOMICS_H)

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Platform/Platform.h>
#include <GoddamnEngine/Core/Platform/Microsoft/MicrosoftAtomics.h>

GD_NAMESPACE_BEGIN

	//! @brief Represents Windows definitions for atomic instructions.
	typedef MicrosoftAtomics WindowsAtomics;

	//! @brief  Represents definitions for atomic instructions.
	typedef WindowsAtomics Atomics;

GD_NAMESPACE_END
