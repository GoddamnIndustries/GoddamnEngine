// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#define GD_IGRAPHICS_DIRECT3D1X_IMPL 1

/*!
 * @file GoddamnEngine/Core/System/Graphics/GraphicsDirect3D1x/GraphicsDirect3D1xShaders.cpp
 * File contains Implementation for Direct3D1x Implementation of the graphics interface: shaders.
 */
#include <GoddamnEngine/GraphicsDirect3D1x/GraphicsDirect3D1x.h>
#if GD_PLATFORM_API_MICROSOFT

#include <d3dcompiler.h>

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Vertex shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	/*!
	 * Creates a new Vertex GPU shader with specified parameters and specifies the input layout
	 * of the Vertex GPU shader.
	 *
	 * @param gfxShaderCreationInfo Creation information for the Vertex shader.
	 * @param gfxVertexArrayLayout input layout of the vertex shader input data.
	 */
	GDAPI IGraphicsDirect3D1xVertexShader::IGraphicsDirect3D1xVertexShader(IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo)
		: IGraphicsVertexShader(gfxShaderCreationInfo), D3DVertexShader(nullptr)
	{
		ThrowIfFailed(GD_IGRAPHICS_DIRECT3D1X->m_D3DDevice->CreateVertexShader(gfxShaderCreationInfo->ShaderByteCode
			, gfxShaderCreationInfo->ShaderByteCodeLength
#if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11			
			, nullptr	// Class Linkage.
#endif	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
			, &D3DVertexShader.p));
	}

	/*!
	 * Creates a new Vertex GPU shader with specified parameters and specifies the input layout
	 * of the Vertex GPU shader.
	 *
	 * @param gfxVertexShaderPtr Pointer for output.
	 * @param gfxShaderCreationInfo Creation information for the Vertex shader.
	 * @param gfxVertexArrayLayout input layout of the vertex shader input data.
	 *
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsDirect3D1xWithVertexShaders::GfxImm_VertexShaderCreate(IGraphicsVertexShader** const gfxVertexShaderPtr
		, IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo, IGraphicsVertexArrayLayout const* const gfxVertexArrayLayout)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxImm_VertexShaderCreate(gfxVertexShaderPtr, gfxShaderCreationInfo, gfxVertexArrayLayout)))
			return IResult::InvalidArguments;
#endif	// if GD_DEBUG

		// Copying parameters and forcedly specifying Vertex shader type.
		auto gfxShaderCreationInfoCopy(*gfxShaderCreationInfo);
		gfxShaderCreationInfoCopy.ShaderType = IGRAPHICS_SHADER_TYPE_VERTEX;

		*gfxVertexShaderPtr = gd_new IGraphicsDirect3D1xVertexShader(&gfxShaderCreationInfoCopy, gfxVertexArrayLayout);
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
	GDAPI void IGraphicsDirect3D1xWithVertexShaders::GfxCmd_VertexShaderBindUniformBuffers(IGraphicsCommandList* const gfxCommandList
		, IGraphicsBuffer const* const* const gfxUniformBuffers, SizeTp const gfxUniformBuffersCount)
	{
		GD_NOT_USED(gfxCommandList);
		auto const d3dConstantBuffers = reinterpret_cast<IGraphicsDirect3D1xBuffer const* const*>(gfxUniformBuffers);

		Vector<ID3D1x(Buffer)*> d3dConstantBuffersList(0, gfxUniformBuffersCount);
		for (SizeTp i = 0; i < gfxUniformBuffersCount; ++i)
		{
			if (d3dConstantBuffers != nullptr && d3dConstantBuffers[i] != nullptr)
			{
				auto const d3dConstantBuffer = d3dConstantBuffers[i];
				d3dConstantBuffersList.InsertLast(d3dConstantBuffer->m_D3DBuffer);
			}
			else
			{
				d3dConstantBuffersList.InsertLast(nullptr);
			}
		}

		GD_IGRAPHICS_DIRECT3D1X->m_D3DDeviceContext->VSSetConstantBuffers(0
			, static_cast<UINT>(gfxUniformBuffersCount), d3dConstantBuffersList.GetData());
	}

	/*!
	 * Binds specified resources into Vertex shader input. Layout of the resources in
	 * shader and in parameters should be preserved.
	 * 
	 * @param gfxCommandList Command list into which this command would be written.
	 * @param gfxResources Pointer to the resources list.
	 * @param gfxResourcesCount Number of the resources to bind.
	 */
	GDAPI void IGraphicsDirect3D1xWithVertexShaders::GfxCmd_VertexShaderBindResources(IGraphicsCommandList* const gfxCommandList
		, IGraphicsShaderResourceView const* const* const gfxResources, SizeTp const gfxResourcesCount)
	{
		GD_NOT_USED(gfxCommandList);
		auto const d3dResources = reinterpret_cast<IGraphicsDirect3D1xShaderResourceView const* const*>(gfxResources);
		
		Vector<ID3D1x(ShaderResourceView)*> d3dShaderResourceViewsList(0, gfxResourcesCount);
		for (SizeTp i = 0; i < gfxResourcesCount; ++i)
		{
			if (d3dResources != nullptr && d3dResources[i] != nullptr)
			{
				auto const d3dResource = d3dResources[i];
				d3dShaderResourceViewsList.InsertLast(d3dResource->m_D3DShaderResourceView);
			}
			else
			{
				d3dShaderResourceViewsList.InsertLast(nullptr);
			}
		}

		GD_IGRAPHICS_DIRECT3D1X->m_D3DDeviceContext->VSSetShaderResources(0
			, static_cast<UINT>(gfxResourcesCount), d3dShaderResourceViewsList.GetData());
	}

	/*!
	 * Binds specified samplers into Vertex shader input. Layout of the resources in
	 * shader and in parameters should be preserved.
	 * 
	 * @param gfxCommandList Command list into which this command would be written.
	 * @param gfxSamplerStates Pointer to the samplers list.
	 * @param gfxSamplerStatesCount Number of the resources to bind.
	 */
	GDAPI void IGraphicsDirect3D1xWithVertexShaders::GfxCmd_VertexShaderBindSamplers(IGraphicsCommandList* const gfxCommandList
		, IGraphicsSamplerState const* const* const gfxSamplerStates, SizeTp const gfxSamplerStatesCount)
	{
		GD_NOT_USED(gfxCommandList);
		auto const d3dSamplerStates = reinterpret_cast<IGraphicsDirect3D1xSamplerState const* const*>(gfxSamplerStates);
		
		Vector<ID3D1x(SamplerState)*> d3dSamplerStatesList(0, gfxSamplerStatesCount);
		for (SizeTp i = 0; i < gfxSamplerStatesCount; ++i)
		{
			if (d3dSamplerStates != nullptr && d3dSamplerStates[i] != nullptr)
			{
				auto const d3dSamplerState = d3dSamplerStates[i];
				d3dSamplerStatesList.InsertLast(d3dSamplerState->m_D3DSamplerState);
			}
			else
			{
				d3dSamplerStatesList.InsertLast(nullptr);
			}
		}

		GD_IGRAPHICS_DIRECT3D1X->m_D3DDeviceContext->VSSetSamplers(0
			, static_cast<UINT>(gfxSamplerStatesCount), d3dSamplerStatesList.GetData());
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Pixel shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Creates a GPU Pixel shader with specified parameters.
	//! @param gfxBufferCreationInfo Pointer to the buffer creation information.
	//! @param gfxBufferInitialData Initial data of the buffer. Can be null pointer for en empty buffer.
	GDAPI IGraphicsDirect3D1xPixelShader::IGraphicsDirect3D1xPixelShader(IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo)
		: IGraphicsPixelShader(gfxShaderCreationInfo), D3DPixelShader(nullptr)
	{
		ThrowIfFailed(GD_IGRAPHICS_DIRECT3D1X->m_D3DDevice->CreatePixelShader(gfxShaderCreationInfo->gfxShaderByteCode, gfxShaderCreationInfo->gfxShaderByteCodeLength
#if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11			
			, nullptr	// Class Linkage.
#endif	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
			, &D3DPixelShader.p));
	}

	// ------------------------------------------------------------------------------------------
	//! Deletes current GPU Pixel shader.
	GDAPI IGraphicsDirect3D1xPixelShader::~IGraphicsDirect3D1xPixelShader()
	{
	}

	// ------------------------------------------------------------------------------------------
	//! Creates a new Pixel GPU shader with specified parameters.
	//! @param gfxPixelShaderPtr Pointer for output.
	//! @param gfxShaderCreationInfo Creation information for the Pixel shader.
	//! @param gfxPixelArrayLayout input layout of the Pixel shader input data.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsDirect3D1xWithPixelShaders::GfxImm_PixelShaderCreate(IGraphicsPixelShader** const gfxPixelShaderPtr
		, IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxImm_PixelShaderCreate(gfxPixelShaderPtr, gfxShaderCreationInfo, gfxPixelArrayLayout)))
			return IResult::InvalidArguments;
#endif	// if GD_DEBUG

		// Copying parameters and forcedly specifying Pixel shader type.
		auto gfxShaderCreationInfoCopy(*gfxShaderCreationInfo);
		gfxShaderCreationInfoCopy.gfxShaderType = IGRAPHICS_SHADER_TYPE_PIXEL;
		*gfxPixelShaderPtr = GD_NEW(IGraphicsDirect3D1xPixelShader, &gfxShaderCreationInfoCopy);
		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Binds specified uniform buffers into Pixel shader input. Layout of the buffers in
	//! shader and in parameters should be preserved.
	//! @param gfxCommandList Command list into which this command would be written.
	//! @param gfxUniformBuffers Pointer to the uniform buffers list.
	//! @param gfxUniformBuffersCount Number of the uniform buffers to bind.
	GDAPI void IGraphicsDirect3D1xWithPixelShaders::GfxCmd_PixelShaderBindUniformBuffers(IGraphicsCommandList* const gfxCommandList
		, IGraphicsBuffer const* const* const gfxUniformBuffers, SizeTp const gfxUniformBuffersCount)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_PixelShaderBindUniformBuffers(gfxCommandList, gfxUniformBuffers, gfxUniformBuffersCount)))
			return;
#endif	// if GD_DEBUG

		GD_NOT_USED(gfxCommandList);
		auto const d3dUniformBuffers = reinterpret_cast<IGraphicsDirect3D1xBuffer const* const*>(gfxUniformBuffers);
		if (d3dUniformBuffers != nullptr)
		{
			Vector<ID3D1x(Buffer)*> d3dConstantBuffersList(0, gfxUniformBuffersCount);
			for (SizeTp d3dConstantBuffer = 0; d3dConstantBuffer < gfxUniformBuffersCount; ++d3dConstantBuffer)
				d3dConstantBuffersList.InsertLast(d3dUniformBuffers[d3dConstantBuffer]->D3DBuffer);
			GD_IGRAPHICS_DIRECT3D1X->m_D3DDeviceContext->PSSetConstantBuffers(0, static_cast<UINT>(gfxUniformBuffersCount), d3dConstantBuffersList.GetData());
		}
	}

	// ------------------------------------------------------------------------------------------
	//! Binds specified resources into Pixel shader input. Layout of the resources in
	//! shader and in parameters should be preserved.
	//! @param gfxCommandList Command list into which this command would be written.
	//! @param gfxResources Pointer to the resources list.
	//! @param gfxResourcesCount Number of the resources to bind.
	GDAPI void IGraphicsDirect3D1xWithPixelShaders::GfxCmd_PixelShaderBindResources(IGraphicsCommandList* const gfxCommandList
		, IGraphicsShaderResourceView const* const* const gfxResources, SizeTp const gfxResourcesCount)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_PixelShaderBindResources(gfxCommandList, gfxResources, gfxResourcesCount)))
			return;
#endif	// if GD_DEBUG

		GD_NOT_USED(gfxCommandList);
		auto const d3dResources = reinterpret_cast<IGraphicsDirect3D1xShaderResourceView const* const*>(gfxResources);
		if (d3dResources != nullptr)
		{
			Vector<ID3D1x(ShaderResourceView)*> d3dShaderResourceViewsList(0, gfxResourcesCount);
			for (SizeTp d3dResource = 0; d3dResource < gfxResourcesCount; ++d3dResource)
				d3dShaderResourceViewsList.InsertLast(d3dResources[d3dResource]->m_D3DShaderResourceView);
			GD_IGRAPHICS_DIRECT3D1X->m_D3DDeviceContext->PSSetShaderResources(0, static_cast<UINT>(gfxResourcesCount), d3dShaderResourceViewsList.GetData());
		}
	}

	// ------------------------------------------------------------------------------------------
	//! Binds specified samplers into Pixel shader input. Layout of the resources in
	//! shader and in parameters should be preserved.
	//! @param gfxCommandList Command list into which this command would be written.
	//! @param gfxSamplers Pointer to the samplers list.
	//! @param gfxSamplersCount Number of the resources to bind.
	GDAPI void IGraphicsDirect3D1xWithPixelShaders::GfxCmd_PixelShaderBindSamplers(IGraphicsCommandList* const gfxCommandList
		, IGraphicsSamplerState const* const* const gfxSamplerStates, SizeTp const gfxSamplerStatesCount)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_PixelShaderBindSamplers(gfxCommandList, gfxSamplerStates, gfxSamplerStatesCount)))
			return;
#endif	// if GD_DEBUG

		GD_NOT_USED(gfxCommandList);
		auto const d3dSamplerStates = reinterpret_cast<IGraphicsDirect3D1xSamplerState const* const*>(gfxSamplerStates);
		if (d3dSamplerStates != nullptr)
		{
			Vector<ID3D1x(SamplerState)*> d3dSamplerStatesList(0, gfxSamplerStatesCount);
			for (SizeTp d3dSamplerState = 0; d3dSamplerState < gfxSamplerStatesCount; ++d3dSamplerState)
				d3dSamplerStatesList.InsertLast(d3dSamplerStates[d3dSamplerState]->m_D3DSamplerState);
			GD_IGRAPHICS_DIRECT3D1X->m_D3DDeviceContext->PSSetSamplers(0, static_cast<UINT>(gfxSamplerStatesCount), d3dSamplerStatesList.GetData());
		}
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Geometry shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Creates a GPU Geometry shader with specified parameters.
	//! @param gfxBufferCreationInfo Pointer to the buffer creation information.
	//! @param gfxBufferInitialData Initial data of the buffer. Can be null pointer for en empty buffer.
	GDAPI IGraphicsDirect3D1xGeometryShader::IGraphicsDirect3D1xGeometryShader(IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo)
		: IGraphicsGeometryShader(gfxShaderCreationInfo), D3DGeometryShader(nullptr)
	{
		ThrowIfFailed(GD_IGRAPHICS_DIRECT3D1X->m_D3DDevice->CreateGeometryShader(gfxShaderCreationInfo->gfxShaderByteCode, gfxShaderCreationInfo->gfxShaderByteCodeLength
#if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11			
			, nullptr	// Class Linkage.
#endif	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
			, &D3DGeometryShader.p));
	}

	// ------------------------------------------------------------------------------------------
	//! Deletes current GPU Geometry shader.
	GDAPI IGraphicsDirect3D1xGeometryShader::~IGraphicsDirect3D1xGeometryShader()
	{
	}

	// ------------------------------------------------------------------------------------------
	//! Creates a new Geometry GPU shader with specified parameters.
	//! @param gfxGeometryShaderPtr Pointer for output.
	//! @param gfxShaderCreationInfo Creation information for the Geometry shader.
	//! @param gfxGeometryArrayLayout input layout of the Geometry shader input data.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsDirect3D1xWithGeometryShaders::GfxImm_GeometryShaderCreate(IGraphicsGeometryShader** const gfxGeometryShaderPtr
		, IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxImm_GeometryShaderCreate(gfxGeometryShaderPtr, gfxShaderCreationInfo, gfxGeometryArrayLayout)))
			return IResult::InvalidArguments;
#endif	// if GD_DEBUG

		// Copying parameters and forcedly specifying Geometry shader type.
		auto gfxShaderCreationInfoCopy(*gfxShaderCreationInfo);
		gfxShaderCreationInfoCopy.gfxShaderType = IGRAPHICS_SHADER_TYPE_GEOMETRY;
		*gfxGeometryShaderPtr = GD_NEW(IGraphicsDirect3D1xGeometryShader, &gfxShaderCreationInfoCopy);
		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Binds specified uniform buffers into Geometry shader input. Layout of the buffers in
	//! shader and in parameters should be preserved.
	//! @param gfxCommandList Command list into which this command would be written.
	//! @param gfxUniformBuffers Pointer to the uniform buffers list.
	//! @param gfxUniformBuffersCount Number of the uniform buffers to bind.
	GDAPI void IGraphicsDirect3D1xWithGeometryShaders::GfxCmd_GeometryShaderBindUniformBuffers(IGraphicsCommandList* const gfxCommandList
		, IGraphicsBuffer const* const* const gfxUniformBuffers, SizeTp const gfxUniformBuffersCount)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_GeometryShaderBindUniformBuffers(gfxCommandList, gfxUniformBuffers, gfxUniformBuffersCount)))
			return;
#endif	// if GD_DEBUG

		GD_NOT_USED(gfxCommandList);
		auto const d3dUniformBuffers = reinterpret_cast<IGraphicsDirect3D1xBuffer const* const*>(gfxUniformBuffers);
		if (d3dUniformBuffers != nullptr)
		{
			Vector<ID3D1x(Buffer)*> d3dConstantBuffersList(0, gfxUniformBuffersCount);
			for (SizeTp d3dConstantBuffer = 0; d3dConstantBuffer < gfxUniformBuffersCount; ++d3dConstantBuffer)
				d3dConstantBuffersList.InsertLast(d3dUniformBuffers[d3dConstantBuffer]->D3DBuffer);
			GD_IGRAPHICS_DIRECT3D1X->m_D3DDeviceContext->GSSetConstantBuffers(0, static_cast<UINT>(gfxUniformBuffersCount), d3dConstantBuffersList.GetData());
		}
	}

	// ------------------------------------------------------------------------------------------
	//! Binds specified resources into Geometry shader input. Layout of the resources in
	//! shader and in parameters should be preserved.
	//! @param gfxCommandList Command list into which this command would be written.
	//! @param gfxResources Pointer to the resources list.
	//! @param gfxResourcesCount Number of the resources to bind.
	GDAPI void IGraphicsDirect3D1xWithGeometryShaders::GfxCmd_GeometryShaderBindResources(IGraphicsCommandList* const gfxCommandList
		, IGraphicsShaderResourceView const* const* const gfxResources, SizeTp const gfxResourcesCount)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_GeometryShaderBindResources(gfxCommandList, gfxResources, gfxResourcesCount)))
			return;
#endif	// if GD_DEBUG

		GD_NOT_USED(gfxCommandList);
		auto const d3dResources = reinterpret_cast<IGraphicsDirect3D1xShaderResourceView const* const*>(gfxResources);
		if (d3dResources != nullptr)
		{
			Vector<ID3D1x(ShaderResourceView)*> d3dShaderResourceViewsList(0, gfxResourcesCount);
			for (SizeTp d3dResource = 0; d3dResource < gfxResourcesCount; ++d3dResource)
				d3dShaderResourceViewsList.InsertLast(d3dResources[d3dResource]->m_D3DShaderResourceView);
			GD_IGRAPHICS_DIRECT3D1X->m_D3DDeviceContext->GSSetShaderResources(0, static_cast<UINT>(gfxResourcesCount), d3dShaderResourceViewsList.GetData());
		}
	}

	// ------------------------------------------------------------------------------------------
	//! Binds specified samplers into Geometry shader input. Layout of the resources in
	//! shader and in parameters should be preserved.
	//! @param gfxCommandList Command list into which this command would be written.
	//! @param gfxSamplers Pointer to the samplers list.
	//! @param gfxSamplersCount Number of the resources to bind.
	GDAPI void IGraphicsDirect3D1xWithGeometryShaders::GfxCmd_GeometryShaderBindSamplers(IGraphicsCommandList* const gfxCommandList
		, IGraphicsSamplerState const* const* const gfxSamplerStates, SizeTp const gfxSamplerStatesCount)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_GeometryShaderBindSamplers(gfxCommandList, gfxSamplerStates, gfxSamplerStatesCount)))
			return;
#endif	// if GD_DEBUG

		GD_NOT_USED(gfxCommandList);
		auto const d3dSamplerStates = reinterpret_cast<IGraphicsDirect3D1xSamplerState const* const*>(gfxSamplerStates);
		if (d3dSamplerStates != nullptr)
		{
			Vector<ID3D1x(SamplerState)*> d3dSamplerStatesList(0, gfxSamplerStatesCount);
			for (SizeTp d3dSamplerState = 0; d3dSamplerState < gfxSamplerStatesCount; ++d3dSamplerState)
				d3dSamplerStatesList.InsertLast(d3dSamplerStates[d3dSamplerState]->m_D3DSamplerState);
			GD_IGRAPHICS_DIRECT3D1X->m_D3DDeviceContext->GSSetSamplers(0, static_cast<UINT>(gfxSamplerStatesCount), d3dSamplerStatesList.GetData());
		}
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_API_MICROSOFT
