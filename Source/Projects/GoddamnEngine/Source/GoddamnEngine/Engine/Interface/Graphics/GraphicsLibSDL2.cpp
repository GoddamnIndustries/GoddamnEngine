// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Engine/Interface/Graphics/Graphics/GraphicsSDL2.cpp
 * File contains Implementation for the export interface for SDL2 Implementation of the graphics interface.
 */
#include <GoddamnEngine/Engine/Interface/Graphics/GraphicsLibSDL2.h>
#if GD_PLATFORM_API_LIBSDL2

#include <GoddamnEngine/Engine/Interface/OutputDevice/OutputDevice.h>
#define GD_DLOG_CAT "GFX device (LibSDL2@Canvas)"

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Runtime interface's functionality Implementation.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Function would be called on the global initialization step, before all other interfaces
	//! are initialized.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsSDL2::OnRuntimePreInitialize()
	{
		IResult const _BaseResult = IGraphicsPlatform::OnRuntimePreInitialize();
		if (IFailed(_BaseResult))
			return _BaseResult;

		// Retrieving information about the list of supported screen resolutions.
		{
			int static const sdlCurrentDisplay = 0;	// We have no support for multiple display systems.
			
			int const sdlDisplayModesCount = SDL_GetNumDisplayModes(sdlCurrentDisplay);
			GD_ASSERT(sdlDisplayModesCount > 0, "No display mods information was retrieved.");
			for (SizeTp sdlDisplayModeIndex = 0; sdlDisplayModeIndex < sdlDisplayModesCount; ++sdlDisplayModeIndex)
			{
				// Retrieving display lists in the reverse order..
				SDL_DisplayMode sdlDisplayMode = {};
				if (SDL_GetDisplayMode(sdlCurrentDisplay, sdlDisplayModesCount - sdlDisplayModeIndex - 1, &sdlDisplayMode) != GD_FALSE)
				{
					GfxResolutionsList.InsertLast({ static_cast<UInt32>(sdlDisplayMode.w), static_cast<UInt32>(sdlDisplayMode.w)
						, sdlDisplayMode.refresh_rate, 1 });
				}
			}
		}

		//! @todo Load default parameters.
		GfxCanvasMode = IGRAPHICS_OUTPUT_MODE_WINDOWED;	// Easier for debugging purposes.
		GfxResolutionSelected = &GfxResolutionsList.GetData()[8];

		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Function would be called on the global initialization step.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsSDL2::OnRuntimeInitialize()
	{
		_CheckNotInitialized();
		ConsoleDevice->LogFormat(GD_DLOG_CAT ": going to initialize canvas devices:"
			"\n\tMode       - %d"
			"\n\tResolution - %dx%d@%d..."
			, GfxCanvasMode, GfxResolutionSelected->Width, GfxResolutionSelected->Height
			, GfxResolutionSelected->VsyncNumerator / GfxResolutionSelected->VsyncDenominator);

#if GD_DEBUG
		if (GfxCanvasMode == IGRAPHICS_OUTPUT_MODE_FULLSCREEN)
		{
			// In some cases and GPUs debugging in fullscreen may cause driver crashes or
			// inability to switch back to the canvas window and continue executing the app.
			// Pseudo-fullscreen mode can save for us a lot of health points.
			//
			// P.S. You can still manually set mode to true fullscreen. Happy debugging then.
			GfxCanvasMode = IGRAPHICS_OUTPUT_MODE_PSEUDO_FULLSCREEN;
			ConsoleDevice->LogWarning(GD_DLOG_CAT ": ...default mode was set to be fullscreen - forcedly setting to pseudo-fullscreen for debug purposes.");
		}
#endif	// if GD_DEBUG

		// Initializing the SDL2
		GD_ASSERT(SDL_Init(SDL_INIT_EVERYTHING) == 0
			, "'SDL_Init' function has failed");

		// Creating the output window..
		SDLWindow = SDL_CreateWindow("(GoddamnEngine) Main window."
			, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED	// Position on screen.
			, static_cast<int>(GfxResolutionSelected->Width), static_cast<int>(GfxResolutionSelected->Height)
			, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
		GD_ASSERT(SDLWindow != nullptr, "'SDL_CreateWindow' function has failed.");
		
		ConsoleDevice->Log(GD_DLOG_CAT ": ... initialized.");
		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Function would be called on the global initialization step, after all other interfaces
	//! are initialized.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsSDL2::OnRuntimePostInitialize()
	{
		Canvas_SetMode(GfxCanvasMode, true);
		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Function would be called on the global deinitialization step.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsSDL2::OnRuntimeDeinitialize()
	{
		SDL_DestroyWindow(SDLWindow);
		SDLWindow = nullptr;

		SDL_Quit();

		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Function would be called once per frame.
	GDAPI void IGraphicsSDL2::OnRuntimeUpdate()
	{
		_CheckInitialized();

		SDL_Event sdlEvent = {};
		while (SDL_PollEvent(&sdlEvent) != GD_FALSE)
		{
			//! @todo Do something with the events.
		}
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Canvas functionality management.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ... Canvas mode ...

	// ------------------------------------------------------------------------------------------
	//! Retrieves the mode in which graphics canvas device is running. 
	//! @returns The mode in which graphics canvas device is running.
	GDAPI IGraphicsCanvasMode IGraphicsSDL2::Canvas_GetMode() const
	{
		_CheckInitialized();
		return GfxCanvasMode;
	}

	// ------------------------------------------------------------------------------------------
	//! Changes the mode in which the graphics canvas device is running. 
	//! @param GfxCanvasMode The new canvas mode that would be set.
	//! @param gfxForcelyReapply Do forcedly reapply mode, even if device is running is the same mode.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsSDL2::Canvas_SetMode(IGraphicsCanvasMode const gfxCanvasMode
		, bool const gfxForcelyReapply /*= false*/)
	{
		_CheckInitialized();
		ConsoleDevice->LogFormat(GD_DLOG_CAT ": mode is going to be set to %d...", gfxCanvasMode);

		GfxCanvasMode = gfxCanvasMode;
		GD_ASSERT(SDL_SetWindowFullscreen(SDLWindow, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
			| gfxCanvasMode != IGRAPHICS_OUTPUT_MODE_WINDOWED ? SDL_WINDOW_FULLSCREEN : 0) == 0
			, "'SDL_SetWindowFullscreen' function has failed.");

		return IResult::NothingWasDone;
	}
	
	// ... Canvas device resolutions ...

	// ------------------------------------------------------------------------------------------
	//! Returns a vector reference with all supported resolution descriptors.
	//! @returns A vector reference with all supported resolution descriptors.
	GDAPI GeomResolution const* IGraphicsSDL2::Canvas_GetSupportedResolutions() const
	{
		_CheckInitialized();
		return GfxResolutionsList.GetData();
	}
	
	// ------------------------------------------------------------------------------------------
	//! Returns pointer to the resolution descriptor that is currently set.
	//! @returns Pointer to the resolution descriptor that is currently set or null on failure.
	GDAPI GeomResolution const* IGraphicsSDL2::Canvas_GetResolution() const
	{
		_CheckInitialized();
		return GfxResolutionSelected;
	}
	
	// ------------------------------------------------------------------------------------------
	//! Changes resolution in which graphics canvas device is rendering and resize several frame
	//! buffers (those, that have a corresponding flag specified).
	//! @param gfxResolution Pointer to the resolution descriptor that would be set.
	//! @param gfxForcelyReapply Do forcedly resolution mode, even if device is running is the same resolution.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsSDL2::Canvas_SetResolution(GeomResolution const* const gfxResolution
		, bool const gfxForcelyReapply /*= false*/)
	{
		_CheckInitialized();
		ConsoleDevice->LogFormat(GD_DLOG_CAT ": resolution is going to be set to %dx%d@%d"
			, GfxResolutionSelected->Width, GfxResolutionSelected->Height
			, GfxResolutionSelected->VsyncNumerator / GfxResolutionSelected->VsyncDenominator);

		GfxResolutionSelected = gfxResolution;
		SDL_SetWindowSize(SDLWindow, static_cast<int>(GfxResolutionSelected->Width), static_cast<int>(GfxResolutionSelected->Height));
		
		return IResult::NothingWasDone;
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_API_LIBSDL2
