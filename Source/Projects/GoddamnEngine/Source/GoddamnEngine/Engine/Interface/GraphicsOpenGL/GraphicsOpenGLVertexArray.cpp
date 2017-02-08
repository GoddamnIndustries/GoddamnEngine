// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#define GD_IGRAPHICS_OPENGL_IMPL 1

/*!
 * @file GoddamnEngine/Engine/Interface/GraphicsOpenGL/GraphicsOpenGLVertexArray.cpp
 * File contains Implementation for OpenGL 4.3 Implementation of the graphics interface: vertex array objects.
 */
#include <GoddamnEngine/Engine/Interface/GraphicsOpenGL/GraphicsOpenGL.h>
#include <GoddamnEngine/Engine/Interface/GraphicsOpenGL/GraphicsOpenGLFormat.h>

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Vertex array objects.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	/*!
	 * Creates a GPU vertex array with specified parameters.
	 * @param gfxVertexArrayCreationInfo Pointer to the vertex array creation information.
	 */
	GDAPI IGraphicsOpenGLVertexArray::IGraphicsOpenGLVertexArray(IGraphicsVertexArrayCreationInfo const* const gfxVertexArrayCreationInfo)
		: IGraphicsVertexArray(gfxVertexArrayCreationInfo), m_GLVertexArrayID(0)
	{
		glGenVertexArrays(1, &m_GLVertexArrayID);
		glBindVertexArray(m_GLVertexArrayID);
		for (auto gfxSlotIndex = 0; gfxSlotIndex < gfxVertexArrayCreationInfo->VertexArrayLayout->LayoutSlotsCount; ++gfxSlotIndex)
		{
			// Binding all the m_Index buffers..
			// All the binded data here is stored into our vertex array and can be safely binded when we render stuff.
			auto const  gfxVertexBuffer = static_cast<IGraphicsOpenGLBuffer const*>(gfxVertexArrayCreationInfo->ArrayVertexBuffers[gfxSlotIndex]);
			auto const& gfxVertexArrayLayoutSlot = gfxVertexArrayCreationInfo->VertexArrayLayout->LayoutSlots[gfxSlotIndex];

			// We are using old OpenGL API. It takes less code to work with data, also 
			// it matches our vertex layout model in better way.
			// https://www.opengl.org/wiki/Vertex_Specification#Separate_attribute_format
			glEnableVertexAttribArray(gfxSlotIndex);
			glBindBuffer(GL_ARRAY_BUFFER, gfxVertexBuffer->m_GLBufferID);
			glVertexAttribPointer(gfxSlotIndex, static_cast<GLint>(IGraphicsFormatGetLayout(gfxVertexArrayLayoutSlot.SlotFormat))
				, IGraphicsOpenGLFormatGetType(gfxVertexArrayLayoutSlot.SlotFormat), GL_FALSE, 0, nullptr);
			if (gfxVertexArrayLayoutSlot.SlotClass == IGRAPHICS_INPUT_SLOT_CLASS_PER_INSTANCE)
			{
				// We need to specify the divisor for current vertex attribute.
				// If this is a per-vertex slot, any step rate is ignored.
				glVertexAttribDivisor(gfxSlotIndex, gfxVertexArrayLayoutSlot.SlotDataStepRate);
			}

		}
		if (gfxVertexArrayCreationInfo->ArrayIndexBuffer != nullptr)
		{
			// Binding the index buffer (if it actually exists)..
			auto const gfxIndexBuffer = static_cast<IGraphicsOpenGLBuffer const*>(gfxVertexArrayCreationInfo->ArrayIndexBuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gfxIndexBuffer->m_GLBufferID);
		}

		// Cleaning up..
		glBindVertexArray(0);
	}

	/*!
	 * Deletes current GPU vertex array.
	 */
	GDAPI IGraphicsOpenGLVertexArray::~IGraphicsOpenGLVertexArray()
	{
		glBindVertexArray(0);
		glDeleteVertexArrays(1, &m_GLVertexArrayID);
	}

	/*!
	 * Creates a GPU vertex array with specified parameters.
	 *
	 * @param gfxVertexArrayPtr Pointer for output.
	 * @param gfxVertexArrayCreationInfo Pointer to the vertex array creation information.
	 *
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsOpenGLWithVertexArrays::GfxImm_VertexArrayCreate(IGraphicsVertexArray** const gfxVertexArrayPtr
		, IGraphicsVertexArrayCreationInfo const* const gfxVertexArrayCreationInfo)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxImm_VertexArrayCreate(gfxVertexArrayPtr, gfxVertexArrayCreationInfo)))
			return IResult::InvalidArguments;
#endif	// if GD_DEBUG

		*gfxVertexArrayPtr = gd_new IGraphicsOpenGLVertexArray(gfxVertexArrayCreationInfo);
		return IResult::Ok;
	}

	/*!
	 * Binds specified GPU vertex array into active GPU vertex array.
	 * 
	 * @param gfxCommandList Command list into which this command would be written.
	 * @param gfxVertexArray Pointer to the vertex array.
	 */
	GDAPI void IGraphicsOpenGLWithVertexArrays::GfxCmd_VertexArrayBind(IGraphicsCommandList* const gfxCommandList
		, IGraphicsVertexArray* const gfxVertexArray)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_VertexArrayBind(gfxCommandList, gfxVertexArray)))
			return;
#endif	// if GD_DEBUG

		auto const glVertexArray = static_cast<IGraphicsOpenGLVertexArray*>(gfxVertexArray);
		if (glVertexArray != nullptr)
		{
			// Binding the index buffer..
			glBindVertexArray(glVertexArray->m_GLVertexArrayID);
		}
		else
		{
			// Unbinding everything..
			glBindVertexArray(0);
		}
	}

GD_NAMESPACE_END
