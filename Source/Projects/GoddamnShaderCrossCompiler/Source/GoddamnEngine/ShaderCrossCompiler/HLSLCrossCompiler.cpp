// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#include <GoddamnEngine/Include.h>
#if GD_PLATFORM_DESKTOP

#include <GoddamnEngine/ShaderCrossCompiler/HLSLCrossCompiler.h>
#include <GoddamnEngine/ShaderCrossCompiler/HLSLTokenizer.h>
#include <GoddamnEngine/ShaderCrossCompiler/HLSLParser.h>
#include <GoddamnEngine/ShaderCrossCompiler/HLSLValidator.h>

#include <GoddamnEngine/ShaderCrossCompiler/GLSLBackend/GLSLBackend.h>
#include <GoddamnEngine/ShaderCrossCompiler/HLSLBackend/HLSLBackend.h>
//#include <GoddamnEngine/ShaderCrossCompiler/HLSL-HLSLCompilerBackend/HLSL-MetalCompiler.h>

GD_NAMESPACE_BEGIN

	// ------------------------------------------------------------------------------------------
	//! Compiles HLSL shader into some specific platform implementation Byte code.
	//! @param ShaderSourceInputStream input stream that contains shader source code.
	//! @param ShaderByteCodeOutputStream Output stream that would contain shader compiled Byte code. If specified as null then no code is generated/compiled.
	//! @param ShaderType Type of shader. Vertex, pixel, etc.
	//! @param ShaderTargetPlatform Target platform for which shader would be generated.
	//! @param ShaderEntryPointName Name of shader entry point.
	//! @returns Shader instance description, generated using provided data on success, or nullptr on fail.
	GDAPI /*RefPtr<HRIShaderInstanceDesc>*/void DHLSLCrossCompiler::CrossCompileShader(InputStream* ShaderSourceInputStream
		, UniquePtr<OutputStream> const& ShaderByteCodeOutputStream, IGraphicsShaderType const  ShaderType, DHLSLShaderCrossCompilerTarget const  ShaderTargetPlatform
		, String const& ShaderEntryPointName)
	{	
		try 
		{
			// Parsing shader source.
			UniquePtr<HLSLScope const> const ShaderParsedData(HLSLParser().ParseShader(ShaderSourceInputStream));
			/*RefPtr<HRIShaderInstanceDesc> const ShaderInstanceDesc*/(HLSLValidator().ValidateAndGenerateDescription(ShaderParsedData.GetPointer(), ShaderEntryPointName));

			if (ShaderByteCodeOutputStream != nullptr) 
			{	
				// If required, compiling shader.
				switch (ShaderTargetPlatform) 
				{
#if GD_PLATFORM_WINDOWS
					case DHLSL_CC_TARGET_HLSL50:
						HLSLCompilerBackend().GenerateAndCompileShader(ShaderByteCodeOutputStream, ShaderParsedData.GetPointer(), ShaderType, ShaderEntryPointName, ShaderTargetPlatform);
						break;
#endif	// if GD_PLATFORM_WINDOWS

#if GD_PLATFORM_OS_X || GD_PLATFORM_IOS
					case DHLSL_CC_TARGET_METAL:
						MetalCompiler(Toolchain).GenerateAndCompileShader(ShaderByteCodeOutputStream, ShaderParsedData.GetPointer(), ShaderType, ShaderEntryPointName);
						break;
#endif	// if GD_PLATFORM_OS_X || GD_PLATFORM_IOS

					case DHLSL_CC_TARGET_GLSL430:
					case DHLSL_CC_TARGET_GLSLES2:
					case DHLSL_CC_TARGET_GLSLES3:
						GLSLCompiler().GenerateAndCompileShader(ShaderByteCodeOutputStream, ShaderParsedData.GetPointer(), ShaderType, ShaderEntryPointName, ShaderTargetPlatform);
						break;

					default:
						GD_ASSERT_FALSE("Compiling for this target is unsupported on this platform.");
				}
			}
			return /*ShaderInstanceDesc*/;
		} 
		catch (ToolchainException const& Exception) 
		{	
			// Some error occurred while parsing/compiling shader.
			throw Exception;
		}
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_DESKTOP
