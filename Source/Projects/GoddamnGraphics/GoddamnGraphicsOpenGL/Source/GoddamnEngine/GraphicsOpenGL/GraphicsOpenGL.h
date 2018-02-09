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
	GDINL static IResult CheckIfFailed()
	{
		//auto a = glGetError();
		//GD_VERIFY(a == GL_NO_ERROR, "WINAPI\\D3D11 function has failed.");
	}

	GDEXP extern IGraphics* CreateIGraphicsInstance();
	GD_DEFINE_PARTIAL_CLASS_FIRST(IGraphicsOpenGL, IGraphicsOpenGLPlatform);

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                         IGraphicsCommandList interface.                          ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	GD_DEFINE_PARTIAL_CLASS(IGraphicsOpenGLWithCommandLists, IGraphicsOpenGL)
	{
	public:
		GDAPI virtual IResult GfxImm_CommandListCreate(IGraphicsCommandList*& gfxObjPtr, IGraphicsCommandListCreateInfo const& gfxObjCreateInfo) override final;
		GDAPI virtual IResult GfxImm_CommandListDestroy(IGraphicsCommandList* const gfxObj) override final;
	};	// class IGraphicsOpenGLWithCommandLists

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                           Graphics Resource interface.                           ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	// ------------------------------------------------------------------------------------------
	// Buffers.
	// ------------------------------------------------------------------------------------------

	class IGraphicsOpenGLBuffer final : public IGraphicsBuffer
	{
	public:
		GLuint const BufferID;
		GLenum const BufferType;
		Handle BufferMappedMemory = nullptr;
	public:
		GDINL explicit IGraphicsOpenGLBuffer(IGraphicsBufferCreateInfo const& gfxBufferCreateInfo, GLuint const glBufferID, GLenum const glBufferType)
			: IGraphicsBuffer(gfxBufferCreateInfo), BufferID(glBufferID), BufferType(glBufferType) {}
	};	// class IGraphicsOpenGLBuffer

	GD_DEFINE_PARTIAL_CLASS(IGraphicsOpenGLWithBuffers, IGraphicsOpenGL)
	{
	public:
		GDAPI virtual IResult GfxImm_BufferCreate(IGraphicsBuffer*& gfxBufferPtr, IGraphicsBufferCreateInfo const& gfxBufferCreateInfo, CHandle const gfxBufferInitialData) override final;
		GDAPI virtual IResult GfxImm_BufferDestroy(IGraphicsBuffer* const gfxBufferPtr) override final;
		GDAPI virtual IResult GfxImm_BufferMap(IGraphicsBuffer* const gfxBuffer, IGraphicsResourceMapMode const gfxMapMode, Handle& gfxMappedMemory) override final;
		GDAPI virtual IResult GfxImm_BufferUnmap(IGraphicsBuffer* const gfxBuffer) override final;
	};	// class IGraphicsOpenGLWithBuffers

	// ------------------------------------------------------------------------------------------
	// 2D, 3D Texture Arrays and Cube textures.
	// ------------------------------------------------------------------------------------------

	class IGraphicsOpenGLTexture2D final : public IGraphicsTexture2D
	{
	public:
		GLuint const Texture2DID;
		GLuint const Texture2DBufferID;
	public:
		GDINL explicit IGraphicsOpenGLTexture2D(IGraphicsTexture2DCreateInfo const& gfxTexture2DCreateInfo, GLuint const glTexture2DID, GLuint const glTexture2DBufferID)
			: IGraphicsTexture2D(gfxTexture2DCreateInfo), Texture2DID(glTexture2DID), Texture2DBufferID(glTexture2DBufferID) {}
	};	// class IGraphicsOpenGLTexture2D

	class IGraphicsOpenGLTexture3D final : public IGraphicsTexture3D
	{
	public:
		GLuint const Texture3DID;
		GLuint const Texture3DBufferID;
	public:
		GDINL explicit IGraphicsOpenGLTexture3D(IGraphicsTexture3DCreateInfo const& gfxTexture3DCreateInfo, GLuint const glTexture3DID, GLuint const glTexture3DBufferID)
			: IGraphicsTexture3D(gfxTexture3DCreateInfo), Texture3DID(glTexture3DID), Texture3DBufferID(glTexture3DBufferID) {}
	};	// class IGraphicsOpenGLTexture3D

	GD_DEFINE_PARTIAL_CLASS(IGraphicsOpenGLWithTextures, IGraphicsOpenGL)
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
	};	// class IGraphicsOpenGLWithTextures

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                              Graphics View interface.                            ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	class IGraphicsOpenGLShaderResourceView : public IGraphicsShaderResourceView
	{
	public:
		GLuint ShaderResourceID;
		GLenum ShaderResourceType;
	public:
		GDAPI explicit IGraphicsOpenGLShaderResourceView(IGraphicsShaderResourceViewType const gfxResourceType);
		GDAPI virtual ~IGraphicsOpenGLShaderResourceView();
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
		GDINT explicit IGraphicsOpenGLBaseShader(IGraphicsShaderCreateInfo const& gfxShaderCreateInfo);
		GDINT virtual ~IGraphicsOpenGLBaseShader();

	public:
		// No public interface functions here..
	};	// class IGraphicsOpenGLBaseShader<T>

	GD_DEFINE_PARTIAL_CLASS(IGraphicsBaseOpenGLWithBaseShaders, IGraphicsOpenGL)
	{
	protected:
		template<IGraphicsShaderType TShaderType>
		GDINT IResult GfxCmd_BaseShaderBindUniformBuffers(IGraphicsBuffer const* const* const gfxUniformBuffers
			, SizeTp const gfxUniformBuffersCount);
		template<IGraphicsShaderType TShaderType>
		GDINT IResult GfxCmd_BaseShaderBindResources(IGraphicsShaderResourceView const* const* const gfxResources
			, SizeTp const gfxResourcesCount);
		template<IGraphicsShaderType TShaderType>
		GDINT IResult GfxCmd_BaseShaderBindSamplers(IGraphicsSampler const* const* const gfxSamplers
			, SizeTp const gfxSamplersCount);
	};	// class IGraphicsBaseOpenGLWithTextures2D

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Vertex shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	typedef IGraphicsOpenGLBaseShader<IGraphicsVertexShader> IGraphicsOpenGLVertexShader;
	GD_DEFINE_PARTIAL_CLASS(IGraphicsOpenGLWithVertexShaders, IGraphicsOpenGL)
	{
	public:
		GDAPI virtual IResult GfxImm_VertexShaderCreate(IGraphicsVertexShader*& gfxVertexShaderPtr
			, IGraphicsShaderCreateInfo const& gfxShaderCreateInfo
			, IGraphicsVertexArrayLayout const* const gfxVertexArrayLayout) override final;
		GDAPI virtual IResult GfxCmd_VertexShaderBindUniformBuffers(IGraphicsCommandList* const gfxCommandList
			, IGraphicsBuffer const* const* const gfxUniformBuffers, SizeTp const gfxUniformBuffersCount) override final;
		GDAPI virtual IResult GfxCmd_VertexShaderBindResources(IGraphicsCommandList* const gfxCommandList
			, IGraphicsShaderResourceView const* const* const gfxResources, SizeTp const gfxResourcesCount) override final;
		GDAPI virtual IResult GfxCmd_VertexShaderBindSamplers(IGraphicsCommandList* const gfxCommandList
			, IGraphicsSampler const* const* const gfxSamplers, SizeTp const gfxSamplersCount) override final;
	};	// class IGraphicsOpenGLWithVertexShaders

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Pixel (fragment) shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	typedef IGraphicsOpenGLBaseShader<IGraphicsFragmentShader> IGraphicsOpenGLFragmentShader;
	GD_DEFINE_PARTIAL_CLASS(IGraphicsOpenGLWithFragmentShaders, IGraphicsOpenGL)
	{
	public:
		GDAPI virtual IResult GfxImm_FragmentShaderCreate(IGraphicsFragmentShader*& gfxFragmentShaderPtr
			, IGraphicsShaderCreateInfo const& gfxShaderCreateInfo) override final;
		GDAPI virtual IResult GfxCmd_FragmentShaderBindUniformBuffers(IGraphicsCommandList* const gfxCommandList
			, IGraphicsBuffer const* const* const gfxUniformBuffers, SizeTp const gfxUniformBuffersCount) override final;
		GDAPI virtual IResult GfxCmd_FragmentShaderBindResources(IGraphicsCommandList* const gfxCommandList
			, IGraphicsShaderResourceView const* const* const gfxResources, SizeTp const gfxResourcesCount) override final;
		GDAPI virtual IResult GfxCmd_FragmentShaderBindSamplers(IGraphicsCommandList* const gfxCommandList
			, IGraphicsSampler const* const* const gfxSamplers, SizeTp const gfxSamplersCount) override final;
	};	// class IGraphicsOpenGLWithFragmentShaders

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Geometry shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	typedef IGraphicsOpenGLBaseShader<IGraphicsGeometryShader> IGraphicsOpenGLGeometryShader;
	GD_DEFINE_PARTIAL_CLASS(IGraphicsOpenGLWithGeometryShaders, IGraphicsOpenGL)
	{
	public:
		GDAPI virtual IResult GfxImm_GeometryShaderCreate(IGraphicsGeometryShader*& gfxGeometryShaderPtr
			, IGraphicsShaderCreateInfo const& gfxShaderCreateInfo) override final;
		GDAPI virtual IResult GfxCmd_GeometryShaderBindUniformBuffers(IGraphicsCommandList* const gfxCommandList
			, IGraphicsBuffer const* const* const gfxUniformBuffers, SizeTp const gfxUniformBuffersCount) override final;
		GDAPI virtual IResult GfxCmd_GeometryShaderBindResources(IGraphicsCommandList* const gfxCommandList
			, IGraphicsShaderResourceView const* const* const gfxResources, SizeTp const gfxResourcesCount) override final;
		GDAPI virtual IResult GfxCmd_GeometryShaderBindSamplers(IGraphicsCommandList* const gfxCommandList
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
		GDAPI explicit IGraphicsOpenGLVertexArray(IGraphicsVertexArrayCreateInfo const& gfxVertexArrayCreateInfo);
		GDAPI virtual ~IGraphicsOpenGLVertexArray();

	public:
		// No public interface functions here..
	};	// class IGraphicsOpenGLVertexArray
	
	GD_DEFINE_PARTIAL_CLASS(IGraphicsOpenGLWithVertexArrays, IGraphicsOpenGL)
	{
	public:
		GDAPI virtual IResult GfxImm_VertexArrayCreate(IGraphicsVertexArray*& gfxVertexArrayPtr
			, IGraphicsVertexArrayCreateInfo const& gfxVertexArrayCreateInfo) override final;
		GDAPI virtual IResult GfxCmd_VertexArrayBind(IGraphicsCommandList* const gfxCommandList
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
		GDAPI explicit IGraphicsOpenGLPipelineState(IGraphicsPipelineStateCreateInfo const& gfxPipelineStateCreateInfo);
		GDAPI virtual ~IGraphicsOpenGLPipelineState();

	public:
		// No public interface functions here..
	};	// class IGraphicsOpenGLPipelineState
	GD_DEFINE_PARTIAL_CLASS(IGraphicsOpenGLWithPipelineStates, IGraphicsOpenGL)
	{
	public:
		GDAPI virtual IResult GfxImm_PipelineCreate(IGraphicsPipelineState*& gfxPipelinePtr
			, IGraphicsPipelineStateCreateInfo const& gfxPipelineCreateInfo) override final;
		GDAPI virtual IResult GfxCmd_PipelineBind(IGraphicsCommandList* const gfxCommandList
			, IGraphicsPipelineState* const gfxPipeline) override final;
	};	// class IGraphicsOpenGLPipelineState

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Render targets.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GD_DEFINE_PARTIAL_CLASS(IGraphicsOpenGLWithRenderTargets, IGraphicsOpenGL)
	{
	public:
		GDAPI virtual IResult GfxCmd_RenderTargetClearDepth(IGraphicsCommandList* const gfxCommandList) override final;
		GDAPI virtual IResult GfxCmd_RenderTargetClearColor(IGraphicsCommandList* const gfxCommandList
			, GeomColor const gfxClearColor) override final;
		GDAPI virtual IResult GfxCmd_RenderTargetRender(IGraphicsCommandList* const gfxCommandList
			, IGraphicsTopologyType const gfxTopology, SizeTp const gfxVerticesCount) override final;
		GDAPI virtual IResult GfxCmd_RenderTargetRenderIndexed(IGraphicsCommandList* const gfxCommandList
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
