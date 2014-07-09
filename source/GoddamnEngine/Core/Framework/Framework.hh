/// ==========================================================================================
/// Framework.hh - Framework interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 10.06.2014 - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_FRAMEWORK
#define GD_CORE_FRAMEWORK

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/Math/Math.hh>
#include <GoddamnEngine/Core/Text/String/String.hh>

GD_NAMESPACE_BEGIN

	/// Pointer to function that listens this event.
	typedef void (*WindowEventListenerProc)(chandle const Param);

	/// List of events that can be emited by window.
	enum WindowEvent : size_t
	{
		GD_WINDOW_EVENT_ON_WINDOW_CLOSED,       /// User has closed window. 'Param' is nullptr 
		GD_WINDOW_EVENT_ON_KEY_DOWN,            /// User has pressed key on keyboard. 'Param' is pointer to KeyCode for corresponding key
		GD_WINDOW_EVENT_ON_KEY_UP,              /// User has released key on keyboard. 'Param' is pointer to KeyCode for corresponding key
		GD_WINDOW_EVENT_ON_MOUSEBUTTON_DOWN,    /// User has pressed button on mouse. 'Param' is pointer to MouseButton for corresponding mouse button
		GD_WINDOW_EVENT_ON_MOUSEBUTTON_UP,      /// User has released button on mouse. 'Param' is pointer to MouseButton for corresponding mouse button
		GD_WINDOW_EVENT_ON_MOUSEWHEEL_ROTATED,  /// User has rotated wheel on mouse. 'Param' is pointer to float for wheel rotation in percents
		GD_WINDOW_EVENT_ON_MOUSE_MOVED,         /// User has moved the mouse. 'Param' is pointer to Resolution for mouse position
#if 0
		/// @todo Add gamepad support here
		GD_WINDOW_EVENT_ON_GAMEPAD_BUTTON_DOWN, ///< User has pressed button on gamepad. 'Param' is pointer to GamepadButton for corresponding gamepad button
		GD_WINDOW_EVENT_ON_GAMEPAD_BUTTON_UP,   ///< User has released button on gamepad. 'Param' is pointer to GamepadButton for corresponding gamepad button
#endif	// if 0
		GD_WINDOW_EVENT_UNKNOWN,			    ///< Unknown event (Invertal usage only)
		GD_WINDOW_EVENT_COUNT = GD_WINDOW_EVENT_UNKNOWN

	};	// enum WindowEvent

	/// Native window / context wrapper class.
	class Window
	{
	private:
		GD_CLASS_UNASSIGNABLE(Window);
		GD_CLASS_UNCOPIABLE  (Window);
		struct WindowImpl;
		Window* const ParentWindow = nullptr;
		WindowImpl* const Impl = nullptr;
		WindowEventListenerProc EventListeners[GD_WINDOW_EVENT_COUNT];

	public:
		GDAPI static Str       const DefaultWindowTitle;
		GDAPI static Rectangle const DefaultWindowRectangle;

	protected:
		GDAPI explicit Window();
		GDAPI virtual ~Window();

	public:
		GDAPI void WindowUpdate() const;

		/// Returns native handle of this window (e.g. HWND on WinAPI)
		GDAPI         handle    GetWindowNativeHandle() const;

		/// Returns rectangle of this window (position and size).
		GDAPI virtual Rectangle GetWindowRectangle() const;
		/// Changes position and size of this window.
		GDAPI virtual void      SetWindowRectangle(Rectangle const& NewRectangle);

		/// Returns true if window is running fullscreen.
		GDAPI virtual bool      GetWindowIsFullscreen() const;
		/// Changes fullscreen mode of this window.
		GDAPI virtual void      SetWindowIsFullscreen(bool const IsFullscreen);

		/// Returns title of this window.
		GDAPI virtual String    GetWindowTitle() const;
		/// Changes title of this window.
		GDAPI virtual void      SetWindowTitle(String const& NewTitle);
	};	// class Window

GD_NAMESPACE_END

#endif
