// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/GraphicsOpenGL/GraphicsOpenGLSDL2.cpp
 * File contains Implementations for Windows-specific code of the OpenGL 4.3 Implementation 
 * of the graphics interface.
 */
#include <GoddamnEngine/GraphicsOpenGL/GraphicsOpenGLSDL2.h>
#if GD_PLATFORM_API_LIBSDL2

#include <GoddamnEngine/Core/System/OutputDevice/OutputDevice.h>
#define GD_DLOG_CAT "GFX device (SDL2@OpenGL)"

GD_NAMESPACE_BEGIN

	// ------------------------------------------------------------------------------------------
	//! Function would be called on the global initialization step.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsOpenGLSDL2::OnRuntimeInitialize()
	{
		_CheckNotInitialized();
		ConsoleDevice->Log(GD_DLOG_CAT ": going to initialize graphics devices...");

		IResult const _BaseResult = IGraphicsPlatform::OnRuntimeInitialize();
		if (IFailed(_BaseResult))
			return _BaseResult;



		ConsoleDevice->Log(GD_DLOG_CAT ": ... initialized.");
		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Function would be called on the global deinitialization step.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsOpenGLSDL2::OnRuntimeDeinitialize()
	{
		

		return IGraphicsPlatform::OnRuntimeDeinitialize();
	}

	// ------------------------------------------------------------------------------------------
	//! Function would be called once per frame, after all other runtime 
	//! interfaces are deinitialized.
	GDAPI void IGraphicsOpenGLSDL2::OnRuntimePostUpdate()
	{
		//! @todo Uncomment this.
		/*IGraphicsPlatform::OnRuntimePostUpdate();
		SwapBuffers(HGLRCDeviceContext);*/
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_API_LIBSDL2
