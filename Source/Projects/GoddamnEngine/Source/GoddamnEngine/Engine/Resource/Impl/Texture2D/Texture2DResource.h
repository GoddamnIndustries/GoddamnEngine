#ifndef GD_ENGINE_RESUORCE_HRI_TEXTURE2D_LOADER
#define GD_ENGINE_RESUORCE_HRI_TEXTURE2D_LOADER

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Resource/Resource.h>

GD_NAMESPACE_BEGIN

	class HRITexture2D;
	class Texture2DResource final : public Resource
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(Texture2DResource, Resource, GDAPI);
		HRITexture2D* Texture = nullptr;

		/// ==========================================================================================
		GDINL Texture2DResource(
			String const& identifier
		) : Resource(identifier) { }

		/// ==========================================================================================
		GDINL virtual ~Texture2DResource(
		) { }

	public:
		/// ==========================================================================================
		/// Returns loaded texture
		GDINL HRITexture2D* GetHRITexture2D(
		) const { return this->Texture; }

	private:

		/// ==========================================================================================
		GDINL virtual void OnResourceLoad(
			UniquePtr<InputStream> const& InputResourceData
		);

		/// ==========================================================================================
		GDINL virtual void OnResourceUnload(
		);
	};

GD_NAMESPACE_END

#endif