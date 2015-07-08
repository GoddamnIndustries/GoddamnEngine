// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/DInterface/DGFX/_DGFXD3X11/_DGFXD3X11.cpp
 * File contains export implementation for interface for DirectX 11 implementation of the graphics interface.
 */
#include <GoddamnEngine/DInterface/DGFX/_DGFXDX11/_DGFXDX11.h>
#define GD_DLOG_CAT "GFX device (DirectX 11)"

#include <GoddamnEngine/Core/Containers/StaticVector.h>

//! @todo Move this as a dependency.
#include <d3dcompiler.h>
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "dxgi.lib")

GD_NAMESPACE_BEGIN

	GDINL static void _Hr(HRESULT const _Hr)
	{
		GD_ASSERT(SUCCEEDED(_Hr), "WINAPI\\D3D11 function has failed.");
	}

	// ------------------------------------------------------------------------------------------
	//! Function would be called on the global initialization step, before all other interfaces
	//! are initialized.
	//! @returns Non-negative value if the operation succeeded.
	GD_DFUNCTION("public DIResult Runtime_PostInitialize() @nogc nothrow")
	GDAPI DIResult _DGFXDX11::Runtime_PreInitialize()
	{
		// Retrieving information about the list of supported screen resolutions.
		{
			CComPtr<IDXGIFactory> _DxgiFactory = nullptr;
			CComPtr<IDXGIAdapter> _DxgiAdapter = nullptr;
			CComPtr<IDXGIOutput>  _DxgiAdapterOutput = nullptr;
			UINT _DxgiMatchedModesNum = 0;
			_Hr(CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&_DxgiFactory.p)));
			_Hr(_DxgiFactory->EnumAdapters(0, &_DxgiAdapter.p));
			_Hr(_DxgiAdapter->EnumOutputs(0, &_DxgiAdapterOutput.p));
			_Hr(_DxgiAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &_DxgiMatchedModesNum, nullptr));

			Vector<DXGI_MODE_DESC> _DxgiMatchedModes(_DxgiMatchedModesNum);
			_Hr(_DxgiAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &_DxgiMatchedModesNum, _DxgiMatchedModes.GetData()));
			_GfxResolutionsList.Reserve(_DxgiMatchedModesNum);
			for (auto const& _DxgiMode : Reverse(_DxgiMatchedModes))
				_GfxResolutionsList.InsertLast({ _DxgiMode.Width, _DxgiMode.Height, _DxgiMode.RefreshRate.Numerator, _DxgiMode.RefreshRate.Denominator });
			_GfxResolutionSelected = &_GfxResolutionsList.GetData()[0];
		}

		//! @todo Load default parameters.
		_GfxOutputMode = DGFX_OUTPUT_MODE_PSEUDO_FULLSCREEN;	// Easier for debugging purposes.
		_GfxResolutionSelected = &_GfxResolutionsList.GetData()[0];

		return DIResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Function would be called on the global initialization step.
	//! @returns Non-negative value if the operation succeeded.
	GD_DFUNCTION("public DIResult Runtime_Initialize() @nogc nothrow")
	GDAPI DIResult _DGFXDX11::Runtime_Initialize()
	{
		_CheckNotInitialized();
		GD_DLOG(GD_DLOG_CAT ": going to initialize graphics devices...");

		DIResult const _BaseResult = _DGFXBase::Runtime_Initialize();
		if (DIFailed(_BaseResult))
			return _BaseResult;

		// Initializing Direct3D 11 inside our main window..
		D3D_FEATURE_LEVEL _D3dCreatedFeatureLevel;
		D3D_FEATURE_LEVEL static const _D3dDesiredFeatureLevel = D3D_FEATURE_LEVEL_11_0;
		DXGI_SWAP_CHAIN_DESC _DxgiSwapChainDesc           = { };
		_DxgiSwapChainDesc.BufferCount                    = 1;
		_DxgiSwapChainDesc.BufferDesc.Width               = _GfxResolutionSelected->Width;
		_DxgiSwapChainDesc.BufferDesc.Height              = _GfxResolutionSelected->Height;
		_DxgiSwapChainDesc.BufferDesc.Format              = DXGI_FORMAT_R8G8B8A8_UNORM;
		_DxgiSwapChainDesc.BufferDesc.RefreshRate         = { _GfxResolutionSelected->VsyncNumerator, _GfxResolutionSelected->VsyncDenominator };
		_DxgiSwapChainDesc.BufferUsage                    = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		_DxgiSwapChainDesc.OutputWindow                   = _HwndMain; 
		_DxgiSwapChainDesc.SampleDesc                     = { 1, 0 };
		_DxgiSwapChainDesc.Windowed                       = true;	// _GfxOutputMode != DGFX_OUTPUT_MODE_FULLSCREEN; // - would be set later.
		_DxgiSwapChainDesc.BufferDesc.ScanlineOrdering    = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		_DxgiSwapChainDesc.BufferDesc.Scaling             = DXGI_MODE_SCALING_UNSPECIFIED;
		_DxgiSwapChainDesc.SwapEffect                     = DXGI_SWAP_EFFECT_DISCARD;
		_DxgiSwapChainDesc.Flags                          = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		_Hr(D3D11CreateDeviceAndSwapChain(nullptr
			, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0
			, &_D3dDesiredFeatureLevel, 1, D3D11_SDK_VERSION, &_DxgiSwapChainDesc
			, &_Dx11SwapChain.p, &_Dx11Device.p, &_D3dCreatedFeatureLevel, &_Dx11DeviceContext.p)
			);
		{
			CComPtr<ID3D11Texture2D> _D3dBackBuffer;
			_Hr(_Dx11SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&_D3dBackBuffer.p)));
			_Hr(_Dx11Device->CreateRenderTargetView(_D3dBackBuffer.p, nullptr, &_Dx11RenderTargetView.p));
		}
		_Dx11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// ..primitive depth buffer..
		D3D11_TEXTURE2D_DESC _D3d11DepthBufferDesc        = { };
		_D3d11DepthBufferDesc.Width                       = _GfxResolutionSelected->Width;
		_D3d11DepthBufferDesc.Height                      = _GfxResolutionSelected->Height;
		_D3d11DepthBufferDesc.MipLevels                   = 1;
		_D3d11DepthBufferDesc.ArraySize                   = 1;
		_D3d11DepthBufferDesc.Format                      = DXGI_FORMAT_D24_UNORM_S8_UINT;
		_D3d11DepthBufferDesc.SampleDesc                  = { 1, 0 };
		_D3d11DepthBufferDesc.Usage                       = D3D11_USAGE_DEFAULT;
		_D3d11DepthBufferDesc.BindFlags                   = D3D11_BIND_DEPTH_STENCIL;
		_D3d11DepthBufferDesc.CPUAccessFlags              = 0;
		_D3d11DepthBufferDesc.MiscFlags                   = 0;
		_Hr(_Dx11Device->CreateTexture2D(&_D3d11DepthBufferDesc, nullptr, &_Dx11DepthStencilBuffer.p));

		// ..and depth stencil buffer..
		D3D11_DEPTH_STENCIL_DESC _D3dDepthStencilDesc     = { };
		_D3dDepthStencilDesc.DepthEnable                  = true;
		_D3dDepthStencilDesc.DepthWriteMask               = D3D11_DEPTH_WRITE_MASK_ALL;
		_D3dDepthStencilDesc.DepthFunc                    = D3D11_COMPARISON_LESS;
		_D3dDepthStencilDesc.StencilEnable                = true;
		_D3dDepthStencilDesc.StencilReadMask              = 0xFF;
		_D3dDepthStencilDesc.StencilWriteMask             = 0xFF;
		_D3dDepthStencilDesc.FrontFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;
		_D3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		_D3dDepthStencilDesc.FrontFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;
		_D3dDepthStencilDesc.FrontFace.StencilFunc        = D3D11_COMPARISON_ALWAYS;
		_D3dDepthStencilDesc.BackFace.StencilFailOp       = D3D11_STENCIL_OP_KEEP;
		_D3dDepthStencilDesc.BackFace.StencilDepthFailOp  = D3D11_STENCIL_OP_DECR;
		_D3dDepthStencilDesc.BackFace.StencilPassOp       = D3D11_STENCIL_OP_KEEP;
		_D3dDepthStencilDesc.BackFace.StencilFunc         = D3D11_COMPARISON_ALWAYS;
		_Hr(_Dx11Device->CreateDepthStencilState(&_D3dDepthStencilDesc, &_Dx11DepthStencilState.p));
		_Dx11DeviceContext->OMSetDepthStencilState(_Dx11DepthStencilState, 1);

		// ..and depth stencil buffer (2x)..
		D3D11_DEPTH_STENCIL_VIEW_DESC _D3dDepthStencilViewDesc = { };
		_D3dDepthStencilViewDesc.Format                   = DXGI_FORMAT_D24_UNORM_S8_UINT;
		_D3dDepthStencilViewDesc.ViewDimension            = D3D11_DSV_DIMENSION_TEXTURE2D;
		_D3dDepthStencilViewDesc.Texture2D.MipSlice       = 0;
		_Hr(_Dx11Device->CreateDepthStencilView(_Dx11DepthStencilBuffer, &_D3dDepthStencilViewDesc, &_Dx11StencilView.p));
		_Dx11DeviceContext->OMSetRenderTargets(1, &_Dx11RenderTargetView.p, _Dx11StencilView);

		// ..and rasterizer..
		D3D11_RASTERIZER_DESC _D3d11RasterDesc            = { };
		_D3d11RasterDesc.AntialiasedLineEnable            = false;
		_D3d11RasterDesc.CullMode                         = D3D11_CULL_BACK;
		_D3d11RasterDesc.DepthBias                        = 0;
		_D3d11RasterDesc.DepthBiasClamp                   = 0.0f;
		_D3d11RasterDesc.DepthClipEnable                  = true;
		_D3d11RasterDesc.FillMode                         = D3D11_FILL_SOLID;
		_D3d11RasterDesc.FrontCounterClockwise            = false;
		_D3d11RasterDesc.MultisampleEnable                = false;
		_D3d11RasterDesc.ScissorEnable                    = false;
		_D3d11RasterDesc.SlopeScaledDepthBias             = 0.0f;
		_Hr(_Dx11Device->CreateRasterizerState(&_D3d11RasterDesc, &_Dx11RasterState.p));
		_Dx11DeviceContext->RSSetState(_Dx11RasterState);

		// ..and some default viewport.
		D3D11_VIEWPORT _D3dViewport                       = { };
		_D3dViewport.Width                                = static_cast<float>(_GfxResolutionSelected->Width);
		_D3dViewport.Height                               = static_cast<float>(_GfxResolutionSelected->Height);
		_D3dViewport.MinDepth                             = 0.0f;
		_D3dViewport.MaxDepth                             = 1.0f;
		_D3dViewport.TopLeftX                             = 0.0f;
		_D3dViewport.TopLeftY                             = 0.0f;
		_Dx11DeviceContext->RSSetViewports(1, &_D3dViewport);
		
#if 0
		Runtime_PostInitialize();

		typedef float float4[4];
		struct VertexType
		{
			float4 position;
			float4 color;
		};
		static const VertexType triangle [] = {
			{ { -1.0f, -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
			{ { 0.0f, 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
			{ { 1.0f, -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
		};
		UINT indices [] = { 0, 1, 2 };

		ID3D11InputLayout* m_layout;
		ID3D11VertexShader* m_vertexShader;
		ID3D11PixelShader* m_pixelShader;
		ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
		{
			D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
			D3D11_SUBRESOURCE_DATA vertexData, indexData;

			vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			vertexBufferDesc.ByteWidth = sizeof(triangle);
			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertexBufferDesc.CPUAccessFlags = 0;
			vertexBufferDesc.MiscFlags = 0;
			vertexBufferDesc.StructureByteStride = 0;
			vertexData.pSysMem = triangle;
			vertexData.SysMemPitch = 0;
			vertexData.SysMemSlicePitch = 0;
			_Hr(_Dx11Device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer));

			indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			indexBufferDesc.ByteWidth = sizeof(indices);
			indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			indexBufferDesc.CPUAccessFlags = 0;
			indexBufferDesc.MiscFlags = 0;
			indexBufferDesc.StructureByteStride = 0;
			indexData.pSysMem = indices;
			indexData.SysMemPitch = 0;
			indexData.SysMemSlicePitch = 0;
			_Hr(_Dx11Device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer));

			ID3D10Blob* errorMessage;
			ID3D10Blob* vertexShaderBuffer;
			ID3D10Blob* pixelShaderBuffer;
			D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
			_Hr(D3DCompileFromFile(L"xvs.hlsl", NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
				&vertexShaderBuffer, &errorMessage));
			_Hr(D3DCompileFromFile(L"xps.hlsl", NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
				&pixelShaderBuffer, &errorMessage));

			_Hr(_Dx11Device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader));
			_Hr(_Dx11Device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader));

			polygonLayout[0].SemanticName = "POSITION";
			polygonLayout[0].SemanticIndex = 0;
			polygonLayout[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			polygonLayout[0].InputSlot = 0;
			polygonLayout[0].AlignedByteOffset = 0;
			polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			polygonLayout[0].InstanceDataStepRate = 0;
			polygonLayout[1].SemanticName = "COLOR";
			polygonLayout[1].SemanticIndex = 0;
			polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			polygonLayout[1].InputSlot = 0;
			polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			polygonLayout[1].InstanceDataStepRate = 0;
			_Hr(_Dx11Device->CreateInputLayout(polygonLayout, 2, vertexShaderBuffer->GetBufferPointer(),
				vertexShaderBuffer->GetBufferSize(), &m_layout));
		}

		while (true)
		{
			MSG msg;
			while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			float c [] = { 0.3f, 0.275f, 1.0f, 1.0f };
			_Dx11DeviceContext->ClearRenderTargetView(_Dx11RenderTargetView, c);
			_Dx11DeviceContext->ClearDepthStencilView(_Dx11StencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

			{
				unsigned int stride;
				unsigned int offset;
				stride = sizeof(VertexType);
				offset = 0;
				_Dx11DeviceContext->IASetInputLayout(m_layout);
				_Dx11DeviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
				_Dx11DeviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
				_Dx11DeviceContext->VSSetShader(m_vertexShader, NULL, 0);
				_Dx11DeviceContext->PSSetShader(m_pixelShader, NULL, 0);
				_Dx11DeviceContext->DrawIndexed(3, 0, 0);
			}

			_Dx11SwapChain->Present(true ? 1 : 0, 0);
		}
#endif

		GD_DLOG(GD_DLOG_CAT ": ... initialized.");
		return DIResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Function would be called on the global deinitialization step.
	//! @returns Non-negative value if the operation succeeded.
	GD_DFUNCTION("public DIResult Runtime_Deinitialize() @nogc nothrow")
	GDAPI DIResult _DGFXDX11::Runtime_Deinitialize()
	{
		//! @todo Forcedly release all GFX's objects.

		_Dx11RasterState.Release();
		_Dx11StencilView.Release();
		_Dx11DepthStencilState.Release();
		_Dx11DepthStencilBuffer.Release();
		_Dx11RenderTargetView.Release();
		_Dx11DeviceContext.Release();
		_Dx11Device.Release();
		_Dx11SwapChain.Release();

		return _DGFXBase::Runtime_Deinitialize();
	}

	// ------------------------------------------------------------------------------------------
	GDAPI void _DGFXDX11::Runtime_Update()
	{
		_DGFXBase::Runtime_Update();

		float static const c [] = { 0.3f, 0.275f, 1.0f, 1.0f };
		_Dx11DeviceContext->ClearRenderTargetView(_Dx11RenderTargetView, c);
		_Dx11DeviceContext->ClearDepthStencilView(_Dx11StencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	// ------------------------------------------------------------------------------------------
	GDAPI void _DGFXDX11::Runtime_PostUpdate()
	{
		_Dx11SwapChain->Present(_GfxVsyncMode == DGFX_OUTPUT_VSYNC_DISABLED ? 1 : 0, 0);
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Output functionality management.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Changes the mode in which the graphics device is running. 
	//! @returns Non-negative value if the operation succeeded.
	GD_DFUNCTION("public DIResult Output_SetMode(DGFXOutputMode GfxOutputMode, bool GfxForcelyReapply = false) @nogc nothrow")
	GDAPI DIResult _DGFXDX11::Output_SetMode(
		DGFXOutputMode const GfxOutputMode,		//!< The new output mode that would be set.
		bool           const GfxForcelyReapply	//!< Do forcedly reapply mode, even if device is running is the same mode.
	){
		_CheckInitialized();
		GD_DLOG(GD_DLOG_CAT ": mode is going to be set to %d...", GfxOutputMode);

		DIResult const _BaseResult = _DGFXBase::Output_SetMode(GfxOutputMode, GfxForcelyReapply);
		if (_BaseResult != DIResult::Ok)
			return _BaseResult;

		_Hr(_Dx11SwapChain->SetFullscreenState(GfxOutputMode == DGFX_OUTPUT_MODE_FULLSCREEN, nullptr));
		return DIResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Changes resolution in which graphics output device is rendering and resize several frame
	//! buffers (those, that have a corresponding flag specified).
	//! @returns Non-negative value if the operation succeeded.
	GD_DFUNCTION("public DIResult Output_SetResolution(const(GeomResolution)* GfxResolution, bool GfxForcelyReapply = false) @nogc nothrow")
	GDAPI DIResult _DGFXDX11::Output_SetResolution(
		GeomResolution const* const GfxResolution,    //!< Pointer to the resolution descriptor that would be set.
		bool                  const GfxForcelyReapply //!< Do forcedly resolution mode, even if device is running is the same resolution.
	){
		GD_NOT_USED_L(GfxResolution, GfxForcelyReapply);
		return DIResult::NotImplemented;
#if 0
		_CheckInitialized();
		GD_DLOG(GD_DLOG_CAT ": resolution is going to be set to %dx%d@%d..."
			, _GfxResolutionSelected->Width, _GfxResolutionSelected->Height, _GfxResolutionSelected->VsyncNumerator / _GfxResolutionSelected->VsyncDenominator);

		DIResult const _BaseResult = _DGFXBase::Output_SetResolution(GfxResolution, GfxForcelyReapply);
		if (_BaseResult != DIResult::Ok)
			return _BaseResult;

		// Resizing screen targets.
		DXGI_MODE_DESC _DxgiModeDesc = {};
		_DxgiModeDesc.Width       = _GfxResolutionSelected->Width;
		_DxgiModeDesc.Height      = _GfxResolutionSelected->Height;
		_DxgiModeDesc.Format      = DXGI_FORMAT_R8G8B8A8_UNORM;
		_DxgiModeDesc.RefreshRate = { GfxResolution->VsyncNumerator, GfxResolution->VsyncDenominator };
		_Hr(_Dx11SwapChain->ResizeTarget(&_DxgiModeDesc));

		//! @todo Resize all primitive buffers.
		//! @todo Now we need to resize all frame buffers, that have a corresponding flag.

		return DIResult::Ok;
#endif	// if 0
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Graphics device <-> CPU buffers functions.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	struct _DGFXDX11BufferHandle : public DGFX::_DGFXHandle
	{
		CComPtr<ID3D11Buffer>    BuffPtr        = nullptr;
		D3D11_BUFFER_DESC        BuffDesc;
		D3D11_MAPPED_SUBRESOURCE BuffMappedData;
	};	// struct _DGFXDX11BufferInst

	// ------------------------------------------------------------------------------------------
	//! Creates a GPU buffer with specified parameters.
	//! @returns Non-negative value if the operation succeeded.
	GD_DFUNCTION("public DIResult Gfx_BufferCreate(DGFXBufferHandle* GfxBufferPtr, DFGXBufferType GfxBufferType"
		", size_t GfxBufferSize, const(void)* GfxBufferInitialData) @nogc nothrow")
	GDAPI DIResult _DGFXDX11::Gfx_BufferCreate(
		DGFXHandle*    const GfxBufferPtr,			//!< Pointer for output.
		DFGXBufferType const GfxBufferType,			//!< Type of the buffer: vertex, index, constant or etc buffer.
		SizeTp         const GfxBufferSize,			//!< Size of the buffer in bytes.
		CHandle        const GfxBufferInitialData	//!< Initial data of the buffer. Can be null pointer for en empty buffer.
	){
		_CheckInitialized();

		_DGFXDX11BufferHandle* _GfxNewBuffer = GD_NEW(_DGFXDX11BufferHandle);
		_GfxNewBuffer->BuffDesc.ByteWidth              = static_cast<UINT>(GfxBufferSize);
		_GfxNewBuffer->BuffDesc.StructureByteStride    = 0;
		switch (GfxBufferType)
		{
			case DGFX_BUFFER_TYPE_VERTEX: 
				GD_DASSERT(GfxBufferInitialData != nullptr
					, "Empty initial data is not allowed for this types of buffers.");
				_GfxNewBuffer->BuffDesc.Usage          = D3D11_USAGE_IMMUTABLE;
				_GfxNewBuffer->BuffDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
				_GfxNewBuffer->BuffDesc.CPUAccessFlags = 0;
				_GfxNewBuffer->BuffDesc.MiscFlags      = 0;
				break;

			case DGFX_BUFFER_TYPE_INDEX: 
				GD_DASSERT(GfxBufferInitialData != nullptr
					, "Empty initial data is not allowed for this types of buffers.");
				_GfxNewBuffer->BuffDesc.Usage          = D3D11_USAGE_IMMUTABLE;
				_GfxNewBuffer->BuffDesc.BindFlags      = D3D11_BIND_INDEX_BUFFER;
				_GfxNewBuffer->BuffDesc.CPUAccessFlags = 0;
				_GfxNewBuffer->BuffDesc.MiscFlags      = 0;
				break;

			case DGFX_BUFFER_TYPE_CONSTANT: 
				_GfxNewBuffer->BuffDesc.Usage          = D3D11_USAGE_DYNAMIC;
				_GfxNewBuffer->BuffDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
				_GfxNewBuffer->BuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				_GfxNewBuffer->BuffDesc.MiscFlags      = 0;
				break;

			default: 
				GD_DEBUG_ASSERT_FALSE("'_DGFXDX11::Gfx_CreateBuffer' error: "
					"Invalid buffer type was specified.");
		}

		D3D11_SUBRESOURCE_DATA _Dx11NewBufferInitialData = {}, *_Dx11NewBufferInitialDataPtr = nullptr;
		if (GfxBufferInitialData != nullptr)
		{
			_Dx11NewBufferInitialDataPtr               = &_Dx11NewBufferInitialData;
			_Dx11NewBufferInitialData.pSysMem          = GfxBufferInitialData;
			_Dx11NewBufferInitialData.SysMemPitch      = 0;
			_Dx11NewBufferInitialData.SysMemSlicePitch = 0;
		}

		_Hr(_Dx11Device->CreateBuffer(&_GfxNewBuffer->BuffDesc, _Dx11NewBufferInitialDataPtr, &_GfxNewBuffer->BuffPtr.p));
		*GfxBufferPtr = _Handle_InitializeObject<_DGFXDX11BufferHandle>(_GfxNewBuffer);
		return DIResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Maps a GPU buffer onto CPU memory.
	//! @returns Pointer to mapped memory or null pointer, if failed.
	GD_DFUNCTION("public void* Gfx_BufferMap(DGFXHandle GfxBufferPtr) @nogc nothrow")
	GDAPI Handle _DGFXDX11::Gfx_BufferMap(
		DGFXHandle const GfxBuffer	//!< Buffer to map.
	){
		//! @todo Too simply. Need more stuff to do here.
		//! @see https://github.com/EpicGames/UnrealEngine/blob/release/Engine/Source/Runtime/Windows/D3D11RHI/Private/D3D11VertexBuffer.cpp#L88
		_CheckInitialized();
		if (GfxBuffer != nullptr)
		{
			auto const _GfxBuffer = _Handle_DereferenceObject<_DGFXDX11BufferHandle>(GfxBuffer);
			if (_GfxBuffer->BuffMappedData.pData == nullptr)
			{
				// Buffer was not already mapped, mapping.
				_Hr(_Dx11DeviceContext->Map(_GfxBuffer->BuffPtr, 0, D3D11_MAP_WRITE_DISCARD, 0, &_GfxBuffer->BuffMappedData));
			}
			return _GfxBuffer->BuffMappedData.pData;
		}
		return nullptr;

	}

	// ------------------------------------------------------------------------------------------
	//! Unmaps a GPU buffer and uploads data back to GPU memory.
	GD_DFUNCTION("public void Gfx_BufferUnmap(DGFXHandle GfxBufferPtr) @nogc nothrow")
	GDAPI void _DGFXDX11::Gfx_BufferUnmap(
		DGFXHandle const GfxBuffer	//!< Buffer to unmap.
	){
		//! @todo Too simply. Need more stuff to do here.
		//! @see https://github.com/EpicGames/UnrealEngine/blob/release/Engine/Source/Runtime/Windows/D3D11RHI/Private/D3D11VertexBuffer.cpp#L160
		_CheckInitialized();
		if (GfxBuffer != nullptr)
		{
			auto const _GfxBuffer = _Handle_DereferenceObject<_DGFXDX11BufferHandle>(GfxBuffer);
			if (_GfxBuffer->BuffMappedData.pData != nullptr)
			{
				// Buffer was mapped, unmapping.
				_Dx11DeviceContext->Unmap(_GfxBuffer->BuffPtr, 0);
				_GfxBuffer->BuffDesc = {};
			}
		}
	}
	
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Shape objects.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	struct _DGFXDX11ShapeHandle : public DGFX::_DGFXHandle
	{
		Vector<ID3D11Buffer*>    ShapeVertexBuffers;
		ID3D11Buffer*            ShapeIndexBuffer = nullptr;
		D3D11_PRIMITIVE_TOPOLOGY ShapeTopology    = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
	};	// struct _DGFXDX11BufferInst

	// ------------------------------------------------------------------------------------------
	//! Created a new shape that contains indexed vertices.
	//! @returns Non-negative value if the operation succeeded.
	GD_DFUNCTION("public DIResult Gfx_ShapeCreate(DGFXHandle* GfxBufferPtr"
		", DGFXTopologyType GfxTopologyType) @nogc nothrow")
	GDAPI DIResult _DGFXDX11::Gfx_ShapeCreate(
		DGFXHandle*      const GfxShapePtr,	//!< Pointer to the output handle.
		DGFXTopologyType const GfxTopologyType		//!< Type of topology that shape represents.
	){
		D3D_PRIMITIVE_TOPOLOGY static const _D3d11TopologyTable [] = {
			/* DGFX_TOPOLOGY_TYPE_UNKNOWN      */ D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED,
			/* DGFX_TOPOLOGY_TYPE_TRAINGLELIST */ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		};

		_CheckInitialized();
		_DGFXDX11ShapeHandle* _GfxNewShape = GD_NEW(_DGFXDX11ShapeHandle);
		_GfxNewShape->ShapeTopology = _D3d11TopologyTable[GfxTopologyType];
		*GfxShapePtr = _Handle_InitializeObject<_DGFXDX11ShapeHandle>(_GfxNewShape);
		return DIResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Attaches a new vertex buffer into indexed shape object. The order of the attached buffers
	//! should match the corresponding order in the vertex shader input layout.
	//! @returns Non-negative value if the operation succeeded.
	GD_DFUNCTION("public DIResult Gfx_ShapeAttachVertexBuffer(DGFXHandle GfxShape"
		", DGFXHandle GfxVertexBuffer) @nogc nothrow")
	GDAPI DIResult _DGFXDX11::Gfx_ShapeAttachVertexBuffer(
		DGFXHandle const GfxShape,			//!< Current shape handle.
		DGFXHandle const GfxVertexBuffer			//!< Vertex buffer to be attached.
	){
		_CheckInitialized();
		if (GfxShape != nullptr && GfxVertexBuffer != nullptr)
		{
			auto const _GfxShape = _Handle_DereferenceObject<_DGFXDX11ShapeHandle>(GfxShape);
			auto const _GfxVertexBuffer = _Handle_DereferenceObject<_DGFXDX11BufferHandle>(GfxVertexBuffer);
			_GfxShape->ShapeVertexBuffers.InsertLast(_GfxVertexBuffer->BuffPtr.p);
			return DIResult::Ok;
		}
		return DIResult::NothingWasDone;
	}

	// ------------------------------------------------------------------------------------------
	//! Attaches an index buffer to this indexed shape. If the index buffer was already attached
	//! to shape, changes it.
	//! @returns Non-negative value if the operation succeeded.
	GD_DFUNCTION("public DIResult Gfx_ShapeAttachVertexBuffer(DGFXHandle GfxShape"
		", DGFXHandle GfxIndexBuffer) @nogc nothrow")
	GDAPI DIResult _DGFXDX11::Gfx_ShapeAttachIndexBuffer(
		DGFXHandle       const GfxShape,		//!< Current shape handle.
		DGFXHandle       const GfxIndexBuffer		//!< Index buffer to be attached.
	){
		_CheckInitialized();
		if (GfxShape != nullptr && GfxIndexBuffer != nullptr)
		{
			auto const _GfxShape = _Handle_DereferenceObject<_DGFXDX11ShapeHandle>(GfxShape);
			auto const _GfxIndexBuffer = _Handle_DereferenceObject<_DGFXDX11BufferHandle>(GfxIndexBuffer);
			_GfxShape->ShapeIndexBuffer = _GfxIndexBuffer->BuffPtr.p;
			return DIResult::Ok;
		}
		return DIResult::NothingWasDone;
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Shader objects.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	struct _DGFXDX11ShaderHandle : public DGFX::_DGFXHandle
	{
		DGFXShaderType		        ShaderType;
		CComPtr<ID3D11VertexShader> ShaderVertexPtr;
		CComPtr<ID3D11InputLayout>  ShaderVertexInputLayout;
		CComPtr<ID3D11PixelShader>  ShaderPixelPtr;
	};	// struct _DGFXDX11BufferInst

	// ------------------------------------------------------------------------------------------
	//! Creates a GPU shader with specified parameters.
	//! @returns Non-negative value if the operation succeeded.
	GD_DFUNCTION("public DIResult Gfx_BufferCreate(DGFXHandle* GfxShaderPtr, DGFXShaderType GfxShaderType"
		", const(void)* GfxShaderByteCode, size_t GfxShaderByteCodeLength) @nogc nothrow")
		GDAPI DIResult _DGFXDX11::Gfx_ShaderCreate(
		DGFXHandle*    const GfxShaderPtr,			 //!< Pointer for output.
		DGFXShaderType const GfxShaderType,			 //!< Shader type.
		CHandle        const GfxShaderByteCode,		 //!< Pointer to the data of the shader byte code.
		SizeTp         const GfxShaderByteCodeLength //!< Length of the shader byte code data in bytes.
	){
		_CheckInitialized();
		_DGFXDX11ShaderHandle* _GfxNewShader = GD_NEW(_DGFXDX11ShaderHandle);
		_GfxNewShader->ShaderType = GfxShaderType;
		switch (_GfxNewShader->ShaderType)
		{
			// Creating the vertex shader. This is the most complicated one - we need to
			// handle the input layout too.
			case DGFX_SHADER_TYPE_VERTEX:
			{
				UINT _D3dLastSlotIndex = 0;
				D3D11_INPUT_ELEMENT_DESC _D3d11VertexShaderInputLayoutDescs[DGFX_SEMANTICS_COUNT] = {};
			//	for (UINT _D3dSemanticIndex = DGFX_SEMANTIC_POSITION; _D3dSemanticIndex != DGFX_SEMANTICS_COUNT; ++_D3dSemanticIndex)
			//	{
			//		//! @todo Check semantics here and generate all the trash.
			//		++_D3dLastSlotIndex;
			//	}

				// Creating vertex shader and the corresponding input layout.
				auto const _D3dUsedInputLayoutSlots = _D3dLastSlotIndex;
				_Hr(_Dx11Device->CreateVertexShader(GfxShaderByteCode, GfxShaderByteCodeLength, nullptr, &_GfxNewShader->ShaderVertexPtr.p));
				_Hr(_Dx11Device->CreateInputLayout(_D3d11VertexShaderInputLayoutDescs, _D3dUsedInputLayoutSlots
					, GfxShaderByteCode, GfxShaderByteCodeLength, &_GfxNewShader->ShaderVertexInputLayout.p));
			} break;

			// Creating the simple vertex shader.
			case DGFX_SHADER_TYPE_PIXEL: 
				_Hr(_Dx11Device->CreatePixelShader(GfxShaderByteCode, GfxShaderByteCodeLength, nullptr, &_GfxNewShader->ShaderPixelPtr.p));
				break;

			default: 
				GD_DEBUG_ASSERT_FALSE("Invalid shader type specified.");
		}

		*GfxShaderPtr = _Handle_InitializeObject<_DGFXDX11ShaderHandle>(_GfxNewShader);
		return DIResult::Ok;
	}


GD_NAMESPACE_END
