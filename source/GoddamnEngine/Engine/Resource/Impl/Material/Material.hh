#ifndef GD_ENGINE_RESOURCE_MATERIAL
#define GD_ENGINE_RESOURCE_MATERIAL

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Engine/Renderer/Shader/Shader.hh>
#include <GoddamnEngine/Engine/Resource/Resource.hh>

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

		HRIShaderProgram* MaterialProgram;
		HRIShaderInstance* MaterialInstance;

		/// ==========================================================================================
		GDINL Material(
			_In_ String const& identifier
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
		GDINL HRIShaderProgram* GetShaderProgram(
		) const 
		{ 
			return self->MaterialProgram; 
		}

		/// ==========================================================================================
		/// New shader program material would use
		GDAPI void SetShaderProgram(
			_In_ HRIShaderProgram* const shaderProgram
		);
			
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
			_In_ String const& propertyName
		) const
		{
			if (self->MaterialInstance != nullptr)
			{
				return self->MaterialInstance->GetParamByName(propertyName);
			}

			return nullptr;
		}

		/// ==========================================================================================
		/// Copies all prosperities from other material
		/// @param swapShader		Use the shader from other material
		GDAPI void CopyPropertiesFrom(
			_In_ Material* const material, 
			_In_ bool const swapShader = true
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
		GDAPI virtual void OnResourceLoad(InputStream* const InputResourceData); 
		GDAPI virtual void OnResourceUnload();
	};

GD_NAMESPACE_END

#endif
