// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/GraphicsVulkan/GraphicsVulkanWindows.h
 * File contains export interface for Windows-specific code of the Vulkan Implementation 
 * of the graphics interface.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Interface/Graphics/Graphics.h>
#if GD_PLATFORM_WINDOWS

GD_NAMESPACE_BEGIN

	// ------------------------------------------------------------------------------------------
	//! Windows-specific code of the Vulkan graphics device implementation.
	struct IGraphicsVulkanWindows : public IGraphicsPlatform
	{
	private:
		HGLRC HGLRCContext = nullptr;
		HDC   HGLRCDeviceContext = nullptr;

	public:
		GDAPI virtual IResult OnRuntimePreInitialize() override;
		GDAPI virtual IResult OnRuntimeInitialize() override;
		GDAPI virtual IResult OnRuntimeDeinitialize() override;
		GDAPI virtual void OnRuntimePreUpdate() override {}
		GDAPI virtual void OnRuntimePostUpdate() override;

	};	// struct IGraphicsVulkanWindows

	typedef IGraphicsVulkanWindows IGraphicsVulkanPlatform;

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_WINDOWS
