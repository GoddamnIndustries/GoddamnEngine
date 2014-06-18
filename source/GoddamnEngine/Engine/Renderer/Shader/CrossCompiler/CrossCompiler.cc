//////////////////////////////////////////////////////////////////////////
/// CrossCompiler.cc - implementation for ICCSL compiler
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 27.03.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/CrossCompiler.hh>
#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/Parser/Parser.hh>
#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/Compiler/Compiler.hh>
#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/Validator/Validator.hh>

#include <GoddamnEngine/Core/IO/Stream/FileStream/FileStream.hh>

GD_NAMESPACE_BEGIN

	////////////////////////////////////////////////////////////////////////////
	/// HRIShaderCrossCompiler class
	////////////////////////////////////////////////////////////////////////////

	bool HRIShaderCrossCompiler::ParseShader(			
		Stream*                const  ShaderSourceInputStream, 
		HRIShaderInstanceDesc      *& OutputShaderDescription,
		String                 const& EntryPointName
	)
	{
		HLSLScope const* const Scope = HLSLParser(self->Toolchain).ParseShader(ShaderSourceInputStream);
		if (Scope != nullptr)
		{
			OutputShaderDescription = HLSLValidator(self->Toolchain).ValidateAndGenerateDescription(nullptr, Scope, EntryPointName);
			
			FileOutputStream ShaderOutput("shaderoutput.glsl.c");
			HRIShaderCrossCompilerCompiler(self->Toolchain).GenerateAndCompileShader(&ShaderOutput, Scope, GD_HRI_SHADERCC_COMPILER_TARGET_GLSL430, GD_HRI_SHADER_TYPE_VERTEX, EntryPointName);
			ShaderOutput.Dispose();
			return true;
		}
		return false;
	}

	bool HRIShaderCrossCompiler::TranslateShader(
		Stream*                       const  ShaderSourceInputStream,
		Stream*                       const  ShaderTranslatedOutputStream,
		HRIShaderInstanceDesc*        const  OutputShaderDescription,
		HRIShaderCrossCompilerProfile const  ShaderTargetProfile,
		String                        const& EntryPointName
	)
	{
		GD_UNUSED(ShaderSourceInputStream);
		GD_UNUSED(ShaderTranslatedOutputStream);
		GD_UNUSED(OutputShaderDescription);
		GD_UNUSED(ShaderTargetProfile);
		GD_UNUSED(EntryPointName);
		return false;
	}

GD_NAMESPACE_END
