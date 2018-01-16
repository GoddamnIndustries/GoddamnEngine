// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#define GD_IGRAPHICS_OPENGL_IMPL 1

/*!
 * @file GoddamnEngine/GraphicsVulkan/GraphicsVulkanShaders.cpp
 * File contains implementation for Vulkan graphics interface: shaders.
 */
#include <GoddamnEngine/GraphicsVulkan/GraphicsVulkan.h>
#include <GoddamnEngine/Core/Interaction/Debug.h>

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Base shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	/*!
	 * Creates an Vulkan GPU shader with specified parameters.
	 * @param gfxShaderCreateInfo Creation information for the shader.
	 */
	template<typename TIGraphicsShaderBase>
	GDINT IGraphicsVulkanBaseShader<TIGraphicsShaderBase>::IGraphicsVulkanBaseShader(IGraphicsShaderCreateInfo const* const gfxShaderCreateInfo)
		: TIGraphicsShaderBase(gfxShaderCreateInfo)
	{
	}

	/*!
	 * Deletes the specified GPU shader.
	 */
	template<typename TIGraphicsShaderBase>
	GDINT IGraphicsVulkanBaseShader<TIGraphicsShaderBase>::~IGraphicsVulkanBaseShader()
	{
	}

	/*!
	 * Binds specified uniform buffers into some shader input. Layout of the buffers in
	 * shader and in parameters should be preserved.
	 * 
	 * @param gfxCommandList Command list into which this command would be written.
	 * @param gfxUniformBuffers Pointer to the uniform buffers list.
	 * @param gfxUniformBuffersCount Number of the uniform buffers to bind.
	 */
	template<IGraphicsShaderType TShaderType>
	GDINT void IGraphicsBaseVulkanWithBaseShaders::GfxCmd_BaseShaderBindUniformBuffers(IGraphicsBuffer const* const* const gfxUniformBuffers
		, SizeTp const gfxUniformBuffersCount)
	{
	}

	/*!
	 * Binds specified resources into some shader input. Layout of the resources in
	 * shader and in parameters should be preserved.
	 * 
	 * @param gfxResources Pointer to the resources list.
	 * @param gfxResourcesCount Number of the resources to bind.
	 */
	template<IGraphicsShaderType TShaderType>
	GDINT void IGraphicsBaseVulkanWithBaseShaders::GfxCmd_BaseShaderBindResources(IGraphicsShaderResourceView const* const* const gfxResources
		, SizeTp const gfxResourcesCount)
	{
	}

	/*!
	 * Binds specified samplers into some shader input. Layout of the resources in
	 * shader and in parameters should be preserved.
	 * 
	 * @param gfxSamplers Pointer to the samplers list.
	 * @param gfxSamplersCount Number of the resources to bind.
	 */
	template<IGraphicsShaderType TShaderType>
	GDINT void IGraphicsBaseVulkanWithBaseShaders::GfxCmd_BaseShaderBindSamplers(IGraphicsSampler const* const* const gfxSamplers
		, SizeTp const gfxSamplersCount)
	{
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Vertex shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	/*!
	 * Creates a new Vertex GPU shader with specified parameters and specifies the input layout
	 * of the Vertex GPU shader.
	 *
	 * @param gfxVertexShaderPtr Pointer for output.
	 * @param gfxShaderCreateInfo Creation information for the Vertex shader.
	 * @param gfxVertexArrayLayout input layout of the vertex shader input data.
	 *
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsVulkanWithVertexShaders::GfxImm_VertexShaderCreate(IGraphicsVertexShader** const gfxVertexShaderPtr
		, IGraphicsShaderCreateInfo const* const gfxShaderCreateInfo, IGraphicsVertexArrayLayout const* const gfxVertexArrayLayout)
	{
#if GD_DEBUG
		GD_ARG_VERIFY(gfxVertexShaderPtr != nullptr);
		GD_ARG_VERIFY(gfxShaderCreateInfo != nullptr);
		GD_ARG_VERIFY(gfxShaderCreateInfo->ShaderSource || gfxShaderCreateInfo->ShaderByteCode != nullptr);
		GD_ARG_VERIFY(gfxShaderCreateInfo->ShaderSource || gfxShaderCreateInfo->ShaderByteCodeLength > 0);
#endif	// if GD_DEBUG

		// Input layouts are not used in Vulkan.
		GD_NOT_USED(gfxVertexArrayLayout);

		// Copying parameters and forcedly specifying Vertex shader type.
		auto gfxShaderCreateInfoCopy(*gfxShaderCreateInfo);
		gfxShaderCreateInfoCopy.ShaderType = IGRAPHICS_SHADER_TYPE_VERTEX;
		
		*gfxVertexShaderPtr = gd_new IGraphicsVulkanVertexShader(&gfxShaderCreateInfoCopy);
		return IResult::Ok;
	}

	/*!
	 * Binds specified uniform buffers into Vertex shader input. Layout of the buffers in
	 * shader and in parameters should be preserved.
	 * 
	 * @param gfxCommandList Command list into which this command would be written.
	 * @param gfxUniformBuffers Pointer to the uniform buffers list.
	 * @param gfxUniformBuffersCount Number of the uniform buffers to bind.
	 */
	GDAPI void IGraphicsVulkanWithVertexShaders::GfxCmd_VertexShaderBindUniformBuffers(IGraphicsCommandList* const gfxCommandList
		, IGraphicsBuffer const* const* const gfxUniformBuffers, SizeTp const gfxUniformBuffersCount)
	{
		GD_NOT_USED(gfxCommandList);
		GfxCmd_BaseShaderBindUniformBuffers<IGRAPHICS_SHADER_TYPE_VERTEX>(gfxUniformBuffers, gfxUniformBuffersCount);
	}

	/*!
	 * Binds specified resources into Vertex shader input. Layout of the resources in
	 * shader and in parameters should be preserved.
	 * 
	 * @param gfxCommandList Command list into which this command would be written.
	 * @param gfxResources Pointer to the resources list.
	 * @param gfxResourcesCount Number of the resources to bind.
	 */
	GDAPI void IGraphicsVulkanWithVertexShaders::GfxCmd_VertexShaderBindResources(IGraphicsCommandList* const gfxCommandList
		, IGraphicsShaderResourceView const* const* const gfxResources, SizeTp const gfxResourcesCount)
	{
		GD_NOT_USED(gfxCommandList);
		GfxCmd_BaseShaderBindResources<IGRAPHICS_SHADER_TYPE_VERTEX>(gfxResources, gfxResourcesCount);
	}

	/*!
	 * Binds specified samplers into Vertex shader input. Layout of the resources in
	 * shader and in parameters should be preserved.
	 * 
	 * @param gfxCommandList Command list into which this command would be written.
	 * @param gfxSamplers Pointer to the samplers list.
	 * @param gfxSamplersCount Number of the resources to bind.
	 */
	GDAPI void IGraphicsVulkanWithVertexShaders::GfxCmd_VertexShaderBindSamplers(IGraphicsCommandList* const gfxCommandList
		, IGraphicsSampler const* const* const gfxSamplers, SizeTp const gfxSamplersCount)
	{
		GD_NOT_USED(gfxCommandList);
		GfxCmd_BaseShaderBindSamplers<IGRAPHICS_SHADER_TYPE_VERTEX>(gfxSamplers, gfxSamplersCount);
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Pixel shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	/*!
	 * Creates a new Pixel GPU shader with specified parameters and specifies the input layout
	 * of the Pixel GPU shader.
	 *
	 * @param gfxPixelShaderPtr Pointer for output.
	 * @param gfxShaderCreateInfo Creation information for the Pixel shader.
	 *
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsVulkanWithPixelShaders::GfxImm_PixelShaderCreate(IGraphicsPixelShader** const gfxPixelShaderPtr
		, IGraphicsShaderCreateInfo const* const gfxShaderCreateInfo)
	{
#if GD_DEBUG
		GD_ARG_VERIFY(gfxPixelShaderPtr != nullptr);
		GD_ARG_VERIFY(gfxShaderCreateInfo != nullptr);
		GD_ARG_VERIFY(gfxShaderCreateInfo->ShaderSource || gfxShaderCreateInfo->ShaderByteCode != nullptr);
		GD_ARG_VERIFY(gfxShaderCreateInfo->ShaderSource || gfxShaderCreateInfo->ShaderByteCodeLength > 0);
#endif	// if GD_DEBUG

		// Copying parameters and forcedly specifying Pixel shader type.
		auto gfxShaderCreateInfoCopy(*gfxShaderCreateInfo);
		gfxShaderCreateInfoCopy.ShaderType = IGRAPHICS_SHADER_TYPE_PIXEL;
		
		*gfxPixelShaderPtr = gd_new IGraphicsVulkanPixelShader(&gfxShaderCreateInfoCopy);
		return IResult::Ok;
	}

	/*!
	 * Binds specified uniform buffers into Pixel shader input. Layout of the buffers in
	 * shader and in parameters should be preserved.
	 * 
	 * @param gfxCommandList Command list into which this command would be written.
	 * @param gfxUniformBuffers Pointer to the uniform buffers list.
	 * @param gfxUniformBuffersCount Number of the uniform buffers to bind.
	 */
	GDAPI void IGraphicsVulkanWithPixelShaders::GfxCmd_PixelShaderBindUniformBuffers(IGraphicsCommandList* const gfxCommandList
		, IGraphicsBuffer const* const* const gfxUniformBuffers, SizeTp const gfxUniformBuffersCount)
	{
		GD_NOT_USED(gfxCommandList);
		GfxCmd_BaseShaderBindUniformBuffers<IGRAPHICS_SHADER_TYPE_PIXEL>(gfxUniformBuffers, gfxUniformBuffersCount);
	}

	/*!
	 * Binds specified resources into Pixel shader input. Layout of the resources in
	 * shader and in parameters should be preserved.
	 * 
	 * @param gfxCommandList Command list into which this command would be written.
	 * @param gfxResources Pointer to the resources list.
	 * @param gfxResourcesCount Number of the resources to bind.
	 */
	GDAPI void IGraphicsVulkanWithPixelShaders::GfxCmd_PixelShaderBindResources(IGraphicsCommandList* const gfxCommandList
		, IGraphicsShaderResourceView const* const* const gfxResources, SizeTp const gfxResourcesCount)
	{
		GD_NOT_USED(gfxCommandList);
		GfxCmd_BaseShaderBindResources<IGRAPHICS_SHADER_TYPE_PIXEL>(gfxResources, gfxResourcesCount);
	}

	/*!
	 * Binds specified samplers into Pixel shader input. Layout of the resources in
	 * shader and in parameters should be preserved.
	 * 
	 * @param gfxCommandList Command list into which this command would be written.
	 * @param gfxSamplers Pointer to the samplers list.
	 * @param gfxSamplersCount Number of the resources to bind.
	 */
	GDAPI void IGraphicsVulkanWithPixelShaders::GfxCmd_PixelShaderBindSamplers(IGraphicsCommandList* const gfxCommandList
		, IGraphicsSampler const* const* const gfxSamplers, SizeTp const gfxSamplersCount)
	{
		GD_NOT_USED(gfxCommandList);
		GfxCmd_BaseShaderBindSamplers<IGRAPHICS_SHADER_TYPE_PIXEL>(gfxSamplers, gfxSamplersCount);
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Geometry shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	/*!
	 * Creates a new Geometry GPU shader with specified parameters and specifies the input layout
	 * of the Geometry GPU shader.
	 *
	 * @param gfxGeometryShaderPtr Pointer for output.
	 * @param gfxShaderCreateInfo Creation information for the Geometry shader.
	 *
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsVulkanWithGeometryShaders::GfxImm_GeometryShaderCreate(IGraphicsGeometryShader** const gfxGeometryShaderPtr
		, IGraphicsShaderCreateInfo const* const gfxShaderCreateInfo)
	{
#if GD_DEBUG
		GD_ARG_VERIFY(gfxGeometryShaderPtr != nullptr);
		GD_ARG_VERIFY(gfxShaderCreateInfo != nullptr);
		GD_ARG_VERIFY(gfxShaderCreateInfo->ShaderSource || gfxShaderCreateInfo->ShaderByteCode != nullptr);
		GD_ARG_VERIFY(gfxShaderCreateInfo->ShaderSource || gfxShaderCreateInfo->ShaderByteCodeLength > 0);
#endif	// if GD_DEBUG

		// Copying parameters and forcedly specifying Geometry shader type.
		auto gfxShaderCreateInfoCopy(*gfxShaderCreateInfo);
		gfxShaderCreateInfoCopy.ShaderType = IGRAPHICS_SHADER_TYPE_GEOMETRY;

		*gfxGeometryShaderPtr = gd_new IGraphicsVulkanGeometryShader(&gfxShaderCreateInfoCopy);
		return IResult::Ok;
	}

	/*!
	 * Binds specified uniform buffers into Geometry shader input. Layout of the buffers in
	 * shader and in parameters should be preserved.
	 * 
	 * @param gfxCommandList Command list into which this command would be written.
	 * @param gfxUniformBuffers Pointer to the uniform buffers list.
	 * @param gfxUniformBuffersCount Number of the uniform buffers to bind.
	 */
	GDAPI void IGraphicsVulkanWithGeometryShaders::GfxCmd_GeometryShaderBindUniformBuffers(IGraphicsCommandList* const gfxCommandList
		, IGraphicsBuffer const* const* const gfxUniformBuffers, SizeTp const gfxUniformBuffersCount)
	{
		GD_NOT_USED(gfxCommandList);
		GfxCmd_BaseShaderBindUniformBuffers<IGRAPHICS_SHADER_TYPE_GEOMETRY>(gfxUniformBuffers, gfxUniformBuffersCount);
	}

	/*!
	 * Binds specified resources into Geometry shader input. Layout of the resources in
	 * shader and in parameters should be preserved.
	 *
	 * @param gfxCommandList Command list into which this command would be written.
	 * @param gfxResources Pointer to the resources list.
	 * @param gfxResourcesCount Number of the resources to bind.
	 */
	GDAPI void IGraphicsVulkanWithGeometryShaders::GfxCmd_GeometryShaderBindResources(IGraphicsCommandList* const gfxCommandList
		, IGraphicsShaderResourceView const* const* const gfxResources, SizeTp const gfxResourcesCount)
	{
		GD_NOT_USED(gfxCommandList);
		GfxCmd_BaseShaderBindResources<IGRAPHICS_SHADER_TYPE_GEOMETRY>(gfxResources, gfxResourcesCount);
	}

	/*!
	 * Binds specified samplers into Geometry shader input. Layout of the resources in
	 * shader and in parameters should be preserved.
	 * 
	 * @param gfxCommandList Command list into which this command would be written.
	 * @param gfxSamplers Pointer to the samplers list.
	 * @param gfxSamplersCount Number of the resources to bind.
	 */
	GDAPI void IGraphicsVulkanWithGeometryShaders::GfxCmd_GeometryShaderBindSamplers(IGraphicsCommandList* const gfxCommandList
		, IGraphicsSampler const* const* const gfxSamplers, SizeTp const gfxSamplersCount)
	{
		GD_NOT_USED(gfxCommandList);
		GfxCmd_BaseShaderBindSamplers<IGRAPHICS_SHADER_TYPE_GEOMETRY>(gfxSamplers, gfxSamplersCount);
	}

GD_NAMESPACE_END
