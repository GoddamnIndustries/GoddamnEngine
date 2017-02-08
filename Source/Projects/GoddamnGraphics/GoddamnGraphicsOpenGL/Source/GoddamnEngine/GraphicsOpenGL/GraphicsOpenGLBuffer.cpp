// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/GraphicsOpenGL/GraphicsOpenGLBuffers.cpp
 * File contains Implementation for OpenGL 4.3 Implementation of the graphics interface: buffer objects.
 */
#include <GoddamnEngine/GraphicsOpenGL/GraphicsOpenGL.h>

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Buffers.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	/*!
	 * Creates a GPU buffer with specified parameters.
	 *
	 * @param gfxBufferCreationInfo Pointer to the buffer creation information.
	 * @param gfxBufferInitialData Initial data of the buffer. Can be null pointer for en empty buffer.
	 */
	GDAPI IGraphicsOpenGLBuffer::IGraphicsOpenGLBuffer(IGraphicsBufferCreationInfo const* const gfxBufferCreationInfo, CHandle const gfxBufferInitialData)
		: IGraphicsBuffer(gfxBufferCreationInfo), m_GLBufferID(0), m_GLBufferType(IGraphicsOpenGLBufferTypesTable[gfxBufferCreationInfo->gfxBufferType]), m_GLBufferMappedMemory(nullptr)
	{
#if GD_PLATFORM_DESKTOP
		GLbitfield glBufferFlags = 0;
		if (gfxBufferCreationInfo->gfxBufferFlags != 0)
		{
			glBufferFlags |= GL_DYNAMIC_STORAGE_BIT;
			if ((gfxBufferCreationInfo->gfxBufferFlags & IGRAPHICS_BUFFER_FLAGS_DYNAMIC_READ ) != 0) glBufferFlags |= GL_MAP_READ_BIT; 
			if ((gfxBufferCreationInfo->gfxBufferFlags & IGRAPHICS_BUFFER_FLAGS_DYNAMIC_WRITE) != 0) glBufferFlags |= GL_MAP_WRITE_BIT;
		}
#endif	// if GD_PLATFORM_DESKTOP

		glGenBuffers(1, &m_GLBufferID);
		glBindBuffer(m_GLBufferType, m_GLBufferID);

#if GD_PLATFORM_DESKTOP
		glBufferStorage(m_GLBufferType, gfxBufferCreationInfo->gfxBufferSize, gfxBufferInitialData, glBufferFlags);
#else	// if GD_PLATFORM_DESKTOP
		if (gfxBufferInitialData != nullptr)
		{
			// Copying the data to the buffer in traditional way.
			glBufferData(GLBufferType, gfxBufferCreationInfo->gfxBufferSize, gfxBufferInitialData, gfxBufferCreationInfo->gfxBufferFlags != 0 ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
		}
#endif	// if GD_PLATFORM_DESKTOP

		glBindBuffer(m_GLBufferType, 0);
	}

	/*!
	 * Deletes current GPU buffer.
	 */
	GDAPI IGraphicsOpenGLBuffer::~IGraphicsOpenGLBuffer()
	{
		Imm_Unmap();
		glBindBuffer(m_GLBufferType, 0);
		glDeleteBuffers(1, &m_GLBufferID);
	}

	/*!
	 * Maps all GPU's buffer data on the CPU memory.
	 *
	 * @param gfxMappedWOMemory Pointer to the write-only memory Handle.
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsOpenGLBuffer::Imm_MapForWriting(Handle* const gfxMappedWOMemory)
	{
		if (gfxMappedWOMemory == nullptr) return IResult::InvalidArguments;
		if (m_GLBufferMappedMemory == nullptr)
		{
			// Mapping the unmapped buffer in the write-only mode.
			glBindBuffer(m_GLBufferType, m_GLBufferID);
			m_GLBufferMappedMemory = glMapBuffer(m_GLBufferType, GL_WRITE_ONLY);
			glBindBuffer(m_GLBufferType, 0);
		}
		*gfxMappedWOMemory = m_GLBufferMappedMemory;
		return IResult::Ok;
	}

	/*!
	 * Maps all GPU's buffer data on the CPU memory.
	 *
	 * @param gfxMappedROMemory Pointer to the read-only memory Handle.
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsOpenGLBuffer::Imm_MapForReading(CHandle* const gfxMappedROMemory)
	{
		if (gfxMappedROMemory == nullptr) return IResult::InvalidArguments;
		if (m_GLBufferMappedMemory == nullptr)
		{
			// Mapping the unmapped buffer in the read-only mode.
			glBindBuffer(m_GLBufferType, m_GLBufferID);
			m_GLBufferMappedMemory = glMapBuffer(m_GLBufferType, GL_READ_ONLY);
			glBindBuffer(m_GLBufferType, 0);
		}
		*gfxMappedROMemory = m_GLBufferMappedMemory;
		return IResult::Ok;
	}

	/*!
	 * Unmaps the buffer from the CPU memory.
	 * @param gfxBuffer Buffer that would be unmapped.
	 */
	GDAPI void IGraphicsOpenGLBuffer::Imm_Unmap()
	{
		if (m_GLBufferMappedMemory != nullptr)
		{
			// Unmapping the mapped memory..
			glBindBuffer(m_GLBufferType, m_GLBufferID);
			glUnmapBuffer(m_GLBufferType);
			glBindBuffer(m_GLBufferType, 0);
			m_GLBufferMappedMemory = nullptr;
		}
	}

	/*!
	 * Creates a GPU buffer with specified parameters.
	 *
	 * @param gfxBufferPtr Pointer for output.
	 * @param gfxBufferCreationInfo Pointer to the buffer creation information.
	 * @param gfxBufferInitialData Initial data of the buffer. Can be null pointer for en empty buffer.
	 *
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsOpenGLWithBuffers::GfxImm_BufferCreate(IGraphicsBuffer** const gfxBufferPtr, IGraphicsBufferCreationInfo const* const gfxBufferCreationInfo
		, CHandle const gfxBufferInitialData)
	{
		if (gfxBufferPtr == nullptr) return IResult::InvalidArguments;
		if (gfxBufferCreationInfo == nullptr) return IResult::InvalidArguments;
		if (gfxBufferCreationInfo->gfxBufferType >= IGRAPHICS_BUFFER_TYPE_UNKNOWN) return IResult::InvalidArguments;

		*gfxBufferPtr = gd_new IGraphicsOpenGLBuffer(gfxBufferCreationInfo, gfxBufferInitialData);
		return IResult::Ok;
	}

GD_NAMESPACE_END
