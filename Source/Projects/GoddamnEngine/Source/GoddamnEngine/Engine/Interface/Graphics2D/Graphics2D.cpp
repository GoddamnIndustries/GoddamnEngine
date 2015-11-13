// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Engine/Interface/Graphics/Graphics2D.cpp
 * File contains export interface for the engine 2D graphics subsystem.
 */
#include <GoddamnEngine/Engine/Interface/Graphics2D/Graphics2D.h>
#include <GoddamnEngine/Engine/Pipeline/Pipeline.h>

GD_NAMESPACE_BEGIN

	// ==========================================================================================
	// IGraphics2D interface.
	// ==========================================================================================

	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphics2D);
	GDAPI extern IGraphics2D* CreateIGraphics2DInstance()
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

	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphics2DRenderer);
	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphics2DBitmap);
	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphics2DRectRenderer);
	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphics2DFont);
	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphics2DTextRenderer);

	// ------------------------------------------------------------------------------------------
	//! Global 2D graphics interface pointer.
	GDAPI IUniqueInterfacePointer<IGraphics2D> Graphics2D(CreateIGraphics2DInstance(), PipelineManager->GetAppropriateRenderingPipeline());
	GD_SINGLETON_IMPLEMENTATION(IGraphics2D);

GD_NAMESPACE_END
