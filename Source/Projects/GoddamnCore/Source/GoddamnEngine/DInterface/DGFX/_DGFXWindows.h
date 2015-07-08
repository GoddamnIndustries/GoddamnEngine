// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/DInterface/DGFX/_DGFXWindows.h
 * File contains export interface for Windows implementation of the graphics interface.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#if GD_PLATFORM_WINDOWS

#include <GoddamnEngine/DInterface/DGFX/DGFX.h>
#include <Windows.h>

GD_NAMESPACE_BEGIN

	struct _DGFXWindows : public DGFX
	{
		typedef DGFX _DGFXBase;

	protected:
		struct HwndStateData final
		{
			RECT  DataRect;
			DWORD DataStyle;
			DWORD DataStyleEx;
			bool  DataWasMaximized;
		};	// struct HwndStateData
		
		CHAR static const                _HwndBackgroundClassName[];
		CHAR static const                _HwndMainClassName[];
		HINSTANCE                        _HwndHinstance          = HINSTANCE();      
		HWND						 	 _HwndBackground         = HWND();
		HWND							 _HwndMain               = HWND();
		HwndStateData                    _HwndStateData;

	protected:
		GDINT void _CheckNotInitialized() const;
		GDINT void _CheckInitialized() const;

	public:
		GDAPI virtual DIResult Runtime_Initialize() override;
		GDAPI virtual DIResult Runtime_PostInitialize() override;
		GDAPI virtual DIResult Runtime_Deinitialize() override;

		GDAPI virtual void Runtime_PreUpdate() override {}
		GDAPI virtual void Runtime_Update() override;
		GDAPI virtual void Runtime_PostUpdate() override {}

		GDAPI virtual DGFXOutputMode Output_GetMode() const override;
		GDAPI virtual DIResult Output_SetMode(DGFXOutputMode const GfxOutputMode, bool const ForcelyReapply = false) override;

		GDAPI virtual DVectorRef<GeomResolution> Output_GetSupportedResolutions() const override;
		GDAPI virtual GeomResolution const* Output_GetResolution() const override;
		GDAPI virtual DIResult Output_SetResolution(GeomResolution const* const GfxResolution, bool const ForcelyReapply = false) override;
	};	// struct _DGFXWindows

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_WINDOWS