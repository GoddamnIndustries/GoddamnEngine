#ifndef GD_INPUT
#define GD_INPUT

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/LowLevelSystem/LowLevelSystem.h>
#include <GoddamnEngine/Engine/Component/Static/StaticComponent.h>

#define GD_INPUT_KEYS_SIZE		((size_t)(KeyCode::Unknown))
#define GD_INPUT_MOUSE_SIZE		((size_t)(MouseButton::Unknown))

GD_NAMESPACE_BEGIN

	/// @brief			Enumeration describing state of touch on touch screen
	enum class TouchState : byte
	{
		None,		///< Not touched
		Pressed,	///< Screen is just touched and finger is holden steel
		Moved,		///< Finger was moved
		Released	///< User has released this touch
	};

	enum class Device
	{
		Keyboard,
		Mouse,
		Gamepad,
		Touchscreen,
		Unknown
	};

	/// @brief Input devices management
	class Input : public StaticComponent,
				  public Singleton<Input>
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(Input, StaticComponent, GDAPI);

	protected:
		GDINL Input() :
			GD_EXTENDS_SERIALIZABLE_LIST(StaticComponent, StaticComponentPriority::High)
		{
		}
	
		GDINL virtual ~Input()
		{
		}

	public:
		
		/// @name Devices testing
		/// @{

		/// @brief				Checks if this system supports device
		/// @param device		Type of device we are looking for
		/// @returns			Boolean for this device is supported
		GDAPI virtual bool IsDeviceSupported(const Device device) const abstract;

		/// @}

		/// @name Keyboard management
		/// @{

		/// @brief				Returns boolean for if this key is in required state
		/// @param keyCode		Key we are looking for
		/// @param keyState		Required key state
		/// @returns			Boolean for if this key is in required state
		GDAPI virtual bool IsKeyInState(const KeyCode keyCode, const KeyState keyState) const abstract;

		/// @brief				Returns boolean for if this key is down
		/// @param keyCode		Key we are looking for
		/// @returns			Boolean for if this key is down
		GDINL static bool IsKeyDown(const KeyCode keyCode)
		{
			return Input::GetInstance().IsKeyInState(keyCode, KeyState::Down);
		}

		/// @brief				Returns boolean for if this key is up
		/// @param keyCode		Key we are looking for
		/// @returns			Boolean for if this key is up
		GDAPI static bool IsKeyUp(const KeyCode keyCode)
		{
			return Input::GetInstance().IsKeyInState(keyCode, KeyState::Up);
		}

		/// @}

		/// @name Mouse management
		/// @{

	public:

		/// @brief				Returns coordinates of mouse on screen
		/// @returns			Coordinates of mouse on screen
		GDAPI virtual const Resolution& GetMousePosition() const abstract;

		/// @brief				Returns angle on which mouse was rotated
		/// @returns			Angle on which mouse was rotated
		GDAPI virtual float GetMouseWheelDeltaRotation() const abstract;

		/// @brief				Returns boolean for if this mouse button is in required state
		/// @param mouseButton	Mouse button we are looking for
		/// @param keyState		Required mouse button state
		/// @returns			Boolean for if this mouse button is in required state
		GDINL virtual bool IsMouseButtonInState(const MouseButton mouseButton, const KeyState keyState) const abstract;

		/// @brief				Returns boolean for if this key is down
		/// @param mouseButton	Mouse button we are looking for
		/// @returns			Boolean for if this mouse button is down
		GDINL bool IsMouseButtonDown(const MouseButton mouseButton) const
		{
			return Input::GetInstance().IsMouseButtonInState(mouseButton, KeyState::Down);
		}

		/// @brief				Returns boolean for if this key is up
		/// @param mouseButton	Mouse button we are looking for
		/// @returns			Boolean for if this mouse button is up
		GDAPI bool IsMouseButtonInState(const MouseButton mouseButton) const
		{
			return Input::GetInstance().IsMouseButtonInState(mouseButton, KeyState::Up);
		}

		/// @}
		
		/// @name Gamepad management
		/// @{



		/// @}

		GDAPI static bool GetTouchPressed();
		GDAPI static bool GetTouchMoved();
		GDAPI static bool GetTouchReleased();
		GDAPI static const Vector2& GetTouchPosition();
	};

GD_NAMESPACE_END

#endif