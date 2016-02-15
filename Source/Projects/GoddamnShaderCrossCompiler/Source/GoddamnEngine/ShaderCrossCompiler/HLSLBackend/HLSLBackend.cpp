// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/ShaderCrossCompiler/HLSLBackend/HLSLBackend.cpp
 * File contains HLSL backend implementation for the shader cross compiler.
 */
#include <GoddamnEngine/Include.h>
#if GD_PLATFORM_WINDOWS

#include <GoddamnEngine/ShaderCrossCompiler/HLSLBackend/HLSLBackend.h>
#include <GoddamnEngine/ShaderCrossCompiler/HLSLCrossCompiler.h>
#include <GoddamnEngine/ShaderCrossCompiler/HLSLTokenizer.h>
#include <GoddamnEngine/ShaderCrossCompiler/HLSLParser.h>
#include <GoddamnEngine/Core/Containers/StringBuilder.h>

// We need this header to access the compiler functionality.
#include <atlbase.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

GD_NAMESPACE_BEGIN

	//! Represents compiler error description.
	typedef ToolchainException HLSLCompilerErrorException;

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// HLSLGenerator class.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	class HLSLGenerator final
	{
	public:
		GDINT void GenerateShader(StringBuilder& Builder, HLSLScope const* const ShaderParsedData);
	private:
		GDINT void GenerateShaderStruct(HLSLStruct const* const Struct, StringBuilder& Builder);
		GDINT void GenerateShaderConstantBuffer(HLSLCBuffer const* const ConstantBuffer, StringBuilder& Builder);
		GDINT void GenerateShaderStaticVariable(HLSLVariable const* const StaticVariable, StringBuilder& Builder);
		GDINT void GenerateShaderStaticFunction(HLSLFunction const* const StaticFunction, StringBuilder& Builder);
	}; // class HLSLGenerator

	// ------------------------------------------------------------------------------------------
	//! Generates a HLSL source code from the specified parsed data.
	//! @param Builder Output for the shader code.
	//! @param ShaderParsedData Parsed shader data, produced by @ref HLSLParser.
	GDINT void HLSLGenerator::GenerateShader(StringBuilder& Builder, HLSLScope const* const ShaderParsedData)
	{
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
				GenerateShaderConstantBuffer(ConstantBuffer, Builder);
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
	}

	// ------------------------------------------------------------------------------------------
	//! Generates a HLSL source code from the specified parsed structure definition.
	//! @param Struct Parsed shader structure, produced by @ref HLSLParser.
	//! @param Builder Output for the shader code.
	GDINT void HLSLGenerator::GenerateShaderStruct(HLSLStruct const* const Struct, StringBuilder& Builder)
	{
		Builder.AppendFormat("\n\nstruct %s\n{", Struct->Name.CStr());
		for (auto const& Definition : Struct->InnerDefinitions)
		{
			auto const StructField = static_cast<HLSLVariable const*>(Definition);
			Builder.AppendFormat("\n\t%s %s", StructField->Type->Name.CStr(), StructField->Name.CStr());
			if (StructField->ArrayRank != 0)
			{
				// Adding array information for our field..
				Builder.AppendFormat("[%d]", static_cast<int>(StructField->ArrayRank));
			}
			if (StructField->Binding != nullptr)
			{
				// Adding semantic for our structure field..
				auto const StructFieldSemantic = static_cast<HLSLSemantic const*>(StructField->Binding);
				Builder.AppendFormat(" : %s%d", HLSLSemanticToStr(StructFieldSemantic->Semantic), static_cast<int>(StructFieldSemantic->SemanticID));
			}
			Builder.Append(';');
		}
		Builder.Append("\n};");
	}

	// ------------------------------------------------------------------------------------------
	//! Generates a HLSL source code from the specified parsed constant buffer definition.
	//! @param ConstantBuffer Parsed shader constant buffer, produced by @ref HLSLParser.
	//! @param Builder Output for the shader code.
	GDINT void HLSLGenerator::GenerateShaderConstantBuffer(HLSLCBuffer const* const ConstantBuffer, StringBuilder& Builder)
	{
		Builder.AppendFormat("\n\ncbuffer %s", ConstantBuffer->Name.CStr());
		if (ConstantBuffer->Register != nullptr)
		{
			// Adding register information to our duffer declaration..
			Builder.Append(" : register(b%d)", static_cast<int>(ConstantBuffer->Register->RegisterID));
		}
		Builder.Append("\n{");
		for (auto const& Definition : ConstantBuffer->InnerDefinitions)
		{
			// Emitting new constant buffer field..
			HLSLVariable const* const ConstantBufferField = static_cast<HLSLVariable const*>(Definition);
			Builder.AppendFormat("\n\t%s %s", ConstantBufferField->Type->Name.CStr(), ConstantBufferField->Name.CStr());
			if (ConstantBufferField->ArrayRank != 0)
			{
				// Adding array information for our field..
				Builder.AppendFormat("[%d]", static_cast<int>(ConstantBufferField->ArrayRank));
			}
			if (ConstantBufferField->Binding != nullptr)
			{
				
#if 0			// Adding packing information information for our field..
				HLSLPackOffset const* const FieldPackOffset = static_cast<HLSLPackOffset const*>(Field->Binding);
				Builder.Append(" : packoffset(c%s%d;)", HLSLSemanticToStr(FieldSemantic->Semantic), static_cast<int>(FieldSemantic->SemanticID));
#endif	// if 0
				GD_NOT_IMPLEMENTED();
			}

			Builder.Append(';');
		}
		Builder.Append("\n}");
	}

	// ------------------------------------------------------------------------------------------
	//! Generates a HLSL source code from the specified parsed static variable definition.
	//! @param StaticVariable Parsed shader static variable, produced by @ref HLSLParser.
	//! @param Builder Output for the shader code.
	GDINT void HLSLGenerator::GenerateShaderStaticVariable(HLSLVariable const* const StaticVariable, StringBuilder& Builder)
	{
		Builder.AppendFormat("\n\n%s %s", StaticVariable->Type->Name.CStr(), StaticVariable->Name.CStr());
		if (StaticVariable->ArrayRank != 0)
		{
			// Adding array information for our variable..
			Builder.AppendFormat("[%d]", static_cast<int>(StaticVariable->ArrayRank));
		}
		if (StaticVariable->Binding != nullptr)
		{
			// Adding register information for our variable..
			// Only 't'/'s' registers can be located here. 
			HLSLRegister const* const StaticVariableRegister = static_cast<HLSLRegister const*>(StaticVariable->Binding);
			Builder.AppendFormat(" : register(%c%d)", StaticVariableRegister->Register == GD_HLSL_REGISTER_T ? 't' : 's' 
				, static_cast<int>(StaticVariableRegister->RegisterID));
		}
		Builder.Append(';');
	}

	// ------------------------------------------------------------------------------------------
	//! Generates a HLSL source code from the specified parsed static function definition.
	//! @param StaticFunction Parsed shader static function, produced by @ref HLSLParser.
	//! @param Builder Output for the shader code.
	GDINT void HLSLGenerator::GenerateShaderStaticFunction(HLSLFunction const* const StaticFunction, StringBuilder& Builder)
	{
		Builder.AppendFormat("\n\n%s %s(", StaticFunction->Type->Name.CStr(), StaticFunction->Name.CStr());
		for (SizeTp cnt = 0; cnt < StaticFunction->Arguments.GetLength(); ++cnt)
		{
			// Emitting function parameter..
			auto const& Argument = StaticFunction->Arguments[cnt];
			Builder.AppendFormat("%s%s %s %s", (Argument->AccsessType & GD_HLSL_ARGUMENT_IN) != 0 ? "in" : "", (Argument->AccsessType & GD_HLSL_ARGUMENT_OUT) != 0 ? "out" : ""
				, Argument->Type->Name.CStr(), Argument->Name.CStr());
			if (Argument->Binding != nullptr)
			{
				// Adding Semantic information for our variable..
				HLSLSemantic const* const Semantic = static_cast<HLSLSemantic const* const>(Argument->Binding);
				Builder.AppendFormat(" : %s%d", HLSLSemanticToStr(Semantic->Semantic), static_cast<int>(Semantic->SemanticID));
			}
			if (cnt != StaticFunction->Arguments.GetLength() - 1)
			{
				// This is not the last argument, appending comma after it.
				Builder.AppendFormat(", ");
			}
		}
		Builder.Append(')');

		if (StaticFunction->Semantic != nullptr)
		{
			// Adding Semantic information for our return value..
			Builder.AppendFormat(": %s%d", HLSLSemanticToStr(StaticFunction->Semantic->Semantic), static_cast<int>(StaticFunction->Semantic->SemanticID));
		}

		// Emitting function body..
		Builder.AppendFormat("\n%s", StaticFunction->Body.CStr());
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
	void HLSLCompilerBackend::GenerateAndCompileShader(UniquePtr<OutputStream> const& ShaderByteCodeOutputStream
		, HLSLScope const* const ShaderParsedData, IGraphicsShaderType const ShaderType, String const& ShaderEntryName
		, DHLSLShaderCrossCompilerTarget const ShaderTargetPlatform)
	{
		// Generating our shader code...
		StringBuilder HLSLGeneratorOutput;
		HLSLGenerator().GenerateShader(HLSLGeneratorOutput, ShaderParsedData);

		UINT static const D3DCompileFlags = D3DCOMPILE_ENABLE_STRICTNESS
			| D3DCOMPILE_PACK_MATRIX_ROW_MAJOR
			| D3DCOMPILE_AVOID_FLOW_CONTROL
			| D3DCOMPILE_WARNINGS_ARE_ERRORS
#if GD_DEBUG
			| D3DCOMPILE_DEBUG	// We need to debug shaders.
#else // if GD_DEBUG
			| D3DCOMPILE_OPTIMIZATION_LEVEL3	// We need to optimize our shader as much as possible.
#endif // if GD_DEBUG
			;

		LPCSTR static const IGraphicsD3DShaderProfilesTable50[] = {
			/* IGRAPHICS_SHADER_TYPE_VERTEX   */ "vs_5_0",
			/* IGRAPHICS_SHADER_TYPE_PIXEL    */ "ps_5_0",
			/* IGRAPHICS_SHADER_TYPE_GEOMETRY */ "gs_5_0",
			/* IGRAPHICS_SHADER_TYPE_HULL     */ "hs_5_0",
			/* IGRAPHICS_SHADER_TYPE_DOMAIN   */ "ds_5_0",
			/* IGRAPHICS_SHADER_TYPE_COMPUTE  */ "cs_5_0",
			/* IGRAPHICS_SHADER_TYPE_UNKNOWN  */ nullptr,
		};

		LPCSTR static const IGraphicsD3DShaderProfilesTable51[] = {
			/* IGRAPHICS_SHADER_TYPE_VERTEX   */ "vs_5_1",
			/* IGRAPHICS_SHADER_TYPE_PIXEL    */ "ps_5_1",
			/* IGRAPHICS_SHADER_TYPE_GEOMETRY */ "gs_5_1",
			/* IGRAPHICS_SHADER_TYPE_HULL     */ "hs_5_1",
			/* IGRAPHICS_SHADER_TYPE_DOMAIN   */ "ds_5_1",
			/* IGRAPHICS_SHADER_TYPE_COMPUTE  */ "cs_5_1",
			/* IGRAPHICS_SHADER_TYPE_UNKNOWN  */ nullptr,
		};

		// Trying to compile our shader code..
		CComPtr<ID3DBlob> ErrorBlob = nullptr;
		CComPtr<ID3DBlob> OutputBlob = nullptr;
		if (FAILED(D3DCompile(HLSLGeneratorOutput.CStr(), HLSLGeneratorOutput.GetLength()
			, nullptr, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, ShaderEntryName.CStr()
			, IGraphicsD3DShaderProfilesTable50[ShaderType], D3DCompileFlags, 0, &OutputBlob.p, &ErrorBlob.p)))
		{ 
			// Compiling failed..
			if (ErrorBlob != nullptr)
			{ 
				// ..And we know what happened..
				throw HLSLCompilerErrorException(D3DCOMPILER_DLL_A " returned error: \n%s\nGenerated code:\n%s", reinterpret_cast<CStr>(ErrorBlob->GetBufferPointer()), HLSLGeneratorOutput.CStr());
			}
			// ..or not..
			throw HLSLCompilerErrorException("unknown error occurred while compiling shader.");
		}

		// Writing compiled	 shader Byte code.
		ShaderByteCodeOutputStream->Write(OutputBlob->GetBufferPointer(), OutputBlob->GetBufferSize(), 1);
	}

GD_NAMESPACE_END

#endif // if GD_PLATFORM_WINDOWS
