#include <GoddamnEngine/Engine/Resource/Impl/ShaderProgram/ShaderProgramResource.h>
#include <GoddamnEngine/Engine/Resource/ResourceStreamer/ResourceStreamer.h>
#include <GoddamnEngine/Engine/Renderer/Renderer.h>
#include <GoddamnEngine/Engine/Renderer/Shader/Shader.h>
#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/CrossCompiler.h>

#include <GoddamnEngine/Core/IO/Stream/StringStream/StringStream.h>
#include <mxml.h>

GD_NAMESPACE_BEGIN

	GD_TYPEINFORMATION_IMPLEMENTATION(ShaderProgramResource, Resource, GDAPI, GD_RESOURCE_ARGS);
	
	/// ==========================================================================================
	void ShaderProgramResource::OnResourceLoad(
		_In_ InputStream* const InputResourceData
	)
	{
		String shaderProgramDescStr(nullptr, InputResourceData->GetSize() / sizeof(Char));
		InputResourceData->Read(&shaderProgramDescStr[0], 1, InputResourceData->GetSize());

		mxml_node_t* const shaderProgramDescXml = mxmlLoadString(nullptr, &shaderProgramDescStr[0], MXML_TEXT_CALLBACK);
		GD_ASSERT((shaderProgramDescXml != nullptr), "Failed to parse XML");

		mxml_node_t* const shaderProgramDescNode = mxmlFindElement(shaderProgramDescXml, shaderProgramDescXml, "ShaderProgramDesc", nullptr, nullptr, MXML_DESCEND_FIRST);
		GD_ASSERT((shaderProgramDescNode != nullptr), "Failed locate root node ('ShaderProgramDesc')");

		self->Program = HRInterface::GetInstance().CreateShaderProgram();
		GD_MXML_FOREACH_CHILD(shaderDescNode, shaderProgramDescNode)
		{
			StringBuilder ShaderOutput;
			BinaryResource* const ShaderResource = ResourceStreamer::GetInstance().LoadImmediately<BinaryResource>(mxmlElementGetAttr(shaderDescNode, "Id"));
			UniquePtr<InputStream> ShaderInputStream(new StringInputStream((char*)&ShaderResource->BinaryData[0]));
			UniquePtr<OutputStream> ShaderOutputStream(new StringOutputStream(ShaderOutput));

			IToolchain Toolchain;
			RefPtr<HRIShaderInstanceDesc> ShaderInstanceDesc(HRIShaderCrossCompiler(&Toolchain).CrossCompileShader(
				Move(ShaderInputStream), ShaderOutputStream, GD_HRI_SHADER_TYPE_VERTEX, GD_HRI_SHADERCC_COMPILER_TARGET_HLSL, "Main"));
			HRIShaderCtorInfo const CtorInfo(
				self->Program,
				ShaderOutput.GetPointer(),
				ShaderOutput.GetSize(),
				ShaderInstanceDesc.GetPointer()
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

	/// ==========================================================================================
	void ShaderProgramResource::OnResourceUnload(
	)
	{
		SafeRelease(self->Program);
	}

GD_NAMESPACE_END
