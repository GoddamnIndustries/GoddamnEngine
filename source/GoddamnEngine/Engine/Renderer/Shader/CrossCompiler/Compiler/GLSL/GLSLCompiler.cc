//////////////////////////////////////////////////////////////////////////
/// GLSLCompiler.hh: GLSL compiler generator and compiler implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 13.06.2014 - Created by James Jhuighuy.
//////////////////////////////////////////////////////////////////////////

#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/Compiler/GLSL/GLSLCompiler.hh>
#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/Parser/Parser.hh>
#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/CrossCompiler.hh>

// We have Warning 4 level (Wx), but glslang has W3 -Wx.
#if (defined(GD_COMPILER_MSC))
#	pragma warning(push, 0)
#endif	// if (defined(GD_COMPILER_MSC))

#include <glsl_optimizer.h>
#include <glslang.h>

#if (defined(GD_COMPILER_MSC))
#	pragma warning(pop)
#endif	// if (defined(GD_COMPILER_MSC))

GD_NAMESPACE_BEGIN

	typedef Str GLSLCompilerErrorDesc;
	typedef Str GLSLCompilerWarningDesc;

	namespace GLSLInsertations
	{
		static Char const* const Copyright = R"(//////////////////////////////////////////////////////////////////////////
/// This file was automatically translated from HLSL by Goddamn Engine.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//////////////////////////////////////////////////////////////////////////
)";

		static Char const* const _420Preambule = R"(
#version 420
#extension GL_ARB_shading_language_420pack : enable
#extension GL_ARB_separate_shader_objects  : enable
)";

		static Char const* const ES3Preambule = R"(
#version 300 es
)";
		static Char const* const ES2Preambule = R"(
#version 200 es
)";

		static Char const* const Insertation = R"(
// ~~~~~~~~~~~~~~~~~~~~~~~~
// Mesa incompetence layer.
// We do not have this keywords in GLSL (or Mesa does not likes them)...
#define static
#define inline
#define const

// ~~~~~~~~~~~~~~~~~~~~~~~~~
// HLSL compatibility layer.
// Types:
// Objects:
#define Sampler2D   sampler2D
#define SamplerCube samplerCube
// Vectors:
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
// Matrices
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

// Math functions:
#define mul(First, Second) (First * Second)
)";
	}	// namespace GLSLInsertations

	class GLSLGenerator final : public IToolchainTool
	{
	public:
		GDINL  GLSLGenerator(IToolchain* const Toolchain) : IToolchainTool(Toolchain) { }
		GDINL ~GLSLGenerator() { }
		GDINT bool GenerateShader(String& Output, HLSLScope const* const Input, String const& EntryName, HRIShaderCrossCompilerTarget const Target);
	private:
		GDINT bool GenerateShaderStruct(HLSLStruct const* const Struct, String& Output);
		GDINT bool GenerateShaderConstantBuffer(HLSLCBuffer const* const ConstantBuffer, String& Output, bool const SupportsConstantBuffers);
		GDINT bool GenerateShaderStaticVariable(HLSLVariable const* const StaticVariable, String& Output);
		GDINT bool GenerateShaderStaticFunction(HLSLFunction const* const StaticFunction, String& Output);
		GDINT bool GenerateShaderEntry(HLSLFunction const* const EntryPoint, String& Output);
	};	// class GLSLGenerator

	bool GLSLGenerator::GenerateShaderStruct(HLSLStruct const* const Struct, String& Output)
	{
		Output += String::Format("\n\nstruct %s\n{", &Struct->Name[0]);
		for (auto const Definition : Struct->InnerDefinitions)
		{
			HLSLVariable const* const Field = static_cast<HLSLVariable const*>(Definition);
			Output += String::Format("\n\t%s %s;", &Field->Type->Name[0], &Field->Name[0]);
		}

		Output += "\n};";
		return true;
	}

	bool GLSLGenerator::GenerateShaderConstantBuffer(HLSLCBuffer const* const ConstantBuffer, String& Output, bool const SupportsConstantBuffers)
	{
		if (ConstantBuffer->Register == nullptr)
		{
			GLSLCompilerErrorDesc static const ExplicitConstantBufferRegisterNotSpecified("constant buffer 's' does not contains meta information about register.");
			self->RaiseError(ExplicitConstantBufferRegisterNotSpecified, &ConstantBuffer->Name[0]);
			self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

			return false;
		}

		if (SupportsConstantBuffers)
			Output += String::Format("\n\nlayout(std140, row_major, binding=%d) uniform %s \n{", static_cast<int>(ConstantBuffer->Register->RegisterID), &ConstantBuffer->Name[0]);

		for (auto const Definition : ConstantBuffer->InnerDefinitions)
		{
			HLSLVariable const* const Field = static_cast<HLSLVariable const*>(Definition);
			if (SupportsConstantBuffers)
				Output += String::Format("\n\t%s %s;", &Field->Type->Name[0], &Field->Name[0]);
			else
				Output += String::Format("\nlayout(row_major) uniform %s %s;", &Field->Type->Name[0], &Field->Name[0]);
		}

		if (SupportsConstantBuffers)
			Output += "\n};";

		return true;
	}

	bool GLSLGenerator::GenerateShaderStaticVariable(HLSLVariable const* const StaticVariable, String& Output)
	{
		if (StaticVariable->Type->Class == GD_HLSL_TYPE_CLASS_SAMPLER)	// Skipping samplers.
			return true;

		if (StaticVariable->ExprColon != nullptr)
		{
			HLSLRegister const* const StaticVariableRegister =  static_cast<HLSLRegister const*>(StaticVariable->ExprColon);
			Output += String::Format("\n\nlayout(binding=%d) uniform", static_cast<int>(StaticVariableRegister->RegisterID));
		}
		else
			Output += "\n\n";

		Output += String::Format("%s %s;", &StaticVariable->Type->Name[0], &StaticVariable->Name[0]);
		return true;
	}

	bool GLSLGenerator::GenerateShaderStaticFunction(HLSLFunction const* const StaticFunction, String& Output)
	{
		if (StaticFunction->Type->Name == "main")
		{
			GLSLCompilerErrorDesc static const MainFunctionRedifinionError("it is not allowed to name a function 'main'.");
			self->RaiseError(MainFunctionRedifinionError);
			self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

			return false;
		}

		Output += String::Format("\n\ninline %s %s(", &StaticFunction->Type->Name[0], &StaticFunction->Name[0]);
		for (auto const Argument : StaticFunction->Arguments)
			Output += String::Format("%s%s %s %s, ",
			(((Argument->AccsessType & GD_HLSL_ARGUMENT_IN ) != 0) ? "in"  : ""),
			(((Argument->AccsessType & GD_HLSL_ARGUMENT_OUT) != 0) ? "out" : ""),
			&Argument->Type->Name[0], &Argument->Name[0]);
		Output.PopLast();
		Output.SetLastElement(')');
		Output += String::Format("\n%s", &StaticFunction->Body[0]);
		return true;
	}

	bool GLSLGenerator::GenerateShaderEntry(HLSLFunction const* const EntryPoint, String& Output)
	{
		String FakeEntryExternals;
		String FakeEntryInternals;
		String FakeEntryInvocation;
		String FakeEntryAssigmentIn;
		String FakeEntryAssigmentOut;
		int LastUsedSemanticIn = 0, LastUsedSemanticOut = 0;
		for (auto const Argument : EntryPoint->Arguments)
		{
			Str const ArgumentAccessType = ((Argument->AccsessType == GD_HLSL_ARGUMENT_IN) ? "in" : "out");
			int&      LastUsedSemantic   = ((Argument->AccsessType == GD_HLSL_ARGUMENT_IN) ? LastUsedSemanticIn : LastUsedSemanticOut);

			FakeEntryInternals += String::Format("\n\t%s %s;", &Argument->Type->Name[0], &Argument->Name[0]);
			FakeEntryInvocation += String::Format("%s, ", &Argument->Name[0]);
			if (Argument->Type->Class == GD_HLSL_TYPE_CLASS_STRUCT)
			{
				HLSLStruct const* const Struct = static_cast<HLSLStruct const*>(Argument->Type);
				for (auto const Definition : Struct->InnerDefinitions)
				{
					HLSLVariable const* const StructField = static_cast<HLSLVariable const*>(Definition);
					HLSLSemantic const* const StructFieldSemantic = static_cast<HLSLSemantic const*>(StructField->ExprColon);
					if (Argument->AccsessType == GD_HLSL_ARGUMENT_OUT)
					{
						if (StructFieldSemantic->Semantic == GD_HLSL_SEMANTIC_SV_Position)
							FakeEntryAssigmentOut += String::Format("\n\tgl_Position=%s.%s;", &Struct->Name[0], &StructField->Name[0]);

						FakeEntryAssigmentOut += String::Format("\n\tios_%s_%s=%s.%s;", &Struct->Name[0], &StructField->Name[0], &Struct->Name[0], &StructField->Name[0]);
					}
					else
						FakeEntryAssigmentIn += String::Format("\n\t%s.%s=ios_%s_%s;", &Struct->Name[0], &StructField->Name[0], &Struct->Name[0], &StructField->Name[0]);

					FakeEntryExternals += String::Format("\nlayout(location=%d) %s %s ios_%s_%s;", LastUsedSemantic, ArgumentAccessType, &StructField->Type->Name[0], &Struct->Name[0], &StructField->Name[0]);
					LastUsedSemantic += 1;
				}
			}
			else 
			{
				HLSLSemantic const* const ArgumentSemantic = static_cast<HLSLSemantic const*>(Argument->ExprColon);
				if (Argument->AccsessType == GD_HLSL_ARGUMENT_IN)
				{
					if (ArgumentSemantic->Semantic == GD_HLSL_SEMANTIC_SV_Position)
						FakeEntryAssigmentOut += String::Format("\n\tgl_Position=%s;", &Argument->Name[0]);

					FakeEntryAssigmentOut += String::Format("\n\tioa_%s=%s;", &Argument->Name[0], &Argument->Name[0]);
				}
				else
					FakeEntryAssigmentIn += String::Format("\n\t%s=ioa_%s;", &Argument->Name[0], &Argument->Name[0]);

				FakeEntryExternals += String::Format("\nlayout(location=%d) %s %s ioa_%s;", LastUsedSemantic, ArgumentAccessType, &Argument->Type->Name[0], &Argument->Name[0]);
				LastUsedSemantic += 1;
			}
		}

		FakeEntryInvocation.PopLast();	// Removing trailing comma.
		FakeEntryInvocation.PopLast();	// Removing trailing space.
		FakeEntryInvocation = String::Format("\n\t%s(%s);", &EntryPoint->Name[0], &FakeEntryInvocation[0]);
		if (EntryPoint->Semantic != nullptr)
		{
			FakeEntryInternals += String::Format("\n\t %s r%s;", &EntryPoint->Type->Name[0], &EntryPoint->Name[0]);
			FakeEntryInvocation = String::Format("r%s=%s", &EntryPoint->Name[0], &FakeEntryInvocation[0]);

			if (EntryPoint->Type->Class == GD_HLSL_TYPE_CLASS_STRUCT)
			{
				HLSLStruct const* const ReturnStruct = static_cast<HLSLStruct const*>(EntryPoint->Type);
				for (auto const Definition : ReturnStruct->InnerDefinitions)
				{
					HLSLVariable const* const ReturnStructField = static_cast<HLSLVariable const*>(Definition);
					HLSLSemantic const* const ReturnStructFieldSemantic = static_cast<HLSLSemantic const*>(ReturnStructField->ExprColon);
					
					FakeEntryExternals += String::Format("\nlayout(location=%d) out %s iors_%s;", LastUsedSemanticOut, &ReturnStructField->Type->Name[0], &ReturnStructField->Name[0]);
					FakeEntryAssigmentOut += String::Format("\n\tiors_%s=r%s.%s;", &ReturnStructField->Name[0], &EntryPoint->Name[0], &ReturnStructField->Name[0]);
					LastUsedSemanticOut += 1;
				}
			}
			else
			{
				FakeEntryExternals += String::Format("\nlayout(location=%d) out %s ior_%s;", LastUsedSemanticOut, &EntryPoint->Type->Name[0], &EntryPoint->Name[0]);
				FakeEntryAssigmentOut += String::Format("\n\tior_%s=r%s;", &EntryPoint->Name[0], &EntryPoint->Name[0]);
			}
		}

		Output += String::Format("\n%s\n\nvoid main()\n{%s\n%s\n%s\n%s\n}\n", &FakeEntryExternals[0], &FakeEntryInternals[0], &FakeEntryAssigmentIn[0], &FakeEntryInvocation[0], &FakeEntryAssigmentOut[0]);
		return true;
	}

	bool GLSLGenerator::GenerateShader(String& Output, HLSLScope const* const Input, String const& EntryName, HRIShaderCrossCompilerTarget const Target)
	{
		Output = GLSLInsertations::Copyright;
		switch (Target)
		{
		case GD_HRI_SHADERCC_COMPILER_TARGET_GLSL420:
			Output += GLSLInsertations::_420Preambule;
			break;
		case GD_HRI_SHADERCC_COMPILER_TARGET_GLSLES3:
			Output += GLSLInsertations::ES3Preambule;
			break;
		case GD_HRI_SHADERCC_COMPILER_TARGET_GLSLES2:
			Output += GLSLInsertations::ES2Preambule;
			break;
		}
		Output += GLSLInsertations::Insertation;

		for (auto const Definition : Input->InnerDefinitions)
		{
			HLSLType const* const Type = HLSLDynamicCast<HLSLType const*>(Definition);
			if (Type != nullptr)
			{
				if (!self->GenerateShaderStruct(static_cast<HLSLStruct const*>(Type), Output))
					return false;
				continue;
			}

			HLSLCBuffer const* const ConstantBuffer = HLSLDynamicCast<HLSLCBuffer const*>(Definition);
			if (ConstantBuffer != nullptr)
			{
				if (!self->GenerateShaderConstantBuffer(ConstantBuffer, Output, Target != GD_HRI_SHADERCC_COMPILER_TARGET_GLSLES2))
					return false;
				continue;
			}

			HLSLVariable const* const StaticVariable = HLSLDynamicCast<HLSLVariable const*>(Definition);
			if (StaticVariable != nullptr)
			{
				if (!self->GenerateShaderStaticVariable(StaticVariable, Output))
					return false;
				continue;
			}

			HLSLFunction const* const StaticFunction = HLSLDynamicCast<HLSLFunction const*>(Definition);
			if (StaticFunction != nullptr)
			{
				if (!self->GenerateShaderStaticFunction(StaticFunction, Output))
					return false;
				continue;
			}
		}

		if (!self->GenerateShaderEntry(Input->FindFunction(EntryName), Output))
			return false;

		return true;
	}

	bool GLSLCompiler::GenerateAndCompileShader(
		OutputStream                      * const Output,
		HLSLScope                    const* const Input,
		HRIShaderCrossCompilerTarget const        Target,
		HRIShaderType                const        Type,
		String                       const&       EntryName
	)
	{
		String GLSLGeneratorOutput;
		if (!GLSLGenerator(self->Toolchain).GenerateShader(GLSLGeneratorOutput, Input, EntryName, Target))
			return false;

		FILE* f = fopen(R"(D:\GoddamnEngine\source\GoddamnEngine\Engine\_Dependencies\glslang\test.c)", "w");
		fprintf(f, "%s", &GLSLGeneratorOutput[0]);
		fclose(f);

		String GLSLOptmizerOutput;
		bool WasOptimized = false;	// Trying to optimize out shader using glsl_optimizer by aras-p.
		if (Target != GD_HRI_SHADERCC_COMPILER_TARGET_GLSL420)	
		{	// glsl_optimizer supports only desktop GLSL 140 version. We are using 420...
			// glsl_optimizer requires context for each thread.
			if ((Type != GD_HRI_SHADER_TYPE_VERTEX) && (Type != GD_HRI_SHADER_TYPE_FRAGMENT))
			{
				GLSLCompilerErrorDesc static const InvalidShaderTypeError("GLSL(ES) supports only Vertex and Fragment shaders.");
				self->RaiseError(InvalidShaderTypeError);
				self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

				return false;
			}

			glslopt_ctx   * const OptimizerContext = glslopt_initialize(((Target == GD_HRI_SHADERCC_COMPILER_TARGET_GLSLES3) ? kGlslTargetOpenGLES30 : kGlslTargetOpenGLES20));
			glslopt_shader* const OptimizedShader = glslopt_optimize(OptimizerContext, ((Type == GD_HRI_SHADER_TYPE_VERTEX) ? kGlslOptShaderVertex : kGlslOptShaderFragment), &GLSLGeneratorOutput[0], 0);
			if (!glslopt_get_status(OptimizedShader))
			{
				Str const ShaderOptimizationLog = glslopt_get_log(OptimizedShader);
				GLSLCompilerWarningDesc static const NoExplicitRegisterInformationWarning("shader optmization failed with following log: \n%s");
				self->RaiseWarning(NoExplicitRegisterInformationWarning, ShaderOptimizationLog);

				GLSLOptmizerOutput = Move(GLSLGeneratorOutput);
			}
			else
			{
				WasOptimized = true;
				GLSLOptmizerOutput = glslopt_get_output(OptimizedShader);
			}

			glslopt_shader_delete(OptimizedShader);
			glslopt_cleanup(OptimizerContext);
		}
		else
			GLSLOptmizerOutput = Move(GLSLGeneratorOutput);

		// No we need to validate our shader. 
		if (!WasOptimized)
		{	// If shader was optmized validation is not required - it was already validated by optimizer.
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
				EShLangCompute,        // = GD_HRI_SHADER_TYPE_COMPUTE
				EShLangGeometry,       // = GD_HRI_SHADER_TYPE_GEOMETRY,
				EShLangVertex,         // = GD_HRI_SHADER_TYPE_VERTEX,
				EShLangTessControl,    // = GD_HRI_SHADER_TYPE_TESSELLATION_CONTROL,
				EShLangTessEvaluation, // = GD_HRI_SHADER_TYPE_TESSELLATION_EVALUATION,
				EShLangFragment,       // = GD_HRI_SHADER_TYPE_FRAGMENT,
			};

			char const* const GLSLOptmizerOutputPtr = &GLSLOptmizerOutput[0];
			glslang::TShader glslangShader(HRI2GLSLangShaderType[Type]);
			glslangShader.setStrings(&GLSLOptmizerOutputPtr, 1);
			if (!glslangShader.parse(&glslangInitializer.glslangDefaultResources, 0, true, EShMsgDefault))
			{	// Failed to validate our shader.
				GLSLCompilerErrorDesc static const glslangValidationFailed("glslang returned errors: \n%s");
				printf(glslangValidationFailed, glslangShader.getInfoLog());
				self->RaiseError(glslangValidationFailed, glslangShader.getInfoLog());
				self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);
				 
				return false;
			}
		}

		size_t const TotalShaderBytecodeSizePredicted = GLSLOptmizerOutput.GetSize() * sizeof(GLSLOptmizerOutput[0]);
		size_t const TotalShaderBytecodeSizeWritten = Output->Write(&GLSLOptmizerOutput[0], 0, TotalShaderBytecodeSizePredicted);
		GD_ASSERT(TotalShaderBytecodeSizeWritten == TotalShaderBytecodeSizePredicted, "Failed to write shader to output");
		return true;
	}

GD_NAMESPACE_END
