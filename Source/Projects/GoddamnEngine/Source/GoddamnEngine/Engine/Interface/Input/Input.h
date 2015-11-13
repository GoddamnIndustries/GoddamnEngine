// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Engine/Interface/Input/Input.h
 * File contains export interface for the input subsystem.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Geometry.h>
#include <GoddamnEngine/Core/Singleton.h>
#include <GoddamnEngine/Engine/Interface/UniqueInterface.h>

GD_NAMESPACE_BEGIN

	// ==========================================================================================
	// IInput interface.
	// ==========================================================================================

	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL_BASE(IInput, IRuntimeUniqueInterface);

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Common definitions.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Enumeration that contains all supported states for the input buttons.
	GD_DENUM()
	enum IInputButtonState : UInt8
	{
		IINPUT_BUTTONSTATE_UNKNOWN,
		IINPUT_BUTTONSTATE_RELEASED,
		IINPUT_BUTTONSTATE_UP,
		IINPUT_BUTTONSTATE_DOWN,
		IINPUT_BUTTONSTATE_PRESSED,
	};	// enum IInputButtonState

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Gamepad support.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Enumeration that contains all supported gamepad input buttons.
	GD_DENUM()
	enum IInputGamepadButton
	{
		IINPUT_BUTTON_GAMEPAD_XBOX_A,
		IINPUT_BUTTON_GAMEPAD_XBOX_B,
		IINPUT_BUTTON_GAMEPAD_XBOX_X,
		IINPUT_BUTTON_GAMEPAD_XBOX_Y,
		IINPUT_BUTTON_GAMEPAD_PLAYSTATION_CROSS = IINPUT_BUTTON_GAMEPAD_XBOX_A,
		IINPUT_BUTTON_GAMEPAD_PLAYSTATION_CIRCLE = IINPUT_BUTTON_GAMEPAD_XBOX_B,
		IINPUT_BUTTON_GAMEPAD_PLAYSTATION_SQUARE = IINPUT_BUTTON_GAMEPAD_XBOX_X,
		IINPUT_BUTTON_GAMEPAD_PLAYSTATION_TRIANGLE = IINPUT_BUTTON_GAMEPAD_XBOX_Y,
		IINPUT_BUTTON_GAMEPAD_DPAD_DOWN,
		IINPUT_BUTTON_GAMEPAD_DPAD_RIGHT,
		IINPUT_BUTTON_GAMEPAD_DPAD_LEFT,
		IINPUT_BUTTON_GAMEPAD_DPAD_UP,
		IINPUT_BUTTON_GAMEPAD_XBOX_RB,
		IINPUT_BUTTON_GAMEPAD_XBOX_LB,
		IINPUT_BUTTON_GAMEPAD_XBOX_RT,
		IINPUT_BUTTON_GAMEPAD_XBOX_LT,
		IINPUT_BUTTON_GAMEPAD_PLAYSTATION_R1 = IINPUT_BUTTON_GAMEPAD_XBOX_RB,
		IINPUT_BUTTON_GAMEPAD_PLAYSTATION_L1 = IINPUT_BUTTON_GAMEPAD_XBOX_LB,
		IINPUT_BUTTON_GAMEPAD_PLAYSTATION_R2 = IINPUT_BUTTON_GAMEPAD_XBOX_RT,
		IINPUT_BUTTON_GAMEPAD_PLAYSTATION_L2 = IINPUT_BUTTON_GAMEPAD_XBOX_LT,
		IINPUT_BUTTON_GAMEPAD_RIGHT_STICK,
		IINPUT_BUTTON_GAMEPAD_RIGHT_STICK_DOWN,
		IINPUT_BUTTON_GAMEPAD_RIGHT_STICK_RIGHT,
		IINPUT_BUTTON_GAMEPAD_RIGHT_STICK_LEFT,
		IINPUT_BUTTON_GAMEPAD_RIGHT_STICK_UP,
		IINPUT_BUTTON_GAMEPAD_LEFT_STICK,
		IINPUT_BUTTON_GAMEPAD_LEFT_STICK_DOWN,
		IINPUT_BUTTON_GAMEPAD_LEFT_STICK_RIGHT,
		IINPUT_BUTTON_GAMEPAD_LEFT_STICK_LEFT,
		IINPUT_BUTTON_GAMEPAD_LEFT_STICK_UP,
		IINPUT_BUTTON_GAMEPAD_XBOX_OPTIONS,
		IINPUT_BUTTON_GAMEPAD_XBOX_MENU,
		IINPUT_BUTTON_GAMEPAD_PLAYSTATION_OPTIONS = IINPUT_BUTTON_GAMEPAD_XBOX_MENU,
		IINPUT_BUTTON_GAMEPAD_PLAYSTATION_SELECT = IINPUT_BUTTON_GAMEPAD_XBOX_OPTIONS,
		IINPUT_BUTTON_GAMEPAD_PLAYSTATION_START = IINPUT_BUTTON_GAMEPAD_XBOX_MENU,
		IINPUT_BUTTON_GAMEPAD_UNKNOWN,
		IINPUT_BUTTONS_GAMEPAD_COUNT,
	};	// enum IInputGamepadButton

	// ------------------------------------------------------------------------------------------
	//! Enumeration that contains all supported gamepad input axes.
	GD_DENUM()
	enum IInputGamepadAxis : UInt8
	{
		IINPUT_AXIS_GAMEPAD_RIGHT_TRIGGER,
		IINPUT_AXIS_GAMEPAD_LEFT_TRIGGER,
		IINPUT_AXIS_GAMEPAD_RIGHT_STICK_HORIZONTAL,
		IINPUT_AXIS_GAMEPAD_RIGHT_STICK_VERTICAL,
		IINPUT_AXIS_GAMEPAD_LEFT_STICK_HORIZONTAL,
		IINPUT_AXIS_GAMEPAD_LEFT_STICK_VERTICAL,
	};	// enum IInputAxis : UInt8

	// ------------------------------------------------------------------------------------------
	//! Adds support for gamepads to the zero IInput interface.
	GD_DINTERFACE()
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IInputBase_WithGamepad, IInput)
	{
		// ------------------------------------------------------------------------------------------
		//! Returns the maximum amount of the supported gamepads.
		//! @returns The maximum amount of the supported gamepads.
		GD_DFUNCTION()
		GDAPI virtual SizeTp Gamepad_GetMaxSupportedDevices() const GD_PURE_VIRTUAL;

		// ------------------------------------------------------------------------------------------
		//! Returns the states of the specified gamepad button in the specified device m_Index.
		//! @param inputGamepadButton ID of the desired gamepad button.
		//! @param inputGamepadIndex m_Index of the gamepad. Zero by default.
		//! @returns State of the specified button.
		GD_DFUNCTION()
		GDAPI virtual IInputButtonState Gamepad_GetButtonState(IInputGamepadButton const inputGamepadButton
			, SizeTp const inputGamepadIndex = 0) GD_PURE_VIRTUAL;

		// ------------------------------------------------------------------------------------------
		//! Returns the raw value of the specified gamepad axis.
		//! @param inputGamepadAxis ID of the desired gamepad axis.
		//! @param inputGamepadIndex m_Index of the gamepad. Zero by default.
		//! @returns Floating value of the axis in the range [-1; +1].
		GD_DFUNCTION()
		GDAPI virtual Float32 Gamepad_GetRawAxisValue(IInputGamepadAxis const inputGamepadAxis
			, SizeTp const inputGamepadIndex = 0) GD_PURE_VIRTUAL;

		// ------------------------------------------------------------------------------------------
		//! Vibrates the specified gamepad (if the vibration is not supported, does nothing).
		//! @param inputGamepadDuration Duration of the vibration.
		//! @param inputGamepadRightMotor The vibration value in range [0; +1] for the m_Right motor.
		//! @param inputGamepadLeftMotor The vibration value in range [0; +1] for the m_Left motor.
		//! @param inputGamepadIndex m_Index of the gamepad. Zero by default.
		GD_DFUNCTION()
		GDAPI virtual void Gamepad_Vibrate(Float64 const inputGamepadDuration
			, Float32 const inputGamepadRightMotor, Float32 const inputGamepadLeftMotor
			, SizeTp const inputGamepadIndex = 0) GD_PURE_VIRTUAL;

		//! @todo Add more Dualshock4-specific features here.
#if GD_PLATFORM_PLAYSTATION4

		// ------------------------------------------------------------------------------------------
		//! @param inputGamepadIndex m_Index of the gamepad. Zero by default.
		GDAPI virtual void GamepadDualshock_SetLightBarColor(GeomColor const inputGamepadDualshockColor
			, SizeTp const inputGamepadIndex = 0) GD_PURE_VIRTUAL;

#endif	// if GD_PLATFORM_PLAYSTATION4
	};	// uinterface IInputBase_WithGamepad

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Keyboard support.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Enumeration that contains all supported keyboard input buttons.
	GD_DENUM()
	enum IInputKeyboardButton
	{
		IINPUT_BUTTON_KEYBOARD_F1,	// Functional keys.
		IINPUT_BUTTON_KEYBOARD_F2,	
		IINPUT_BUTTON_KEYBOARD_F3,
		IINPUT_BUTTON_KEYBOARD_F4,
		IINPUT_BUTTON_KEYBOARD_F5,
		IINPUT_BUTTON_KEYBOARD_F6,
		IINPUT_BUTTON_KEYBOARD_F7,
		IINPUT_BUTTON_KEYBOARD_F8,
		IINPUT_BUTTON_KEYBOARD_F9,
		IINPUT_BUTTON_KEYBOARD_F10,
		IINPUT_BUTTON_KEYBOARD_F11,
		IINPUT_BUTTON_KEYBOARD_F12,
		IINPUT_BUTTON_KEYBOARD_F13,
		IINPUT_BUTTON_KEYBOARD_F14,
		IINPUT_BUTTON_KEYBOARD_F15,
		IINPUT_BUTTON_KEYBOARD_1,	// Alpha-numeric numbers.
		IINPUT_BUTTON_KEYBOARD_2,
		IINPUT_BUTTON_KEYBOARD_3,
		IINPUT_BUTTON_KEYBOARD_4,
		IINPUT_BUTTON_KEYBOARD_5,
		IINPUT_BUTTON_KEYBOARD_6,
		IINPUT_BUTTON_KEYBOARD_7,
		IINPUT_BUTTON_KEYBOARD_8,
		IINPUT_BUTTON_KEYBOARD_9,
		IINPUT_BUTTON_KEYBOARD_0,
		IINPUT_BUTTON_KEYBOARD_Q,	// Alpha-numeric letters.
		IINPUT_BUTTON_KEYBOARD_W,
		IINPUT_BUTTON_KEYBOARD_E,
		IINPUT_BUTTON_KEYBOARD_R,
		IINPUT_BUTTON_KEYBOARD_T,
		IINPUT_BUTTON_KEYBOARD_Y,
		IINPUT_BUTTON_KEYBOARD_U,
		IINPUT_BUTTON_KEYBOARD_I,
		IINPUT_BUTTON_KEYBOARD_O,
		IINPUT_BUTTON_KEYBOARD_P,
		IINPUT_BUTTON_KEYBOARD_A,
		IINPUT_BUTTON_KEYBOARD_S,
		IINPUT_BUTTON_KEYBOARD_D,
		IINPUT_BUTTON_KEYBOARD_F,
		IINPUT_BUTTON_KEYBOARD_G,
		IINPUT_BUTTON_KEYBOARD_H,
		IINPUT_BUTTON_KEYBOARD_J,
		IINPUT_BUTTON_KEYBOARD_K,
		IINPUT_BUTTON_KEYBOARD_L,
		IINPUT_BUTTON_KEYBOARD_Z,
		IINPUT_BUTTON_KEYBOARD_X,
		IINPUT_BUTTON_KEYBOARD_C,
		IINPUT_BUTTON_KEYBOARD_V,
		IINPUT_BUTTON_KEYBOARD_B,
		IINPUT_BUTTON_KEYBOARD_N,
		IINPUT_BUTTON_KEYBOARD_M,
		IINPUT_BUTTON_KEYBOARD_BACKQUOTE,	// Punctuation buttons.
		IINPUT_BUTTON_KEYBOARD_MINUS,
		IINPUT_BUTTON_KEYBOARD_EQUALS,
		IINPUT_BUTTON_KEYBOARD_LBRACKET,
		IINPUT_BUTTON_KEYBOARD_RBRACKET,
		IINPUT_BUTTON_KEYBOARD_SEMICOLON,
		IINPUT_BUTTON_KEYBOARD_QUOTE,
		IINPUT_BUTTON_KEYBOARD_BACKSLASH,
		IINPUT_BUTTON_KEYBOARD_COMMA,
		IINPUT_BUTTON_KEYBOARD_DOT,
		IINPUT_BUTTON_KEYBOARD_SLASH,
		IINPUT_BUTTON_KEYBOARD_ESCAPE,	// Control keys.
		IINPUT_BUTTON_KEYBOARD_BREAK,
		IINPUT_BUTTON_KEYBOARD_BACKSPACE,
		IINPUT_BUTTON_KEYBOARD_INSERT,
		IINPUT_BUTTON_KEYBOARD_HOME,
		IINPUT_BUTTON_KEYBOARD_PGUP,
		IINPUT_BUTTON_KEYBOARD_ENTER,
		IINPUT_BUTTON_KEYBOARD_DELETE,
		IINPUT_BUTTON_KEYBOARD_END,
		IINPUT_BUTTON_KEYBOARD_PGDN,
		IINPUT_BUTTON_KEYBOARD_SPACE,
		IINPUT_BUTTON_KEYBOARD_TAB,
		IINPUT_BUTTON_KEYBOARD_NUM_LOCK,
		IINPUT_BUTTON_KEYBOARD_CAPS_LOCK,
		IINPUT_BUTTON_KEYBOARD_SCROLL_LOCK,
		IINPUT_BUTTON_KEYBOARD_LCONTROL,	// Modifier keys.
		IINPUT_BUTTON_KEYBOARD_RCONTROL,
		IINPUT_BUTTON_KEYBOARD_LSHIFT,
		IINPUT_BUTTON_KEYBOARD_RSHIFT,
		IINPUT_BUTTON_KEYBOARD_LALT,
		IINPUT_BUTTON_KEYBOARD_RALT,
		IINPUT_BUTTON_KEYBOARD_LWIN,
		IINPUT_BUTTON_KEYBOARD_RWIN,
		IINPUT_BUTTON_KEYBOARD_LCOMMAND = IINPUT_BUTTON_KEYBOARD_LWIN,
		IINPUT_BUTTON_KEYBOARD_RCOMMAND = IINPUT_BUTTON_KEYBOARD_RWIN,
		IINPUT_BUTTON_KEYBOARD_ARROW_DOWN,	// Arrow keys
		IINPUT_BUTTON_KEYBOARD_ARROW_RIGHT,
		IINPUT_BUTTON_KEYBOARD_ARROW_LEFT,
		IINPUT_BUTTON_KEYBOARD_ARROW_UP,
		IINPUT_BUTTON_KEYBOARD_NUMPAD_7, // Numpad keys.
		IINPUT_BUTTON_KEYBOARD_NUMPAD_8,
		IINPUT_BUTTON_KEYBOARD_NUMPAD_9,
		IINPUT_BUTTON_KEYBOARD_NUMPAD_4,
		IINPUT_BUTTON_KEYBOARD_NUMPAD_5,
		IINPUT_BUTTON_KEYBOARD_NUMPAD_6,
		IINPUT_BUTTON_KEYBOARD_NUMPAD_1,
		IINPUT_BUTTON_KEYBOARD_NUMPAD_2,
		IINPUT_BUTTON_KEYBOARD_NUMPAD_3,
		IINPUT_BUTTON_KEYBOARD_NUMPAD_0,
		IINPUT_BUTTON_KEYBOARD_NUMPAD_SLASH,
		IINPUT_BUTTON_KEYBOARD_NUMPAD_STAR,
		IINPUT_BUTTON_KEYBOARD_NUMPAD_MINUS,
		IINPUT_BUTTON_KEYBOARD_NUMPAD_PLUS,
		IINPUT_BUTTON_KEYBOARD_NUMPAD_COMMA,
		IINPUT_BUTTON_KEYBOARD_NUMPAD_ENTER,
		IINPUT_BUTTON_KEYBOARD_UNKNOWN,
		IINPUT_BUTTONS_KEYBOARD_COUNT,
	};	// IInputKeyboardButton

	// ------------------------------------------------------------------------------------------
	//! Adds support for keyboards to the zero IInput interface.
	GD_DINTERFACE()
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IInputBase_WithKeyboard, IInput)
	{
		// ------------------------------------------------------------------------------------------
		//! Returns the states of the specified keyboard button in the specified device m_Index.
		//! @param inputKeyboardButton ID of the desired keyboard button.
		//! @returns State of the specified button.
		GD_DFUNCTION()
		GDAPI virtual IInputButtonState Keyboard_GetButtonState(IInputKeyboardButton const inputKeyboardButton) GD_PURE_VIRTUAL;

		//! @todo Add support for the text input methods.
#if GD_PLATFORM_DESKTOP
#endif	// if GD_PLATFORM_DESKTOP
	};	// uinterface IInputBase_WithKeyboard

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Mouse support.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Enumeration that contains all supported mouse buttons.
	GD_DENUM()
	enum IInputMouseButton : UInt8
	{
		IINPUT_BUTTON_MOUSE_0,
		IINPUT_BUTTON_MOUSE_1,
		IINPUT_BUTTON_MOUSE_2,
		IINPUT_BUTTON_MOUSE_3,
		IINPUT_BUTTON_MOUSE_4,
		IINPUT_BUTTON_MOUSE_5,
		IINPUT_BUTTON_MOUSE_LEFT = IINPUT_BUTTON_MOUSE_0,
		IINPUT_BUTTON_MOUSE_RIGHT = IINPUT_BUTTON_MOUSE_1,
		IINPUT_BUTTON_MOUSE_MIDDLE = IINPUT_BUTTON_MOUSE_2,
		IINPUT_BUTTON_MOUSE_UNKNOWN,
		IINPUT_BUTTONS_MOUSE_COUNT,
	};	// enum IInputButton
	
	// ------------------------------------------------------------------------------------------
	//! Adds support for mice to the zero IInput interface.
	GD_DINTERFACE()
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IInputBase_WithMouse, IInput)
	{
		// ------------------------------------------------------------------------------------------
		//! Returns the states of the specified mouse button in the specified device m_Index.
		//! @param inputMouseButton ID of the desired mouse button.
		//! @returns State of the specified button.
		GD_DFUNCTION()
		GDAPI virtual IInputButtonState Mouse_GetButtonState(IInputMouseButton const inputMouseButton) GD_PURE_VIRTUAL;

		// ------------------------------------------------------------------------------------------
		//! Returns the position of the mouse cursor on the screen. Values range between 0 and screen side size.
		//! @returns The position of the mouse cursor on the screen.
		GD_DFUNCTION()
		GDAPI virtual GeomPoint Mouse_GetPosition() GD_PURE_VIRTUAL;

		// ------------------------------------------------------------------------------------------
		//! Returns value of the delta wheel rotation. Zero represents zero rotation, one - full circle.
		//! @returns Value of the delta wheel rotation.
		GD_DFUNCTION()
		GDAPI virtual Float32 Mouse_GetWheelDelta() GD_PURE_VIRTUAL;

	};	// uinterface IInputBase_WithMouse
	
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Touchscreen support.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Enumeration that contains phases of the touch.
	GD_DENUM()
	enum IInputTouchPhase : UInt8
	{
		IINPUT_TOUCH_PHASE_BEGAN,
		IINPUT_TOUCH_PHASE_MOVED,
		IINPUT_TOUCH_PHASE_STATIONARY,
		IINPUT_TOUCH_PHASE_ENDED,
		IINPUT_TOUCH_PHASE_CANCELED,
		IINPUT_TOUCH_PHASE_UNKNOWN,
		IINPUT_TOUCH_PHASES_COUNT,
	};	// enum IInputTouchPhase

	//! @todo Add Touchscreen support.
	// ------------------------------------------------------------------------------------------
	//! Adds support for touchscreens to the zero IInput interface.
	GD_DINTERFACE()
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IInputBase_WithTouchscren, IInput)
	{
	};	// uinterface IInputBase_WithTouchscren

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Accelerometer/Gyroscope support.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	//! @todo Add accelerometer/gyroscope support.
	// ------------------------------------------------------------------------------------------
	//! Adds support for touchscreens to the zero IInput interface.
	GD_DINTERFACE()
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IInputBase_WithAccelerometer, IInput)
	{
	};	// uinterface IInputBase_WithAccelerometer

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Global interface.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Generic interface for the input subsystems.
	GD_DINTERFACE()
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IInput, IInput)
#if !GD_D_REFLECTOR 	// DReflector should not parse singletons declarations.
		, public Singleton<IInput>
#endif	// if !GD_D_REFLECTOR
	{
		GD_DEFINE_CLASS_INFORMATION(IInput, IRuntimeUniqueInterface);
	};	// uinterface IInput

	// ------------------------------------------------------------------------------------------
	//! Global input interface pointer.
	GD_DSTATIC_VARIABLE()
	GDAPI extern IUniqueInterfacePointer<IInput> Input;

	// ==========================================================================================
	// IInputManager interface.
	// ==========================================================================================

	// ------------------------------------------------------------------------------------------
	//! Provides mapping for different devices to the virtual IDs.
	GD_DINTERFACE()
	uinterface IInputManager : public IInterface
#if !GD_D_REFLECTOR 	// DReflector should not parse singletons declarations.
		, public Singleton<IInputManager>
#endif	// if !GD_D_REFLECTOR
	{
		GD_DEFINE_CLASS_INFORMATION(IInputManager, IInterface);

	public:
		//! @todo Create input manager API and mapping for specific input devices.
	};	// uinterface IInputManager

	// ------------------------------------------------------------------------------------------
	//! Global input manager interface pointer.
	GD_DSTATIC_VARIABLE()
	GDAPI extern IUniqueInterfacePointer<IInputManager> inputManager;

GD_NAMESPACE_END
