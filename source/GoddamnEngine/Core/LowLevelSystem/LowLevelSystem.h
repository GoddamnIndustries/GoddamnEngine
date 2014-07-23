#ifndef GD_CORE_WINDOW_WRAPPER
#define GD_CORE_WINDOW_WRAPPER

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Math/Vector2.h>
#include <GoddamnEngine/Core/Object/Object.h>
#include <GoddamnEngine/Core/Object/Singleton/Singleton.h>

#define GD_RESOLUTION_DEFAULT_WIDTH 1280
#define GD_RESOLUTION_DEFAULT_HEIGHT 720

GD_NAMESPACE_BEGIN

	/// ==========================================================================================
	/// ==========================================================================================
	/// Enumeration describing keyboard keys
	enum class KeyCode : UInt8
	{
		Backspace = 0x08,
		Tab = 0x09,
		Clear = 0x0C,
		Enter = 0x0D,
		Sht = 0x10,
		Control = 0x11,
		Alt = 0x12,
		Pause = 0x13,
		CapsLock = 0x14,
		Escape = 0x1B,
		Space = 0x20,
		PageUp = 0x21,
		PageDown = 0x22,
		End = 0x23,
		Home = 0x24,
		Left = 0x25,
		Up = 0x26,
		Right = 0x27,
		Down = 0x28,
		Select = 0x29,
		Print = 0x2A,
		Execute = 0x2B,
		PrintScreen = 0x2C,
		Insert = 0x2D,
		Delete = 0x2E,
		Help = 0x2F,
		Zero = 0x30,
		One = 0x31,
		Two = 0x32,
		Three = 0x33,
		Four = 0x34,
		Five = 0x35,
		Six = 0x36,
		Seven = 0x37,
		Eight = 0x38,
		Nine = 0x39,
		A = 0x41,
		B = 0x42,
		C = 0x43,
		D = 0x44,
		E = 0x45,
		F = 0x46,
		G = 0x47,
		H = 0x48,
		I = 0x49,
		J = 0x4A,
		K = 0x4B,
		L = 0x4C,
		M = 0x4D,
		N = 0x4E,
		O = 0x4F,
		P = 0x50,
		Q = 0x51,
		R = 0x52,
		S = 0x53,
		T = 0x54,
		U = 0x55,
		V = 0x56,
		W = 0x57,
		X = 0x58,
		Y = 0x59,
		Z = 0x5A,
		LeftWindowsKey = 0x5B,
		RightWindowsKey = 0x5C,
		ApplicationsKey = 0x5D,
		Sleep = 0x5F,
		NumPad0 = 0x60,
		NumPad1 = 0x61,
		NumPad2 = 0x62,
		NumPad3 = 0x63,
		NumPad4 = 0x64,
		NumPad5 = 0x65,
		NumPad6 = 0x66,
		NumPad7 = 0x67,
		NumPad8 = 0x68,
		NumPad9 = 0x69,
		Multiply = 0x6A,
		Add = 0x6B,
		Seperator = 0x6C,
		Subtract = 0x6D,
		Decimal = 0x6E,
		Divide = 0x6F,
		F1 = 0x70,
		F2 = 0x71,
		F3 = 0x72,
		F4 = 0x73,
		F5 = 0x74,
		F6 = 0x75,
		F7 = 0x76,
		F8 = 0x77,
		F9 = 0x78,
		F10 = 0x79,
		F11 = 0x7A,
		F12 = 0x7B,
		F13 = 0x7C,
		F14 = 0x7D,
		F15 = 0x7E,
		F16 = 0x7F,
		F17 = 0x80,
		F18 = 0x81,
		F19 = 0x82,
		F20 = 0x83,
		F21 = 0x84,
		F22 = 0x85,
		F23 = 0x86,
		F24 = 0x87,
		Numlock = 0x90,
		ScrollLock = 0x91,
		LeftSht = 0xA0,
		RightSht = 0xA1,
		LeftControl = 0xA2,
		RightContol = 0xA3,
		LeftMenu = 0xA4,
		RightMenu = 0xA5,
		BrowserBack = 0xA6,
		BrowserForward = 0xA7,
		BrowserRefresh = 0xA8,
		BrowserStop = 0xA9,
		BrowserSearch = 0xAA,
		BrowserFavorites = 0xAB,
		BrowseRHome = 0xAC,
		VolumeMute = 0xAD,
		VolumeDown = 0xAE,
		VolumeUp = 0xAF,
		NextTrack = 0xB0,
		PreviousTrack = 0xB1,
		StopMedia = 0xB2,
		PlayPause = 0xB3,
		LaunchMail = 0xB4,
		SelectMedia = 0xB5,
		LaunchApp1 = 0xB6,
		LaunchApp2 = 0xB7,
		Oem1 = 0xBA,
		OemPlus = 0xB8,
		OemComma = 0xBC,
		OemMinus = 0xBD,
		OemPeriod = 0xBE,
		Oem2 = 0xBF,
		Oem3 = 0xC0,
		Oem4 = 0xDB,
		Oem5 = 0xDC,
		Oem6 = 0xDD,
		Oem7 = 0xDE,
		Oem8 = 0xDF,
		Oem102 = 0xE2,
		Process = 0xE5,
		Packet = 0xE7,
		Attn = 0xF6,
		CrSel = 0xF7,
		ExSel = 0xF8,
		EraseEof = 0xF9,
		Play = 0xFA,
		Zoom = 0xFB,
		PA1 = 0xFD,
		OemClear = 0xFE,
		Unknown
	};

	/// ==========================================================================================
	/// Enumeration describing buttons on mouse 
	enum class MouseButton : UInt8
	{
		Left,
		Right,
		Middle,
		Unknown
	};

	/// ==========================================================================================
	/// ==========================================================================================
	/// Enumeration describing how button is been pressed
	enum class KeyState : UInt8
	{
		None,		///< Key is not pressed
		Down,		///< Key has been pressed and being holden
		Up			///< Key was just released
	};

	typedef struct HInstanceDef* HInstance;
	typedef struct HWindowDef* HWindow;
	typedef struct HDeviceContextDef* HDeviceContext;

	typedef UInt8 LowLevelEvent;
	enum LowLevelEventEnumeration : LowLevelEvent
	{
		/// User has closed window. 
		/// 'argument' is nullptr 
		GD_LL_EVENT_ON_WINDOW_CLOSED,

		/// User has pressed key on keyboard. 
		/// 'argument' is pointer to KeyCode for corresponding key
		GD_LL_EVENT_ON_KEY_DOWN,
		
		/// User has released key on keyboard. 
		/// 'argument' is pointer to KeyCode for corresponding key
		GD_LL_EVENT_ON_KEY_UP,
		
		/// User has pressed button on mouse. 
		/// 'argument' is pointer to MouseButton for corresponding mouse button
		GD_LL_EVENT_ON_MOUSEBUTTON_DOWN,
		
		/// User has released button on mouse. 
		/// 'argument' is pointer to MouseButton for corresponding mouse button
		GD_LL_EVENT_ON_MOUSEBUTTON_UP,

		/// User has rotated wheel on mouse. 
		/// 'argument' is pointer to float for wheel rotation in percents
		GD_LL_EVENT_ON_MOUSEWHEEL_ROTATED,

		/// User has moved the mouse. 
		/// 'argument' is pointer to Resolution for mouse position
		GD_LL_EVENT_ON_MOUSE_MOVED,

#if 0
		/// @todo Add gamepad support here
		/// User has pressed button on gamepad. 'argument' is pointer to GamepadButton 
		/// for corresponding gamepad button
		GD_LL_EVENT_ON_GAMEPAD_BUTTON_DOWN,

		/// User has released button on gamepad. 'argument' is pointer to GamepadButton 
		/// for corresponding gamepad button
		GD_LL_EVENT_ON_GAMEPAD_BUTTON_UP,
#endif

		GD_LL_EVENT_UNKNOWN,			///< Unknown event
		GD_LL_EVENT_COUNT = GD_LL_EVENT_UNKNOWN
	};

	typedef void (*LowLevelEventListenerProc)(chandle const parameter);
	typedef StackVector<LowLevelEventListenerProc, GD_LL_EVENT_COUNT> 
		LowLevelEventListenersList;

	class LowLevelSystem final : public HRIObject,
								 public Singleton<LowLevelSystem>
	{
		GD_CLASS_UNASSIGNABLE(LowLevelSystem);
		GD_CLASS_UNCOPIABLE(LowLevelSystem);

	private:
		GDINT static void WindowWrapperEventListenerDummpyProc(chandle const parameter) { GD_UNUSED(parameter); }
		LowLevelEventListenersList eventListeners;
	
	public:
		HInstance		hInstance;
		HWindow			hWindow;
		HDeviceContext	hDeviceContext;

		bool IsFullscreen;
		Resolution WindowResolution;
		Position WindowPosition;

		GDAPI static Dimension GetScreenWidth();
		GDAPI static Dimension GetScreenHeight();

		GDAPI LowLevelSystem();
		GDAPI virtual ~LowLevelSystem();

		GDINL LowLevelEventListenerProc GetEventCallBack(LowLevelEvent const event) const;
		GDINL void SetEventCallBack(LowLevelEvent const event, 
									LowLevelEventListenerProc const callback);

		GDAPI void UpdateWindowParameters() const;
		GDAPI void UpdateWindow() const;
	};

GD_NAMESPACE_END

#include <GoddamnEngine/Core/LowLevelSystem/LowLevelSystem.inl>

#endif