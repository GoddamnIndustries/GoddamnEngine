// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/GraphicsOpenGL/GraphicsOpenGLBuffers.cpp
 * File contains implementation for OpenGL(ES) graphics interface: buffer objects.
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
	GDAPI IResult IGraphicsOpenGLWithBuffers::GfxImm_BufferCreate(IGraphicsBuffer*& gfxBufferPtr
		, IGraphicsBufferCreateInfo const& gfxBufferCreateInfo)
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
#if GD_DEBUG
			default:
				GD_DEBUG_VERIFY(gfxBufferCreateInfo.BufferType >= IGRAPHICS_BUFFER_TYPE_UNKNOWN);
				return IResult::InvalidArguments;
#endif	// if GD_DEBUG
		}

		// Setting up the usage..
		GLbitfield glBufferFlags = 0;
		switch (gfxBufferCreateInfo.ResourceUsage)
		{
			case IGRAPHICS_RESOURCE_USAGE_DEFAULT:
			case IGRAPHICS_RESOURCE_USAGE_IMMUTABLE:
#if GD_DEBUG
				if (gfxBufferCreateInfo.ResourceCpuAccessFlags != IGRAPHICS_RESOURCE_CPU_ACCESS_FLAGS_NONE)
				{
					return IResult::InvalidArguments;
				}
#endif	// if GD_DEBUG
				break;

			case IGRAPHICS_RESOURCE_USAGE_DYNAMIC:
				if ((gfxBufferCreateInfo.ResourceCpuAccessFlags & IGRAPHICS_RESOURCE_CPU_ACCESS_FLAGS_WRITE) != 0)
				{
					glBufferFlags |= GL_MAP_WRITE_BIT;
					glBufferFlags |= GL_MAP_PERSISTENT_BIT;
				}
#if GD_DEBUG
				if ((gfxBufferCreateInfo.ResourceCpuAccessFlags & IGRAPHICS_RESOURCE_CPU_ACCESS_FLAGS_READ) != 0)
				{
					return IResult::InvalidArguments;
				}
#endif	// if GD_DEBUG
				break;

			case IGRAPHICS_RESOURCE_USAGE_STAGING:
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
				break;

			default:
#if GD_DEBUG
				GD_DEBUG_VERIFY(gfxBufferCreateInfo.ResourceUsage >= IGRAPHICS_RESOURCE_USAGE_UNKNOWN);
				return IResult::InvalidArguments;
#endif	// if GD_DEBUG
		}

		// Creating the buffer..
		GLuint glBufferID = 0;
		glThrowIfFailed(glCreateBuffers(1, &glBufferID));

		// Allocating and passing the initial data..
		GLsizeiptr const glBufferSize = gfxBufferCreateInfo.BufferSize;
		glThrowIfFailed(glNamedBufferStorage(glBufferID, glBufferSize, nullptr, glBufferFlags));

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
		auto const glBufferID = glBuffer->m_GLBufferID;
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
	GDAPI IResult IGraphicsOpenGLWithBuffers::GfxImm_BufferMap(IGraphicsBuffer* const gfxBuffer
		, IGraphicsResourceMapMode const gfxMapMode, Handle& gfxMappedMemory)
	{
		GD_DEBUG_VERIFY(gfxBuffer != nullptr);

		auto const glBuffer = static_cast<IGraphicsOpenGLBuffer*>(gfxBuffer);
		auto const glBufferID = glBuffer->m_GLBufferID;
		if (glBuffer->m_GLBufferMappedMemory == nullptr)
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
#if GD_DEBUG
				default:
					GD_DEBUG_VERIFY(gfxMapMode >= IGRAPHICS_RESOURCE_MAP_MODE_UNKNOWN);
					return IResult::InvalidArguments;
#endif	// if GD_DEBUG
			}
			glBuffer->m_GLBufferMappedMemory = glMapNamedBuffer(glBufferID, glMapMode);
			glThrowIfFailed();
		}
		gfxMappedMemory = glBuffer->m_GLBufferMappedMemory;
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
		auto const glBufferID = glBuffer->m_GLBufferID;
		if (glBuffer->m_GLBufferMappedMemory != nullptr)
		{
			// Unmapping the mapped memory..
			glThrowIfFailed(glUnmapNamedBuffer(glBufferID));
			glBuffer->m_GLBufferMappedMemory = nullptr;
		}
		return IResult::Ok;
	}

GD_NAMESPACE_END
