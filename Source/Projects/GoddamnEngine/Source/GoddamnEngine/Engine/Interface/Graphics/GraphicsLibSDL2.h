// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Engine/Interface/Graphics/Graphics/GraphicsSDL2.h
 * File contains export interface for SDL2 Implementation of the graphics interface.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#if GD_PLATFORM_API_LIBSDL2

#include <GoddamnEngine/Engine/Interface/Graphics/Graphics.h>
#include <SDL2/SDL.h>

GD_NAMESPACE_BEGIN

	// ------------------------------------------------------------------------------------------
	//! SDL2-specific code of the graphics device implementation.
	struct IGraphicsSDL2 : public IGraphics
	{
		friend uinterface IInputWindows;
		friend uinterface IGraphics2DDirect2D;

	protected:
		SDL_Window* SDLWindow;

	protected:
		GDINT void _CheckNotInitialized() const;
		GDINT void _CheckInitialized() const;

	public:
		GDAPI virtual IResult OnRuntimePreInitialize() override;
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
	};	// struct IGraphicsSDL2

	typedef IGraphicsSDL2 IGraphicsPlatform;

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_API_LIBSDL2
