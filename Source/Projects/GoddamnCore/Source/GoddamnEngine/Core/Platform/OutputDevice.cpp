// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Platform/OutputDevice.cpp
//! Cross-platform output device.

#include <GoddamnEngine/Core/Platform/OutputDevice.h>

GD_NAMESPACE_BEGIN

	//! @brief Writes a formatted string to OutputDevice.
	//! @param Verbosity Verbosity level of a message.
	//! @param Color Overridden color of the output.
	//! @param Message Message to print.
	//! @param VariableArguments Formatting arguments.
	GDAPI void IGenericOutputDevice::PrintWithVerbosity(OutputDeviceVerbosityTp const Verbosity, OutputDeviceColorTp const Color, CStr const Message, va_list VariableArguments)
	{
		if (this->DeviceVerbosity >= Verbosity)
		{
			this->SetDataColor(Color);

			switch (this->DeviceTimeFormat)
			{
				case OutputDeviceTimeFormat::TimeFormatSinceStart:
					this->Print(String::Format("[%s] ", DateTime::GetSecondsString().CStr()));
					break;
				case OutputDeviceTimeFormat::TimeFormatUTC:
					this->Print(String::Format("[%s] ", DateTime::GetTimeUTCString().CStr()));
					break;
			}

			this->Print(VariableArguments != nullptr ? String::FormatVa(Message, VariableArguments).CStr() : Message);
			if (this->DeviceDoAutoEmitLineTerminator)
			{
				this->Print("\n");
			}

			this->SetDataColor(this->DeviceDefaultColor);
		}
	}

GD_NAMESPACE_END
