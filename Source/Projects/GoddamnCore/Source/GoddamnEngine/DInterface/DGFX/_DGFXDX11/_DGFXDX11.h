// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/DInterface/DGFX/_DGFXDX11/_DGFXDX11.h
 * File contains export interface for DirectX 11 implementation of the graphics interface.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#if GD_PLATFORM_API_MICROSOFT

#include <GoddamnEngine/DInterface/DGFX/DGFX.h>
#include <GoddamnEngine/DInterface/DGFX/_DGFXWindows.h>

#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Containers/Vector.h>
#include <GoddamnEngine/Core/UniquePtr.h>
#include <GoddamnEngine/Core/Geometry.h>

#include <atlbase.h>
#include <d3d11.h>
#include <dxgi.h>

GD_NAMESPACE_BEGIN

	struct _DGFXDX11 final : public _DGFXWindows
	{
		typedef _DGFXWindows _DGFXBase;

	protected:
		CComPtr<IDXGISwapChain>		     _Dx11SwapChain          = nullptr; 
		CComPtr<ID3D11Device>            _Dx11Device             = nullptr;
		CComPtr<ID3D11DeviceContext>     _Dx11DeviceContext      = nullptr;
		CComPtr<ID3D11RenderTargetView>  _Dx11RenderTargetView   = nullptr;
		CComPtr<ID3D11Texture2D>         _Dx11DepthStencilBuffer = nullptr;
		CComPtr<ID3D11DepthStencilState> _Dx11DepthStencilState  = nullptr;
		CComPtr<ID3D11DepthStencilView>  _Dx11StencilView        = nullptr;
		CComPtr<ID3D11RasterizerState>   _Dx11RasterState        = nullptr;

	public:
		// Runtime stuff.
		GDAPI virtual DIResult Runtime_PreInitialize() override final;
		GDAPI virtual DIResult Runtime_Initialize() override final;
		GDAPI virtual DIResult Runtime_Deinitialize() override final;
		GDAPI virtual void Runtime_PreUpdate() override {}
		GDAPI virtual void Runtime_Update() override;
		GDAPI virtual void Runtime_PostUpdate() override;

		// Output stuff.
		GDAPI virtual DIResult Output_SetMode(DGFXOutputMode const GfxOutputMode, bool const ForcelyReapply = false) override final;
		GDAPI virtual DIResult Output_SetResolution(GeomResolution const* const GfxResolution, bool const ForcelyReapply = false) override final;

		// Buffers stuff.
		GDAPI virtual DIResult Gfx_BufferCreate(DGFXHandle* const GfxBufferPtr, DFGXBufferType const GfxBufferType, SizeTp const GfxBufferSize, CHandle const GfxBufferInitialData) override final;
		GDAPI virtual Handle Gfx_BufferMap(DGFXHandle const GfxBuffer) override final;
		GDAPI virtual void Gfx_BufferUnmap(DGFXHandle const GfxBuffer) override final;

		// Shapes stuff.
		GDAPI virtual DIResult Gfx_ShapeCreate(DGFXHandle* const GfxShapePtr, DGFXTopologyType const GfxTopologyType) override final;
		GDAPI virtual DIResult Gfx_ShapeAttachVertexBuffer(DGFXHandle const GfxShape, DGFXHandle const GfxVertexBuffer) override final;
		GDAPI virtual DIResult Gfx_ShapeAttachIndexBuffer(DGFXHandle const GfxShape, DGFXHandle const GfxIndexBuffer) override final;

		// Shader stuff.
		GDAPI virtual DIResult Gfx_ShaderCreate(DGFXHandle* const GfxShaderPtr, DGFXShaderType const GfxShaderType, CHandle const GfxShaderByteCode, SizeTp const GfxShaderByteCodeLength) override final;

	};	// struct _DGFXDX11

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_API_MICROSOFT
