// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file
 * Direct3D11 Implementation of the graphics interface: buffer objects.
 */
#include <GoddamnEngine/GraphicsDirect3D11/GraphicsDirect3D11.h>
#if GD_PLATFORM_API_MICROSOFT

GD_NAMESPACE_BEGIN

	GDEXP extern IGraphics* CreateIGraphicsInstance()
	{
#pragma comment(lib, "d3d11.lib")
		auto a = gd_new IGraphicsDirect3D11WithTextures();
		ThrowIfFailed(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &a->m_D3DDevice.p, nullptr, &a->m_D3DDeviceContext.p));
		return a;
	}

	// ------------------------------------------------------------------------------------------
	// Buffers.
	// ------------------------------------------------------------------------------------------

	/*!
	 * Creates a GPU buffer with specified parameters.
	 *
	 * @param gfxBufferPtr Pointer for output.
	 * @param gfxBufferCreateInfo Pointer to the buffer creation information.
	 * @param gfxBufferInitialData Initial data.
	 *
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsDirect3D11WithBuffers::GfxImm_BufferCreate(IGraphicsBuffer*& gfxBufferPtr, IGraphicsBufferCreateInfo const& gfxBufferCreateInfo, CHandle const gfxBufferInitialData)
	{
		D3D11_BUFFER_DESC d3dBufferDesc = {};
		d3dBufferDesc.ByteWidth = gfxBufferCreateInfo.BufferSize;

		// Setting up the buffer type..
		switch (gfxBufferCreateInfo.BufferType)
		{
			case IGRAPHICS_BUFFER_TYPE_VERTEX:
				d3dBufferDesc.BindFlags |= D3D11_BIND_VERTEX_BUFFER;
				break;
			case IGRAPHICS_BUFFER_TYPE_INDEX:
				d3dBufferDesc.BindFlags |= D3D11_BIND_INDEX_BUFFER;
				break;
			case IGRAPHICS_BUFFER_TYPE_UNIFORM:
				d3dBufferDesc.BindFlags |= D3D11_BIND_CONSTANT_BUFFER | D3D11_BIND_SHADER_RESOURCE;
				break;
			default:
				return IResult::InvalidArguments;
		}

		// Setting up the usage..
		switch (gfxBufferCreateInfo.ResourceUsage)
		{
			case IGRAPHICS_RESOURCE_USAGE_DEFAULT:
				d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
				break;
			case IGRAPHICS_RESOURCE_USAGE_IMMUTABLE:
				d3dBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
				break;
			case IGRAPHICS_RESOURCE_USAGE_DYNAMIC:
				d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
				break;
			case IGRAPHICS_RESOURCE_USAGE_STAGING:
				d3dBufferDesc.Usage = D3D11_USAGE_STAGING;
				break;
			default:
				return IResult::InvalidArguments;
		}

		// Setting up the CPU access flags..
		if ((gfxBufferCreateInfo.ResourceCpuAccessFlags & IGRAPHICS_RESOURCE_CPU_ACCESS_FLAGS_WRITE) != 0)
		{
			d3dBufferDesc.CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;
		}
		if ((gfxBufferCreateInfo.ResourceCpuAccessFlags & IGRAPHICS_RESOURCE_CPU_ACCESS_FLAGS_READ) != 0)
		{
			d3dBufferDesc.CPUAccessFlags |= D3D11_CPU_ACCESS_READ;
		}

		// Setting up the initial data..
		D3D11_SUBRESOURCE_DATA d3dInitialData;
		d3dInitialData.pSysMem = gfxBufferInitialData;

		// Creating the buffer..
		CComPtr<ID3D11Buffer> d3dBufferID;
		D3DThrowIfFailed(m_D3DDevice->CreateBuffer(&d3dBufferDesc, &d3dInitialData, &d3dBufferID.p));

		gfxBufferPtr = gd_new IGraphicsDirect3DBuffer(gfxBufferCreateInfo, d3dBufferID);
		return IResult::Ok;
	}

	/*!
	 * Destroys the GPU buffer.
	 *
	 * @param gfxBuffer Buffer to destroy.
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsDirect3D11WithBuffers::GfxImm_BufferDestroy(IGraphicsBuffer* const gfxBuffer)
	{
		GD_DEBUG_VERIFY(gfxBuffer != nullptr);

		gd_delete gfxBuffer;
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
	GDAPI IResult IGraphicsDirect3D11WithBuffers::GfxImm_BufferMap(IGraphicsBuffer* const gfxBuffer, IGraphicsResourceMapMode const gfxMapMode, Handle& gfxMappedMemory)
	{
		GD_DEBUG_VERIFY(gfxBuffer != nullptr);

		auto const d3dBuffer = static_cast<IGraphicsDirect3DBuffer*>(gfxBuffer);
		auto const d3dBufferID = d3dBuffer->BufferID;

		D3D11_MAP d3dMapMode;
		switch (gfxMapMode)
		{
			case IGRAPHICS_RESOURCE_MAP_MODE_READ:
				d3dMapMode = D3D11_MAP_READ;
				break;
			case IGRAPHICS_RESOURCE_MAP_MODE_WRITE:
				d3dMapMode = D3D11_MAP_WRITE;
				break;
			case IGRAPHICS_RESOURCE_MAP_MODE_READ_WRITE:
				d3dMapMode = D3D11_MAP_READ_WRITE;
				break;
#if GD_DEBUG
			default:
				GD_DEBUG_VERIFY(gfxMapMode >= IGRAPHICS_RESOURCE_MAP_MODE_UNKNOWN);
				return IResult::InvalidArguments;
#endif	// if GD_DEBUG
		}

		D3D11_MAPPED_SUBRESOURCE d3dMappedMemory;
		D3DThrowIfFailed(m_D3DDeviceContext->Map(d3dBufferID, 0, d3dMapMode, 0, &d3dMappedMemory));

		gfxMappedMemory = d3dMappedMemory.pData;
		return IResult::Ok;
	}

	/*!
	 * Unmaps the buffer from the CPU memory.
	 *
	 * @param gfxBuffer Buffer to unmap.
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsDirect3D11WithBuffers::GfxImm_BufferUnmap(IGraphicsBuffer* const gfxBuffer)
	{
		GD_DEBUG_VERIFY(gfxBuffer != nullptr);

		auto const d3dBuffer = static_cast<IGraphicsDirect3DBuffer*>(gfxBuffer);
		auto const d3dBufferID = d3dBuffer->BufferID;

		m_D3DDeviceContext->Unmap(d3dBufferID, 0);
		return IResult::Ok;
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_API_MICROSOFT
