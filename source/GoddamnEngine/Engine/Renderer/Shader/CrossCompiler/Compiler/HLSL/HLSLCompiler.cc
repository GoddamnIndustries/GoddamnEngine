//////////////////////////////////////////////////////////////////////////
/// HLSLCompiler.cc: HLSL compiler generator and compiler implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 13.06.2014 - Created by James Jhuighuy.
//////////////////////////////////////////////////////////////////////////

#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/Compiler/HLSL/HLSLCompiler.hh>
#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/Parser/Parser.hh>
#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/CrossCompiler.hh>

#include <GoddamnEngine/Core/Reflection/Assembly/Assembly.hh>
#include <GoddamnEngine/Core/Object/RefPtr/RefPtr.hh>
#include <d3dcompiler.h>

GD_NAMESPACE_BEGIN

	typedef Str HLSLCompilerErrorDesc;

#if (!defined(GD_D3D_LINKED))
	static pD3DCompile D3DCompile;
	struct pD3DCompileLoader final 
	{
		GDINT pD3DCompileLoader()
		{
			RefPtr<Assembly const> const D3DCompilerAssembly(Assembly::LoadAssemblyFromPath(D3DCOMPILER_DLL));
			GD_ASSERT(D3DCompilerAssembly != nullptr, "Failed to load '"D3DCOMPILER_DLL"' assembly.");
			
			D3DCompile = reinterpret_cast<pD3DCompile>(D3DCompilerAssembly->GetNativeMethod("D3DCompile"));
			GD_ASSERT(D3DCompile != nullptr, "Failed to load 'D3DCompile' funation from '"D3DCOMPILER_DLL"' assembly.");
		}
	} static const D3DCompileLoader;
#endif	// if (!defined(GD_D3D_LINKED))

	class HLSLGenerator final : public IToolchainTool
	{
	public:
		GDINL  HLSLGenerator(IToolchain* const Toolchain) : IToolchainTool(Toolchain) { }
		GDINL ~HLSLGenerator(                           ) { }
		GDINT bool GenerateShader(String& Output, HLSLScope const* const Input);
	private:
		GDINT void GenerateShaderStruct(HLSLStruct const* const Struct, String& Output);
		GDINT void GenerateShaderConstantBuffer(HLSLCBuffer const* const ConstantBuffer, String& Output);
		GDINT void GenerateShaderStaticVariable(HLSLVariable const* const StaticVariable, String& Output);
		GDINT void GenerateShaderStaticFunction(HLSLFunction const* const StaticFunction, String& Output);
	};	// class HLSLGenerator

	void HLSLGenerator::GenerateShaderStruct(HLSLStruct const* const Struct, String& Output)
	{
		Output += String::Format("\n\nstruct %s\n{", &Struct->Name[0]);
		
		for (auto const Definition : Struct->InnerDefinitions)
		{
			HLSLVariable const* const Field = static_cast<HLSLVariable const*>(Definition);
			Output += String::Format("\n\t%s %s", &Field->Type->Name[0], &Field->Name[0]);
			if (Field->ExprColon != nullptr)
			{
				HLSLSemantic const* const FieldSemantic = static_cast<HLSLSemantic const*>(Field->ExprColon);
				Output += String::Format(" : %s%d;", HLSLSemanticToStr(FieldSemantic->Semantic), static_cast<int>(FieldSemantic->SemanticID));
			}
			else
				Output += ';';
		}

		Output += '}';
	}

	void HLSLGenerator::GenerateShaderConstantBuffer(HLSLCBuffer const* const ConstantBuffer, String& Output)
	{
		Output += String::Format("\n\ncbuffer %s", &ConstantBuffer->Name[0]);

		if (ConstantBuffer->Register != nullptr)
			Output += String::Format(" : register(b%d)\n{", static_cast<int>(ConstantBuffer->Register->RegisterID));
		else
			Output += "\n{";

		for (auto const Definition : ConstantBuffer->InnerDefinitions)
		{
			HLSLVariable const* const Field = static_cast<HLSLVariable const*>(Definition);
			Output += String::Format("\n\t%s %s", &Field->Type->Name[0], &Field->Name[0]);
			if (Field->ExprColon != nullptr)
			{
				//	HLSLPackOffset const* const FieldPackOffset = static_cast<HLSLPackOffset const*>(Field->ExprColon);
				//	Output += String::Format(" : packoffset(c%s%d;)", HLSLSemanticToStr(FieldSemantic->Semantic), static_cast<int>(FieldSemantic->SemanticID));
				GD_NOT_IMPLEMENTED();
			}
			else
				Output += ';';
		}

		Output += '}';
	}

	void HLSLGenerator::GenerateShaderStaticVariable(HLSLVariable const* const StaticVariable, String& Output)
	{
		Output += String::Format("\n\n%s %s", &StaticVariable->Type->Name[0], &StaticVariable->Name[0]);
		if (StaticVariable->ExprColon != nullptr)
		{
			HLSLRegister const* const StaticVariableRegister = static_cast<HLSLRegister const*>(StaticVariable->ExprColon);
			Output += String::Format(" : register(%c%d);", 
				((StaticVariableRegister->Register == GD_HLSL_REGISTER_T) ? 't' : 's'),	// Only 't'/'s' registers can be located here. 
				static_cast<int>(StaticVariableRegister->RegisterID));
		}
		else
			Output += ';';
	}

	void HLSLGenerator::GenerateShaderStaticFunction(HLSLFunction const* const StaticFunction, String& Output)
	{
		Output += String::Format("\n\n%s %s(", &StaticFunction->Type->Name[0], &StaticFunction->Name[0]);
		for (auto const Argument : StaticFunction->Arguments)
			Output += String::Format("%s%s %s %s, ", 
				(((Argument->AccsessType & GD_HLSL_ARGUMENT_IN ) != 0) ? "in"  : ""),
				(((Argument->AccsessType & GD_HLSL_ARGUMENT_OUT) != 0) ? "out" : ""),
				&Argument->Type->Name[0], &Argument->Name[0]);
		Output.PopLast();
		Output.SetLastElement(')');

		if (StaticFunction->Semantic != nullptr)
			Output += String::Format(" : %s%d", HLSLSemanticToStr(StaticFunction->Semantic->Semantic), static_cast<int>(StaticFunction->Semantic->SemanticID));
		Output += String::Format("\n%s", &StaticFunction->Body[0]);
	}

	bool HLSLGenerator::GenerateShader(String& Output, HLSLScope const* const Input)
	{
		for (auto const Definition : Input->InnerDefinitions)
		{
			HLSLType const* const Type = HLSLDynamicCast<HLSLType const*>(Definition);
			if (Type != nullptr)
			{
				self->GenerateShaderStruct(static_cast<HLSLStruct const*>(Type), Output);
				continue;
			}

			HLSLCBuffer const* const ConstantBuffer = HLSLDynamicCast<HLSLCBuffer const*>(Definition);
			if (ConstantBuffer != nullptr)
			{
				self->GenerateShaderConstantBuffer(ConstantBuffer, Output);
				continue;
			}

			HLSLVariable const* const StaticVariable = HLSLDynamicCast<HLSLVariable const*>(Definition);
			if (StaticVariable != nullptr)
			{
				self->GenerateShaderStaticVariable(StaticVariable, Output);
				continue;
			}

			HLSLFunction const* const StaticFunction = HLSLDynamicCast<HLSLFunction const*>(Definition);
			if (StaticFunction != nullptr)
			{
				self->GenerateShaderStaticFunction(StaticFunction, Output);
				continue;
			}
		}

		return true;
	}

	bool HLSLCompiler::GenerateAndCompileShader(
		OutputStream                      * const Output,
		HLSLScope                    const* const Input,
		HRIShaderType                const        Type,
		String                       const&       EntryName
	)
	{
		String HLSLGeneratorOutput;
		if (!HLSLGenerator(self->Toolchain).GenerateShader(HLSLGeneratorOutput, Input))
			return false;

		UINT D3DCompileFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if (defined(GD_DEBUG))
		D3DCompileFlags |= D3DCOMPILE_DEBUG;
#else	// if (defined(GD_DEBUG))
		D3DCompileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif	// if (defined(GD_DEBUG))
		if (self->Toolchain->DoTreatWarningsAsError())
			D3DCompileFlags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;

		LPCSTR D3DCompileProfile = nullptr;
		switch (Type)
		{
		case GD_HRI_SHADER_TYPE_COMPUTE:
			D3DCompileProfile = "cs_5_0";
			break;

		case GD_HRI_SHADER_TYPE_GEOMETRY:
			D3DCompileProfile = "gs_5_0";
			break;

		case GD_HRI_SHADER_TYPE_VERTEX:
			D3DCompileProfile = "vs_5_0";
			break;

		case GD_HRI_SHADER_TYPE_HULL:
			D3DCompileProfile = "hs_5_0";
			break;

		case GD_HRI_SHADER_TYPE_DOMAIN:
			D3DCompileProfile = "ds_5_0";
			break;

		case GD_HRI_SHADER_TYPE_PIXEL:
			D3DCompileProfile = "ps_5_0";
			break;

		default:
			GD_ASSERT_FALSE("Invalid shader type specified");
			return false;
		}

		ID3DBlob *OutputBlob = nullptr, *ErrorBlob = nullptr;	// Note: D3DCompile is thread-safe.
		HRESULT const Result = D3DCompile(&HLSLGeneratorOutput[0], HLSLGeneratorOutput.GetSize(), 
			nullptr,  nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, &EntryName[0],
			D3DCompileProfile, D3DCompileFlags, 0, &OutputBlob, &ErrorBlob
		);

		if (FAILED(Result))
		{
			if (ErrorBlob != nullptr)
			{
				String const CompilationErrorsList = reinterpret_cast<Str>(ErrorBlob->GetBufferPointer());
				size_t CompilationErrorIndex = strstr(&CompilationErrorsList[0], "(") - (&CompilationErrorsList[0]);
				for (auto const& CompilationError : CompilationErrorsList.Split('\n'))
				{
					Str const CompilationErrorWithoutFile = CompilationError.Begin() + CompilationErrorIndex;
					HLSLCompilerErrorDesc static const D3DCompilerError("D3DCompile returned error: '%s'");
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

		size_t const TotalShaderBytecodeSizeWritten = Output->Write(OutputBlob->GetBufferPointer(), 0, OutputBlob->GetBufferSize());
		GD_ASSERT(TotalShaderBytecodeSizeWritten == OutputBlob->GetBufferSize(), "Failed to write shader to output");
		return true;
	}

GD_NAMESPACE_END
