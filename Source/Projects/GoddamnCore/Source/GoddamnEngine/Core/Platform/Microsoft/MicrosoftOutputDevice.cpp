// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Platform/Microsoft/MicrosoftOutputDevice.cpp
//! File contains Microsoft Output Device implementation.

#include <GoddamnEngine/Core/Platform/OutputDevice.h>
#if GD_PLATFORM_API_MICROSOFT

GD_NAMESPACE_BEGIN
	
	//! @brief Sets new color of data printed to OutputDevice.
	//! @param DeviceColor New color of data printed to OutputDevice.
	GDAPI void MicrosoftOutputDeviceConsole::SetDataColor(OutputDeviceColorTp const DeviceColor)
	{
		if (this->Console != nullptr)
		{
			WORD const MicrosoftOutputDeviceColor = 0
				| ((DeviceColor & OutputDeviceColor::ForegroundDarkRed  ) != 0 ? FOREGROUND_RED       : 0)
				| ((DeviceColor & OutputDeviceColor::ForegroundDarkGreen) != 0 ? FOREGROUND_GREEN     : 0)
				| ((DeviceColor & OutputDeviceColor::ForegroundDarkBlue ) != 0 ? FOREGROUND_BLUE      : 0)
				| ((DeviceColor & OutputDeviceColor::ForegroundIntensity) != 0 ? FOREGROUND_INTENSITY : 0)
				| ((DeviceColor & OutputDeviceColor::BackgroundDarkRed  ) != 0 ? BACKGROUND_RED       : 0)
				| ((DeviceColor & OutputDeviceColor::BackgroundDarkGreen) != 0 ? BACKGROUND_GREEN     : 0)
				| ((DeviceColor & OutputDeviceColor::BackgroundDarkBlue ) != 0 ? BACKGROUND_BLUE      : 0)
				| ((DeviceColor & OutputDeviceColor::BackgroundIntensity) != 0 ? BACKGROUND_INTENSITY : 0)
				;
			SetConsoleTextAttribute(this->Console, MicrosoftOutputDeviceColor);
		}
	}

	//! @brief Internally dumps all buffered data to output device.
	GDAPI void MicrosoftOutputDeviceConsole::Flush()
	{
		// Nothing to do
	}

	//! @brief Closes output device and cleans all data.
	GDAPI void MicrosoftOutputDeviceConsole::TearDown()
	{
		this->Flush();
	}

	//! @brief Internally writes a string to OutputDevice.
	//! @param Message Message to print.
	GDAPI void MicrosoftOutputDeviceConsole::Print(String const& Message)
	{
		if (this->Console != nullptr)
		{
			WriteConsoleA(this->Console, Message.CStr(), static_cast<DWORD>(Message.GetLength()), nullptr, nullptr);
		}
	}

	//! @brief Shows or hides a console window.
	//! @param DoShow Whether to show or hide a console window.
	GDAPI void MicrosoftOutputDeviceConsole::ShowConsole(bool DoShow)
	{
		if (DoShow)
		{
			if (this->Console == nullptr)
			{
				this->IsAttached = !AllocConsole();
				this->Console = GetStdHandle(STD_OUTPUT_HANDLE);
				if (this->Console != INVALID_HANDLE_VALUE)
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

					SetConsoleScreenBufferSize(this->Console, ConsoleContextSize);
					SetWindowPos(GetConsoleWindow(), HWND_TOP, ConsoleCoordX, ConsoleCoordY, 0, 0, SWP_NOSIZE | SWP_NOSENDCHANGING | SWP_NOZORDER);
				}
			}
		}
		else
		{
			if (this->Console != nullptr)
			{
				this->SaveConsoleConfig();
				this->Console = nullptr;
				this->IsAttached = false;
				FreeConsole();
			}
		}
	}

	//! @brief Returns true if console window is currently shown or not.
	//! @returns True if console window is currently shown or not.
	GDAPI bool MicrosoftOutputDeviceConsole::IsConsoleShown() const
	{
		return this->Console != nullptr;
	}

	//! @brief Returns true if application is already attached to some console window.
	//! @returns True if application is already attached to some console window.
	GDAPI bool MicrosoftOutputDeviceConsole::IsConsoleAttached() const
	{
		if (this->Console != nullptr)
		{
			return IsAttached;
		}
		else
		{
			if (AttachConsole(ATTACH_PARENT_PROCESS))
			{
				FreeConsole();
				return false;
			}
			else
			{
				return GetLastError() == ERROR_ACCESS_DENIED;
			}
		}
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_API_MICROSOFT
