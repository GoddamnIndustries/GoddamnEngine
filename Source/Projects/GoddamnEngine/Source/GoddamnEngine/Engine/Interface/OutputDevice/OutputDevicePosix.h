// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Engine/Interface/OutputDevice/OutputDevicePosix.h
 * @note This file should be never directly included, please consider using <GoddamnEngine/Engine/Interface/OutputDevice/OutputDevice.h> instead.
 * Posix output device definitions.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#if GD_PLATFORM_API_POSIX 

#include <GoddamnEngine/Engine/Interface/OutputDevice/OutputDevice.h>

GD_NAMESPACE_BEGIN

	// ------------------------------------------------------------------------------------------
	//! Implements a Posix-specific console output device.
	class IConsoleDevicePosix : public IConsoleDevice
	{
		GD_DEFINE_CLASS_INFORMATION(IConsoleDevicePosix, IConsoleDevice);

	public:
		GDAPI virtual void SetDataColor(IOutputDeviceColor const DeviceColor) override;
		GDAPI virtual void Flush() override;
		GDAPI virtual void TearDown() override;
		GDAPI virtual void Print(CStr const Message) override;
		GDAPI virtual void ShowConsole(bool DoShow) override;
		GDAPI virtual bool IsConsoleShown() const override;
		GDAPI virtual bool IsConsoleAttached() const override;
	};	// class IConsoleDevicePosix

GD_NAMESPACE_END\

#endif	// if GD_PLATFORM_API_POSIX
