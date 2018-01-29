// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Interaction/Debug.h
 * Cross-platform output device definitions.
 */
#pragma once
#define GD_INSIDE_OUTPUT_OUTPUTDEVICE_H

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/PlatformSpecificInclude.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/CStdlib/CStdio.h>

GD_NAMESPACE_BEGIN
	
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**	
	// ******                          'DebugOutputDevice' class.                              ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	/*!
	 * Describes a color of data printed to Debug.
	 */
	//! @{
	enum class DebugOutputDeviceColor : UInt8
	{
		/*! 
		 * Foreground color. 
		 */
		//! @{
		ForegroundBlack			= 0,
		ForegroundDarkRed		= GD_BIT(0),
		ForegroundDarkGreen		= GD_BIT(1),
		ForegroundDarkBlue		= GD_BIT(2),
		ForegroundIntensity		= GD_BIT(3),
		ForegroundRed			= ForegroundDarkRed | ForegroundIntensity,
		ForegroundGreen			= ForegroundDarkGreen | ForegroundIntensity,
		ForegroundBlue			= ForegroundDarkBlue | ForegroundIntensity,
		ForegroundDarkYellow	= ForegroundDarkRed | ForegroundDarkGreen,
		ForegroundYellow		= ForegroundDarkYellow | ForegroundIntensity,
		ForegroundDarkCyan		= ForegroundDarkGreen | ForegroundDarkBlue,
		ForegroundCyan			= ForegroundDarkCyan | ForegroundIntensity,
		ForegroundDarkMagenda	= ForegroundDarkRed | ForegroundDarkBlue,
		ForegroundMagenda		= ForegroundDarkMagenda | ForegroundIntensity,
		ForegroundGrey			= ForegroundDarkRed | ForegroundDarkGreen | ForegroundDarkBlue,
		ForegroundWhite			= ForegroundGrey | ForegroundIntensity,
		//! @}

		/*! 
		 * Background color. 
		 */
		//! @{
		BackgroundBlack			= 0,
		BackgroundDarkRed		= GD_BIT(4),
		BackgroundDarkGreen		= GD_BIT(5),
		BackgroundDarkBlue		= GD_BIT(6),
		BackgroundIntensity		= GD_BIT(7),
		BackgroundRed			= BackgroundDarkRed | BackgroundIntensity,
		BackgroundGreen			= BackgroundDarkGreen | BackgroundIntensity,
		BackgroundBlue			= BackgroundDarkBlue | BackgroundIntensity,
		BackgroundDarkYellow	= BackgroundDarkRed | BackgroundDarkGreen,
		BackgroundYellow		= BackgroundDarkYellow | BackgroundIntensity,
		BackgroundDarkCyan		= BackgroundDarkGreen | BackgroundDarkBlue,
		BackgroundCyan			= BackgroundDarkCyan | BackgroundIntensity,
		BackgroundDarkMagenda	= BackgroundDarkRed | BackgroundDarkBlue,
		BackgroundMagenda		= BackgroundDarkMagenda | BackgroundIntensity,
		BackgroundGrey			= BackgroundDarkRed | BackgroundDarkGreen | BackgroundDarkBlue,
		BackgroundWhite			= BackgroundGrey | BackgroundIntensity,
		//! @}

		/*! 
		 * Default presets. 
		 */
		//! @{
		DefaultLog				= ForegroundGrey | BackgroundBlack,
		DefaultWarning			= ForegroundYellow | BackgroundBlack,
		DefaultError			= ForegroundWhite | BackgroundRed,
		Default					= DefaultLog,
		Invalid					= UInt8Max,
		//! @}
	};	// enum class DebugOutputDeviceColor
	GD_ENUM_CLASS_DEFINE_FLAG_OPERATORS(DebugOutputDeviceColor);
	//! @}

	/*!
	 * Describes a filter for messages printed to Debug.
	 */
	enum class DebugOutputDeviceVerbosity : UInt8
	{
		Error   = 1,
		Warning = 2,
		Log     = 3,
		Min		= 0,
		Max		= UInt8Max,
		Default	= Max,
	};	// enum class OutputDeviceVerbosity

	/*!
	 * Describes a format of time printed to Debug.
	 */
	enum class DebugOutputDeviceTimeFormat : UInt8
	{
		None	   = 0,
		UTC		   = 1,
		SinceStart = 2,
		Default    = SinceStart,
	};	// enum DebugOutputDeviceTimeFormat

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Declares a single output device.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class DebugOutputDevice : public IVirtuallyDestructible
	{
	private:
		DebugOutputDeviceColor      const m_DeviceDefaultColor;
		DebugOutputDeviceVerbosity  const m_DeviceDefaultVerbosity;
		DebugOutputDeviceTimeFormat const m_DeviceTimeFormat;
		bool                        const m_DeviceDoAutoEmitLineTerminator;

	protected:

		/*!
		 * Initializes a new output device with parameters.
		 *
		 * @param deviceDefaultColor Default output color for this device.
		 * @param deviceDefaultVerbosity Default verbosity for this device.
		 * @param deviceTimeFormat Default time format for this device.
		 * @param deviceDoAutoEmitLineTerminator Do emit new line terminator after each logging event.
		 */
		GDAPI explicit DebugOutputDevice(DebugOutputDeviceColor const deviceDefaultColor = DebugOutputDeviceColor::Default
			, DebugOutputDeviceVerbosity const deviceDefaultVerbosity = DebugOutputDeviceVerbosity::Default
		    , DebugOutputDeviceTimeFormat const deviceTimeFormat = DebugOutputDeviceTimeFormat::Default
			, bool const deviceDoAutoEmitLineTerminator = true
			);

	public:

		/*!
		 * Sets new color of data printed to this output device.
		 * @param deviceColor New color of data printed to  this output device.
		 */
		GDAPI virtual void SetDataColor(DebugOutputDeviceColor const deviceColor)
		{
			GD_NOT_USED(deviceColor);
		}

		/*!
		 * Internally dumps all buffered data to this output device.
		 */
		GDAPI virtual void Flush()
		{
			CStdio::Fflush(stderr);
		}

		/*!
		 * Internally writes a string to this output device.
		 * @param message The message to print.
		 */
		GDAPI virtual void Print(CStr const message)
		{
			CStdio::Puts(message, stderr);
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
		GDAPI void PrintWithVerbosity(DebugOutputDeviceVerbosity const verbosity, DebugOutputDeviceColor const color, CStr const message, va_list variableArguments);

	};	// class OutputDevice
	
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**	
	// ******                                'Debug' class.                                    ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Declares a generic debug class.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class DebugGeneric : public TNonCreatable
	{
	public:

		// ------------------------------------------------------------------------------------------
		// Logging functions.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Writes a log string.
		 * @param message The message to print.
		 */
		//! @{
		GDAPI static void LogFormat(CStr const message, ...);
		GDAPI static void Log(CStr const message);
		//! @}

		/*!
		 * Writes a warning string.
		 * @param message The message to print.
		 */
		//! @{
		GDAPI static void LogWarningFormat(CStr const message, ...);
		GDAPI static void LogWarning(CStr const message);
		//! @}

		/*!
		 * Writes a error string.
		 * @param message The message to print.
		 */
		//! @{
		GDAPI static void LogErrorFormat(CStr const message, ...);
		GDAPI static void LogError(CStr const message);
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
		GDAPI static CStr FormatAssertMessage(DebugAssertData const* const data, va_list const args);

		/*!
		 * Provides inner functionality for handling fatal asserts. Should not be invoked directly.
		 *
		 * @param data Pointer to assertion description data structure.
		 * @param args Arguments for formatting Message string.
		 */
		GDAPI static GD_NORETURN void HandleFatalAssertData(DebugAssertData const* const data, va_list const args);

		/*!
		 * Provides inner functionality for handling regular asserts. Should not be invoked directly.
		 *
		 * @param data Pointer to assertion description data structure.
		 * @param args Arguments for formatting Message string.
		 */
		GDAPI static DebugAssertDialogResult HandleAssertData(DebugAssertData const* const data, va_list const args);

	};	// class DebugGeneric

	using Debug = DebugGeneric;

GD_NAMESPACE_END

// ReSharper disable once CppUnusedIncludeDirective
#include GD_PLATFORM_INCLUDE(GoddamnEngine/Core/Interaction, Debug)
#undef GD_INSIDE_OUTPUT_OUTPUTDEVICE_H
