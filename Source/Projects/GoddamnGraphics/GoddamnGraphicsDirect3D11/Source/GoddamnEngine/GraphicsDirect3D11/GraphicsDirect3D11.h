// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/System/Graphics/GraphicsOpenGL/GraphicsDirect3D11.h
 * File contains export interface for Direct3D 10 and 11 Implementation of the graphics interface.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Interface/Graphics/Graphics.h>
#if GD_PLATFORM_API_MICROSOFT

#include <atlbase.h>
#include <dxgi.h>
#include <d3d11.h>

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Command lists.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	uinterface IGraphicsDirect3D11CommandList final : public IGraphicsCommandList
	{
	private:
		friend uinterface IGraphicsDirect3D11;
		GDAPI explicit IGraphicsDirect3D11CommandList(...) {}
		GDAPI virtual ~IGraphicsDirect3D11CommandList() {}
	public:
		// No public interface functions here..
	};	// uinterface IGraphicsDirect3D11CommandList
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsDirect3D11WithCommandLists, IGraphicsDirect3D11)
	{
	public:
		//! @todo Implement command lists here.
	};	// uinterface IGraphicsBase_WithCommandLists

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Buffers.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	uinterface IGraphicsDirect3D11Buffer final : public IGraphicsBuffer
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsDirect3D11Buffer, IGraphicsBuffer);
	public:
		CComPtr<interface ID3D11(Buffer)> m_D3DBuffer;
#if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11			
		union {
			D3D11_MAPPED_SUBRESOURCE      m_D3DBufferMappedSubresource;
			Handle						  m_D3DBufferMappedMemory;
		};	// anonymous union
#else	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
		Handle						      m_D3DBufferMappedMemory;
#endif	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
	private:
		friend uinterface IGraphicsDirect3D11WithBuffers;
		GDAPI explicit IGraphicsDirect3D11Buffer(IGraphicsBufferCreationInfo const* const gfxBufferCreationInfo
			, CHandle const gfxBufferInitialData);
		GDAPI virtual ~IGraphicsDirect3D11Buffer();
	public:
		GDAPI virtual IResult Imm_MapForWriting(Handle* const gfxMappedWOMemory) override final;
		GDAPI virtual IResult Imm_MapForReading(CHandle* const gfxMappedROMemory) override final;
		GDAPI virtual void Imm_Unmap() override final;
	};	// uinterface IGraphicsDirect3D11Buffer
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsDirect3D11WithBuffers, IGraphicsDirect3D11)
	{
	public:
		GDAPI virtual IResult GfxImm_BufferCreate(IGraphicsBuffer** const gfxBufferPtr
			, IGraphicsBufferCreationInfo const* const gfxBufferCreationInfo, CHandle const gfxBufferInitialData) override final;
	};	// uinterface IGraphicsDirect3D11WithBuffers

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Vertex array objects.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	struct IGraphicsDirect3D11VertexArray final : public IGraphicsVertexArray
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsDirect3D11VertexArray, IGraphicsVertexArray);
	public:
		GLuint GLVertexArrayID;
	private:
		friend uinterface IGraphicsDirect3D11WithVertexArrays;
		GDAPI explicit IGraphicsDirect3D11VertexArray(IGraphicsVertexArrayCreationInfo const* const gfxVertexArrayCreationInfo);
		GDAPI virtual ~IGraphicsDirect3D11VertexArray();
	public:
		// No public interface functions here..
	};	// uinterface IGraphicsDirect3D11VertexArray
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsDirect3D11WithVertexArrays, IGraphicsDirect3D11)
	{
	public:
		GDAPI virtual IResult GfxImm_VertexArrayCreate(IGraphicsVertexArray** const gfxVertexArrayPtr
			, IGraphicsVertexArrayCreationInfo const* const gfxVertexArrayCreationInfo) override final;
	};	// uinterface IGraphicsDirect3D11WithVertexArrays

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Sampler state objects.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if GD_IGRAPHICS_DIRECT3D1X_IMPL
	D3D11_(TEXTURE_ADDRESS_MODE) static const IGraphicsDirect3D11TextureAddressModesTable[IGRAPHICS_TEXTURE_ADDRESS_MODES_COUNT] = {
		/* IGRAPHICS_TEXTURE_ADDRESS_MODE_CLAMP_TO_EDGE */ D3D11_(TEXTURE_ADDRESS_MIRROR_ONCE),
		/* IGRAPHICS_TEXTURE_ADDRESS_MODE_CLAMP_TO_ZERO */ D3D11_(TEXTURE_ADDRESS_CLAMP),
		/* IGRAPHICS_TEXTURE_ADDRESS_MODE_REPEAT        */ D3D11_(TEXTURE_ADDRESS_WRAP),
		/* IGRAPHICS_TEXTURE_ADDRESS_MODE_MIRROR        */ D3D11_(TEXTURE_ADDRESS_MIRROR),
		/* IGRAPHICS_TEXTURE_ADDRESS_MODE_UNKNOWN       */ D3D11_(TEXTURE_ADDRESS_MODE)(0),
	};
#endif	// if GD_IGRAPHICS_DIRECT3D1X_IMPL

	uinterface IGraphicsDirect3D11SamplerState final : public IGraphicsSamplerState
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsDirect3D11SamplerState, IGraphicsSamplerState);
	public:
		CComPtr<interface ID3D11(SamplerState)> m_D3DSamplerState;
	private:
		friend class Allocator;
		friend uinterface IGraphicsDirect3D11WithSamplerStates;
		GDAPI explicit IGraphicsDirect3D11SamplerState(IGraphicsSamplerStateCreationInfo const* const gfxSamplerStateCreationInfo);
		GDAPI virtual ~IGraphicsDirect3D11SamplerState();
	public:
		// No public interface functions here..
	};	// uinterface IGraphicsDirect3D11SamplerState;
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsDirect3D11WithSamplerStates, IGraphicsDirect3D11)
	{
	public:
		GDAPI virtual IResult GfxImm_SamplerStateCreate(IGraphicsSamplerState** const gfxSamplerStatePtr
			, IGraphicsSamplerStateCreationInfo const* const gfxSamplerStateCreationInfo) override final;
	};	// uinterface IGraphicsDirect3D11WithSamplerStates

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Shader Resources common.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! A GPU shader resource view interface.
	GD_DINTERFACE()
	uinterface IGraphicsDirect3D11ShaderResourceView : public IGraphicsShaderResourceView
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsDirect3D11ShaderResourceView, IGraphicsShaderResourceView);
	public:
		CComPtr<interface ID3D11(ShaderResourceView)> m_D3DShaderResourceView;
	private:
		friend class Allocator;
		friend uinterface IGraphicsDirect3D11WithSamplerStates;
		GDAPI explicit IGraphicsDirect3D11ShaderResourceView(IGraphicsResourceType const gfxResourceType);
		GDAPI virtual ~IGraphicsDirect3D11ShaderResourceView();
	public:
		// No public interface functions here..
	};	// uinterface IGraphicsShaderResourceView

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// 2D Textures.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	uinterface IGraphicsDirect3D11Texture2D final : public IGraphicsTexture2D
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsDirect3D11Texture2D, IGraphicsTexture2D);
	public:
		CComPtr<interface ID3D11(Texture2D)>            D3DTexture2D;
		DIRefPtr<IGraphicsDirect3D11ShaderResourceView> m_D3DShaderResourceView;
	private:
		friend class Allocator;
		friend uinterface IGraphicsBaseDirect3D11_WithTextures2D;
		GDAPI explicit IGraphicsDirect3D11Texture2D(IGraphicsTexture2DCreationInfo const* const gfxTexture2DCreationInfo, IGraphicsTexture2DData const* const gfxTexture2DInitialData);
		GDAPI virtual ~IGraphicsDirect3D11Texture2D();
	public:
		GDAPI virtual IGraphicsShaderResourceView const* CPU_GetShaderResourceView() const override final;
		GDAPI virtual IResult Imm_UpdateData(IGraphicsTexture2DData const* const gfxTexture2DData) override final;
	};	// uinterface IGraphicsDirect3D11Texture2D
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsBaseDirect3D11_WithTextures2D, IGraphicsDirect3D11)
	{
	public:
		GDAPI virtual IResult GfxImm_Texture2DCreate(IGraphicsTexture2D** const gfxTexture2DPtr
			, IGraphicsTexture2DCreationInfo const* const gfxTexture2DCreationInfo
			, IGraphicsTexture2DData const* const gfxTexture2DData) override final;
	};	// uinterface IGraphicsBaseDirect3D11_WithTextures2D
		
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Cube map textures.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	uinterface IGraphicsDirect3D11TextureCube final : public IGraphicsTextureCube
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsDirect3D11TextureCube, IGraphicsTextureCube);
	public:
		CComPtr<interface ID3D11(TextureCube)>          D3DTextureCube;
		DIRefPtr<IGraphicsDirect3D11ShaderResourceView> m_D3DShaderResourceView;
	private:
		friend class Allocator;
		friend uinterface IGraphicsBaseDirect3D11_WithTexturesCube;
		GDAPI explicit IGraphicsDirect3D11TextureCube(IGraphicsTextureCubeCreationInfo const* const gfxTextureCubeCreationInfo, IGraphicsTextureCubeData const* const gfxTextureCubeInitialData);
		GDAPI virtual ~IGraphicsDirect3D11TextureCube();
	public:
		GDAPI virtual IGraphicsShaderResourceView const* CPU_GetShaderResourceView() const override final;
		GDAPI virtual IResult Imm_UpdateData(IGraphicsTextureCubeData const* const gfxTextureCubeData) override final;
	};	// uinterface IGraphicsDirect3D11TextureCube
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsBaseDirect3D11_WithTexturesCube, IGraphicsDirect3D11)
	{
	public:
		GDAPI virtual IResult GfxImm_TextureCubeCreate(IGraphicsTextureCube** const gfxTextureCubePtr
			, IGraphicsTextureCubeCreationInfo const* const gfxTextureCubeCreationInfo
			, IGraphicsTextureCubeData const* const gfxTextureInitialCubeData) override final;
	};	// uinterface IGraphicsBaseDirect3D11_WithTexturesCube

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Base shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Vertex shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	uinterface IGraphicsDirect3D11VertexShader final : public IGraphicsVertexShader
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsDirect3D11VertexShader, IGraphicsVertexShader);
	public:
		CComPtr<interface ID3D11(VertexShader)> D3DVertexShader;
	private:
		friend uinterface IGraphicsBaseDirect3D11_WithBaseShaders;
		GDAPI explicit IGraphicsDirect3D11VertexShader(IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo);
		GDAPI virtual ~IGraphicsDirect3D11VertexShader() {}
	public:
		// No public interface functions here..
	};	// uinterface IGraphicsDirect3D11VertexShader
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsDirect3D11WithVertexShaders, IGraphicsDirect3D11)
	{
	public:
		GDAPI virtual IResult GfxImm_VertexShaderCreate(IGraphicsVertexShader** const gfxVertexShaderPtr
			, IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo
			, IGraphicsVertexArrayLayout const* const gfxVertexArrayLayout) override final;
		GDAPI virtual void GfxCmd_VertexShaderBindUniformBuffers(IGraphicsCommandList* const gfxCommandList
			, IGraphicsBuffer const* const* const gfxUniformBuffers, SizeTp const gfxUniformBuffersCount) override final;
		GDAPI virtual void GfxCmd_VertexShaderBindResources(IGraphicsCommandList* const gfxCommandList
			, IGraphicsShaderResourceView const* const* const gfxResources, SizeTp const gfxResourcesCount) override final;
		GDAPI virtual void GfxCmd_VertexShaderBindSamplers(IGraphicsCommandList* const gfxCommandList
			, IGraphicsSamplerState const* const* const gfxSamplerStates, SizeTp const gfxSamplerStatesCount) override final;
	};	// uinterface IGraphicsDirect3D11WithVertexShaders

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Pixel shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	uinterface IGraphicsDirect3D11PixelShader final : public IGraphicsPixelShader
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsDirect3D11PixelShader, IGraphicsPixelShader);
	public:
		CComPtr<interface ID3D11(PixelShader)> D3DPixelShader;
	private:
		friend class Allocator;
		friend uinterface IGraphicsBaseDirect3D11_WithBaseShaders;
		GDAPI explicit IGraphicsDirect3D11PixelShader(IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo);
		GDAPI virtual ~IGraphicsDirect3D11PixelShader();
	public:
		// No public interface functions here..
	};	// uinterface IGraphicsDirect3D11PixelShader
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsDirect3D11WithPixelShaders, IGraphicsDirect3D11)
	{
	public:
		GDAPI virtual IResult GfxImm_PixelShaderCreate(IGraphicsPixelShader** const gfxPixelShaderPtr
			, IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo) override final;
		GDAPI virtual void GfxCmd_PixelShaderBindUniformBuffers(IGraphicsCommandList* const gfxCommandList
			, IGraphicsBuffer const* const* const gfxUniformBuffers, SizeTp const gfxUniformBuffersCount) override final;
		GDAPI virtual void GfxCmd_PixelShaderBindResources(IGraphicsCommandList* const gfxCommandList
			, IGraphicsShaderResourceView const* const* const gfxResources, SizeTp const gfxResourcesCount) override final;
		GDAPI virtual void GfxCmd_PixelShaderBindSamplers(IGraphicsCommandList* const gfxCommandList
			, IGraphicsSamplerState const* const* const gfxSamplerStates, SizeTp const gfxSamplerStatesCount) override final;
	};	// uinterface IGraphicsDirect3D11WithPixelShaders

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Geometry shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	uinterface IGraphicsDirect3D11GeometryShader final : public IGraphicsGeometryShader
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsDirect3D11GeometryShader, IGraphicsGeometryShader);
	public:
		CComPtr<interface ID3D11(GeometryShader)> D3DGeometryShader;
	private:
		friend class Allocator;
		friend uinterface IGraphicsBaseDirect3D11_WithBaseShaders;
		GDAPI explicit IGraphicsDirect3D11GeometryShader(IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo);
		GDAPI virtual ~IGraphicsDirect3D11GeometryShader();
	public:
		// No public interface functions here..
	};	// uinterface IGraphicsDirect3D11GeometryShader
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsDirect3D11WithGeometryShaders, IGraphicsDirect3D11)
	{
	public:
		GDAPI virtual IResult GfxImm_GeometryShaderCreate(IGraphicsGeometryShader** const gfxGeometryShaderPtr
			, IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo) override final;
		GDAPI virtual void GfxCmd_GeometryShaderBindUniformBuffers(IGraphicsCommandList* const gfxCommandList
			, IGraphicsBuffer const* const* const gfxUniformBuffers, SizeTp const gfxUniformBuffersCount) override final;
		GDAPI virtual void GfxCmd_GeometryShaderBindResources(IGraphicsCommandList* const gfxCommandList
			, IGraphicsShaderResourceView const* const* const gfxResources, SizeTp const gfxResourcesCount) override final;
		GDAPI virtual void GfxCmd_GeometryShaderBindSamplers(IGraphicsCommandList* const gfxCommandList
			, IGraphicsSamplerState const* const* const gfxSamplerStates, SizeTp const gfxSamplerStatesCount) override final;
	};	// uinterface IGraphicsDirect3D11WithGeometryShaders

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Hull shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if GD_FALSE

	uinterface IGraphicsDirect3D11HullShader final : public IGraphicsHullShader
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsDirect3D11HullShader, IGraphicsHullShader);
	public:
#if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11			
		CComPtr<interface ID3D11(HullShader)> D3DHullShader;
#endif	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
	private:
		friend class Allocator;
		friend uinterface IGraphicsBaseDirect3D11_WithBaseShaders;
		GDAPI explicit IGraphicsDirect3D11HullShader(IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo);
		GDAPI virtual ~IGraphicsDirect3D11HullShader();
	public:
		// No public interface functions here..
	};	// uinterface IGraphicsDirect3D11HullShader
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsDirect3D11WithHullShaders, IGraphicsDirect3D11)
	{
	public:
		GDAPI virtual IResult GfxImm_HullShaderCreate(IGraphicsHullShader** const gfxHullShaderPtr
			, IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo) override final;
		GDAPI virtual void GfxCmd_HullShaderBindUniformBuffers(IGraphicsCommandList* const gfxCommandList
			, IGraphicsBuffer const* const* const gfxUniformBuffers, SizeTp const gfxUniformBuffersCount) override final;
		GDAPI virtual void GfxCmd_HullShaderBindResources(IGraphicsCommandList* const gfxCommandList
			, IGraphicsShaderResourceView const* const* const gfxResources, SizeTp const gfxResourcesCount) override final;
		GDAPI virtual void GfxCmd_HullShaderBindSamplers(IGraphicsCommandList* const gfxCommandList
			, IGraphicsSamplerState const* const* const gfxSamplerStates, SizeTp const gfxSamplerStatesCount) override final;
	};	// uinterface IGraphicsDirect3D11WithHullShaders

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Domain shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	uinterface IGraphicsDirect3D11DomainShader final : public IGraphicsDomainShader
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsDirect3D11DomainShader, IGraphicsDomainShader);
	public:
#if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11			
		CComPtr<interface ID3D11(DomainShader)> D3DDomainShader;
#endif	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
	private:
		friend class Allocator;
		friend uinterface IGraphicsBaseDirect3D11_WithBaseShaders;
		GDAPI explicit IGraphicsDirect3D11DomainShader(IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo);
		GDAPI virtual ~IGraphicsDirect3D11DomainShader();
	public:
		// No public interface functions here..
	};	// uinterface IGraphicsDirect3D11DomainShader
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsDirect3D11WithDomainShaders, IGraphicsDirect3D11)
	{
	public:
		GDAPI virtual IResult GfxImm_DomainShaderCreate(IGraphicsDomainShader** const gfxDomainShaderPtr
			, IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo) override final;
		GDAPI virtual void GfxCmd_DomainShaderBindUniformBuffers(IGraphicsCommandList* const gfxCommandList
			, IGraphicsBuffer const* const* const gfxUniformBuffers, SizeTp const gfxUniformBuffersCount) override final;
		GDAPI virtual void GfxCmd_DomainShaderBindResources(IGraphicsCommandList* const gfxCommandList
			, IGraphicsShaderResourceView const* const* const gfxResources, SizeTp const gfxResourcesCount) override final;
		GDAPI virtual void GfxCmd_DomainShaderBindSamplers(IGraphicsCommandList* const gfxCommandList
			, IGraphicsSamplerState const* const* const gfxSamplerStates, SizeTp const gfxSamplerStatesCount) override final;
	};	// uinterface IGraphicsDirect3D11WithDomainShaders

#endif	// if GD_FALSE

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Compute shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if GD_FALSE

	uinterface IGraphicsDirect3D11ComputeShader final : public IGraphicsComputeShader
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsDirect3D11ComputeShader, IGraphicsComputeShader);
	public:
#if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11			
		CComPtr<interface ID3D11(ComputeShader)> D3DComputeShader;
#endif	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
	private:
		friend class Allocator;
		friend uinterface IGraphicsBaseDirect3D11_WithBaseShaders;
		GDAPI explicit IGraphicsDirect3D11ComputeShader(IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo);
		GDAPI virtual ~IGraphicsDirect3D11ComputeShader();
	public:
		// No public interface functions here..
	};	// uinterface IGraphicsDirect3D11ComputeShader
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsDirect3D11WithComputeShaders, IGraphicsDirect3D11)
	{
	public:
		//! @todo Provide functionality for compute shaders.
	};	// uinterface IGraphicsDirect3D11WithComputeShaders

#endif	// if GD_FALSE

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// PPipeline state object.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GD_DINTERFACE()
	uinterface IGraphicsDirect3D11PipelineState : public IGraphicsPipelineState
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsDirect3D11PipelineState, IGraphicsPipelineState);
	public:
		GLuint GLProgramPipelineID;
	private:
		friend class Allocator;
		friend uinterface IGraphicsDirect3D11WithPipelineStates;
		GDAPI explicit IGraphicsDirect3D11PipelineState(IGraphicsPipelineStateCreationInfo const* const gfxPipelineStateCreationInfo);
		GDAPI virtual ~IGraphicsDirect3D11PipelineState();
	public:
		// No public interface functions here..
	};	// uinterface IGraphicsDirect3D11PipelineState
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsDirect3D11WithPipelineStates, IGraphicsDirect3D11)
	{
	public:
		GDAPI virtual IResult GfxImm_PipelineCreate(IGraphicsPipelineState** const gfxPipelinePtr
			, IGraphicsPipelineStateCreationInfo const* const gfxPipelineCreationInfo) override final;
	};	// uinterface IGraphicsDirect3D11PipelineState

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Global interface.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsDirect3D11, IGraphicsDirect3D11)
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsDirect3D11, IGraphics);

	public:
		CComPtr<interface IDXGIFactory>				m_D3DDXGIFactory;
#if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11			// Direct3D 10 has no separation for Device and DeviceContext interfaces.
		CComPtr<interface ID3D11(Device)>			m_D3DDevice;
		CComPtr<interface ID3D11(DeviceContext)>	m_D3DDeviceContext;
#else	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
		CComPtr<interface ID3D10Device>				m_D3DDevice;
		ID3D10Device*								m_D3DDeviceContext;	
#endif	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11

	};	// uinterface IGraphicsOpenGL

		//! Reference to the global Direct2D interface.
#if GD_IGRAPHICS_DIRECT3D1X_IMPL
#	define GD_IGRAPHICS_DIRECT3D1X static_cast<GD::IGraphicsDirect3D11*>(GD::Graphics)
#endif	// if GD_IGRAPHICS_DIRECT3D1X_IMPL

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_API_MICROSOFT
