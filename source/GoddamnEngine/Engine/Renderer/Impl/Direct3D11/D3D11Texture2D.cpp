#include <GoddamnEngine/Engine/Renderer/Impl/Direct3D11/D3D11Texture2D.h>

GD_NAMESPACE_BEGIN

	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRID3D11Texture2D, HRITexture2D, GDINT, nullptr);
	HRID3D11Texture2D::HRID3D11Texture2D(HRITexture2DCtorInfo const& CtorInfo, chandle const InitialData, size_t const InitialiDataLength) 
		: HRITexture2D(CtorInfo, InitialData, InitialiDataLength)
		,	Texture(nullptr)
		,	TextureShaderResource(nullptr)
	{
		ID3D11Device* const Device = HRD3D11Interface::GetInstance().Device.Get();
		this->CreateTexture(InitialData, Device);
		this->CreateShaderResourceView(Device);
		this->CreateTextureSample(Device);
	}

	void HRID3D11Texture2D::CreateTexture(chandle const InitialData, ID3D11Device* const device)
	{
		HRESULT result = S_OK;
		DXGI_FORMAT const texturePixelFormat = ToDxgiFormatUnorm(this->PixelFormat);
		bool textureGenerateMipMaps = false;
		{
			UINT texturePixelFormatSupport = 0;
			if (SUCCEEDED(device->CheckFormatSupport(texturePixelFormat, &texturePixelFormatSupport))
				&& ((texturePixelFormatSupport & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN) != 0)
				&& (this->Mode == GD_HRI_TEXTURE_2D_MODE_STATIC))
			{
				textureGenerateMipMaps = true;
			}
		}

		D3D11_TEXTURE2D_DESC texture2DDescription;
		ZeroMemory(&texture2DDescription, sizeof(texture2DDescription));
		texture2DDescription.Width  = static_cast<UINT>(this->TheResolution.Width);
		texture2DDescription.Height = static_cast<UINT>(this->TheResolution.Height);
		texture2DDescription.MipLevels = 1;
		texture2DDescription.ArraySize = 1;
		texture2DDescription.Format = texturePixelFormat;
		texture2DDescription.SampleDesc.Quality = 0;
		texture2DDescription.SampleDesc.Count   = 1;
		texture2DDescription.MiscFlags			= 0;

		switch (this->Mode)
		{
		case GD_HRI_TEXTURE_2D_MODE_STATIC:
			{
				GD_ASSERT((InitialData != nullptr), "For Static textures initial data should be specified");
				texture2DDescription.CPUAccessFlags = 0;
				if (textureGenerateMipMaps)
				{
					texture2DDescription.MipLevels  = 0;
					texture2DDescription.Usage	    = D3D11_USAGE_DEFAULT;
					texture2DDescription.MiscFlags  = D3D11_RESOURCE_MISC_GENERATE_MIPS;
					texture2DDescription.BindFlags  = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
				}
				else
				{
					texture2DDescription.Usage		= D3D11_USAGE_IMMUTABLE;
					texture2DDescription.BindFlags  = D3D11_BIND_SHADER_RESOURCE;
				}
			}	break;
		case GD_HRI_TEXTURE_2D_MODE_DYNAMIC:
			{
				texture2DDescription.Usage			= D3D11_USAGE_DYNAMIC;
				texture2DDescription.BindFlags		= D3D11_BIND_SHADER_RESOURCE;
				texture2DDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			}	break;
		case GD_HRI_TEXTURE_2D_MODE_RenderTarget:
			{
				texture2DDescription.Usage			= D3D11_USAGE_DEFAULT;
				texture2DDescription.BindFlags		= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
				texture2DDescription.CPUAccessFlags = 0;
			}	break;
		default: GD_DEBUG_ASSERT_FALSE("Unknwon mod specified"); break;
		}

		D3D11_SUBRESOURCE_DATA texture2DData;
		ZeroMemory(&texture2DData, sizeof(texture2DData));
		if (InitialData != nullptr)
		{
			texture2DData.pSysMem = InitialData;
			texture2DData.SysMemPitch = static_cast<UINT>(GD_FORMAT_SIZEOF(this->PixelFormat)
				* this->TheResolution.Width);
			texture2DData.SysMemSlicePitch = static_cast<UINT>(GD_FORMAT_SIZEOF(this->PixelFormat)
				* this->TheResolution.Width
				* this->TheResolution.Height);
		}

		result = device->CreateTexture2D(
			&texture2DDescription, 
			(((InitialData != nullptr) && (!textureGenerateMipMaps)) ? &texture2DData : nullptr),
			&this->Texture
		);

		GD_ASSERT((SUCCEEDED(result) && (this->Texture != nullptr)), "Failed to create texture");

		D3D11_SHADER_RESOURCE_VIEW_DESC texture2DShaderResource;
		ZeroMemory(&texture2DShaderResource, sizeof(texture2DShaderResource));
		texture2DShaderResource.Format = ToDxgiFormatUnorm(this->PixelFormat);
		texture2DShaderResource.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		texture2DShaderResource.Texture2D.MipLevels = (textureGenerateMipMaps) ? (-1) : 1;
		texture2DShaderResource.Texture2D.MostDetailedMip = 0;

		result = device->CreateShaderResourceView(
			this->Texture.Get(),
			&texture2DShaderResource,
			&this->TextureShaderResource
		);

		GD_ASSERT((SUCCEEDED(result) && (this->TextureShaderResource != nullptr)),
			"Failed to create shader resource");

		if (textureGenerateMipMaps)
		{
			ID3D11DeviceContext* const Context = HRD3D11Interface::GetInstance().Context.Get();
			Context->UpdateSubresource(this->Texture.Get(), 0, nullptr,
				texture2DData.pSysMem, texture2DData.SysMemPitch,
				texture2DData.SysMemSlicePitch);
			Context->GenerateMips(this->TextureShaderResource.Get());
		}
	}

	void HRID3D11Texture2D::CreateShaderResourceView(ID3D11Device* const /*device*/)
	{
	}

	void HRID3D11Texture2D::CreateTextureSample(ID3D11Device* const device)
	{
		D3D11_SAMPLER_DESC texture2DSamplerDesription;
		ZeroMemory(&texture2DSamplerDesription, sizeof(texture2DSamplerDesription));
		texture2DSamplerDesription.MipLODBias = 0.0f;
		texture2DSamplerDesription.MaxAnisotropy = 1;
		texture2DSamplerDesription.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		texture2DSamplerDesription.BorderColor[0] = 0.0f;
		texture2DSamplerDesription.BorderColor[1] = 0.0f;
		texture2DSamplerDesription.BorderColor[2] = 0.0f;
		texture2DSamplerDesription.BorderColor[3] = 0.0f;
		texture2DSamplerDesription.MinLOD = 0.0f;
		texture2DSamplerDesription.MaxLOD = D3D11_FLOAT32_MAX;

		switch (this->FilteringMode)
		{
		case GD_HRI_TEXTURE_2D_FILTERING_MODE_POINT:	texture2DSamplerDesription.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; break;
		case GD_HRI_TEXTURE_2D_FILTERING_MODE_BILINEAR: texture2DSamplerDesription.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; break;
		default: GD_DEBUG_ASSERT_FALSE("Unknwon mod specified"); break;
		}

		switch (this->WrapMode)
		{
			case GD_HRI_TEXTURE_2D_WRAP_MODE_CLAMP:
			{
				texture2DSamplerDesription.AddressU
					= texture2DSamplerDesription.AddressV
					= texture2DSamplerDesription.AddressW
					= D3D11_TEXTURE_ADDRESS_CLAMP;
			} break;
		case GD_HRI_TEXTURE_2D_WRAP_MODE_REPEAT:
			{
				texture2DSamplerDesription.AddressU
					= texture2DSamplerDesription.AddressV
					= texture2DSamplerDesription.AddressW
					= D3D11_TEXTURE_ADDRESS_WRAP;
			} break;
		default:
			{
				GD_DEBUG_ASSERT_FALSE("Unknwon mod specified");
			} break;
		}

		HRESULT const result = device->CreateSamplerState(
			&texture2DSamplerDesription,
			&this->TextureSample
		);

		GD_ASSERT((SUCCEEDED(result) && (this->TextureSample != nullptr)),
			"Failed to create sample state");
	}

	HRITexture2D* HRD3D11Interface::CreateTexture2D(HRITexture2DCtorInfo const& CtorInfo, chandle const InitialData, size_t  const InitialDataLength)
	{
		return new HRID3D11Texture2D(CtorInfo, InitialData, InitialDataLength);
	}

GD_NAMESPACE_END
