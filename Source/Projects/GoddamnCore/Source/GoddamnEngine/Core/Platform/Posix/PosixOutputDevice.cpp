// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Platform/Posix/PosixOutputDevice.ñpp
//! File contains Posix Output Device implementation.

#include <GoddamnEngine/Core/Platform/OutputDevice.h>
#if GD_PLATFORM_API_POSIX

GD_NAMESPACE_BEGIN

	//! @brief Sets new color of data printed to OutputDevice.
	//! @param DeviceColor New color of data printed to OutputDevice.
	GDAPI void PosixOutputDeviceConsole::SetDataColor(OutputDeviceColorTp const DeviceColor)
	{
		// http://www.linuxjournal.com/article/8603
		GD_DEBUG_ASSERT((DeviceColor & 0x0F) != ((DeviceColor & 0xF0) >> 4), "OutputDevice color has same background and foreground.");
		Int32 ParsedAttributes = 0;
		Int32 ParsedForeground;
		Int32 ParsedBackground;

		// Parsing the attributes..
		OutputDeviceColorTp ParsedDeviceColor = DeviceColor;
		if ((ParsedDeviceColor & OutputDeviceColor::ForegroundIntensity) != 0)
		{
			ParsedAttributes = 1;
		}

		// Parsing the foreground color..
		ParsedDeviceColor &= ~OutputDeviceColor::ForegroundIntensity;
		switch (ParsedDeviceColor & 0x0F)
		{
			case OutputDeviceColor::ForegroundBlack:
				ParsedForeground = 0;
				break;
			case OutputDeviceColor::ForegroundDarkRed:
				ParsedForeground = 31;
				break;
			case OutputDeviceColor::ForegroundDarkGreen:
				ParsedForeground = 32;
				break;
			case OutputDeviceColor::ForegroundDarkYellow:
				ParsedForeground = 33;
				break;
			case OutputDeviceColor::ForegroundDarkBlue:
				ParsedForeground = 34;
				break;
			case OutputDeviceColor::ForegroundDarkMagenta:
				ParsedForeground = 35;
				break;
			case OutputDeviceColor::ForegroundDarkCyan:
				ParsedForeground = 36;
				break;
			case OutputDeviceColor::ForegroundGrey:
				ParsedForeground = 37;
				break;
			default:
				ParsedForeground = 37;
				break;
		}

		// Parsing the background color..
		ParsedDeviceColor &= ~OutputDeviceColor::BackgroundIntensity;
		switch (ParsedDeviceColor & 0x0F)
		{
			case OutputDeviceColor::BackgroundBlack:
				ParsedBackground = 0;
				break;
			case OutputDeviceColor::BackgroundDarkRed:
				ParsedBackground = 41;
				break;
			case OutputDeviceColor::BackgroundDarkGreen:
				ParsedBackground = 42;
				break;
			case OutputDeviceColor::BackgroundDarkYellow:
				ParsedBackground = 43;
				break;
			case OutputDeviceColor::BackgroundDarkBlue:
				ParsedBackground = 44;
				break;
			case OutputDeviceColor::BackgroundDarkMagenta:
				ParsedBackground = 45;
				break;
			case OutputDeviceColor::BackgroundDarkCyan:
				ParsedBackground = 46;
				break;
			case OutputDeviceColor::BackgroundGrey:
				ParsedBackground = 47;
				break;
			default:
				ParsedBackground = 0;
				break;
		}

		this->Print(String::Format("\x1B[%d;%d;%dm", ParsedAttributes, ParsedForeground, ParsedBackground));
	}

	//! @brief Internally dumps all buffered data to output device.
	GDAPI void PosixOutputDeviceConsole::Flush()
	{
		// Nothing to do
	}

	//! @brief Closes output device and cleans all data.
	GDAPI void PosixOutputDeviceConsole::TearDown()
	{
		this->Flush();
	}

	//! @brief Internally writes a string to OutputDevice.
	//! @param Message Message to print.
	GDAPI void PosixOutputDeviceConsole::Print(String const& Message)
	{
		std::printf("%s", Message.CStr());
	}

	//! @brief Shows or hides a console window.
	//! @param DoShow Whether to show or hide a console window.
	GDAPI void PosixOutputDeviceConsole::ShowConsole(bool DoShow)
	{
		GD_NOT_USED(DoShow);
		// Nothing to do
	}

	//! @brief Returns true if console window is currently shown or not.
	//! @returns True if console window is currently shown or not.
	GDAPI bool PosixOutputDeviceConsole::IsConsoleShown() const
	{
		return true;
	}

	//! @brief Returns true if application is already attached to some console window.
	//! @returns True if application is already attached to some console window.
	GDAPI bool PosixOutputDeviceConsole::IsConsoleAttached() const
	{
		return false;
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_API_POSIX
