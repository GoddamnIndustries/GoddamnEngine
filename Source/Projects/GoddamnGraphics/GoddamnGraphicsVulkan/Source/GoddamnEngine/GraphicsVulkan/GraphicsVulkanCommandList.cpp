// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#define GD_IGRAPHICS_OPENGL_IMPL 1

/*!
 * @file GoddamnEngine/GraphicsVulkan/GraphicsVulkanCommandLists.cpp
 * File contains implementation for Vulkan graphics interface: command lists.
 */ 
#include <GoddamnEngine/GraphicsVulkan/GraphicsVulkan.h>

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Command lists.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	/*!
	 * Creates a GPU command list with specified parameters.
	 *
	 * @param gfxCommandListPtr Pointer for output.
	 * @param gfxCommandListCreateInfo Pointer to the command list creation information.
	 *
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsVulkanWithCommandLists::GfxImm_CommandListCreate(IGraphicsCommandList** const gfxCommandListPtr
		, IGraphicsCommandListCreateInfo const* const gfxCommandListCreateInfo)
	{
#if GD_DEBUG
		GD_ARG_VERIFY(gfxCommandListPtr != nullptr);
		GD_ARG_VERIFY(gfxCommandListCreateInfo != nullptr);
#endif	// if GD_DEBUG

		*gfxCommandListPtr = nullptr;
		return IResult::NotSupported;
	}

GD_NAMESPACE_END

