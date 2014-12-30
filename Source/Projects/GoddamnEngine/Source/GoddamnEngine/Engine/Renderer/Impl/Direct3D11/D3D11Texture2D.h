#ifndef GD_ENGINE_RENDERER_IMPL_D3D11_HRI_TEXTURE_2D
#define GD_ENGINE_RENDERER_IMPL_D3D11_HRI_TEXTURE_2D

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Renderer/Impl/Direct3D11/D3D11Renderer.h>

GD_NAMESPACE_BEGIN

	class HRID3D11Texture2D final : public HRITexture2D
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRID3D11Texture2D, HRITexture2D, GDINT);
		D3D11RefPtr<ID3D11Texture2D> Texture;
		D3D11RefPtr<ID3D11SamplerState> TextureSample;
		D3D11RefPtr<ID3D11ShaderResourceView> TextureShaderResource;

		GDINT void CreateTexture     (chandle const initialData, ID3D11Device* const device);
		GDINT void CreateShaderResourceView(         ID3D11Device* const device);
		GDINT void CreateTextureSample  (         ID3D11Device* const device);

	public:
		GDINT    HRID3D11Texture2D(HRITexture2DCtorInfo const& CtorInfo, chandle const InitialData, size_t const InitialiDataLength);
		GDINL virtual ~HRID3D11Texture2D() { }
		GDINL virtual handle GetNativePointer() const { return this->Texture.Get(); }
		GDINL ID3D11Texture2D*    GetTexture    () const { return this->Texture.Get(); }
		GDINL ID3D11SamplerState*    GetSample  () const { return this->TextureSample.Get(); }
		GDINL ID3D11ShaderResourceView* GetShaderResource() const { return this->TextureShaderResource.Get(); }
	};	// class HRID3D11Texture2D

GD_NAMESPACE_END

#endif
