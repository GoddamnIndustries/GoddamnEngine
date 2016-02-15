// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/System/Interaction/WindowWindows.h
 * Windows window.
 */
#pragma once
#if !defined(GD_INSIDE_WINDOW_H)
#	error This file should be never directly included, please consider using < GoddamnEngine/Core/System/Interaction/Window.h> instead.
#endif	// if !defined(GD_INSIDE_WINDOW_H)

#include <Windows.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Windows window class. 
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class WindowWindows : public WindowGeneric
	{
	protected:
		struct HwndStateData final
		{
			RECT  DataRect;
			DWORD DataStyle;
			DWORD DataStyleEx;
		};	// struct HwndStateData

		HWND		  m_WindowBackgroundHandle;
		HWND		  m_WindowMainHandle;
		HwndStateData m_WindowMainStateData;
		HDC           m_WindowDeviceContext;
		HGLRC         m_WindowOpenGL;

	public:
		GDAPI WindowWindows(WindowMode const windowMode, WindowFlags const windowFlags, UInt32 const windowWidth, UInt32 const windowHeight);
		GDAPI virtual ~WindowWindows();

	public:
		GDAPI virtual void ShowWindow(bool const doShow = true) const override final;
		GDAPI virtual void SetWindowMode(WindowMode const windowMode, bool const doForceReapply = false) override final;
		GDAPI virtual void SetWindowDimensions(UInt32 const windowWidth, UInt32 const windowHeight, bool const doForceReapply = false) override final;
		GDAPI virtual void OnUpdate() override;
	};	// class WindowWindows

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Cross-platform window class. 
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	using Window = WindowWindows;

GD_NAMESPACE_END
