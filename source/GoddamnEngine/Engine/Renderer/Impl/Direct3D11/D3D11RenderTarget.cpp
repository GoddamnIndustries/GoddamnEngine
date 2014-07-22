#include <GoddamnEngine/Engine/Renderer/Impl/Direct3D11/D3D11RenderTarget.h>
#include <GoddamnEngine/Engine/Renderer/Impl/Direct3D11/D3D11Texture2D.h>

GD_NAMESPACE_BEGIN

	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRID3D11RenderTarget, HRIRenderTarget, GDINT, nullptr);	
	HRID3D11RenderTarget::HRID3D11RenderTarget(HRIRenderTargetCtorInfo const& CtorInfo) : HRIRenderTarget(CtorInfo)
	{
		HRESULT Result = E_FAIL;
		ID3D11Device* const device = HRD3D11Interface::GetInstance().Device.Get();
		for (auto const TheTexture2D : self->RenderTargetTextures)
		{	// Creating render target for each texture.
			HRID3D11Texture2D const* const Texture2D = object_cast<HRID3D11Texture2D const*>(TheTexture2D);

			D3D11_RENDER_TARGET_VIEW_DESC RenderTargetViewDesc;
			ZeroMemory(&RenderTargetViewDesc, sizeof(RenderTargetViewDesc));
			RenderTargetViewDesc.Format = ToDxgiFormatUnorm(Texture2D->PixelFormat);
			RenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			RenderTargetViewDesc.Texture2D.MipSlice = 0;

			ID3D11RenderTargetView* RenderTargetView = nullptr;
			if (FAILED(Result = device->CreateRenderTargetView(Texture2D->GetTexture(), &RenderTargetViewDesc, &RenderTargetView))) {
				throw D3D11Exception("Failed to create render target view");
			}

			self->RenderTargetRenderTargets.PushLast(RenderTargetView);
		}
	}
		
	HRID3D11RenderTarget::~HRID3D11RenderTarget() 
	{
		for (auto const RenderTarget : self->RenderTargetRenderTargets)
			RenderTarget->Release();
	}

	void HRID3D11RenderTarget::BindRenderTarget() const
	{
		ID3D11DeviceContext   * const Context          = HRD3D11Interface::GetInstance().Context.Get();
		ID3D11DepthStencilView* const DepthStencilView = HRD3D11Interface::GetInstance().DepthStencilView.Get();
		Context->OMSetRenderTargets(static_cast<UINT>(self->RenderTargetRenderTargets.GetSize()), &self->RenderTargetRenderTargets[0], DepthStencilView);
	}

	void HRID3D11RenderTarget::UnbindRenderTarget() const
	{
		ID3D11DeviceContext   * const Context          = HRD3D11Interface::GetInstance().Context.Get();
		ID3D11DepthStencilView* const DepthStencilView = HRD3D11Interface::GetInstance().DepthStencilView.Get();
		ID3D11RenderTargetView* const RenderTargetView = HRD3D11Interface::GetInstance().RenderTargetView.Get();
		Context->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);
	}

	HRIRenderTarget* HRD3D11Interface::CreateRenderTarget(HRIRenderTargetCtorInfo const& CtorInfo)
	{
		return new HRID3D11RenderTarget(CtorInfo);
	}

GD_NAMESPACE_END
