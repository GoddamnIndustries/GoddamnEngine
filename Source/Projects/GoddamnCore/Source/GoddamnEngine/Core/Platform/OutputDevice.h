// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Platform/OutputDevice.h
//! Cross-platform output device.
#pragma once
#define GD_INSIDE_OUTPUT_DEVICE_H

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Platform/Platform.h>
#include <GoddamnEngine/Core/Platform/DateTime.h>
#include <GoddamnEngine/Core/Containers/String.h>

GD_NAMESPACE_BEGIN

	// ------------------------------------------------------------------------------------------
	// OutputDevice-specific enumerations.
	// ------------------------------------------------------------------------------------------

	//! Describes a color of data printed to OutputDevice.
	//! @{
	struct OutputDeviceColor final
	{
		typedef UInt8 Type;
		Type static const ForegroundBlack		= 0;	                                                        //!< Black foreground color.
		Type static const ForegroundDarkRed		= GD_BIT(0);	                                                //!< Dark red foreground color.
		Type static const ForegroundDarkGreen	= GD_BIT(1);													//!< Dark green foreground color.
		Type static const ForegroundDarkBlue	= GD_BIT(2);													//!< Dark blue foreground color.
		Type static const ForegroundIntensity	= GD_BIT(3);													//!< Foreground color would be intensified.
		Type static const ForegroundRed			= ForegroundDarkRed | ForegroundIntensity;	                    //!< Red foreground color.
		Type static const ForegroundGreen		= ForegroundDarkGreen | ForegroundIntensity;	                //!< Green foreground color.
		Type static const ForegroundBlue		= ForegroundDarkBlue | ForegroundIntensity;						//!< Blue foreground color.
		Type static const ForegroundDarkYellow	= ForegroundDarkRed | ForegroundDarkGreen;						//!< Dark yellow foreground color.
		Type static const ForegroundYellow		= ForegroundDarkYellow | ForegroundIntensity;	                //!< Yellow foreground color.
		Type static const ForegroundDarkCyan	= ForegroundDarkGreen | ForegroundDarkBlue;						//!< Dark cyan foreground color.
		Type static const ForegroundCyan		= ForegroundDarkCyan | ForegroundIntensity;	                    //!< Cyan foreground color.
		Type static const ForegroundDarkMagenta = ForegroundDarkRed | ForegroundDarkBlue;						//!< Dark magenta foreground color.
		Type static const ForegroundMagenda		= ForegroundDarkMagenta | ForegroundIntensity;					//!< Magenta foreground color.
		Type static const ForegroundGrey		= ForegroundDarkRed | ForegroundDarkGreen | ForegroundDarkBlue;	//!< Grey foreground color.
		Type static const ForegroundWhite		= ForegroundGrey | ForegroundIntensity;							//!< White foreground color.

		Type static const BackgroundBlack		= 0;	                                                        //!< Black background color.
		Type static const BackgroundDarkRed		= GD_BIT(4);	                                                //!< Dark red background color.
		Type static const BackgroundDarkGreen	= GD_BIT(5);													//!< Dark green background color.
		Type static const BackgroundDarkBlue	= GD_BIT(6);													//!< Dark blue background color.
		Type static const BackgroundIntensity	= GD_BIT(7);													//!< Background color would be intensified.
		Type static const BackgroundRed			= BackgroundDarkRed | BackgroundIntensity;	                    //!< Red background color.
		Type static const BackgroundGreen		= BackgroundDarkGreen | BackgroundIntensity;	                //!< Green background color.
		Type static const BackgroundBlue		= BackgroundDarkBlue | BackgroundIntensity;						//!< Blue background color.
		Type static const BackgroundDarkYellow	= BackgroundDarkRed | BackgroundDarkGreen;						//!< Dark yellow background color.
		Type static const BackgroundYellow		= BackgroundDarkYellow | BackgroundIntensity;	                //!< Yellow background color.
		Type static const BackgroundDarkCyan	= BackgroundDarkGreen | BackgroundDarkBlue;						//!< Dark cyan background color.
		Type static const BackgroundCyan		= BackgroundDarkCyan | BackgroundIntensity;	                    //!< Cyan background color.
		Type static const BackgroundDarkMagenta = BackgroundDarkRed | BackgroundDarkBlue;						//!< Dark magenta background color.
		Type static const BackgroundMagenda		= BackgroundDarkMagenta | BackgroundIntensity;					//!< Magenta background color.
		Type static const BackgroundGrey		= BackgroundDarkRed | BackgroundDarkGreen | BackgroundDarkBlue;	//!< Grey background color.
		Type static const BackgroundWhite		= BackgroundGrey | BackgroundIntensity;							//!< White background color.

		Type static const DefaultLog			= ForegroundGrey | BackgroundBlack;	                            //!< Default output device color for logging messages.
		Type static const DefaultWarning		= ForegroundYellow | BackgroundBlack;	                        //!< Default output device color for warnings.
		Type static const DefaultError			= ForegroundWhite | BackgroundRed;	                            //!< Default output device color for errors.

		Type static const Default				= DefaultLog;	                                                //!< Default output device color.
		Type static const Invalid				= UInt8Max;														//!< Invalid output device color.
	};	// struct OutputDeviceColor
	typedef OutputDeviceColor::Type OutputDeviceColorTp;
	//! @}

	//! Describes a filter for messages printed to OutputDevice.
	//! @{
	struct OutputDeviceVerbosity final
	{
		typedef UInt8 Type;
		Type static const VerbosityError		= 1;															//!< Only errors would be printed to output device.
		Type static const VerbosityWarning		= 2;															//!< Only warnings and errors would be printed to output device.
		Type static const VerbosityLog			= 3;															//!< Only logging messages, warnings and errors would be printed to output device.

		Type static const VerbosityMin			= 0;															//!< Nothing would be printed to output device.
		Type static const VerbosityMax			= UInt8Max;														//!< All messages would be printed to OutputDevice.
		
		Type static const Default				= VerbosityMax;													//!< Default filter for messages printed to OutputDevice.
	};	// struct OutputDeviceVerbosity
	typedef OutputDeviceVerbosity::Type OutputDeviceVerbosityTp;
	//! @}

	//! Describes a format of time printed to OutputDevice.
	//! @{
	struct OutputDeviceTimeFormat final
	{
		typedef UInt8 Type;
		Type static const TimeFormatNone		= 0;															//!< Time would not be printed to OutputDevice.
		Type static const TimeFormatUTC			= 1;															//!< Current time would be printed to OutputDevice in UTC format.
		Type static const TimeFormatSinceStart	= 2;															//!< Time in seconds since Engine start would be printed to OutputDevice.

		Type static const Default				= TimeFormatSinceStart;											//!< Default format of time printed to OutputDevice.
	};	// struct OutputDeviceTimeFormat
	typedef OutputDeviceTimeFormat::Type OutputDeviceTimeFormatTp;
	//! @}

	// ------------------------------------------------------------------------------------------
	// OutputDevice generic implementation class.
	// ------------------------------------------------------------------------------------------

	// ------------------------------------------------------------------------------------------
	//! Defines an output device.
	//! @{
	class IGenericOutputDevice : IPlatformObject<IGenericOutputDevice, PlatformObjectFlags::FlagsHasNoTickFunction>
	{
	public:
		OutputDeviceColorTp			DeviceDefaultColor;															//!< Default color of data printed to OutputDevice.
		OutputDeviceVerbosityTp		DeviceVerbosity;															//!< Filter for messages printed to OutputDevice.
		OutputDeviceTimeFormatTp	DeviceTimeFormat;															//!< A format of time printed to OutputDevice.
		bool						DeviceDoAutoEmitLineTerminator;												//!< Is true, each printed message will emit line terminator.

	public:

		// ------------------------------------------------------------------------------------------
		//! Initializes an output device.
		//! @param DeviceDefaultColor
		//! @param DeviceVerbosity
		//! @param DeviceTimeFormat
		//! @param DeviceDoAutoEmitLineTerminator
		GDINL IGenericOutputDevice(OutputDeviceColorTp const DeviceDefaultColor = OutputDeviceColor::Default, OutputDeviceVerbosityTp const DeviceVerbosity = OutputDeviceVerbosity::Default
			, OutputDeviceTimeFormatTp const DeviceTimeFormat = OutputDeviceTimeFormat::Default, bool DeviceDoAutoEmitLineTerminator = true)
			: DeviceDefaultColor(DeviceDefaultColor)
			, DeviceVerbosity(DeviceVerbosity)
			, DeviceTimeFormat(DeviceTimeFormat)
			, DeviceDoAutoEmitLineTerminator(DeviceDoAutoEmitLineTerminator)
		{
		}

		// ------------------------------------------------------------------------------------------
		//! Deinitializes an output device.
		GDINL virtual ~IGenericOutputDevice()
		{
		}

	public:

		// ------------------------------------------------------------------------------------------
		//! Sets new color of data printed to OutputDevice.
		//! @param DeviceColor New color of data printed to OutputDevice.
		GDAPI virtual void SetDataColor(OutputDeviceColorTp const DeviceColor) abstract;

		// ------------------------------------------------------------------------------------------
		//! Internally dumps all buffered data to output device.
		GDAPI virtual void Flush() abstract;

		// ------------------------------------------------------------------------------------------
		//! Closes output device and cleans all data.
		GDAPI virtual void TearDown() abstract;

		// ------------------------------------------------------------------------------------------
		//! Internally writes a string to OutputDevice.
		//! @param Message Message to print.
		GDAPI virtual void Print(String const& Message) abstract;

		// ------------------------------------------------------------------------------------------
		//! Writes a formatted string to OutputDevice.
		//! @param Verbosity Verbosity level of a message.
		//! @param Color Overridden color of the output.
		//! @param Message Message to print.
		//! @param VariableArguments Formatting arguments.
		GDAPI virtual void PrintWithVerbosity(OutputDeviceVerbosityTp const Verbosity, OutputDeviceColorTp const Color, CStr const Message, va_list VariableArguments);
		
		// ------------------------------------------------------------------------------------------
		//! Writes a log string to OutputDevice.
		//! @param Message Message to print.
		//! @{
		GDINL void Log(CStr const Message)
		{
			this->PrintWithVerbosity(OutputDeviceVerbosity::VerbosityLog, OutputDeviceColor::DefaultLog, Message, nullptr);
		}
		GDINL void LogFormat(CStr const Message, ...)
		{
			va_list FormatArguments;
			va_start(FormatArguments, Message);
			this->PrintWithVerbosity(OutputDeviceVerbosity::VerbosityLog, OutputDeviceColor::DefaultLog, Message, FormatArguments);
			va_end(FormatArguments);
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Writes a warning string to OutputDevice.
		//! @param Message Message to print.
		//! @{
		GDINL void LogWarning(CStr const Message)
		{
			this->PrintWithVerbosity(OutputDeviceVerbosity::VerbosityWarning, OutputDeviceColor::DefaultWarning, Message, nullptr);
		}
		GDINL void LogWarningFormat(CStr const Message, ...)
		{
			va_list FormatArguments;
			va_start(FormatArguments, Message);
			this->PrintWithVerbosity(OutputDeviceVerbosity::VerbosityWarning, OutputDeviceColor::DefaultWarning, Message, FormatArguments);
			va_end(FormatArguments);
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Writes a error string to OutputDevice.
		//! @param Message Message to print.
		//! @{
		GDINL void LogError(CStr const Message)
		{
			this->PrintWithVerbosity(OutputDeviceVerbosity::VerbosityError, OutputDeviceColor::DefaultError, Message, nullptr);
		}
		GDINL void LogErrorFormat(CStr const Message, ...)
		{
			va_list FormatArguments;
			va_start(FormatArguments, Message);
			this->PrintWithVerbosity(OutputDeviceVerbosity::VerbosityError, OutputDeviceColor::DefaultError, Message, FormatArguments);
			va_end(FormatArguments);
		}
		//! @}
	};	// class IGenericOutputDevice
	typedef IGenericOutputDevice IOutputDevice;
	//! @}

	// ------------------------------------------------------------------------------------------
	//! Defines a console output device.
	class IGenericOutputDeviceConsole : public IGenericOutputDevice
	{
	public:
		String ConsoleConfigFilename;

	public:

		// ------------------------------------------------------------------------------------------
		//! Initializes a console output device.
		//! @param DeviceDefaultColor
		//! @param DeviceVerbosity
		//! @param DeviceTimeFormat
		//! @param DeviceDoAutoEmitLineTerminator
		GDINL IGenericOutputDeviceConsole(String ConsoleConfigFilename = ""
			, OutputDeviceColorTp const DeviceDefaultColor = OutputDeviceColor::Default, OutputDeviceVerbosityTp const DeviceVerbosity = OutputDeviceVerbosity::Default
			, OutputDeviceTimeFormatTp const DeviceTimeFormat = OutputDeviceTimeFormat::Default, bool DeviceDoAutoEmitLineTerminator = true)
			: IGenericOutputDevice(DeviceDefaultColor, DeviceVerbosity, DeviceTimeFormat, DeviceDoAutoEmitLineTerminator)
			, ConsoleConfigFilename(ConsoleConfigFilename)
		{
		}

		// ------------------------------------------------------------------------------------------
		//! Deinitializes a console output device.
		GDINL virtual ~IGenericOutputDeviceConsole()
		{
			IGenericOutputDevice::~IGenericOutputDevice();
		}

	public:

		// ------------------------------------------------------------------------------------------
		//! Shows or hides a console window.
		//! @param DoShow Whether to show or hide a console window.
		GDAPI virtual void ShowConsole(bool DoShow) abstract;

		// ------------------------------------------------------------------------------------------
		//! Shows a console window.
		GDINL void ShowConsole()
		{
			this->ShowConsole(true);
		}

		// ------------------------------------------------------------------------------------------
		//! Hides a console window.
		GDINL void HideConsole()
		{
			this->ShowConsole(false);
		}

		// ------------------------------------------------------------------------------------------
		//! Returns true if console window is currently shown or not.
		//! @returns True if console window is currently shown or not.
		GDAPI virtual bool IsConsoleShown() const abstract;

		// ------------------------------------------------------------------------------------------
		//! Returns true if application is already attached to some console window.
		//! @returns True if application is already attached to some console window.
		GDAPI virtual bool IsConsoleAttached() const abstract;

	};	// class IGenericOutputDeviceConsole

GD_NAMESPACE_END

#include GD_PLATFORM_INCLUDE(GoddamnEngine/Core/Platform, OutputDevice.h)
#undef GD_INSIDE_OUTPUT_DEVICE_H
