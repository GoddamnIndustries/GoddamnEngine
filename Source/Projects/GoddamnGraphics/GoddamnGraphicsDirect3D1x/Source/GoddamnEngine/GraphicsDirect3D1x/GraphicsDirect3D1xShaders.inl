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
 * File contains Implementation for Direct3D1x 4.3 Implementation of the graphics interface: shaders.
 */
#include <GoddamnEngine/Core/System/Graphics/GraphicsDirect3D1x/GraphicsDirect3D1x.h>
#if GD_PLATFORM_API_MICROSOFT

#include <d3dcompiler.h>

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Vertex shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphicsDirect3D1xVertexShader);

	// ------------------------------------------------------------------------------------------
	//! Creates a GPU Vertex shader with specified parameters.
	//! @param gfxBufferCreationInfo Pointer to the buffer creation information.
	//! @param gfxBufferInitialData Initial data of the buffer. Can be null pointer for en empty buffer.
	GDAPI IGraphicsDirect3D1xVertexShader::IGraphicsDirect3D1xVertexShader(IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo)
		: IGraphicsVertexShader(gfxShaderCreationInfo), D3DVertexShader(nullptr)
	{
		ThrowIfFailed(GD_IGRAPHICS_DIRECT3D1X->D3DDevice->CreateVertexShader(gfxShaderCreationInfo->gfxShaderByteCode, gfxShaderCreationInfo->gfxShaderByteCodeLength
#if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11			
			, nullptr	// Class Linkage.
#endif	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
			, &D3DVertexShader.p));
	}

	// ------------------------------------------------------------------------------------------
	//! Deletes current GPU Vertex shader.
	GDAPI IGraphicsDirect3D1xVertexShader::~IGraphicsDirect3D1xVertexShader()
	{
	}

	// ------------------------------------------------------------------------------------------
	//! Creates a new Vertex GPU shader with specified parameters.
	//! @param gfxVertexShaderPtr Pointer for output.
	//! @param gfxShaderCreationInfo Creation information for the Vertex shader.
	//! @param gfxVertexArrayLayout input layout of the vertex shader input data.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsDirect3D1xWithVertexShaders::GfxImm_VertexShaderCreate(IGraphicsVertexShader** const gfxVertexShaderPtr
		, IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo
		, IGraphicsVertexArrayLayout const* const gfxVertexArrayLayout)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxImm_VertexShaderCreate(gfxVertexShaderPtr, gfxShaderCreationInfo, gfxVertexArrayLayout)))
			return IResult::InvalidArguments;
#endif	// if GD_DEBUG

		// input layouts are not used in Direct3D1x.
		GD_NOT_USED(gfxVertexArrayLayout);

		// Copying parameters and forcedly specifying Vertex shader type.
		auto gfxShaderCreationInfoCopy(*gfxShaderCreationInfo);
		gfxShaderCreationInfoCopy.gfxShaderType = IGRAPHICS_SHADER_TYPE_VERTEX;
		*gfxVertexShaderPtr = GD_NEW(IGraphicsDirect3D1xVertexShader, &gfxShaderCreationInfoCopy);
		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Binds specified uniform buffers into Vertex shader input. Layout of the buffers in
	//! shader and in parameters should be preserved.
	//! @param gfxCommandList Command list into which this command would be written.
	//! @param gfxUniformBuffers Pointer to the uniform buffers list.
	//! @param gfxUniformBuffersCount Number of the uniform buffers to bind.
	GDAPI void IGraphicsDirect3D1xWithVertexShaders::GfxCmd_VertexShaderBindUniformBuffers(IGraphicsCommandList* const gfxCommandList
		, IGraphicsBuffer const* const* const gfxUniformBuffers, SizeTp const gfxUniformBuffersCount)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_VertexShaderBindUniformBuffers(gfxCommandList, gfxUniformBuffers, gfxUniformBuffersCount)))
			return;
#endif	// if GD_DEBUG

		GD_NOT_USED(gfxCommandList);
		auto const d3dUniformBuffers = reinterpret_cast<IGraphicsDirect3D1xBuffer const* const*>(gfxUniformBuffers);
		if (d3dUniformBuffers != nullptr)
		{
			Vector<ID3D1x(Buffer)*> d3dConstantBuffersList(0, gfxUniformBuffersCount);
			for (SizeTp d3dConstantBuffer = 0; d3dConstantBuffer < gfxUniformBuffersCount; ++d3dConstantBuffer)
				d3dConstantBuffersList.InsertLast(d3dUniformBuffers[d3dConstantBuffer]->D3DBuffer);
			GD_IGRAPHICS_DIRECT3D1X->D3DDeviceContext->VSSetConstantBuffers(0, static_cast<UINT>(gfxUniformBuffersCount), d3dConstantBuffersList.GetData());
		}
	}

	// ------------------------------------------------------------------------------------------
	//! Binds specified resources into Vertex shader input. Layout of the resources in
	//! shader and in parameters should be preserved.
	//! @param gfxCommandList Command list into which this command would be written.
	//! @param gfxResources Pointer to the resources list.
	//! @param gfxResourcesCount Number of the resources to bind.
	GDAPI void IGraphicsDirect3D1xWithVertexShaders::GfxCmd_VertexShaderBindResources(IGraphicsCommandList* const gfxCommandList
		, IGraphicsShaderResourceView const* const* const gfxResources, SizeTp const gfxResourcesCount)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_VertexShaderBindResources(gfxCommandList, gfxResources, gfxResourcesCount)))
			return;
#endif	// if GD_DEBUG

		GD_NOT_USED(gfxCommandList);
		auto const d3dResources = reinterpret_cast<IGraphicsDirect3D1xShaderResourceView const* const*>(gfxResources);
		if (d3dResources != nullptr)
		{
			Vector<ID3D1x(ShaderResourceView)*> d3dShaderResourceViewsList(0, gfxResourcesCount);
			for (SizeTp d3dResource = 0; d3dResource < gfxResourcesCount; ++d3dResource)
				d3dShaderResourceViewsList.InsertLast(d3dResources[d3dResource]->D3DShaderResourceView);
			GD_IGRAPHICS_DIRECT3D1X->D3DDeviceContext->VSSetShaderResources(0, static_cast<UINT>(gfxResourcesCount), d3dShaderResourceViewsList.GetData());
		}
	}

	// ------------------------------------------------------------------------------------------
	//! Binds specified samplers into Vertex shader input. Layout of the resources in
	//! shader and in parameters should be preserved.
	//! @param gfxCommandList Command list into which this command would be written.
	//! @param gfxSamplers Pointer to the samplers list.
	//! @param gfxSamplersCount Number of the resources to bind.
	GDAPI void IGraphicsDirect3D1xWithVertexShaders::GfxCmd_VertexShaderBindSamplers(IGraphicsCommandList* const gfxCommandList
		, IGraphicsSamplerState const* const* const gfxSamplerStates, SizeTp const gfxSamplerStatesCount)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_VertexShaderBindSamplers(gfxCommandList, gfxSamplerStates, gfxSamplerStatesCount)))
			return;
#endif	// if GD_DEBUG

		GD_NOT_USED(gfxCommandList);
		auto const d3dSamplerStates = reinterpret_cast<IGraphicsDirect3D1xSamplerState const* const*>(gfxSamplerStates);
		if (d3dSamplerStates != nullptr)
		{
			Vector<ID3D1x(SamplerState)*> d3dSamplerStatesList(0, gfxSamplerStatesCount);
			for (SizeTp d3dSamplerState = 0; d3dSamplerState < gfxSamplerStatesCount; ++d3dSamplerState)
				d3dSamplerStatesList.InsertLast(d3dSamplerStates[d3dSamplerState]->D3DSamplerState);
			GD_IGRAPHICS_DIRECT3D1X->D3DDeviceContext->VSSetSamplers(0, static_cast<UINT>(gfxSamplerStatesCount), d3dSamplerStatesList.GetData());
		}
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Pixel shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphicsDirect3D1xPixelShader);

	// ------------------------------------------------------------------------------------------
	//! Creates a GPU Pixel shader with specified parameters.
	//! @param gfxBufferCreationInfo Pointer to the buffer creation information.
	//! @param gfxBufferInitialData Initial data of the buffer. Can be null pointer for en empty buffer.
	GDAPI IGraphicsDirect3D1xPixelShader::IGraphicsDirect3D1xPixelShader(IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo)
		: IGraphicsPixelShader(gfxShaderCreationInfo), D3DPixelShader(nullptr)
	{
		ThrowIfFailed(GD_IGRAPHICS_DIRECT3D1X->D3DDevice->CreatePixelShader(gfxShaderCreationInfo->gfxShaderByteCode, gfxShaderCreationInfo->gfxShaderByteCodeLength
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
			GD_IGRAPHICS_DIRECT3D1X->D3DDeviceContext->PSSetConstantBuffers(0, static_cast<UINT>(gfxUniformBuffersCount), d3dConstantBuffersList.GetData());
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
				d3dShaderResourceViewsList.InsertLast(d3dResources[d3dResource]->D3DShaderResourceView);
			GD_IGRAPHICS_DIRECT3D1X->D3DDeviceContext->PSSetShaderResources(0, static_cast<UINT>(gfxResourcesCount), d3dShaderResourceViewsList.GetData());
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
				d3dSamplerStatesList.InsertLast(d3dSamplerStates[d3dSamplerState]->D3DSamplerState);
			GD_IGRAPHICS_DIRECT3D1X->D3DDeviceContext->PSSetSamplers(0, static_cast<UINT>(gfxSamplerStatesCount), d3dSamplerStatesList.GetData());
		}
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Geometry shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphicsDirect3D1xGeometryShader);

	// ------------------------------------------------------------------------------------------
	//! Creates a GPU Geometry shader with specified parameters.
	//! @param gfxBufferCreationInfo Pointer to the buffer creation information.
	//! @param gfxBufferInitialData Initial data of the buffer. Can be null pointer for en empty buffer.
	GDAPI IGraphicsDirect3D1xGeometryShader::IGraphicsDirect3D1xGeometryShader(IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo)
		: IGraphicsGeometryShader(gfxShaderCreationInfo), D3DGeometryShader(nullptr)
	{
		ThrowIfFailed(GD_IGRAPHICS_DIRECT3D1X->D3DDevice->CreateGeometryShader(gfxShaderCreationInfo->gfxShaderByteCode, gfxShaderCreationInfo->gfxShaderByteCodeLength
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
			GD_IGRAPHICS_DIRECT3D1X->D3DDeviceContext->GSSetConstantBuffers(0, static_cast<UINT>(gfxUniformBuffersCount), d3dConstantBuffersList.GetData());
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
				d3dShaderResourceViewsList.InsertLast(d3dResources[d3dResource]->D3DShaderResourceView);
			GD_IGRAPHICS_DIRECT3D1X->D3DDeviceContext->GSSetShaderResources(0, static_cast<UINT>(gfxResourcesCount), d3dShaderResourceViewsList.GetData());
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
				d3dSamplerStatesList.InsertLast(d3dSamplerStates[d3dSamplerState]->D3DSamplerState);
			GD_IGRAPHICS_DIRECT3D1X->D3DDeviceContext->GSSetSamplers(0, static_cast<UINT>(gfxSamplerStatesCount), d3dSamplerStatesList.GetData());
		}
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Hull shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if GD_FALSE

	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphicsDirect3D1xHullShader);

	// ------------------------------------------------------------------------------------------
	//! Creates a GPU Hull shader with specified parameters.
	//! @param gfxBufferCreationInfo Pointer to the buffer creation information.
	//! @param gfxBufferInitialData Initial data of the buffer. Can be null pointer for en empty buffer.
	GDAPI IGraphicsDirect3D1xHullShader::IGraphicsDirect3D1xHullShader(IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo)
		: IGraphicsHullShader(gfxShaderCreationInfo)
#if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11			
		, D3DHullShader(nullptr)
#endif	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
	{
#if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11			
		ThrowIfFailed(GD_IGRAPHICS_DIRECT3D1X->D3DDevice->CreateHullShader(gfxShaderCreationInfo->gfxShaderByteCode, gfxShaderCreationInfo->gfxShaderByteCodeLength
			, nullptr	// Class Linkage.
			, &D3DHullShader.p));
#endif	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
	}

	// ------------------------------------------------------------------------------------------
	//! Deletes current GPU Hull shader.
	GDAPI IGraphicsDirect3D1xHullShader::~IGraphicsDirect3D1xHullShader()
	{
	}

	// ------------------------------------------------------------------------------------------
	//! Creates a new Hull GPU shader with specified parameters.
	//! @param gfxHullShaderPtr Pointer for output.
	//! @param gfxShaderCreationInfo Creation information for the Hull shader.
	//! @param gfxHullArrayLayout input layout of the Hull shader input data.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsDirect3D1xWithHullShaders::GfxImm_HullShaderCreate(IGraphicsHullShader** const gfxHullShaderPtr
		, IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxImm_HullShaderCreate(gfxHullShaderPtr, gfxShaderCreationInfo, gfxHullArrayLayout)))
			return IResult::InvalidArguments;
#endif	// if GD_DEBUG

		// Copying parameters and forcedly specifying Hull shader type.
		auto gfxShaderCreationInfoCopy(*gfxShaderCreationInfo);
		gfxShaderCreationInfoCopy.gfxShaderType = IGRAPHICS_SHADER_TYPE_HULL;
		*gfxHullShaderPtr = GD_NEW(IGraphicsDirect3D1xHullShader, &gfxShaderCreationInfoCopy);
		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Binds specified uniform buffers into Hull shader input. Layout of the buffers in
	//! shader and in parameters should be preserved.
	//! @param gfxCommandList Command list into which this command would be written.
	//! @param gfxUniformBuffers Pointer to the uniform buffers list.
	//! @param gfxUniformBuffersCount Number of the uniform buffers to bind.
	GDAPI void IGraphicsDirect3D1xWithHullShaders::GfxCmd_HullShaderBindUniformBuffers(IGraphicsCommandList* const gfxCommandList
		, IGraphicsBuffer const* const* const gfxUniformBuffers, SizeTp const gfxUniformBuffersCount)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_HullShaderBindUniformBuffers(gfxCommandList, gfxUniformBuffers, gfxUniformBuffersCount)))
			return;
#endif	// if GD_DEBUG

		GD_NOT_USED(gfxCommandList);
#if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
		auto const d3dUniformBuffers = reinterpret_cast<IGraphicsDirect3D1xBuffer const* const*>(gfxUniformBuffers);
		if (d3dUniformBuffers != nullptr)
		{
			Vector<ID3D1x(Buffer)*> d3dConstantBuffersList(0, gfxUniformBuffersCount);
			for (SizeTp d3dConstantBuffer = 0; d3dConstantBuffer < gfxUniformBuffersCount; ++d3dConstantBuffer)
				d3dConstantBuffersList.InsertLast(d3dUniformBuffers[d3dConstantBuffer]->D3DBuffer);
			GD_IGRAPHICS_DIRECT3D1X->D3DDeviceContext->HSSetConstantBuffers(0, static_cast<UINT>(gfxUniformBuffersCount), d3dConstantBuffersList.GetData());
		}
#else	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
		GD_NOT_USED_L(gfxUniformBuffers, gfxUniformBuffersCount);
#endif	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
	}

	// ------------------------------------------------------------------------------------------
	//! Binds specified resources into Hull shader input. Layout of the resources in
	//! shader and in parameters should be preserved.
	//! @param gfxCommandList Command list into which this command would be written.
	//! @param gfxResources Pointer to the resources list.
	//! @param gfxResourcesCount Number of the resources to bind.
	GDAPI void IGraphicsDirect3D1xWithHullShaders::GfxCmd_HullShaderBindResources(IGraphicsCommandList* const gfxCommandList
		, IGraphicsShaderResourceView const* const* const gfxResources, SizeTp const gfxResourcesCount)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_HullShaderBindResources(gfxCommandList, gfxResources, gfxResourcesCount)))
			return;
#endif	// if GD_DEBUG

		GD_NOT_USED(gfxCommandList);
#if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
		auto const d3dResources = reinterpret_cast<IGraphicsDirect3D1xShaderResourceView const* const*>(gfxResources);
		if (d3dResources != nullptr)
		{
			Vector<ID3D1x(ShaderResourceView)*> d3dShaderResourceViewsList(0, gfxResourcesCount);
			for (SizeTp d3dResource = 0; d3dResource < gfxResourcesCount; ++d3dResource)
				d3dShaderResourceViewsList.InsertLast(d3dResources[d3dResource]->D3DShaderResourceView);
			GD_IGRAPHICS_DIRECT3D1X->D3DDeviceContext->HSSetShaderResources(0, static_cast<UINT>(gfxResourcesCount), d3dShaderResourceViewsList.GetData());
		}
#else	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
		GD_NOT_USED_L(gfxResources, gfxResourcesCount);
#endif	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
	}

	// ------------------------------------------------------------------------------------------
	//! Binds specified samplers into Hull shader input. Layout of the resources in
	//! shader and in parameters should be preserved.
	//! @param gfxCommandList Command list into which this command would be written.
	//! @param gfxSamplers Pointer to the samplers list.
	//! @param gfxSamplersCount Number of the resources to bind.
	GDAPI void IGraphicsDirect3D1xWithHullShaders::GfxCmd_HullShaderBindSamplers(IGraphicsCommandList* const gfxCommandList
		, IGraphicsSamplerState const* const* const gfxSamplerStates, SizeTp const gfxSamplerStatesCount)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_HullShaderBindSamplers(gfxCommandList, gfxSamplerStates, gfxSamplerStatesCount)))
			return;
#endif	// if GD_DEBUG

		GD_NOT_USED(gfxCommandList);
#if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
		auto const d3dSamplerStates = reinterpret_cast<IGraphicsDirect3D1xSamplerState const* const*>(gfxSamplerStates);
		if (d3dSamplerStates != nullptr)
		{
			Vector<ID3D1x(SamplerState)*> d3dSamplerStatesList(0, gfxSamplerStatesCount);
			for (SizeTp d3dSamplerState = 0; d3dSamplerState < gfxSamplerStatesCount; ++d3dSamplerState)
				d3dSamplerStatesList.InsertLast(d3dSamplerStates[d3dSamplerState]->D3DSamplerState);
			GD_IGRAPHICS_DIRECT3D1X->D3DDeviceContext->HSSetSamplers(0, static_cast<UINT>(gfxSamplerStatesCount), d3dSamplerStatesList.GetData());
		}
#else	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
		GD_NOT_USED_L(gfxSamplerStates, gfxSamplerStatesCount);
#endif	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Domain shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphicsDirect3D1xDomainShader);

	// ------------------------------------------------------------------------------------------
	//! Creates a GPU Domain shader with specified parameters.
	//! @param gfxBufferCreationInfo Pointer to the buffer creation information.
	//! @param gfxBufferInitialData Initial data of the buffer. Can be null pointer for en empty buffer.
	GDAPI IGraphicsDirect3D1xDomainShader::IGraphicsDirect3D1xDomainShader(IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo)
		: IGraphicsDomainShader(gfxShaderCreationInfo)
#if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11			
		, D3DDomainShader(nullptr)
#endif	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
	{
#if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11			
		ThrowIfFailed(GD_IGRAPHICS_DIRECT3D1X->D3DDevice->CreateDomainShader(gfxShaderCreationInfo->gfxShaderByteCode, gfxShaderCreationInfo->gfxShaderByteCodeLength
			, nullptr	// Class Linkage.
			, &D3DDomainShader.p));
#endif	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
	}

	// ------------------------------------------------------------------------------------------
	//! Deletes current GPU Domain shader.
	GDAPI IGraphicsDirect3D1xDomainShader::~IGraphicsDirect3D1xDomainShader()
	{
	}

	// ------------------------------------------------------------------------------------------
	//! Creates a new Domain GPU shader with specified parameters.
	//! @param gfxDomainShaderPtr Pointer for output.
	//! @param gfxShaderCreationInfo Creation information for the Domain shader.
	//! @param gfxDomainArrayLayout input layout of the Domain shader input data.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsDirect3D1xWithDomainShaders::GfxImm_DomainShaderCreate(IGraphicsDomainShader** const gfxDomainShaderPtr
		, IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxImm_DomainShaderCreate(gfxDomainShaderPtr, gfxShaderCreationInfo, gfxDomainArrayLayout)))
			return IResult::InvalidArguments;
#endif	// if GD_DEBUG

		// Copying parameters and forcedly specifying Domain shader type.
		auto gfxShaderCreationInfoCopy(*gfxShaderCreationInfo);
		gfxShaderCreationInfoCopy.gfxShaderType = IGRAPHICS_SHADER_TYPE_DOMAIN;
		*gfxDomainShaderPtr = GD_NEW(IGraphicsDirect3D1xDomainShader, &gfxShaderCreationInfoCopy);
		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Binds specified uniform buffers into Domain shader input. Layout of the buffers in
	//! shader and in parameters should be preserved.
	//! @param gfxCommandList Command list into which this command would be written.
	//! @param gfxUniformBuffers Pointer to the uniform buffers list.
	//! @param gfxUniformBuffersCount Number of the uniform buffers to bind.
	GDAPI void IGraphicsDirect3D1xWithDomainShaders::GfxCmd_DomainShaderBindUniformBuffers(IGraphicsCommandList* const gfxCommandList
		, IGraphicsBuffer const* const* const gfxUniformBuffers, SizeTp const gfxUniformBuffersCount)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_DomainShaderBindUniformBuffers(gfxCommandList, gfxUniformBuffers, gfxUniformBuffersCount)))
			return;
#endif	// if GD_DEBUG

		GD_NOT_USED(gfxCommandList);
#if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11			
		auto const d3dUniformBuffers = reinterpret_cast<IGraphicsDirect3D1xBuffer const* const*>(gfxUniformBuffers);
		if (d3dUniformBuffers != nullptr)
		{
			Vector<ID3D1x(Buffer)*> d3dConstantBuffersList(0, gfxUniformBuffersCount);
			for (SizeTp d3dConstantBuffer = 0; d3dConstantBuffer < gfxUniformBuffersCount; ++d3dConstantBuffer)
				d3dConstantBuffersList.InsertLast(d3dUniformBuffers[d3dConstantBuffer]->D3DBuffer);
			GD_IGRAPHICS_DIRECT3D1X->D3DDeviceContext->DSSetConstantBuffers(0, static_cast<UINT>(gfxUniformBuffersCount), d3dConstantBuffersList.GetData());
		}
#else	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
		GD_NOT_USED_L(gfxUniformBuffers, gfxUniformBuffersCount);
#endif	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
	}

	// ------------------------------------------------------------------------------------------
	//! Binds specified resources into Domain shader input. Layout of the resources in
	//! shader and in parameters should be preserved.
	//! @param gfxCommandList Command list into which this command would be written.
	//! @param gfxResources Pointer to the resources list.
	//! @param gfxResourcesCount Number of the resources to bind.
	GDAPI void IGraphicsDirect3D1xWithDomainShaders::GfxCmd_DomainShaderBindResources(IGraphicsCommandList* const gfxCommandList
		, IGraphicsShaderResourceView const* const* const gfxResources, SizeTp const gfxResourcesCount)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_DomainShaderBindResources(gfxCommandList, gfxResources, gfxResourcesCount)))
			return;
#endif	// if GD_DEBUG

		GD_NOT_USED(gfxCommandList);
#if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11			
		auto const d3dResources = reinterpret_cast<IGraphicsDirect3D1xShaderResourceView const* const*>(gfxResources);
		if (d3dResources != nullptr)
		{
			Vector<ID3D1x(ShaderResourceView)*> d3dShaderResourceViewsList(0, gfxResourcesCount);
			for (SizeTp d3dResource = 0; d3dResource < gfxResourcesCount; ++d3dResource)
				d3dShaderResourceViewsList.InsertLast(d3dResources[d3dResource]->D3DShaderResourceView);
			GD_IGRAPHICS_DIRECT3D1X->D3DDeviceContext->DSSetShaderResources(0, static_cast<UINT>(gfxResourcesCount), d3dShaderResourceViewsList.GetData());
		}
#else	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
		GD_NOT_USED_L(gfxResources, gfxResourcesCount);
#endif	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
	}

	// ------------------------------------------------------------------------------------------
	//! Binds specified samplers into Domain shader input. Layout of the resources in
	//! shader and in parameters should be preserved.
	//! @param gfxCommandList Command list into which this command would be written.
	//! @param gfxSamplers Pointer to the samplers list.
	//! @param gfxSamplersCount Number of the resources to bind.
	GDAPI void IGraphicsDirect3D1xWithDomainShaders::GfxCmd_DomainShaderBindSamplers(IGraphicsCommandList* const gfxCommandList
		, IGraphicsSamplerState const* const* const gfxSamplerStates, SizeTp const gfxSamplerStatesCount)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_DomainShaderBindSamplers(gfxCommandList, gfxSamplerStates, gfxSamplerStatesCount)))
			return;
#endif	// if GD_DEBUG

		GD_NOT_USED(gfxCommandList);
#if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11			
		auto const d3dSamplerStates = reinterpret_cast<IGraphicsDirect3D1xSamplerState const* const*>(gfxSamplerStates);
		if (d3dSamplerStates != nullptr)
		{
			Vector<ID3D1x(SamplerState)*> d3dSamplerStatesList(0, gfxSamplerStatesCount);
			for (SizeTp d3dSamplerState = 0; d3dSamplerState < gfxSamplerStatesCount; ++d3dSamplerState)
				d3dSamplerStatesList.InsertLast(d3dSamplerStates[d3dSamplerState]->D3DSamplerState);
			GD_IGRAPHICS_DIRECT3D1X->D3DDeviceContext->DSSetSamplers(0, static_cast<UINT>(gfxSamplerStatesCount), d3dSamplerStatesList.GetData());
		}
#else	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
		GD_NOT_USED_L(gfxSamplerStates, gfxSamplerStatesCount);
#endif	// if GD_IGRAPHICS_DIRECT3D1X_VERSION >= 11
	}

#endif	// if GD_FALSE

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Compute shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
#if GD_FALSE

	//! @todo Provide functionality for compute shaders.

#endif	// if GD_FALSE

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_API_MICROSOFT
