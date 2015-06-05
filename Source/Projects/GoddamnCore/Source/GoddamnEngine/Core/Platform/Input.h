// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Platform/Input.h
//! Cross-platform Input Interface definitions.
#pragma once
#define GD_INSIDE_INPUT_H

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Platform/Platform.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Math/Vector2.h>

GD_NAMESPACE_BEGIN

	//! @{
	struct KeyboardKeys final
	{
		typedef UInt8 Type;
		SizeTp static const KeysCount = 80;
	};	// struct KeyboardKeys
	typedef KeyboardKeys::Type KeyboardKeysTp;
	//! @}

	//! Describes mouse buttons.
	//! @{
	struct MouseButtons final
	{
		typedef UInt8 Type;
		Type static const ButtonLeft					= 0;
		Type static const ButtonMiddle					= 1;
		Type static const ButtonRight					= 2;
		Type static const ButtonsCount					= 3;
	};	// struct MouseButtons
	typedef MouseButtons::Type MouseButtonsTp;
	//! @}

	//! Describes gamepad buttons.
	//! @{
	struct GamepadButtons final
	{
		typedef UInt8 Type;
		
		Type static const ButtonA						= 0;
		Type static const ButtonB						= 2;
		Type static const ButtonX						= 1;
		Type static const ButtonY						= 2;
		Type static const ButtonCross					= ButtonA;
		Type static const ButtonCircle					= ButtonB;
		Type static const ButtonSquare					= ButtonX;
		Type static const ButtonTriangle				= ButtonY;

		Type static const ButtonDPadDown				= 3;
		Type static const ButtonDPadRight				= 4;
		Type static const ButtonDPadLeft				= 5;
		Type static const ButtonDPadUp					= 6;

		Type static const ButtonRB						= 7;
		Type static const ButtonLB						= 8;
		Type static const ButtonRT						= 9;
		Type static const ButtonLT						= 10;
		Type static const ButtonR1						= ButtonRB;
		Type static const ButtonL1						= ButtonLB;
		Type static const ButtonR2						= ButtonRT;
		Type static const ButtonL2						= ButtonLT;

		Type static const ButtonRightStick				= 11;
		Type static const ButtonRightStickDown			= 12;
		Type static const ButtonRightStickRight			= 13;
		Type static const ButtonRightStickLeft			= 14;
		Type static const ButtonRightStickUp			= 15;

		Type static const ButtonLeftStick				= 16;
		Type static const ButtonLeftStickDown			= 17;
		Type static const ButtonLeftStickRight			= 18;
		Type static const ButtonLeftStickLeft			= 19;
		Type static const ButtonLeftStickUp				= 20;

		Type static	const ButtonOptions					= 21;
		Type static	const ButtonMenu					= 22;
		Type static	const ButtonXboxOptions				= ButtonOptions;
		Type static	const ButtonXboxMenu				= ButtonMenu;
		Type static	const ButtonPS4Options				= ButtonMenu;
		Type static	const ButtonSelect					= ButtonOptions;
		Type static	const ButtonStart					= ButtonMenu;

	//	Type static const ButtonXbox					= 23;	// These buttons are unaddressable.
	//	Type static const ButtonPlayStation				= ButtonXbox;

	//	SizeTp static const ButtonsCount				= 24;
		SizeTp static const ButtonsCount				= 23;
	};	// struct GamepadButtons
	typedef GamepadButtons::Type GamepadButtonsTp;
	//! @}

	//! Describes a vibration feedback for a gamepad.
	struct GamepadVibration
	{
		Float32 VibrationRightSmall;
		Float32 VibrationRightLarge;
		Float32 VibrationLeftSmall;
		Float32 VibrationLeftLarge;
	};	// struct GamepadVibration

	//! 
	//! @{
	struct GestureEvents final
	{
		typedef UInt8 Type;
		Type static const GestureNone					= 0;
		Type static const GestureScroll					= 1;
		Type static const GestureScale					= 2;
		Type static const GestureSwipe					= 3;
		Type static const GestureRotate					= 4;
		Type static const GestureCount					= 5;
	};	// struct GestureEvents
	typedef GestureEvents::Type GestureEventsTp;
	//! @}

	//! Defines a generic input interface.
	class IGenericInput : public IPlatformObject<IGenericInput, PlatformObjectFlags::FlagsGenericInterfaceIsDummy>
	{
	public:

		//! Max amount of supported controllers.
		SizeTp static const MaxSupportedGamepads = 4;

	protected:

		//! @brief
		//! @{
		struct InputButtonState final
		{
			typedef UInt8 Type;
			Type static const ButtonNone					= 0;
			Type static const ButtonDown					= 1;
			Type static const ButtonHolding					= 2;
			Type static const ButtonUp						= 3;
		};	// struct InputButtonState
		typedef InputButtonState::Type InputButtonStateTp;
		//! @}

		//! @brief
		struct GamepadState
		{
			bool				GamepadIsConnected;
			Int32				GamepadID;
			InputButtonStateTp	GamepadPressedButtons[GamepadButtons::ButtonsCount];
			Float32x2			GamepadRightStickRaw;
			Float32x2			GamepadLeftStickRaw;
			Float32				GamepadRightTriggerValue;
			Float32				GamepadLeftTriggerValue;
			GamepadVibration	GamepadVibrationFeedback;
		};	// struct WindowsGamepadState

		GamepadState			InputGamepadStates[MaxSupportedGamepads];
		KeyboardKeysTp			InputKeyboardKeys [KeyboardKeys::KeysCount];

	public:

		//! Initializes an input interface.
		GDINL IGenericInput() { }

		//! Destroys an input interface.
		GDINL virtual ~IGenericInput() { }

	protected:

		//! Updates state of a button.
		//! @param InputButton Reference to the state of the button.
		//! @param InputIsPressed Is button currently pressed.
		GDINL static void UpdateButtonState(InputButtonStateTp& InputButton, bool const InputIsPressed)
		{
			switch (InputButton)
			{
				case InputButtonState::ButtonUp:
				case InputButtonState::ButtonNone:
					InputButton = InputIsPressed ? InputButtonState::ButtonDown : InputButtonState::ButtonNone;
					break;
				case InputButtonState::ButtonDown:
				case InputButtonState::ButtonHolding:
					InputButton = InputIsPressed ? InputButtonState::ButtonHolding : InputButtonState::ButtonUp;
					break;
			}
		}

		//! Initializes an input interface.
		GD_PLATFORM_DEFINE_METHOD0(void, InitializeObject, override { });

		//! Destroys an input interface.
		GD_PLATFORM_DEFINE_METHOD0(void, DestroyObject, override { });

		//! Ticks a frame.
		GD_PLATFORM_DEFINE_METHOD0(void, TickObject, override { });

	public:

#if 0

		//! Returns true if user holds keyboard key with specified key code.
		//! @param InputKeyboardKey Key code of a desired keyboard button.
		//! @returns True if user holds keyboard key with specified key code.
		GD_PLATFORM_DEFINE_METHOD1(bool, GetKeyboardKey, KeyboardKeyTp const, InputKeyboardKey, const { return false; });

		//! Returns true if user has pressed keyboard key with specified key code.
		//! @param InputKeyboardKey Key code of a desired keyboard button.
		//! @returns True if user has pressed keyboard key with specified key code.
		GD_PLATFORM_DEFINE_METHOD1(bool, GetKeyboardKeyDown, KeyboardKeyTp const, InputKeyboardKey, const { return false; });

		//! Returns true if user has released keyboard key with specified key code.
		//! @param InputKeyboardKey Key code of a desired keyboard button.
		//! @returns True if user has released keyboard key with specified key code.
		GD_PLATFORM_DEFINE_METHOD1(bool, GetKeyboardKeyUp, KeyboardKeyTp const, InputKeyboardKey, const{ return false; });

		//! Returns true if user holds mouse button with specified key code.
		//! @param InputMouseButton Key code of a desired mouse button.
		//! @returns True if user holds mouse button with specified key code.
		GD_PLATFORM_DEFINE_METHOD1(bool, GetMouseButton, MouseButtonsTp const, InputMouseButton, const { return false; });

		//! Returns true if user has pressed mouse button with specified key code.
		//! @param InputMouseButton Key code of a desired mouse button.
		//! @returns True if user has pressed mouse button with specified key code.
		GD_PLATFORM_DEFINE_METHOD1(bool, GetMouseButtonDown, MouseButtonsTp const, InputMouseButton, const { return false; });

		//! Returns true if user has released mouse button with specified key code.
		//! @param InputMouseButton Key code of a desired mouse button.
		//! @returns True if user has released mouse button with specified key code.
		GD_PLATFORM_DEFINE_METHOD1(bool, GetMouseButtonUp, MouseButtonsTp const, InputMouseButton, const { return false; });

		//! Returns true if user holds gamepad button with specified key code.
		//! @param InputGamepadIndex Index of a gamepad.
		//! @param InputGamepadButton Key code of a desired mouse button.
		//! @returns True if user holds gamepad button with specified key code.
		GD_PLATFORM_DEFINE_METHOD2(bool, GetGamepadButton, SizeTp const, InputGamepadIndex, GamepadButtonsTp const, InputGamepadButton, const { return false; });

		//! Returns true if user has pressed gamepad button with specified key code.
		//! @param InputGamepadIndex Index of a gamepad.
		//! @param InputGamepadButton Key code of a desired gamepad button.
		//! @returns True if user has pressed gamepad button with specified key code.
		GD_PLATFORM_DEFINE_METHOD2(bool, GetGamepadButtonDown, SizeTp const, InputGamepadIndex, GamepadButtonsTp const, InputGamepadButton, const { return false; });

		//! Returns true if user has released gamepad button with specified key code.
		//! @param InputGamepadIndex Index of a gamepad.
		//! @param InputGamepadButton Key code of a desired gamepad button.
		//! @returns True if user has released gamepad button with specified key code.
		GD_PLATFORM_DEFINE_METHOD2(bool, GetGamepadButtonUp, SizeTp const, InputGamepadIndex, GamepadButtonsTp const, InputGamepadButton, const { return false; });

#endif	// if 0

	};	// class IGenericInput 

GD_NAMESPACE_END

#include GD_PLATFORM_INCLUDE(GoddamnEngine/Core/Platform, Input.h)
#undef GD_INSIDE_INPUT_H
