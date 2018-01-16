// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#define GD_IGRAPHICS_OPENGL_IMPL 1

/*!
 * @file GoddamnEngine/GraphicsOpenGL/GraphicsOpenGLRenderTarget.cpp
 * File contains implementation for OpenGL(ES) graphics interface: render targets.
 */ 
#include <GoddamnEngine/GraphicsOpenGL/GraphicsOpenGL.h>
#include <GoddamnEngine/GraphicsOpenGL/GraphicsOpenGLFormat.h>

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Render targets.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GLenum static const g_OpenGLTopologyTypesTable[IGRAPHICS_SHADER_TOPOLOGIES_COUNT] = {
		/* IGRAPHICS_SHADER_TOPOLOGY_POINTS         */ GL_POINTS,
		/* IGRAPHICS_SHADER_TOPOLOGY_LINE_STRIP     */ GL_LINE_STRIP,
		/* IGRAPHICS_SHADER_TOPOLOGY_LINES          */ GL_LINES,
		/* IGRAPHICS_SHADER_TOPOLOGY_TRIANGLE_STRIP */ GL_TRIANGLE_STRIP,
		/* IGRAPHICS_SHADER_TOPOLOGY_TRIANGLES      */ GL_TRIANGLES,
		/* IGRAPHICS_SHADER_TOPOLOGY_UNKNOWN        */ 0,
	};

	/*!
	 * Clears current render target depth buffer.
	 * @param gfxCommandList Command list into which this command would be written.
	 */
	GDAPI void IGraphicsOpenGLWithRenderTargets::GfxCmd_RenderTargetClearDepth(IGraphicsCommandList* const gfxCommandList)
	{
		glClearDepth(1.0f);
	}

	/*!
	 * Clear current render target with a specified color.
	 *
	 * @param gfxCommandList Command list into which this command would be written.
	 * @param gfxClearColor Color that would be used to fill the render target.
	 */
	GDAPI void IGraphicsOpenGLWithRenderTargets::GfxCmd_RenderTargetClearColor(IGraphicsCommandList* const gfxCommandList
		, GeomColor const gfxClearColor)
	{
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
		glDrawArrays(g_OpenGLTopologyTypesTable[gfxTopology], 0, static_cast<GLsizei>(gfxVerticesCount));
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
		glDrawElements(g_OpenGLTopologyTypesTable[gfxTopology], static_cast<GLsizei>(gfxIndicesCount)
			, g_OpenGLFormatTypesTable[gfxIndexType], nullptr);
	}

GD_NAMESPACE_END
