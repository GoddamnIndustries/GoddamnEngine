/// ==========================================================================================
/// CrossCompiler.cpp - implementation for ICCSL compiler
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 27.03.2014 - Created by James Jhuighuy
/// ==========================================================================================

#include <GoddamnEngine/Include.h>
#if (defined(GD_PLATFORM_DESKTOP))

#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/CrossCompiler.h>
#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/Parser/Parser.h>
#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/Validator/Validator.h>
#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/Impl/GLSL/GLSLCompiler.h>
#if (defined(GD_PLATFORM_WINDOWS))
#	include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/Impl/HLSL/HLSLCompiler.h>
#endif	// if (defined(GD_PLATFORM_WINDOWS))
#if (defined(GD_PLATFORM_OSX))
#	include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/Impl/Metal/MetalCompiler.h>
#endif	// if (defined(GD_PLATFORM_OSX))

GD_NAMESPACE_BEGIN

	RefPtr<HRIShaderInstanceDesc> HRIShaderCrossCompiler::CrossCompileShader(
		UniquePtr<InputStream >          && ShaderSourceInputStream, 
		UniquePtr<OutputStream>      const& ShaderByteCodeOutputStream,
		HRIShaderType                const  ShaderType,
		HRIShaderCrossCompilerTarget const  ShaderTargetPlatform,
		String                       const& ShaderEntryPointName
	)
	{	// Parsing shader source.
		GD_DEBUG_ASSERT(ShaderSourceInputStream != nullptr, "Nullptr input stream specified");
		try {
			UniquePtr<HLSLScope const>    const ShaderParsedData(HLSLParser(this->Toolchain).ParseShader(Forward<UniquePtr<InputStream>>(ShaderSourceInputStream)));
			RefPtr<HRIShaderInstanceDesc> const ShaderInstanceDesc(HLSLValidator(this->Toolchain).ValidateAndGenerateDescription(ShaderParsedData.GetPointer(), ShaderEntryPointName));

			if (ShaderByteCodeOutputStream != nullptr) {	// If required, compiling shader.
				switch (ShaderTargetPlatform) {
#if (defined(GD_PLATFORM_WINDOWS))
					case GD_HRI_SHADERCC_COMPILER_TARGET_HLSL:
						HLSLCompiler(this->Toolchain).GenerateAndCompileShader(ShaderByteCodeOutputStream, ShaderParsedData.GetPointer(), ShaderType, ShaderEntryPointName);
						break;
#endif	// if (defined(GD_PLATFORM_WINDOWS))

#if (defined(GD_PLATFORM_OSX))
					case GD_HRI_SHADERCC_COMPILER_TARGET_METAL:
						MetalCompiler(this->Toolchain).GenerateAndCompileShader(ShaderByteCodeOutputStream, ShaderParsedData.GetPointer(), ShaderType, ShaderEntryPointName);
						break;
#endif	// if (defined(GD_PLATFORM_OSX))

					case GD_HRI_SHADERCC_COMPILER_TARGET_GLSL430:
					case GD_HRI_SHADERCC_COMPILER_TARGET_GLSLES2:
					case GD_HRI_SHADERCC_COMPILER_TARGET_GLSLES3:
						GLSLCompiler(this->Toolchain).GenerateAndCompileShader(ShaderByteCodeOutputStream, ShaderParsedData.GetPointer(), ShaderType, ShaderEntryPointName, ShaderTargetPlatform);
						break;

					default:
						GD_DEBUG_ASSERT(ShaderTargetPlatform < GD_HRI_SHADERCC_COMPILER_TARGETS_COUNT, "Invalid target specified.");
						throw LogicException("Compiling for this target is unsupported on this platform.");
				}
			}

			return ShaderInstanceDesc;
		} catch (ToolchainException const& Exception) {	// Some error occured while parsing/compiling shader.
			throw Exception;
		}
	}

GD_NAMESPACE_END

#endif
