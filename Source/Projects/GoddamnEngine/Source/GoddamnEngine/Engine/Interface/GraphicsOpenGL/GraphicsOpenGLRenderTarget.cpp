// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#define GD_IGRAPHICS_OPENGL_IMPL 1

/*!
 * @file GoddamnEngine/Engine/Interface/GraphicsOpenGL/GraphicsOpenGLRenderTarget.cpp
 * File contains Implementation for OpenGL 4.3 Implementation of the graphics interface: render targets.
 */ 
#include <GoddamnEngine/Engine/Interface/GraphicsOpenGL/GraphicsOpenGL.h>
#include <GoddamnEngine/Engine/Interface/GraphicsOpenGL/GraphicsOpenGLFormat.h>
 //#include <GoddamnEngine/Core/OutputDevice/OutputDevice.h>

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Render targets.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	/*!
	 * Clears current render target depth buffer.
	 * @param gfxCommandList Command list into which this command would be written.
	 */
	GDAPI void IGraphicsOpenGLWithRenderTargets::GfxCmd_RenderTargetClearDepth(IGraphicsCommandList* const gfxCommandList)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_RenderTargetClearDepth(gfxCommandList)))
			return;
#endif	// if GD_DEBUG

		glClearDepth(1.0f);
	}

	/*!
	 * Clear current render target with a specified color.
	 *
	 * @param gfxCommandList Command list into which this command would be written.
	 * @param gfxClearColor Color that would be used to fill the render target.
	 */
	GDAPI void IGraphicsOpenGLWithRenderTargets::GfxCmd_RenderTargetClearColor(IGraphicsCommandList* const gfxCommandList, GeomColor const gfxClearColor)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_RenderTargetClearColor(gfxCommandList, gfxClearColor)))
			return;
#endif	// if GD_DEBUG

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(gfxClearColor.R, gfxClearColor.G, gfxClearColor.B, gfxClearColor.A);
	}

	/*!
	 * Renders currently binded vertex array with the binded pipeline state.
	 * 
	 * @param gfxCommandList Command list into which this command would be written.
	 * @param gfxTopology Topology of the binded vertex array.
	 * @param gfxVerticesCount Amount of the vertices in the vertex array.
	 */
	GDAPI void IGraphicsOpenGLWithRenderTargets::GfxCmd_RenderTargetRender(IGraphicsCommandList* const gfxCommandList
		, IGraphicsTopologyType const gfxTopology, SizeTp const gfxVerticesCount)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_RenderTargetRender(gfxCommandList, gfxTopology, gfxVerticesCount)))
			return;
#endif	// if GD_DEBUG

		glDrawArrays(IGraphicsOpenGLTopologyTypesTable[gfxTopology], 0, static_cast<GLsizei>(gfxVerticesCount));
	}

	/*!
	 * Renders currently binded indexed vertex array with the binded pipeline state.
	 * 
	 * @param gfxCommandList Command list into which this command would be written.
	 * @param gfxTopology Topology of the binded vertex array.
	 * @param gfxIndicesCount Amount of the indeces in the index array.
	 * @param gfxIndexType Type of the single index.
	 */
	GDAPI void IGraphicsOpenGLWithRenderTargets::GfxCmd_RenderTargetRenderIndexed(IGraphicsCommandList* const gfxCommandList
		, IGraphicsTopologyType const gfxTopology, SizeTp const gfxIndicesCount, IGraphicsFormatType gfxIndexType)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_RenderTargetRenderIndexed(gfxCommandList, gfxTopology, gfxIndicesCount, gfxIndexType)))
			return;
#endif	// if GD_DEBUG

		glDrawElements(IGraphicsOpenGLTopologyTypesTable[gfxTopology], static_cast<GLsizei>(gfxIndicesCount)
			, IGraphicsOpenGLFormatTypesTable[gfxIndexType], nullptr);
	}

GD_NAMESPACE_END
