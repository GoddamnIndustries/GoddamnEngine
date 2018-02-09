// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file
 * File contains export interface for Direct3D 10 and 11 Implementation of the graphics interface.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Interface/Graphics/Graphics.h>
#if GD_PLATFORM_API_MICROSOFT

#include <atlbase.h>
#include <dxgi.h>
#include <d3d11.h>

#define D3DThrowIfFailed ThrowIfFailed

GD_NAMESPACE_BEGIN

	GDEXP extern IGraphics* CreateIGraphicsInstance();
	GD_DEFINE_PARTIAL_CLASS_FIRST(IGraphicsDirect3D11, IGraphics);

	GD_DEFINE_PARTIAL_CLASS(IGraphicsDirect3D11WithDevice, IGraphicsDirect3D11)
	{
	public:
		CComPtr<ID3D11Device> m_D3DDevice;
		CComPtr<ID3D11DeviceContext> m_D3DDeviceContext;
	};

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                           Graphics Resource interface.                           ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	// ------------------------------------------------------------------------------------------
	// Buffers.
	// ------------------------------------------------------------------------------------------

	class IGraphicsDirect3DBuffer final : public IGraphicsBuffer
	{
	public:
		CComPtr<ID3D11Buffer> BufferID;
	public:
		GDINL explicit IGraphicsDirect3DBuffer(IGraphicsBufferCreateInfo const& gfxBufferCreateInfo, CComPtr<ID3D11Buffer> const& d3dBufferID)
			: IGraphicsBuffer(gfxBufferCreateInfo), BufferID(d3dBufferID) {}
	};	// class IGraphicsDirect3DBuffer

	GD_DEFINE_PARTIAL_CLASS(IGraphicsDirect3D11WithBuffers, IGraphicsDirect3D11)
	{
	public:
		GDAPI virtual IResult GfxImm_BufferCreate(IGraphicsBuffer*& gfxBufferPtr, IGraphicsBufferCreateInfo const& gfxBufferCreateInfo, CHandle const gfxObjInitialData) override final;
		GDAPI virtual IResult GfxImm_BufferDestroy(IGraphicsBuffer* const gfxBufferPtr) override final;
		GDAPI virtual IResult GfxImm_BufferMap(IGraphicsBuffer* const gfxBuffer, IGraphicsResourceMapMode const gfxMapMode, Handle& gfxMappedMemory) override final;
		GDAPI virtual IResult GfxImm_BufferUnmap(IGraphicsBuffer* const gfxBuffer) override final;
	};	// class IGraphicsDirect3D11WithBuffers

	// ------------------------------------------------------------------------------------------
	// 1D, 2D, 3D Texture Arrays and Cube textures.
	// ------------------------------------------------------------------------------------------

	class IGraphicsDirect3DTexture2D final : public IGraphicsTexture2D
	{
	public:
		CComPtr<ID3D11Texture2D> Texture2DID;
	public:
		GDINL explicit IGraphicsDirect3DTexture2D(IGraphicsTexture2DCreateInfo const& gfxTexture2DCreateInfo, CComPtr<ID3D11Texture2D> const& d3dTexture2DID)
			: IGraphicsTexture2D(gfxTexture2DCreateInfo), Texture2DID(d3dTexture2DID) {}
	};	// class IGraphicsDirect3DTexture2D

	class IGraphicsDirect3DTexture3D final : public IGraphicsTexture3D
	{
	public:
		CComPtr<ID3D11Texture3D> Texture3DID;
	public:
		GDINL explicit IGraphicsDirect3DTexture3D(IGraphicsTexture3DCreateInfo const& gfxTexture3DCreateInfo, CComPtr<ID3D11Texture3D> const& d3dTexture3DID)
			: IGraphicsTexture3D(gfxTexture3DCreateInfo), Texture3DID(d3dTexture3DID) {}
	};	// class IGraphicsDirect3DTexture3D

	GD_DEFINE_PARTIAL_CLASS(IGraphicsDirect3D11WithTextures, IGraphicsDirect3D11)
	{
	public:
		GDAPI virtual IResult GfxImm_Texture2DCreate(IGraphicsTexture2D*& gfxObjPtr, IGraphicsTexture2DCreateInfo const& gfxObjCreateInfo, IGraphicsResourceData const& gfxObjInitialData) override final;
		GDAPI virtual IResult GfxImm_Texture2DDestroy(IGraphicsTexture2D* const gfxObj) override final;
		GDAPI virtual IResult GfxImm_Texture2DMap(IGraphicsTexture2D* const gfxObj, IGraphicsResourceMapMode const gfxMapMode, IGraphicsResourceData& gfxMappedMemory) override final;
		GDAPI virtual IResult GfxImm_Texture2DUnmap(IGraphicsTexture2D* const gfxObj) override final;

		GDAPI virtual IResult GfxImm_Texture3DCreate(IGraphicsTexture3D*& gfxObjPtr, IGraphicsTexture3DCreateInfo const& gfxObjCreateInfo, IGraphicsResourceData const& gfxObjInitialData) override final;
		GDAPI virtual IResult GfxImm_Texture3DDestroy(IGraphicsTexture3D* const gfxObj) override final;
		GDAPI virtual IResult GfxImm_Texture3DMap(IGraphicsTexture3D* const gfxObj, IGraphicsResourceMapMode const gfxMapMode, IGraphicsResourceData& gfxMappedMemory) override final;
		GDAPI virtual IResult GfxImm_Texture3DUnmap(IGraphicsTexture3D* const gfxObj) override final;
	};	// class IGraphicsDirect3D11WithTextures

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_API_MICROSOFT
