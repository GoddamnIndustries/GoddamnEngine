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
#include <GoddamnEngine/Core/Templates/UniquePtr.h>

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
		GamepadState                 m_InputGamepadStates[4];
#endif	// if !GD_PLATFORM_WINDOWS_PHONE
#if GD_PLATFORM_WINDOWS
		IInputButtonState            m_InputKeyboardButtons[256] = {};
		IInputButtonState            m_InputMouseButtons[8] = {};

		CComPtr<IDirectInput8>       m_InputDirectInputInterface;
		CComPtr<IDirectInputDevice8> m_InputDeviceKeyboard;
		CComPtr<IDirectInputDevice8> m_InputDeviceMouse;
		UniquePtr<KeyboardState>     m_InputDeviceKeyboardCurrentState;
		UniquePtr<KeyboardState>     m_InputDeviceKeyboardPreviousState;
		UniquePtr<DIMOUSESTATE2>     m_InputDeviceMouseCurrentState;
		UniquePtr<DIMOUSESTATE2>     m_InputDeviceMousePreviousState;
		GeomPoint                    m_InputDeviceMousePosition;
#endif	// if GD_PLATFORM_WINDOWS

	public:
		GDAPI virtual IResult OnRuntimeInitialize() override;
		GDAPI virtual IResult OnRuntimeDeinitialize() override;
		GDAPI virtual void OnRuntimeUpdate() override;
		GDAPI virtual void OnRuntimePostUpdate() override;

	public:

		// Gamepad support..
		GDAPI virtual SizeTp GetGamepadMaxSupportedDevices() const override final;
		GDAPI virtual IInputButtonState GetGamepadButtonState(IInputGamepadButton const inputGamepadButton, SizeTp const inputGamepadIndex = 0) override final;
		GDAPI virtual Float32 GetGamepadRawAxisValue(IInputGamepadAxis const inputGamepadAxis, SizeTp const inputGamepadIndex = 0) override final;
		GDAPI virtual void GamepadVibrate(Float64 const inputGamepadDuration, Float32 const inputGamepadRightMotor, Float32 const inputGamepadLeftMotor, SizeTp const inputGamepadIndex = 0) override final;

		// Keyboard support..
		GDAPI virtual IInputButtonState GetKeyboardButtonState(IInputKeyboardButton const inputKeyboardButton) override final;
#if GD_PLATFORM_DESKTOP
#endif	// if GD_PLATFORM_DESKTOP

		// Mouse support..
		GDAPI virtual IInputButtonState GetMouseButtonState(IInputMouseButton const inputMouseButton) override final;
		GDAPI virtual GeomPoint GetMousePosition() override final;
		GDAPI virtual Float32 GetMouseWheelDelta() override final;
	};	// uinterface IInputWindows

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_WINDOWS || GD_PLATFORM_WINDOWS_UAP || GD_PLATFORM_WINDOWS_PHONE
