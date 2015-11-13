// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Engine/Interface/Graphics/GraphicsHLSLCrossCompiler/HLSLBackend/HLSLBackend.h
 * File contains HLSL backend interface for the shader cross compiler.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#if GD_PLATFORM_WINDOWS

#include <GoddamnEngine/Engine/Interface/Graphics/GraphicsHLSLCrossCompiler/HLSLCrossCompiler.h>

GD_NAMESPACE_BEGIN

	struct HLSLScope;

	// ------------------------------------------------------------------------------------------
	//! Defines a HLSL compiler backend.
	//! @sa DHLSL_CC_TARGET_HLSL50, DHLSL_CC_TARGET_HLSL51.
	class HLSLCompilerBackend final
	{
	public:

		// ------------------------------------------------------------------------------------------
		//! Generates the HLSL shader Byte code.
		//! @param ShaderByteCodeOutputStream Output for the shader code.
		//! @param ShaderParsedData Parsed shader data, produced by @ref HLSLParser
		//! @param ShaderType Type of the shader to generate code.
		//! @param ShaderEntryName Name of the shader entry point.
		//! @param ShaderTargetPlatform Target platform for the GLSL code.
		GDINT void GenerateAndCompileShader(UniquePtr<OutputStream> const& ShaderByteCodeOutputStream
			, HLSLScope const* const ShaderParsedData, IGraphicsShaderType const ShaderType, String const& ShaderEntryName
			, DHLSLShaderCrossCompilerTarget const ShaderTargetPlatform);

	};	// class HLSLCompilerBackend

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_WINDOWS
