#include <GoddamnEngine/Engine/Resource/ShaderProgram/ShaderProgramResource.hh>
#include <GoddamnEngine/Engine/Resource/ResourceStreamer/ResourceStreamer.hh>
#include <GoddamnEngine/Engine/Renderer/Renderer.hh>
#include <GoddamnEngine/Engine/Renderer/Shader/Shader.hh>
#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/CrossCompiler.hh>

#include <GoddamnEngine/Core/IO/Stream/StringStream/StringStream.hh>
#include <mxml.h>

GD_NAMESPACE_BEGIN

	GD_TYPEINFORMATION_IMPLEMENTATION(ShaderProgramResource, Resource, GDAPI, GD_RESOURCE_ARGS);
	
	//////////////////////////////////////////////////////////////////////////
	void ShaderProgramResource::OnResourceLoad(
		_In_ ResourceLoader* const resourceLoader
	)
	{
		String shaderProgramDescStr(nullptr, resourceLoader->GetResourceDataSize() / sizeof(Char));
		resourceLoader->Read(&shaderProgramDescStr[0], resourceLoader->GetResourceDataSize());

		mxml_node_t* const shaderProgramDescXml = mxmlLoadString(nullptr, &shaderProgramDescStr[0], MXML_TEXT_CALLBACK);
		GD_ASSERT((shaderProgramDescXml != nullptr), "Failed to parse XML");

		mxml_node_t* const shaderProgramDescNode = mxmlFindElement(shaderProgramDescXml, shaderProgramDescXml, "ShaderProgramDesc", nullptr, nullptr, MXML_DESCEND_FIRST);
		GD_ASSERT((shaderProgramDescNode != nullptr), "Failed locate root node ('ShaderProgramDesc')");

		self->Program = HRInterface::GetInstance().CreateShaderProgram();
		GD_MXML_FOREACH_CHILD(shaderDescNode, shaderProgramDescNode)
		{
			BinaryResource* const ShaderResource = ResourceStreamer::GetInstance().LoadImmediately<BinaryResource>(mxmlElementGetAttr(shaderDescNode, "Id"));
			StringInputStream    ShaderInputStream((char*) &ShaderResource->BinaryData[0]);

			IToolchain Toolchain;
			HRIShaderInstanceDesc* ShaderInstanceDesc = nullptr;
			HRIShaderCrossCompiler(&Toolchain).ParseShader(&ShaderInputStream, ShaderInstanceDesc, "Main");
			HRIShaderCtorInfo const CtorInfo(
				self->Program,
				&ShaderResource->BinaryData[0],
				ShaderResource->BinaryData.GetSize(),
				ShaderInstanceDesc
			);

			String const ShaderTypeStr(mxmlElementGetAttr(shaderDescNode, "Type"));
			/**/ if (ShaderTypeStr == "GD_HRI_SHADER_TYPE_COMPUTE")
				/*HRInterface::GetInstance().CreateComputeShader(CtorInfo)*/;
			else if (ShaderTypeStr == "GD_HRI_SHADER_TYPE_GEOMETRY")
				/*HRInterface::GetInstance().CreateGeometryShader(CtorInfo)*/;
			else if (ShaderTypeStr == "GD_HRI_SHADER_TYPE_VERTEX")
				HRInterface::GetInstance().CreateVertexShader(CtorInfo);
			else if (ShaderTypeStr == "GD_HRI_SHADER_TYPE_HULL")
				HRInterface::GetInstance().CreateHullShader(CtorInfo);
			else if (ShaderTypeStr == "GD_HRI_SHADER_TYPE_DOMAIN")
				HRInterface::GetInstance().CreateDomainShader(CtorInfo);
			else if (ShaderTypeStr == "GD_HRI_SHADER_TYPE_PIXEL")
				HRInterface::GetInstance().CreatePixelShader(CtorInfo);
#if (defined(GD_HRI_SHADER_OPENGL_ALISASING))
			else if (ShaderTypeStr == "GD_HRI_SHADER_TYPE_TESSELLATION_CONTROL")
				HRInterface::GetInstance().CreateHullShader(CtorInfo);
			else if (ShaderTypeStr == "GD_HRI_SHADER_TYPE_TESSELLATION_EVALUATION")
				HRInterface::GetInstance().CreateDomainShader(CtorInfo);
			else if (ShaderTypeStr == "GD_HRI_SHADER_TYPE_FRAGMENT")
				HRInterface::GetInstance().CreatePixelShader(CtorInfo);
#endif	// if (defined(GD_HRI_SHADER_OPENGL_ALISASING))
			else GD_ASSERT_FALSE("Invalid shader type.");
		}

		mxmlDelete(shaderProgramDescXml);
	}

	//////////////////////////////////////////////////////////////////////////
	void ShaderProgramResource::OnResourceUnload(
	)
	{
		SafeRelease(self->Program);
	}

GD_NAMESPACE_END
