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

	//! @brief Describes a color of data printed to OutputDevice.
	//! @{
	struct OutputDeviceColor final
	{
		typedef UInt8 Type;
		Type static const ForegroundBlack		= 0;	                                                        //!< @brief Black foreground color.
		Type static const ForegroundDarkRed		= GD_BIT(0);	                                                //!< @brief Dark red foreground color.
		Type static const ForegroundDarkGreen	= GD_BIT(1);													//!< @brief Dark green foreground color.
		Type static const ForegroundDarkBlue	= GD_BIT(2);													//!< @brief Dark blue foreground color.
		Type static const ForegroundIntensity	= GD_BIT(3);													//!< @brief Foreground color would be intensified.
		Type static const ForegroundRed			= ForegroundDarkRed | ForegroundIntensity;	                    //!< @brief Red foreground color.
		Type static const ForegroundGreen		= ForegroundDarkGreen | ForegroundIntensity;	                //!< @brief Green foreground color.
		Type static const ForegroundBlue		= ForegroundDarkBlue | ForegroundIntensity;						//!< @brief Blue foreground color.
		Type static const ForegroundDarkYellow	= ForegroundDarkRed | ForegroundDarkGreen;						//!< @brief Dark yellow foreground color.
		Type static const ForegroundYellow		= ForegroundDarkYellow | ForegroundIntensity;	                //!< @brief Yellow foreground color.
		Type static const ForegroundDarkCyan	= ForegroundDarkGreen | ForegroundDarkBlue;						//!< @brief Dark cyan foreground color.
		Type static const ForegroundCyan		= ForegroundDarkCyan | ForegroundIntensity;	                    //!< @brief Cyan foreground color.
		Type static const ForegroundDarkMagenta = ForegroundDarkRed | ForegroundDarkBlue;						//!< @brief Dark magenta foreground color.
		Type static const ForegroundMagenda		= ForegroundDarkMagenta | ForegroundIntensity;					//!< @brief Magenta foreground color.
		Type static const ForegroundGrey		= ForegroundDarkRed | ForegroundDarkGreen | ForegroundDarkBlue;	//!< @brief Grey foreground color.
		Type static const ForegroundWhite		= ForegroundGrey | ForegroundIntensity;							//!< @brief White foreground color.

		Type static const BackgroundBlack		= 0;	                                                        //!< @brief Black background color.
		Type static const BackgroundDarkRed		= GD_BIT(4);	                                                //!< @brief Dark red background color.
		Type static const BackgroundDarkGreen	= GD_BIT(5);													//!< @brief Dark green background color.
		Type static const BackgroundDarkBlue	= GD_BIT(6);													//!< @brief Dark blue background color.
		Type static const BackgroundIntensity	= GD_BIT(7);													//!< @brief Background color would be intensified.
		Type static const BackgroundRed			= BackgroundDarkRed | BackgroundIntensity;	                    //!< @brief Red background color.
		Type static const BackgroundGreen		= BackgroundDarkGreen | BackgroundIntensity;	                //!< @brief Green background color.
		Type static const BackgroundBlue		= BackgroundDarkBlue | BackgroundIntensity;						//!< @brief Blue background color.
		Type static const BackgroundDarkYellow	= BackgroundDarkRed | BackgroundDarkGreen;						//!< @brief Dark yellow background color.
		Type static const BackgroundYellow		= BackgroundDarkYellow | BackgroundIntensity;	                //!< @brief Yellow background color.
		Type static const BackgroundDarkCyan	= BackgroundDarkGreen | BackgroundDarkBlue;						//!< @brief Dark cyan background color.
		Type static const BackgroundCyan		= BackgroundDarkCyan | BackgroundIntensity;	                    //!< @brief Cyan background color.
		Type static const BackgroundDarkMagenta = BackgroundDarkRed | BackgroundDarkBlue;						//!< @brief Dark magenta background color.
		Type static const BackgroundMagenda		= BackgroundDarkMagenta | BackgroundIntensity;					//!< @brief Magenta background color.
		Type static const BackgroundGrey		= BackgroundDarkRed | BackgroundDarkGreen | BackgroundDarkBlue;	//!< @brief Grey background color.
		Type static const BackgroundWhite		= BackgroundGrey | BackgroundIntensity;							//!< @brief White background color.

		Type static const DefaultLog			= ForegroundGrey | BackgroundBlack;	                            //!< @brief Default output device color for logging messages.
		Type static const DefaultWarning		= ForegroundYellow | BackgroundBlack;	                        //!< @brief Default output device color for warnings.
		Type static const DefaultError			= ForegroundWhite | BackgroundRed;	                            //!< @brief Default output device color for errors.

		Type static const Default				= DefaultLog;	                                                //!< @brief Default output device color.
		Type static const Invalid				= UInt8Max;														//!< @brief Invalid output device color.
	};	// struct OutputDeviceColor
	typedef OutputDeviceColor::Type OutputDeviceColorTp;
	//! @}

	//! @brief Describes a filter for messages printed to OutputDevice.
	//! @{
	struct OutputDeviceVerbosity final
	{
		typedef UInt8 Type;
		Type static const VerbosityError		= 1;															//!< @brief Only errors would be printed to output device.
		Type static const VerbosityWarning		= 2;															//!< @brief Only warnings and errors would be printed to output device.
		Type static const VerbosityLog			= 3;															//!< @brief Only logging messages, warnings and errors would be printed to output device.

		Type static const VerbosityMin			= 0;															//!< @brief Nothing would be printed to output device.
		Type static const VerbosityMax			= UInt8Max;														//!< @brief All messages would be printed to OutputDevice.
		
		Type static const Default				= VerbosityMax;													//!< @brief Default filter for messages printed to OutputDevice.
	};	// struct OutputDeviceVerbosity
	typedef OutputDeviceVerbosity::Type OutputDeviceVerbosityTp;
	//! @}

	//! @brief Describes a format of time printed to OutputDevice.
	//! @{
	struct OutputDeviceTimeFormat final
	{
		typedef UInt8 Type;
		Type static const TimeFormatNone		= 0;															//!< @brief Time would not be printed to OutputDevice.
		Type static const TimeFormatUTC			= 1;															//!< @brief Current time would be printed to OutputDevice in UTC format.
		Type static const TimeFormatSinceStart	= 2;															//!< @brief Time in seconds since Engine start would be printed to OutputDevice.

		Type static const Default				= TimeFormatSinceStart;											//!< @brief Default format of time printed to OutputDevice.
	};	// struct OutputDeviceTimeFormat
	typedef OutputDeviceTimeFormat::Type OutputDeviceTimeFormatTp;
	//! @}

	// ------------------------------------------------------------------------------------------
	// OutputDevice generic implementation class.
	// ------------------------------------------------------------------------------------------

	//! @brief Defines an output device.
	//! @{
	class IGenericOutputDevice : IPlatformObject<IGenericOutputDevice, PlatformObjectFlags::FlagsHasNoTickFunction>
	{
	public:
		OutputDeviceColorTp			DeviceDefaultColor;															//!< @brief Default color of data printed to OutputDevice.
		OutputDeviceVerbosityTp		DeviceVerbosity;															//!< @brief Filter for messages printed to OutputDevice.
		OutputDeviceTimeFormatTp	DeviceTimeFormat;															//!< @brief A format of time printed to OutputDevice.
		bool						DeviceDoAutoEmitLineTerminator;												//!< @brief Is true, each printed message will emit line terminator.

	public:

		//! @brief Initializes an output device.
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

		//! @brief Deinitializes an output device.
		GDINL virtual ~IGenericOutputDevice()
		{
		}

	public:

		//! @brief Sets new color of data printed to OutputDevice.
		//! @param DeviceColor New color of data printed to OutputDevice.
		GDAPI virtual void SetDataColor(OutputDeviceColorTp const DeviceColor) abstract;

		//! @brief Internally dumps all buffered data to output device.
		GDAPI virtual void Flush() abstract;

		//! @brief Closes output device and cleans all data.
		GDAPI virtual void TearDown() abstract;

		//! @brief Internally writes a string to OutputDevice.
		//! @param Message Message to print.
		GDAPI virtual void Print(String const& Message) abstract;

		//! @brief Writes a formatted string to OutputDevice.
		//! @param Verbosity Verbosity level of a message.
		//! @param Color Overridden color of the output.
		//! @param Message Message to print.
		//! @param VariableArguments Formatting arguments.
		GDAPI virtual void PrintWithVerbosity(OutputDeviceVerbosityTp const Verbosity, OutputDeviceColorTp const Color, CStr const Message, va_list VariableArguments);
		
		//! @brief Writes a log string to OutputDevice.
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

		//! @brief Writes a warning string to OutputDevice.
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

		//! @brief Writes a error string to OutputDevice.
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

	//! @brief Defines a console output device.
	class IGenericOutputDeviceConsole : public IGenericOutputDevice
	{
	public:
		String ConsoleConfigFilename;

	public:

		//! @brief Initializes a console output device.
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

		//! @brief Deinitializes a console output device.
		GDINL virtual ~IGenericOutputDeviceConsole()
		{
			IGenericOutputDevice::~IGenericOutputDevice();
		}

	public:

		//! @brief Shows or hides a console window.
		//! @param DoShow Whether to show or hide a console window.
		GDAPI virtual void ShowConsole(bool DoShow) abstract;

		//! @brief Shows a console window.
		GDINL void ShowConsole()
		{
			this->ShowConsole(true);
		}

		//! @brief Hides a console window.
		GDINL void HideConsole()
		{
			this->ShowConsole(false);
		}

		//! @brief Returns true if console window is currently shown or not.
		//! @returns True if console window is currently shown or not.
		GDAPI virtual bool IsConsoleShown() const abstract;

		//! @brief Returns true if application is already attached to some console window.
		//! @returns True if application is already attached to some console window.
		GDAPI virtual bool IsConsoleAttached() const abstract;

	};	// class IGenericOutputDeviceConsole

GD_NAMESPACE_END

#include GD_PLATFORM_INCLUDE(GoddamnEngine/Core/Platform, OutputDevice.h)
#undef GD_INSIDE_OUTPUT_DEVICE_H
