/// ==========================================================================================
/// CrossCompiler.hh - common interface for HLSL cross compiler.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 27.03.2014 - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_ENGINE_RENDERER_SHADER_CROSS_COMPILER
#define GD_ENGINE_RENDERER_SHADER_CROSS_COMPILER

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/Object/Singleton/Singleton.hh>
#include <GoddamnEngine/Core/Compilers/Toolchain/Toolchain.hh>
#include <GoddamnEngine/Engine/Renderer/Shader/Shader.hh>

#if (!defined(GD_PLATFORM_DESKTOP))
#	error "This API can be used on desktop platfroms only."
#endif	// if (!defined(GD_PLATFORM_DESKTOP))

GD_NAMESPACE_BEGIN

	class InputStream;
	class OutputStream;

	/// Describes list of shader generation-compilation targets.
	enum HRIShaderCrossCompilerTarget : UInt8
	{
		GD_HRI_SHADERCC_COMPILER_TARGET_HLSL,		///< HLSL Shading language (Microsoft-specific plaftorms).
		GD_HRI_SHADERCC_COMPILER_TARGET_PSSL,		///< PSSL Shading language (PlayStation4).
		GD_HRI_SHADERCC_COMPILER_TARGET_GLSL430,	///< GLSL Shading language, version 4.3 (OpenGL desktop).
		GD_HRI_SHADERCC_COMPILER_TARGET_GLSLES3,	///< GLSL(ES) 3.0 Shading language (OpenGL embedded, with constant buffers).
		GD_HRI_SHADERCC_COMPILER_TARGET_GLSLES2,	///< GLSL(ES) 2.0 Shading language (OpenGL embedded, without constant buffers).
		GD_HRI_SHADERCC_COMPILER_TARGET_METAL,		///< Metal Shading language (Apple plaforms).
		GD_HRI_SHADERCC_COMPILER_TARGET_UNKNOWN,    ///< Unknown shading language (internal usage only). 
		GD_HRI_SHADERCC_COMPILER_TARGETS_COUNT      = GD_HRI_SHADERCC_COMPILER_TARGET_UNKNOWN,
	};	// enum HRIShaderCrossCompilerTarget

	/// Provides compilation of some subset of HLSL code into HLSL, PSSL and Metal bytecode and 
	/// translation into GLSL and GLSL(ES) source.
	/// 
	/// Note that this is not a full parser, it just parsers a constant buffer, textures declarations and
	/// shader entry point. Whole function bodies are not translated. Some HLSL functionality requires to 
	/// be wrapped with macroes to make function bodies compile under non-HLSL languages.
	/// 
	/// Please, visit http://youtu.be/45O7WTc6k2Y for cross compiler implementation details.
	/// Implementation is not quite same but something very similar.
	///
	/// Some stuff from HLSL 5.0 that is currently not supported:
	/// * Template/Object-Oriented syntax.
	/// * Preprocessing.
	class HRIShaderCrossCompiler final : public IToolchainTool
	{
	private:
		GD_CLASS_UNASSIGNABLE(HRIShaderCrossCompiler);
		GD_CLASS_UNCOPIABLE  (HRIShaderCrossCompiler);

	public /*Class public API*/:
		GDINL explicit HRIShaderCrossCompiler(IToolchain* const Toolchain) : IToolchainTool(Toolchain) { }
		GDINL virtual ~HRIShaderCrossCompiler(                           ) { }

		/// @brief   Compiles HLSL shader into some specific platform implementation bytecode.
		/// @param   ShaderSourceInputStream    Input stream that contains shader source code.
		/// @param   ShaderByteCodeOutputStream Output stream that would contain shader compiled bytecode. If specified as null then no code is generated/compiled.
		/// @param   ShaderType                 Type of shader. Vertex, pixel, etc.
		/// @param   ShaderTargetPlatform       Target platfrom for which shader would be generated.
		/// @param   ShaderEntryPointName       Name of shader entry point.
		/// @returns Shader instance decription, genereated using provided data on success, or nullptr on fail.
		GDAPI HRIShaderInstanceDesc* CrossCompileShader(
			InputStream                * const  ShaderSourceInputStream, 
			OutputStream               * const  ShaderByteCodeOutputStream,
			HRIShaderType                const  ShaderType,
			HRIShaderCrossCompilerTarget const  ShaderTargetPlatform,
			String                       const& ShaderEntryPointName
		);
	};	// class HRIShaderCrossCompiler

GD_NAMESPACE_END

#endif	// ifndef GD_ENGINE_RENDERER_SHADER_CROSS_COMPILER 