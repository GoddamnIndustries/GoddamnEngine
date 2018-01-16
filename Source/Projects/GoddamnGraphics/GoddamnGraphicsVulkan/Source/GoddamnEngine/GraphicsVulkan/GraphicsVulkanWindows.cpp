// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/GraphicsVulkan/GraphicsVulkanWindows.cpp
 * File contains Implementations for Windows-specific code of the Vulkan Implementation 
 * of the graphics interface.
 */
#include <GoddamnEngine/GraphicsVulkan/GraphicsVulkan.h>
#if GD_PLATFORM_WINDOWS

//#include <GoddamnEngine/Core/OutputDevice/OutputDevice.h>
#define GD_DLOG_CAT "GFX device (Windows@Vulkan)"

//! @todo Move this as a dependency.
#pragma comment (lib, "opengl32.lib")
#include <Windows.h>

GD_NAMESPACE_BEGIN

	// ------------------------------------------------------------------------------------------
	//! Function would be called on the global initialization step, before all other interfaces
	//! are initialized.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsVulkanWindows::OnRuntimePreInitialize()
	{
		IResult const _BaseResult = IGraphicsPlatform::OnRuntimePreInitialize();
		if (IFailed(_BaseResult))
			return _BaseResult;

		// Retrieving information about the list of supported screen resolutions.
		{
			DEVMODE HGLRCTestCanvasMode;
			HGLRCTestCanvasMode.dmSize = sizeof(HGLRCTestCanvasMode);
			for (DWORD _Cnt = 0; EnumDisplaySettings(nullptr, _Cnt, &HGLRCTestCanvasMode) != GD_FALSE; ++_Cnt)
			{
				// Testing current resolution..
				if (ChangeDisplaySettings(&HGLRCTestCanvasMode, CDS_TEST) == DISP_CHANGE_SUCCESSFUL)
				{
					m_GfxResolutionsList.InsertAt(0, { static_cast<UInt32>(HGLRCTestCanvasMode.dmPelsWidth), static_cast<UInt32>(HGLRCTestCanvasMode.dmPelsHeight), 30, 1 });
					m_GfxResolutionsList.InsertAt(0, { static_cast<UInt32>(HGLRCTestCanvasMode.dmPelsWidth), static_cast<UInt32>(HGLRCTestCanvasMode.dmPelsHeight), 60, 1 });
				}
			}
		}

		//! @todo Load default parameters.
		m_GfxCanvasMode = IGRAPHICS_OUTPUT_MODE_WINDOWED;	// Easier for debugging purposes.
		m_GfxResolutionSelected = &m_GfxResolutionsList.GetData()[8];

		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Function would be called on the global initialization step.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsVulkanWindows::OnRuntimeInitialize()
	{
	//	_CheckNotInitialized();
	//	ConsoleDevice->Log(GD_DLOG_CAT ": going to initialize graphics devices...");

		IResult const _BaseResult = IGraphicsPlatform::OnRuntimeInitialize();
		if (IFailed(_BaseResult))
			return _BaseResult;

		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Function would be called on the global deinitialization step.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsVulkanWindows::OnRuntimeDeinitialize()
	{
		return IGraphicsPlatform::OnRuntimeDeinitialize();
	}

	// ------------------------------------------------------------------------------------------
	//! Function would be called once per frame, after all other runtime 
	//! interfaces are deinitialized.
	GDAPI void IGraphicsVulkanWindows::OnRuntimePostUpdate()
	{
		//! @todo Uncomment this.
		IGraphicsPlatform::OnRuntimePostUpdate();
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_WINDOWS
