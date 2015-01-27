#ifndef GD_CORE_WINDOW_WRAPPER
#define GD_CORE_WINDOW_WRAPPER

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Math/Vector2.h>
#include <GoddamnEngine/Core/Containers/Vector.h>
#include <GoddamnEngine/Core/Containers/StaticVector.h>
#include <GoddamnEngine/Core/Object/Object.h>
#include <GoddamnEngine/Core/Templates/Singleton.h>
#include <GoddamnEngine/Core/Framework/Framework.h>

#define GD_RESOLUTION_DEFAULT_WIDTH 1280
#define GD_RESOLUTION_DEFAULT_HEIGHT 720

GD_NAMESPACE_BEGIN

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
	typedef StaticVector<LowLevelEventListenerProc, GD_LL_EVENT_COUNT> LowLevelEventListenersList;

	class LowLevelSystem final : public Object,
								 public Singleton<LowLevelSystem>
	{
		GD_CLASS_UNASSIGNABLE(LowLevelSystem);
		GD_CLASS_UNCOPIABLE(LowLevelSystem);

	private:
		GDINT static void WindowWrapperEventListenerDummpyProc(chandle const parameter) { GD_NOT_USED(parameter); }
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