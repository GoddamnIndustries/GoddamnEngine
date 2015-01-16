#include <GoddamnEngine/Engine/Resource/Impl/Effect/Effect.h>
#include <GoddamnEngine/Engine/Renderer/Renderer.h>
#include <GoddamnEngine/Engine/Renderer/Shader/Shader.h>
#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/CrossCompiler.h>

#include <GoddamnEngine/Core/IO/Stream/StringStream/StringStream.h>
#include <mxml.h>

GD_NAMESPACE_BEGIN

	GD_TYPEINFORMATION_IMPLEMENTATION(RSEffect, Resource, GDAPI, GD_RESOURCE_ARGS);
	
	void RSEffect::OnResourceLoad(UniquePtr<InputStream> const& InputResourceData)
	{
#if 0
		String shaderProgramDescStr(nullptr, InputResourceData->GetLength() / sizeof(Char));
		InputResourceData->Read(&shaderProgramDescStr[0], 1, InputResourceData->GetLength());

		mxml_node_t* const shaderProgramDescXml = mxmlLoadString(nullptr, &shaderProgramDescStr[0], MXML_TEXT_CALLBACK);
		GD_ASSERT((shaderProgramDescXml != nullptr), "Failed to parse XML");

		mxml_node_t* const shaderProgramDescNode = mxmlFindElement(shaderProgramDescXml, shaderProgramDescXml, "ShaderProgramDesc", nullptr, nullptr, MXML_DESCEND_FIRST);
		GD_ASSERT((shaderProgramDescNode != nullptr), "Failed locate root node ('ShaderProgramDesc')");

		this->Program = HRInterface::GetInstance().CreateShaderProgram();
		GD_MXML_FOREACH_CHILD(shaderDescNode, shaderProgramDescNode)
		{
			StringBuilder ShaderOutput;
			RefPtr<RSBinary> const ShaderResource = RSStreamer::GetInstance().LoadImmediately<RSBinary>(mxmlElementGetAttr(shaderDescNode, "Id"));
			UniquePtr<InputStream> ShaderInputStream(new StringInputStream((char*)&ShaderResource->BinaryData[0]));
			UniquePtr<OutputStream> ShaderOutputStream(new StringOutputStream(ShaderOutput));

			IToolchain Toolchain;
			RefPtr<HRIShaderInstanceDesc> ShaderInstanceDesc(HRIShaderCrossCompiler(&Toolchain).CrossCompileShader(
				Move(ShaderInputStream), ShaderOutputStream, GD_HRI_SHADER_TYPE_VERTEX, GD_HRI_SHADERCC_COMPILER_TARGET_HLSL, "Main"));
			HRIShaderCtorInfo const CtorInfo(
				this->Program,
				ShaderOutput.GetPointer(),
				ShaderOutput.GetLength(),
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
			else GD_DEBUG_ASSERT_FALSE("Invalid shader type.");
		}

		mxmlDelete(shaderProgramDescXml);
#endif	// if 0
		this->EffectShaderProgram = HRInterface::GetInstance().CreateShaderProgram();
		
		String ShaderPathes(InputResourceData->GetLength());
		InputResourceData->Read(ShaderPathes.CStr(), 1, InputResourceData->GetLength());

		size_t const CommaIndex = ShaderPathes.Find(',');
		IToolchain Toolchain;
		{	// Creating vertex shader.
			String const VertexShaderPath = ShaderPathes.Substring(0, CommaIndex);
			StringBuilder VertexShaderOutput;
			Vector<UInt8> const VertexShaderData = RSStreamer::GetInstance().LoadImmediately<RSBinary>(VertexShaderPath)->BinaryData;
			UniquePtr<InputStream> VertexShaderInputStream(new StringInputStream(reinterpret_cast<CharAnsi const*>(&VertexShaderData[0]), VertexShaderData.GetLength()));
			UniquePtr<OutputStream> VertexShaderOutputStream(new StringOutputStream(VertexShaderOutput));
			RefPtr<HRIShaderInstanceDesc> VertexShaderInstanceDesc(HRIShaderCrossCompiler(&Toolchain).CrossCompileShader(Move(VertexShaderInputStream), VertexShaderOutputStream, GD_HRI_SHADER_TYPE_VERTEX, GD_HRI_SHADERCC_COMPILER_TARGET_HLSL /*GD_HRI_SHADERCC_COMPILER_TARGET_GLSL430*/, "Main"));
			HRIShaderCtorInfo const VertexShaderCtorInfo(this->EffectShaderProgram.GetPointer(), VertexShaderOutput.CStr(), VertexShaderOutput.GetLength(), VertexShaderInstanceDesc.GetPointer());
			HRInterface::GetInstance().CreateVertexShader(VertexShaderCtorInfo);
		}
		{	// Creating vertex shader.
			String const PixelShaderPath = ShaderPathes.Substring(CommaIndex + 1);
			StringBuilder PixelShaderOutput;
			Vector<UInt8> const PixelShaderData = RSStreamer::GetInstance().LoadImmediately<RSBinary>(PixelShaderPath)->BinaryData;
			UniquePtr<InputStream> PixelShaderInputStream(new StringInputStream(reinterpret_cast<CharAnsi const*>(&PixelShaderData[0]), PixelShaderData.GetLength()));
			UniquePtr<OutputStream> PixelShaderOutputStream(new StringOutputStream(PixelShaderOutput));
			RefPtr<HRIShaderInstanceDesc> PixelShaderInstanceDesc(HRIShaderCrossCompiler(&Toolchain).CrossCompileShader(Move(PixelShaderInputStream), PixelShaderOutputStream, GD_HRI_SHADER_TYPE_PIXEL, GD_HRI_SHADERCC_COMPILER_TARGET_HLSL /*GD_HRI_SHADERCC_COMPILER_TARGET_GLSL430*/, "Main"));
			HRIShaderCtorInfo const PixelShaderCtorInfo(this->EffectShaderProgram.GetPointer(), PixelShaderOutput.CStr(), PixelShaderOutput.GetLength(), PixelShaderInstanceDesc.GetPointer());
			HRInterface::GetInstance().CreatePixelShader(PixelShaderCtorInfo);
		}
	}

	void RSEffect::OnResourceUnload()
	{
	}

GD_NAMESPACE_END
