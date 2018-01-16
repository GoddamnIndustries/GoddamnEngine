// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/GraphicsOpenGL/GraphicsOpenGL.h
 * File contains export interface for OpenGL 4.3 Implementation of the graphics interface.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Interface/Graphics/Graphics.h>

#define GD_PLATFORM_API_OPENGL GD_PLATFORM_DESKTOP || (GD_PLATFORM_MOBILE && !GD_PLATFORM_API_MICROSOFT)
#if GD_PLATFORM_API_OPENGL

#include <GoddamnEngine/Core/PlatformSpecificInclude.h>
#include GD_PLATFORM_INCLUDE(GoddamnEngine/GraphicsOpenGL, GraphicsOpenGL)

#if !defined(GD_IGRAPHICS_OPENGL_IMPL)
#	define GD_IGRAPHICS_OPENGL_IMPL GD_TRUE
#endif	// if !defined(GD_IGRAPHICS_OPENGL_IMPL)

#include <GL/glew.h>


GD_NAMESPACE_BEGIN

#define glThrowIfFailed(...) __VA_ARGS__; CheckIfFailed();
	GDINL static void CheckIfFailed(void)
	{
		//auto a = glGetError();
		//GD_VERIFY(a == GL_NO_ERROR, "WINAPI\\D3D11 function has failed.");
	}

	GDEXP extern IGraphics* CreateIGraphicsInstance();
	GD_DEFINE_PARTIAL_CALSS_FIRST(IGraphicsOpenGL, IGraphicsOpenGLPlatform);

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                         IGraphicsCommandList interface.                          ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	GD_DEFINE_PARTIAL_CLASS(IGraphicsOpenGLWithCommandLists, IGraphicsOpenGL)
	{
	public:
		GDAPI virtual IResult GfxImm_CommandListCreate(IGraphicsCommandList** const gfxCommandListPtr
			, IGraphicsCommandListCreateInfo const* const gfxCommandListCreateInfo) override final;
	};	// class IGraphicsOpenGLWithCommandLists

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                            IGraphicsBuffer interface.                            ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	class IGraphicsOpenGLBuffer final : public IGraphicsBuffer
	{
	public:
		GLuint const m_GLBufferID;
		GLenum const m_GLBufferType;
		Handle m_GLBufferMappedMemory = nullptr;

	public:
		GDINL explicit IGraphicsOpenGLBuffer(IGraphicsBufferCreateInfo const& gfxBufferCreateInfo
			, GLuint const glBufferID, GLenum const glBufferType)
			: IGraphicsBuffer(gfxBufferCreateInfo), m_GLBufferID(glBufferID), m_GLBufferType(glBufferType) {}
	};	// class IGraphicsOpenGLBuffer

	GD_DEFINE_PARTIAL_CLASS(IGraphicsOpenGLWithBuffers, IGraphicsOpenGL)
	{
	public:
		GDAPI virtual IResult GfxImm_BufferDestroy(IGraphicsBuffer* const gfxBufferPtr) override final;
		GDAPI virtual IResult GfxImm_BufferCreate(IGraphicsBuffer*& const gfxBufferPtr
			, IGraphicsBufferCreateInfo const& gfxBufferCreateInfo) override final;

		GDAPI virtual IResult GfxImm_BufferUnmap(IGraphicsBuffer* const gfxBuffer) override final;
		GDAPI virtual IResult GfxImm_BufferMap(IGraphicsBuffer* const gfxBuffer
			, IGraphicsResourceMapMode const gfxMapMode, Handle& gfxMappedMemory) override final;
	};	// class IGraphicsOpenGLWithBuffers

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                            IGraphicsTexture2D interface.                         ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	class IGraphicsOpenGLTexture2D final : public IGraphicsTexture2D
	{
	public:
		GLuint const m_GLTexture2DID;
		GLuint const m_GLTexture2DBufferID;
	
	public:
		GDINL explicit IGraphicsOpenGLTexture2D(IGraphicsTexture2DCreateInfo const& gfxTexture2DCreateInfo
			, GLuint const glTexture2DID, GLuint const glTexture2DBufferID)
			: IGraphicsTexture2D(gfxTexture2DCreateInfo), m_GLTexture2DID(glTexture2DID), m_GLTexture2DBufferID(glTexture2DBufferID) {}
	};	// class IGraphicsOpenGLTexture2D

	GD_DEFINE_PARTIAL_CLASS(IGraphicsBaseOpenGLWithTextures2D, IGraphicsOpenGL)
	{
	public:
		GDAPI virtual IResult GfxImm_Texture2DDestroy(IGraphicsTexture2D* const gfxTexture2D) override final;
		GDAPI virtual IResult GfxImm_Texture2DCreate(IGraphicsTexture2D*& gfxTexture2DPtr
			, IGraphicsTexture2DCreateInfo const& gfxTexture2DCreateInfo) override final;

		GDAPI virtual IResult GfxImm_TextureUnmap(IGraphicsTexture2D* const gfxTexture2D) override final;
		GDAPI virtual IResult GfxImm_Texture2DMap(IGraphicsTexture2D* const gfxTexture2D
			, IGraphicsResourceMapMode const gfxMapMode, Handle& gfxMappedMemory) override final;
	};	// class IGraphicsBaseOpenGLWithTextures2D
	
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                           IGraphicsTextureCube interface.                        ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

#if 0
	class IGraphicsOpenGLTextureCube final : public IGraphicsTextureCube
	{
	public:
		GLuint m_GLTextureCubeID;
		SharedPtr<IGraphicsOpenGLShaderResourceView> m_GLShaderResourceView;
	
	public:
		GDAPI explicit IGraphicsOpenGLTextureCube(IGraphicsTextureCubeCreateInfo const* const gfxTextureCubeCreateInfo
			, IGraphicsTextureCubeData const* const gfxTextureCubeInitialData);
		GDAPI virtual ~IGraphicsOpenGLTextureCube();

	public:
		GDAPI virtual IGraphicsShaderResourceView const* CPU_GetShaderResourceView() const override final;
		GDAPI virtual IResult Imm_UpdateData(IGraphicsTextureCubeData const* const gfxTextureCubeData) override final;
	};	// class IGraphicsOpenGLTextureCube

	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphicsBaseOpenGLWithTexturesCube, IGraphicsOpenGL)
	{
	public:
		GDAPI virtual IResult GfxImm_TextureCubeCreate(IGraphicsTextureCube** const gfxTextureCubePtr
			, IGraphicsTextureCubeCreateInfo const* const gfxTextureCubeCreateInfo
			, IGraphicsTextureCubeData const* const gfxTextureInitialCubeData) override final;
	};	// class IGraphicsBaseOpenGLWithTexturesCube
#endif

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                         IGraphicsResource(View) interface.                       ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	class IGraphicsOpenGLShaderResourceView : public IGraphicsShaderResourceView
	{
	public:
		GLuint m_GLShaderResourceID;
		GLenum m_GLShaderResourceType;

	public:
		GDAPI explicit IGraphicsOpenGLShaderResourceView(IGraphicsShaderResourceType const gfxResourceType);
		GDAPI virtual ~IGraphicsOpenGLShaderResourceView();

	public:
		// No public interface functions here..
	};	// class IGraphicsShaderResourceView

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                         IGraphicsSampler interface.                         ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	class IGraphicsOpenGLSampler final : public IGraphicsSampler
	{
	public:
		GLuint const m_GLSamplerID;

	public:
		GDINL explicit IGraphicsOpenGLSampler(IGraphicsSamplerCreateInfo const& gfxSamplerCreateInfo
			, GLuint const glSamplerID) 
			: IGraphicsSampler(gfxSamplerCreateInfo), m_GLSamplerID(glSamplerID) {}
	};	// class IGraphicsOpenGLSampler;
	
	GD_DEFINE_PARTIAL_CLASS(IGraphicsOpenGLWithSamplers, IGraphicsOpenGL)
	{
	public:
		GDAPI virtual IResult GfxImm_SamplerDestroy(IGraphicsSampler* const gfxSampler) override final;
		GDAPI virtual IResult GfxImm_SamplerCreate(IGraphicsSampler*& gfxSamplerPtr
			, IGraphicsSamplerCreateInfo const& gfxSamplerCreateInfo) override final;
	};	// class IGraphicsOpenGLWithSamplers

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                             IGraphicsShader interface.                           ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	template<typename TIGraphicsShaderBase>
	class IGraphicsOpenGLBaseShader : public TIGraphicsShaderBase
	{
	public:
		GLuint m_GLShaderProgramID;
		GLenum m_GLShaderProgramType;

	public:
		GDINT explicit IGraphicsOpenGLBaseShader(IGraphicsShaderCreateInfo const* const gfxShaderCreateInfo);
		GDINT virtual ~IGraphicsOpenGLBaseShader();

	public:
		// No public interface functions here..
	};	// class IGraphicsOpenGLBaseShader<T>

	GD_DEFINE_PARTIAL_CLASS(IGraphicsBaseOpenGLWithBaseShaders, IGraphicsOpenGL)
	{
	protected:
		template<IGraphicsShaderType TShaderType>
		GDINT void GfxCmd_BaseShaderBindUniformBuffers(IGraphicsBuffer const* const* const gfxUniformBuffers
			, SizeTp const gfxUniformBuffersCount);
		template<IGraphicsShaderType TShaderType>
		GDINT void GfxCmd_BaseShaderBindResources(IGraphicsShaderResourceView const* const* const gfxResources
			, SizeTp const gfxResourcesCount);
		template<IGraphicsShaderType TShaderType>
		GDINT void GfxCmd_BaseShaderBindSamplers(IGraphicsSampler const* const* const gfxSamplers
			, SizeTp const gfxSamplersCount);
	};	// class IGraphicsBaseOpenGLWithTextures2D

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Vertex shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	typedef IGraphicsOpenGLBaseShader<IGraphicsVertexShader> IGraphicsOpenGLVertexShader;
	GD_DEFINE_PARTIAL_CLASS(IGraphicsOpenGLWithVertexShaders, IGraphicsOpenGL)
	{
	public:
		GDAPI virtual IResult GfxImm_VertexShaderCreate(IGraphicsVertexShader** const gfxVertexShaderPtr
			, IGraphicsShaderCreateInfo const* const gfxShaderCreateInfo
			, IGraphicsVertexArrayLayout const* const gfxVertexArrayLayout) override final;
		GDAPI virtual void GfxCmd_VertexShaderBindUniformBuffers(IGraphicsCommandList* const gfxCommandList
			, IGraphicsBuffer const* const* const gfxUniformBuffers, SizeTp const gfxUniformBuffersCount) override final;
		GDAPI virtual void GfxCmd_VertexShaderBindResources(IGraphicsCommandList* const gfxCommandList
			, IGraphicsShaderResourceView const* const* const gfxResources, SizeTp const gfxResourcesCount) override final;
		GDAPI virtual void GfxCmd_VertexShaderBindSamplers(IGraphicsCommandList* const gfxCommandList
			, IGraphicsSampler const* const* const gfxSamplers, SizeTp const gfxSamplersCount) override final;
	};	// class IGraphicsOpenGLWithVertexShaders

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Pixel (fragment) shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	typedef IGraphicsOpenGLBaseShader<IGraphicsPixelShader> IGraphicsOpenGLPixelShader;
	GD_DEFINE_PARTIAL_CLASS(IGraphicsOpenGLWithPixelShaders, IGraphicsOpenGL)
	{
	public:
		GDAPI virtual IResult GfxImm_PixelShaderCreate(IGraphicsPixelShader** const gfxPixelShaderPtr
			, IGraphicsShaderCreateInfo const* const gfxShaderCreateInfo) override final;
		GDAPI virtual void GfxCmd_PixelShaderBindUniformBuffers(IGraphicsCommandList* const gfxCommandList
			, IGraphicsBuffer const* const* const gfxUniformBuffers, SizeTp const gfxUniformBuffersCount) override final;
		GDAPI virtual void GfxCmd_PixelShaderBindResources(IGraphicsCommandList* const gfxCommandList
			, IGraphicsShaderResourceView const* const* const gfxResources, SizeTp const gfxResourcesCount) override final;
		GDAPI virtual void GfxCmd_PixelShaderBindSamplers(IGraphicsCommandList* const gfxCommandList
			, IGraphicsSampler const* const* const gfxSamplers, SizeTp const gfxSamplersCount) override final;
	};	// class IGraphicsOpenGLWithPixelShaders

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Geometry shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	typedef IGraphicsOpenGLBaseShader<IGraphicsGeometryShader> IGraphicsOpenGLGeometryShader;
	GD_DEFINE_PARTIAL_CLASS(IGraphicsOpenGLWithGeometryShaders, IGraphicsOpenGL)
	{
	public:
		GDAPI virtual IResult GfxImm_GeometryShaderCreate(IGraphicsGeometryShader** const gfxGeometryShaderPtr
			, IGraphicsShaderCreateInfo const* const gfxShaderCreateInfo) override final;
		GDAPI virtual void GfxCmd_GeometryShaderBindUniformBuffers(IGraphicsCommandList* const gfxCommandList
			, IGraphicsBuffer const* const* const gfxUniformBuffers, SizeTp const gfxUniformBuffersCount) override final;
		GDAPI virtual void GfxCmd_GeometryShaderBindResources(IGraphicsCommandList* const gfxCommandList
			, IGraphicsShaderResourceView const* const* const gfxResources, SizeTp const gfxResourcesCount) override final;
		GDAPI virtual void GfxCmd_GeometryShaderBindSamplers(IGraphicsCommandList* const gfxCommandList
			, IGraphicsSampler const* const* const gfxSamplers, SizeTp const gfxSamplersCount) override final;
	};	// class IGraphicsOpenGLWithGeometryShaders

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                          IGraphicsVertexArray interface.                         ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	class IGraphicsOpenGLVertexArray final : public IGraphicsVertexArray
	{
	public:
		GLuint m_GLVertexArrayID;

	public:
		GDAPI explicit IGraphicsOpenGLVertexArray(IGraphicsVertexArrayCreateInfo const* const gfxVertexArrayCreateInfo);
		GDAPI virtual ~IGraphicsOpenGLVertexArray();

	public:
		// No public interface functions here..
	};	// class IGraphicsOpenGLVertexArray
	
	GD_DEFINE_PARTIAL_CLASS(IGraphicsOpenGLWithVertexArrays, IGraphicsOpenGL)
	{
	public:
		GDAPI virtual IResult GfxImm_VertexArrayCreate(IGraphicsVertexArray** const gfxVertexArrayPtr
			, IGraphicsVertexArrayCreateInfo const* const gfxVertexArrayCreateInfo) override final;
		GDAPI virtual void GfxCmd_VertexArrayBind(IGraphicsCommandList* const gfxCommandList
			, IGraphicsVertexArray* const gfxVertexArray) override final;
	};	// class IGraphicsOpenGLWithVertexArrays

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Pipeline state object.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	class IGraphicsOpenGLPipelineState : public IGraphicsPipelineState
	{
	public:
		GLuint m_GLProgramPipelineID;

	public:
		GDAPI explicit IGraphicsOpenGLPipelineState(IGraphicsPipelineStateCreateInfo const* const gfxPipelineStateCreateInfo);
		GDAPI virtual ~IGraphicsOpenGLPipelineState();

	public:
		// No public interface functions here..
	};	// class IGraphicsOpenGLPipelineState
	GD_DEFINE_PARTIAL_CLASS(IGraphicsOpenGLWithPipelineStates, IGraphicsOpenGL)
	{
	public:
		GDAPI virtual IResult GfxImm_PipelineCreate(IGraphicsPipelineState** const gfxPipelinePtr
			, IGraphicsPipelineStateCreateInfo const* const gfxPipelineCreateInfo) override final;
		GDAPI virtual void GfxCmd_PipelineBind(IGraphicsCommandList* const gfxCommandList
			, IGraphicsPipelineState* const gfxPipeline) override final;
	};	// class IGraphicsOpenGLPipelineState

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Render targets.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GD_DEFINE_PARTIAL_CLASS(IGraphicsOpenGLWithRenderTargets, IGraphicsOpenGL)
	{
	public:
		GDAPI virtual void GfxCmd_RenderTargetClearDepth(IGraphicsCommandList* const gfxCommandList) override final;
		GDAPI virtual void GfxCmd_RenderTargetClearColor(IGraphicsCommandList* const gfxCommandList
			, GeomColor const gfxClearColor) override final;
		GDAPI virtual void GfxCmd_RenderTargetRender(IGraphicsCommandList* const gfxCommandList
			, IGraphicsTopologyType const gfxTopology, SizeTp const gfxVerticesCount) override final;
		GDAPI virtual void GfxCmd_RenderTargetRenderIndexed(IGraphicsCommandList* const gfxCommandList
			, IGraphicsTopologyType const gfxTopology, SizeTp const gfxIndicesCount, IGraphicsFormatType gfxIndexType) override final;
	};	// class IGraphicsOpenGLPipelineState

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Global interface.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GD_DEFINE_PARTIAL_CLASS(IGraphicsOpenGL, IGraphicsOpenGL)
	{
		GDAPI virtual IResult OnRuntimeInitialize() override final;
	};	// class IGraphicsOpenGL

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_API_OPENGL
