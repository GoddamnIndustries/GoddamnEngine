// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Engine/Interface/OutputDevice/OutputDevice.cpp
 * Cross-platform output device implementation.
 */
#include <GoddamnEngine/Engine/Interface/OutputDevice/OutputDevice.h>
#include <GoddamnEngine/Engine/Interface/DateTime/DateTime.h>
#include <GoddamnEngine/Engine/Pipeline/Pipeline.h>

GD_NAMESPACE_BEGIN

	// ==========================================================================================
	// IGenericOutputDevice interface.
	// ==========================================================================================

	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGenericOutputDevice);

	// ==========================================================================================
	// IConsoleDevice interface.
	// ==========================================================================================

	GDAPI extern IConsoleDevice* CreateIConsoleDeviceInstance();

	// ------------------------------------------------------------------------------------------
	//! Global console output device interface pointer.
	GDAPI IUniqueInterfacePointer<IConsoleDevice> ConsoleDevice(CreateIConsoleDeviceInstance(), PipelineManager->GetMainThreadPipeline());
	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IConsoleDevice);
	GD_SINGLETON_IMPLEMENTATION(IConsoleDevice);

	// ------------------------------------------------------------------------------------------
	//! Writes a formatted string to OutputDevice.
	//! @param Verbosity The verbosity level of a message.
	//! @param Color Overridden color of the output.
	//! @param Message The message to print.
	//! @param VariableArguments Formatting arguments.
	GDAPI void IGenericOutputDevice::PrintWithVerbosity(IOutputDeviceVerbosity const Verbosity, IOutputDeviceColor const Color, CStr const Message, va_list VariableArguments)
	{
		if (DeviceVerbosity >= Verbosity)
		{
			SetDataColor(Color);
			switch (DeviceTimeFormat)
			{
			case IOUTPUTDEVICE_TIME_FORMAT_SINCESTART:
				Print(String::Format("[%s] ", IDateTime::GetInstance().GetSecondsString().CStr()).CStr());
				break;

			case IOUTPUTDEVICE_TIME_FORMAT_UTC:
				Print(String::Format("[%s] ", IDateTime::GetInstance().GetTimeUTCString().CStr()).CStr());
				break;
			}

			Print(VariableArguments != nullptr ? String::FormatVa(Message, VariableArguments).CStr() : Message);
			if (DeviceDoAutoEmitLineTerminator)
				Print("\n");

			SetDataColor(DeviceDefaultColor);
		}
	}

GD_NAMESPACE_END
