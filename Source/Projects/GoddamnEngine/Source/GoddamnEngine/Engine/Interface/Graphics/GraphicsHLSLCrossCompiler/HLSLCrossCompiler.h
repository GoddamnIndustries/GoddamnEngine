// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Engine/Interface/Graphics/GraphicsHLSLCrossCompiler/HLSLCrossCompiler.h
 * File contains export interface for the engine shader cross compiler subsystem.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#if GD_PLATFORM_DESKTOP

#include <GoddamnEngine/Core/UniquePtr.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Engine/Interface/Graphics/Graphics.h>

GD_NAMESPACE_BEGIN

	class InputStream;
	class OutputStream;

	// ------------------------------------------------------------------------------------------
	//! Describes list of shader generation-compilation targets.
	GD_DENUM()
	enum DHLSLShaderCrossCompilerTarget : UInt8
	{
		DHLSL_CC_TARGET_GLSL430,
		DHLSL_CC_TARGET_GLSLES3,
		DHLSL_CC_TARGET_GLSLES2,
		DHLSL_CC_TARGET_HLSL50,
		DHLSL_CC_TARGET_HLSL51,
		DHLSL_CC_TARGET_UNKNOWN,
	};	// enum DHLSLShaderCrossCompilerTarget

#define GD_HRI_SHADER_MAX_CBUFFERS_LOCATIONS 14

	// ------------------------------------------------------------------------------------------
	//! Provides compilation of some subset of HLSL code into HLSL, PSSL and Metal Byte code and 
	//! translation into GLSL and GLSL(ES) source.
	//! 
	//! Note that this is not a full parser, it just parsers a constant buffer, textures declarations and
	//! shader entry point. Whole function bodies are not translated. Some HLSL functionality requires to 
	//! be wrapped with macros to make function bodies compile under non-HLSL languages.
	//! 
	//! Please, visit http://youtu.be/45O7WTc6k2Y for cross compiler implementation details.
	//! Implementation is not quite same but something very similar.
	//! 
	//! Some stuff from HLSL 5.0 that is currently not supported:
	//! * Template/Object-Oriented syntax.
	//! * Preprocessing outside functions.
	GD_DINTERFACE()
	uinterface DHLSLCrossCompiler : public IInterface
	{
		GD_DEFINE_CLASS_INFORMATION(DHLSLCrossCompiler, IInterface);

	public:
			
		// ------------------------------------------------------------------------------------------
		//! Compiles HLSL shader into some specific platform implementation Byte code.
		//! @param ShaderSourceInputStream input stream that contains shader source code.
		//! @param ShaderByteCodeOutputStream Output stream that would contain shader compiled Byte code. If specified as null then no code is generated/compiled.
		//! @param ShaderType Type of shader. Vertex, pixel, etc.
		//! @param ShaderTargetPlatform Target platform for which shader would be generated.
		//! @param ShaderEntryPointName Name of shader entry point.
		//! @returns Shader instance description, generated using provided data on success, or nullptr on fail.
		GDAPI virtual /*RefPtr<HRIShaderInstanceDesc>*/void CrossCompileShader(InputStream* ShaderSourceInputStream 
			, UniquePtr<OutputStream> const& ShaderByteCodeOutputStream, IGraphicsShaderType const ShaderType, DHLSLShaderCrossCompilerTarget const  ShaderTargetPlatform
			, String const& ShaderEntryPointName);

	};	// uinterface DHLSLCrossCompiler

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_DESKTOP
