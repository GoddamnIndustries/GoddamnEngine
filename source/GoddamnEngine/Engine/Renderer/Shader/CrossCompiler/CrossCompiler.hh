//////////////////////////////////////////////////////////////////////////
/// CrossCompiler.h - common interface for HLSL cross compiler.
/// Copyright (C) $(GD_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 27.03.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_ENGINE_RENDERER_SHADER_CROSS_COMPILER
#define GD_ENGINE_RENDERER_SHADER_CROSS_COMPILER

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/Object/Singleton/Singleton.hh>
#include <GoddamnEngine/Core/Compilers/Toolchain/Toolchain.hh>
#include <GoddamnEngine/Engine/Renderer/Shader/Shader.hh>

#if (!defined(GD_PLATFORM_DESKTOP))
#	error "This API can be used on desktop platfroms only."
#endif

GD_NAMESPACE_BEGIN

	/// Contains codes of exceptions that streamed lexer throws.
	enum HRIShaderCrossCompilerExceptionCodes : ToolchainException
	{
		GD_HRI_SHADERCC_EXCEPTION_MODULE =  1000L,					     			///< HRIShader cross compiler module.
		GD_HRI_SHADERCC_EXCEPTION_SYNTAX = +GD_HRI_SHADERCC_EXCEPTION_MODULE + 1L,  ///< Syntax error.
	};	// enum StreamedLexerExceptionCodes

	/// Describes list of supported profiles that HRIShader Cross-Compiler may generate code for.
	enum HRIShaderCrossCompilerProfile : UInt8
	{
		GD_HRI_SHADERCC_PROFILE_NONE,  ///< Shader Cross-Compiler does not translates but just generates meta-data.
		GD_HRI_SHADERCC_PROFILE_GLSL,  ///< Shader would be translated into OpenGL Shading language (version 4.10).
		GD_HRI_SHADERCC_PROFILE_PSSL,  ///< Shader would be translated into PlayStation4 Shading language.
		GD_HRI_SHADERCC_PROFILE_METAL, ///< Shader would be translated into Apple Metal Shading language.
	};	// enum HRIShaderCrossCompilerProfile

	class HRIShaderCrossCompilerMetaData;

	/// @brief Provides trasnslation of some subset of HLSL code into GLSL and PSSL.
	/// 
	/// Note that this is not a full parser, it just parsers a constant buffer, textures declarations and
	/// shader entry point. Whole function bodies are tranlated using some prediefined macros. 
	/// 
	/// Also this parser can generate headers for existing shaders that can be loaded and
	/// used to create meta-data stuff inside shaders.
	/// 
	/// Some stuff from HLSL 5.0 that is currently not supported:
	/// * Preprocessing.
	/// * HLSL method bodies parsing.
	class HRIShaderCrossCompiler final : public IToolchainTool
	{
	private:
		GD_CLASS_UNASSIGNABLE(HRIShaderCrossCompiler);
		GD_CLASS_UNCOPIABLE(HRIShaderCrossCompiler);

	public /*Class public API*/:
		GDINL  HRIShaderCrossCompiler(IToolchain* const Toolchain) : IToolchainTool(Toolchain) { }
		GDINL ~HRIShaderCrossCompiler(                           ) { }

		/// Parses shader infrastructure.
		/// @param ShaderSourceInputStream		Input stream that contains shader source.
		/// @param OutputShaderDescription		Pointer to output HLSL shader infrastructure.
		/// @param Type							Specifies type of HLSL shader.
		/// @param EntryPointName				Specifies name of that entry point inside HLSL shader.
		/// @returns True if operation succeded.
		GDAPI bool ParseShader(
			Stream*                const  ShaderSourceInputStream, 
			HRIShaderInstanceDesc      *& OutputShaderDescription,
			String                 const& EntryPointName
		);

		/// Translates HLSL shader into other language.
		/// @param ShaderSourceInputStream		Input stream that contains shader source.
		/// @param ShaderTranslatedOutputStream Output stream to write generated code to. May be nullptr if Profile is none.
		/// @param OutputShaderDescription		Pointer to output HLSL shader infrastructure.
		/// @param ShaderTargetProfile			Specifies target shader language.
		/// @param Type							Specifies type of HLSL shader.
		/// @param EntryPointName				Specifies name of that entry point inside HLSL shader.
		/// @returns True if operation succeded.
		GDAPI bool TranslateShader(
			Stream*                       const  ShaderSourceInputStream, 
			Stream*                       const  ShaderTranslatedOutputStream,
			HRIShaderInstanceDesc*        const  OutputShaderDescription,
			HRIShaderCrossCompilerProfile const  ShaderTargetProfile,
			String                        const& EntryPointName
		);
	};	// class HRIShaderCrossCompiler

GD_NAMESPACE_END

#endif	// ifndef GD_ENGINE_RENDERER_SHADER_CROSS_COMPILER 