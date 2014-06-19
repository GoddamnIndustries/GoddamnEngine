//////////////////////////////////////////////////////////////////////////
/// GLSLCompiler.hh: GLSL compiler generator and compiler implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 13.06.2014 - Created by James Jhuighuy.
//////////////////////////////////////////////////////////////////////////

// We have Warning 4 level (Wx), but glslang has W3 -Wx.
#if (defined(_MSC_VER))
#	define _CRT_SECURE_NO_WARNINGS
#	define _USE_MATH_DEFINES
#	pragma warning(push, 0)
#endif	// if (defined(_MSC_VER))
#include <glsl_optimizer.h>
#include <glslang.h>

extern "C"
{
#include <mcpp_lib.h>
}

#if (defined(_MSC_VER))
#	undef _CRT_SECURE_NO_WARNINGS
#	undef _USE_MATH_DEFINES
#	pragma warning(pop)
#endif	// if (defined(_MSC_VER))

#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/Impl/GLSL/GLSLCompiler.hh>
#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/Parser/Parser.hh>
#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/CrossCompiler.hh>

#include <GoddamnEngine/Core/Text/StringBuilder/StringBuilder.hh>
#include <GoddamnEngine/Core/Threading/CriticalSection/CriticalSection.hh>
#include <GoddamnEngine/Core/IO/Stream/FileStream/FileStream.hh>
#include <GoddamnEngine/Core/IO/Path/Path.hh>

/// Define this to perform dual check of shaders using both glsl_optimizer and glslang.
#define GD_HRI_SHADERCC_GLSL_COMPILER_DUAL_CHECK

GD_NAMESPACE_BEGIN

	typedef Str GLSLCompilerErrorDesc;
	typedef Str GLSLCompilerWarningDesc;

	namespace GLSLInsertations
	{
		static Char const GLSLCopyright[] = 
R"(//////////////////////////////////////////////////////////////////////////
/// This file was automatically translated from HLSL by Goddamn Engine.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//////////////////////////////////////////////////////////////////////////
)";

		static Char const GLSL430Preambule[] =
R"(
#version 430
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_shading_language_420pack  : enable
#extension GL_ARB_separate_shader_objects   : enable
)";

		static Char const GLSLES3Preambule[] =
R"(
#version 300 es
)";
		static Char const GLSLES2Preambule[] =
R"(
#version 200 es
)";

		static Char const GLSLInsertation[] =
R"(
#define static
#define inline
#define const
#define Sampler2D   sampler2D
#define SamplerCube samplerCube
#define bool2   bvec2
#define bool3   bvec3
#define bool4   bvec4
#define int2    ivec2
#define int3    ivec3
#define int4    ivec4
#define uint2   uvec2
#define uint3   uvec3
#define uint4   uvec4
#define float2   vec2
#define float3   vec3
#define float4   vec4
#define double2 dvec2
#define double3 dvec3
#define double4 dvec4
#define float2x2   mat2x2
#define float2x3   mat2x3
#define float3x2   mat3x2
#define float3x3   mat3x3
#define float3x4   mat3x4
#define float4x3   mat4x3
#define float4x4   mat4x4
#define double2x2 dmat2x2
#define double2x3 dmat2x3
#define double3x2 dmat3x2
#define double3x3 dmat3x3
#define double3x4 dmat3x4
#define double4x3 dmat4x3
#define double4x4 dmat4x4
#define mul(First, Second) (First * Second)
)";
	}	// namespace GLSLInsertations

	class GLSLGenerator final : public IToolchainTool
	{
	public:
		GDINL  GLSLGenerator(IToolchain* const Toolchain) : IToolchainTool(Toolchain) { }
		GDINL ~GLSLGenerator() { }
		GDINT bool GenerateShader(StringBuilder& Builder, HLSLScope const* const ShaderParsedData, String const& ShaderEntryName, HRIShaderCrossCompilerTarget const ShaderTargetPlatform);
	private:
		GDINT bool GenerateShaderStruct(HLSLStruct const* const Struct, StringBuilder& Builder);
		GDINT bool GenerateShaderConstantBuffer(HLSLCBuffer const* const ConstantBuffer, StringBuilder& Builder, bool const SupportsConstantBuffers);
		GDINT bool GenerateShaderStaticVariable(HLSLVariable const* const StaticVariable, StringBuilder& Builder);
		GDINT bool GenerateShaderStaticFunction(HLSLFunction const* const StaticFunction, StringBuilder& Builder);
		GDINT bool GenerateShaderEntry(HLSLFunction const* const EntryPoint, StringBuilder& Builder);
	};	// class GLSLGenerator

	bool GLSLGenerator::GenerateShaderStruct(HLSLStruct const* const Struct, StringBuilder& Builder)
	{
		Builder.AppendFormat("\n\nstruct %s\n{", Struct->Name.CStr());
		for (auto const Definition : Struct->InnerDefinitions)
		{
			HLSLVariable const* const StructField = static_cast<HLSLVariable const*>(Definition);
			Builder.AppendFormat("\n\t%s %s", StructField->Type->Name.CStr(), StructField->Name.CStr());
			if (StructField->ArrayIndex != 0)
				Builder.AppendFormat("[%d]", static_cast<int>(StructField->ArrayIndex));
			
			Builder.Append(';');
		}

		Builder.Append("\n};");
		return true;
	}

	bool GLSLGenerator::GenerateShaderConstantBuffer(HLSLCBuffer const* const ConstantBuffer, StringBuilder& Builder, bool const SupportsConstantBuffers)
	{
		if (ConstantBuffer->Register == nullptr)
		{
			GLSLCompilerErrorDesc static const ExplicitConstantBufferRegisterNotSpecified("constant buffer 's' does not contains meta information about register.");
			self->RaiseError(ExplicitConstantBufferRegisterNotSpecified, ConstantBuffer->Name.CStr());
			self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

			return false;
		}

		if (SupportsConstantBuffers)
			Builder.AppendFormat("\n\nlayout(std140, row_major, binding = %d) uniform %s \n{", static_cast<int>(ConstantBuffer->Register->RegisterID), ConstantBuffer->Name.CStr());
		else
			Builder.AppendFormat("\n\n// uniform %s {", ConstantBuffer->Name.CStr());

		for (auto const Definition : ConstantBuffer->InnerDefinitions)
		{
			HLSLVariable const* const ConstantBufferField = static_cast<HLSLVariable const*>(Definition);
		
			if (SupportsConstantBuffers)
				Builder.AppendFormat("\n\t%s %s", ConstantBufferField->Type->Name.CStr(), ConstantBufferField->Name.CStr());
			else
				Builder.AppendFormat("\nlayout(row_major) uniform %s %s", ConstantBufferField->Type->Name.CStr(), ConstantBufferField->Name.CStr());

			if (ConstantBufferField->ArrayIndex != 0)
				Builder.AppendFormat("[%d]", static_cast<int>(ConstantBufferField->ArrayIndex));
				
			Builder.Append(';');
		}

		if (SupportsConstantBuffers)
			Builder.Append("\n};");
		else
			Builder.AppendFormat("\n// }\t// uniform %s.", ConstantBuffer->Name.CStr());

		return true;
	}

	bool GLSLGenerator::GenerateShaderStaticVariable(HLSLVariable const* const StaticVariable, StringBuilder& Builder)
	{
		if (StaticVariable->Type->Class == GD_HLSL_TYPE_CLASS_SAMPLER)	// Skipping samplers.
			return true;
		
		Builder.Append("\n\n");
		if (StaticVariable->ExprColon != nullptr)
		{
			HLSLRegister const* const StaticVariableRegister =  static_cast<HLSLRegister const*>(StaticVariable->ExprColon);
			Builder.Append("layout(location = %d) uniform", static_cast<int>(StaticVariableRegister->RegisterID));
		}

		Builder.AppendFormat("%s %", StaticVariable->Type->Name.CStr(), StaticVariable->Name.CStr());
		if (StaticVariable->ArrayIndex != 0)
			Builder.AppendFormat("[%d]", static_cast<int>(StaticVariable->ArrayIndex));
			
		Builder.Append(';');
		return true;
	}

	bool GLSLGenerator::GenerateShaderStaticFunction(HLSLFunction const* const StaticFunction, StringBuilder& Builder)
	{
		if (StaticFunction->Type->Name == "main")
		{
			GLSLCompilerErrorDesc static const MainFunctionRedifinionError("it is not allowed to name a function 'main'.");
			self->RaiseError(MainFunctionRedifinionError);
			self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

			return false;
		}

		Builder.AppendFormat("\n\ninline %s %s(", StaticFunction->Type->Name.CStr(), StaticFunction->Name.CStr());
		for (auto const Argument : StaticFunction->Arguments)
			Builder.AppendFormat("%s%s %s %s, ",
			(((Argument->AccsessType & GD_HLSL_ARGUMENT_IN ) != 0) ? "in"  : ""),
			(((Argument->AccsessType & GD_HLSL_ARGUMENT_OUT) != 0) ? "out" : ""),
			Argument->Type->Name.CStr(), Argument->Name.CStr());
		*(Builder.GetPointer() + Builder.GetSize() - 2) =  ')';
		*(Builder.GetPointer() + Builder.GetSize() - 1) = '\n';
		Builder.Append(StaticFunction->Body);
		return true;
	}

	bool GLSLGenerator::GenerateShaderEntry(HLSLFunction const* const EntryPoint, StringBuilder& Builder)
	{
		StringBuilder FakeEntryAssigment;
		StringBuilder FakeEntryInvocation;
		StringBuilder FakeEntryInternals, FakeEntryExternals;
		int LastUsedSemanticIn = 0, LastUsedSemanticOut = 0;
		for (auto const Argument : EntryPoint->Arguments)
		{
			Str const ArgumentAccessType = ((Argument->AccsessType == GD_HLSL_ARGUMENT_IN) ? "in" : "out");
			int&      LastUsedSemantic   = ((Argument->AccsessType == GD_HLSL_ARGUMENT_IN) ? LastUsedSemanticIn : LastUsedSemanticOut);

			if (Argument->Type->Class == GD_HLSL_TYPE_CLASS_STRUCT)
			{
				if (Argument->AccsessType == GD_HLSL_ARGUMENT_IN)
					FakeEntryInvocation.AppendFormat("%s(", Argument->Type->Name.CStr(), Argument->Name.CStr());
				else
				{
					FakeEntryInternals.AppendFormat("\n\t%s %s;", Argument->Type->Name.CStr(), Argument->Name.CStr());
					FakeEntryInvocation.AppendFormat("%s, ", Argument->Name.CStr());
				}

				HLSLStruct const* const Struct = static_cast<HLSLStruct const*>(Argument->Type);
				for (auto const Definition : Struct->InnerDefinitions)
				{
					HLSLVariable const* const StructField = static_cast<HLSLVariable const*>(Definition);
					HLSLSemantic const* const StructFieldSemantic = static_cast<HLSLSemantic const*>(StructField->ExprColon);
					FakeEntryExternals.AppendFormat("\nlayout(location = %d) %s %s Varying%s%s;", LastUsedSemantic, ArgumentAccessType, StructField->Type->Name.CStr(), Argument->Name.CStr(), StructField->Name.CStr());

					if (Argument->AccsessType == GD_HLSL_ARGUMENT_IN)
						FakeEntryInvocation.AppendFormat("Varying%s%s, ", Argument->Name.CStr(), StructField->Name.CStr());
					else
						FakeEntryAssigment.AppendFormat("\n\tVarying%s%s = %s.%s;", Argument->Name.CStr(), StructField->Name.CStr(), Argument->Name.CStr(), StructField->Name.CStr());

					LastUsedSemantic += 1;
				}
				
				if (Argument->AccsessType == GD_HLSL_ARGUMENT_IN)
				{
					*(FakeEntryInvocation.GetPointer() + FakeEntryInvocation.GetSize() - 2) = ')';	// Removing trailing comma.
					*(FakeEntryInvocation.GetPointer() + FakeEntryInvocation.GetSize() - 1) = ',';	// Removing trailing space.
					  FakeEntryInvocation.Append(' ');
				}
			}
			else 
			{
				HLSLSemantic const* const ArgumentSemantic = static_cast<HLSLSemantic const*>(Argument->ExprColon);
				if (Argument->AccsessType == GD_HLSL_ARGUMENT_OUT)
				if (ArgumentSemantic->Semantic == GD_HLSL_SEMANTIC_SV_Position)
					FakeEntryAssigment.AppendFormat("\n\tgl_Position = %s;", Argument->Name.CStr());

				FakeEntryExternals.AppendFormat("\nlayout(location = %d) %s %s Varying%s;", LastUsedSemantic, ArgumentAccessType, Argument->Type->Name.CStr(), Argument->Name.CStr());
				FakeEntryInvocation.AppendFormat("Varying%s, ", Argument->Name.CStr());
			
				LastUsedSemantic += 1;
			}
		}

		*(FakeEntryInvocation.GetPointer() + FakeEntryInvocation.GetSize() - 2) = ')';	// Removing trailing comma.
		*(FakeEntryInvocation.GetPointer() + FakeEntryInvocation.GetSize() - 1) = ';';	// Removing trailing space.
		  FakeEntryInvocation = Move(StringBuilder().AppendFormat("\n\t%s(%s", EntryPoint->Name.CStr(), FakeEntryInvocation.GetPointer()));

		if (EntryPoint->Type->Class == GD_HLSL_TYPE_CLASS_STRUCT)
		{
			FakeEntryInternals.AppendFormat("\n\t%s Ret;", EntryPoint->Type->Name.CStr());
			FakeEntryInvocation = Move(StringBuilder().AppendFormat("\n\tRet = %s", FakeEntryInvocation.GetPointer() + 2));

			HLSLStruct const* const ReturnStruct = static_cast<HLSLStruct const*>(EntryPoint->Type);
			for (auto const Definition : ReturnStruct->InnerDefinitions)
			{
				HLSLVariable const* const ReturnStructField = static_cast<HLSLVariable const*>(Definition);
				HLSLSemantic const* const ReturnStructFieldSemantic = static_cast<HLSLSemantic const*>(ReturnStructField->ExprColon);
					
				FakeEntryExternals.AppendFormat("\nlayout(location = %d) out %s VaryingRet%s;", LastUsedSemanticOut, ReturnStructField->Type->Name.CStr(), ReturnStructField->Name.CStr());
				FakeEntryAssigment.AppendFormat("\n\tVaryingRet%s = Ret.%s;", ReturnStructField->Name.CStr(), ReturnStructField->Name.CStr());
				LastUsedSemanticOut += 1;
			}
		}
		else if (EntryPoint->Semantic != nullptr)
		{
			FakeEntryExternals.AppendFormat("\nlayout(location = %d) out %s VaryingRet;", LastUsedSemanticOut, EntryPoint->Name.CStr());
			FakeEntryInvocation = Move(StringBuilder().AppendFormat("\n\tVaryingRet = %s", FakeEntryInvocation.GetPointer() + 2));
		}

		Builder.AppendFormat("\n%s\n\nvoid main()\n{%s\n%s\n%s\n}\n", FakeEntryExternals.GetPointer(), FakeEntryInternals.GetPointer(), FakeEntryInvocation.GetPointer(), FakeEntryAssigment.GetPointer());
		return true;
	}

	bool GLSLGenerator::GenerateShader(StringBuilder& Builder, HLSLScope const* const ShaderParsedData, String const& ShaderEntryName, HRIShaderCrossCompilerTarget const ShaderTargetPlatform)
	{
		Builder.Append(GLSLInsertations::GLSLInsertation, GD_ARRAY_SIZE(GLSLInsertations::GLSLInsertation) - 1);
		for (auto const Definition : ShaderParsedData->InnerDefinitions)
		{
			HLSLType const* const Type = HLSLDynamicCast<HLSLType const*>(Definition);
			if (Type != nullptr)
			{
				if (!self->GenerateShaderStruct(static_cast<HLSLStruct const*>(Type), Builder))
					return false;
				continue;
			}

			HLSLCBuffer const* const ConstantBuffer = HLSLDynamicCast<HLSLCBuffer const*>(Definition);
			if (ConstantBuffer != nullptr)
			{
				if (!self->GenerateShaderConstantBuffer(ConstantBuffer, Builder, ShaderTargetPlatform != GD_HRI_SHADERCC_COMPILER_TARGET_GLSLES2))
					return false;
				continue;
			}

			HLSLVariable const* const StaticVariable = HLSLDynamicCast<HLSLVariable const*>(Definition);
			if (StaticVariable != nullptr)
			{
				if (!self->GenerateShaderStaticVariable(StaticVariable, Builder))
					return false;
				continue;
			}

			HLSLFunction const* const StaticFunction = HLSLDynamicCast<HLSLFunction const*>(Definition);
			if (StaticFunction != nullptr)
			{
				if (!self->GenerateShaderStaticFunction(StaticFunction, Builder))
					return false;
				continue;
			}
		}

		if (!self->GenerateShaderEntry(ShaderParsedData->FindFunction(ShaderEntryName), Builder))
			return false;

		return true;
	}

	bool GLSLCompiler::GenerateAndCompileShader(
		OutputStream                      * const ShaderByteCodeOutputStream,
		HLSLScope                    const* const ShaderParsedData,
		HRIShaderType                const        ShaderType,
		String                       const&       ShaderEntryName,
		HRIShaderCrossCompilerTarget const        ShaderTargetPlatform
	)
	{
		StringBuilder GLSLGeneratorBuilder;
		if (!GLSLGenerator(self->Toolchain).GenerateShader(GLSLGeneratorBuilder, ShaderParsedData, ShaderEntryName, ShaderTargetPlatform))
			return false;

		// Now we need just preprocess generated code to reduñe loading time.
		StringBuilder GLSLPreprocessedBuilder;
		GLSLPreprocessedBuilder.Append(GLSLInsertations::GLSLCopyright, GD_ARRAY_SIZE(GLSLInsertations::GLSLCopyright) - 1);
		switch (ShaderTargetPlatform)
		{
		case GD_HRI_SHADERCC_COMPILER_TARGET_GLSL430:
			GLSLPreprocessedBuilder.Append(GLSLInsertations::GLSL430Preambule, GD_ARRAY_SIZE(GLSLInsertations::GLSL430Preambule) - 1);
			break;

		case GD_HRI_SHADERCC_COMPILER_TARGET_GLSLES3:
			GLSLPreprocessedBuilder.Append(GLSLInsertations::GLSLES3Preambule, GD_ARRAY_SIZE(GLSLInsertations::GLSLES3Preambule) - 1);
			break;

		case GD_HRI_SHADERCC_COMPILER_TARGET_GLSLES2:
			GLSLPreprocessedBuilder.Append(GLSLInsertations::GLSLES2Preambule, GD_ARRAY_SIZE(GLSLInsertations::GLSLES2Preambule) - 1);
			break;
		}

		// We use mcpp preprocessor because it is faster than our one.
		String const MCPPSourceFilePath = Path::GetTemporaryFileName();
		String const MCPPOutputFilePath = Path::GetTemporaryFileName();
		Str    const MCPPArguments[]    = { nullptr, "-P", MCPPSourceFilePath.CStr(), MCPPOutputFilePath.CStr() };

		// Saving output to some temporary file to make is accessable from MCPP
		FileOutputStream MCPPSourceFile(MCPPSourceFilePath);
		MCPPSourceFile.WriteCheck(GLSLGeneratorBuilder.GetPointer(), 0, GLSLGeneratorBuilder.GetSize() * sizeof(Char));
		MCPPSourceFile.Dispose();

		{	// mcpp is not thread-safe.
			CriticalSection static MCPPCriticalSection;
			ScopedLock MCPPLock(MCPPCriticalSection);
			int const  MCPPResult = mcpp_lib_main(GD_ARRAY_SIZE(MCPPArguments), const_cast<char**>(&MCPPArguments[0]));
			GD_DEBUG_ASSERT(MCPPResult == 0, "Failed to preprocess shader source.");
		}

		// Reading mcpp result.
		FileInputStream MCPPOutputFile(MCPPOutputFilePath);
		size_t const GLSLPreprocessedBuilderPos = GLSLPreprocessedBuilder.GetSize();
		GLSLPreprocessedBuilder.Resize(GLSLPreprocessedBuilder.GetSize() + MCPPOutputFile.GetLength());
		MCPPOutputFile.ReadCheck(GLSLPreprocessedBuilder.GetPointer() + GLSLPreprocessedBuilderPos, 0, MCPPOutputFile.GetLength());
		MCPPOutputFile.Dispose();

		GD_DEBUG_ASSERT(remove(MCPPSourceFilePath.CStr()) == 0, "Failed to remove mcpp source file.");
		GD_DEBUG_ASSERT(remove(MCPPOutputFilePath.CStr()) == 0, "Failed to remove mcpp output file.");
		
		StringBuilder GLSLOptmizerBuilder;
#if (!defined(GD_HRI_SHADERCC_GLSL_COMPILER_DUAL_CHECK))
		bool WasOptimized = false;	// Trying to optimize out shader using glsl_optimizer by aras-p.
#endif	// if (!defined(GD_HRI_SHADERCC_GLSL_COMPILER_DUAL_CHECK))
		if (ShaderTargetPlatform != GD_HRI_SHADERCC_COMPILER_TARGET_GLSL430)
		{	// glsl_optimizer supports only desktop GLSL 140 version. We are using 430...
			// glsl_optimizer requires context for each thread.
			if ((ShaderTargetPlatform != GD_HRI_SHADER_TYPE_VERTEX) && (ShaderTargetPlatform != GD_HRI_SHADER_TYPE_FRAGMENT))
			{
				GLSLCompilerErrorDesc static const InvalidShaderTypeError("GLSL(ES) supports only Vertex and Fragment shaders.");
				self->RaiseError(InvalidShaderTypeError);
				self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

				return false;
			}

			glslopt_ctx   * const OptimizerContext = glslopt_initialize(((ShaderTargetPlatform == GD_HRI_SHADERCC_COMPILER_TARGET_GLSLES3) ? kGlslTargetOpenGLES30 : kGlslTargetOpenGLES20));
			glslopt_shader* const OptimizedShader  = glslopt_optimize(OptimizerContext, ((ShaderTargetPlatform == GD_HRI_SHADER_TYPE_VERTEX) ? kGlslOptShaderVertex : kGlslOptShaderFragment), GLSLPreprocessedBuilder.GetPointer(), 0);
			if (!glslopt_get_status(OptimizedShader))
			{
				Str const ShaderOptimizationLog = glslopt_get_log(OptimizedShader);
				GLSLCompilerWarningDesc static const OptimizerFailedError("shader optmization failed with following log: \n%s");
				self->RaiseWarning(OptimizerFailedError, ShaderOptimizationLog);

				GLSLOptmizerBuilder = Move(GLSLPreprocessedBuilder);
			}
			else
			{
#if (!defined(GD_HRI_SHADERCC_GLSL_COMPILER_DUAL_CHECK))
				WasOptimized = true;
#endif	// if (!defined(GD_HRI_SHADERCC_GLSL_COMPILER_DUAL_CHECK))
				GLSLOptmizerBuilder.Append(glslopt_get_output(OptimizedShader));
			}

			glslopt_shader_delete(OptimizedShader);
			glslopt_cleanup(OptimizerContext);
		}
		else
			GLSLOptmizerBuilder = Move(GLSLPreprocessedBuilder);

		// No we need to validate our shader. 
#if (!defined(GD_HRI_SHADERCC_GLSL_COMPILER_DUAL_CHECK))
		if (!WasOptimized)
#endif	// if (!defined(GD_HRI_SHADERCC_GLSL_COMPILER_DUAL_CHECK))
		{	// If shader was optmized validation is required only if dual check was specified - it was already validated by optimizer.
			struct glslangInitializerType final
			{	// glslang is thread safe. We can statically initialize it.
				TBuiltInResource glslangDefaultResources;
				GDINT ~glslangInitializerType() { glslang::FinalizeProcess(); }
				GDINT  glslangInitializerType()
				{ 
					GD_ASSERT(glslang::InitializeProcess(), "Failed to initialize glslang."); 
					memset(&glslangDefaultResources, 0x01, sizeof(glslangDefaultResources));
				}
			} static const glslangInitializer;

			static EShLanguage const HRI2GLSLangShaderType[] = {
				/* GD_HRI_SHADER_TYPE_COMPUTE                 = */ EShLangCompute,        
				/* GD_HRI_SHADER_TYPE_GEOMETRY                = */ EShLangGeometry,       
				/* GD_HRI_SHADER_TYPE_VERTEX                  = */ EShLangVertex,         
				/* GD_HRI_SHADER_TYPE_TESSELLATION_CONTROL    = */ EShLangTessControl,    
				/* GD_HRI_SHADER_TYPE_TESSELLATION_EVALUATION = */ EShLangTessEvaluation, 
				/* GD_HRI_SHADER_TYPE_FRAGMENT                = */ EShLangFragment,
			};

			char const* const GLSLOptmizerBuilderPtr = GLSLOptmizerBuilder.GetPointer();
			glslang::TShader glslangShader(HRI2GLSLangShaderType[ShaderType]);
			glslangShader.setStrings(&GLSLOptmizerBuilderPtr, 1);
			if (!glslangShader.parse(&glslangInitializer.glslangDefaultResources, 100, true, EShMsgDefault))
			{	// Failed to compile our shader.
				GLSLCompilerErrorDesc static const glslangCompilationFailed("glslang compilation returned errors: \n%s");
				self->RaiseError(glslangCompilationFailed, glslangShader.getInfoLog());
				self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

				return false;
			}
			else
				self->RaiseMessage(glslangShader.getInfoLog());

			glslang::TProgram glslangProgram;
			glslangProgram.addShader(&glslangShader);
			if (!glslangProgram.link(EShMsgDefault))
			{	// Failed to link our shader.
				GLSLCompilerErrorDesc static const glslangLinkingFailed("glslang linking returned errors: \n%s");
				self->RaiseError(glslangLinkingFailed, glslangProgram.getInfoLog());
				self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

				return false;
			}
			else
				self->RaiseMessage(glslangProgram.getInfoLog());
		}

		size_t const TotalShaderBytecodeSizePredicted = GLSLOptmizerBuilder.GetSize() * sizeof(GLSLOptmizerBuilder.GetPointer()[0]);
		size_t const TotalShaderBytecodeSizeWritten = ShaderByteCodeOutputStream->Write(GLSLOptmizerBuilder.GetPointer(), 0, TotalShaderBytecodeSizePredicted);
		GD_ASSERT(TotalShaderBytecodeSizeWritten == TotalShaderBytecodeSizePredicted, "Failed to write shader to output");
		return true;
	}

GD_NAMESPACE_END
