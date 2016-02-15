// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#include <GoddamnEngine/Include.h>
#if	GD_PLATFORM_DESKTOP

#include <GoddamnEngine/ShaderCrossCompiler/GLSLBackend/GLSLBackend.h>
#include <GoddamnEngine/ShaderCrossCompiler/HLSLCrossCompiler.h>
#include <GoddamnEngine/ShaderCrossCompiler/HLSLTokenizer.h>
#include <GoddamnEngine/ShaderCrossCompiler/HLSLParser.h>

#include <GoddamnEngine/Core/Containers/StringBuilder.h>
//#include <GoddamnEngine/Core/Threading/CriticalSection/CriticalSection.h>
#include <GoddamnEngine/Core/System/IO/FileStream.h>

// Including all dependencies inside our shader compiler.
#if GD_COMPILER_MSVC
#	pragma warning(push, 0)
#endif // if GD_COMPILER_MSVC
#include <glsl_optimizer.h>
#include <glslang.h>
extern "C"
{
#if GD_COMPILER_MSVC
#	pragma push_macro("OUT")
#	undef OUT
#endif // if GD_COMPILER_MSVC
#include <mcpp_lib.h>
#if GD_COMPILER_MSVC
#	pragma pop_macro("OUT")
#endif // if GD_COMPILER_MSVC
}
#if GD_COMPILER_MSVC
#	pragma warning(pop)
#endif // if GD_COMPILER_MSVC

GD_NAMESPACE_BEGIN
	
	//! Represents compiler error description.
	typedef ToolchainException GLSLCompilerErrorException;

	namespace GLSLInsertations
	{
		// ------------------------------------------------------------------------------------------
		//! Copyright and automation warning.
		Char static const GLSLCopyright[] =
			"\n// This shader was automatically translated from HLSL by GoddamnEngine."
			;

		// ------------------------------------------------------------------------------------------
		//! Preambule for the GLSL code.
		CStr static const GLSLPreambule[] = {
			"\n#version 430 core"	// Preambule for the GLSL for OpenGL 4.3 code.
			"\n#extension GL_ARB_separate_shader_objects : enable"
			"\n#extension GL_ARB_shading_language_420pack : enable"
			"\n#extension GL_ARB_explicit_uniform_location : enable",

			"\n#version 310 es"		// Preambule for the GLSL for OpenGL(ES) 3.1 code.
			"\n#extension GL_ARB_separate_shader_objects : enable"
			"\n#extension GL_ARB_explicit_uniform_location : enable",

			"\n#version 200 es"		// Preambule for the GLSL for OpenGL(ES) 2.0 code.
			"\n#extension GL_ARB_separate_shader_objects : enable"
			"\n#extension GL_ARB_explicit_uniform_location : enable",
		};
			
		// ------------------------------------------------------------------------------------------
		//! Standard library and abstraction code for GLSL.
		Char static const GLSLInsertation[] =
			// Some keywords, that are not supported by some GLSL compilers.
			"\n#define static " "\n#define const " "\n#define inline "

			// Texture types abstraction.
			"\n#define Texture2D sampler2D " "\n#define TextureCube samplerCube " "\n#define Texture3D sampler3D "

			// Vector types abstraction.
			"\n#define   bool2 bvec2 " "\n#define   bool3 bvec3 " "\n#define   bool4 bvec4 "
			"\n#define    int2 ivec2 " "\n#define    int3 ivec3 " "\n#define    int4 ivec4 "
			"\n#define   uint2 uvec2 " "\n#define   uint3 uvec3 " "\n#define   uint4 uvec4 "
			"\n#define  float2  vec2 " "\n#define  float3  vec3 " "\n#define  float4  vec4 "
			"\n#define double2 dvec2 " "\n#define double3 dvec3 " "\n#define double4 dvec4 " 

			// Matrix types abstraction.
			"\n#define float2x2   mat2x2 " "\n#define float2x3   mat2x3 " "\n#define float3x2   mat3x2 "
			"\n#define float3x3   mat3x3 " "\n#define float3x4   mat3x4 " "\n#define float4x3   mat4x3 "
			"\n#define float4x4   mat4x4 "
			"\n#define double2x2 dmat2x2 " "\n#define double2x3 dmat2x3 " "\n#define double3x2 dmat3x2 "
			"\n#define double3x3 dmat3x3 " "\n#define double3x4 dmat3x4 " "\n#define double4x3 dmat4x3 "
			"\n#define double4x4 dmat4x4 "

			// Functions, that are available in HLSL and not in GLSL.
			// http://www.shaderific.com/glsl-functions/ 
			// https://msdn.microsoft.com/en-us/library/windows/desktop/ff471376(v=vs.85).aspx 
			"\n#define mul(x, y)   ((y) * (x)) "		// https://msdn.microsoft.com/en-us/library/windows/desktop/bb509628(v=vs.85).aspx
			"\n#define saturate(x) (clamp((x), 0, 1))"	// https://msdn.microsoft.com/en-us/library/windows/desktop/bb509645(v=vs.85).aspx
			;
	} // namespace GLSLInsertations

	// ==========================================================================================
	// HLSLGenerator class.
	// ==========================================================================================

	class GLSLGenerator final
	{
	public:
		GDINT void GenerateShader(StringBuilder& Builder, HLSLScope const* const ShaderParsedData, String const& ShaderEntryName
			, DHLSLShaderCrossCompilerTarget const ShaderTargetPlatform, IGraphicsShaderType const ShaderType);
	private:
		GDINT void GenerateShaderStruct(HLSLStruct const* const Struct, StringBuilder& Builder);
		GDINT void GenerateShaderConstantBuffer(HLSLCBuffer const* const ConstantBuffer, StringBuilder& Builder, bool const SupportsConstantBuffers, IGraphicsShaderType const ShaderType);
		GDINT void GenerateShaderStaticVariable(HLSLVariable const* const StaticVariable, StringBuilder& Builder);
		GDINT void GenerateShaderStaticFunction(HLSLFunction const* const StaticFunction, StringBuilder& Builder);
		GDINT void GenerateShaderEntryPoint(HLSLFunction const* const EntryPoint, StringBuilder& Builder);
	}; // class GLSLGenerator

	// ------------------------------------------------------------------------------------------
	//! Generates the GLSL shader code.
	//! @param Builder Output for the shader code.
	//! @param ShaderParsedData Parsed HLSL code.
	//! @param ShaderEntryName Name of the shader entry point.
	//! @param ShaderTargetPlatform Target platform for the GLSL code.
	//! @param ShaderType Type of the shader to generate code.
	void GLSLGenerator::GenerateShader(StringBuilder& Builder, HLSLScope const* const ShaderParsedData, String const& ShaderEntryName
		, DHLSLShaderCrossCompilerTarget const ShaderTargetPlatform, IGraphicsShaderType const ShaderType)
	{
		Builder.Append(GLSLInsertations::GLSLInsertation, GD_ARRAY_LENGTH(GLSLInsertations::GLSLInsertation) - 1);
		for (auto const& Definition : ShaderParsedData->InnerDefinitions)
		{ 
			auto const Type = HLSLDynamicCast<HLSLType const*>(Definition);
			if (Type != nullptr)
			{
				// Generating information about our struct..
				GenerateShaderStruct(static_cast<HLSLStruct const*>(Type), Builder);
				continue;
			}

			auto const ConstantBuffer = HLSLDynamicCast<HLSLCBuffer const*>(Definition);
			if (ConstantBuffer != nullptr)
			{
				// Generating information about our constant buffer..
				GenerateShaderConstantBuffer(ConstantBuffer, Builder, ShaderTargetPlatform != DHLSL_CC_TARGET_GLSLES2, ShaderType);
				continue;
			}
				
			auto const StaticVariable = HLSLDynamicCast<HLSLVariable const*>(Definition);
			if (StaticVariable != nullptr)
			{
				// Generating information about static variable..
				GenerateShaderStaticVariable(StaticVariable, Builder);
				continue;
			}

			auto const StaticFunction = HLSLDynamicCast<HLSLFunction const*>(Definition);
			if (StaticFunction != nullptr)
			{ 
				// Generating information about static function..
				GenerateShaderStaticFunction(StaticFunction, Builder);
				continue;
			}
		}

		// And finally generating the entry point..
		GenerateShaderEntryPoint(ShaderParsedData->FindFunction(ShaderEntryName), Builder);
	}

	// ------------------------------------------------------------------------------------------
	//! Generates a structure inside GLSL shader.
	//! @param Struct Structure object definition.
	//! @param Builder Output for the shader code.
	void GLSLGenerator::GenerateShaderStruct(HLSLStruct const* const Struct, StringBuilder& Builder)
	{
		Builder.AppendFormat("\n\nstruct %s\n{", Struct->Name.CStr());
		for (auto const& Definition : Struct->InnerDefinitions)
		{
			HLSLVariable const* const StructField = static_cast<HLSLVariable const*>(Definition);
			Builder.AppendFormat("\n\t%s %s", StructField->Type->Name.CStr(), StructField->Name.CStr());
			if (StructField->ArrayRank != 0)
			{
				// Adding array information for our field..
				Builder.AppendFormat("[%d]", static_cast<int>(StructField->ArrayRank));
			}
			Builder.Append(';');
		}
		Builder.Append("\n};");
	}

	// ------------------------------------------------------------------------------------------
	//! Generates a structure inside GLSL shader.
	//! @param ConstantBuffer Constant buffer object definition.
	//! @param Builder Output for the shader code.
	//! @param SupportsConstantBuffers Boolean variable for the support of the uniform buffers.
	//! @param ShaderType Target type of the shader.
	void GLSLGenerator::GenerateShaderConstantBuffer(HLSLCBuffer const* const ConstantBuffer, StringBuilder& Builder, bool const SupportsConstantBuffers, IGraphicsShaderType const ShaderType)
	{
		if (ConstantBuffer->Register == nullptr)
		{
			throw GLSLCompilerErrorException("constant buffer '%s' does not contains meta information about register.", ConstantBuffer->Name.CStr());
		}

		if (SupportsConstantBuffers)
		{
			// This platform supports constant buffers, defining it.
			Builder.AppendFormat("\n\nlayout(std140, row_major, binding = %d) \nuniform %s \n{"
				, static_cast<int>(ConstantBuffer->Register->RegisterID) + static_cast<int>(ShaderType) * static_cast<int>(GD_HRI_SHADER_MAX_CBUFFERS_LOCATIONS)
				, ConstantBuffer->Name.CStr());
		}
		else
		{
			// Emulating behavior of the constant buffers through uniform variables.
			Builder.AppendFormat("\n\n// uniform %s {", ConstantBuffer->Name.CStr());
		}

		for (auto const& Definition : ConstantBuffer->InnerDefinitions)
		{
			auto const ConstantBufferField = static_cast<UniquePtr<HLSLVariable const>>(Definition);
			if (SupportsConstantBuffers)
			{
				// Emitting a field of the inform buffer.
				Builder.AppendFormat("\n\t%s %s", ConstantBufferField->Type->Name.CStr(), ConstantBufferField->Name.CStr());
			}
			else
			{
				// Emitting a standalone uniform variable.
				Builder.AppendFormat("\nlayout(row_major) \nuniform %s %s", ConstantBufferField->Type->Name.CStr(), ConstantBufferField->Name.CStr());
			}

			if (ConstantBufferField->ArrayRank != 0)
			{
				// Adding array information for our field..
				Builder.AppendFormat("[%d]", static_cast<int>(ConstantBufferField->ArrayRank));
			}
			Builder.Append(';');
		}

		if (SupportsConstantBuffers)
			Builder.Append("\n};");
		else
			Builder.AppendFormat("\n// }\t// uniform %s.", ConstantBuffer->Name.CStr());
	}

	// ------------------------------------------------------------------------------------------
	//! Generates a HLSL source code from the specified parsed static variable definition.
	//! @param StaticVariable Parsed shader static variable, produced by @ref HLSLParser.
	//! @param Builder Output for the shader code.
	void GLSLGenerator::GenerateShaderStaticVariable(HLSLVariable const* const StaticVariable, StringBuilder& Builder)
	{
		// Skipping samplers.
		if (StaticVariable->Type->Class == GD_HLSL_TYPE_CLASS_SAMPLER)
			return;

		Builder.Append("\n\n");
		if (StaticVariable->Binding != nullptr)
		{
			// Emitting the global uniform variable.
			HLSLRegister const* const StaticVariableRegister = static_cast<HLSLRegister const*>(StaticVariable->Binding);
			Builder.AppendFormat("layout(location = %d) \nuniform ", static_cast<int>(StaticVariableRegister->RegisterID));
		}

		Builder.AppendFormat("%s %s", StaticVariable->Type->Name.CStr(), StaticVariable->Name.CStr());
		if (StaticVariable->ArrayRank != 0)
		{
			// Adding array information for our field..
			Builder.AppendFormat("[%d]", static_cast<int>(StaticVariable->ArrayRank));
		}
		Builder.Append(';');
	}

	// ------------------------------------------------------------------------------------------
	//! Generates a HLSL source code from the specified parsed static function definition.
	//! @param StaticFunction Parsed shader static function, produced by @ref HLSLParser.
	//! @param Builder Output for the shader code.
	void GLSLGenerator::GenerateShaderStaticFunction(HLSLFunction const* const StaticFunction, StringBuilder& Builder)
	{
		if (StaticFunction->Type->Name == "main")
		{
			//! @todo Possibly this is an entry point? We need just to mangle code here.
			throw GLSLCompilerErrorException("it is not allowed to name a function 'main'.");
		}

		Builder.AppendFormat("\n\ninline %s %s(", StaticFunction->Type->Name.CStr(), StaticFunction->Name.CStr());
		for (SizeTp cnt = 0; cnt < StaticFunction->Arguments.GetLength(); ++cnt)
		{
			// Emitting function parameter..
			auto const& Argument = StaticFunction->Arguments[cnt];
			Builder.AppendFormat("%s%s %s %s", (Argument->AccsessType & GD_HLSL_ARGUMENT_IN) != 0 ? "in" : "", (Argument->AccsessType & GD_HLSL_ARGUMENT_OUT) != 0 ? "out" : ""
				, Argument->Type->Name.CStr(), Argument->Name.CStr());
			if (cnt != StaticFunction->Arguments.GetLength() - 1)
			{
				// This is not the last argument, appending comma after it.
				Builder.AppendFormat(", ");
			}
		}
		Builder.AppendFormat(")\n%s", StaticFunction->Body.CStr());
	}

	// ------------------------------------------------------------------------------------------
	//! @todo Document and cleanup me.
	void GLSLGenerator::GenerateShaderEntryPoint(HLSLFunction const* const EntryPoint, StringBuilder& Builder)
	{
		StringBuilder FakeEntryAssigment;
		StringBuilder FakeEntryInvocation;
		StringBuilder FakeEntryInternals, FakeEntryExternals;
		int LastUsedSemanticIn = 0, LastUsedSemanticOut = 0;
		for (auto const& Argument : EntryPoint->Arguments)
		{
			CStr const ArgumentAccessType = Argument->AccsessType == GD_HLSL_ARGUMENT_IN ? "in " : "out";
			int& LastUsedSemantic = Argument->AccsessType == GD_HLSL_ARGUMENT_IN ? LastUsedSemanticIn : LastUsedSemanticOut;

			if (Argument->Type->Class == GD_HLSL_TYPE_CLASS_STRUCT)
			{
				if (Argument->AccsessType == GD_HLSL_ARGUMENT_IN)
				{
					FakeEntryInvocation.AppendFormat("%s(", Argument->Type->Name.CStr(), Argument->Name.CStr());
				}
				else
				{
					FakeEntryInvocation.AppendFormat("%s, ", Argument->Name.CStr());
					FakeEntryInternals.AppendFormat("\n\t%s %s;", Argument->Type->Name.CStr(), Argument->Name.CStr());
				}

				auto const Struct = static_cast<HLSLStruct const*>(Argument->Type);
				for (auto const& Definition : Struct->InnerDefinitions)
				{
					auto const StructField = static_cast<HLSLVariable const*>(Definition);
				//	auto const StructFieldSemantic = static_cast<HLSLSemantic const*>(StructField->Binding);
					FakeEntryExternals.AppendFormat("\nlayout(location = %d) %s %s Varying%s%s;", LastUsedSemantic
						, ArgumentAccessType, StructField->Type->Name.CStr(), Argument->Name.CStr(), StructField->Name.CStr());

					if (Argument->AccsessType == GD_HLSL_ARGUMENT_IN)
						FakeEntryInvocation.AppendFormat("Varying%s%s, ", Argument->Name.CStr(), StructField->Name.CStr());
					else
						FakeEntryAssigment.AppendFormat("\n\tVarying%s%s = %s.%s;", Argument->Name.CStr(), StructField->Name.CStr(), Argument->Name.CStr(), StructField->Name.CStr());
					LastUsedSemantic += 1;
				}

				if (Argument->AccsessType == GD_HLSL_ARGUMENT_IN)
				{
					//! @todo Refactor this awful code.
					*(FakeEntryInvocation.CStr() + FakeEntryInvocation.GetLength() - 2) = ')'; // Removing trailing comma.
					*(FakeEntryInvocation.CStr() + FakeEntryInvocation.GetLength() - 1) = ','; // Removing trailing space.
					FakeEntryInvocation.Append(' ');
				}
			}
			else
			{
				auto const ArgumentSemantic = static_cast<HLSLSemantic const*>(Argument->Binding);
				if (Argument->AccsessType == GD_HLSL_ARGUMENT_OUT)
				{
					if (ArgumentSemantic->Semantic == GD_HLSL_SEMANTIC_SV_Position)
						FakeEntryAssigment.AppendFormat("\n\tgl_Position = %s;", Argument->Name.CStr());
				}

				FakeEntryExternals.AppendFormat("\nlayout(location = %d) %s %s Varying%s;", LastUsedSemantic, ArgumentAccessType, Argument->Type->Name.CStr(), Argument->Name.CStr());
				FakeEntryInvocation.AppendFormat("Varying%s, ", Argument->Name.CStr());

				LastUsedSemantic += 1;
			}
		}

		//! @todo Refactor this awful code.
		*(FakeEntryInvocation.CStr() + FakeEntryInvocation.GetLength() - 2) = ')'; // Removing trailing comma.
		*(FakeEntryInvocation.CStr() + FakeEntryInvocation.GetLength() - 1) = ';'; // Removing trailing space.
		FakeEntryInvocation = Move(StringBuilder().AppendFormat("\n\t%s(%s", EntryPoint->Name.CStr(), FakeEntryInvocation.CStr()));

		if (EntryPoint->Type->Class == GD_HLSL_TYPE_CLASS_STRUCT)
		{
			FakeEntryInternals.AppendFormat("\n\t%s Ret;", EntryPoint->Type->Name.CStr());
			FakeEntryInvocation = Move(StringBuilder().AppendFormat("\n\tRet = %s", FakeEntryInvocation.CStr() + 2));

			HLSLStruct const* const ReturnStruct = static_cast<HLSLStruct const*>(EntryPoint->Type);
			for (auto const& Definition : ReturnStruct->InnerDefinitions)
			{
				HLSLVariable const* const ReturnStructField = static_cast<HLSLVariable const*>(Definition);
			//	HLSLSemantic const* const ReturnStructFieldSemantic = static_cast<HLSLSemantic const*>(ReturnStructField->Binding);
				FakeEntryExternals.AppendFormat("\nlayout(location = %d) out %s VaryingRet%s;", LastUsedSemanticOut, ReturnStructField->Type->Name.CStr(), ReturnStructField->Name.CStr());
				FakeEntryAssigment.AppendFormat("\n\tVaryingRet%s = Ret.%s;", ReturnStructField->Name.CStr(), ReturnStructField->Name.CStr());
				LastUsedSemanticOut += 1;
			}
		}
		else if (EntryPoint->Semantic != nullptr)
		{
			FakeEntryExternals.AppendFormat("\nlayout(location = %d) out %s VaryingRet;", LastUsedSemanticOut, EntryPoint->Name.CStr());
			FakeEntryInvocation = Move(StringBuilder().AppendFormat("\n\tVaryingRet = %s", FakeEntryInvocation.CStr() + 2));
		}

		Builder.AppendFormat("\n%s\n\nvoid main()\n{\n%s\n%s\n%s}\n", FakeEntryExternals.CStr()
			, FakeEntryInternals.CStr(), FakeEntryInvocation.CStr(), FakeEntryAssigment.CStr());
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// HLSLCompilerBackend class.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Generates the HLSL shader Byte code.
	//! @param ShaderByteCodeOutputStream Output for the shader code.
	//! @param ShaderParsedData Parsed shader data, produced by @ref HLSLParser
	//! @param ShaderType Type of the shader to generate code.
	//! @param ShaderEntryName Name of the shader entry point.
	//! @param ShaderTargetPlatform Target platform for the GLSL code.
	void GLSLCompiler::GenerateAndCompileShader(UniquePtr<OutputStream> const& ShaderByteCodeOutputStream
		, HLSLScope const* const ShaderParsedData, IGraphicsShaderType const ShaderType, String const& ShaderEntryName
		, DHLSLShaderCrossCompilerTarget const ShaderTargetPlatform)
	{
		StringBuilder GLSLGeneratorBuilder;
		GLSLGenerator().GenerateShader(GLSLGeneratorBuilder, ShaderParsedData, ShaderEntryName, ShaderTargetPlatform, ShaderType);

		// Now we need just preprocess generated code to reduce loading time.
		StringBuilder GLSLPreprocessedBuilder;
		GLSLPreprocessedBuilder.Append(GLSLInsertations::GLSLCopyright);
		GLSLPreprocessedBuilder.Append(GLSLInsertations::GLSLPreambule[ShaderTargetPlatform - DHLSL_CC_TARGET_GLSL430]);

		// We use MCPP preprocessor because it is faster than our one.
		//! @todo Implement temporary files access here.
		String const MCPPSourceFilePath = "a.tmp";// Path::GetTemporaryFileName();
		String const MCPPOutputFilePath = "b.tmp";// Path::GetTemporaryFileName();
		CStr   const MCPPArguments[] = { nullptr, "-P", MCPPSourceFilePath.CStr(), MCPPOutputFilePath.CStr() };

		// Saving output to some temporary file to make is accessible from MCPP
		FileOutputStream MCPPSourceFile(/*MCPPSourceFilePath*/L"a.tmp");
		MCPPSourceFile.Write(GLSLGeneratorBuilder.CStr(), GLSLGeneratorBuilder.GetLength(), sizeof(Char));
		MCPPSourceFile.Close();

		{ 
		//	MCPP is not thread-safe.
		//	CriticalSection static MCPPCriticalSection;
		//	ScopedCriticalSection MCPPLock(MCPPCriticalSection);
			int const MCPPResult = mcpp_lib_main(GD_ARRAY_LENGTH(MCPPArguments), const_cast<char**>(&MCPPArguments[0]));
			GD_DEBUG_ASSERT(MCPPResult == 0, "Failed to preprocess shader source.");
		}
		// Reading MCPP result.
		{
			FileInputStream MCPPOutputFile(/*MCPPOutputFilePath*/L"b.tmp");
			SizeTp const GLSLPreprocessedBuilderPos = GLSLPreprocessedBuilder.GetLength();
			GLSLPreprocessedBuilder.Resize(GLSLPreprocessedBuilder.GetLength() + (UInt32) MCPPOutputFile.GetLength());
			MCPPOutputFile.Read(GLSLPreprocessedBuilder.CStr() + GLSLPreprocessedBuilderPos, MCPPOutputFile.GetLength(), 1);
			MCPPOutputFile.Close();
			GLSLPreprocessedBuilder.Append('\0');
			GD_DEBUG_ASSERT(::remove(MCPPSourceFilePath.CStr()) == 0, "Failed to remove mcpp source file.");
			GD_DEBUG_ASSERT(::remove(MCPPOutputFilePath.CStr()) == 0, "Failed to remove mcpp output file.");
		}

		// Trying to optimize the GLSL code..
		StringBuilder GLSLOptimizerBuilder;
		/**/if (ShaderTargetPlatform != DHLSL_CC_TARGET_GLSL430									// glsl_optimizer supports only desktop GLSL 140 version. We are using 430...			
			&& (ShaderType != IGRAPHICS_SHADER_TYPE_VERTEX && ShaderType != IGRAPHICS_SHADER_TYPE_PIXEL))	// glsl_optimizer supports only vertex and pixel shaders...
		{
			// Optimizing is supported for this shader type and target.
			glslopt_target static const IGraphicsGLSLangTargetsTable[] = {
				/* DHLSL_CC_TARGET_GLSL430 */ kGlslTargetOpenGL,
				/* DHLSL_CC_TARGET_GLSLES3 */ kGlslTargetOpenGLES20,
				/* DHLSL_CC_TARGET_GLSLES2 */ kGlslTargetOpenGLES30,
			};
			glslopt_shader_type static const IGraphicsGLSLangShaderTypesTable[] = {
				/* IGRAPHICS_SHADER_TYPE_VERTEX */ kGlslOptShaderVertex,
				/* IGRAPHICS_SHADER_TYPE_PIXEL  */ kGlslOptShaderFragment,
			};

			auto const OptimizerContext = glslopt_initialize(IGraphicsGLSLangTargetsTable[ShaderTargetPlatform]);
			auto const OptimizedShader = glslopt_optimize(OptimizerContext, IGraphicsGLSLangShaderTypesTable[ShaderType - DHLSL_CC_TARGET_GLSL430]
				, GLSLPreprocessedBuilder.CStr(), kGlslOptionSkipPreprocessor);
			if (!glslopt_get_status(OptimizedShader))
			{
				// Shader was not optimized..
				// This does not mean that our shader is incorrect, just MESA-based optimizer may not parse
#if 0			// our code correctly.
				CStr const ShaderOptimizationLog = glslopt_get_log(OptimizedShader);
				throw GLSLCompilerErrorException("shader optimization failed with following log: \n%s", ShaderOptimizationLog);
#endif	// if 0
				GLSLOptimizerBuilder = Move(GLSLPreprocessedBuilder);
			}
			else
			{
				// Shader was optimized..
				GLSLOptimizerBuilder.Append(glslopt_get_output(OptimizedShader));
			}

			glslopt_shader_delete(OptimizedShader);
			glslopt_cleanup(OptimizerContext);
		}
		else
		{
			// Shader was not optimized - using default version.
			GLSLOptimizerBuilder = Move(GLSLPreprocessedBuilder);
		}

		// No we need to validate our shader.. 
		TBuiltInResource static const GLSLangDefaultResources{ INT_MAX };
		EShLanguage static const IGraphicsGLSLangShaderTypesTable[IGRAPHICS_SHADER_TYPES_COUNT] = {
			/* IGRAPHICS_SHADER_TYPE_VERTEX   */ EShLangVertex,
			/* IGRAPHICS_SHADER_TYPE_PIXEL    */ EShLangFragment,
			/* IGRAPHICS_SHADER_TYPE_GEOMETRY */ EShLangGeometry,
			/* IGRAPHICS_SHADER_TYPE_HULL     */ EShLangTessControl,
			/* IGRAPHICS_SHADER_TYPE_DOMAIN   */ EShLangTessEvaluation,
			/* IGRAPHICS_SHADER_TYPE_COMPUTE  */ EShLangCompute,
			/* IGRAPHICS_SHADER_TYPE_UNKNOWN  */ EShLangCount,
		};

		glslang::InitializeProcess();

		// Trying to compile our shader..
		char const* const GLSLOptimizerBuilderPtr = GLSLOptimizerBuilder.CStr();
		glslang::TShader GLSLangShader(IGraphicsGLSLangShaderTypesTable[ShaderType]);
		GLSLangShader.setStrings(&GLSLOptimizerBuilderPtr, 1);
		if (!GLSLangShader.parse(&GLSLangDefaultResources, 100, true, EShMsgDefault))
		{ 
			// Failed to compile our shader.
			throw GLSLCompilerErrorException("GLSLang compilation returned errors: \n%s\nGenerated code:\n%s", GLSLangShader.getInfoLog(), GLSLOptimizerBuilderPtr);
		}
		else
		{
#if 0
			Debug::Log(GLSLangShader.getInfoLog());
#endif	// if 0
		}

		// Trying to link our shader program..
		glslang::TProgram GLSLangProgram;
		GLSLangProgram.addShader(&GLSLangShader);
		if (!GLSLangProgram.link(EShMsgDefault))
		{ 
			throw GLSLCompilerErrorException("GLSLang linking returned errors: \n%s", GLSLangProgram.getInfoLog());
		}
		else
		{
#if 0
			Debug::Log(GLSLangProgram.getInfoLog());
#endif	// if 0
		}

		glslang::FinalizeProcess();

		ShaderByteCodeOutputStream->Write(GLSLOptimizerBuilder.CStr(), GLSLOptimizerBuilder.GetLength(), sizeof(Char));
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_DESKTOP
