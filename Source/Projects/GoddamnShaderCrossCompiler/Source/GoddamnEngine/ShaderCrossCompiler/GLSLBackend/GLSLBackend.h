// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/ShaderCrossCompiler/GLSLBackend/GLSLBackend.h
 * File contains GLSL backend interface for the shader cross compiler.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#if GD_PLATFORM_WINDOWS

#include <GoddamnEngine/ShaderCrossCompiler/HLSLCrossCompiler.h>

GD_NAMESPACE_BEGIN

	struct HLSLScope;

	// ------------------------------------------------------------------------------------------
	//! Defines a GLSL compiler backend.
	//! @sa DHLSL_CC_TARGET_GLSL430, DHLSL_CC_TARGET_GLSLES30, DHLSL_CC_TARGET_GLSLES20.
	class GLSLCompiler final
	{
	public:

		// ------------------------------------------------------------------------------------------
		//! Generates the HLSL shader Byte code.
		//! @param ShaderByteCodeOutputStream Output for the shader code.
		//! @param ShaderParsedData Parsed shader data, produced by @ref HLSLParser
		//! @param ShaderType Type of the shader to generate code.
		//! @param ShaderEntryName Name of the shader entry point.
		//! @param ShaderTargetPlatform Target platform for the GLSL code.
		GDAPI void GenerateAndCompileShader(UniquePtr<OutputStream> const& ShaderByteCodeOutputStream
			, HLSLScope const* const ShaderParsedData, IGraphicsShaderType const ShaderType, String const& ShaderEntryName
			, DHLSLShaderCrossCompilerTarget const ShaderTargetPlatform);

	};	// class GLSLCompiler

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_WINDOWS
