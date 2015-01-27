#include <GoddamnEngine/Engine/Component/Static/Input/Input.h>
#include <GoddamnEngine/Core/LowLevelSystem/LowLevelSystem.h>
#include <GoddamnEngine/Core/Containers/StaticVector.h>

GD_NAMESPACE_BEGIN

	class Input_WIN32_WINAPI : public Input
	{
	private:
		GD_CLASSINFO_DEFINITION(Input_WIN32_WINAPI, Input, GDINT);

		StaticVector<KeyState, GD_INPUT_KEYS_SIZE> keyboardButtons;
		StaticVector<KeyState, GD_INPUT_MOUSE_SIZE> mouseButtons;
		Resolution mousePosition;
		float mouseDeltaRotation;

		StaticVector<KeyState*, 8> upBindings;

	public:

		/// ==========================================================================================
		// Devices testing
		/// ==========================================================================================

		GDINT virtual bool IsDeviceSupported(const Device device) const;

		/// ==========================================================================================
		// Keyboard management
		/// ==========================================================================================

		GDINL virtual bool IsKeyInState(const KeyCode keyCode, const KeyState keyState) const
		{
			return (this->keyboardButtons[(size_t)keyCode] == keyState);
		}

		/// ==========================================================================================
		// Mouse management
		/// ==========================================================================================

		GDINL virtual bool IsMouseButtonInState(const MouseButton mouseButton, const KeyState keyState) const
		{
			return (this->mouseButtons[(size_t)mouseButton] == keyState);
		}

		GDINL virtual const Resolution& GetMousePosition() const
		{
			return this->mousePosition;
		}

		GDINL virtual float GetMouseWheelDeltaRotation() const
		{
			return this->mouseDeltaRotation;
		}

		/// ==========================================================================================
		// Messaging functions
		/// ==========================================================================================

		GDINT virtual void OnInitializeSelf();
		GDAPI virtual void OnUpdateSelf();
	};

	bool Input_WIN32_WINAPI::IsDeviceSupported(const Device device) const
	{
		switch(device)
		{
		case Device::Keyboard: return true;
		case Device::Mouse: return true;
		}

		return false;
	}

	GD_CLASSINFO_IMPLEMENTATION(Input_WIN32_WINAPI, Input, GDINT);

	/// ==========================================================================================
	// Messaging functions
	/// ==========================================================================================

	void Input_WIN32_WINAPI::OnInitializeSelf()
	{
		this->keyboardButtons.Resize(this->keyboardButtons.GetCapacity());
		this->mouseButtons.Resize(this->mouseButtons.GetCapacity());

		LowLevelSystem& lowLevelSystem = LowLevelSystem::GetInstance();

		/// ==========================================================================================
		// Keyboard
		/// ==========================================================================================

		lowLevelSystem.SetEventCallBack(GD_LL_EVENT_ON_KEY_DOWN, [](chandle const argument)
		{
			const KeyState keyState = *(KeyState*)argument;

			Input_WIN32_WINAPI& input = (Input_WIN32_WINAPI&)Input::GetInstance();
			input.keyboardButtons[(size_t)keyState] = KeyState::Down;
		});

		lowLevelSystem.SetEventCallBack(GD_LL_EVENT_ON_KEY_UP, [](chandle const argument)
		{
			const KeyState keyState = *(KeyState*)argument;

			Input_WIN32_WINAPI& input = (Input_WIN32_WINAPI&)Input::GetInstance();
			input.upBindings.PushLast(&(input.keyboardButtons[(size_t)keyState] = KeyState::Up));
		});

		/// ==========================================================================================
		// Mouse 
		/// ==========================================================================================

		lowLevelSystem.SetEventCallBack(GD_LL_EVENT_ON_MOUSE_MOVED, [](chandle const argument)
		{
			Input_WIN32_WINAPI& input = (Input_WIN32_WINAPI&)Input::GetInstance();
			input.mousePosition = *(Resolution const*)argument;	
		});

		lowLevelSystem.SetEventCallBack(GD_LL_EVENT_ON_MOUSEWHEEL_ROTATED, [](chandle const argument)
		{
			Input_WIN32_WINAPI& input = (Input_WIN32_WINAPI&)Input::GetInstance();
			input.mouseDeltaRotation = *((float const*)argument);
		});

		lowLevelSystem.SetEventCallBack(GD_LL_EVENT_ON_MOUSEBUTTON_DOWN, [](chandle const argument)
		{
			KeyState const keyState = *(KeyState const*)argument;
			Input_WIN32_WINAPI& input = (Input_WIN32_WINAPI&)Input::GetInstance();
			input.mouseButtons[(size_t)keyState] = KeyState::Down;
		});

		lowLevelSystem.SetEventCallBack(GD_LL_EVENT_ON_MOUSEBUTTON_UP, [](chandle const argument)
		{
			KeyState const keyState = *(KeyState const*)argument;
			Input_WIN32_WINAPI& input = (Input_WIN32_WINAPI&)Input::GetInstance();
			input.upBindings.PushLast(&(input.mouseButtons[(size_t)keyState] = KeyState::Up));
		});
	}

	void Input_WIN32_WINAPI::OnUpdateSelf()
	{
		for (size_t cnt = 0; cnt < this->upBindings.GetLength(); cnt += 1)
		{
			*(this->upBindings[cnt]) = KeyState::None;
		}

		this->upBindings.Emptify();
	}

GD_NAMESPACE_END