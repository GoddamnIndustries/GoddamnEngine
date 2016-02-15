// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Engine/Interface/Input/InputWindows.cpp
 * File contains Windows input subsystem implementation.
 */
#include <GoddamnEngine/Include.h>
#if (GD_PLATFORM_WINDOWS || GD_PLATFORM_WINDOWS_UAP || GD_PLATFORM_WINDOWS_PHONE) && 0

#include <GoddamnEngine/Engine/Interface/Input/InputWindows.h>
#include <GoddamnEngine/Engine/Interface/Graphics/GraphicsWindows.h>
//#include <GoddamnEngine/Engine/Interface/OutputDevice/OutputDevice.h>
#include <GoddamnEngine/Engine/Interface/DateTime/DateTime.h>
#define GD_DLOG_CAT "Input device (Windows)"

#if !GD_PLATFORM_WINDOWS_PHONE
#	pragma comment(lib, "xinput.lib")
#endif	// if !GD_PLATFORM_WINDOWS_PHONE
#if GD_PLATFORM_WINDOWS
#	pragma comment(lib, "dinput8.lib")
#	pragma comment(lib, "dxguid.lib")
#endif	// if GD_PLATFORM_WINDOWS

GD_NAMESPACE_BEGIN
	
	// ==========================================================================================
	// IInputWindows interface.
	// ==========================================================================================

	GD_IMPLEMENT_CLASS_INFORMATION(IInputWindows);
	GDEXP IInput* CreateIInputInstance()
	{
		return GD_NEW(IInputWindows);
	}

	GDINL static IInputButtonState PressedValuesToState(bool const IsPressed, bool const WasPressed)
	{
		return static_cast<IInputButtonState>(((IsPressed ? GD_BIT(1) : 0) | (WasPressed ? GD_BIT(0) : 0)) + 1);
	}

	GDINL static IInputButtonState PressedValuesBytesToState(BYTE const* const CurrentlyPressedValues, BYTE const* const PreviousPressedValues, SizeTp const Index)
	{
		return PressedValuesToState((CurrentlyPressedValues[Index] & 0x80) != 0, (PreviousPressedValues[Index] & 0x80) != 0);
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Runtime interface's functionality Implementation.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Function would be called on the global initialization step.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IInputWindows::OnRuntimeInitialize()
	{
#if GD_PLATFORM_WINDOWS		// We are using the DirectInput API to process the keyboard events on Windows.

		// We need our graphics device to initialize input for the window, into which we are rendering render.
		auto const* graphicsWindows = static_cast<IGraphicsWindows const*>(Graphics.Get());

		// We are using DirectInput to Handle keyboard and mouse events on the desktop Windows platform.
		ThrowIfFailed(DirectInput8Create(graphicsWindows->m_HwndHinstance, DIRECTINPUT_VERSION, IID_IDirectInput8
			, reinterpret_cast<LPVOID*>(&m_InputDirectInputInterface.p), nullptr));

		// Now we are trying to initialize our Keyboard device.
		if (SUCCEEDED(m_InputDirectInputInterface->CreateDevice(GUID_SysKeyboard, &m_InputDeviceKeyboard.p, nullptr)))
		{
			// Keyboard device was successfully initialized.
			ThrowIfFailed(m_InputDeviceKeyboard->SetDataFormat(&c_dfDIKeyboard));
			ThrowIfFailed(m_InputDeviceKeyboard->SetCooperativeLevel(graphicsWindows->m_HwndMain, DISCL_FOREGROUND | DISCL_EXCLUSIVE));
			m_InputDeviceKeyboard->Acquire();
			m_InputDeviceKeyboardCurrentState = GD_NEW(KeyboardState);
			m_InputDeviceKeyboardPreviousState = GD_NEW(KeyboardState);
		}
		else
		{
			// No Keyboard device was found. Really strange.
			//ConsoleDevice->LogWarning(GD_DLOG_CAT "Failed to create a DirectInput8 keyboard device.");
		}

		// Now we are trying to initialize our Mouse device.
		if (SUCCEEDED(m_InputDirectInputInterface->CreateDevice(GUID_SysMouse, &m_InputDeviceMouse.p, nullptr)))
		{
			// Mouse device was successfully initialized.
			ThrowIfFailed(m_InputDeviceMouse->SetDataFormat(&c_dfDIMouse2));
			ThrowIfFailed(m_InputDeviceMouse->SetCooperativeLevel(graphicsWindows->m_HwndMain, DISCL_FOREGROUND | DISCL_EXCLUSIVE));
			m_InputDeviceMouse->Acquire();
			m_InputDeviceMouseCurrentState = GD_NEW(DIMOUSESTATE2);
			m_InputDeviceMousePreviousState = GD_NEW(DIMOUSESTATE2);
			m_InputDeviceMousePosition = {};
		}
		else
		{
			// No Mouse device was found. Really strange.
			//ConsoleDevice->LogWarning(GD_DLOG_CAT "Failed to create a DirectInput8 Mouse device.");
		}

#else	// if GD_PLATFORM_WINDOWS	// ModernUI implementation for Windows Store.

#	error Not implemented.

#endif	// if GD_PLATFORM_WINDOWS

		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Function would be called on the global deinitialization step.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IInputWindows::OnRuntimeDeinitialize()
	{
#if GD_PLATFORM_WINDOWS	// We are using the DirectInput API to process the keyboard events on Windows.

		// Releasing the DirectInput's keyboard.
		if (m_InputDeviceKeyboard != nullptr)
		{
			m_InputDeviceKeyboard->Unacquire();
			m_InputDeviceKeyboardCurrentState = nullptr;
			m_InputDeviceKeyboardPreviousState = nullptr;
		}
		m_InputDeviceKeyboard.Release();

		// Releasing the DirectInput's mouse.
		if (m_InputDeviceMouse != nullptr)
		{
			m_InputDeviceMouse->Unacquire();
			m_InputDeviceMouseCurrentState = nullptr;
			m_InputDeviceMousePreviousState = nullptr;
		}
		m_InputDeviceMouse.Release();

		// Releasing the DirectInput interface.
		m_InputDirectInputInterface.Release();

#else	// if GD_PLATFORM_WINDOWS	// ModernUI implementation for Windows Store.

#	error Not implemented.

#endif	// if GD_PLATFORM_WINDOWS

		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Function would be called once per frame.
	GDAPI void IInputWindows::OnRuntimeUpdate()
	{
#if !GD_PLATFORM_WINDOWS_PHONE	// Currently gamepads are not supported on Windows Phone.

		// Reading the states of the connected gamepad devices.
		for (DWORD cnt = 0; cnt < GetGamepadMaxSupportedDevices(); ++cnt)
		{
			// Gamepad at current slot was connected, we can attempt to read it's state.
			auto& inputGamepadState = m_InputGamepadStates[cnt];
			if (FAILED(XInputGetState(cnt, &inputGamepadState.GamepadCurrentState)))
				inputGamepadState.GamepadIsDisconnected = true;
		}

#endif	// if !GD_PLATFORM_WINDOWS_PHONE

		// Reading the states of the connected keyboard device.
#if GD_PLATFORM_WINDOWS	 // We are using the DirectInput API to process the keyboard events on Windows.

		if (m_InputDeviceKeyboard != nullptr)
		{
			// Trying to read the state of the connected Keyboard.
			auto const inputDeviceKeyboardResult = m_InputDeviceKeyboard->GetDeviceState(sizeof(*m_InputDeviceKeyboardCurrentState), m_InputDeviceKeyboardCurrentState.Get());
			if (FAILED(inputDeviceKeyboardResult))
			{
				// Some problem with the keyboard device: maybe is just lost focus.
				if (inputDeviceKeyboardResult != DIERR_INPUTLOST && inputDeviceKeyboardResult != DIERR_NOTACQUIRED)
					ThrowIfFailed(inputDeviceKeyboardResult);
				m_InputDeviceKeyboard->Acquire();
			}
		}
#else	// if GD_PLATFORM_WINDOWS	// ModernUI implementation for Windows Store.

#	error Not implemented.

#endif	// if GD_PLATFORM_WINDOWS

		// Reading the states of the connected mouse device.
#if GD_PLATFORM_WINDOWS	 // We are using the DirectInput API to process the keyboard events on Windows.

		if (m_InputDeviceMouse != nullptr)
		{
			// Trying to read the state of the connected Mouse.
			auto const inputDeviceMouseResult = m_InputDeviceMouse->GetDeviceState(sizeof(*m_InputDeviceMouseCurrentState), m_InputDeviceMouseCurrentState.Get());
			if (FAILED(inputDeviceMouseResult))
			{
				// Some problem with the Mouse device: maybe is just lost focus.
				if (inputDeviceMouseResult != DIERR_INPUTLOST && inputDeviceMouseResult != DIERR_NOTACQUIRED)
					ThrowIfFailed(inputDeviceMouseResult);
				m_InputDeviceMouse->Acquire();
			}
			else
			{
				// This mouse is connected, we can process it, we need to modify global 
				// mouse position.
				m_InputDeviceMousePosition.X += m_InputDeviceMouseCurrentState->lX;
				m_InputDeviceMousePosition.Y += m_InputDeviceMouseCurrentState->lY;
			}
		}

#else	// if GD_PLATFORM_WINDOWS	// ModernUI implementation for Windows Store.

#	error Not implemented.

#endif	// if GD_PLATFORM_WINDOWS
	}

	// ------------------------------------------------------------------------------------------
	//! Function would be called once per frame, after all other runtime 
	//! interfaces are deinitialized.
	GDAPI void IInputWindows::OnRuntimePostUpdate()
	{
#if !GD_PLATFORM_WINDOWS_PHONE	// Currently gamepads are not supported on Windows Phone.

		for (DWORD cnt = 0; cnt < GetGamepadMaxSupportedDevices(); ++cnt)
		{
			// Swapping states of the connected gamepads.
			auto& inputGamepadState = m_InputGamepadStates[cnt];
			if (!inputGamepadState.GamepadIsDisconnected)
			{
				// Swapping states of the connected gamepads.
				inputGamepadState.GamepadPreviousState.Gamepad = inputGamepadState.GamepadCurrentState.Gamepad;
				inputGamepadState.GamepadVibrationTimeout -= DateTime->GetFrameDeltaTime();
				if (inputGamepadState.GamepadVibrationTimeout <= 0.0)
				{
					// It is timeout for our vibration event.
					XINPUT_VIBRATION inputGamepadZeroVibration = {};
					XInputSetState(cnt, &inputGamepadZeroVibration);
				}
			}
		}

#endif	// if !GD_PLATFORM_WINDOWS_PHONE

		// Swapping states of the connected keyboard and mouse.
#if GD_PLATFORM_WINDOWS	// DirectInput implementation for Windows.
		Swap(m_InputDeviceKeyboardCurrentState, m_InputDeviceKeyboardPreviousState);
		Swap(m_InputDeviceMouseCurrentState, m_InputDeviceMousePreviousState);
#else	// if GD_PLATFORM_WINDOWS	// ModernUI implementation for Windows Store.
#	error Not implemented.
#endif	// if GD_PLATFORM_WINDOWS
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Gamepad support.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	/*!
	 * Returns the maximum amount of the supported gamepads.
	 * @returns The maximum amount of the supported gamepads.
	 */
	GDAPI SizeTp IInputWindows::GetGamepadMaxSupportedDevices() const
	{
#if !GD_PLATFORM_WINDOWS_PHONE	// Currently gamepads are not supported on Windows Phone.

		// According to the link below XInput supports up to four connected gamepads.
		// https://msdn.microsoft.com/ru-ru/library/windows/desktop/ee417001(v=vs.85).aspx#multiple_controllers
		return 4;

#else	// if !GD_PLATFORM_WINDOWS_PHONE

		return 0;

#endif	// if !GD_PLATFORM_WINDOWS_PHONE
	}	

	/*!
	 * Returns the states of the specified gamepad button in the specified device m_Index.
	 * @param inputGamepadButton ID of the desired gamepad button.
	 * @param inputGamepadIndex Index of the gamepad. Zero by default.
	 * @returns State of the specified button.
	 */
	GDAPI IInputButtonState IInputWindows::GetGamepadButtonState(IInputGamepadButton const inputGamepadButton, SizeTp const inputGamepadIndex /*= 0*/)
	{
#if !GD_PLATFORM_WINDOWS_PHONE	

		WORD static const IInputGamepadButtonToXInputTable[IINPUT_BUTTONS_GAMEPAD_COUNT] = {
			/* IINPUT_BUTTON_GAMEPAD_XBOX_A            */ XINPUT_GAMEPAD_A,
			/* IINPUT_BUTTON_GAMEPAD_XBOX_B            */ XINPUT_GAMEPAD_B,
			/* IINPUT_BUTTON_GAMEPAD_XBOX_X            */ XINPUT_GAMEPAD_X,
			/* IINPUT_BUTTON_GAMEPAD_XBOX_Y            */ XINPUT_GAMEPAD_Y,
			/* IINPUT_BUTTON_GAMEPAD_DPAD_DOWN         */ XINPUT_GAMEPAD_DPAD_DOWN,
			/* IINPUT_BUTTON_GAMEPAD_DPAD_RIGHT        */ XINPUT_GAMEPAD_DPAD_RIGHT,
			/* IINPUT_BUTTON_GAMEPAD_DPAD_LEFT         */ XINPUT_GAMEPAD_DPAD_LEFT,
			/* IINPUT_BUTTON_GAMEPAD_DPAD_UP           */ XINPUT_GAMEPAD_DPAD_UP,
			/* IINPUT_BUTTON_GAMEPAD_XBOX_RB           */ XINPUT_GAMEPAD_RIGHT_SHOULDER,
			/* IINPUT_BUTTON_GAMEPAD_XBOX_LB           */ XINPUT_GAMEPAD_LEFT_SHOULDER,
			/* IINPUT_BUTTON_GAMEPAD_XBOX_RT           */ 0,
			/* IINPUT_BUTTON_GAMEPAD_XBOX_LT           */ 0,
			/* IINPUT_BUTTON_GAMEPAD_RIGHT_STICK       */ XINPUT_GAMEPAD_RIGHT_THUMB,
			/* IINPUT_BUTTON_GAMEPAD_RIGHT_STICK_DOWN  */ 0,
			/* IINPUT_BUTTON_GAMEPAD_RIGHT_STICK_RIGHT */ 0,
			/* IINPUT_BUTTON_GAMEPAD_RIGHT_STICK_LEFT  */ 0 ,
			/* IINPUT_BUTTON_GAMEPAD_RIGHT_STICK_UP    */ 0,
			/* IINPUT_BUTTON_GAMEPAD_LEFT_STICK        */ XINPUT_GAMEPAD_LEFT_THUMB,
			/* IINPUT_BUTTON_GAMEPAD_LEFT_STICK_DOWN   */ 0,
			/* IINPUT_BUTTON_GAMEPAD_LEFT_STICK_RIGHT  */ 0,
			/* IINPUT_BUTTON_GAMEPAD_LEFT_STICK_LEFT   */ 0,
			/* IINPUT_BUTTON_GAMEPAD_LEFT_STICK_UP     */ 0,
			/* IINPUT_BUTTON_GAMEPAD_XBOX_OPTIONS      */ XINPUT_GAMEPAD_START,
			/* IINPUT_BUTTON_GAMEPAD_XBOX_MENU         */ XINPUT_GAMEPAD_BACK,
		};

		auto const& inputGamepadState = m_InputGamepadStates[inputGamepadIndex];
		if (!inputGamepadState.GamepadIsDisconnected)
		{
			// This gamepad is connected, we can process it.
			auto const inputButtonMask = IInputGamepadButtonToXInputTable[inputGamepadIndex];
			if (inputButtonMask != 0)
			{
				// This is a regular button. Just applying the mask to the read states.
				return PressedValuesToState((inputGamepadState.GamepadCurrentState.Gamepad.wButtons & inputButtonMask) != 0
					, (inputGamepadState.GamepadPreviousState.Gamepad.wButtons & inputButtonMask) != 0);
			}

			// We need to process code of the gamepad button in special way.
			switch (inputGamepadIndex)
			{
				// Trigger button.
				case IINPUT_BUTTON_GAMEPAD_XBOX_RT:
					return PressedValuesToState(inputGamepadState.GamepadCurrentState.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD
						, inputGamepadState.GamepadPreviousState.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
				case IINPUT_BUTTON_GAMEPAD_XBOX_LT:
					return PressedValuesToState(inputGamepadState.GamepadCurrentState.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD
						, inputGamepadState.GamepadPreviousState.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);

					// Right stick.
				case IINPUT_BUTTON_GAMEPAD_RIGHT_STICK_DOWN:
					return PressedValuesToState(inputGamepadState.GamepadCurrentState.Gamepad.sThumbRY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE
						, inputGamepadState.GamepadPreviousState.Gamepad.sThumbRY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
				case IINPUT_BUTTON_GAMEPAD_RIGHT_STICK_RIGHT:
					return PressedValuesToState(inputGamepadState.GamepadCurrentState.Gamepad.sThumbRX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE
						, inputGamepadState.GamepadPreviousState.Gamepad.sThumbRX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
				case IINPUT_BUTTON_GAMEPAD_RIGHT_STICK_LEFT:
					return PressedValuesToState(inputGamepadState.GamepadCurrentState.Gamepad.sThumbRX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE
						, inputGamepadState.GamepadPreviousState.Gamepad.sThumbRX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
				case IINPUT_BUTTON_GAMEPAD_RIGHT_STICK_UP:
					return PressedValuesToState(inputGamepadState.GamepadCurrentState.Gamepad.sThumbRY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE
						, inputGamepadState.GamepadPreviousState.Gamepad.sThumbRY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);

				// Left stick.
				case IINPUT_BUTTON_GAMEPAD_LEFT_STICK_DOWN:
					return PressedValuesToState(inputGamepadState.GamepadCurrentState.Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE
						, inputGamepadState.GamepadPreviousState.Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
				case IINPUT_BUTTON_GAMEPAD_LEFT_STICK_LEFT:
					return PressedValuesToState(inputGamepadState.GamepadCurrentState.Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE
						, inputGamepadState.GamepadPreviousState.Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
				case IINPUT_BUTTON_GAMEPAD_LEFT_STICK_RIGHT:
					return PressedValuesToState(inputGamepadState.GamepadCurrentState.Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE
						, inputGamepadState.GamepadPreviousState.Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
				case IINPUT_BUTTON_GAMEPAD_LEFT_STICK_UP:
					return PressedValuesToState(inputGamepadState.GamepadCurrentState.Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE
						, inputGamepadState.GamepadPreviousState.Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);

				default:
					GD_NOT_IMPLEMENTED();
			}
		}

#endif	// if !GD_PLATFORM_WINDOWS_PHONE
		return IINPUT_BUTTONSTATE_UNKNOWN;
	}

	/*!
	 * Returns the raw value of the specified gamepad axis.
	 * @param inputGamepadAxis ID of the desired gamepad axis.
	 * @param inputGamepadIndex Index of the gamepad. Zero by default.
	 * @returns Floating value of the axis in the range [-1; +1].
	 */
	GDAPI Float32 IInputWindows::GetGamepadRawAxisValue(IInputGamepadAxis const inputGamepadAxis, SizeTp const inputGamepadIndex /*= 0*/)
	{
#if !GD_PLATFORM_WINDOWS_PHONE	

		auto const& inputGamepadState = m_InputGamepadStates[inputGamepadIndex];
		if (!inputGamepadState.GamepadIsDisconnected)
		{
			// This gamepad is connected, we can process it.
			switch (inputGamepadAxis)
			{
				// Trigger button.
				case IINPUT_AXIS_GAMEPAD_RIGHT_TRIGGER:
					return static_cast<Float32>(inputGamepadState.GamepadCurrentState.Gamepad.bRightTrigger) / static_cast<Float32>(MAXBYTE);
				case IINPUT_AXIS_GAMEPAD_LEFT_TRIGGER:
					return static_cast<Float32>(inputGamepadState.GamepadCurrentState.Gamepad.bLeftTrigger) / static_cast<Float32>(MAXBYTE);

					// Right stick.
				case IINPUT_AXIS_GAMEPAD_RIGHT_STICK_HORIZONTAL:
					return static_cast<Float32>(inputGamepadState.GamepadCurrentState.Gamepad.sThumbRX) / static_cast<Float32>(MAXSHORT);
				case IINPUT_AXIS_GAMEPAD_RIGHT_STICK_VERTICAL:
					return static_cast<Float32>(inputGamepadState.GamepadCurrentState.Gamepad.sThumbRY) / static_cast<Float32>(MAXSHORT);

					// Left stick.
				case IINPUT_AXIS_GAMEPAD_LEFT_STICK_HORIZONTAL:
					return static_cast<Float32>(inputGamepadState.GamepadCurrentState.Gamepad.sThumbLX) / static_cast<Float32>(MAXSHORT);
				case IINPUT_AXIS_GAMEPAD_LEFT_STICK_VERTICAL:
					return static_cast<Float32>(inputGamepadState.GamepadCurrentState.Gamepad.sThumbLY) / static_cast<Float32>(MAXSHORT);

				default:
					GD_NOT_IMPLEMENTED();
			}
		}

#endif	// if !GD_PLATFORM_WINDOWS_PHONE
		return 0.0f;
	}

	/*!
	 * Vibrates the specified gamepad (if the vibration is not supported, does nothing).
	 * @param inputGamepadDuration Duration of the vibration.
	 * @param inputGamepadRightMotor The vibration value in range [0; +1] for the m_Right motor.
	 * @param inputGamepadLeftMotor The vibration value in range [0; +1] for the m_Left motor.
	 * @param inputGamepadIndex Index of the gamepad. Zero by default.
	 */
	GDAPI void IInputWindows::GamepadVibrate(Float64 const inputGamepadDuration, Float32 const inputGamepadRightMotor, Float32 const inputGamepadLeftMotor, SizeTp const inputGamepadIndex /*= 0*/)
	{
#if !GD_PLATFORM_WINDOWS_PHONE	

		auto& inputGamepadState = m_InputGamepadStates[inputGamepadIndex];
		if (!inputGamepadState.GamepadIsDisconnected)
		{
			// This gamepad is connected, we can process it.
			XINPUT_VIBRATION inputGamepadVibration = {};
			inputGamepadVibration.wRightMotorSpeed = static_cast<WORD>((inputGamepadRightMotor * static_cast<Float32>(MAXWORD)));
			inputGamepadVibration.wLeftMotorSpeed = static_cast<WORD>((inputGamepadLeftMotor * static_cast<Float32>(MAXWORD)));
			if (XInputSetState(static_cast<DWORD>(inputGamepadIndex), &inputGamepadVibration) == ERROR_SUCCESS)
			{
				// Vibration was successfully set, now we need to set the timeout.
				inputGamepadState.GamepadVibrationTimeout = inputGamepadDuration;
			}
		}

#endif	// if !GD_PLATFORM_WINDOWS_PHONE
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Keyboard support.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	/*!
	 * Returns the states of the specified keyboard button in the specified device m_Index.
	 * @param inputKeyboardButton ID of the desired keyboard button.
	 * @returns State of the specified button.
	 */
	GDAPI IInputButtonState IInputWindows::GetKeyboardButtonState(IInputKeyboardButton const inputKeyboardButton)
	{
#if !GD_PLATFORM_WINDOWS_PHONE	
#	if GD_PLATFORM_WINDOWS	
		BYTE static const IInputGamepadButtonToXInputTable[IINPUT_BUTTONS_KEYBOARD_COUNT] = {
			/* IINPUT_BUTTON_KEYBOARD_F1           */ DIK_F1,	
			/* IINPUT_BUTTON_KEYBOARD_F2           */ DIK_F2,	
			/* IINPUT_BUTTON_KEYBOARD_F3           */ DIK_F3,
			/* IINPUT_BUTTON_KEYBOARD_F4           */ DIK_F4,
			/* IINPUT_BUTTON_KEYBOARD_F5           */ DIK_F5,
			/* IINPUT_BUTTON_KEYBOARD_F6           */ DIK_F6,
			/* IINPUT_BUTTON_KEYBOARD_F7           */ DIK_F7,
			/* IINPUT_BUTTON_KEYBOARD_F8           */ DIK_F8,
			/* IINPUT_BUTTON_KEYBOARD_F9           */ DIK_F9,
			/* IINPUT_BUTTON_KEYBOARD_F10          */ DIK_F10,
			/* IINPUT_BUTTON_KEYBOARD_F11          */ DIK_F11,
			/* IINPUT_BUTTON_KEYBOARD_F12          */ DIK_F12,
			/* IINPUT_BUTTON_KEYBOARD_F13          */ DIK_F13,
			/* IINPUT_BUTTON_KEYBOARD_F14          */ DIK_F14,
			/* IINPUT_BUTTON_KEYBOARD_F15          */ DIK_F15,
			/* IINPUT_BUTTON_KEYBOARD_1            */ DIK_1,	// Alpha-numeric numbers.
			/* IINPUT_BUTTON_KEYBOARD_2            */ DIK_2,
			/* IINPUT_BUTTON_KEYBOARD_3            */ DIK_3,
			/* IINPUT_BUTTON_KEYBOARD_4            */ DIK_4,
			/* IINPUT_BUTTON_KEYBOARD_5            */ DIK_5,
			/* IINPUT_BUTTON_KEYBOARD_6            */ DIK_6,
			/* IINPUT_BUTTON_KEYBOARD_7            */ DIK_7,
			/* IINPUT_BUTTON_KEYBOARD_8            */ DIK_8,
			/* IINPUT_BUTTON_KEYBOARD_9            */ DIK_9,
			/* IINPUT_BUTTON_KEYBOARD_0            */ DIK_0,
			/* IINPUT_BUTTON_KEYBOARD_Q            */ DIK_Q,	// Alpha-numeric letters.
			/* IINPUT_BUTTON_KEYBOARD_W            */ DIK_W,
			/* IINPUT_BUTTON_KEYBOARD_E            */ DIK_R,
			/* IINPUT_BUTTON_KEYBOARD_R            */ DIK_R,
			/* IINPUT_BUTTON_KEYBOARD_T            */ DIK_T,
			/* IINPUT_BUTTON_KEYBOARD_Y            */ DIK_Y,
			/* IINPUT_BUTTON_KEYBOARD_U            */ DIK_U,
			/* IINPUT_BUTTON_KEYBOARD_I            */ DIK_I,
			/* IINPUT_BUTTON_KEYBOARD_O            */ DIK_O,
			/* IINPUT_BUTTON_KEYBOARD_P            */ DIK_P,
			/* IINPUT_BUTTON_KEYBOARD_A            */ DIK_A,
			/* IINPUT_BUTTON_KEYBOARD_S            */ DIK_S,
			/* IINPUT_BUTTON_KEYBOARD_D            */ DIK_D,
			/* IINPUT_BUTTON_KEYBOARD_F            */ DIK_F,
			/* IINPUT_BUTTON_KEYBOARD_G            */ DIK_G,
			/* IINPUT_BUTTON_KEYBOARD_H            */ DIK_H,
			/* IINPUT_BUTTON_KEYBOARD_J            */ DIK_J,
			/* IINPUT_BUTTON_KEYBOARD_K            */ DIK_K,
			/* IINPUT_BUTTON_KEYBOARD_L            */ DIK_L,
			/* IINPUT_BUTTON_KEYBOARD_Z            */ DIK_Z,
			/* IINPUT_BUTTON_KEYBOARD_X            */ DIK_X,
			/* IINPUT_BUTTON_KEYBOARD_C            */ DIK_C,
			/* IINPUT_BUTTON_KEYBOARD_V            */ DIK_V,
			/* IINPUT_BUTTON_KEYBOARD_B            */ DIK_B,
			/* IINPUT_BUTTON_KEYBOARD_N            */ DIK_N,
			/* IINPUT_BUTTON_KEYBOARD_M            */ DIK_M,
			/* IINPUT_BUTTON_KEYBOARD_BACKQUOTE    */ DIK_GRAVE,	// Punctuation buttons.
			/* IINPUT_BUTTON_KEYBOARD_MINUS        */ DIK_MINUS,
			/* IINPUT_BUTTON_KEYBOARD_EQUALS       */ DIK_EQUALS,
			/* IINPUT_BUTTON_KEYBOARD_LBRACKET     */ DIK_LBRACKET,
			/* IINPUT_BUTTON_KEYBOARD_RBRACKET     */ DIK_RBRACKET,
			/* IINPUT_BUTTON_KEYBOARD_SEMICOLON    */ DIK_SEMICOLON,
			/* IINPUT_BUTTON_KEYBOARD_QUOTE        */ DIK_APOSTROPHE,
			/* IINPUT_BUTTON_KEYBOARD_BACKSLASH    */ DIK_BACKSLASH,
			/* IINPUT_BUTTON_KEYBOARD_COMMA        */ DIK_COMMA,
			/* IINPUT_BUTTON_KEYBOARD_DOT          */ DIK_PERIOD,
			/* IINPUT_BUTTON_KEYBOARD_SLASH        */ DIK_SLASH,
			/* IINPUT_BUTTON_KEYBOARD_ESCAPE       */ DIK_ESCAPE,	// Control keys.
			/* IINPUT_BUTTON_KEYBOARD_BREAK        */ DIK_PAUSE,
			/* IINPUT_BUTTON_KEYBOARD_BACKSPACE    */ DIK_BACKSPACE,
			/* IINPUT_BUTTON_KEYBOARD_INSERT       */ DIK_INSERT,
			/* IINPUT_BUTTON_KEYBOARD_HOME         */ DIK_HOME,
			/* IINPUT_BUTTON_KEYBOARD_PGUP         */ DIK_PGUP,
			/* IINPUT_BUTTON_KEYBOARD_ENTER        */ DIK_RETURN,
			/* IINPUT_BUTTON_KEYBOARD_DELETE       */ DIK_DELETE,
			/* IINPUT_BUTTON_KEYBOARD_END          */ DIK_END,
			/* IINPUT_BUTTON_KEYBOARD_PGDN         */ DIK_PGDN,
			/* IINPUT_BUTTON_KEYBOARD_SPACE        */ DIK_SPACE,
			/* IINPUT_BUTTON_KEYBOARD_TAB          */ DIK_TAB,
			/* IINPUT_BUTTON_KEYBOARD_NUM_LOCK     */ DIK_NUMLOCK,
			/* IINPUT_BUTTON_KEYBOARD_CAPS_LOCK    */ DIK_CAPSLOCK,
			/* IINPUT_BUTTON_KEYBOARD_SCROLL_LOCK  */ DIK_SCROLL,
			/* IINPUT_BUTTON_KEYBOARD_LCONTROL     */ DIK_LCONTROL,	// Modifier keys.
			/* IINPUT_BUTTON_KEYBOARD_RCONTROL     */ DIK_RCONTROL,
			/* IINPUT_BUTTON_KEYBOARD_LSHIFT       */ DIK_LSHIFT,
			/* IINPUT_BUTTON_KEYBOARD_RSHIFT       */ DIK_RSHIFT,
			/* IINPUT_BUTTON_KEYBOARD_LALT         */ DIK_LALT,
			/* IINPUT_BUTTON_KEYBOARD_RALT         */ DIK_RALT,
			/* IINPUT_BUTTON_KEYBOARD_LWIN         */ DIK_LWIN,
			/* IINPUT_BUTTON_KEYBOARD_RWIN         */ DIK_RWIN,
			/* IINPUT_BUTTON_KEYBOARD_ARROW_DOWN   */ DIK_DOWNARROW,	// Arrow keys
			/* IINPUT_BUTTON_KEYBOARD_ARROW_RIGHT  */ DIK_RIGHTARROW,
			/* IINPUT_BUTTON_KEYBOARD_ARROW_LEFT   */ DIK_LEFTARROW,
			/* IINPUT_BUTTON_KEYBOARD_ARROW_UP     */ DIK_UPARROW,
			/* IINPUT_BUTTON_KEYBOARD_NUMPAD_7     */ DIK_NUMPAD7,	// Numpad keys.
			/* IINPUT_BUTTON_KEYBOARD_NUMPAD_8     */ DIK_NUMPAD8,
			/* IINPUT_BUTTON_KEYBOARD_NUMPAD_9     */ DIK_NUMPAD9,	
			/* IINPUT_BUTTON_KEYBOARD_NUMPAD_4     */ DIK_NUMPAD4,
			/* IINPUT_BUTTON_KEYBOARD_NUMPAD_5     */ DIK_NUMPAD5,
			/* IINPUT_BUTTON_KEYBOARD_NUMPAD_6     */ DIK_NUMPAD6,
			/* IINPUT_BUTTON_KEYBOARD_NUMPAD_1     */ DIK_NUMPAD1,
			/* IINPUT_BUTTON_KEYBOARD_NUMPAD_2     */ DIK_NUMPAD2,
			/* IINPUT_BUTTON_KEYBOARD_NUMPAD_3     */ DIK_NUMPAD3,
			/* IINPUT_BUTTON_KEYBOARD_NUMPAD_0     */ DIK_NUMPAD0,
			/* IINPUT_BUTTON_KEYBOARD_NUMPAD_SLASH */ DIK_NUMPADSLASH,
			/* IINPUT_BUTTON_KEYBOARD_NUMPAD_STAR  */ DIK_NUMPADSTAR,
			/* IINPUT_BUTTON_KEYBOARD_NUMPAD_MINUS */ DIK_NUMPADMINUS,
			/* IINPUT_BUTTON_KEYBOARD_NUMPAD_PLUS  */ DIK_NUMPADPLUS,
			/* IINPUT_BUTTON_KEYBOARD_NUMPAD_COMMA */ DIK_NUMPADCOMMA,
			/* IINPUT_BUTTON_KEYBOARD_NUMPAD_ENTER */ DIK_NUMPADENTER,
		};
		if (m_InputDeviceKeyboard != nullptr)
		{
			// The keyboard is connected, we can process it.
			auto const inputButtonIndex = IInputGamepadButtonToXInputTable[inputKeyboardButton];
			return PressedValuesBytesToState(*m_InputDeviceKeyboardCurrentState, *m_InputDeviceKeyboardPreviousState, inputButtonIndex);
		}

#	else	// if GD_PLATFORM_WINDOWS	// ModernUI implementation for Windows Store.

#		error Not implemented.

#	endif	// if GD_PLATFORM_WINDOWS

#endif	// if !GD_PLATFORM_WINDOWS_PHONE
		return IINPUT_BUTTONSTATE_UNKNOWN;
	}

	//! @todo Add support for the text input methods.
#if GD_PLATFORM_DESKTOP

#endif	// if GD_PLATFORM_DESKTOP

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Mouse support.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Returns the states of the specified mouse button in the specified device m_Index.
	//! @param inputMouseButton ID of the desired mouse button.
	//! @returns State of the specified button.
	GDAPI IInputButtonState IInputWindows::Mouse_GetButtonState(IInputMouseButton const inputMouseButton)
	{
#if !GD_PLATFORM_WINDOWS_PHONE	// Mice are not supported on Windows Phone - we emulate them with touches.
#	if GD_PLATFORM_WINDOWS	// We are using the DirectInput API to process the mouse events on Windows.
		if (m_InputDeviceMouse != nullptr)
		{
			// This mouse is connected, we can process it.
			auto const inputButtonIndex = inputMouseButton;
			return PressedValuesBytesToState(m_InputDeviceMouseCurrentState->rgbButtons
				, m_InputDeviceMousePreviousState->rgbButtons, inputButtonIndex);
		}
#	else	// if GD_PLATFORM_WINDOWS	// ModernUI implementation for Windows Store.
#		error Not implemented.
#	endif	// if GD_PLATFORM_WINDOWS
#else	// if !GD_PLATFORM_WINDOWS_PHONE
#	error Insert touch API call here.
#endif	// if !GD_PLATFORM_WINDOWS_PHONE
		return IINPUT_BUTTONSTATE_UNKNOWN;
	}

	// ------------------------------------------------------------------------------------------
	//! Returns the position of the mouse cursor on the screen. Values range between 0 and screen side size.
	//! @returns The position of the mouse cursor on the screen.
	GDAPI GeomPoint IInputWindows::Mouse_GetPosition()
	{
#if !GD_PLATFORM_WINDOWS_PHONE	// Mice are not supported on Windows Phone - we emulate them with touches.
#	if GD_PLATFORM_WINDOWS	// We are using the DirectInput API to process the mouse events on Windows.
		if (m_InputDeviceMouse != nullptr)
		{
			// This mouse is connected, we can process it.
			return m_InputDeviceMousePosition;
		}
#	else	// if GD_PLATFORM_WINDOWS	// ModernUI implementation for Windows Store.
#		error Not implemented.
#	endif	// if GD_PLATFORM_WINDOWS
#else	// if !GD_PLATFORM_WINDOWS_PHONE
#	error Insert touch API call here.
#endif	// if !GD_PLATFORM_WINDOWS_PHONE
		return {};
	}

	// ------------------------------------------------------------------------------------------
	//! Returns value of the delta wheel rotation. Zero represents zero rotation, one - full circle.
	//! @returns Value of the delta wheel rotation.
	GDAPI Float32 IInputWindows::Mouse_GetWheelDelta()
	{
#if !GD_PLATFORM_WINDOWS_PHONE	// Mice are not supported on Windows Phone - we emulate them with touches.
#	if GD_PLATFORM_WINDOWS	// We are using the DirectInput API to process the mouse events on Windows.
		if (m_InputDeviceMouse != nullptr)
		{
			// This mouse is connected, we can process it.
			return static_cast<Float32>(m_InputDeviceMouseCurrentState->lZ);
		}
#	else	// if GD_PLATFORM_WINDOWS	// ModernUI implementation for Windows Store.
#		error Not implemented.
#	endif	// if GD_PLATFORM_WINDOWS
#else	// if !GD_PLATFORM_WINDOWS_PHONE
#	error Insert touch API call here.
#endif	// if !GD_PLATFORM_WINDOWS_PHONE
		return 0.0f;
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_WINDOWS || GD_PLATFORM_WINDOWS_UAP || GD_PLATFORM_WINDOWS_PHONE
