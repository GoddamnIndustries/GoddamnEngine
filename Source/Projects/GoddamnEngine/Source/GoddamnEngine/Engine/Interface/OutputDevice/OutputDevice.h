// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Engine/Interface/OutputDevice/OutputDevice.h
 * Cross-platform output device definitions.
 */
#pragma once
#define GD_INSIDE_OUTPUT_DEVICE_H

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Interface/UniqueInterface.h>
#include <GoddamnEngine/Core/Singleton.h>

GD_NAMESPACE_BEGIN

	// ------------------------------------------------------------------------------------------
	//! Describes a color of data printed to OutputDevice.
	GD_DENUM()
	enum IOutputDeviceColor : UInt8
	{
		IOUTPUTDEVICE_COLOR_FOREGROUND_BLACK		= 0,
		IOUTPUTDEVICE_COLOR_FOREGROUND_DARK_RED		= GD_BIT(0),
		IOUTPUTDEVICE_COLOR_FOREGROUND_DARK_GREEN	= GD_BIT(1),
		IOUTPUTDEVICE_COLOR_FOREGROUND_DARK_BLUE	= GD_BIT(2),
		IOUTPUTDEVICE_COLOR_FOREGROUND_INTENSITY	= GD_BIT(3),
		IOUTPUTDEVICE_COLOR_FOREGROUND_RED			= IOUTPUTDEVICE_COLOR_FOREGROUND_DARK_RED | IOUTPUTDEVICE_COLOR_FOREGROUND_INTENSITY,
		IOUTPUTDEVICE_COLOR_FOREGROUND_GREEN		= IOUTPUTDEVICE_COLOR_FOREGROUND_DARK_GREEN | IOUTPUTDEVICE_COLOR_FOREGROUND_INTENSITY,
		IOUTPUTDEVICE_COLOR_FOREGROUND_BLUE			= IOUTPUTDEVICE_COLOR_FOREGROUND_DARK_BLUE | IOUTPUTDEVICE_COLOR_FOREGROUND_INTENSITY,
		IOUTPUTDEVICE_COLOR_FOREGROUND_DARK_YELLOW	= IOUTPUTDEVICE_COLOR_FOREGROUND_DARK_RED | IOUTPUTDEVICE_COLOR_FOREGROUND_DARK_GREEN,
		IOUTPUTDEVICE_COLOR_FOREGROUND_YELLOW		= IOUTPUTDEVICE_COLOR_FOREGROUND_DARK_YELLOW | IOUTPUTDEVICE_COLOR_FOREGROUND_INTENSITY,
		IOUTPUTDEVICE_COLOR_FOREGROUND_DARK_CYAN	= IOUTPUTDEVICE_COLOR_FOREGROUND_DARK_GREEN | IOUTPUTDEVICE_COLOR_FOREGROUND_DARK_BLUE,
		IOUTPUTDEVICE_COLOR_FOREGROUND_CYAN			= IOUTPUTDEVICE_COLOR_FOREGROUND_DARK_CYAN | IOUTPUTDEVICE_COLOR_FOREGROUND_INTENSITY,
		IOUTPUTDEVICE_COLOR_FOREGROUND_DARK_MAGENDA = IOUTPUTDEVICE_COLOR_FOREGROUND_DARK_RED | IOUTPUTDEVICE_COLOR_FOREGROUND_DARK_BLUE,
		IOUTPUTDEVICE_COLOR_FOREGROUND_MAGENDA		= IOUTPUTDEVICE_COLOR_FOREGROUND_DARK_MAGENDA | IOUTPUTDEVICE_COLOR_FOREGROUND_INTENSITY,
		IOUTPUTDEVICE_COLOR_FOREGROUND_GREY			= IOUTPUTDEVICE_COLOR_FOREGROUND_DARK_RED | IOUTPUTDEVICE_COLOR_FOREGROUND_DARK_GREEN | IOUTPUTDEVICE_COLOR_FOREGROUND_DARK_BLUE,
		IOUTPUTDEVICE_COLOR_FOREGROUND_WHITE		= IOUTPUTDEVICE_COLOR_FOREGROUND_GREY | IOUTPUTDEVICE_COLOR_FOREGROUND_INTENSITY,
		IOUTPUTDEVICE_COLOR_BACKGROUND_BLACK		= 0,
		IOUTPUTDEVICE_COLOR_BACKGROUND_DARK_RED		= GD_BIT(4),
		IOUTPUTDEVICE_COLOR_BACKGROUND_DARK_GREEN	= GD_BIT(5),
		IOUTPUTDEVICE_COLOR_BACKGROUND_DARK_BLUE	= GD_BIT(6),
		IOUTPUTDEVICE_COLOR_BACKGROUND_INTENSITY	= GD_BIT(7),
		IOUTPUTDEVICE_COLOR_BACKGROUND_RED			= IOUTPUTDEVICE_COLOR_BACKGROUND_DARK_RED | IOUTPUTDEVICE_COLOR_BACKGROUND_INTENSITY,
		IOUTPUTDEVICE_COLOR_BACKGROUND_GREEN		= IOUTPUTDEVICE_COLOR_BACKGROUND_DARK_GREEN | IOUTPUTDEVICE_COLOR_BACKGROUND_INTENSITY,
		IOUTPUTDEVICE_COLOR_BACKGROUND_BLUE			= IOUTPUTDEVICE_COLOR_BACKGROUND_DARK_BLUE | IOUTPUTDEVICE_COLOR_BACKGROUND_INTENSITY,
		IOUTPUTDEVICE_COLOR_BACKGROUND_DARK_YELLOW	= IOUTPUTDEVICE_COLOR_BACKGROUND_DARK_RED | IOUTPUTDEVICE_COLOR_BACKGROUND_DARK_GREEN,
		IOUTPUTDEVICE_COLOR_BACKGROUND_YELLOW		= IOUTPUTDEVICE_COLOR_BACKGROUND_DARK_YELLOW | IOUTPUTDEVICE_COLOR_BACKGROUND_INTENSITY,
		IOUTPUTDEVICE_COLOR_BACKGROUND_DARK_CYAN	= IOUTPUTDEVICE_COLOR_BACKGROUND_DARK_GREEN | IOUTPUTDEVICE_COLOR_BACKGROUND_DARK_BLUE,
		IOUTPUTDEVICE_COLOR_BACKGROUND_CYAN			= IOUTPUTDEVICE_COLOR_BACKGROUND_DARK_CYAN | IOUTPUTDEVICE_COLOR_BACKGROUND_INTENSITY,
		IOUTPUTDEVICE_COLOR_BACKGROUND_DARK_MAGENDA = IOUTPUTDEVICE_COLOR_BACKGROUND_DARK_RED | IOUTPUTDEVICE_COLOR_BACKGROUND_DARK_BLUE,
		IOUTPUTDEVICE_COLOR_BACKGROUND_MAGENDA		= IOUTPUTDEVICE_COLOR_BACKGROUND_DARK_MAGENDA | IOUTPUTDEVICE_COLOR_BACKGROUND_INTENSITY,
		IOUTPUTDEVICE_COLOR_BACKGROUND_GREY			= IOUTPUTDEVICE_COLOR_BACKGROUND_DARK_RED | IOUTPUTDEVICE_COLOR_BACKGROUND_DARK_GREEN | IOUTPUTDEVICE_COLOR_BACKGROUND_DARK_BLUE,
		IOUTPUTDEVICE_COLOR_BACKGROUND_WHITE		= IOUTPUTDEVICE_COLOR_BACKGROUND_GREY | IOUTPUTDEVICE_COLOR_BACKGROUND_INTENSITY,
		IOUTPUTDEVICE_COLOR_DEFAULT_LOG				= IOUTPUTDEVICE_COLOR_FOREGROUND_GREY | IOUTPUTDEVICE_COLOR_BACKGROUND_BLACK,
		IOUTPUTDEVICE_COLOR_DEFAULT_WARNING			= IOUTPUTDEVICE_COLOR_FOREGROUND_YELLOW | IOUTPUTDEVICE_COLOR_BACKGROUND_BLACK,
		IOUTPUTDEVICE_COLOR_DEFAULT_ERROR			= IOUTPUTDEVICE_COLOR_FOREGROUND_WHITE | IOUTPUTDEVICE_COLOR_BACKGROUND_RED,
		IOUTPUTDEVICE_COLOR_DEFAULT					= IOUTPUTDEVICE_COLOR_DEFAULT_LOG,
		IOUTPUTDEVICE_COLOR_INVALID					= UInt8Max,
	};	// enum IOutputDeviceColor

	// ------------------------------------------------------------------------------------------
	//! Describes a filter for messages printed to OutputDevice.
	GD_DENUM()
	enum IOutputDeviceVerbosity : UInt8
	{
		IOUTPUTDEVICE_VERBOSITY_ERROR				= 1,
		IOUTPUTDEVICE_VERBOSITY_WARNING				= 2,
		IOUTPUTDEVICE_VERBOSITY_LOG					= 3,
		IOUTPUTDEVICE_VERBOSITY_MIN					= 0,
		IOUTPUTDEVICE_VERBOSITY_MAX					= UInt8Max,
		IOUTPUTDEVICE_VERBOSITY_DEFAULT				= IOUTPUTDEVICE_VERBOSITY_MAX,
	};	// enum IOutputDeviceVerbosity

	// ------------------------------------------------------------------------------------------
	//! Describes a format of time printed to OutputDevice.
	GD_DENUM()
	enum IOutputDeviceTimeFormat : UInt8
	{
		IOUTPUTDEVICE_TIME_FORMAT_NONE				= 0,
		IOUTPUTDEVICE_TIME_FORMAT_UTC				= 1,
		IOUTPUTDEVICE_TIME_FORMAT_SINCESTART		= 2,
		IOUTPUTDEVICE_DEFAULT						= IOUTPUTDEVICE_TIME_FORMAT_SINCESTART,
	};	// enum IOutputDeviceTimeFormat

	// ------------------------------------------------------------------------------------------
	//! Defines an output device.
	GD_DINTERFACE()
	class IGenericOutputDevice : public IRuntimeUniqueInterface
	{
		GD_DEFINE_CLASS_INFORMATION(IGenericOutputDevice, IRuntimeUniqueInterface);

	public:
#if !GD_D_REFLECTOR
		IOutputDeviceColor		DeviceDefaultColor             = IOUTPUTDEVICE_COLOR_DEFAULT;		//!< Default color of data printed to OutputDevice.
		IOutputDeviceVerbosity	DeviceVerbosity				   = IOUTPUTDEVICE_VERBOSITY_DEFAULT;	//!< Filter for messages printed to OutputDevice.
		IOutputDeviceTimeFormat	DeviceTimeFormat               = IOUTPUTDEVICE_DEFAULT;				//!< A format of time printed to OutputDevice.
		bool					DeviceDoAutoEmitLineTerminator = true;								//!< Is true, each printed message will emit line terminator.
#endif	// if !GD_D_REFLECTOR

	public:

		// ------------------------------------------------------------------------------------------
		//! Sets new color of data printed to OutputDevice.
		//! @param DeviceColor New color of data printed to OutputDevice.
		GD_DFUNCTION()
		GDAPI virtual void SetDataColor(IOutputDeviceColor const DeviceColor) GD_PURE_VIRTUAL;

		// ------------------------------------------------------------------------------------------
		//! Internally dumps all buffered data to output device.
		GD_DFUNCTION()
		GDAPI virtual void Flush() GD_PURE_VIRTUAL;

		// ------------------------------------------------------------------------------------------
		//! Closes output device and cleans all data.
		GD_DFUNCTION()
		GDAPI virtual void TearDown() GD_PURE_VIRTUAL;

		// ------------------------------------------------------------------------------------------
		//! Internally writes a string to OutputDevice.
		//! @param Message The message to print.
		GD_DFUNCTION()
		GDAPI virtual void Print(CStr const Message) GD_PURE_VIRTUAL;

		// ------------------------------------------------------------------------------------------
		//! Writes a formatted string to OutputDevice.
		//! @param Verbosity The verbosity level of a message.
		//! @param Color Overridden color of the output.
		//! @param Message The message to print.
		//! @param VariableArguments Formatting arguments.
		GD_DFUNCTION()
		GDAPI virtual void PrintWithVerbosity(IOutputDeviceVerbosity const Verbosity, IOutputDeviceColor const Color, CStr const Message, va_list VariableArguments);
		
#if !GD_D_REFLECTOR
		// ------------------------------------------------------------------------------------------
		//! Writes a log string to OutputDevice.
		//! @param Message The message to print.
		//! @{
		GDINL void Log(CStr const Message)
		{
			PrintWithVerbosity(IOUTPUTDEVICE_VERBOSITY_LOG, IOUTPUTDEVICE_COLOR_DEFAULT_LOG, Message, nullptr);
		}
		GDINL void LogFormat(CStr const Message, ...)
		{
			va_list FormatArguments;
			va_start(FormatArguments, Message);
			PrintWithVerbosity(IOUTPUTDEVICE_VERBOSITY_LOG, IOUTPUTDEVICE_COLOR_DEFAULT_LOG, Message, FormatArguments);
			va_end(FormatArguments);
		}
		//! @}
#endif	// if !GD_D_REFLECTOR

#if !GD_D_REFLECTOR
		// ------------------------------------------------------------------------------------------
		//! Writes a warning string to OutputDevice.
		//! @param Message The message to print.
		//! @{
		GDINL void LogWarning(CStr const Message)
		{
			PrintWithVerbosity(IOUTPUTDEVICE_VERBOSITY_WARNING, IOUTPUTDEVICE_COLOR_DEFAULT_WARNING, Message, nullptr);
		}
		GDINL void LogWarningFormat(CStr const Message, ...)
		{
			va_list FormatArguments;
			va_start(FormatArguments, Message);
			PrintWithVerbosity(IOUTPUTDEVICE_VERBOSITY_WARNING, IOUTPUTDEVICE_COLOR_DEFAULT_WARNING, Message, FormatArguments);
			va_end(FormatArguments);
		}
		//! @}
#endif	// if !GD_D_REFLECTOR

#if !GD_D_REFLECTOR
		// ------------------------------------------------------------------------------------------
		//! Writes a error string to OutputDevice.
		//! @param Message The message to print.
		//! @{
		GDINL void LogError(CStr const Message)
		{
			PrintWithVerbosity(IOUTPUTDEVICE_VERBOSITY_ERROR, IOUTPUTDEVICE_COLOR_DEFAULT_ERROR, Message, nullptr);
		}
		GDINL void LogErrorFormat(CStr const Message, ...)
		{
			va_list FormatArguments;
			va_start(FormatArguments, Message);
			PrintWithVerbosity(IOUTPUTDEVICE_VERBOSITY_ERROR, IOUTPUTDEVICE_COLOR_DEFAULT_ERROR, Message, FormatArguments);
			va_end(FormatArguments);
		}
		//! @}
#endif	// if !GD_D_REFLECTOR
	};	// class IGenericOutputDevice

	// ------------------------------------------------------------------------------------------
	//! Defines a console output device.
	GD_DINTERFACE()
	class IConsoleDevice : public IGenericOutputDevice
#if !GD_D_REFLECTOR	// DReflector should not parse singletons declarations.
		, public Singleton<IConsoleDevice>
#endif	// if !GD_D_REFLECTOR
	{
		GD_DEFINE_CLASS_INFORMATION(IConsoleDevice, IGenericOutputDevice);

	public:

		// ------------------------------------------------------------------------------------------
		//! Shows or hides a console window.
		//! @param DoShow Whether to show or hide a console window.
		GD_DFUNCTION()
		GDAPI virtual void ShowConsole(bool DoShow) GD_PURE_VIRTUAL;

#if !GD_D_REFLECTOR
		// ------------------------------------------------------------------------------------------
		//! Shows a console window.
		GDINL void ShowConsole()
		{
			ShowConsole(true);
		}

		// ------------------------------------------------------------------------------------------
		//! Hides a console window.
		GDINL void HideConsole()
		{
			ShowConsole(false);
		}
#endif	// if !GD_D_REFLECTOR

		// ------------------------------------------------------------------------------------------
		//! Returns true if console window is currently shown or not.
		//! @returns True if console window is currently shown or not.
		GD_DFUNCTION()
		GDAPI virtual bool IsConsoleShown() const GD_PURE_VIRTUAL;

		// ------------------------------------------------------------------------------------------
		//! Returns true if application is already attached to some console window.
		//! @returns True if application is already attached to some console window.
		GD_DFUNCTION()
		GDAPI virtual bool IsConsoleAttached() const GD_PURE_VIRTUAL;

	};	// class IConsoleDevice

	// ------------------------------------------------------------------------------------------
	//! Global date/time interface pointer.
	GD_DSTATIC_VARIABLE()
	GDAPI extern IUniqueInterfacePointer<IConsoleDevice> ConsoleDevice;

GD_NAMESPACE_END
