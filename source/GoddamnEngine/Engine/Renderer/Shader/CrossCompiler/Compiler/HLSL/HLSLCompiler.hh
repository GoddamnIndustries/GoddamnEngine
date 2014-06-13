//////////////////////////////////////////////////////////////////////////
/// HLSLCompiler.hh: HLSL compiler generator and compiler interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 13.06.2014 - Created by James Jhuighuy.
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_ENGINE_RENDERER_SHADER_CROSS_COMPILER_COMPILER_HLSLCOMPILER
#define GD_ENGINE_RENDERER_SHADER_CROSS_COMPILER_COMPILER_HLSLCOMPILER

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/IO/Stream/Stream.hh>
#include <GoddamnEngine/Core/Compilers/Toolchain/Toolchain.hh>
#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/Compiler/Compiler.hh>

#if (!defined(GD_PLATFORM_DESKTOP))
#	error "This API can be used on desktop platfroms only."
#endif	// if (!defined(GD_PLATFORM_DESKTOP))

GD_NAMESPACE_BEGIN

	struct HLSLScope;
	class HLSLCompiler final : public IToolchainTool
	{
	public:
		GDINL  HLSLCompiler(IToolchain* const Toolchain) : IToolchainTool(Toolchain) { }
		GDINL ~HLSLCompiler(                           ) { }
		GDINT bool GenerateAndCompileShader(
			OutputStream                      * const Output,
			HLSLScope                    const* const Input,
			HRIShaderType                const        Type,
			String                       const&       EntryName
		);
	};	// class HLSLCompiler

GD_NAMESPACE_END

#endif
