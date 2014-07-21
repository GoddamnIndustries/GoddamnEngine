#include <GoddamnEngine/Engine/Resource/Impl/Material/Material.h>
#include <GoddamnEngine/Engine/Resource/Impl/ShaderProgram/ShaderProgramResource.h>
#include <GoddamnEngine/Engine/Resource/Impl/Texture2D/Texture2DResource.h>
#include <GoddamnEngine/Engine/Resource/ResourceStreamer/ResourceStreamer.h>

#include <GoddamnEngine/Engine/Renderer/Renderer.h>
#include <GoddamnEngine/Engine/Renderer/Shader/Shader.h>

#include <GoddamnEngine/Core/Format/FormatValue.h>
#include <mxml.h>

GD_NAMESPACE_BEGIN

	GD_TYPEINFORMATION_IMPLEMENTATION(Material, Resource, GDAPI, GD_RESOURCE_ARGS);

	/// ==========================================================================================
	void Material::SetShaderProgram(
		_In_ HRIShaderProgram* const shaderProgram
	) 
	{
		GD_ASSERT((shaderProgram != nullptr), "Invalid shader program specified");
		if (self->MaterialProgram != shaderProgram)
		{
			HRIPixelShader* const pixelShader = shaderProgram->GetProgramPixelShader();
			if (pixelShader != nullptr)
			{
				HRIShaderInstance* const shaderInstance = HRInterface::GetInstance().CreateShaderInstance(pixelShader->ShaderDesc);

				if (self->MaterialInstance != nullptr)
				{
					 /// @todo Add parameters movement from old instance to new one
					 self->MaterialInstance->RemoveReference();
				}

				SafeRelease(self->MaterialProgram);
				(self->MaterialInstance = shaderInstance)->AddReference();
				(self->MaterialProgram = shaderProgram)->AddReference();
			}
		}
	}

	/// ==========================================================================================
	void Material::CopyPropertiesFrom(
		_In_ Material* const material, 
		_In_ bool const swapShader /* = true */)
	{
		GD_NOT_IMPLEMENTED();
	}

	/// ==========================================================================================
	void Material::BindMaterial(
	) const
	{
		HRIPixelShader* const pixelShader = self->MaterialProgram->GetProgramPixelShader();
		if (pixelShader != nullptr)
		{
			self->MaterialInstance->GetInstanceResourcesLocation()->UploadAllParameters();
			pixelShader->BindShader(self->MaterialInstance);
		}
	}

	/// ==========================================================================================
	void Material::UnbindMaterial() const
	{
		HRIPixelShader* const pixelShader = self->MaterialProgram->GetProgramPixelShader();
		if (pixelShader != nullptr)
		{
			pixelShader->UnbindShader();
		}
	}

	void Material::OnResourceLoad(InputStream* const InputResourceData)
	{
		String materialStr(nullptr, InputResourceData->GetSize() / sizeof(Char));
		InputResourceData->Read(&materialStr[0], 1, InputResourceData->GetSize());

		mxml_node_t* const materialXml = mxmlLoadString(nullptr, &materialStr[0], MXML_TEXT_CALLBACK);
		GD_ASSERT((materialXml != nullptr), "Failed to parse XML");
		mxml_node_t* const materialRootNode = mxmlGetFirstChild(materialXml);

		String const metrialShaderId = mxmlElementGetAttr(materialRootNode, "Shader");
		ShaderProgramResource const* const materialShaderProgramResource
			= ResourceStreamer::GetInstance().LoadImmediately<ShaderProgramResource>(metrialShaderId);
		if (materialShaderProgramResource != nullptr)
		{
			self->SetShaderProgram(materialShaderProgramResource->Program);
			materialShaderProgramResource->RemoveReference();
		}

		GD_MXML_FOREACH_CHILD (materialParamNode, materialRootNode)
		{
			String const materialParamName = mxmlElementGetAttr(materialParamNode, "Name");
			String const materialParamValue = mxmlElementGetAttr(materialParamNode, "Value");

			HRIShaderParam* const materialParam = self->MaterialInstance->GetParamByName(materialParamName);
			GD_ASSERT((materialParam != nullptr), "Param with specified name does not exists in shader");

			switch (materialParam->ParamDesc->ParamType)
			{
				case GD_HRI_SHADER_PARAM_DESC_TYPE_FORMATABLE:
				{
					Vector<UInt8> materialParamValueData(materialParam->ParamDesc->GetParamSize());
					FormatValueFromString(materialParamValue, materialParam->ParamDesc->ParamFormat, &materialParamValueData[0]);
					materialParam->SetValue(static_cast<chandle>(&materialParamValueData[0]));
				} break;
				case GD_HRI_SHADER_PARAM_DESC_TYPE_MATRIX4X4:
				case GD_HRI_SHADER_PARAM_DESC_TYPE_MATRIX3X3:
				{
					GD_NOT_IMPLEMENTED();
				} break;
				case GD_HRI_SHADER_PARAM_DESC_TYPE_STRING:
				{
					materialParam->SetValue(static_cast<chandle>(&materialParamValue[0]));
				} break;
				case GD_HRI_SHADER_PARAM_DESC_TYPE_TEXTURE2D:
				{
					Texture2DResource const* const textureResource = ResourceStreamer::GetInstance().LoadImmediately<Texture2DResource>(materialParamValue);
					if (textureResource != nullptr)
					{
						materialParam->SetValue<HRITexture2D const*>(textureResource->GetHRITexture2D());
					}
				} break;
				case GD_HRI_SHADER_PARAM_DESC_TYPE_TEXTURECUBE:
				{
					GD_NOT_IMPLEMENTED();
				} break;
				default: GD_ASSERT_FALSE("Invalid paramter type");
			}
		}

		mxmlDelete(materialXml);
	}

	/// ==========================================================================================
	void Material::OnResourceUnload()
	{
		SafeRelease(self->MaterialInstance);
		SafeRelease(self->MaterialProgram);
	}

GD_NAMESPACE_END
