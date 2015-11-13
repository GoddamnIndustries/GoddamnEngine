// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Engine/Interface/Graphics/GraphicsOpenGL/GraphicsOpenGL.h
 * File contains export interface for OpenGL 4.3 Implementation of the graphics interface.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#define GD_PLATFORM_API_OPENGL GD_PLATFORM_DESKTOP || (GD_PLATFORM_MOBILE && !GD_PLATFORM_API_MICROSOFT)
#if GD_PLATFORM_API_OPENGL

#include <GoddamnEngine/Core/Platform/Platform.h>
#if GD_PLATFORM_API_LIBSDL1 || GD_PLATFORM_API_LIBSDL2
#	include GD_PLATFORM_API_INCLUDE(GoddamnEngine/Engine/Interface/Graphics/GraphicsOpenGL, GraphicsOpenGL)
#else // if GD_PLATFORM_API_LIBSDL1 || GD_PLATFORM_API_LIBSDL2
#	include GD_PLATFORM_INCLUDE(GoddamnEngine/Engine/Interface/Graphics/GraphicsOpenGL, GraphicsOpenGL)
#endif	// if GD_PLATFORM_API_LIBSDL1 || GD_PLATFORM_API_LIBSDL2

#if !defined(GD_IGRAPHICS_OPENGL_IMPL)
#	define GD_IGRAPHICS_OPENGL_IMPL GD_FALSE
#endif	// if !defined(GD_IGRAPHICS_OPENGL_IMPL)

#if GD_IGRAPHICS_OPENGL_IMPL
#	include <GL/glew.h>	// Use this header is for implementations only.
#else	// if GD_IGRAPHICS_OPENGL_IMPL
#	include <GL/GL.h>	// Available everywhere, we can simply include this header to provide header interface.
#endif	// if GD_IGRAPHICS_OPENGL_IMPL

GD_NAMESPACE_BEGIN

	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL_BASE(IGraphicsOpenGL, IGraphicsOpenGLPlatform);

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Command lists.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	uinterface IGraphicsOpenGLCommandList final : public IGraphicsCommandList
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsOpenGLCommandList, IGraphicsCommandList);
	private:
		friend uinterface IGraphicsOpenGL;
		GDAPI explicit IGraphicsOpenGLCommandList(...) {}
		GDAPI virtual ~IGraphicsOpenGLCommandList() {}
	public:
		// No public interface functions here..
	};	// uinterface IGraphicsOpenGLCommandList
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsOpenGLWithCommandLists, IGraphicsOpenGL)
	{
	public:
		//! @todo Implement command lists here.
	};	// uinterface IGraphicsBase_WithCommandLists

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Buffers.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if GD_IGRAPHICS_OPENGL_IMPL
	GLenum static const IGraphicsOpenGLBufferTypesTable[IGRAPHICS_BUFFER_TYPES_COUNT] = {
		/* IGRAPHICS_BUFFER_TYPE_VERTEX   */ GL_ARRAY_BUFFER,
		/* IGRAPHICS_BUFFER_TYPE_INDEX    */ GL_ELEMENT_ARRAY_BUFFER,
		/* IGRAPHICS_BUFFER_TYPE_UNIFORM  */ GL_UNIFORM_BUFFER,
		/* IGRAPHICS_BUFFER_TYPE_UNKNOWN  */ 0,
	};
#endif	// if GD_IGRAPHICS_OPENGL_IMPL

	uinterface IGraphicsOpenGLBuffer final : public IGraphicsBuffer
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsOpenGLBuffer, IGraphicsBuffer);
	public:
		GLuint GLBufferID;
		GLenum GLBufferType;
		Handle GLBufferMappedMemory;
	private:
		friend class Allocator;
		friend uinterface IGraphicsOpenGLWithBuffers;
		GDAPI explicit IGraphicsOpenGLBuffer(IGraphicsBufferCreationInfo const* const gfxBufferCreationInfo, CHandle const gfxBufferInitialData);
		GDAPI virtual ~IGraphicsOpenGLBuffer();
	public:
		GDAPI virtual IResult Imm_MapForWriting(Handle* const gfxMappedWOMemory) override final;
		GDAPI virtual IResult Imm_MapForReading(CHandle* const gfxMappedROMemory) override final;
		GDAPI virtual void Imm_Unmap() override final;
	};	// uinterface IGraphicsOpenGLBuffer
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsOpenGLWithBuffers, IGraphicsOpenGL)
	{
	public:
		GDAPI virtual IResult GfxImm_BufferCreate(IGraphicsBuffer** const gfxBufferPtr
			, IGraphicsBufferCreationInfo const* const gfxBufferCreationInfo, CHandle const gfxBufferInitialData) override final;
	};	// uinterface IGraphicsOpenGLWithBuffers

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Vertex array objects.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	struct IGraphicsOpenGLVertexArray final : public IGraphicsVertexArray
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsOpenGLVertexArray, IGraphicsVertexArray);
	public:
		GLuint GLVertexArrayID;
	private:
		friend class Allocator;
		friend uinterface IGraphicsOpenGLWithVertexArrays;
		GDAPI explicit IGraphicsOpenGLVertexArray(IGraphicsVertexArrayCreationInfo const* const gfxVertexArrayCreationInfo);
		GDAPI virtual ~IGraphicsOpenGLVertexArray();
	public:
		// No public interface functions here..
	};	// uinterface IGraphicsOpenGLVertexArray
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsOpenGLWithVertexArrays, IGraphicsOpenGL)
	{
	public:
		GDAPI virtual IResult GfxImm_VertexArrayCreate(IGraphicsVertexArray** const gfxVertexArrayPtr
			, IGraphicsVertexArrayCreationInfo const* const gfxVertexArrayCreationInfo) override final;
	};	// uinterface IGraphicsOpenGLWithVertexArrays

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Sampler state objects.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if GD_IGRAPHICS_OPENGL_IMPL
	GLint static const IGraphicsOpenGLTextureAddressModesTable[IGRAPHICS_TEXTURE_ADDRESS_MODES_COUNT] = {
		/* IGRAPHICS_TEXTURE_ADDRESS_MODE_CLAMP_TO_EDGE */ GL_CLAMP_TO_EDGE,
		/* IGRAPHICS_TEXTURE_ADDRESS_MODE_CLAMP_TO_ZERO */ GL_CLAMP_TO_BORDER,
		/* IGRAPHICS_TEXTURE_ADDRESS_MODE_REPEAT        */ GL_REPEAT,
		/* IGRAPHICS_TEXTURE_ADDRESS_MODE_MIRROR        */ GL_MIRRORED_REPEAT,
		/* IGRAPHICS_TEXTURE_ADDRESS_MODE_UNKNOWN       */ 0,
	};
#endif	// if GD_IGRAPHICS_OPENGL_IMPL

#if GD_IGRAPHICS_OPENGL_IMPL
	GLint static const IGraphicsOpenGLTextureMinFiltersTable[IGRAPHICS_TEXTURE_MINMAG_FILTERS_COUNT] = {
		/* IGRAPHICS_TEXTURE_MINMAG_FILTER_NEAREST      */ GL_NEAREST_MIPMAP_NEAREST,
		/* IGRAPHICS_TEXTURE_MINMAG_FILTER_LINEAR       */ GL_LINEAR_MIPMAP_LINEAR,
		/* IGRAPHICS_TEXTURE_MINMAG_FILTER_UNKNOWN      */ 0,
	};
#endif	// if GD_IGRAPHICS_OPENGL_IMPL

#if GD_IGRAPHICS_OPENGL_IMPL
	GLint static const IGraphicsOpenGLTextureMagFiltersTable[IGRAPHICS_TEXTURE_MINMAG_FILTERS_COUNT] = {
		/* IGRAPHICS_TEXTURE_MINMAG_FILTER_NEAREST      */ GL_NEAREST,
		/* IGRAPHICS_TEXTURE_MINMAG_FILTER_LINEAR       */ GL_LINEAR,
		/* IGRAPHICS_TEXTURE_MINMAG_FILTER_UNKNOWN      */ 0,
	};
#endif	// if GD_IGRAPHICS_OPENGL_IMPL

	uinterface IGraphicsOpenGLSamplerState final : public IGraphicsSamplerState
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsOpenGLSamplerState, IGraphicsSamplerState);
	public:
		GLuint GLSamplerStateID;
	private:
		friend class Allocator;
		friend uinterface IGraphicsOpenGLWithSamplerStates;
		GDAPI explicit IGraphicsOpenGLSamplerState(IGraphicsSamplerStateCreationInfo const* const gfxSamplerStateCreationInfo);
		GDAPI virtual ~IGraphicsOpenGLSamplerState();
	public:
		// No public interface functions here..
	};	// uinterface IGraphicsOpenGLSamplerState;
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsOpenGLWithSamplerStates, IGraphicsOpenGL)
	{
	public:
		GDAPI virtual IResult GfxImm_SamplerStateCreate(IGraphicsSamplerState** const gfxSamplerStatePtr
			, IGraphicsSamplerStateCreationInfo const* const gfxSamplerStateCreationInfo) override final;
	};	// uinterface IGraphicsOpenGLWithSamplerStates

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Shader Resources common.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if GD_IGRAPHICS_OPENGL_IMPL
	GLenum static const IGraphicsOpenGLResourceTypesTable[IGRAPHICS_RESOURCE_TYPES_COUNT] = {
		/* IGRAPHICS_RESOURCE_TYPE_TEXTURE_2D   */ GL_TEXTURE_2D,
		/* IGRAPHICS_RESOURCE_TYPE_TEXTURE_3D   */ GL_TEXTURE_3D,
		/* IGRAPHICS_RESOURCE_TYPE_TEXTURE_CUBE */ GL_TEXTURE_CUBE_MAP,
		/* IGRAPHICS_RESOURCE_TYPE_UNKNOWN      */ 0,
	};
#endif	// if GD_IGRAPHICS_OPENGL_IMPL

	// ------------------------------------------------------------------------------------------
	//! A GPU shader resource view interface.
	GD_DINTERFACE()
	uinterface IGraphicsOpenGLShaderResourceView : public IGraphicsShaderResourceView
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsOpenGLShaderResourceView, IGraphicsShaderResourceView);
	public:
		GLuint GLShaderResourceID;
		GLenum GLShaderResourceType;
	private:
		friend class Allocator;
		friend uinterface IGraphicsOpenGLWithSamplerStates;
		GDAPI explicit IGraphicsOpenGLShaderResourceView(IGraphicsResourceType const gfxResourceType);
		GDAPI virtual ~IGraphicsOpenGLShaderResourceView();
	public:
		// No public interface functions here..
	};	// uinterface IGraphicsShaderResourceView

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// 2D Textures.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if GD_IGRAPHICS_OPENGL_IMPL
	GLenum static const IGraphicsOpenGLTextureCompressionModesTable[IGRAPHICS_TEXTURE_2D_COMPRESSIONS_COUNT] = {
		/* IGRAPHICS_TEXTURE_2D_COMPRESSION_RAW     */ 0,
		/* IGRAPHICS_TEXTURE_2D_COMPRESSION_DXT1    */ GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
		/* IGRAPHICS_TEXTURE_2D_COMPRESSION_DXT3    */ GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
		/* IGRAPHICS_TEXTURE_2D_COMPRESSION_DXT5    */ GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
		/* IGRAPHICS_TEXTURE_2D_COMPRESSION_UNKNOWN */ 0,
	};
#endif	// if GD_IGRAPHICS_OPENGL_IMPL

	uinterface IGraphicsOpenGLTexture2D final : public IGraphicsTexture2D
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsOpenGLTexture2D, IGraphicsTexture2D);
	public:
		GLuint GLTexture2DID;
		DIRefPtr<IGraphicsOpenGLShaderResourceView> GLShaderResourceView;
	private:
		friend class Allocator;
		friend uinterface IGraphicsBaseOpenGL_WithTextures2D;
		GDAPI explicit IGraphicsOpenGLTexture2D(IGraphicsTexture2DCreationInfo const* const gfxTexture2DCreationInfo, IGraphicsTexture2DData const* const gfxTexture2DInitialData);
		GDAPI virtual ~IGraphicsOpenGLTexture2D();
	public:
		GDAPI virtual IGraphicsShaderResourceView const* CPU_GetShaderResourceView() const override final;
		GDAPI virtual IResult Imm_UpdateData(IGraphicsTexture2DData const* const gfxTexture2DData) override final;
	};	// uinterface IGraphicsOpenGLTexture2D
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsBaseOpenGL_WithTextures2D, IGraphicsOpenGL)
	{
	public:
		GDAPI virtual IResult GfxImm_Texture2DCreate(IGraphicsTexture2D** const gfxTexture2DPtr
			, IGraphicsTexture2DCreationInfo const* const gfxTexture2DCreationInfo
			, IGraphicsTexture2DData const* const gfxTexture2DData) override final;
	};	// uinterface IGraphicsBaseOpenGL_WithTextures2D
		
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Cube map textures.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	uinterface IGraphicsOpenGLTextureCube final : public IGraphicsTextureCube
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsOpenGLTextureCube, IGraphicsTextureCube);
	public:
		GLuint GLTextureCubeID;
		DIRefPtr<IGraphicsOpenGLShaderResourceView> GLShaderResourceView;
	private:
		friend class Allocator;
		friend uinterface IGraphicsBaseOpenGL_WithTexturesCube;
		GDAPI explicit IGraphicsOpenGLTextureCube(IGraphicsTextureCubeCreationInfo const* const gfxTextureCubeCreationInfo, IGraphicsTextureCubeData const* const gfxTextureCubeInitialData);
		GDAPI virtual ~IGraphicsOpenGLTextureCube();
	public:
		GDAPI virtual IGraphicsShaderResourceView const* CPU_GetShaderResourceView() const override final;
		GDAPI virtual IResult Imm_UpdateData(IGraphicsTextureCubeData const* const gfxTextureCubeData) override final;
	};	// uinterface IGraphicsOpenGLTextureCube
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsBaseOpenGL_WithTexturesCube, IGraphicsOpenGL)
	{
	public:
		GDAPI virtual IResult GfxImm_TextureCubeCreate(IGraphicsTextureCube** const gfxTextureCubePtr
			, IGraphicsTextureCubeCreationInfo const* const gfxTextureCubeCreationInfo
			, IGraphicsTextureCubeData const* const gfxTextureInitialCubeData) override final;
	};	// uinterface IGraphicsBaseOpenGL_WithTexturesCube

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// 3D Textures.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if GD_FALSE

	uinterface IGraphicsOpenGLTexture3D final : public IGraphicsTexture3D
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsOpenGLTexture3D, IGraphicsTexture3D);
	public:
		DIRefPtr<IGraphicsOpenGLShaderResourceView> GLShaderResourceView;
	private:
		friend class Allocator;
		friend uinterface IGraphicsBaseOpenGL_WithTextures3D;
		GDAPI explicit IGraphicsOpenGLTexture3D(IGraphicsTexture3DCreationInfo const* const gfxTexture3DCreationInfo);
		GDAPI virtual ~IGraphicsOpenGLTexture3D();
	public:
		GDAPI virtual IGraphicsShaderResourceView const* CPU_GetShaderResourceView() const override final;
		GDAPI virtual IResult Imm_UpdateData(IGraphicsTexture3DData const* const gfxTexture3DData) override final;
	};	// uinterface IGraphicsOpenGLTexture3D
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsBaseOpenGL_WithTextures3D, IGraphicsOpenGL)
	{
	public:
		GDAPI virtual IResult GfxImm_Texture3DCreate(IGraphicsTexture3D** const gfxTexture3DPtr
			, IGraphicsTexture3DCreationInfo const* const gfxTexture3DCreationInfo
			, IGraphicsTexture3DData const* const gfxTexture3DData) override final { return IResult::Error; }
	};	// uinterface IGraphicsBaseOpenGL_WithTextures3D

#endif	// if GD_FALSE

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Base shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if GD_IGRAPHICS_OPENGL_IMPL
	GLenum static const IGraphicsOpenGLShaderTypesTable[IGRAPHICS_SHADER_TYPES_COUNT] = {
		/* IGRAPHICS_SHADER_TYPE_VERTEX   */ GL_VERTEX_SHADER,
		/* IGRAPHICS_SHADER_TYPE_PIXEL    */ GL_FRAGMENT_SHADER,
		/* IGRAPHICS_SHADER_TYPE_GEOMETRY */ GL_GEOMETRY_SHADER,
		/* IGRAPHICS_SHADER_TYPE_HULL     */ GL_TESS_CONTROL_SHADER,
		/* IGRAPHICS_SHADER_TYPE_DOMAIN   */ GL_TESS_EVALUATION_SHADER,
		/* IGRAPHICS_SHADER_TYPE_COMPUTE  */ GL_COMPUTE_SHADER,
		/* IGRAPHICS_SHADER_TYPE_UNKNOWN  */ 0,
	};
#endif	// if GD_IGRAPHICS_OPENGL_IMPL

	GD_DINTERFACE()
	template<typename IGraphicsShaderBase>
	uinterface IGraphicsOpenGLBaseShader : public IGraphicsShaderBase
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsOpenGLBaseShader<IGraphicsShaderBase>, IGraphicsShaderBase);
	public:
		GLuint GLShaderProgramID;
		GLenum GLShaderProgramType;
	private:
		friend class Allocator;
		friend uinterface IGraphicsBaseOpenGL_WithBaseShaders;
		GDINT explicit IGraphicsOpenGLBaseShader(IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo);
		GDINT virtual ~IGraphicsOpenGLBaseShader();
	public:
		// No public interface functions here..
	};	// uinterface IGraphicsOpenGLBaseShader<T>
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsBaseOpenGL_WithBaseShaders, IGraphicsOpenGL)
	{
	protected:
		template<IGraphicsShaderType gfxShaderType>
		GDINT void GfxCmd_BaseShaderBindUniformBuffers(IGraphicsBuffer const* const* const gfxUniformBuffers, SizeTp const gfxUniformBuffersCount);
		template<IGraphicsShaderType gfxShaderType>
		GDINT void GfxCmd_BaseShaderBindResources(IGraphicsShaderResourceView const* const* const gfxResources, SizeTp const gfxResourcesCount);
		template<IGraphicsShaderType gfxShaderType>
		GDINT void GfxCmd_BaseShaderBindSamplers(IGraphicsSamplerState const* const* const gfxSamplerStates, SizeTp const gfxSamplerStatesCount);
	};	// uinterface IGraphicsBaseOpenGL_WithTextures2D

	template<typename IGraphicsShaderBase>
	struct IClassConstrutor<IGraphicsOpenGLBaseShader<IGraphicsShaderBase>> { GDINL static IClassConstrutorProc Get() { return nullptr; } };
	template<typename IGraphicsShaderBase>
	GD_IMPLEMENT_CLASS_INFORMATION_FORCE_NO_TAGS(IGraphicsOpenGLBaseShader<IGraphicsShaderBase>);

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Vertex shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	typedef IGraphicsOpenGLBaseShader<IGraphicsVertexShader> IGraphicsOpenGLVertexShader;
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsOpenGLWithVertexShaders, IGraphicsOpenGL)
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
	};	// uinterface IGraphicsOpenGLWithVertexShaders

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Pixel (fragment) shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	typedef IGraphicsOpenGLBaseShader<IGraphicsPixelShader> IGraphicsOpenGLPixelShader;
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsOpenGLWithPixelShaders, IGraphicsOpenGL)
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
	};	// uinterface IGraphicsOpenGLWithPixelShaders

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Geometry shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	typedef IGraphicsOpenGLBaseShader<IGraphicsGeometryShader> IGraphicsOpenGLGeometryShader;
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsOpenGLWithGeometryShaders, IGraphicsOpenGL)
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
	};	// uinterface IGraphicsOpenGLWithGeometryShaders

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Hull (tessellation control) shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if GD_FALSE

	typedef IGraphicsOpenGLBaseShader<IGraphicsHullShader> IGraphicsOpenGLHullShader;
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsOpenGLWithHullShaders, IGraphicsOpenGL)
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
	};	// uinterface IGraphicsOpenGLWithHullShaders

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Domain (tessellation evaluation) shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	typedef IGraphicsOpenGLBaseShader<IGraphicsDomainShader> IGraphicsOpenGLDomainShader;
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsOpenGLWithDomainShaders, IGraphicsOpenGL)
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
	};	// uinterface IGraphicsOpenGLWithDomainShaders

#endif	// if GD_FALSE

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Compute shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if GD_FALSE

	typedef IGraphicsOpenGLBaseShader<IGraphicsComputeShader> IGraphicsOpenGLComputeShader;
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsOpenGLWithComputeShaders, IGraphicsOpenGL)
	{
	public:
		//! @todo Provide functionality for compute shaders.
	};	// uinterface IGraphicsOpenGLWithComputeShaders

#endif	// if GD_FALSE

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// PPipeline state object.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GD_DINTERFACE()
	uinterface IGraphicsOpenGLPipelineState : public IGraphicsPipelineState
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsOpenGLPipelineState, IGraphicsPipelineState);
	public:
		GLuint GLProgramPipelineID;
	private:
		friend class Allocator;
		friend uinterface IGraphicsOpenGLWithPipelineStates;
		GDAPI explicit IGraphicsOpenGLPipelineState(IGraphicsPipelineStateCreationInfo const* const gfxPipelineStateCreationInfo);
		GDAPI virtual ~IGraphicsOpenGLPipelineState();
	public:
		// No public interface functions here..
	};	// uinterface IGraphicsOpenGLPipelineState
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsOpenGLWithPipelineStates, IGraphicsOpenGL)
	{
	public:
		GDAPI virtual IResult GfxImm_PipelineCreate(IGraphicsPipelineState** const gfxPipelinePtr
			, IGraphicsPipelineStateCreationInfo const* const gfxPipelineCreationInfo) override final;
	};	// uinterface IGraphicsOpenGLPipelineState

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Global interface.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsOpenGL, IGraphicsOpenGL)
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphicsOpenGL, IGraphics);
		GDAPI virtual IResult OnRuntimeInitialize() override final;
	};	// uinterface IGraphicsOpenGL

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_API_OPENGL
