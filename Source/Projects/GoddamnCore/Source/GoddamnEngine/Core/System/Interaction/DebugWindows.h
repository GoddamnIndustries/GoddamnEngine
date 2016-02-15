// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/System/Interaction/DebugWindows.h
 * Windows output device definitions.
 */
#pragma once
#if !defined(GD_INSIDE_OUTPUT_OUTPUTDEVICE_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/System/Debug.h> instead.
#endif	// if !defined(GD_INSIDE_OUTPUT_OUTPUTDEVICE_H)

#include <GoddamnEngine/Include.h>

#include <Windows.h>
#include <ConsoleAPI.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**	
	// ******                            'OutputDevice' class.                                 ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Declares a Microsoft console output device.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class ConsoleOutputDeviceWindows final : public DebugOutputDevice
	{
	private:
		HANDLE m_Console;

	public:

		/*!
		 * Initializes a new output device with parameters.
		 *
		 * @param deviceDefaultColor Default output color for this device.
		 * @param deviceDefaultVerbosity Default verbosity for this device.
		 * @param deviceTimeFormat Default time format for this device.
		 * @param deviceDoAutoEmitLineTerminator Do emit new line terminator after each logging event.
		 */
		GDINL explicit ConsoleOutputDeviceWindows(DebugOutputDeviceColor const deviceDefaultColor = DebugOutputDeviceColor::Default
			, DebugOutputDeviceVerbosity const deviceDefaultVerbosity = DebugOutputDeviceVerbosity::Default
		    , DebugOutputDeviceTimeFormat const deviceTimeFormat = DebugOutputDeviceTimeFormat::Default
			, bool const deviceDoAutoEmitLineTerminator = true
			)
			: DebugOutputDevice(deviceDefaultColor, deviceDefaultVerbosity, deviceTimeFormat, deviceDoAutoEmitLineTerminator)
		{
			AllocConsole();
			m_Console = GetStdHandle(STD_OUTPUT_HANDLE);
		}

		GDINL virtual ~ConsoleOutputDeviceWindows()
		{
			FreeConsole();
		}

	public:

		/*!
		 * Sets new color of data printed to this output device.
		 * @param deviceColor New color of data printed to  this output device.
		 */
		GDAPI virtual void SetDataColor(DebugOutputDeviceColor const deviceColor) override final
		{
			if (m_Console != INVALID_HANDLE_VALUE)
			{
				WORD const systemDeviceColor = 0
					| ((deviceColor & DebugOutputDeviceColor::ForegroundDarkRed  ) != 0 ? FOREGROUND_RED : 0)
					| ((deviceColor & DebugOutputDeviceColor::ForegroundDarkGreen) != 0 ? FOREGROUND_GREEN : 0)
					| ((deviceColor & DebugOutputDeviceColor::ForegroundDarkBlue ) != 0 ? FOREGROUND_BLUE : 0)
					| ((deviceColor & DebugOutputDeviceColor::ForegroundIntensity) != 0 ? FOREGROUND_INTENSITY : 0)
					| ((deviceColor & DebugOutputDeviceColor::BackgroundDarkRed  ) != 0 ? BACKGROUND_RED : 0)
					| ((deviceColor & DebugOutputDeviceColor::BackgroundDarkGreen) != 0 ? BACKGROUND_GREEN : 0)
					| ((deviceColor & DebugOutputDeviceColor::BackgroundDarkBlue ) != 0 ? BACKGROUND_BLUE : 0)
					| ((deviceColor & DebugOutputDeviceColor::BackgroundIntensity) != 0 ? BACKGROUND_INTENSITY : 0)
					;
				SetConsoleTextAttribute(m_Console, systemDeviceColor);
			}
		}

		/*!
		 * Internally dumps all buffered data to this output device.
		 */
		GDAPI virtual void Flush() override final 
		{
			if (m_Console == INVALID_HANDLE_VALUE)
			{
				DebugOutputDevice::Flush();
			}
		}

		/*!
		 * Internally writes a string to this output device.
		 * @param message The message to print.
		 */
		GDAPI virtual void Print(CStr const message) override final
		{
			if (m_Console != INVALID_HANDLE_VALUE)
			{
				WriteConsole(m_Console, message, static_cast<DWORD>(CString::Strlen(message)), nullptr, nullptr);
				OutputDebugString(message);
			}
			else
			{
				DebugOutputDevice::Print(message);
			}
		}

	};	// class ConsoleOutputDeviceWindows

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Declares a cross-platform console output device.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	using ConsoleOutputDevice = ConsoleOutputDeviceWindows;

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**	
	// ******                                'Debug' class.                                    ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Declares a Windows debug class.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class DebugWidnows final : public DebugGeneric
	{
		// ------------------------------------------------------------------------------------------
		// Error handling functions.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Formats the assert message. Should not be invoked directly.
		 *
		 * @param data Pointer to assertion description data structure.
		 * @param args Arguments for formating Message string.
		 */
		GDAPI static CStr FormatAssertMessage(DebugAssertData const* const data, va_list const args);

		/*!
		 * Provides inner functionality for handling fatal asserts. Should not be invoked directly.
		 *
		 * @param data Pointer to assertion description data structure.
		 * @param args Arguments for formating Message string.
		 */
		GDAPI static GD_NORETURN void HandleFatalAssertData(DebugAssertData const* const data, va_list const args);

		/*!
		 * Provides inner functionality for handling regular asserts. Should not be invoked directly.
		 *
		 * @param data Pointer to assertion description data structure.
		 * @param args Arguments for formating Message string.
		 */
		GDAPI static DebugAssertDialogResult HandleAssertData(DebugAssertData const* const data, va_list const args);

	};	// class DebugWidnows

GD_NAMESPACE_END
