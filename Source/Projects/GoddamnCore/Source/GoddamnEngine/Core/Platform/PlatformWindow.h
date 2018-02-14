// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file
 * Platform window.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/SharedPtr.h>

GD_NAMESPACE_BEGIN

	/*!
	 * Position of a window or a cursor inside a window.
	 */
	struct WindowPosition final
	{
	public:
		Int32 X;
		Int32 Y;
		GDINL explicit WindowPosition(Int32 const x = 0, Int32 const y = 0) 
			: X(x), Y(y)
		{
		}
	};	// struct WindowPosition

	/*!
	 * Size of a window.
	 */
	struct WindowSize final
	{
	public:
		UInt32 Width;
		UInt32 Height;
		GDINL explicit WindowSize(UInt32 const width = 0, UInt32 const height = 0)
			: Width(width), Height(height)
		{
		}
	};	// struct WindowSize
	
	enum class WindowType
	{
		Windowed,
		WindowedFullscreen,
		FullScreen,
	};	// enum class WindowType

	enum class WindowFlags
	{
		None = 0
	};	// enum class WindowFlags
	GD_ENUM_CLASS_DEFINE_FLAG_OPERATORS(WindowFlags)

	/*!
	 * Mouse button.
	 */
	enum class MouseButton
	{
		Left, 
		Middle, 
		Right, 
		Thumb1, 
		Thumb2
	};	// enum class MouseButton

	/*!
	 * Keyboard button.
	 */
	enum class KeyboardButton
	{
		Tab,
		Space,
		Backspace,
		CapsLock,
		Escape,
		Return,
		LeftShift, 
		RightShift, 
		LeftControl, 
		RightControl, 
		LeftAlt, 
		RightAlt,
		LeftWindows, 
		RightWindows, 
		LeftCommand, 
		RightCommand, 
		Pause, 
		PrintScreen,
		Insert,
		Delete,
		Home,
		End,
		PageUp,
		PageDown, 
		ArrowLeft, 
		ArrowUp, 
		ArrowRight, 
		ArrowDown, 
		Alpha0 = '0', 
		Alpha1 = '1', 
		Alpha2 = '2', 
		Alpha3 = '3', 
		Alpha4 = '4', 
		Alpha5 = '5', 
		Alpha6 = '6', 
		Alpha7 = '7', 
		Alpha8 = '8', 
		Alpha9 = '9',
		A = 'A', 
		B = 'B', 
		C = 'C', 
		D = 'D', 
		E = 'E', 
		F = 'F', 
		G = 'G', 
		H = 'H', 
		I = 'I', 
		J = 'J', 
		K = 'K', 
		L = 'L', 
		M = 'M', 
		N = 'N', 
		O = 'O', 
		P = 'P', 
		Q = 'Q', 
		R = 'R', 
		S = 'S', 
		T = 'T', 
		U = 'U', 
		V = 'V', 
		W = 'W', 
		X = 'X', 
		Y = 'Y', 
		Z = 'Z',
		Plus,		
		Minus,
		LeftBracket,	// '[{' for US
		RigthBracket,	// ']}' for US
		Colon,			// ';:' for US
		Quote,			// ''"' for US
		Backslash,		// '\|' for US
		Comma,			
		Period,
		Slash,			// '/?' for US
		Tilde,			// '`~' for US
		Numpad0, 
		Numpad1, 
		Numpad2, 
		Numpad3, 
		Numpad4,
		Numpad5, 
		Numpad6, 
		Numpad7, 
		Numpad8, 
		Numpad9,
		NumpadReturn,
		NumpadPlus,
		NumpadMinus,
		NumpadAsterisc,
		NumpadSlash,
		NumpadPeriod,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
	};	// enum class KeyboardButton

	/*!
	 * Window delegate.
	 */
	class IWindowDelegate : public IVirtuallyDestructible
	{
	public:

		// ------------------------------------------------------------------------------------------
		// Window events.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Callback being triggered by the owning window after user activates the window.
		 */
		GDINT virtual void OnWindowFocused()
		{
		}

		/*!
		 * Callback being triggered by the owning window after user deactivates the window.
		 */
		GDINT virtual void OnWindowLostFocus()
		{
		}

		/*!
		 * Callback being triggered by the owning window after user minimizes the window.
		 */
		GDINT virtual void OnWindowMinimized()
		{
		}

		/*!
		 * Callback being triggered by the owning window after user maximizes the window.
		 * @param windowSize New window size.
		 */
		GDINT virtual void OnWindowMaximized(WindowSize const& windowSize)
		{
			GD_NOT_USED_L(windowSize);
		}

		/*!
		 * Callback being triggered by the owning window after user moves the window.
		 * @param windowPosition Position of the window client rect relative to the upper-left monitor corner.
		 */
		GDINT virtual void OnWindowMoved(WindowPosition const& windowPosition)
		{
			GD_NOT_USED_L(windowPosition);
		}

		/*!
		 * Callback being triggered by the owning window after user restores it from being minimized.
		 */
		GDINT virtual void OnWindowRestored()
		{
		}

		/*!
		 * Callback being triggered by the owning window after user resizes the window.
		 * @param windowSize New window size.
		 */
		GDINT virtual void OnWindowResized(WindowSize const& windowSize)
		{
			GD_NOT_USED_L(windowSize);
		}
		
		/*!
		 * Callback being triggered by the owning window after user closes the window.
		 */
		GDINT virtual void OnWindowClosed()
		{
		}

		// ------------------------------------------------------------------------------------------
		// Mouse events.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Callback being triggered by the owning window after user presses the mouse button.
		 * @param mouseButton Mouse button being pressed.
		 */
		GDINT virtual void OnMouseButtonDown(MouseButton const mouseButton)
		{
			GD_NOT_USED_L(mouseButton);
		}
		
		/*!
		 * Callback being triggered by the owning window after user double-clicks the mouse button.
		 * @param mouseButton Mouse button being double-clicked.
		 */
		GDINT virtual void OnMouseButtonDoubleClick(MouseButton const mouseButton)
		{
			GD_NOT_USED_L(mouseButton);
		}
		
		/*!
		 * Callback being triggered by the owning window after user releases the mouse button.
		 * @param mouseButton Mouse button being released.
		 */
		GDINT virtual void OnMouseButtonUp(MouseButton const mouseButton)
		{
			GD_NOT_USED_L(mouseButton);
		}

		/*!
		 * Callback being triggered by the owning window after user rotates the mouse wheel.
		 * @param mouseWheelDelta Mouse wheel delta rotation.
		 */
		GDINT virtual void OnMouseWheel(Float32 const mouseWheelDelta)
		{
			GD_NOT_USED_L(mouseWheelDelta);
		}

		/*!
		 * Callback being triggered by the owning window after user moves the mouse.
		 * @param mouseRelativePosition Position of the cursor relative to the upper-left window corner.
		 */
		GDINT virtual void OnMouseMovement(WindowPosition const& mouseRelativePosition)
		{
			GD_NOT_USED_L(mouseRelativePosition);
		}

		// ------------------------------------------------------------------------------------------
		// Keyboard events.
		// ------------------------------------------------------------------------------------------
		
		/*!
		 * Callback being triggered by the owning window after user presses the keyboard button.
		 * @param keyboardButton Keyboard button being pressed.
		 */
		GDINT virtual void OnKeyboardButtonDown(KeyboardButton const keyboardButton)
		{
			GD_NOT_USED_L(keyboardButton);
		}

		/*!
		 * Callback being triggered by the owning window after user releases the keyboard button.
		 * @param keyboardButton Keyboard button being released.
		 */
		GDINT virtual void OnKeyboardButtonUp(KeyboardButton const keyboardButton)
		{
			GD_NOT_USED_L(keyboardButton);
		}
	};	// class IWindowDelegate

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Window.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class IWindow : public ReferenceTarget
	{
	public:

		/*!
		 * Sets size of the window client area.
		 * 
		 * @param windowSize New window size.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool SetWindowSize(WindowSize const& windowSize) GD_PURE_VIRTUAL;

		/*!
		 * Sets type of the window.
		 * 
		 * @param windowType New window type.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool SetWindowType(WindowType const windowType) GD_PURE_VIRTUAL;

		/*!
		 * Sets flags of the window.
		 * 
		 * @param windowFlags New window flags.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool SetWindowFlags(WindowFlags const windowFlags) GD_PURE_VIRTUAL;

	public:
		GDINT virtual void Update() const
		{
		}
	};	// class IWindow

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! System window.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL IPlatformWindow : public IWindow
	{	
	public:

		/*!
		 * Returns system handle of this window.
		 */
		GDINT virtual CHandle GetSystemHandle() const GD_PURE_VIRTUAL;

	};	// class IPlatformSystemWindow

GD_NAMESPACE_END
