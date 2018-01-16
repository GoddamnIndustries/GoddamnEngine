// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#define GD_IGRAPHICS_OPENGL_IMPL 1

/*!
 * @file GoddamnEngine/GraphicsVulkan/GraphicsVulkanShaders.cpp
 * File contains implementation for Vulkan graphics interface: pipeline states.
 */ 
#include <GoddamnEngine/GraphicsVulkan/GraphicsVulkan.h>
#include <GoddamnEngine/Core/Interaction/Debug.h>

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Pipeline state object.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	/*!
	 * Creates a new GPU pipeline with specified parameters.
	 * @param gfxPipelineCreateInfo Creation information for the pipeline.
	 */
	GDAPI IGraphicsVulkanPipelineState::IGraphicsVulkanPipelineState(IGraphicsPipelineStateCreateInfo const* const gfxPipelineStateCreateInfo)
		: IGraphicsPipelineState(gfxPipelineStateCreateInfo)
	{
	}

	/*!
	 * Deletes the specified GPU pipeline.
	 */
	GDAPI IGraphicsVulkanPipelineState::~IGraphicsVulkanPipelineState()
	{
	}

	/*!
	 * Creates a new GPU pipeline with specified parameters.
	 *
	 * @param gfxPipelinePtr Pointer for output.
	 * @param gfxPipelineCreateInfo Creation information for the pipeline.
	 *
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsVulkanWithPipelineStates::GfxImm_PipelineCreate(IGraphicsPipelineState** const gfxPipelinePtr
		, IGraphicsPipelineStateCreateInfo const* const gfxPipelineCreateInfo)
	{
#if GD_DEBUG
		GD_ARG_VERIFY(gfxPipelinePtr != nullptr);
		GD_ARG_VERIFY(gfxPipelineCreateInfo != nullptr);
#endif	// if GD_DEBUG

		*gfxPipelinePtr = gd_new IGraphicsVulkanPipelineState(gfxPipelineCreateInfo);
		return IResult::Ok;
	}

	/*!
	 * Binds specified GPU pipeline into active GPU pipeline.
	 * 
	 * @param gfxCommandList Command list into which this command would be written.
	 * @param gfxPipeline Pointer to the pipeline.
	 */
	GDAPI void IGraphicsVulkanWithPipelineStates::GfxCmd_PipelineBind(IGraphicsCommandList* const gfxCommandList
		, IGraphicsPipelineState* const gfxPipeline)
	{
	}

GD_NAMESPACE_END
