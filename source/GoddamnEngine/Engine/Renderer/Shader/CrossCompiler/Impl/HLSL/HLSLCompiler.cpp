/// ==========================================================================================
/// HLSLCompiler.cpp: HLSL compiler generator and compiler implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 13.06.2014 - Created by James Jhuighuy.
/// ==========================================================================================

#include <GoddamnEngine/Include.h>
#if (defined(GD_PLATFORM_WINDOWS))

#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/Impl/HLSL/HLSLCompiler.h>
#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/Parser/Parser.h>
#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/CrossCompiler.h>
#include <GoddamnEngine/Core/Text/StringBuilder/StringBuilder.h>
#if (!defined(GD_D3D_LINKED))
#	include <GoddamnEngine/Core/Reflection/Assembly/Assembly.h>
#	include <cstdio>
#	if (defined(GD_PLATFORM_API_WINAPI))
#		define popen _popen
#		define pclose _pclose
#	endif	// if (defined(GD_PLATFORM_API_WINAPI))
#endif	// if (!defined(GD_D3D_LINKED))
#include <d3dcompiler.h>

GD_NAMESPACE_BEGIN

	/// Represents compiler error description.
	struct HLSLCompilerErrorDesc final : public ToolchainMessageDesc
	{
	public:
		GDINL explicit HLSLCompilerErrorDesc(String const& Message) : ToolchainMessageDesc(Message) { }
	};	// struct StreamedLexerErrorDesc

	/// Compiling exception.
	GD_DEFINE_EXCPETION_DERIVED_FINAL(HLSLCompilerErrorException, ToolchainException);

#if (!defined(GD_D3D_LINKED))
	static pD3DCompile D3DCompile;
	struct pD3DCompileLoader final 
	{
		UniquePtr<Assembly const> D3DCompilerAssembly;
		GDINT pD3DCompileLoader()
		{	// Trying to load compile function from standart D3D compiler DLL.
			if ((this->D3DCompilerAssembly = Assembly::LoadAssemblyFromPath(D3DCOMPILER_DLL)) != nullptr) {
				if ((D3DCompile = reinterpret_cast<pD3DCompile>(D3DCompilerAssembly->GetNativeMethod("D3DCompile"))) == nullptr) {
					this->D3DCompilerAssembly = nullptr;
					Debug::Error("Failed to load 'D3DCompile' function from '"D3DCOMPILER_DLL"' assembly.");
				}

				return;
			} else {
				Debug::Error("Failed to load '"D3DCOMPILER_DLL"' assembly.");
			}

			// No compiler DLL or compile method was found. Lets emulate it via call to 'fxc.exe' utility.
			D3DCompile = [](
				LPCVOID                         pSrcData,
				SIZE_T                          SrcDataSize,
				LPCSTR                          pFileName,
				CONST D3D_SHADER_MACRO*         pDefines,
				ID3DInclude*                    pInclude,
				LPCSTR                          pEntrypoint,
				LPCSTR                          pTarget,
				UINT                            Flags1,
				UINT                            Flags2,
				ID3DBlob**                      ppCode,
				ID3DBlob**                      ppErrorMsgs) -> HRESULT
			{
#if (defined(GD_PLATFORM_64BIT))
				static char const FXCPath[] = R"("%DXSDK_DIR%Utilities\bin\x64\fxc.exe ")";
#else	// if (defined(GD_PLATFORM_64BIT))
				static char const FXCPath[] = R"("%DXSDK_DIR%Utilities\bin\x68\fxc.exe ")";
#endif	// if (defined(GD_PLATFORM_64BIT))
				
				// Building command.
				StringBuilder FXCCommandBuilder;
				FXCCommandBuilder.Append(FXCPath);

				GD_NOT_IMPLEMENTED();
#if 0
				FILE* const FXCPipe = ::popen(FXCCommandBuilder.GetPointer(), "r");
				if (FXCPipe != nullptr) {
					int const FXCExecutionResult = ::pclose(FXCPipe);
					if (FXCExecutionResult == 0) {
						return S_OK;
					}
				}

				return E_FAIL;
#endif	// if 0
			};
		}
	} static const D3DCompileLoader;
#endif	// if (!defined(GD_D3D_LINKED))

	/// Provides generation of D3D HLSL code.
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

	/// ==========================================================================================
	/// HLSLGenerator class.
	/// Provides generation of D3D HLSL code.
	/// ==========================================================================================

	/// ------------------------------------------------------------------------------------------
	/// Public class API:
	/// ------------------------------------------------------------------------------------------

	void HLSLGenerator::GenerateShader(StringBuilder& Builder, HLSLScope const* const ShaderParsedData)
	{
		for (auto const& Definition : ShaderParsedData->InnerDefinitions) {	// Enumerating all definitions in shader.
			HLSLType const* const Type = HLSLDynamicCast<HLSLType const*>(Definition);
			if (Type != nullptr) {
				this->GenerateShaderStruct(static_cast<HLSLStruct const*>(Type), Builder);
			} else {	// This definition is not a type.
				HLSLCBuffer const* const ConstantBuffer = HLSLDynamicCast<HLSLCBuffer const*>(Definition);
				if (ConstantBuffer != nullptr) {
					this->GenerateShaderConstantBuffer(ConstantBuffer, Builder);
				} else {	// This definition is not a constant buffer.
					HLSLVariable const* const StaticVariable = HLSLDynamicCast<HLSLVariable const*>(Definition);
					if (StaticVariable != nullptr) {
						this->GenerateShaderStaticVariable(StaticVariable, Builder);
					} else {	// This definition is not a static variable.
						HLSLFunction const* const StaticFunction = HLSLDynamicCast<HLSLFunction const*>(Definition);
						if (StaticFunction != nullptr) {	// This definition is static function.
							this->GenerateShaderStaticFunction(StaticFunction, Builder);
						}
					}
				}
			}
		}
	}


	/// ------------------------------------------------------------------------------------------
	/// Internal class API:
	/// ------------------------------------------------------------------------------------------

	void HLSLGenerator::GenerateShaderStruct(HLSLStruct const* const Struct, StringBuilder& Builder)
	{
		Builder.AppendFormat("\n\nstruct %s\n{", Struct->Name.CStr());
		
		for (auto const& Definition : Struct->InnerDefinitions) {
			HLSLVariable const* const StructField = static_cast<HLSLVariable const*>(Definition);
			Builder.AppendFormat("\n\t%s %s", StructField->Type->Name.CStr(), StructField->Name.CStr());
			if (StructField->ExprColon != nullptr) {
				HLSLSemantic const* const StructFieldSemantic = static_cast<HLSLSemantic const*>(StructField->ExprColon);
				Builder.AppendFormat(" : %s%d", HLSLSemanticToStr(StructFieldSemantic->Semantic), static_cast<int>(StructFieldSemantic->SemanticID));
			} else if (StructField->ArrayIndex != 0) {
				Builder.AppendFormat("[%d]", static_cast<int>(StructField->ArrayIndex));
			}

			Builder.Append(';');
		}

		Builder.Append("\n};");
	}

	void HLSLGenerator::GenerateShaderConstantBuffer(HLSLCBuffer const* const ConstantBuffer, StringBuilder& Builder)
	{
		Builder.AppendFormat("\n\ncbuffer %s", ConstantBuffer->Name.CStr());
		if (ConstantBuffer->Register != nullptr) {
			Builder.Append(" : register(b%d)", static_cast<int>(ConstantBuffer->Register->RegisterID));
		}

		Builder.Append("\n{");

		for (auto const& Definition : ConstantBuffer->InnerDefinitions)	{
			HLSLVariable const* const ConstantBufferField = static_cast<HLSLVariable const*>(Definition);
			Builder.AppendFormat("\n\t%s %s", ConstantBufferField->Type->Name.CStr(), ConstantBufferField->Name.CStr());
			
			if (ConstantBufferField->ArrayIndex != 0) {
				Builder.AppendFormat("[%d]", static_cast<int>(ConstantBufferField->ArrayIndex));
			}

			if (ConstantBufferField->ExprColon != nullptr) {
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
		if (StaticVariable->ArrayIndex != 0) {
			Builder.AppendFormat("[%d]", static_cast<int>(StaticVariable->ArrayIndex));
		} else if (StaticVariable->ExprColon != nullptr) {
			HLSLRegister const* const StaticVariableRegister = static_cast<HLSLRegister const*>(StaticVariable->ExprColon);
			Builder.AppendFormat(" : register(%c%d)", ((StaticVariableRegister->Register == GD_HLSL_REGISTER_T) ? 't' : 's'),	// Only 't'/'s' registers can be located here. 
								       static_cast<int>(StaticVariableRegister->RegisterID));
		}

		Builder.Append(';');
	}

	void HLSLGenerator::GenerateShaderStaticFunction(HLSLFunction const* const StaticFunction, StringBuilder& Builder)
	{
		Builder.AppendFormat("\n\n%s %s(", StaticFunction->Type->Name.CStr(), StaticFunction->Name.CStr());
		for (auto const& Argument : StaticFunction->Arguments) {
			Builder.AppendFormat("%s%s %s %s",
				(((Argument->AccsessType & GD_HLSL_ARGUMENT_IN ) != 0) ? "in"  : ""),
				(((Argument->AccsessType & GD_HLSL_ARGUMENT_OUT) != 0) ? "out" : ""),
				Argument->Type->Name.CStr(), Argument->Name.CStr());
			if (Argument->ExprColon != nullptr)	{
				HLSLSemantic const* const Semantic = static_cast<HLSLSemantic const* const>(Argument->ExprColon);
				Builder.AppendFormat(" : %s%d", HLSLSemanticToStr(Semantic->Semantic), static_cast<int>(Semantic->SemanticID));
			}

			Builder.Append(", ");
		}
		*(Builder.GetPointer() + Builder.GetSize() - 2) = ')';	// Replacing trailing comma.
		*(Builder.GetPointer() + Builder.GetSize() - 1) = ' ';	// Replacing trailing space.

		if (StaticFunction->Semantic != nullptr) {
			Builder.AppendFormat(": %s%d", HLSLSemanticToStr(StaticFunction->Semantic->Semantic), static_cast<int>(StaticFunction->Semantic->SemanticID));
		}
		Builder.AppendFormat("\n%s", StaticFunction->Body.CStr());
	}

	/// ==========================================================================================
	/// HLSLCompiler class.
	/// Provides generation and compilation of D3D HLSL code.
	/// ==========================================================================================

	/// ------------------------------------------------------------------------------------------
	/// Public class API:
	/// ------------------------------------------------------------------------------------------

	void HLSLCompiler::GenerateAndCompileShader(
		UniquePtr<OutputStream>      const&       ShaderByteCodeOutputStream,
		HLSLScope                    const* const ShaderParsedData,
		HRIShaderType                const        ShaderType,
		String                       const&       ShaderEntryName
	)
	{
		StringBuilder HLSLGeneratorOutput;
		HLSLGenerator(this->Toolchain).GenerateShader(HLSLGeneratorOutput, ShaderParsedData);

		UINT D3DCompileFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR | D3DCOMPILE_AVOID_FLOW_CONTROL;
#if (defined(GD_DEBUG))
		D3DCompileFlags |= D3DCOMPILE_DEBUG;
#else	// if (defined(GD_DEBUG))
		D3DCompileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif	// if (defined(GD_DEBUG))
		if (this->Toolchain->DoTreatWarningsAsError()) {
			D3DCompileFlags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
		}

		LPCSTR static const D3DCompileProfiles[] = {
			/* GD_HRI_SHADER_TYPE_VERTEX   = */ "vs_5_0",
			/* GD_HRI_SHADER_TYPE_HULL     = */ "hs_5_0",
			/* GD_HRI_SHADER_TYPE_DOMAIN   = */ "ds_5_0",
			/* GD_HRI_SHADER_TYPE_PIXEL    = */ "ps_5_0",
			/* GD_HRI_SHADER_TYPE_COMPUTE  = */ "cs_5_0",
			/* GD_HRI_SHADER_TYPE_GEOMETRY = */ "gs_5_0",
		};
		GD_DEBUG_ASSERT(ShaderType < GD_HRI_SHADER_TYPE_UNKNOWN, "Invalid shader type specified.");
		LPCSTR const D3DCompileProfile = D3DCompileProfiles[ShaderType];

		ID3DBlob *OutputBlob = nullptr, *ErrorBlob = nullptr;	// Note: D3DCompile is thread-safe.
		HRESULT const Result = D3DCompile(HLSLGeneratorOutput.GetPointer(), HLSLGeneratorOutput.GetSize(),
			nullptr,  nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, ShaderEntryName.CStr(),
			D3DCompileProfile, D3DCompileFlags, 0, &OutputBlob, &ErrorBlob
		);

		if (FAILED(Result)) {	// Compiling failed.
			if (ErrorBlob != nullptr) {	// And we know what happened.
				HLSLCompilerErrorDesc static const D3DCompilerError("D3DCompile returned error: \n%s\nGenerated code:\n%s");
				throw HLSLCompilerErrorException(D3DCompilerError.ToString(nullptr, reinterpret_cast<Str>(ErrorBlob->GetBufferPointer()), HLSLGeneratorOutput.GetPointer()));
			} else {
				HLSLCompilerErrorDesc static const D3DUnknownError("unknown error occured while compiling shader.");
				throw HLSLCompilerErrorException(D3DUnknownError.ToString(nullptr));
			}
		}

		ShaderByteCodeOutputStream->Write(OutputBlob->GetBufferPointer(), OutputBlob->GetBufferSize(), 1);
	}

GD_NAMESPACE_END
#endif	// #if (defined(GD_PLATFORM_WINDOWS))
