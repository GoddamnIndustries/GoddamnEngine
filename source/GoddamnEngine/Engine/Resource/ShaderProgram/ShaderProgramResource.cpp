#include <GoddamnEngine/Engine/Resource/ShaderProgram/ShaderProgramResource.h>
#include <GoddamnEngine/Engine/Resource/ResourceStreamer/ResourceStreamer.h>
#include <GoddamnEngine/Engine/Renderer/Renderer.h>
#include <GoddamnEngine/Engine/Renderer/Shader/Shader.h>
#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/CrossCompiler.h>

#include <GoddamnEngine/Core/IO/Stream/Stream.h>
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

		mxml_node_t* const shaderProgramDescNode
			= mxmlFindElement(shaderProgramDescXml, shaderProgramDescXml,
				"ShaderProgramDesc", nullptr, nullptr, MXML_DESCEND_FIRST);
		GD_ASSERT((shaderProgramDescNode != nullptr), "Failed locate root node ('ShaderProgramDesc')");

		self->Program = HRInterface::GetInstance().CreateShaderProgram();
		GD_MXML_FOREACH_CHILD(shaderDescNode, shaderProgramDescNode)
		{
			BinaryResource* const ShaderResource = ResourceStreamer::GetInstance().LoadImmediately<BinaryResource>(mxmlElementGetAttr(shaderDescNode, "Id"));
			DataRefInputStream    ShaderInputStream(ShaderResource->BinaryData);

			IToolchain Toolchain;
			HRIShaderInstanceDesc* ShaderInstanceDesc = nullptr;
			HRIShaderCrossCompiler(&Toolchain).ParseShader(&ShaderInputStream, ShaderInstanceDesc, "Main");
			HRIShaderCtorInfo const CtorInfo(
				self->Program,
				&ShaderResource->BinaryData[0],
				ShaderResource->BinaryData.GetSize(),
				ShaderInstanceDesc
			);

			HRInterface::GetInstance().CreateVertexShader(CtorInfo);
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
