// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Engine/Interface/Input/InputWindows.h
 * File contains export interface for the Windows input subsystem implementation.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#if GD_PLATFORM_WINDOWS || GD_PLATFORM_WINDOWS_UAP || GD_PLATFORM_WINDOWS_PHONE

#include <GoddamnEngine/Engine/Interface/Input/Input.h>
#include <GoddamnEngine/Core/UniquePtr.h>

#include <Windows.h>
#include <atlbase.h>
#if !GD_PLATFORM_WINDOWS_PHONE
#	include <Xinput.h>
#endif	// if !GD_PLATFORM_WINDOWS_PHONE
#if GD_PLATFORM_WINDOWS
#	define DIRECTINPUT_VERSION 0x0800
#	include <dinput.h>
#endif	// if GD_PLATFORM_WINDOWS

GD_NAMESPACE_BEGIN

	// ------------------------------------------------------------------------------------------
	//! Windows input subsystem implementation.
	GD_DINTERFACE()
	uinterface IInputWindows final : public IInput
	{
		GD_DEFINE_CLASS_INFORMATION(IInputWindows, IInput);

		typedef IInput IInputBase;
#if !GD_PLATFORM_WINDOWS_PHONE
		struct GamepadState
		{
			bool         GamepadIsDisconnected = false;
			Float64      GamepadVibrationTimeout = 0.0f;
			XINPUT_STATE GamepadCurrentState = {};
			XINPUT_STATE GamepadPreviousState = {};
		};	// struct GamepadState
#endif	// if !GD_PLATFORM_WINDOWS_PHONE
#if GD_PLATFORM_WINDOWS
		typedef BYTE KeyboardState[256];
#endif	// if GD_PLATFORM_WINDOWS

	private:
#if !GD_PLATFORM_WINDOWS_PHONE
		GamepadState                 inputGamepadStates[4];
#endif	// if !GD_PLATFORM_WINDOWS_PHONE
#if GD_PLATFORM_WINDOWS
		IInputButtonState            inputKeyboardButtons[256] = {};
		IInputButtonState            inputMouseButtons[8] = {};

		CComPtr<IDirectInput8>       inputDirectInputInterface;
		CComPtr<IDirectInputDevice8> inputDeviceKeyboard;
		CComPtr<IDirectInputDevice8> inputDeviceMouse;
		UniquePtr<KeyboardState>     inputDeviceKeyboardCurrentState;
		UniquePtr<KeyboardState>     inputDeviceKeyboardPreviousState;
		UniquePtr<DIMOUSESTATE2>     inputDeviceMouseCurrentState;
		UniquePtr<DIMOUSESTATE2>     inputDeviceMousePreviousState;
		GeomPoint                    inputDeviceMousePosition;
#endif	// if GD_PLATFORM_WINDOWS

	public:
		GDAPI virtual IResult OnRuntimeInitialize() override;
		GDAPI virtual IResult OnRuntimeDeinitialize() override;
		GDAPI virtual void OnRuntimeUpdate() override;
		GDAPI virtual void OnRuntimePostUpdate() override;

	public:

		// Gamepad support..
		GDAPI virtual SizeTp Gamepad_GetMaxSupportedDevices() const override final;
		GDAPI virtual IInputButtonState Gamepad_GetButtonState(IInputGamepadButton const inputGamepadButton
			, SizeTp const inputGamepadIndex = 0) override final;
		GDAPI virtual Float32 Gamepad_GetRawAxisValue(IInputGamepadAxis const inputGamepadAxis
			, SizeTp const inputGamepadIndex = 0) override final;
		GDAPI virtual void Gamepad_Vibrate(Float64 const inputGamepadDuration
			, Float32 const inputGamepadRightMotor, Float32 const inputGamepadLeftMotor
			, SizeTp const inputGamepadIndex = 0) override final;

		// Keyboard support..
		GDAPI virtual IInputButtonState Keyboard_GetButtonState(IInputKeyboardButton const inputKeyboardButton) override final;
#if GD_PLATFORM_DESKTOP
#endif	// if GD_PLATFORM_DESKTOP

		// Mouse support..
		GDAPI virtual IInputButtonState Mouse_GetButtonState(IInputMouseButton const inputMouseButton) override final;
		GDAPI virtual GeomPoint Mouse_GetPosition() override final;
		GDAPI virtual Float32 Mouse_GetWheelDelta() override final;
	};	// uinterface IInputWindows

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_WINDOWS || GD_PLATFORM_WINDOWS_UAP || GD_PLATFORM_WINDOWS_PHONE
