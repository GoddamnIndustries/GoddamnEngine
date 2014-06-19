//////////////////////////////////////////////////////////////////////////
/// CrossCompiler.cc - implementation for ICCSL compiler
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 27.03.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/CrossCompiler.hh>
#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/Parser/Parser.hh>
#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/Validator/Validator.hh>
#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/Impl/GLSL/GLSLCompiler.hh>
#if (defined(GD_PLATFORM_WINDOWS))
#	include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/Impl/HLSL/HLSLCompiler.hh>
#endif	// if (defined(GD_PLATFORM_WINDOWS))
#if (defined(GD_PLATFORM_OSX))
#	include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/Impl/Metal/MetalCompiler.hh>
#endif	// if (defined(GD_PLATFORM_OSX))

GD_NAMESPACE_BEGIN

	HRIShaderInstanceDesc* HRIShaderCrossCompiler::CrossCompileShader(
		InputStream                * const  ShaderSourceInputStream, 
		OutputStream               * const  ShaderByteCodeOutputStream,
		HRIShaderType                const  ShaderType,
		HRIShaderCrossCompilerTarget const  ShaderTargetPlatform,
		String                       const& ShaderEntryPointName
	)
	{	// Parsing shader source.
		GD_DEBUG_ASSERT(ShaderSourceInputStream != nullptr, "Nullptr input stream specified");
		UniquePtr<HLSLScope const> const ParsedData = HLSLParser(self->Toolchain).ParseShader(ShaderSourceInputStream);
		if (ParsedData == nullptr)
			return nullptr;

		// Validating shader source.
		RefPtr<HRIShaderInstanceDesc> ShaderInstanceDesc = HLSLValidator(self->Toolchain).ValidateAndGenerateDescription(ParsedData.GetPointer(), ShaderEntryPointName);
		if (ShaderInstanceDesc == nullptr)
			return nullptr;

		if (ShaderByteCodeOutputStream != nullptr)
		{	// If required, compiling shader.
			switch (ShaderTargetPlatform)
			{
			case GD_HRI_SHADERCC_COMPILER_TARGET_GLSL430:
			case GD_HRI_SHADERCC_COMPILER_TARGET_GLSLES2:
			case GD_HRI_SHADERCC_COMPILER_TARGET_GLSLES3:
				if (!GLSLCompiler(self->Toolchain).GenerateAndCompileShader(ShaderByteCodeOutputStream, ParsedData.GetPointer(), ShaderTargetPlatform, ShaderType, ShaderEntryPointName))
					return nullptr;
				break;

#if (defined(GD_PLATFORM_WINDOWS))
			case GD_HRI_SHADERCC_COMPILER_TARGET_HLSL:
				if (!HLSLCompiler(self->Toolchain).GenerateAndCompileShader(ShaderByteCodeOutputStream, ParsedData.GetPointer(), ShaderType, ShaderEntryPointName))
					return nullptr;
				break;
#endif	// if (defined(GD_PLATFORM_WINDOWS))

#if (defined(GD_PLATFORM_OSX))
			case GD_HRI_SHADERCC_COMPILER_TARGET_METAL:
				if (!MetalCompiler(self->Toolchain).GenerateAndCompileShader(ShaderByteCodeOutputStream, ParsedData.GetPointer(), ShaderType, ShaderEntryPointName))
					return nullptr;
				break;
#endif	// if (defined(GD_PLATFORM_OSX))

			default:
				GD_DEBUG_ASSERT(ShaderTargetPlatform < GD_HRI_SHADERCC_COMPILER_TARGETS_COUNT, "Invalid target specified.");
				GD_DEBUG_ASSERT_FALSE("Compiling for this target is uncupported on this platform.");
				return nullptr;
			}
		}

		return ShaderInstanceDesc.Release();
	}

GD_NAMESPACE_END
