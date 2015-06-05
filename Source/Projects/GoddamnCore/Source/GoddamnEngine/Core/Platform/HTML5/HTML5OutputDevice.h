// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Platform/HTML5/HTML5OutputDevice.h
//! @note This file should be never directly included, please consider using <GoddamnEngine/Platform/OutputDevice.h> instead.
//! File contains Windows Output Device implementation.
#pragma once
#if !defined(GD_INSIDE_OUTPUT_DEVICE_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Platform/OutputDevice.h> instead.
#endif	// if !defined(GD_INSIDE_INCLUDE_H)

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Platform/Platform.h>
#include <GoddamnEngine/Core/Platform/Posix/PosixOutputDevice.h>

GD_NAMESPACE_BEGIN

	//! Implements a HTML5-specific console output device.
	typedef PosixOutputDeviceConsole HTML5OutputDeviceConsole;

	//! Implements a specific console output device.
	typedef HTML5OutputDeviceConsole OutputDeviceConsole;

GD_NAMESPACE_END
