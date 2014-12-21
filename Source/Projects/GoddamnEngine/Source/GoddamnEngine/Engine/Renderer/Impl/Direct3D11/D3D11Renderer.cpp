#include <GoddamnEngine/Engine/Renderer/Impl/Direct3D11/D3D11Renderer.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxgi.lib")

GD_NAMESPACE_BEGIN
	
	GD_TYPEINFORMATION_IMPLEMENTATION(HRD3D11Interface, HRInterface, GDINT);

	bool HRD3D11Interface::CreateContex()
	{
#if 0
		{
			IDXGIFactory* factory = nullptr;
			Result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
			GD_ASSERT((SUCCEEDED(Result) && (factory != nullptr)), "Failed to create 'IDXGIFactory'");
			{
				IDXGIAdapter* adapter = nullptr;
				Result = factory->EnumAdapters(0, &adapter);
				GD_ASSERT((SUCCEEDED(Result) && (adapter != nullptr)), "Failed get D3D11 adapter using 'factory->EnumAdapters'");

				DXGI_ADAPTER_DESC adapterDescription = { 0 };
				Result = adapter->GetDesc(&adapterDescription);
				throw HRID3D11Exception("Getting information about GPU adapter failed");

				const size_t videoCardMemory = adapterDescription.DedicatedVideoMemory / 1024 / 1024;
				const String videoCardDescription(&adapterDescription.Description[0]);

				Debug::Log(String::Format("Found DirectX-compatible GPU named '%s' with video memory of '%d'MB", 
										  &videoCardDescription[0], 
										  (int)videoCardMemory));

				{
					IDXGIOutput* adapterOutput = nullptr;
						Result = adapter->EnumOutputs(0, &adapterOutput);
						GD_ASSERT(SUCCEEDED(Result) && (adapter != nullptr), "Failed get D3D11 adapter output using 'adapter->EnumOutputs'");
					{
						uint modificationsCount = 0;
						Result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &modificationsCount, nullptr);
						GD_ASSERT(SUCCEEDED(Result) && (modificationsCount != 0), "Failed get D3D11 mods count using 'adapterOutput->GetDisplayModeList'");
						
						Vector<DXGI_MODE_DESC>	displayModificationsList;
						displayModificationsList.Resize((size_t)modificationsCount);
						Result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &modificationsCount, &displayModificationsList[0]);
						GD_ASSERT(SUCCEEDED(Result) && (modificationsCount != 0), "Failed get D3D11 mods count using 'adapterOutput->GetDisplayModeList'");

						for (uint cnt = 0; cnt < modificationsCount; cnt += 1)
						{
							// const auto& dml = displayModificationsList[cnt];
							// const VsyncDescription description = { dml.RefreshRate.Numerator, dml.RefreshRate.Denominator };
							// this->resolutionsDescription.AddElement(Resolution((long)dml.Width, (long)dml.Height), description);
						}
					} adapterOutput->Release();
				} adapter->Release();
			} factory->Release();
		}
#endif
		auto const GameWindow = Application::GetInstance().GetApplicationGameWindow();
		HRESULT Result = E_FAIL;

		// Creating Device and swap chain
		DXGI_SWAP_CHAIN_DESC SwapChainDesc; 
		ZeroMemory(&SwapChainDesc, sizeof(SwapChainDesc));
		SwapChainDesc.BufferCount						 = 1;
		SwapChainDesc.BufferUsage						 = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		SwapChainDesc.BufferDesc.Width					 = static_cast<UINT>(this->ContextRectagle.Width);
		SwapChainDesc.BufferDesc.Height					 = static_cast<UINT>(this->ContextRectagle.Height);
		SwapChainDesc.BufferDesc.Format					 = DXGI_FORMAT_R8G8B8A8_UNORM;
		SwapChainDesc.BufferDesc.Scaling				 = DXGI_MODE_SCALING_UNSPECIFIED;
		SwapChainDesc.BufferDesc.ScanlineOrdering		 = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		SwapChainDesc.SwapEffect						 = DXGI_SWAP_EFFECT_DISCARD;
		SwapChainDesc.Flags								 = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		SwapChainDesc.OutputWindow						 = reinterpret_cast<HWND>(GameWindow->GetWindowNativeHandle());
		SwapChainDesc.Windowed							 = BOOL(!GameWindow->GetWindowIsFullscreen());
		SwapChainDesc.BufferDesc.RefreshRate.Numerator	 = 0;
		SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1; 
		SwapChainDesc.SampleDesc.Count					 = 1;
		SwapChainDesc.SampleDesc.Quality				 = 0;

		D3D_FEATURE_LEVEL       CreatedFeatureLevel    = D3D_FEATURE_LEVEL(0);
		D3D_FEATURE_LEVEL static const FeatureLevels[] = {
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
		};
		
		if (D3D11CreateDeviceAndSwapChain(
			nullptr,						/*  IDXGIAdapter *pAdapter */
			D3D_DRIVER_TYPE_HARDWARE,		/*  D3D_DRIVER_TYPE DriverType */
			nullptr,						/*  HMODULE Software */
			(UINT)0,						/*  UINT Flags */
			&FeatureLevels[0],				/*  const D3D_FEATURE_LEVEL *pFeatureLevels */
			GD_ARRAY_SIZE(FeatureLevels),	/*  UINT FeatureLevels */
			D3D11_SDK_VERSION,				/*  UINT SDKVersion */
			&SwapChainDesc,					/*  const DXGI_SWAP_CHAIN_DESC *pSwapChainDesc */
			&this->SwapChain,	            /* _Out_  IDXGISwapChain **ppSwapChain */
			&this->Device,		            /* _Out_  ID3D11Device **ppDevice */
			&CreatedFeatureLevel,			/* _Out_  D3D_FEATURE_LEVEL *pFeatureLevel */
			&this->Context)) {	            /* _Out_  ID3D11DeviceContext **ppImmediateContext */
			throw HRID3D11Exception("Failed to create DirectX Device and Swap Chain");
		}
		
		/* Setting up back buffer */ {
			D3D11RefPtr<ID3D11Texture2D> BackBuffer;
			if (FAILED(Result = this->SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(BackBuffer.GetAddressOf())))) {
				throw HRID3D11Exception("Getting pointer to back buffer failed");
			}

			if (FAILED(Result = this->Device->CreateRenderTargetView(BackBuffer.Get(), nullptr, &this->RenderTargetView))) {
				throw HRID3D11Exception("RenderTargetView creation failed.");
			}
		}

		/*Create depth buffer*/ {
			D3D11_TEXTURE2D_DESC DepthBufferDescription; 
			ZeroMemory(&DepthBufferDescription, sizeof(DepthBufferDescription));
			DepthBufferDescription.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
			DepthBufferDescription.Height             = static_cast<UINT>(this->ContextRectagle.Height);
			DepthBufferDescription.Width              = static_cast<UINT>(this->ContextRectagle.Width);
			DepthBufferDescription.MipLevels          = 1;
			DepthBufferDescription.ArraySize          = 1;
			DepthBufferDescription.SampleDesc.Count   = 1;
			DepthBufferDescription.SampleDesc.Quality = 0;
			DepthBufferDescription.Usage              = D3D11_USAGE_DEFAULT;
			DepthBufferDescription.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
			DepthBufferDescription.CPUAccessFlags     = 0;
			DepthBufferDescription.MiscFlags          = 0;

			if (FAILED(Result = this->Device->CreateTexture2D(&DepthBufferDescription, nullptr, &this->DepthStencilBuffer))) {
				throw HRID3D11Exception("Creation of Depth buffer failed.");
			}
		}

		/*Depth stencil description*/ {
			D3D11_DEPTH_STENCIL_DESC DepthStencilDesc; 
			ZeroMemory(&DepthStencilDesc, sizeof(DepthStencilDesc));
			DepthStencilDesc.DepthEnable                  = TRUE;
			DepthStencilDesc.DepthWriteMask               = D3D11_DEPTH_WRITE_MASK_ALL;
			DepthStencilDesc.DepthFunc                    = D3D11_COMPARISON_LESS;
			DepthStencilDesc.StencilEnable                = TRUE;
			DepthStencilDesc.StencilReadMask              = UINT8(0xFF);
			DepthStencilDesc.StencilWriteMask             = UINT8(0xFF);
			DepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
			DepthStencilDesc.FrontFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;
			DepthStencilDesc.FrontFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;
			DepthStencilDesc.FrontFace.StencilFunc        = D3D11_COMPARISON_ALWAYS;
			DepthStencilDesc.BackFace.StencilDepthFailOp  = D3D11_STENCIL_OP_DECR;
			DepthStencilDesc.BackFace.StencilFailOp       = D3D11_STENCIL_OP_KEEP;
			DepthStencilDesc.BackFace.StencilPassOp       = D3D11_STENCIL_OP_KEEP;
			DepthStencilDesc.BackFace.StencilFunc         = D3D11_COMPARISON_ALWAYS;

			if (FAILED(Result = this->Device->CreateDepthStencilState(&DepthStencilDesc, &this->DepthStencilState))) {
				throw HRID3D11Exception("DepthStencilState creation failed");
			}
		}	this->Context->OMSetDepthStencilState(this->DepthStencilState.Get(), 1);

		/*Depth stencil view description*/ {
			D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc; 
			ZeroMemory(&DepthStencilViewDesc, sizeof(DepthStencilViewDesc));
			DepthStencilViewDesc.Format        = DXGI_FORMAT_D24_UNORM_S8_UINT;
			DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			DepthStencilViewDesc.Texture2D.MipSlice = 0;

			if (FAILED(Result = this->Device->CreateDepthStencilView(this->DepthStencilBuffer.Get(), &DepthStencilViewDesc, &this->DepthStencilView))) {
				throw HRID3D11Exception("DepthStencilView creation failed");
			}
		}	this->Context->OMSetRenderTargets(1, &this->RenderTargetView, this->DepthStencilView.Get() /*nullptr*/);

		/*Rasterizer Description*/ {
			D3D11_RASTERIZER_DESC RasterizerDesc; 
			ZeroMemory(&RasterizerDesc, sizeof(RasterizerDesc));
			RasterizerDesc.AntialiasedLineEnable = FALSE;
			RasterizerDesc.CullMode              = D3D11_CULL_BACK;
			RasterizerDesc.DepthBias             = 0;
			RasterizerDesc.DepthBiasClamp        = 0.0f;
			RasterizerDesc.DepthClipEnable       = TRUE;
			RasterizerDesc.FillMode              = D3D11_FILL_SOLID;
			RasterizerDesc.SlopeScaledDepthBias  = 0.0f;
			RasterizerDesc.FrontCounterClockwise = FALSE;
			RasterizerDesc.MultisampleEnable     = FALSE;
			RasterizerDesc.ScissorEnable         = FALSE;

			if (FAILED(Result = this->Device->CreateRasterizerState(&RasterizerDesc, &this->SolidRasterizerState))) {
				throw HRID3D11Exception("Rasterizer State creation failed");
			}
		}	this->Context->RSSetState(this->SolidRasterizerState.Get());
			
		/*Setting up default viewport*/ {
			D3D11_VIEWPORT DefaultViewport; 
			ZeroMemory(&DefaultViewport, sizeof(DefaultViewport));
			DefaultViewport.Width    = FLOAT(this->ContextRectagle.Width);
			DefaultViewport.Height   = FLOAT(this->ContextRectagle.Height);
			DefaultViewport.TopLeftX = 0.0f;
			DefaultViewport.TopLeftY = 0.0f;
			DefaultViewport.MinDepth = 0.0f;
			DefaultViewport.MaxDepth = 1.0f;
			this->Context->RSSetViewports(1, &DefaultViewport);
		}

		this->Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		return true;
	}

	bool HRD3D11Interface::DestroyContext()
	{
		this->SwapChain->SetFullscreenState(FALSE, nullptr);
		this->SolidRasterizerState.Reset();
		this->DepthStencilView    .Reset();
		this->DepthStencilState   .Reset();
		this->DepthStencilBuffer  .Reset();
		this->RenderTargetView    .Reset();
		this->Context             .Reset();
		this->Device              .Reset();
		this->SwapChain           .Reset();

		return true;
	}

	void HRD3D11Interface::ClearContext(Rectangle const& ClearingViewport, Color const& ClearColor, bool const DoClearDepth /* = true */) 
	{
		GD_UNUSED(ClearingViewport);
		if (DoClearDepth) {
			this->Context->ClearDepthStencilView(this->DepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
		}

		this->Context->ClearRenderTargetView(this->RenderTargetView.Get(), &ClearColor[0]);
	}

	void HRD3D11Interface::SwapBuffers()
	{
		this->SwapChain->Present(0, 0);
	}

GD_NAMESPACE_END

#if (!defined(GD_MONOLITHIC_ENGINE))
/// ==========================================================================================
GDEXP extern bool EnginePluginEntry(GD PluginDescription* const Description)
{
#if (defined(GD_NAMESPACE))
	using namespace GD_NAMESPACE;
#endif	// if (defined(GD_NAMESPACE))

	Description->Type                  = GD_PLUGIN_TYPE_RENDERER;
	Description->Description           = "Default GoddamnEngine`s Direct3D11 Renderer";
	Description->PluginTypeInformation = HRD3D11Interface::GetClassTypeInformation();

	return true;
}
#endif	// if (!defined(GD_MONOLITHIC_ENGINE))
