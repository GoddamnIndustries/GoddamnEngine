#ifndef GD_ENGINE_RESOURCE_MATERIAL
#define GD_ENGINE_RESOURCE_MATERIAL

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Renderer/Shader/Shader.h>
#include <GoddamnEngine/Engine/Resource/Resource.h>

GD_NAMESPACE_BEGIN

	class HRIShaderParam;
	class HRIShaderProgram;
	class HRIShaderInstance;

	/// ==========================================================================================
	/// ==========================================================================================
	/// Interacts shader uniform variables, buffers and
	///	Textures
	class Material : public Resource
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(Material, Resource, GDAPI);

		RefPtr<HRIShaderProgram> MaterialProgram;
		RefPtr<HRIShaderInstance> MaterialInstance;

		/// ==========================================================================================
		GDINL Material(
			String const& identifier
		) : Resource(identifier) { }

		/// ==========================================================================================
		GDINL virtual ~Material(
		) { }

	public:
		/// ==========================================================================================
		/// ==========================================================================================
		/// @name HRIShaderProgram management
		/// @{

		/// ==========================================================================================
		/// Returns shader program that this material is using
		GDINL RefPtr<HRIShaderProgram> GetShaderProgram() const 
		{ 
			return this->MaterialProgram; 
		}

		/// ==========================================================================================
		/// New shader program material would use
		GDAPI void SetShaderProgram(RefPtr<HRIShaderProgram> const& Effect);
			
		/// @}
		/// ==========================================================================================
		/// ==========================================================================================

		/// ==========================================================================================
		/// ==========================================================================================
		/// @name Properties management
		/// @{

		/// ==========================================================================================
		/// Searches for property by name
		GDINL HRIShaderParam* GetProperty(
			String const& propertyName
		) const
		{
			if (this->MaterialInstance != nullptr)
			{
				return this->MaterialInstance->GetParamByName(propertyName);
			}

			return nullptr;
		}

		/// ==========================================================================================
		/// Copies all prosperities from other material
		/// @param swapShader		Use the shader from other material
		GDAPI void CopyPropertiesFrom(
			Material* const material, 
			bool const swapShader = true
		);

		/// @}
		/// ==========================================================================================
		/// ==========================================================================================

		/// ==========================================================================================
		/// ==========================================================================================
		/// @name Material binding
		/// @{

		/// ==========================================================================================
		/// Binds material
		GDAPI void BindMaterial() const;

		/// ==========================================================================================
		/// Unbinds material
		GDAPI void UnbindMaterial() const;

		/// @}
		/// ==========================================================================================
		/// ==========================================================================================

	protected:
		GDAPI virtual void OnResourceLoad(UniquePtr<InputStream> const& InputResourceData); 
		GDAPI virtual void OnResourceUnload();
	};

GD_NAMESPACE_END

#endif
