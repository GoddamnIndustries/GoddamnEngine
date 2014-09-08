#include <GoddamnEngine/Engine/Renderer/Renderer.h>

GD_NAMESPACE_BEGIN
	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIBuffer,         HRIObject, GDAPI, nullptr);
	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIVertexBuffer,   HRIBuffer, GDAPI, nullptr);
	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIIndexBuffer,    HRIBuffer, GDAPI, nullptr);
	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIConstantBuffer, HRIBuffer, GDAPI, nullptr);
	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIRenderTarget,   HRIObject, GDAPI, nullptr);
	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRILinkagePoint,   HRIObject, GDAPI, nullptr);
	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIIndexedShape,   HRIObject, GDAPI, nullptr);
	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRITexture2D,      HRIObject, GDAPI, nullptr);
	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRInterface,       HRIObject, GDAPI, nullptr);
	GD_SINGLETON_IMPLEMENTATION(HRInterface);

	/// ==========================================================================================
	/// Semantics
	/// ==========================================================================================

	extern HRISemanticDesc const& HRISemanticGetDesc(HRISemantic const Semantic)
	{
		static HRISemanticDesc SemanticDescs[] = {
			{ "POSITION",      GD_FORMAT_R32G32B32_FLOAT },
			{ "NORMAL",        GD_FORMAT_R32G32B32_FLOAT },
			{ "TANGENT",       GD_FORMAT_R32G32B32_FLOAT },
			{ "BINORMAL",      GD_FORMAT_R32G32B32_FLOAT },
			{ "TEXCOORD",      GD_FORMAT_R32G32_FLOAT }, 
			{ "BLEND_INDICES", GD_FORMAT_R32G32B32A32_FLOAT },
			{ "BLEND_WEIGHT",  GD_FORMAT_R32G32B32A32_FLOAT }, 
			{ "COLOR",         GD_FORMAT_R32G32B32A32_FLOAT },
		};

		GD_DEBUG_ASSERT(Semantic < GD_HRI_SEMANTICS_COUNT, "Invalid semantic specified.");
		return SemanticDescs[static_cast<size_t>(Semantic)];
	}

	void HRIIndexedShape::SetVertexData(HRISemantic const Semantic, Float32 const* const Data, size_t const Size)
	{
		if (Data == nullptr)
		{
			if (Semantic == GD_HRI_SEMANTIC_UNKNOWN)
			{	// Just clearing out all vertex data.
				for (size_t SemanticIter = 0; SemanticIter < GD_HRI_SEMANTIC_UNKNOWN; SemanticIter += 1)
					this->SetVertexData(static_cast<HRISemantic>(SemanticIter), nullptr, 0);
				return;
			}
			else
				this->VertexBuffers[static_cast<size_t>(Semantic)] = nullptr;
		}
		else
			this->VertexBuffers[static_cast<size_t>(Semantic)] = HRInterface::GetInstance().CreateVertexBuffer(Data, Size);
	}

	void HRIIndexedShape::SetIndexData(chandle const Data, size_t const Size, size_t const Stride)
	{
		this->IndexBuffer = ((Data != nullptr)
			? HRInterface::GetInstance().CreateIndexBuffer(Data, Size, Stride)
			: nullptr
		);
	}

	HRIRenderTargetCtorInfo::HRIRenderTargetCtorInfo(Resolution const& RenderTargetResolution, HRITexture2D const* const FirstTexture /* = nullptr */, ...) 
		: RenderTargetResolution(RenderTargetResolution)
	{
		va_list TexturesList = va_list();
		va_start(TexturesList, FirstTexture);

		for (HRITexture2D const* Texture2D = FirstTexture; Texture2D != nullptr; Texture2D = va_arg(TexturesList, HRITexture2D const*))
		{
			GD_DEBUG_ASSERT((Texture2D->Mode == GD_HRI_TEXTURE_2D_MODE_RenderTarget), "Invalid mode of specified texture");
			GD_DEBUG_ASSERT((Texture2D->TheResolution == this->RenderTargetResolution), "Invalid resolution of specified texture");

			this->RenderTargetTextures.PushLast(Texture2D);
			Texture2D->AddReference();
		}

		va_end(TexturesList);
	}

	HRIRenderTargetCtorInfo::HRIRenderTargetCtorInfo(HRIRenderTargetCtorInfo const& CtorInfo) 
		: RenderTargetTextures(CtorInfo.RenderTargetTextures)
	{
		for (size_t cnt = 0; cnt < this->RenderTargetTextures.GetSize(); cnt += 1)
			this->RenderTargetTextures[cnt]->AddReference();
	}

	HRIRenderTargetCtorInfo::~HRIRenderTargetCtorInfo()
	{
		for (size_t cnt = 0; cnt < this->RenderTargetTextures.GetSize(); cnt += 1)
			this->RenderTargetTextures[cnt]->RemoveReference();
	}

GD_NAMESPACE_END
