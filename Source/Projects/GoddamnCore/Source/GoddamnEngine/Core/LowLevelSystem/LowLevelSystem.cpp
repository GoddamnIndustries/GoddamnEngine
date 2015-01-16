#include <GoddamnEngine/Core/LowLevelSystem/LowLevelSystem.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WindowsX.h>
#include <XInput.h>

#define GD_LL_OSX_FIX	" Use OS X To Fix this problem."
#define GD_LL_CLASSNAME	"INCREDIBLE_CORE_WINAPI_WINDOWS_CLASS"

GD_NAMESPACE_BEGIN

	GD_SINGLETON_IMPLEMENTATION(LowLevelSystem);

	struct HInstanceDef		 : public HINSTANCE__ { };
	struct HWindowDef		 : public HWND__ { };
	struct HDeviceContextDef : public HDC__ { };

	/// ==========================================================================================
	GDINT LRESULT static LowLevelSystemWindowProcedure(HWND hWND, UINT message, 
		WPARAM wParameter, LPARAM lParameter)
	{
		LowLevelSystem const& lowLevelSystem = LowLevelSystem::GetInstance();
		switch (message)
		{
			/// ==========================================================================================
			// GD_LL_EVENT_ON_WINDOW_CLOSED
		case WM_CLOSE: 
			{ 
				lowLevelSystem.GetEventCallBack(GD_LL_EVENT_ON_WINDOW_CLOSED)(nullptr);
			} return false;

			/// ==========================================================================================
			// GD_LL_EVENT_ON_KEY_DOWN & GD_LL_EVENT_ON_KEY_UP
		case WM_KEYDOWN:
			{
				KeyCode const keyCode = (KeyCode)wParameter;
				lowLevelSystem.GetEventCallBack(GD_LL_EVENT_ON_KEY_DOWN)(&keyCode);
			} return false;
		case  WM_KEYUP:
			{
				KeyCode const keyCode = (KeyCode)wParameter;
				lowLevelSystem.GetEventCallBack(GD_LL_EVENT_ON_KEY_UP)(&keyCode);
			} return false;

			/// ==========================================================================================
			// GD_LL_EVENT_ON_MOUSEBUTTON_DOWN
		case WM_LBUTTONDOWN:
			{
				const MouseButton mouseButton = MouseButton::Left;
				lowLevelSystem.GetEventCallBack(GD_LL_EVENT_ON_MOUSEBUTTON_DOWN)(&mouseButton);
			} return false;
		case WM_RBUTTONDOWN:
			{
				const MouseButton mouseButton = MouseButton::Right;
				lowLevelSystem.GetEventCallBack(GD_LL_EVENT_ON_MOUSEBUTTON_DOWN)(&mouseButton);
			} return false;
		case WM_MBUTTONDOWN:
			{
				const MouseButton mouseButton = MouseButton::Middle;
				lowLevelSystem.GetEventCallBack(GD_LL_EVENT_ON_MOUSEBUTTON_DOWN)(&mouseButton);
			} return false;

			/// ==========================================================================================
			// GD_LL_EVENT_ON_MOUSEBUTTON_UP
		case WM_LBUTTONUP:
			{
				const MouseButton mouseButton = MouseButton::Left;
				lowLevelSystem.GetEventCallBack(GD_LL_EVENT_ON_MOUSEBUTTON_UP)(&mouseButton);
			} return false;
		case WM_RBUTTONUP:
			{
				const MouseButton mouseButton = MouseButton::Right;
				lowLevelSystem.GetEventCallBack(GD_LL_EVENT_ON_MOUSEBUTTON_UP)(&mouseButton);
			} return false;
		case WM_MBUTTONUP:
			{
				const MouseButton mouseButton = MouseButton::Middle;
				lowLevelSystem.GetEventCallBack(GD_LL_EVENT_ON_MOUSEBUTTON_UP)(&mouseButton);
			} return false;

			/// ==========================================================================================
			// GD_LL_EVENT_ON_MOUSEWHEEL_ROTATED
		case WM_MOUSEWHEEL:
			{
				const float rotation = (float)GET_WHEEL_DELTA_WPARAM(wParameter) / WHEEL_DELTA;
				lowLevelSystem.GetEventCallBack(GD_LL_EVENT_ON_MOUSEWHEEL_ROTATED)(&rotation);
			} return false;

			/// ==========================================================================================
			// GD_LL_EVENT_ON_MOUSEWHEEL_ROTATED
		case WM_MOUSEMOVE:
			{
				const Resolution mousePosition(GET_X_LPARAM(lParameter), GET_Y_LPARAM(lParameter));
				lowLevelSystem.GetEventCallBack(GD_LL_EVENT_ON_MOUSEWHEEL_ROTATED)(&mousePosition);
			} return false;
		}

		return (DefWindowProc(hWND, message, wParameter, lParameter));
	}

	/// ==========================================================================================
	LowLevelSystem::LowLevelSystem() :
		IsFullscreen(false), 
		WindowResolution((Min(LowLevelSystem::GetScreenWidth(), Dimension(GD_RESOLUTION_DEFAULT_WIDTH))),
						 (Min(LowLevelSystem::GetScreenHeight(), Dimension(GD_RESOLUTION_DEFAULT_HEIGHT)))),
		WindowPosition(((LowLevelSystem::GetScreenWidth() - this->WindowResolution.x) / 2),
					   ((LowLevelSystem::GetScreenHeight() - this->WindowResolution.y) / 2))
	{
		this->eventListeners.Resize(this->eventListeners.GetCapacity());
		for (size_t cnt = 0; cnt < this->eventListeners.GetCapacity(); cnt += 1)
		{
			this->eventListeners[cnt] = (&LowLevelSystem::WindowWrapperEventListenerDummpyProc);
		}

		/// ==========================================================================================
		GD_ASSERT(((this->hInstance = ((HInstance)GetModuleHandleA(nullptr))) != nullptr),
				  "Failed to obtain hInstance." GD_LL_OSX_FIX);

		/// ==========================================================================================
		WNDCLASSEXA wndClassEx		= { 0 };
		wndClassEx.cbSize			= sizeof(wndClassEx);
		wndClassEx.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wndClassEx.hInstance		= hInstance;
		wndClassEx.lpszClassName	= GD_LL_CLASSNAME;
		wndClassEx.hIcon			= LoadIconA(nullptr, IDI_APPLICATION);
		wndClassEx.hCursor			= LoadCursorA(nullptr, IDC_ARROW);
		wndClassEx.lpfnWndProc		= ((WNDPROC)&LowLevelSystemWindowProcedure);
			
		GD_ASSERT(((RegisterClassExA(&wndClassEx) != 0) || (GetLastError() == ERROR_CLASS_ALREADY_EXISTS)), 
				  "'RegisterClassExA' failed." GD_LL_OSX_FIX);

		/// ==========================================================================================
		GD_ASSERT(((this->hWindow = ((HWindow)CreateWindowExA(WS_EX_APPWINDOW, GD_LL_CLASSNAME,
				  "GoddamnCore Window",
				  WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
				  0, 0, 1280, 720, nullptr, nullptr, hInstance, this))) != nullptr),
				  "'CreateWindowExA' failed." GD_LL_OSX_FIX);

		/// ==========================================================================================
		GD_ASSERT(((this->hDeviceContext = ((HDeviceContext)GetDC(this->hWindow))) != nullptr),
				  "'GetDC' failed." GD_LL_OSX_FIX);

		/// ==========================================================================================
		this->UpdateWindowParameters();
	}

	/// ==========================================================================================
	LowLevelSystem::~LowLevelSystem()
	{
		ChangeDisplaySettings(nullptr, CDS_RESET);
		ShowCursor(TRUE);

		ReleaseDC(this->hWindow, this->hDeviceContext);
		this->hDeviceContext = nullptr;

		DestroyWindow(this->hWindow);
		this->hWindow = nullptr;

		UnregisterClassA(GD_LL_CLASSNAME, this->hInstance);
	}

	Dimension LowLevelSystem::GetScreenWidth() { return ((Dimension)GetSystemMetrics(SM_CXSCREEN)); }
	Dimension LowLevelSystem::GetScreenHeight() { return ((Dimension)GetSystemMetrics(SM_CYSCREEN)); }

	/// ==========================================================================================
	void LowLevelSystem::UpdateWindowParameters() const
	{
		DWORD windowStyle = 0;
		DWORD windowStyleEx = 0;

		/// ==========================================================================================
		if (this->IsFullscreen)
		{
			windowStyle = WS_POPUP;
			windowStyleEx = (WS_EX_APPWINDOW | WS_EX_TOPMOST);

			DEVMODEA devMode	  = { 0 };
			devMode.dmSize		  = (sizeof(devMode));
			devMode.dmPelsWidth  = ((DWORD)this->WindowResolution.Width);
			devMode.dmPelsHeight = ((DWORD)this->WindowResolution.Height);
			devMode.dmBitsPerPel = (GetDeviceCaps(this->hDeviceContext, BITSPIXEL));
			devMode.dmFields	  = (DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL);

			GD_ASSERT((ChangeDisplaySettingsA(&devMode, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL),
					  "'ChangeDisplaySettingsA' failed." GD_LL_OSX_FIX);
		}
		else
		{
			windowStyle = (WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
			windowStyleEx = WS_EX_APPWINDOW;

			GD_ASSERT((ChangeDisplaySettingsA(nullptr, CDS_RESET) == DISP_CHANGE_SUCCESSFUL), 
					  "'ChangeDisplaySettingsA' failed." GD_LL_OSX_FIX);
		}

		/// ==========================================================================================
		GD_ASSERT(((SetWindowLongPtrA(this->hWindow, GWL_STYLE, static_cast<LONG_PTR>(windowStyle)) != 0)
				  && (SetWindowLongPtrA(this->hWindow, GWL_EXSTYLE, static_cast<LONG_PTR>(windowStyleEx)) != 0)),
				  "'SetWindowLongA' failed." GD_LL_OSX_FIX);

		/// ==========================================================================================
		RECT windowRectangle	 = { 0 };
		windowRectangle.left	 = ((this->IsFullscreen) ? 0l : this->WindowPosition.x);
		windowRectangle.top	 = ((this->IsFullscreen) ? 0l : this->WindowPosition.y);
		windowRectangle.right	 = (windowRectangle.left + this->WindowResolution.Width);
		windowRectangle.bottom = (windowRectangle.top + this->WindowResolution.Height);
		GD_ASSERT((AdjustWindowRectEx(&windowRectangle, windowStyle, FALSE, windowStyleEx)),
				  "'AdjustWindowRectEx' failed." GD_LL_OSX_FIX);
		GD_ASSERT((SetWindowPos(this->hWindow, HWND_TOP, windowRectangle.left, windowRectangle.top,
				  (windowRectangle.right - windowRectangle.left), 
				  (windowRectangle.bottom - windowRectangle.top), SWP_FRAMECHANGED)), 
				  "'SetWindowPos' failed." GD_LL_OSX_FIX);

		/// ==========================================================================================
		ShowWindow(this->hWindow, SW_SHOW);
		SetForegroundWindow(this->hWindow);
		SetFocus(this->hWindow);
	}

	/// ==========================================================================================
	void LowLevelSystem::UpdateWindow() const
	{
		for (MSG message = { 0 }; PeekMessageA(&message, this->hWindow, 0, 0, PM_REMOVE);)
		{
			TranslateMessage(&message);
			DispatchMessageA(&message);
		}
	}

GD_NAMESPACE_END
