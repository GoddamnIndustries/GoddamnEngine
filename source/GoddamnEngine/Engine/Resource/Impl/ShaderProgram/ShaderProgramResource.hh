#ifndef GD_RESOURCE_SHADER_PROGRAM_SHADER_PROGRAM_RESOURCE
#define GD_RESOURCE_SHADER_PROGRAM_SHADER_PROGRAM_RESOURCE

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Engine/Resource/Resource.hh>
#include <GoddamnEngine/Engine/Renderer/Shader/Shader.hh>

GD_NAMESPACE_BEGIN
	
	/// ==========================================================================================
	/// ==========================================================================================
	/// Class handles shader programs uploading to system
	class ShaderProgramResource final : public Resource
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(ShaderProgramResource, Resource, GDAPI);

		HRIShaderProgram* Program = nullptr;

		/// ==========================================================================================
		GDINL ShaderProgramResource(
			_In_ String const& identifier
		) : Resource(identifier) { }

		/// ==========================================================================================
		GDINL virtual ~ShaderProgramResource(
		) { }

	public:
		/// ==========================================================================================
		/// Returns shader program that was loaded from resource
		HRIShaderProgram* GetProgram(
		) const { return self->Program; }

	protected:
		/// ==========================================================================================
		GDAPI virtual void OnResourceLoad(InputStream* const InputResourceData);

		/// ==========================================================================================
		GDAPI virtual void OnResourceUnload();
	};

GD_NAMESPACE_END

#endif