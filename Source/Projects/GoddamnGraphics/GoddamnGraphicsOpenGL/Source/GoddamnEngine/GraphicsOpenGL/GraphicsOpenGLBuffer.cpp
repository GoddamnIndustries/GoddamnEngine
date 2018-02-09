// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file
 * OpenGL(ES) graphics interface: buffer objects.
 */
#include <GoddamnEngine/GraphicsOpenGL/GraphicsOpenGL.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                            IGraphicsBuffer interface.                            ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	/*!
	 * Creates a GPU buffer with specified parameters.
	 *
	 * @param gfxBufferPtr Pointer for output.
	 * @param gfxBufferCreateInfo Pointer to the buffer creation information.
	 *
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsOpenGLWithBuffers::GfxImm_BufferCreate(IGraphicsBuffer*& gfxBufferPtr, IGraphicsBufferCreateInfo const& gfxBufferCreateInfo, CHandle const gfxBufferInitialData)
	{
		// Setting up the buffer type..
		GLenum glBufferType;
		switch (gfxBufferCreateInfo.BufferType)
		{
			case IGRAPHICS_BUFFER_TYPE_VERTEX:
				glBufferType = GL_ARRAY_BUFFER;
				break;
			case IGRAPHICS_BUFFER_TYPE_INDEX:
				glBufferType = GL_ELEMENT_ARRAY_BUFFER;
				break;
			case IGRAPHICS_BUFFER_TYPE_UNIFORM:
				glBufferType = GL_ELEMENT_ARRAY_BUFFER;
				break;
			default:
				return IResult::InvalidArguments;
		}

		// Setting up the usage..
		GLbitfield glBufferFlags = 0;
		switch (gfxBufferCreateInfo.ResourceUsage)
		{
			case IGRAPHICS_RESOURCE_USAGE_DEFAULT:
			case IGRAPHICS_RESOURCE_USAGE_IMMUTABLE:
			case IGRAPHICS_RESOURCE_USAGE_DYNAMIC:
			case IGRAPHICS_RESOURCE_USAGE_STAGING:
				break;
			default:
				return IResult::InvalidArguments;
		}
		if ((gfxBufferCreateInfo.ResourceCpuAccessFlags & IGRAPHICS_RESOURCE_CPU_ACCESS_FLAGS_WRITE) != 0)
		{
			glBufferFlags |= GL_MAP_WRITE_BIT;
			glBufferFlags |= GL_MAP_PERSISTENT_BIT;
		}
		if ((gfxBufferCreateInfo.ResourceCpuAccessFlags & IGRAPHICS_RESOURCE_CPU_ACCESS_FLAGS_READ) != 0)
		{
			glBufferFlags |= GL_MAP_READ_BIT;
			glBufferFlags |= GL_MAP_PERSISTENT_BIT;
		}

		// Creating the buffer..
		GLuint glBufferID = 0;
		glThrowIfFailed(glCreateBuffers(1, &glBufferID));

		// Allocating and passing the initial data..
		GLsizeiptr const glBufferSize = gfxBufferCreateInfo.BufferSize;
		glThrowIfFailed(glNamedBufferStorage(glBufferID, glBufferSize, gfxBufferInitialData, glBufferFlags));

		gfxBufferPtr = gd_new IGraphicsOpenGLBuffer(gfxBufferCreateInfo, glBufferID, glBufferType);
		return IResult::Ok;
	}

	/*!
	 * Destroys the GPU buffer.
	 *
	 * @param gfxBuffer Buffer to destroy.
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsOpenGLWithBuffers::GfxImm_BufferDestroy(IGraphicsBuffer* const gfxBuffer)
	{
		GD_DEBUG_VERIFY(gfxBuffer != nullptr);
		
		GfxImm_BufferUnmap(gfxBuffer);

		auto const glBuffer = static_cast<IGraphicsOpenGLBuffer*>(gfxBuffer);
		auto const glBufferID = glBuffer->BufferID;
		gd_delete glBuffer;

		glThrowIfFailed(glDeleteBuffers(1, &glBufferID));

		return IResult::Ok;
	}

	/*!
	 * Maps all GPU's buffer data on the CPU memory.
	 *
	 * @param gfxBuffer Buffer to map.
	 * @param gfxMapMode Buffer map mode.
	 * @param gfxMappedMemory Pointer to the memory handle.
	 *
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsOpenGLWithBuffers::GfxImm_BufferMap(IGraphicsBuffer* const gfxBuffer, IGraphicsResourceMapMode const gfxMapMode, Handle& gfxMappedMemory)
	{
		GD_DEBUG_VERIFY(gfxBuffer != nullptr);

		auto const glBuffer = static_cast<IGraphicsOpenGLBuffer*>(gfxBuffer);
		auto const glBufferID = glBuffer->BufferID;
		if (glBuffer->BufferMappedMemory == nullptr)
		{
			GLenum glMapMode;
			switch (gfxMapMode)
			{
				case IGRAPHICS_RESOURCE_MAP_MODE_READ:
					glMapMode = GL_READ_ONLY;
					break;
				case IGRAPHICS_RESOURCE_MAP_MODE_WRITE:
					glMapMode = GL_WRITE_ONLY;
					break;
				case IGRAPHICS_RESOURCE_MAP_MODE_READ_WRITE:
					glMapMode = GL_READ_WRITE;
					break;
				default:
					return IResult::InvalidArguments;
			}
			glBuffer->BufferMappedMemory = glMapNamedBuffer(glBufferID, glMapMode);
			glThrowIfFailed();
		}
		gfxMappedMemory = glBuffer->BufferMappedMemory;
		return IResult::Ok;
	}

	/*!
	 * Unmaps the buffer from the CPU memory.
	 *
	 * @param gfxBuffer Buffer to unmap.
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsOpenGLWithBuffers::GfxImm_BufferUnmap(IGraphicsBuffer* const gfxBuffer)
	{
		GD_DEBUG_VERIFY(gfxBuffer != nullptr);

		auto const glBuffer = static_cast<IGraphicsOpenGLBuffer*>(gfxBuffer);
		auto const glBufferID = glBuffer->BufferID;
		if (glBuffer->BufferMappedMemory != nullptr)
		{
			// Unmapping the mapped memory..
			glThrowIfFailed(glUnmapNamedBuffer(glBufferID));
			glBuffer->BufferMappedMemory = nullptr;
		}
		return IResult::Ok;
	}

GD_NAMESPACE_END
