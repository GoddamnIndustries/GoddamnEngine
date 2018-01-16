// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#define GD_IGRAPHICS_DIRECT3D1X_IMPL 1

/*!
 * @file GoddamnEngine/Core/System/Graphics/GraphicsDirect3D1x/GraphicsDirect3D1xBuffers.cpp
 * File contains Implementation for Direct3D1x Implementation of the graphics interface: buffer objects.
 */
#include <GoddamnEngine/GraphicsDirect3D1x/GraphicsDirect3D1x.h>
#if GD_PLATFORM_API_MICROSOFT

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Buffers.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	D3D1x_(BIND_FLAG) static const g_D3DBufferTypesTable[IGRAPHICS_BUFFER_TYPES_COUNT] = {
		/* IGRAPHICS_BUFFER_TYPE_VERTEX   */ D3D1x_(BIND_VERTEX_BUFFER),
		/* IGRAPHICS_BUFFER_TYPE_INDEX    */ D3D1x_(BIND_INDEX_BUFFER),
		/* IGRAPHICS_BUFFER_TYPE_UNIFORM  */ D3D1x_(BIND_CONSTANT_BUFFER),
		/* IGRAPHICS_BUFFER_TYPE_UNKNOWN  */ D3D1x_(BIND_FLAG)(0),
	};

#if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11			
	static_assert(&static_cast<D3D11_MAPPED_SUBRESOURCE*>(nullptr)->pData == nullptr
			, "The 'pData' field of the mapped subresource has been moved.");
#endif	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11

	/*!
	 * Creates a GPU buffer with specified parameters.
	 *
	 * @param gfxBufferCreationInfo Pointer to the buffer creation information.
	 * @param gfxBufferInitialData Initial data of the buffer. Can be null pointer for en empty buffer.
	 */
	GDAPI IGraphicsDirect3D1xBuffer::IGraphicsDirect3D1xBuffer(IGraphicsBufferCreationInfo const* const gfxBufferCreationInfo
		, CHandle const gfxBufferInitialData)
		: IGraphicsBuffer(gfxBufferCreationInfo)
		, m_D3DBuffer(nullptr), m_D3DBufferMappedMemory(nullptr)
	{
		D3D1x_(BUFFER_DESC) d3dBufferDesc = {};
		d3dBufferDesc.ByteWidth = static_cast<UINT>(BufferSize);
		d3dBufferDesc.BindFlags = g_D3DBufferTypesTable[BufferType];
		if (BufferFlags != 0)
		{
			d3dBufferDesc.Usage = D3D1x_(USAGE_IMMUTABLE);
			if ((BufferFlags & IGRAPHICS_BUFFER_FLAGS_DYNAMIC_READ ) != 0) d3dBufferDesc.CPUAccessFlags |= D3D1x_(CPU_ACCESS_READ);
			if ((BufferFlags & IGRAPHICS_BUFFER_FLAGS_DYNAMIC_WRITE) != 0) d3dBufferDesc.CPUAccessFlags |= D3D1x_(CPU_ACCESS_WRITE);
		}

		D3D1x_(SUBRESOURCE_DATA) d3dBufferSubresource = {}, *d3dBufferSubresourcePtr = nullptr;
		if (gfxBufferInitialData != nullptr)
		{
			d3dBufferSubresourcePtr = &d3dBufferSubresource;
			d3dBufferSubresource.pSysMem = gfxBufferInitialData;
		}

		ThrowIfFailed(GD_IGRAPHICS_DIRECT3D1X->m_D3DDevice->CreateBuffer(&d3dBufferDesc, d3dBufferSubresourcePtr, &m_D3DBuffer.p));
	}

	/*!
	 * Deletes current GPU buffer.
	 */
	GDAPI IGraphicsDirect3D1xBuffer::~IGraphicsDirect3D1xBuffer()
	{
		Imm_Unmap();
		m_D3DBuffer.Release();
	}

	/*!
	 * Maps all GPU's buffer data on the CPU memory.
	 *
	 * @param gfxMappedWOMemory Pointer to the write-only memory Handle.
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsDirect3D1xBuffer::Imm_MapForWriting(Handle* const gfxMappedWOMemory)
	{
#if GD_DEBUG
		GD_ARG_VERIFY(gfxMappedWOMemory != nullptr);
#endif	// if GD_DEBUG
		if (m_D3DBufferMappedMemory == nullptr)
		{
#if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11			
			ThrowIfFailed(GD_IGRAPHICS_DIRECT3D1X->m_D3DDeviceContext->Map(m_D3DBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0
				, &m_D3DBufferMappedSubresource));
#else	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
			ThrowIfFailed(m_D3DBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0
				, const_cast<void**>(&m_D3DBufferMappedMemory)));
#endif	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
		}
		*gfxMappedWOMemory = m_D3DBufferMappedMemory;
		return IResult::Ok;
	}

	/*!
	 * Maps all GPU's buffer data on the CPU memory.
	 *
	 * @param gfxMappedROMemory Pointer to the read-only memory Handle.
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsDirect3D1xBuffer::Imm_MapForReading(CHandle* const gfxMappedROMemory)
	{
#if GD_DEBUG
		GD_ARG_VERIFY(gfxMappedROMemory != nullptr);
#endif	// if GD_DEBUG
		if (m_D3DBufferMappedMemory == nullptr)
		{
#if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11			
			ThrowIfFailed(GD_IGRAPHICS_DIRECT3D1X->m_D3DDeviceContext->Map(m_D3DBuffer, 0, D3D11_MAP_READ, 0
				, &m_D3DBufferMappedSubresource));
#else	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
			ThrowIfFailed(m_D3DBuffer->Map(D3D10_MAP_READ, 0
				, const_cast<void**>(&m_D3DBufferMappedMemory)));
#endif	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
		}
		*gfxMappedROMemory = m_D3DBufferMappedMemory;
		return IResult::Ok;
	}

	/*!
	 * Unmaps the buffer from the CPU memory.
	 */
	GDAPI void IGraphicsDirect3D1xBuffer::Imm_Unmap()
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(Imm_Unmap(gfxBuffer)))
			return;
#endif	// if GD_DEBUG

		if (m_D3DBufferMappedMemory != nullptr)
		{
#if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11			
			GD_IGRAPHICS_DIRECT3D1X->m_D3DDeviceContext->Unmap(m_D3DBuffer, 0);
			m_D3DBufferMappedSubresource = {};
#else	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
			m_D3DBuffer->Unmap();
			m_D3DBufferMappedMemory = nullptr;
#endif	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
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
	GDAPI IResult IGraphicsDirect3D1xWithBuffers::GfxImm_BufferCreate(IGraphicsBuffer** const gfxBufferPtr
		, IGraphicsBufferCreationInfo const* const gfxBufferCreationInfo, CHandle const gfxBufferInitialData)
	{
#if GD_DEBUG
		GD_ARG_VERIFY(gfxBufferPtr != nullptr);
		GD_ARG_VERIFY(gfxBufferCreationInfo != nullptr);
		GD_ARG_VERIFY(gfxBufferCreationInfo->BufferType < IGRAPHICS_BUFFER_TYPE_UNKNOWN);
#endif	// if GD_DEBUG

		*gfxBufferPtr = gd_new IGraphicsDirect3D1xBuffer(gfxBufferCreationInfo, gfxBufferInitialData);
		return IResult::Ok;
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_API_MICROSOFT
