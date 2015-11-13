// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Engine/Interface/Graphics/GraphicsOpenGL/GraphicsDirect3D1x.h
 * File contains export interface for Direct3D 10 and 11 Implementation of the graphics interface.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Interface/Graphics/Graphics.h>
#if GD_PLATFORM_API_MICROSOFT

#if !defined(GD_IGRAPHICS_DIRECT3D1X_IMPL)
#	define GD_IGRAPHICS_DIRECT3D1X_IMPL GD_FALSE
#endif	// if !defined(GD_IGRAPHICS_DIRECT3D1X_IMPL)

#if GD_IGRAPHICS_DIRECT3D1X_IMPL
#if !defined(GD_IGRAPHICS_DIRECT3D1X_VERSION)
#		if GD_PLATFORM_DESKTOP
#			define GD_IGRAPHICS_DIRECT3D1X_VERSION 11
#		else	// if GD_PLATFORM_DESKTOP
#			define GD_IGRAPHICS_DIRECT3D1X_VERSION 11
#		endif	// if GD_PLATFORM_DESKTOP
#	endif	// if !defined(GD_IGRAPHICS_DIRECT3D1X_VERSION)
#else	// if GD_IGRAPHICS_DIRECT3D1X_IMPL
#		define GD_IGRAPHICS_DIRECT3D1X_VERSION 0
#endif	// if GD_IGRAPHICS_DIRECT3D1X_IMPL

#include <atlbase.h>
#if GD_IGRAPHICS_DIRECT3D1X_IMPL
#	include <dxgi.h>
#	if GD_IGRAPHICS_DIRECT3D1X_VERSION == 10
#		include <d3d10.h>
#	endif	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
#	if GD_IGRAPHICS_DIRECT3D1X_VERSION == 11
#		include <d3d11.h>
#	endif	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
#endif	// if GD_IGRAPHICS_DIRECT3D1X_IMPL

#define GD_IGRAPHICS_DIRECT3D1X_INTERFACE(Interface) GD_GLUE(GD_GLUE(ID3D, GD_IGRAPHICS_DIRECT3D1X_VERSION), Interface)
#define GD_IGRAPHICS_DIRECT3D1X_CONSTANT(Constant) GD_GLUE(GD_GLUE(GD_GLUE(D3D, GD_IGRAPHICS_DIRECT3D1X_VERSION), _), Constant)
#define ID3D1x(Interface) GD_IGRAPHICS_DIRECT3D1X_INTERFACE(Interface)
#define D3D1x_(Interface) GD_IGRAPHICS_DIRECT3D1X_CONSTANT(Interface)
#include <GL/glew.h>

GD_NAMESPACE_BEGIN

	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL_BASE(IGraphicsDirect3D1x, IGraphicsPlatform);

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Command lists.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	uinterface IGraphicsDirect3D1xCommandList final : public IGraphicsCommandList
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsDirect3D1xCommandList, IGraphicsCommandList);
	private:
		friend uinterface IGraphicsDirect3D1x;
		GDAPI explicit IGraphicsDirect3D1xCommandList(...) {}
		GDAPI virtual ~IGraphicsDirect3D1xCommandList() {}
	public:
		// No public interface functions here..
	};	// uinterface IGraphicsDirect3D1xCommandList
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsDirect3D1xWithCommandLists, IGraphicsDirect3D1x)
	{
	public:
		//! @todo Implement command lists here.
	};	// uinterface IGraphicsBase_WithCommandLists

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Buffers.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if GD_IGRAPHICS_DIRECT3D1X_IMPL
	D3D1x_(BIND_FLAG) static const IGraphicsDirect3D1xBufferTypesTable[IGRAPHICS_BUFFER_TYPES_COUNT] = {
		/* IGRAPHICS_BUFFER_TYPE_VERTEX   */ D3D1x_(BIND_VERTEX_BUFFER),
		/* IGRAPHICS_BUFFER_TYPE_INDEX    */ D3D1x_(BIND_INDEX_BUFFER),
		/* IGRAPHICS_BUFFER_TYPE_UNIFORM  */ D3D1x_(BIND_CONSTANT_BUFFER),
		/* IGRAPHICS_BUFFER_TYPE_UNKNOWN  */ D3D1x_(BIND_FLAG)(0),
	};
#endif	// if GD_IGRAPHICS_DIRECT3D1X_IMPL

	uinterface IGraphicsDirect3D1xBuffer final : public IGraphicsBuffer
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsDirect3D1xBuffer, IGraphicsBuffer);
	public:
		CComPtr<interface ID3D1x(Buffer)> D3DBuffer;
#if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11			
		union {
			static_assert(&static_cast<D3D11_MAPPED_SUBRESOURCE*>(nullptr)->pData == nullptr
				, "The 'pData' field of the mapped subresource has been moved.");
			D3D11_MAPPED_SUBRESOURCE      D3DBufferMappedSubresource;
			Handle						  D3DBufferMappedMemory;
		};	// anonymous union
#else	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
		Handle						      D3DBufferMappedMemory;
#endif	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
	private:
		friend class Allocator;
		friend uinterface IGraphicsDirect3D1xWithBuffers;
		GDAPI explicit IGraphicsDirect3D1xBuffer(IGraphicsBufferCreationInfo const* const gfxBufferCreationInfo, CHandle const gfxBufferInitialData);
		GDAPI virtual ~IGraphicsDirect3D1xBuffer();
	public:
		GDAPI virtual IResult Imm_MapForWriting(Handle* const gfxMappedWOMemory) override final;
		GDAPI virtual IResult Imm_MapForReading(CHandle* const gfxMappedROMemory) override final;
		GDAPI virtual void Imm_Unmap() override final;
	};	// uinterface IGraphicsDirect3D1xBuffer
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsDirect3D1xWithBuffers, IGraphicsDirect3D1x)
	{
	public:
		GDAPI virtual IResult GfxImm_BufferCreate(IGraphicsBuffer** const gfxBufferPtr
			, IGraphicsBufferCreationInfo const* const gfxBufferCreationInfo, CHandle const gfxBufferInitialData) override final;
	};	// uinterface IGraphicsDirect3D1xWithBuffers

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Vertex array objects.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	struct IGraphicsDirect3D1xVertexArray final : public IGraphicsVertexArray
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsDirect3D1xVertexArray, IGraphicsVertexArray);
	public:
		GLuint GLVertexArrayID;
	private:
		friend class Allocator;
		friend uinterface IGraphicsDirect3D1xWithVertexArrays;
		GDAPI explicit IGraphicsDirect3D1xVertexArray(IGraphicsVertexArrayCreationInfo const* const gfxVertexArrayCreationInfo);
		GDAPI virtual ~IGraphicsDirect3D1xVertexArray();
	public:
		// No public interface functions here..
	};	// uinterface IGraphicsDirect3D1xVertexArray
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsDirect3D1xWithVertexArrays, IGraphicsDirect3D1x)
	{
	public:
		GDAPI virtual IResult GfxImm_VertexArrayCreate(IGraphicsVertexArray** const gfxVertexArrayPtr
			, IGraphicsVertexArrayCreationInfo const* const gfxVertexArrayCreationInfo) override final;
	};	// uinterface IGraphicsDirect3D1xWithVertexArrays

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Sampler state objects.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if GD_IGRAPHICS_DIRECT3D1X_IMPL
	D3D1x_(TEXTURE_ADDRESS_MODE) static const IGraphicsDirect3D1xTextureAddressModesTable[IGRAPHICS_TEXTURE_ADDRESS_MODES_COUNT] = {
		/* IGRAPHICS_TEXTURE_ADDRESS_MODE_CLAMP_TO_EDGE */ D3D1x_(TEXTURE_ADDRESS_MIRROR_ONCE),
		/* IGRAPHICS_TEXTURE_ADDRESS_MODE_CLAMP_TO_ZERO */ D3D1x_(TEXTURE_ADDRESS_CLAMP),
		/* IGRAPHICS_TEXTURE_ADDRESS_MODE_REPEAT        */ D3D1x_(TEXTURE_ADDRESS_WRAP),
		/* IGRAPHICS_TEXTURE_ADDRESS_MODE_MIRROR        */ D3D1x_(TEXTURE_ADDRESS_MIRROR),
		/* IGRAPHICS_TEXTURE_ADDRESS_MODE_UNKNOWN       */ D3D1x_(TEXTURE_ADDRESS_MODE)(0),
	};
#endif	// if GD_IGRAPHICS_DIRECT3D1X_IMPL

	uinterface IGraphicsDirect3D1xSamplerState final : public IGraphicsSamplerState
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsDirect3D1xSamplerState, IGraphicsSamplerState);
	public:
		CComPtr<interface ID3D1x(SamplerState)> D3DSamplerState;
	private:
		friend class Allocator;
		friend uinterface IGraphicsDirect3D1xWithSamplerStates;
		GDAPI explicit IGraphicsDirect3D1xSamplerState(IGraphicsSamplerStateCreationInfo const* const gfxSamplerStateCreationInfo);
		GDAPI virtual ~IGraphicsDirect3D1xSamplerState();
	public:
		// No public interface functions here..
	};	// uinterface IGraphicsDirect3D1xSamplerState;
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsDirect3D1xWithSamplerStates, IGraphicsDirect3D1x)
	{
	public:
		GDAPI virtual IResult GfxImm_SamplerStateCreate(IGraphicsSamplerState** const gfxSamplerStatePtr
			, IGraphicsSamplerStateCreationInfo const* const gfxSamplerStateCreationInfo) override final;
	};	// uinterface IGraphicsDirect3D1xWithSamplerStates

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Shader Resources common.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if GD_IGRAPHICS_DIRECT3D1X_IMPL
	GLenum static const IGraphicsDirect3D1xResourceTypesTable[IGRAPHICS_RESOURCE_TYPES_COUNT] = {
		/* IGRAPHICS_RESOURCE_TYPE_TEXTURE_2D   */ GL_TEXTURE_2D,
		/* IGRAPHICS_RESOURCE_TYPE_TEXTURE_3D   */ GL_TEXTURE_3D,
		/* IGRAPHICS_RESOURCE_TYPE_TEXTURE_CUBE */ GL_TEXTURE_CUBE_MAP,
		/* IGRAPHICS_RESOURCE_TYPE_UNKNOWN      */ 0,
	};
#endif	// if GD_IGRAPHICS_DIRECT3D1X_IMPL

	// ------------------------------------------------------------------------------------------
	//! A GPU shader resource view interface.
	GD_DINTERFACE()
	uinterface IGraphicsDirect3D1xShaderResourceView : public IGraphicsShaderResourceView
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsDirect3D1xShaderResourceView, IGraphicsShaderResourceView);
	public:
		CComPtr<interface ID3D1x(ShaderResourceView)> D3DShaderResourceView;
	private:
		friend class Allocator;
		friend uinterface IGraphicsDirect3D1xWithSamplerStates;
		GDAPI explicit IGraphicsDirect3D1xShaderResourceView(IGraphicsResourceType const gfxResourceType);
		GDAPI virtual ~IGraphicsDirect3D1xShaderResourceView();
	public:
		// No public interface functions here..
	};	// uinterface IGraphicsShaderResourceView

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// 2D Textures.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if GD_IGRAPHICS_DIRECT3D1X_IMPL
	GLenum static const IGraphicsDirect3D1xTextureCompressionModesTable[IGRAPHICS_TEXTURE_2D_COMPRESSIONS_COUNT] = {
		/* IGRAPHICS_TEXTURE_2D_COMPRESSION_RAW     */ 0,
		/* IGRAPHICS_TEXTURE_2D_COMPRESSION_DXT1    */ GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
		/* IGRAPHICS_TEXTURE_2D_COMPRESSION_DXT3    */ GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
		/* IGRAPHICS_TEXTURE_2D_COMPRESSION_DXT5    */ GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
		/* IGRAPHICS_TEXTURE_2D_COMPRESSION_UNKNOWN */ 0,
	};
#endif	// if GD_IGRAPHICS_DIRECT3D1X_IMPL

	uinterface IGraphicsDirect3D1xTexture2D final : public IGraphicsTexture2D
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsDirect3D1xTexture2D, IGraphicsTexture2D);
	public:
		CComPtr<interface ID3D1x(Texture2D)>            D3DTexture2D;
		DIRefPtr<IGraphicsDirect3D1xShaderResourceView> D3DShaderResourceView;
	private:
		friend class Allocator;
		friend uinterface IGraphicsBaseDirect3D1x_WithTextures2D;
		GDAPI explicit IGraphicsDirect3D1xTexture2D(IGraphicsTexture2DCreationInfo const* const gfxTexture2DCreationInfo, IGraphicsTexture2DData const* const gfxTexture2DInitialData);
		GDAPI virtual ~IGraphicsDirect3D1xTexture2D();
	public:
		GDAPI virtual IGraphicsShaderResourceView const* CPU_GetShaderResourceView() const override final;
		GDAPI virtual IResult Imm_UpdateData(IGraphicsTexture2DData const* const gfxTexture2DData) override final;
	};	// uinterface IGraphicsDirect3D1xTexture2D
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsBaseDirect3D1x_WithTextures2D, IGraphicsDirect3D1x)
	{
	public:
		GDAPI virtual IResult GfxImm_Texture2DCreate(IGraphicsTexture2D** const gfxTexture2DPtr
			, IGraphicsTexture2DCreationInfo const* const gfxTexture2DCreationInfo
			, IGraphicsTexture2DData const* const gfxTexture2DData) override final;
	};	// uinterface IGraphicsBaseDirect3D1x_WithTextures2D
		
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Cube map textures.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	uinterface IGraphicsDirect3D1xTextureCube final : public IGraphicsTextureCube
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsDirect3D1xTextureCube, IGraphicsTextureCube);
	public:
		CComPtr<interface ID3D1x(TextureCube)>          D3DTextureCube;
		DIRefPtr<IGraphicsDirect3D1xShaderResourceView> D3DShaderResourceView;
	private:
		friend class Allocator;
		friend uinterface IGraphicsBaseDirect3D1x_WithTexturesCube;
		GDAPI explicit IGraphicsDirect3D1xTextureCube(IGraphicsTextureCubeCreationInfo const* const gfxTextureCubeCreationInfo, IGraphicsTextureCubeData const* const gfxTextureCubeInitialData);
		GDAPI virtual ~IGraphicsDirect3D1xTextureCube();
	public:
		GDAPI virtual IGraphicsShaderResourceView const* CPU_GetShaderResourceView() const override final;
		GDAPI virtual IResult Imm_UpdateData(IGraphicsTextureCubeData const* const gfxTextureCubeData) override final;
	};	// uinterface IGraphicsDirect3D1xTextureCube
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsBaseDirect3D1x_WithTexturesCube, IGraphicsDirect3D1x)
	{
	public:
		GDAPI virtual IResult GfxImm_TextureCubeCreate(IGraphicsTextureCube** const gfxTextureCubePtr
			, IGraphicsTextureCubeCreationInfo const* const gfxTextureCubeCreationInfo
			, IGraphicsTextureCubeData const* const gfxTextureInitialCubeData) override final;
	};	// uinterface IGraphicsBaseDirect3D1x_WithTexturesCube

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// 3D Textures.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if GD_FALSE

	uinterface IGraphicsDirect3D1xTexture3D final : public IGraphicsTexture3D
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsDirect3D1xTexture3D, IGraphicsTexture3D);
	public:
		CComPtr<interface ID3D1x(Texture3D)>            D3DTexture3D;
		DIRefPtr<IGraphicsDirect3D1xShaderResourceView> D3DShaderResourceView;
	private:
		friend class Allocator;
		friend uinterface IGraphicsBaseDirect3D1x_WithTextures3D;
		GDAPI explicit IGraphicsDirect3D1xTexture3D(IGraphicsTexture3DCreationInfo const* const gfxTexture3DCreationInfo);
		GDAPI virtual ~IGraphicsDirect3D1xTexture3D();
	public:
		GDAPI virtual IGraphicsShaderResourceView const* CPU_GetShaderResourceView() const override final;
		GDAPI virtual IResult Imm_UpdateData(IGraphicsTexture3DData const* const gfxTexture3DData) override final;
	};	// uinterface IGraphicsDirect3D1xTexture3D
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsBaseDirect3D1x_WithTextures3D, IGraphicsDirect3D1x)
	{
	public:
		GDAPI virtual IResult GfxImm_Texture3DCreate(IGraphicsTexture3D** const gfxTexture3DPtr
			, IGraphicsTexture3DCreationInfo const* const gfxTexture3DCreationInfo
			, IGraphicsTexture3DData const* const gfxTexture3DData) override final { return IResult::Error; }
	};	// uinterface IGraphicsBaseDirect3D1x_WithTextures3D

#endif	// if GD_FALSE

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Base shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if GD_IGRAPHICS_DIRECT3D1X_IMPL
	GLenum static const IGraphicsDirect3D1xShaderProfilesTable[IGRAPHICS_SHADER_TYPES_COUNT] = {
		/* IGRAPHICS_SHADER_TYPE_VERTEX   */ GL_VERTEX_SHADER,
		/* IGRAPHICS_SHADER_TYPE_PIXEL    */ GL_FRAGMENT_SHADER,
		/* IGRAPHICS_SHADER_TYPE_GEOMETRY */ GL_GEOMETRY_SHADER,
		/* IGRAPHICS_SHADER_TYPE_HULL     */ GL_TESS_CONTROL_SHADER,
		/* IGRAPHICS_SHADER_TYPE_DOMAIN   */ GL_TESS_EVALUATION_SHADER,
		/* IGRAPHICS_SHADER_TYPE_COMPUTE  */ GL_COMPUTE_SHADER,
		/* IGRAPHICS_SHADER_TYPE_UNKNOWN  */ 0,
	};
#endif	// if GD_IGRAPHICS_DIRECT3D1X_IMPL

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Vertex shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	uinterface IGraphicsDirect3D1xVertexShader final : public IGraphicsVertexShader
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsDirect3D1xVertexShader, IGraphicsVertexShader);
	public:
		CComPtr<interface ID3D1x(VertexShader)> D3DVertexShader;
	private:
		friend class Allocator;
		friend uinterface IGraphicsBaseDirect3D1x_WithBaseShaders;
		GDAPI explicit IGraphicsDirect3D1xVertexShader(IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo);
		GDAPI virtual ~IGraphicsDirect3D1xVertexShader();
	public:
		// No public interface functions here..
	};	// uinterface IGraphicsDirect3D1xVertexShader
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsDirect3D1xWithVertexShaders, IGraphicsDirect3D1x)
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
	};	// uinterface IGraphicsDirect3D1xWithVertexShaders

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Pixel shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	uinterface IGraphicsDirect3D1xPixelShader final : public IGraphicsPixelShader
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsDirect3D1xPixelShader, IGraphicsPixelShader);
	public:
		CComPtr<interface ID3D1x(PixelShader)> D3DPixelShader;
	private:
		friend class Allocator;
		friend uinterface IGraphicsBaseDirect3D1x_WithBaseShaders;
		GDAPI explicit IGraphicsDirect3D1xPixelShader(IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo);
		GDAPI virtual ~IGraphicsDirect3D1xPixelShader();
	public:
		// No public interface functions here..
	};	// uinterface IGraphicsDirect3D1xPixelShader
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsDirect3D1xWithPixelShaders, IGraphicsDirect3D1x)
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
	};	// uinterface IGraphicsDirect3D1xWithPixelShaders

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Geometry shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	uinterface IGraphicsDirect3D1xGeometryShader final : public IGraphicsGeometryShader
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsDirect3D1xGeometryShader, IGraphicsGeometryShader);
	public:
		CComPtr<interface ID3D1x(GeometryShader)> D3DGeometryShader;
	private:
		friend class Allocator;
		friend uinterface IGraphicsBaseDirect3D1x_WithBaseShaders;
		GDAPI explicit IGraphicsDirect3D1xGeometryShader(IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo);
		GDAPI virtual ~IGraphicsDirect3D1xGeometryShader();
	public:
		// No public interface functions here..
	};	// uinterface IGraphicsDirect3D1xGeometryShader
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsDirect3D1xWithGeometryShaders, IGraphicsDirect3D1x)
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
	};	// uinterface IGraphicsDirect3D1xWithGeometryShaders

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Hull shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if GD_FALSE

	uinterface IGraphicsDirect3D1xHullShader final : public IGraphicsHullShader
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsDirect3D1xHullShader, IGraphicsHullShader);
	public:
#if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11			
		CComPtr<interface ID3D1x(HullShader)> D3DHullShader;
#endif	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
	private:
		friend class Allocator;
		friend uinterface IGraphicsBaseDirect3D1x_WithBaseShaders;
		GDAPI explicit IGraphicsDirect3D1xHullShader(IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo);
		GDAPI virtual ~IGraphicsDirect3D1xHullShader();
	public:
		// No public interface functions here..
	};	// uinterface IGraphicsDirect3D1xHullShader
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsDirect3D1xWithHullShaders, IGraphicsDirect3D1x)
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
	};	// uinterface IGraphicsDirect3D1xWithHullShaders

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Domain shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	uinterface IGraphicsDirect3D1xDomainShader final : public IGraphicsDomainShader
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsDirect3D1xDomainShader, IGraphicsDomainShader);
	public:
#if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11			
		CComPtr<interface ID3D1x(DomainShader)> D3DDomainShader;
#endif	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
	private:
		friend class Allocator;
		friend uinterface IGraphicsBaseDirect3D1x_WithBaseShaders;
		GDAPI explicit IGraphicsDirect3D1xDomainShader(IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo);
		GDAPI virtual ~IGraphicsDirect3D1xDomainShader();
	public:
		// No public interface functions here..
	};	// uinterface IGraphicsDirect3D1xDomainShader
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsDirect3D1xWithDomainShaders, IGraphicsDirect3D1x)
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
	};	// uinterface IGraphicsDirect3D1xWithDomainShaders

#endif	// if GD_FALSE

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Compute shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if GD_FALSE

	uinterface IGraphicsDirect3D1xComputeShader final : public IGraphicsComputeShader
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsDirect3D1xComputeShader, IGraphicsComputeShader);
	public:
#if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11			
		CComPtr<interface ID3D1x(ComputeShader)> D3DComputeShader;
#endif	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
	private:
		friend class Allocator;
		friend uinterface IGraphicsBaseDirect3D1x_WithBaseShaders;
		GDAPI explicit IGraphicsDirect3D1xComputeShader(IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo);
		GDAPI virtual ~IGraphicsDirect3D1xComputeShader();
	public:
		// No public interface functions here..
	};	// uinterface IGraphicsDirect3D1xComputeShader
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsDirect3D1xWithComputeShaders, IGraphicsDirect3D1x)
	{
	public:
		//! @todo Provide functionality for compute shaders.
	};	// uinterface IGraphicsDirect3D1xWithComputeShaders

#endif	// if GD_FALSE

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// PPipeline state object.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GD_DINTERFACE()
	uinterface IGraphicsDirect3D1xPipelineState : public IGraphicsPipelineState
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsDirect3D1xPipelineState, IGraphicsPipelineState);
	public:
		GLuint GLProgramPipelineID;
	private:
		friend class Allocator;
		friend uinterface IGraphicsDirect3D1xWithPipelineStates;
		GDAPI explicit IGraphicsDirect3D1xPipelineState(IGraphicsPipelineStateCreationInfo const* const gfxPipelineStateCreationInfo);
		GDAPI virtual ~IGraphicsDirect3D1xPipelineState();
	public:
		// No public interface functions here..
	};	// uinterface IGraphicsDirect3D1xPipelineState
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsDirect3D1xWithPipelineStates, IGraphicsDirect3D1x)
	{
	public:
		GDAPI virtual IResult GfxImm_PipelineCreate(IGraphicsPipelineState** const gfxPipelinePtr
			, IGraphicsPipelineStateCreationInfo const* const gfxPipelineCreationInfo) override final;
	};	// uinterface IGraphicsDirect3D1xPipelineState

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Global interface.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsDirect3D1x, IGraphicsDirect3D1x)
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsDirect3D1x, IGraphics);

	public:
		CComPtr<interface IDXGIFactory>				D3DDXGIFactory;
#if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11			// Direct3D 10 has no separation for Device and DeviceContext interfaces.
		CComPtr<interface ID3D1x(Device)>			D3DDevice;
		CComPtr<interface ID3D1x(DeviceContext)>	D3DDeviceContext;
#else	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
		CComPtr<interface ID3D10Device>				D3DDevice;
		ID3D10Device*								D3DDeviceContext;	
#endif	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11

	};	// uinterface IGraphicsOpenGL

		//! Reference to the global Direct2D interface.
#if GD_IGRAPHICS_DIRECT3D1X_IMPL
#	define GD_IGRAPHICS_DIRECT3D1X object_cast<GD::IGraphicsDirect3D1x*>(GD::Graphics.GetPointer())
#endif	// if GD_IGRAPHICS_DIRECT3D1X_IMPL

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_API_MICROSOFT
