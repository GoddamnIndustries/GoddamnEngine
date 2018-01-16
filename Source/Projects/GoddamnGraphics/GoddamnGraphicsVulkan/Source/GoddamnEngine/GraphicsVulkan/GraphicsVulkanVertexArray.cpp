// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#define GD_IGRAPHICS_OPENGL_IMPL 1

/*!
 * @file GoddamnEngine/GraphicsVulkan/GraphicsVulkanVertexArray.cpp
 * File contains implementation for Vulkan graphics interface: vertex array objects.
 */
#include <GoddamnEngine/GraphicsVulkan/GraphicsVulkan.h>
#include <GoddamnEngine/GraphicsVulkan/GraphicsVulkanFormat.h>

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Vertex array objects.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	/*!
	 * Creates a GPU vertex array with specified parameters.
	 * @param gfxVertexArrayCreateInfo Pointer to the vertex array creation information.
	 */
	GDAPI IGraphicsVulkanVertexArray::IGraphicsVulkanVertexArray(IGraphicsVertexArrayCreateInfo const* const gfxVertexArrayCreateInfo)
		: IGraphicsVertexArray(gfxVertexArrayCreateInfo)
	{
	}

	/*!
	 * Deletes current GPU vertex array.
	 */
	GDAPI IGraphicsVulkanVertexArray::~IGraphicsVulkanVertexArray()
	{
	}

	/*!
	 * Creates a GPU vertex array with specified parameters.
	 *
	 * @param gfxVertexArrayPtr Pointer for output.
	 * @param gfxVertexArrayCreateInfo Pointer to the vertex array creation information.
	 *
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsVulkanWithVertexArrays::GfxImm_VertexArrayCreate(IGraphicsVertexArray** const gfxVertexArrayPtr
		, IGraphicsVertexArrayCreateInfo const* const gfxVertexArrayCreateInfo)
	{
#if GD_DEBUG
		GD_ARG_VERIFY(gfxVertexArrayPtr != nullptr);
		GD_ARG_VERIFY(gfxVertexArrayCreateInfo != nullptr);
#endif	// if GD_DEBUG

		*gfxVertexArrayPtr = gd_new IGraphicsVulkanVertexArray(gfxVertexArrayCreateInfo);
		return IResult::Ok;
	}

	/*!
	 * Binds specified GPU vertex array into active GPU vertex array.
	 * 
	 * @param gfxCommandList Command list into which this command would be written.
	 * @param gfxVertexArray Pointer to the vertex array.
	 */
	GDAPI void IGraphicsVulkanWithVertexArrays::GfxCmd_VertexArrayBind(IGraphicsCommandList* const gfxCommandList
		, IGraphicsVertexArray* const gfxVertexArray)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_VertexArrayBind(gfxCommandList, gfxVertexArray)))
			return;
#endif	// if GD_DEBUG
	}

GD_NAMESPACE_END
