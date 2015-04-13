// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Platform/Windows/WindowsInput.cpp
//! Windows Input Interface definitions.

#include <GoddamnEngine/Core/Platform/Input.h>
#if GD_PLATFORM_WINDOWS 

#include <Xinput.h>

GD_NAMESPACE_BEGIN

	GDINL static Float32 NormalizeShort(SHORT const Value)
	{
		Float32 const Norm = Value >= 0 ? static_cast<Float32>(UInt16Max) : static_cast<Float32>(-UInt16Min);
		return static_cast<Float32>(Value) / Norm;
	}

	GDINL static Float32 NormalizeByte(BYTE const Value)
	{
		return static_cast<Float32>(Value) / static_cast<Float32>(UInt8Max);
	}

	//! @brief Initializes an input interface.
	void WindowsInput::InitializeObject_Inst()
	{
	
	}

	//! @brief Destroys an input interface.
	void WindowsInput::DestroyObject_Inst()
	{
	}

	//! @brief Ticks a frame.
	void WindowsInput::TickObject_Inst()
	{
		for (SizeTp InputGamepadIndex = 0; InputGamepadIndex < MaxSupportedGamepads; ++InputGamepadIndex)
		{
			auto& InputGamepadState = this->InputGamepadStates[InputGamepadIndex];
			if (InputGamepadState.GamepadIsConnected)
			{
				InputGamepadState.GamepadID = static_cast<Int32>(InputGamepadIndex);

				XINPUT_STATE InputXControllerState = { 0 };
				DWORD const	 InputXControllerResult = XInputGetState(InputGamepadState.GamepadID, &InputXControllerState) == ERROR_SUCCESS;
				if (InputXControllerResult != 0)
				{
					InputGamepadState.GamepadIsConnected = true;

					UpdateButtonState(InputGamepadState.GamepadPressedButtons[GamepadButtons::ButtonA],					(InputXControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0);
					UpdateButtonState(InputGamepadState.GamepadPressedButtons[GamepadButtons::ButtonB],					(InputXControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0);
					UpdateButtonState(InputGamepadState.GamepadPressedButtons[GamepadButtons::ButtonX],					(InputXControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0);
					UpdateButtonState(InputGamepadState.GamepadPressedButtons[GamepadButtons::ButtonY],					(InputXControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0);

					UpdateButtonState(InputGamepadState.GamepadPressedButtons[GamepadButtons::ButtonDPadDown],			(InputXControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0);
					UpdateButtonState(InputGamepadState.GamepadPressedButtons[GamepadButtons::ButtonDPadRight],			(InputXControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0);
					UpdateButtonState(InputGamepadState.GamepadPressedButtons[GamepadButtons::ButtonDPadLeft],			(InputXControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0);
					UpdateButtonState(InputGamepadState.GamepadPressedButtons[GamepadButtons::ButtonDPadUp],			(InputXControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0);

					UpdateButtonState(InputGamepadState.GamepadPressedButtons[GamepadButtons::ButtonRB],				(InputXControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0);
					UpdateButtonState(InputGamepadState.GamepadPressedButtons[GamepadButtons::ButtonLB],				(InputXControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0);
					UpdateButtonState(InputGamepadState.GamepadPressedButtons[GamepadButtons::ButtonRT],				(InputXControllerState.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD));
					UpdateButtonState(InputGamepadState.GamepadPressedButtons[GamepadButtons::ButtonLT],				(InputXControllerState.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD));

					InputGamepadState.GamepadRightTriggerValue	= NormalizeByte(InputXControllerState.Gamepad.bRightTrigger);
					InputGamepadState.GamepadLeftTriggerValue	= NormalizeByte(InputXControllerState.Gamepad.bLeftTrigger);

					UpdateButtonState(InputGamepadState.GamepadPressedButtons[GamepadButtons::ButtonRightStick],		(InputXControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0);
					UpdateButtonState(InputGamepadState.GamepadPressedButtons[GamepadButtons::ButtonRightStickDown],	(InputXControllerState.Gamepad.sThumbRY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE));
					UpdateButtonState(InputGamepadState.GamepadPressedButtons[GamepadButtons::ButtonRightStickRight],	(InputXControllerState.Gamepad.sThumbRX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE));
					UpdateButtonState(InputGamepadState.GamepadPressedButtons[GamepadButtons::ButtonRightStickLeft],	(InputXControllerState.Gamepad.sThumbRX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE));
					UpdateButtonState(InputGamepadState.GamepadPressedButtons[GamepadButtons::ButtonRightStickUp],		(InputXControllerState.Gamepad.sThumbRY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE));

					InputGamepadState.GamepadRightStickRaw.x	= NormalizeShort(InputXControllerState.Gamepad.sThumbRX);
					InputGamepadState.GamepadRightStickRaw.y	= NormalizeShort(InputXControllerState.Gamepad.sThumbRY);

					UpdateButtonState(InputGamepadState.GamepadPressedButtons[GamepadButtons::ButtonLeftStick],			(InputXControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0);
					UpdateButtonState(InputGamepadState.GamepadPressedButtons[GamepadButtons::ButtonLeftStickDown],		(InputXControllerState.Gamepad.sThumbRY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE));
					UpdateButtonState(InputGamepadState.GamepadPressedButtons[GamepadButtons::ButtonLeftStickRight],	(InputXControllerState.Gamepad.sThumbRX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE));
					UpdateButtonState(InputGamepadState.GamepadPressedButtons[GamepadButtons::ButtonLeftStickLeft],		(InputXControllerState.Gamepad.sThumbRX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE));
					UpdateButtonState(InputGamepadState.GamepadPressedButtons[GamepadButtons::ButtonLeftStickUp],		(InputXControllerState.Gamepad.sThumbRY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE));

					InputGamepadState.GamepadLeftStickRaw.x		= NormalizeShort(InputXControllerState.Gamepad.sThumbLX);
					InputGamepadState.GamepadLeftStickRaw.y		= NormalizeShort(InputXControllerState.Gamepad.sThumbLY);

					UpdateButtonState(InputGamepadState.GamepadPressedButtons[GamepadButtons::ButtonOptions],			(InputXControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0);
					UpdateButtonState(InputGamepadState.GamepadPressedButtons[GamepadButtons::ButtonMenu],				(InputXControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0);

					XINPUT_VIBRATION InputXGamepadVibration = { 0 };
					InputXGamepadVibration.wRightMotorSpeed = static_cast<WORD>(static_cast<Float32>(UInt16Max) * Max(InputGamepadState.GamepadVibrationFeedback.VibrationRightLarge, InputGamepadState.GamepadVibrationFeedback.VibrationRightSmall));
					InputXGamepadVibration.wLeftMotorSpeed  = static_cast<WORD>(static_cast<Float32>(UInt16Max) * Max(InputGamepadState.GamepadVibrationFeedback.VibrationLeftLarge, InputGamepadState.GamepadVibrationFeedback.VibrationLeftSmall));
					XInputSetState(InputGamepadState.GamepadID, &InputXGamepadVibration);
				}
				else
				{
					if (!InputGamepadState.GamepadIsConnected)
					{
						InputGamepadState.GamepadIsConnected = false;
						CMemory::Memset(&InputGamepadState, 0, sizeof(InputGamepadState));
					}
				}
			}
		}
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_WINDOWS 
