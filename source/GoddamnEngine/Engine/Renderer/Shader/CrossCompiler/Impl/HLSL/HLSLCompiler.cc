//////////////////////////////////////////////////////////////////////////
/// HLSLCompiler.cc: HLSL compiler generator and compiler implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 13.06.2014 - Created by James Jhuighuy.
//////////////////////////////////////////////////////////////////////////

#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/Impl/HLSL/HLSLCompiler.hh>
#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/Parser/Parser.hh>
#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/CrossCompiler.hh>

#include <GoddamnEngine/Core/Text/StringBuilder/StringBuilder.hh>
#include <GoddamnEngine/Core/Reflection/Assembly/Assembly.hh>
#include <d3dcompiler.h>

GD_NAMESPACE_BEGIN

	typedef Str HLSLCompilerErrorDesc;

#define GD_D3D_LINKED

#if (!defined(GD_D3D_LINKED))
	static pD3DCompile D3DCompile;
	struct pD3DCompileLoader final 
	{
		GDINT pD3DCompileLoader()
		{
			RefPtr<Assembly const> const D3DCompilerAssembly(Assembly::LoadAssemblyFromPath(D3DCOMPILER_DLL));
			GD_ASSERT(D3DCompilerAssembly != nullptr, "Failed to load '"D3DCOMPILER_DLL"' assembly.");
			
			D3DCompile = reinterpret_cast<pD3DCompile>(D3DCompilerAssembly->GetNativeMethod("D3DCompile"));
			GD_ASSERT(D3DCompile != nullptr, "Failed to load 'D3DCompile' function from '"D3DCOMPILER_DLL"' assembly.");
		}
	} static const D3DCompileLoader;
#endif	// if (!defined(GD_D3D_LINKED))

	class HLSLGenerator final : public IToolchainTool
	{
	public:
		GDINL  HLSLGenerator(IToolchain* const Toolchain) : IToolchainTool(Toolchain) { }
		GDINL ~HLSLGenerator(                           ) { }
		GDINT void GenerateShader(StringBuilder& Builder, HLSLScope const* const ShaderParsedData);
	private:
		GDINT void GenerateShaderStruct(HLSLStruct const* const Struct, StringBuilder& Builder);
		GDINT void GenerateShaderConstantBuffer(HLSLCBuffer const* const ConstantBuffer, StringBuilder& Builder);
		GDINT void GenerateShaderStaticVariable(HLSLVariable const* const StaticVariable, StringBuilder& Builder);
		GDINT void GenerateShaderStaticFunction(HLSLFunction const* const StaticFunction, StringBuilder& Builder);
	};	// class HLSLGenerator

	void HLSLGenerator::GenerateShaderStruct(HLSLStruct const* const Struct, StringBuilder& Builder)
	{
		Builder.AppendFormat("\n\nstruct %s\n{", Struct->Name.CStr());
		
		for (auto const Definition : Struct->InnerDefinitions)
		{
			HLSLVariable const* const StructField = static_cast<HLSLVariable const*>(Definition);
			Builder.AppendFormat("\n\t%s %s", StructField->Type->Name.CStr(), StructField->Name.CStr());
			/**/ if (StructField->ExprColon != nullptr)
			{
				HLSLSemantic const* const StructFieldSemantic = static_cast<HLSLSemantic const*>(StructField->ExprColon);
				Builder.AppendFormat(" : %s%d", HLSLSemanticToStr(StructFieldSemantic->Semantic), static_cast<int>(StructFieldSemantic->SemanticID));
			}
			else if (StructField->ArrayIndex != 0)
				Builder.AppendFormat("[%d]", static_cast<int>(StructField->ArrayIndex));

			Builder.Append(';');
		}

		Builder.Append("\n};");
	}

	void HLSLGenerator::GenerateShaderConstantBuffer(HLSLCBuffer const* const ConstantBuffer, StringBuilder& Builder)
	{
		Builder.AppendFormat("\n\ncbuffer %s", ConstantBuffer->Name.CStr());
		if (ConstantBuffer->Register != nullptr)
			Builder.Append(" : register(b%d)", static_cast<int>(ConstantBuffer->Register->RegisterID));
			
		Builder.Append("\n{");

		for (auto const Definition : ConstantBuffer->InnerDefinitions)
		{
			HLSLVariable const* const ConstantBufferField = static_cast<HLSLVariable const*>(Definition);
			Builder.AppendFormat("\n\t%s %s", ConstantBufferField->Type->Name.CStr(), ConstantBufferField->Name.CStr());
			
			if (ConstantBufferField->ArrayIndex != 0)
				Builder.AppendFormat("[%d]", static_cast<int>(ConstantBufferField->ArrayIndex));

			if (ConstantBufferField->ExprColon != nullptr)
			{
				//	HLSLPackOffset const* const FieldPackOffset = static_cast<HLSLPackOffset const*>(Field->ExprColon);
				//	Builder.Append(" : packoffset(c%s%d;)", HLSLSemanticToStr(FieldSemantic->Semantic), static_cast<int>(FieldSemantic->SemanticID));
				GD_NOT_IMPLEMENTED();
			}

			Builder.Append(';');
		}

		Builder.Append("\n}");
	}

	void HLSLGenerator::GenerateShaderStaticVariable(HLSLVariable const* const StaticVariable, StringBuilder& Builder)
	{
		Builder.AppendFormat("\n\n%s %s", StaticVariable->Type->Name.CStr(), StaticVariable->Name.CStr());
		/**/ if (StaticVariable->ArrayIndex != 0)
			Builder.AppendFormat("[%d]", static_cast<int>(StaticVariable->ArrayIndex));
		else if (StaticVariable->ExprColon != nullptr)
		{
			HLSLRegister const* const StaticVariableRegister = static_cast<HLSLRegister const*>(StaticVariable->ExprColon);
			Builder.AppendFormat(" : register(%c%d)", ((StaticVariableRegister->Register == GD_HLSL_REGISTER_T) ? 't' : 's'),	// Only 't'/'s' registers can be located here. 
										static_cast<int>(StaticVariableRegister->RegisterID));
		}

		Builder.Append(';');
	}

	void HLSLGenerator::GenerateShaderStaticFunction(HLSLFunction const* const StaticFunction, StringBuilder& Builder)
	{
		Builder.AppendFormat("\n\n%s %s(", StaticFunction->Type->Name.CStr(), StaticFunction->Name.CStr());
		for (auto const Argument : StaticFunction->Arguments)
		{
			Builder.AppendFormat("%s%s %s %s",
				(((Argument->AccsessType & GD_HLSL_ARGUMENT_IN ) != 0) ? "in"  : ""),
				(((Argument->AccsessType & GD_HLSL_ARGUMENT_OUT) != 0) ? "out" : ""),
				Argument->Type->Name.CStr(), Argument->Name.CStr());
			if (Argument->ExprColon != nullptr)
			{
				HLSLSemantic const* const Semantic = static_cast<HLSLSemantic const* const>(Argument->ExprColon);
				Builder.AppendFormat(" : %s%d", HLSLSemanticToStr(Semantic->Semantic), static_cast<int>(Semantic->SemanticID));
			}

			Builder.Append(", ");
		}
		*(Builder.GetPointer() + Builder.GetSize() - 2) = ')';	// Replacing trailing comma.
		*(Builder.GetPointer() + Builder.GetSize() - 1) = ' ';	// Replacing trailing space.

		if (StaticFunction->Semantic != nullptr)
			Builder.AppendFormat(": %s%d", HLSLSemanticToStr(StaticFunction->Semantic->Semantic), static_cast<int>(StaticFunction->Semantic->SemanticID));
		Builder.AppendFormat("\n%s", StaticFunction->Body.CStr());
	}

	void HLSLGenerator::GenerateShader(StringBuilder& Builder, HLSLScope const* const ShaderParsedData)
	{
		for (auto const Definition : ShaderParsedData->InnerDefinitions)
		{
			HLSLType const* const Type = HLSLDynamicCast<HLSLType const*>(Definition);
			if (Type != nullptr)
			{
				self->GenerateShaderStruct(static_cast<HLSLStruct const*>(Type), Builder);
				continue;
			}

			HLSLCBuffer const* const ConstantBuffer = HLSLDynamicCast<HLSLCBuffer const*>(Definition);
			if (ConstantBuffer != nullptr)
			{
				self->GenerateShaderConstantBuffer(ConstantBuffer, Builder);
				continue;
			}

			HLSLVariable const* const StaticVariable = HLSLDynamicCast<HLSLVariable const*>(Definition);
			if (StaticVariable != nullptr)
			{
				self->GenerateShaderStaticVariable(StaticVariable, Builder);
				continue;
			}

			HLSLFunction const* const StaticFunction = HLSLDynamicCast<HLSLFunction const*>(Definition);
			if (StaticFunction != nullptr)
			{
				self->GenerateShaderStaticFunction(StaticFunction, Builder);
				continue;
			}
		}
	}

	bool HLSLCompiler::GenerateAndCompileShader(
		OutputStream                      * const ShaderByteCodeOutputStream,
		HLSLScope                    const* const ShaderParsedData,
		HRIShaderType                const        ShaderType,
		String                       const&       ShaderEntryName
	)
	{
		StringBuilder HLSLGeneratorOutput;
		HLSLGenerator(self->Toolchain).GenerateShader(HLSLGeneratorOutput, ShaderParsedData);

		UINT D3DCompileFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if (defined(GD_DEBUG))
		D3DCompileFlags |= D3DCOMPILE_DEBUG;
#else	// if (defined(GD_DEBUG))
		D3DCompileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif	// if (defined(GD_DEBUG))
		if (self->Toolchain->DoTreatWarningsAsError())
			D3DCompileFlags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;

		LPCSTR static const D3DCompileProfiles[] = {
			/* GD_HRI_SHADER_TYPE_COMPUTE  = */ "cs_5_0",
			/* GD_HRI_SHADER_TYPE_GEOMETRY = */ "gs_5_0",
			/* GD_HRI_SHADER_TYPE_VERTEX   = */ "vs_5_0",
			/* GD_HRI_SHADER_TYPE_HULL     = */ "hs_5_0",
			/* GD_HRI_SHADER_TYPE_DOMAIN   = */ "ds_5_0",
			/* GD_HRI_SHADER_TYPE_PIXEL    = */ "ps_5_0",
		};
		GD_DEBUG_ASSERT(ShaderType < GD_HRI_SHADER_TYPE_UNKNOWN, "Invalid shader type specified.");
		LPCSTR const D3DCompileProfile = D3DCompileProfiles[ShaderType];

		ID3DBlob *OutputBlob = nullptr, *ErrorBlob = nullptr;	// Note: D3DCompile is thread-safe.
		HRESULT const Result = D3DCompile(HLSLGeneratorOutput.GetPointer(), HLSLGeneratorOutput.GetSize(),
			nullptr,  nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, ShaderEntryName.CStr(),
			D3DCompileProfile, D3DCompileFlags, 0, &OutputBlob, &ErrorBlob
		);

		if (FAILED(Result))
		{
			if (ErrorBlob != nullptr)
			{
				String const CompilationErrorsList = reinterpret_cast<Str>(ErrorBlob->GetBufferPointer());
				size_t CompilationErrorIndex = strstr(CompilationErrorsList.CStr(), "(") - (CompilationErrorsList.CStr());
				for (auto const& CompilationError : CompilationErrorsList.Split('\n'))
				{
					Str const CompilationErrorWithoutFile = CompilationError.Begin() + CompilationErrorIndex;
					HLSLCompilerErrorDesc static const D3DCompilerError("D3DCompile returned error: \n%s");
					self->RaiseError(D3DCompilerError, CompilationErrorWithoutFile);
					self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);
				}
			}
			else
			{
				HLSLCompilerErrorDesc static const D3DUnknownError("unknown error occured while compiling shader.");
				self->RaiseError(D3DUnknownError);
				self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);
			}

			return false;
		}

		size_t const TotalShaderBytecodeSizeWritten = ShaderByteCodeOutputStream->Write(OutputBlob->GetBufferPointer(), 0, OutputBlob->GetBufferSize());
		GD_ASSERT(TotalShaderBytecodeSizeWritten == OutputBlob->GetBufferSize(), "Failed to write shader to output");
		return true;
	}

GD_NAMESPACE_END
