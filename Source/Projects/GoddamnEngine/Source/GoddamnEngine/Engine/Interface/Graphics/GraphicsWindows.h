// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Engine/Interface/Graphics/Graphics/GraphicsWindows.h
 * File contains export interface for Windows Implementation of the graphics interface.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#if GD_PLATFORM_WINDOWS || GD_PLATFORM_WINDOWS_UAP || GD_PLATFORM_WINDOWS_PHONE

#include <GoddamnEngine/Engine/Interface/Graphics/Graphics.h>
#include <Windows.h>

GD_NAMESPACE_BEGIN

	GDINL static void ThrowIfFailed(HRESULT const _Hr)
	{
		GD_ASSERT(SUCCEEDED(_Hr), "WINAPI\\D3D11 function has failed.");
	}

	// ------------------------------------------------------------------------------------------
	//! Windows-specific code of the graphics device implementation.
	struct IGraphicsWindows : public IGraphics
	{
		friend uinterface IInputWindows;
		friend uinterface IGraphics2DDirect2D;

	protected:
#if GD_PLATFORM_WINDOWS
		struct HwndStateData final
		{
			RECT  DataRect;
			DWORD DataStyle;
			DWORD DataStyleEx;
			bool  DataWasMaximized;
		};	// struct HwndStateData
		
		CHAR static const s_HwndBackgroundClassName[];
		CHAR static const s_HwndMainClassName[];
		HINSTANCE         m_HwndHinstance  = HINSTANCE();      
		HWND			  m_HwndBackground = HWND();
		HWND			  m_HwndMain       = HWND();
		HwndStateData     m_HwndStateData;
#endif	// if GD_PLATFORM_WINDOWS

	protected:
		GDINT void _CheckNotInitialized() const;
		GDINT void _CheckInitialized() const;

	public:
		GDAPI virtual IResult OnRuntimeInitialize() override;
		GDAPI virtual IResult OnRuntimePostInitialize() override;
		GDAPI virtual IResult OnRuntimeDeinitialize() override;

		GDAPI virtual void OnRuntimePreUpdate() override {}
		GDAPI virtual void OnRuntimeUpdate() override;
		GDAPI virtual void OnRuntimePostUpdate() override {}

		GDAPI virtual IGraphicsCanvasMode Canvas_GetMode() const override;
		GDAPI virtual IResult Canvas_SetMode(IGraphicsCanvasMode const gfxCanvasMode, bool const ForcelyReapply = false) override;
		GDAPI virtual GeomResolution const* Canvas_GetSupportedResolutions() const override;
		GDAPI virtual GeomResolution const* Canvas_GetResolution() const override;
		GDAPI virtual IResult Canvas_SetResolution(GeomResolution const* const gfxResolution, bool const ForcelyReapply = false) override;
	};	// struct IGraphicsWindows

	typedef IGraphicsWindows IGraphicsPlatform;

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_WINDOWS || GD_PLATFORM_WINDOWS_UAP || GD_PLATFORM_WINDOWS_PHONE
