// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#define GD_IGRAPHICS_OPENGL_IMPL 1

/*!
 * @file GoddamnEngine/GraphicsVulkan/GraphicsVulkanRenderTarget.cpp
 * File contains implementation for Vulkan graphics interface: render targets.
 */ 
#include <GoddamnEngine/GraphicsVulkan/GraphicsVulkan.h>
#include <GoddamnEngine/GraphicsVulkan/GraphicsVulkanFormat.h>

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Render targets.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	/*!
	 * Clears current render target depth buffer.
	 * @param gfxCommandList Command list into which this command would be written.
	 */
	GDAPI void IGraphicsVulkanWithRenderTargets::GfxCmd_RenderTargetClearDepth(IGraphicsCommandList* const gfxCommandList)
	{
	}

	/*!
	 * Clear current render target with a specified color.
	 *
	 * @param gfxCommandList Command list into which this command would be written.
	 * @param gfxClearColor Color that would be used to fill the render target.
	 */
	GDAPI void IGraphicsVulkanWithRenderTargets::GfxCmd_RenderTargetClearColor(IGraphicsCommandList* const gfxCommandList
		, GeomColor const gfxClearColor)
	{
	}

	/*!
	 * Renders currently binded vertex array with the binded pipeline state.
	 * 
	 * @param gfxCommandList Command list into which this command would be written.
	 * @param gfxTopology Topology of the binded vertex array.
	 * @param gfxVerticesCount Amount of the vertices in the vertex array.
	 */
	GDAPI void IGraphicsVulkanWithRenderTargets::GfxCmd_RenderTargetRender(IGraphicsCommandList* const gfxCommandList
		, IGraphicsTopologyType const gfxTopology, SizeTp const gfxVerticesCount)
	{
	}

	/*!
	 * Renders currently binded indexed vertex array with the binded pipeline state.
	 * 
	 * @param gfxCommandList Command list into which this command would be written.
	 * @param gfxTopology Topology of the binded vertex array.
	 * @param gfxIndicesCount Amount of the indeces in the index array.
	 * @param gfxIndexType Type of the single index.
	 */
	GDAPI void IGraphicsVulkanWithRenderTargets::GfxCmd_RenderTargetRenderIndexed(IGraphicsCommandList* const gfxCommandList
		, IGraphicsTopologyType const gfxTopology, SizeTp const gfxIndicesCount, IGraphicsFormatType gfxIndexType)
	{
	}

GD_NAMESPACE_END
