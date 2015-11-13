// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Engine/Interface/OutputDevice/OutputDeviceMicrosoft.cpp
 * Microsoft output device implementation.
 */
#include <GoddamnEngine/Engine/Interface/OutputDevice/OutputDeviceMicrosoft.h>
#if GD_PLATFORM_API_MICROSOFT

GD_NAMESPACE_BEGIN
	
	// ==========================================================================================
	// IConsoleDeviceMicrosoft interface.
	// ==========================================================================================

	GD_IMPLEMENT_CLASS_INFORMATION(IConsoleDeviceMicrosoft);
	GDAPI IConsoleDevice* CreateIConsoleDeviceInstance()
	{
		return GD_NEW(IConsoleDeviceMicrosoft);
	}

	// ------------------------------------------------------------------------------------------
	//! Sets new color of data printed to OutputDevice.
	//! @param DeviceColor New color of data printed to OutputDevice.
	GDAPI void IConsoleDeviceMicrosoft::SetDataColor(IOutputDeviceColor const DeviceColor)
	{
		if (console != nullptr)
		{
			WORD const MicrosoftOutputDeviceColor = 0
				| ((DeviceColor & IOUTPUTDEVICE_COLOR_FOREGROUND_DARK_RED  ) != 0 ? FOREGROUND_RED       : 0)
				| ((DeviceColor & IOUTPUTDEVICE_COLOR_FOREGROUND_DARK_GREEN) != 0 ? FOREGROUND_GREEN     : 0)
				| ((DeviceColor & IOUTPUTDEVICE_COLOR_FOREGROUND_DARK_BLUE ) != 0 ? FOREGROUND_BLUE      : 0)
				| ((DeviceColor & IOUTPUTDEVICE_COLOR_FOREGROUND_INTENSITY ) != 0 ? FOREGROUND_INTENSITY : 0)
				| ((DeviceColor & IOUTPUTDEVICE_COLOR_BACKGROUND_DARK_RED  ) != 0 ? BACKGROUND_RED       : 0)
				| ((DeviceColor & IOUTPUTDEVICE_COLOR_BACKGROUND_DARK_GREEN) != 0 ? BACKGROUND_GREEN     : 0)
				| ((DeviceColor & IOUTPUTDEVICE_COLOR_BACKGROUND_DARK_BLUE ) != 0 ? BACKGROUND_BLUE      : 0)
				| ((DeviceColor & IOUTPUTDEVICE_COLOR_BACKGROUND_INTENSITY ) != 0 ? BACKGROUND_INTENSITY : 0)
				;
			SetConsoleTextAttribute(console, MicrosoftOutputDeviceColor);
		}
	}

	// ------------------------------------------------------------------------------------------
	//! Internally dumps all buffered data to output device.
	GDAPI void IConsoleDeviceMicrosoft::Flush()
	{
		// Nothing to do
	}

	// ------------------------------------------------------------------------------------------
	//! Closes output device and cleans all data.
	GDAPI void IConsoleDeviceMicrosoft::TearDown()
	{
		Flush();
	}

	// ------------------------------------------------------------------------------------------
	//! Internally writes a string to OutputDevice.
	//! @param Message The message to print.
	GDAPI void IConsoleDeviceMicrosoft::Print(CStr const Message)
	{
		if (console != nullptr)
		{
			WriteConsole(console, Message, static_cast<DWORD>(CString::Strlen(Message)), nullptr, nullptr);
#if GD_DEBUG
			OutputDebugString(Message);
#endif	// if GD_DEBUG
		}
	}

	// ------------------------------------------------------------------------------------------
	//! Shows or hides a console window.
	//! @param DoShow Whether to show or hide a console window.
	GDAPI void IConsoleDeviceMicrosoft::ShowConsole(bool DoShow)
	{
		if (DoShow)
		{
			if (console == nullptr)
			{
				isAttached = !AllocConsole();
				console = GetStdHandle(STD_OUTPUT_HANDLE);
				if (console != INVALID_HANDLE_VALUE)
				{
					Int32 ConsoleWidth = 160;
					Int32 ConsoleHeight = 4000;
					Int32 ConsoleCoordX = 0;
					Int32 ConsoleCoordY = 0;
					bool ConsoleCoorXSpecified = false;
					bool ConsoleCoorYSpecified = false;

					//if (false)
					//{
					//	//! @todo Set console window properties.
					//}

					COORD ConsoleContextSize = { static_cast<SHORT>(ConsoleWidth), static_cast<SHORT>(ConsoleHeight) };
					
					RECT ConsoleWindowRect = { 0 };
					GetWindowRect(GetConsoleWindow(), &ConsoleWindowRect);
					if (!ConsoleCoorXSpecified)
					{
						ConsoleCoordX = ConsoleWindowRect.left;
					}
					if (!ConsoleCoorYSpecified)
					{
						ConsoleCoordY = ConsoleWindowRect.top;
					}

					SetConsoleScreenBufferSize(console, ConsoleContextSize);
					SetWindowPos(GetConsoleWindow(), HWND_TOP, ConsoleCoordX, ConsoleCoordY, 0, 0, SWP_NOSIZE | SWP_NOSENDCHANGING | SWP_NOZORDER);
				}
			}
		}
		else
		{
			if (console != nullptr)
			{
				SaveConsoleConfig();
				console = nullptr;
				isAttached = false;
				FreeConsole();
			}
		}
	}

	// ------------------------------------------------------------------------------------------
	//! Returns true if console window is currently shown or not.
	//! @returns True if console window is currently shown or not.
	GDAPI bool IConsoleDeviceMicrosoft::IsConsoleShown() const
	{
		return console != nullptr;
	}

	// ------------------------------------------------------------------------------------------
	//! Returns true if application is already attached to some console window.
	//! @returns True if application is already attached to some console window.
	GDAPI bool IConsoleDeviceMicrosoft::IsConsoleAttached() const
	{
		if (console != nullptr)
		{
			return isAttached;
		}
		if (AttachConsole(ATTACH_PARENT_PROCESS))
		{
			FreeConsole();
			return false;
		}
		return GetLastError() == ERROR_ACCESS_DENIED;
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_API_MICROSOFT
