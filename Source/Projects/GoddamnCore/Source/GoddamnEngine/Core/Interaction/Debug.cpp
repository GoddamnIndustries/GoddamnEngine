// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Interaction/Debug.cpp
 * Cross-platform output device implementation.
 */
#include <GoddamnEngine/Core/Interaction/Debug.h>
#include <GoddamnEngine/Core/Concurrency/Thread.h>
#include <GoddamnEngine/Core/Containers/Vector.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Templates/UniquePtr.h>

GD_NAMESPACE_BEGIN

// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**	
// ******                            'OutputDevice' class.                                 ******	
// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

/*!
 * Stores pointer to all output devices.
 */
	GDINT static Vector<UniquePtr<DebugOutputDevice>> g_OutputDevices;// = { /*UniquePtr<DebugOutputDevice>(gd_new(ConsoleOutputDevice))*/ };

	/*!
	 * Initializes a new output device with parameters.
	 *
	 * @param deviceDefaultColor Default output color for this device.
	 * @param deviceDefaultVerbosity Default verbosity for this device.
	 * @param deviceTimeFormat Default time format for this device.
	 * @param deviceDoAutoEmitLineTerminator Do emit new line terminator after each logging event.
	 */
	GDAPI DebugOutputDevice::DebugOutputDevice(DebugOutputDeviceColor const deviceDefaultColor /*= OutputDeviceColor::Default*/
		, DebugOutputDeviceVerbosity const deviceDefaultVerbosity /*= OutputDeviceVerbosity::Default*/
		, DebugOutputDeviceTimeFormat const deviceTimeFormat      /*= OutputDeviceTimeFormat::Default*/
		, bool const deviceDoAutoEmitLineTerminator          /*= true*/
		)
		: m_DeviceDefaultColor(deviceDefaultColor), m_DeviceDefaultVerbosity(deviceDefaultVerbosity)
		, m_DeviceTimeFormat(deviceTimeFormat), m_DeviceDoAutoEmitLineTerminator(deviceDoAutoEmitLineTerminator)
	{
		GD_DEBUG_VERIFY(Thread::GetCurrentThreadID() == Thread::GetMainThreadID(), "Creating output devices is allowed only inside main thread.");
		g_OutputDevices.InsertLast(UniquePtr<DebugOutputDevice>(this));
	}

	/*!
	 * Writes a formatted string to this output device.
	 *
	 * @param message The message to print.
	 * @param verbosity The verbosity level of a message.
	 * @param color Overridden color of the output.
	 * @param message The message to print.
	 * @param variableArguments Formatting arguments.
	 */
	GDAPI void DebugOutputDevice::PrintWithVerbosity(DebugOutputDeviceVerbosity const verbosity, DebugOutputDeviceColor const color, CStr const message, va_list variableArguments)
	{
		if (m_DeviceDefaultVerbosity >= verbosity)
		{
			SetDataColor(color);
			switch (m_DeviceTimeFormat)
			{
				case DebugOutputDeviceTimeFormat::SinceStart:
				//	Print(String::Format("[%s] ", IDateTime::GetInstance().GetSecondsString().CStr()).CStr());
					break;

				case DebugOutputDeviceTimeFormat::UTC:
				//	Print(String::Format("[%s] ", IDateTime::GetInstance().GetTimeUTCString().CStr()).CStr());
					break;

				default:
					break;
			}
			Print(variableArguments != nullptr ? String::FormatVa(message, variableArguments).CStr() : message);
			if (m_DeviceDoAutoEmitLineTerminator)
			{
				Print("\n");
			}
			SetDataColor(m_DeviceDefaultColor);
		}
	}

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**	
	// ******                                'Debug' class.                                    ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	// ------------------------------------------------------------------------------------------
	// Logging functions.
	// ------------------------------------------------------------------------------------------

	/*!
	 * Writes a log string.
	 * @param message The message to print.
	 */
	//! @{
	void DebugGeneric::Log(CStr const message)
	{
		for (auto& outputDevices : g_OutputDevices)
		{
			outputDevices->PrintWithVerbosity(DebugOutputDeviceVerbosity::Log, DebugOutputDeviceColor::DefaultLog, message, nullptr);
		}
	}
	void DebugGeneric::LogFormat(CStr const message, ...)
	{
		va_list formatArguments;
		va_start(formatArguments, message);
		for (auto& outputDevices : g_OutputDevices)
		{
			outputDevices->PrintWithVerbosity(DebugOutputDeviceVerbosity::Log, DebugOutputDeviceColor::DefaultLog, message, formatArguments);
		}
		va_end(formatArguments);
	}
	//! @}

	/*!
	 * Writes a warning string.
	 * @param message The message to print.
	 */
	//! @{
	void DebugGeneric::LogWarning(CStr const message)
	{
		for (auto& outputDevices : g_OutputDevices)
		{
			outputDevices->PrintWithVerbosity(DebugOutputDeviceVerbosity::Warning, DebugOutputDeviceColor::DefaultWarning, message, nullptr);
		}
	}
	void DebugGeneric::LogWarningFormat(CStr const message, ...)
	{
		va_list formatArguments;
		va_start(formatArguments, message);
		for (auto& outputDevices : g_OutputDevices)
		{
			outputDevices->PrintWithVerbosity(DebugOutputDeviceVerbosity::Warning, DebugOutputDeviceColor::DefaultWarning, message, formatArguments);
		}
		va_end(formatArguments);
	}
	//! @}

	/*!
	 * Writes a error string.
	 * @param message The message to print.
	 */
	//! @{
	void DebugGeneric::LogError(CStr const message)
	{
		for (auto& outputDevices : g_OutputDevices)
		{
			outputDevices->PrintWithVerbosity(DebugOutputDeviceVerbosity::Error, DebugOutputDeviceColor::DefaultError, message, nullptr);
		}
			
	}
	void DebugGeneric::LogErrorFormat(CStr const message, ...)
	{
		va_list formatArguments;
		va_start(formatArguments, message);
		for (auto& outputDevices : g_OutputDevices)
		{
			outputDevices->PrintWithVerbosity(DebugOutputDeviceVerbosity::Error, DebugOutputDeviceColor::DefaultError, message, formatArguments);
		}
		va_end(formatArguments);
	}
	//! @}

	// ------------------------------------------------------------------------------------------
	// Error handling functions.
	// ------------------------------------------------------------------------------------------

	/*!
	 * Formats the assert message. Should not be invoked directly.
	 *
	 * @param data Pointer to assertion description data structure.
	 * @param args Arguments for formatting Message string.
	 */
	GDAPI CStr DebugGeneric::FormatAssertMessage(DebugAssertData const* const data, va_list const args)
	{
		return "";
	}

	/*!
	 * Provides inner functionality for handling fatal asserts. Should not be invoked directly.
	 *
	 * @param data Pointer to assertion description data structure.
	 * @param args Arguments for formatting Message string.
	 */
	GDAPI void DebugGeneric::HandleFatalAssertData(DebugAssertData const* const data, va_list const args)
	{
		abort();
	}

	/*!
	 * Provides inner functionality for handling regular asserts. Should not be invoked directly.
	 *
	 * @param data Pointer to assertion description data structure.
	 * @param args Arguments for formatting Message string.
	 */
	GDAPI DebugAssertDialogResult DebugGeneric::HandleAssertData(DebugAssertData const* const data, va_list const args)
	{
		return VERIFY_DIALOG_BTN_BREAK;
	}

GD_NAMESPACE_END
