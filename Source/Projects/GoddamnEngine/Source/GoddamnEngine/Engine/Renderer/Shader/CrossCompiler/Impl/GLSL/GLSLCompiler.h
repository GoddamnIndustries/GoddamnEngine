/// ==========================================================================================
/// GLSLCompiler.h: GLSL compiler generator and compiler interface.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// 
/// History:
///		* 13.06.2014 - Created by James Jhuighuy.
/// ==========================================================================================

#pragma once
#ifndef GD_ENGINE_RENDERER_SHADER_CROSS_COMPILER_COMPILER_GLSLCOMPILER
#define GD_ENGINE_RENDERER_SHADER_CROSS_COMPILER_COMPILER_GLSLCOMPILER

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/IO/Stream/Stream.h>
#include <GoddamnEngine/Core/Compilers/Toolchain/Toolchain.h>
#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/CrossCompiler.h>

#if (!defined(GD_PLATFORM_DESKTOP))
#	error "This API can be used on desktop platfroms only."
#endif	// if (!defined(GD_PLATFORM_DESKTOP))

GD_NAMESPACE_BEGIN

	struct HLSLScope;
	class GLSLCompiler final : public IToolchainTool
	{
	public:
		GDINL  GLSLCompiler(IToolchain* const Toolchain) : IToolchainTool(Toolchain) { }
		GDINL ~GLSLCompiler(         ) { }
		GDAPI void GenerateAndCompileShader(
			UniquePtr<OutputStream>   const&    ShaderByteCodeOutputStream,
			HLSLScope     const* const ShaderParsedData,
			HRIShaderType    const  ShaderType,
			String        const&    ShaderEntryName,
			HRIShaderCrossCompilerTarget const  ShaderTargetPlatform
		);
	};	// class GLSLCompiler

GD_NAMESPACE_END

#endif
