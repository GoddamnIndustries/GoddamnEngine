// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Engine/Interface/OutputDevice/OutputDevicePosix.cpp
 * Posix output device implementation.
 */
#include <GoddamnEngine/Engine/Interface/OutputDevice/OutputDevicePosix.h>
#if GD_PLATFORM_API_POSIX

#include <GoddamnEngine/Core/Containers/String.h>

GD_NAMESPACE_BEGIN

	// ==========================================================================================
	// IConsoleDevicePosix interface.
	// ==========================================================================================

	GD_IMPLEMENT_CLASS_INFORMATION(IConsoleDevicePosix);
	GDAPI IConsoleDevice* CreateIConsoleDeviceInstance()
	{
		return GD_NEW(IConsoleDevicePosix);
	}

	// ------------------------------------------------------------------------------------------
	//! Sets new color of data printed to OutputDevice.
	//! @param DeviceColor New color of data printed to OutputDevice.
	GDAPI void IConsoleDevicePosix::SetDataColor(IOutputDeviceColor const DeviceColor)
	{
		// http://www.linuxjournal.com/article/8603
		GD_DEBUG_ASSERT((DeviceColor & 0x0F) != (DeviceColor & 0xF0) >> 4, "OutputDevice color has same background and foreground.");
		Int32 ParsedAttributes = 0;
		Int32 ParsedForeground;
		Int32 ParsedBackground;

		// Parsing the attributes..
		UInt8 ParsedDeviceColor = DeviceColor;
		if ((ParsedDeviceColor & IOUTPUTDEVICE_COLOR_FOREGROUND_INTENSITY) != 0)
		{
			ParsedAttributes = 1;
		}

		// Parsing the foreground color..
		ParsedDeviceColor &= ~IOUTPUTDEVICE_COLOR_FOREGROUND_INTENSITY;
		switch (ParsedDeviceColor & 0x0F)
		{
			case IOUTPUTDEVICE_COLOR_FOREGROUND_BLACK:
				ParsedForeground = 0;
				break;
			case IOUTPUTDEVICE_COLOR_FOREGROUND_DARK_RED:
				ParsedForeground = 31;
				break;
			case IOUTPUTDEVICE_COLOR_FOREGROUND_DARK_GREEN:
				ParsedForeground = 32;
				break;
			case IOUTPUTDEVICE_COLOR_FOREGROUND_DARK_YELLOW:
				ParsedForeground = 33;
				break;
			case IOUTPUTDEVICE_COLOR_FOREGROUND_DARK_BLUE:
				ParsedForeground = 34;
				break;
			case IOUTPUTDEVICE_COLOR_FOREGROUND_DARK_MAGENDA:
				ParsedForeground = 35;
				break;
			case IOUTPUTDEVICE_COLOR_FOREGROUND_DARK_CYAN:
				ParsedForeground = 36;
				break;
			default:
			case IOUTPUTDEVICE_COLOR_FOREGROUND_GREY:
				ParsedForeground = 37;
				break;
		}

		// Parsing the background color..
		ParsedDeviceColor &= ~IOUTPUTDEVICE_COLOR_BACKGROUND_INTENSITY;
		switch (ParsedDeviceColor & 0x0F)
		{
			case IOUTPUTDEVICE_COLOR_BACKGROUND_BLACK:
				ParsedBackground = 0;
				break;
			case IOUTPUTDEVICE_COLOR_BACKGROUND_DARK_RED:
				ParsedBackground = 41;
				break;
			case IOUTPUTDEVICE_COLOR_BACKGROUND_DARK_GREEN:
				ParsedBackground = 42;
				break;
			case IOUTPUTDEVICE_COLOR_BACKGROUND_DARK_YELLOW:
				ParsedBackground = 43;
				break;
			case IOUTPUTDEVICE_COLOR_BACKGROUND_DARK_BLUE:
				ParsedBackground = 44;
				break;
			case IOUTPUTDEVICE_COLOR_BACKGROUND_DARK_MAGENDA:
				ParsedBackground = 45;
				break;
			case IOUTPUTDEVICE_COLOR_BACKGROUND_DARK_CYAN:
				ParsedBackground = 46;
				break;
			default:
			case IOUTPUTDEVICE_COLOR_BACKGROUND_GREY:
				ParsedBackground = 47;
		}

		Print(String::Format("\x1B[%d;%d;%dm", ParsedAttributes, ParsedForeground, ParsedBackground).CStr());
	}

	// ------------------------------------------------------------------------------------------
	//! Internally dumps all buffered data to output device.
	GDAPI void IConsoleDevicePosix::Flush()
	{
		// Nothing to do
	}

	// ------------------------------------------------------------------------------------------
	//! Closes output device and cleans all data.
	GDAPI void IConsoleDevicePosix::TearDown()
	{
		Flush();
	}

	// ------------------------------------------------------------------------------------------
	//! Internally writes a string to OutputDevice.
	//! @param Message The message to print.
	GDAPI void IConsoleDevicePosix::Print(CStr const Message)
	{
		std::printf("%s", Message);
	}

	// ------------------------------------------------------------------------------------------
	//! Shows or hides a console window.
	//! @param DoShow Whether to show or hide a console window.
	GDAPI void IConsoleDevicePosix::ShowConsole(bool DoShow)
	{
		GD_NOT_USED(DoShow);
		// Nothing to do
	}

	// ------------------------------------------------------------------------------------------
	//! Returns true if console window is currently shown or not.
	//! @returns True if console window is currently shown or not.
	GDAPI bool IConsoleDevicePosix::IsConsoleShown() const
	{
		return true;
	}

	// ------------------------------------------------------------------------------------------
	//! Returns true if application is already attached to some console window.
	//! @returns True if application is already attached to some console window.
	GDAPI bool IConsoleDevicePosix::IsConsoleAttached() const
	{
		return false;
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_API_POSIX
