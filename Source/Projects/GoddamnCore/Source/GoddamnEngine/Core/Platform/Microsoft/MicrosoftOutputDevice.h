// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Platform/Microsoft/MicrosoftOutputDevice.h
//! @note This file should be never directly included, please consider using <GoddamnEngine/Platform/OutputDevice.h> instead.
//! File contains Microsoft Output Device implementation.
#pragma once
#if !defined(GD_INSIDE_OUTPUT_DEVICE_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Platform/OutputDevice.h> instead.
#endif	// if !defined(GD_INSIDE_OUTPUT_DEVICE_H)

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Platform/Platform.h>
#include <GoddamnEngine/Core/Platform/OutputDevice.h>

#if !GD_PLATFORM_API_MICROSOFT
#	error Current implementation of output device is not supported on this platform.
#endif	// if !GD_PLATFORM_API_MICROSOFT

#include <Windows.h>
#include <ConsoleAPI.h>

GD_NAMESPACE_BEGIN

	//! Implements a Microsoft-specific console output device.
	class MicrosoftOutputDeviceConsole : public IGenericOutputDeviceConsole
	{
	private:
		HANDLE Console;
		bool IsAttached = false;

	public:

		//! Initializes a console output device.
		//! @param DeviceDefaultColor
		//! @param DeviceVerbosity
		//! @param DeviceTimeFormat
		//! @param DeviceDoAutoEmitLineTerminator
		GDINL MicrosoftOutputDeviceConsole(String ConsoleConfigFilename = ""
			, OutputDeviceColorTp const DeviceDefaultColor = OutputDeviceColor::Default, OutputDeviceVerbosityTp const DeviceVerbosity = OutputDeviceVerbosity::Default
			, OutputDeviceTimeFormatTp const DeviceTimeFormat = OutputDeviceTimeFormat::Default, bool DeviceDoAutoEmitLineTerminator = true)
			: IGenericOutputDeviceConsole(ConsoleConfigFilename, DeviceDefaultColor, DeviceVerbosity, DeviceTimeFormat, DeviceDoAutoEmitLineTerminator)
			, Console(nullptr)
		{
		}

		//! Deinitializes a console output device.
		GDINL virtual ~MicrosoftOutputDeviceConsole()
		{
			this->TearDown();
		}

	private:

		GDAPI void LoadConsoleConfig() { }

		GDAPI void SaveConsoleConfig() { }

	public:

		//! Sets new color of data printed to OutputDevice.
		//! @param DeviceColor New color of data printed to OutputDevice.
		GDAPI virtual void SetDataColor(OutputDeviceColorTp const DeviceColor) override;

		//! Internally dumps all buffered data to output device.
		GDAPI virtual void Flush() override;

		//! Closes output device and cleans all data.
		GDAPI virtual void TearDown() override;

		//! Internally writes a string to OutputDevice.
		//! @param Message Message to print.
		GDAPI virtual void Print(String const& Message) override;

		//! Shows or hides a console window.
		//! @param DoShow Whether to show or hide a console window.
		GDAPI virtual void ShowConsole(bool DoShow) override;

		//! Returns true if console window is currently shown or not.
		//! @returns True if console window is currently shown or not.
		GDAPI virtual bool IsConsoleShown() const override;

		//! Returns true if application is already attached to some console window.
		//! @returns True if application is already attached to some console window.
		GDAPI virtual bool IsConsoleAttached() const override;

	};	// class MicrosoftOutputDevice

GD_NAMESPACE_END
