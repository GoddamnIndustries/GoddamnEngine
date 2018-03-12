#pragma once

#include "../UserInterface/ControlBase.h"

#include <Windows.h>

namespace Engine
{
	namespace UI
	{
		class HandleWindowStation : public WindowStation
		{
		private:
			HWND _window;

		public:
			HandleWindowStation(HWND window);
			~HandleWindowStation() override;

			virtual void SetFocus(Window* window) override;
			virtual Window* GetFocus() override;
			virtual void SetCapture(Window* window) override;
			virtual Window* GetCapture() override;
			virtual void ReleaseCapture() override;
			virtual Point GetCursorPos() override;

			eint ProcessWindowEvents(UInt32 Msg, eint WParam, eint LParam);
		};
	}
}
