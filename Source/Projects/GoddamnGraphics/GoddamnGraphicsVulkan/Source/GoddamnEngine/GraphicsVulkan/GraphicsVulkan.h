// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/GraphicsVulkan/GraphicsVulkan.h
 * File contains export interface for Vulkan Implementation of the graphics interface.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Interface/Graphics/Graphics.h>

#define GD_PLATFORM_API_OPENGL GD_PLATFORM_DESKTOP || (GD_PLATFORM_MOBILE && !GD_PLATFORM_API_MICROSOFT)
#if GD_PLATFORM_API_OPENGL

#define VK_USE_PLATFORM_WIN32_KHR 1

#include <GoddamnEngine/Core/BasePlatform.h>
#include GD_PLATFORM_INCLUDE(GoddamnEngine/GraphicsVulkan, GraphicsVulkan)

#if !defined(GD_IGRAPHICS_OPENGL_IMPL)
#	define GD_IGRAPHICS_OPENGL_IMPL GD_TRUE
#endif	// if !defined(GD_IGRAPHICS_OPENGL_IMPL)

#include <vulkan/vulkan.h>

GD_NAMESPACE_BEGIN

	struct IGraphicsVulkan;
	GD_DEFINE_PARTIAL_CALSS_FIRST(IGraphicsVulkan, IGraphicsVulkanPlatform);
	GDEXP extern IGraphics* CreateIGraphicsInstance();

#define vkThrowIfFailed(...) ThrowIfFailed(__VA_ARGS__)
	GDINL static void ThrowIfFailed(VkResult const vkResult)
	{
		GD_VERIFY(vkResult == 0, "WINAPI\\D3D11 function has failed.");
	}


	GD_DEFINE_PARTIAL_CLASS(IGraphicsVulkanCrap, IGraphicsVulkan)
	{
	public:
		VkInstance m_VkInstance;
		Vector<CStr> m_VkEnabledExtensions = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME };
		VkPhysicalDevice m_VkPhysicalDevice;
		VkDevice m_VkDevice;
		VkQueue m_VkQueue;
		VkSurfaceKHR m_VkSurface;
		VkSwapchainKHR m_VkSwapchain;
		VkCommandPool m_VkCommandPool;
		VkCommandBuffer m_VkCommandBuffer;

	public:
		IResult Cpu_FindMemoryType(uint32_t& r, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
			VkPhysicalDeviceMemoryProperties memProperties;
			vkGetPhysicalDeviceMemoryProperties(m_VkPhysicalDevice, &memProperties);
			for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
				if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
					r = i;
					return IResult::Ok;
				}
			}
			return IResult::Error;
		}
	};	// class IGraphicsVulkanWithCommandLists

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                         IGraphicsCommandList interface.                          ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	GD_DEFINE_PARTIAL_CLASS(IGraphicsVulkanWithCommandLists, IGraphicsVulkan)
	{
	public:
		GDAPI virtual IResult GfxImm_CommandListCreate(IGraphicsCommandList** const gfxCommandListPtr
			, IGraphicsCommandListCreateInfo const* const gfxCommandListCreateInfo) override final;
	};	// class IGraphicsVulkanWithCommandLists

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                            IGraphicsBuffer interface.                            ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	class IGraphicsVulkanBuffer final : public IGraphicsBuffer
	{
	public:
		VkBuffer const m_VKBufferID;
		VkDeviceMemory const m_VKBufferMemoryID;
		Handle m_VkBufferMappedMemory = nullptr;

	public:
		GDINL explicit IGraphicsVulkanBuffer(IGraphicsBufferCreateInfo const* const gfxBufferCreateInfo
			, VkBuffer const vkBufferID, VkDeviceMemory const vkBufferMemoryID)
			: IGraphicsBuffer(gfxBufferCreateInfo), m_VKBufferID(vkBufferID), m_VKBufferMemoryID(vkBufferMemoryID) {}
	};	// class IGraphicsVulkanBuffer

	GD_DEFINE_PARTIAL_CLASS(IGraphicsVulkanWithBuffers, IGraphicsVulkan)
	{
	public:
		GDAPI virtual IResult GfxImm_BufferCreate(IGraphicsBuffer** const gfxBufferPtr
			, IGraphicsBufferCreateInfo const* const gfxBufferCreateInfo) override final;
		GDAPI virtual IResult GfxImm_BufferDestroy(IGraphicsBuffer* const gfxBufferPtr) override final;
		GDAPI virtual IResult GfxImm_BufferMap(IGraphicsBuffer* const gfxBuffer
			, IGraphicsResourceMapMode const gfxMapMode, Handle* const gfxMappedMemory) override final;
		GDAPI virtual IResult GfxImm_BufferUnmap(IGraphicsBuffer* const gfxBuffer) override final;
	};	// class IGraphicsVulkanWithBuffers

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                          IGraphicsVertexArray interface.                         ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	class IGraphicsVulkanVertexArray final : public IGraphicsVertexArray
	{
	public:

	public:
		GDAPI explicit IGraphicsVulkanVertexArray(IGraphicsVertexArrayCreateInfo const* const gfxVertexArrayCreateInfo);
		GDAPI virtual ~IGraphicsVulkanVertexArray();

	public:
		// No public interface functions here..
	};	// class IGraphicsVulkanVertexArray

	GD_DEFINE_PARTIAL_CLASS(IGraphicsVulkanWithVertexArrays, IGraphicsVulkan)
	{
	public:
		GDAPI virtual IResult GfxImm_VertexArrayCreate(IGraphicsVertexArray** const gfxVertexArrayPtr
			, IGraphicsVertexArrayCreateInfo const* const gfxVertexArrayCreateInfo) override final;
		GDAPI virtual void GfxCmd_VertexArrayBind(IGraphicsCommandList* const gfxCommandList
			, IGraphicsVertexArray* const gfxVertexArray) override final;
	};	// class IGraphicsVulkanWithVertexArrays

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                         IGraphicsSampler interface.                         ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	class IGraphicsVulkanSampler final : public IGraphicsSampler
	{
	public:
		VkSampler const m_VKSamplerID = VK_NULL_HANDLE;

	public:
		GDAPI explicit IGraphicsVulkanSampler(IGraphicsSamplerCreateInfo const* const gfxSamplerCreateInfo
			, VkSampler const vkSamplerID)
			: IGraphicsSampler(gfxSamplerCreateInfo), m_VKSamplerID(vkSamplerID) {}
	};	// class IGraphicsVulkanSampler

	GD_DEFINE_PARTIAL_CLASS(IGraphicsVulkanWithSamplers, IGraphicsVulkan)
	{
	public:
		GDAPI virtual IResult GfxImm_SamplerCreate(IGraphicsSampler** const gfxSamplerPtr
			, IGraphicsSamplerCreateInfo const* const gfxSamplerCreateInfo) override final;
		GDAPI virtual IResult GfxImm_SamplerDestroy(IGraphicsSampler* const gfxSampler) override final;
	};	// class IGraphicsVulkanWithSamplers

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                         IGraphicsResource(View) interface.                       ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	class IGraphicsVulkanShaderResourceView : public IGraphicsShaderResourceView
	{
	public:

	public:
		GDAPI explicit IGraphicsVulkanShaderResourceView(IGraphicsShaderResourceType const gfxResourceType);
		GDAPI virtual ~IGraphicsVulkanShaderResourceView();

	public:
		// No public interface functions here..
	};	// class IGraphicsShaderResourceView

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                            IGraphicsTexture2D interface.                         ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	class IGraphicsVulkanTexture2D final : public IGraphicsTexture2D
	{
	public:
	
	public:
		GDAPI explicit IGraphicsVulkanTexture2D(IGraphicsTexture2DCreateInfo const* const gfxTexture2DCreateInfo
			, IGraphicsTexture2DData const* const gfxTexture2DInitialData);
		GDAPI virtual ~IGraphicsVulkanTexture2D();

	public:
		GDAPI virtual IGraphicsShaderResourceView const* CPU_GetShaderResourceView() const override final;
		GDAPI virtual IResult Imm_UpdateData(IGraphicsTexture2DData const* const gfxTexture2DData) override final;
	};	// class IGraphicsVulkanTexture2D
	GD_DEFINE_PARTIAL_CLASS(IGraphicsBaseVulkanWithTextures2D, IGraphicsVulkan)
	{
	public:
		GDAPI virtual IResult GfxImm_Texture2DCreate(IGraphicsTexture2D** const gfxTexture2DPtr
			, IGraphicsTexture2DCreateInfo const* const gfxTexture2DCreateInfo
			, IGraphicsTexture2DData const* const gfxTexture2DData) override final;
	};	// class IGraphicsBaseVulkanWithTextures2D
	
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                           IGraphicsTextureCube interface.                        ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	class IGraphicsVulkanTextureCube final : public IGraphicsTextureCube
	{
	public:
	
	public:
		GDAPI explicit IGraphicsVulkanTextureCube(IGraphicsTextureCubeCreateInfo const* const gfxTextureCubeCreateInfo
			, IGraphicsTextureCubeData const* const gfxTextureCubeInitialData);
		GDAPI virtual ~IGraphicsVulkanTextureCube();

	public:
		GDAPI virtual IGraphicsShaderResourceView const* CPU_GetShaderResourceView() const override final;
		GDAPI virtual IResult Imm_UpdateData(IGraphicsTextureCubeData const* const gfxTextureCubeData) override final;
	};	// class IGraphicsVulkanTextureCube

	GD_DEFINE_PARTIAL_CLASS(IGraphicsBaseVulkanWithTexturesCube, IGraphicsVulkan)
	{
	public:
		GDAPI virtual IResult GfxImm_TextureCubeCreate(IGraphicsTextureCube** const gfxTextureCubePtr
			, IGraphicsTextureCubeCreateInfo const* const gfxTextureCubeCreateInfo
			, IGraphicsTextureCubeData const* const gfxTextureInitialCubeData) override final;
	};	// class IGraphicsBaseVulkanWithTexturesCube

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                             IGraphicsShader interface.                           ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	template<typename TIGraphicsShaderBase>
	class IGraphicsVulkanBaseShader : public TIGraphicsShaderBase
	{
	public:

	public:
		GDINT explicit IGraphicsVulkanBaseShader(IGraphicsShaderCreateInfo const* const gfxShaderCreateInfo);
		GDINT virtual ~IGraphicsVulkanBaseShader();

	public:
		// No public interface functions here..
	};	// class IGraphicsVulkanBaseShader<T>

	GD_DEFINE_PARTIAL_CLASS(IGraphicsBaseVulkanWithBaseShaders, IGraphicsVulkan)
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
	};	// class IGraphicsBaseVulkanWithTextures2D

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Vertex shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	typedef IGraphicsVulkanBaseShader<IGraphicsVertexShader> IGraphicsVulkanVertexShader;
	GD_DEFINE_PARTIAL_CLASS(IGraphicsVulkanWithVertexShaders, IGraphicsVulkan)
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
	};	// class IGraphicsVulkanWithVertexShaders

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Pixel (fragment) shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	typedef IGraphicsVulkanBaseShader<IGraphicsPixelShader> IGraphicsVulkanPixelShader;
	GD_DEFINE_PARTIAL_CLASS(IGraphicsVulkanWithPixelShaders, IGraphicsVulkan)
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
	};	// class IGraphicsVulkanWithPixelShaders

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Geometry shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	typedef IGraphicsVulkanBaseShader<IGraphicsGeometryShader> IGraphicsVulkanGeometryShader;
	GD_DEFINE_PARTIAL_CLASS(IGraphicsVulkanWithGeometryShaders, IGraphicsVulkan)
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
	};	// class IGraphicsVulkanWithGeometryShaders

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Pipeline state object.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	class IGraphicsVulkanPipelineState : public IGraphicsPipelineState
	{
	public:

	public:
		GDAPI explicit IGraphicsVulkanPipelineState(IGraphicsPipelineStateCreateInfo const* const gfxPipelineStateCreateInfo);
		GDAPI virtual ~IGraphicsVulkanPipelineState();

	public:
		// No public interface functions here..
	};	// class IGraphicsVulkanPipelineState
	GD_DEFINE_PARTIAL_CLASS(IGraphicsVulkanWithPipelineStates, IGraphicsVulkan)
	{
	public:
		GDAPI virtual IResult GfxImm_PipelineCreate(IGraphicsPipelineState** const gfxPipelinePtr
			, IGraphicsPipelineStateCreateInfo const* const gfxPipelineCreateInfo) override final;
		GDAPI virtual void GfxCmd_PipelineBind(IGraphicsCommandList* const gfxCommandList
			, IGraphicsPipelineState* const gfxPipeline) override final;
	};	// class IGraphicsVulkanPipelineState

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Render targets.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GD_DEFINE_PARTIAL_CLASS(IGraphicsVulkanWithRenderTargets, IGraphicsVulkan)
	{
	public:
		GDAPI virtual void GfxCmd_RenderTargetClearDepth(IGraphicsCommandList* const gfxCommandList) override final;
		GDAPI virtual void GfxCmd_RenderTargetClearColor(IGraphicsCommandList* const gfxCommandList
			, GeomColor const gfxClearColor) override final;
		GDAPI virtual void GfxCmd_RenderTargetRender(IGraphicsCommandList* const gfxCommandList
			, IGraphicsTopologyType const gfxTopology, SizeTp const gfxVerticesCount) override final;
		GDAPI virtual void GfxCmd_RenderTargetRenderIndexed(IGraphicsCommandList* const gfxCommandList
			, IGraphicsTopologyType const gfxTopology, SizeTp const gfxIndicesCount, IGraphicsFormatType gfxIndexType) override final;
	};	// class IGraphicsVulkanPipelineState

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Global interface.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GD_DEFINE_PARTIAL_CLASS(IGraphicsVulkan, IGraphicsVulkan)
	{
	public:


	public:
		GDAPI virtual IResult OnRuntimePreInitialize() override;
		GDAPI virtual IResult OnRuntimeInitialize() override final;
		GDAPI virtual void OnRuntimePreUpdate() override;
	};	// class IGraphicsVulkan

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_API_OPENGL
