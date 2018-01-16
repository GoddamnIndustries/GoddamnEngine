// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Engine/Interface/Graphics/Graphics.cpp
 * File contains export interface for the engine graphics subsystem.
 */
#include <GoddamnEngine/Engine/Interface/Graphics/Graphics.h>
//#include <GoddamnEngine/Engine/Pipeline/Pipeline.h>


GD_NAMESPACE_BEGIN
	
#if 0
// ==========================================================================================
	// IGraphics interface.
	// ==========================================================================================

	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphics);
	GDAPI extern IGraphics* CreateIGraphicsInstance()
#if GD_ENGINE_MONOLITHIC
	{
#if GD_PLATFORM_WINDOWS
		Char static const CreateIGraphicsInstanceProcName[] = "?CreateIGraphicsInstance@Goddamn@@YAPEAUIGraphics@1@XZ";
#else	// if GD_PLATFORM_WINDOWS
		Char static const CreateIGraphicsInstanceProcName[] = "CreateIGraphicsInstance";
#endif	// if GD_PLATFORM_WINDOWS

		// Listing all dynamic libraries that matches names of the dynamic graphics interfaces..
	}
#else	// if GD_ENGINE_MONOLITHIC
	;
#endif	// if GD_ENGINE_MONOLITHIC

	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphicsCommandList);
	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphicsBuffer);
	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphicsVertexArray);
	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphicsSampler);
	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphicsShaderResourceView);
	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphicsTexture2D);
	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphicsTextureCube);
#if GD_FALSE
	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphicsTexture3D);
#endif	// if GD_FALSE
	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphicsShader);
	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphicsVertexShader);
	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphicsPixelShader);
	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphicsGeometryShader);
#if GD_FALSE
	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphicsHullShader);
	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphicsDomainShader);
#endif	// if GD_FALSE
#if GD_FALSE
	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphicsComputeShader);
#endif	// if GD_FALSE
	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphicsPipelineState);
#endif

	// ------------------------------------------------------------------------------------------
	//! Global graphics interface pointer.
	GDAPI IGraphics* Graphics = nullptr;

GD_NAMESPACE_END

