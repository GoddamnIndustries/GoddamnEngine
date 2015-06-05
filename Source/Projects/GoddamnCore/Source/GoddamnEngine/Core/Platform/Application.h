// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Platform/Application.h
//! Cross-platform Application definitions.
#pragma once
#define GD_INSIDE_APPLICATION_H

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Platform/Platform.h>

#include <GoddamnEngine/Core/Platform/DateTime.h>
#include <GoddamnEngine/Core/Platform/OutputDevice.h>
#include <GoddamnEngine/Core/Platform/Input.h>
#include <GoddamnEngine/Core/Platform/SplashScreen.h>
#include <GoddamnEngine/Core/Platform/Window.h>

#include <GoddamnEngine/Core/Templates/SharedPtr.h>
#include <GoddamnEngine/Core/Containers/Map.h>

GD_NAMESPACE_BEGIN

#if 0

	struct DropEffectTp { };

	//! Class that handles events of the application.
	class IApplicationEventsHandler
	{
	protected:
		GDINL IApplicationEventsHandler() = default;
		GDINL virtual ~IApplicationEventsHandler() { }

	public:

		// ------------------------------------------------------------------------------------------	
		// Input events.
		// ------------------------------------------------------------------------------------------	

		//! Returns true if input events of the window are required to be handled.
		//! @param AppWindow Application window for which input events can be handled.
		//! @returns True if event was handled.
		GDAPI virtual bool ShouldHandleInputEvents(SharedPtr<Window> const& AppWindow) const { return false; }
		
		// ------------------------------------------------------------------------------------------	
		// Keyboard input events.
		// ------------------------------------------------------------------------------------------	

		//! Event triggered when 
		//! @returns True if event was handled.
		GDAPI virtual bool OnKeyboardButtonDown(Int32 const AppKeyCode, UInt32 const AppKeyCharacterCode, bool const AppKeyIsRepeated) { return false; }
		
		//! Event triggered when 
		//! @returns True if event was handled.
		GDAPI virtual bool OnKeyboardButtonDownChar(WideChar const AppKeyCharacter, bool const AppKeyIsRepeated) { return false; }
		
		//! Event triggered when 
		//! @returns True if event was handled.
		GDAPI virtual bool OnKeyboardButtonUp(Int32 const AppKeyCode, UInt32 const AppKeyCharacterCode, bool const AppKeyIsRepeated) { return false; }
		
		// ------------------------------------------------------------------------------------------	
		// Mouse input events.
		// ------------------------------------------------------------------------------------------	

		//! Event triggered when 
		//! @param AppWindow Application window, which triggered the event.
		//! @returns True if event was handled.
		GDAPI virtual bool OnMouseButtonDown(SharedPtr<Window> const& AppWindow, MouseButtonsTp const AppMouseButton) { return false; }
		
		//! Event triggered when 
		//! @returns True if event was handled.
		GDAPI virtual bool OnMouseButtonUp(MouseButtonsTp const AppMouseButton) { return false; }
		
		//! Event triggered when 
		//! @param AppWindow Application window, which triggered the event.
		//! @returns True if event was handled.
		GDAPI virtual bool OnMouseButtonDoubleClick(SharedPtr<Window> const& AppWindow, MouseButtonsTp const AppMouseButton) { return false; }
		
		//! Event triggered when 
		//! @returns True if event was handled.
		GDAPI virtual bool OnMouseWheelRotated(Float32 const AppMouseWheelDelta) { return false; }
		
		//! Event triggered when 
		//! @returns True if event was handled.
		GDAPI virtual bool OnMouseMoved() { return false; }
		
		//! Event triggered when 
		//! @returns True if event was handled.
		GDAPI virtual bool OnMouseMovedToRaw(Vector2 const& AppMouseRawPosition) { return false; }
		
		//! Event triggered when 
		//! @returns True if event was handled.
		GDAPI virtual bool OnMouseCursorSet() { return false; }

		// ------------------------------------------------------------------------------------------
		// Controller input events.
		// ------------------------------------------------------------------------------------------	

		//! Event triggered when 
		//! @returns True if event was handled.
		GDAPI virtual bool OnControllerAnalogMoved(ControllerButtonsTp const AppControllerButton, Int32 const AppControllerID, Float32 const AppAnalogValue) { return false; }
		
		//! Event triggered when 
		//! @returns True if event was handled.
		GDAPI virtual bool OnControllerButtonDown(ControllerButtonsTp const AppControllerButton, Int32 const AppControllerID, bool const AppKeyIsRepeated) { return false; }

		//! Event triggered when 
		//! @returns True if event was handled.
		GDAPI virtual bool OnControllerButtonUp(ControllerButtonsTp const AppControllerButton, Int32 const AppControllerID, bool const AppKeyIsRepeated) { return false; }

		// ------------------------------------------------------------------------------------------
		// Gesture input events.
		// ------------------------------------------------------------------------------------------

		//! Event triggered when gesture input stared.
		GDAPI virtual void OnGestureStared() {}
		
		//! Event triggered while inputing gesture event.
		//! @returns True if event was handled.
		GDAPI virtual bool OnGestureTouch(GestureEventsTp const AppGestureEvent, Vector2 const& AppDeltaCoord, Float32 const AppMouseWheelDelta) { return false; }
		
		//! Event triggered when gesture input ended.
		GDAPI virtual void OnGestureEnded() { }

		// ------------------------------------------------------------------------------------------
		// Touch input events.
		// ------------------------------------------------------------------------------------------

		//! Event triggered when user has touched the touch screen.
		//! @param AppTouchLocation Location of a touch on a screen.
		//! @param AppTouchIndex Index of a touch.
		//! @param AppControllerID ID of a controlled, that had detected the touch.
		//! @returns True if event was handled.
		GDAPI virtual bool OnTouchStarted(SharedPtr<Window> const& AppWindow, Vector2 const& AppTouchLocation, Int32 const AppTouchIndex, Int32 const AppControllerID) { return false; }
		
		//! Event triggered when user has moved finger across the touch screen.
		//! @param AppTouchLocation Location of a touch on a screen.
		//! @param AppControllerID ID of a controlled, that had detected the touch.
		//! @returns True if event was handled.
		GDAPI virtual bool OnTouchMoved(Vector2 const& AppTouchLocation, Int32 const AppTouchIndex, Int32 const AppControllerID) { return false; }
		
		//! Event triggered when user has released the touch screen.
		//! @param AppTouchLocation Location of a touch on a screen.
		//! @param AppTouchIndex Index of a touch.
		//! @param AppControllerID ID of a controlled, that had detected the touch.
		//! @returns True if event was handled.
		GDAPI virtual bool OnTouchEnded(Vector2 const& AppTouchLocation, Int32 const AppTouchIndex, Int32 const AppControllerID) { return false; }

		// ------------------------------------------------------------------------------------------
		// Accelerometer input events.
		// ------------------------------------------------------------------------------------------

		//! Event triggered when accelerometer has detected movement.
		//! @param AppMotionTilt Tilt coordinate of a window.
		//! @param AppMotionRotationRate Rotation rate of a motion.
		//! @param AppMotionGravity Gravity coordinates (relative to accelerometer).
		//! @param AppMotionAcceleration Acceleration of a motion.
		//! @param AppControllerID ID of a controlled, that had detected a motion.
		//! @returns True if event was handled.
		GDAPI virtual bool OnMotionDetected(Vector3 const& AppMotionTilt, Vector3 const& AppMotionRotationRate, Vector3 const& AppMotionGravity, Vector3 const& AppMotionAcceleration, Int32 const AppControllerID) { return false; }

		// ------------------------------------------------------------------------------------------	
		// Window events.
		// ------------------------------------------------------------------------------------------	

		//! Event triggered when 
		//! @param AppWindow Application window, which triggered the event.
		//! @returns True if event was handled.
		GDAPI virtual bool OnWindowSizeChanged(SharedPtr<Window> const& AppWindow, Vector2 const& AppWindowNewSize, bool const AppWasWindowMinimized = false) { return false; }

		//! Event triggered when 
		//! @param AppWindow Application window, which triggered the event.
		//! @returns True if event was handled.
		GDAPI virtual bool OnWindowMoved(SharedPtr<Window> const& AppWindow, Vector2 const& AppWindowNewPosition) { return false; }

		//! Event triggered when 
		//! @param AppWindow Application window, which triggered the event.
		//! @returns True if event was handled.
		GDAPI virtual bool OnWindowReshapingStarted(SharedPtr<Window> const& AppWindow) { return false; }

		//! Event triggered when 
		//! @param AppWindow Application window, which triggered the event.
		GDAPI virtual void OnWindowReshaping(SharedPtr<Window> const& AppWindow) {}

		//! Event triggered when 
		//! @param AppWindow Application window, which triggered the event.
		GDAPI virtual void OnWindowReshapingEnded(SharedPtr<Window> const& AppWindow) {}

		//! Event triggered when 
		//! @param AppWindow Application window, which triggered the event.
		//! @returns True if event was handled.
		GDAPI virtual bool OnWindowActivationChanged(SharedPtr<Window> const& AppWindow, WindowActivationTp const AppWindowActivation) { return false; }
		
		//! Event triggered when 
		//! @param AppWindow Application window, which triggered the event.
		//! @returns True if event was handled.
		GDAPI virtual bool OnWindowOSPaint(SharedPtr<Window> const& AppWindow) { return false; }
		
		//! Event triggered when 
		//! @param AppWindow Application window, which triggered the event.
		//! @returns True if event was handled.
		GDAPI virtual bool OnWindowClosed(SharedPtr<Window> const& AppWindow) { return false; }
		
		//! @param AppWindow Application window, which triggered the event.
		GDAPI virtual WindowZoneTp GetWindowZoneForPoint(SharedPtr<Window> const& AppWindow, Vector2 const& AppPoint) { return WindowZone::WindowZoneNotInWindow; }

		//! Event triggered when 
		//! @returns True if event was handled.
		GDAPI virtual bool OnApplicationActivationChanged(bool const AppIsActive) { return false; }

		// ------------------------------------------------------------------------------------------	
		// Drag-Drop events.
		// ------------------------------------------------------------------------------------------	

		//! Event triggered when user has dragged a text into the window.
		//! @param AppWindow Application window, which triggered the event.
		//! @param AppDraggedText Text that was dragged into the window.
		//! @returns Effect of a cursor while dragging.
		GDAPI virtual DropEffectTp OnDragEnteredText(SharedPtr<Window> const& AppWindow, WideString const& AppDraggedText) { return DropEffect::EffectNone; }
		
		//! Event triggered when user has dragged files into the window.
		//! @param AppWindow Application window, which triggered the event.
		//! @param AppDraggedFiles Files that were dragged into the window.
		//! @returns Effect of a cursor while dragging.
		GDAPI virtual DropEffectTp OnDragEnteredFiles(SharedPtr<Window> const& AppWindow, Vector<WideString> const& AppDraggedFiles) { return DropEffect::EffectNone; }
		
		//! Event triggered when user finished dragging into the window.
		//! @param AppWindow Application window, which triggered the event.
		//! @returns Effect of a cursor while dragging.
		GDAPI virtual DropEffectTp OnDragOver(SharedPtr<Window> const& AppWindow) { return DropEffect::EffectNone; }
		
		//! Event triggered when user has dropped something into the window.
		//! @param AppWindow Application window, which triggered the event.
		//! @returns Effect of a cursor while dragging.
		GDAPI virtual DropEffectTp OnDragDroped(SharedPtr<Window> const& AppWindow) { return DropEffect::EffectNone; }
		
		//! Event triggered when user's cursor (while dragging something) left the area of a window.
		//! @param AppWindow Application window, which triggered the event.
		GDAPI virtual void OnDragLeft(SharedPtr<Window> const& AppWindow) { }
	};	// class IApplicationEventsHandler
#endif	// if 0

	//! Cross-platform Application class.
	class IGenericApplication 
		: public IPlatformObject<IGenericApplication, PlatformObjectFlags::FlagsNone>
	{
	protected:
		bool							AppIsFirstInstance;
		Map<Handle, SharedPtr<Window>>	AppWindowsMap;
		SharedPtr<DateTime>				AppDateTime;
		SharedPtr<SplashScreen>			AppSplashScreen;

	protected:

		//! Initializes an application.
		GDINL IGenericApplication()
			: AppIsFirstInstance(false)
		{
		}

		//! Destroys an application.
		GDINL virtual ~IGenericApplication() { }

	protected:

		//! Initializes all stuff that should be initialized before main components.
		GD_PLATFORM_DEFINE_METHOD0(void, PreInitializeObject, {});

		//! Initializes all stuff that should be initialized after main components.
		GD_PLATFORM_DEFINE_METHOD0(void, PostInitializeObject, {});

		//! Initializes an application.
		GD_PLATFORM_DEFINE_METHOD0(void, InitializeObject, override {});

		//! Destroys an application.
		GD_PLATFORM_DEFINE_METHOD0(void, DestroyObject, override {});
		
	public:

	};	// class IGenericApplication

GD_NAMESPACE_END

#include GD_PLATFORM_INCLUDE(GoddamnEngine/Core/Platform, Application.h)
#undef GD_INSIDE_APPLICATION_H
