//////////////////////////////////////////////////////////////////////////
/// Description.h: shader instance description generator interface.
/// Copyright (C) $(GD_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 17.05.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_ENGINE_RENDERER_SHADER_CROSS_COMPILER_DESCRIPTION
#define GD_ENGINE_RENDERER_SHADER_CROSS_COMPILER_DESCRIPTION

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/Compilers/Toolchain/Toolchain.hh>
#include <GoddamnEngine/Engine/Renderer/Shader/Shader.hh>

#if (!defined(GD_PLATFORM_DESKTOP))
#	error "This API can be used on desktop platfroms only."
#endif

GD_NAMESPACE_BEGIN

	struct HLSLScope;
	struct HLSLVariable;

	/// Provides generation of shader instance description using data generated by HLSLParser.
	class HRIShaderInstanceDescGenerator final : public IToolchainTool
	{
	public:
		GDINL  HRIShaderInstanceDescGenerator(IToolchain* const Toolchain) : IToolchainTool(Toolchain) { }
		GDINL ~HRIShaderInstanceDescGenerator(                           ) { }

		/// Generates shader instance description using data generated by HLSLParser.
		GDINT HRIShaderInstanceDesc* GenerateDescription(
			HRIShader      * const Shader, 
			HLSLScope const* const ParsedData,
			String    const&       EntryName
		);

	private:
		GDINT bool ProcessEntryPointArgument(HLSLVariable const* const Argument, UInt64& ShaderSemanticsList);
	};	// class HRIShaderInstanceDescGenerator

GD_NAMESPACE_END

#endif