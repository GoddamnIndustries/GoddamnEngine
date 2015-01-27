/// ==========================================================================================
/// WindowsWindow.h - Windows window class.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_PLATFORM_WINDOW_WINDOWS
#define GD_CORE_PLATFORM_WINDOW_WINDOWS

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Platform/Window.h>

#include <Windows.h>
#include <Ole2.h>
#include <oleidl.h>

GD_NAMESPACE_BEGIN

	class WindowsWindow : public IGenericWindow
	{
	private:
		HWND HWnd;
		HDC HDc;

	public:

		GDINL WindowsWindow()
		{
			UInt32 WindowStyle = 0;
			UInt32 WindowStyleEx = 0;
			if (!this->HasBorder) {
				WindowStyle |= WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
				WindowStyleEx |= WS_EX_WINDOWEDGE;
				if (this->SupportsTransparency) {
					WindowStyleEx |= WS_EX_LAYERED;
				}
				if (this->AppearsInTaskbar) {
					WindowStyleEx |= WS_EX_APPWINDOW;
				} else {
					WindowStyleEx |= WS_EX_TOOLWINDOW;
				}
				if (this->IsTopmostWindow) {
					WindowStyleEx |= WS_EX_TOPMOST;
				}
				if (!this->AcceptsInput) {
					WindowStyleEx |= WS_EX_TRANSPARENT;
				}
			} else {
				WindowStyle |= WS_POPUP | WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_BORDER | WS_CAPTION;
				WindowStyleEx |= WS_EX_APPWINDOW;
			}
		}

		GDINL virtual ~WindowsWindow()
		{
		}

		/// @brief Changes window position and size.
		/// @param Rect The new window rectangle.
		GDAPI virtual void ReshapeWindow(Rectangle const& Rect) override final
		{
		}

		/// @brief Returns the rectangle of the window.
		/// @param Rect Output for rectangle.
		/// @returns True on success, false otherwise.
		GDAPI virtual bool GetRectangle(Rectangle& Rect) const override final
		{
			return false;
		}

	};	// class IGenericWindow

GD_NAMESPACE_END

#endif	// ifndef GD_CORE_PLATFORM_WINDOW_WINDOWS
