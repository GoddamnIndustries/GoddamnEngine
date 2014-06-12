//////////////////////////////////////////////////////////////////////////
/// Description.cpp: shader instance description generator implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 17.05.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/Description/Description.hh>
#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/Parser/Parser.hh>
#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/CrossCompiler.hh>
#include <GoddamnEngine/Core/Containers/Pointer/Pointer.hh>

GD_NAMESPACE_BEGIN

	typedef Str HRIShaderInstanceDescGeneratorErrorDesc;

	bool HRIShaderInstanceDescGenerator::ProcessEntryPointArgument(HLSLVariable const* const Argument, UInt64& ShaderSemanticsList)
	{
		HLSLSemantic const* ArgumentHLSLSemantic = HLSLDynamicCast<HLSLSemantic const*>(Argument->ExprColon);
		if (ArgumentHLSLSemantic == nullptr)
		{
			HRIShaderInstanceDescGeneratorErrorDesc static const InvalidColonExprError("invalid after-colon-expression in argument '%s'");
			self->RaiseError(InvalidColonExprError, &Argument->Name[0]);
			self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

			return false;
		}

		HRISemantic const ArgumentHRISemantic = HLSLSemanticToHRI(ArgumentHLSLSemantic->Semantic);
		if (ArgumentHRISemantic == GD_HRI_SEMANTIC_UNKNOWN)
		{
		//	HRIShaderInstanceDescGeneratorErrorDesc static const UntranslatableSemanticError("Untranslatable semantic '%s' in argument '%s'");
		//	self->RaiseError(UntranslatableSemanticError, HLSLSemanticToStr(ArgumentHLSLSemantic->Semantic), &Argument->Name[0]);
		//	self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);
		//
		//	return false;
			return true;
		}

		ShaderSemanticsList |= ArgumentHRISemantic;
		return true;
	}

	HRIShaderInstanceDesc* HRIShaderInstanceDescGenerator::GenerateDescription(
		HRIShader      * const Shader,
		HLSLScope const* const ParsedData,
		String    const&       EntryName
	)
	{
		HLSLFunction const* const Entry = ParsedData->FindFunction(EntryName);
		if (Entry == nullptr)
		{
			HRIShaderInstanceDescGeneratorErrorDesc static const NoEntryFunctionError("entry point named '%s' was not found.");
			self->RaiseError(NoEntryFunctionError, &EntryName[0]);
			self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

			return nullptr;
		}

		UInt64 ShaderInputSemantics  = 0;
		UInt64 ShaderOutputSemantics = 0;
		for (auto const Argument : Entry->Arguments)
		{
			UInt64* ShaderSemantics = nullptr;
			/**/ if (Argument->AccsessType == GD_HLSL_ARGUMENT_IN ) ShaderSemantics = &ShaderInputSemantics;
			else if (Argument->AccsessType == GD_HLSL_ARGUMENT_OUT) ShaderSemantics = &ShaderOutputSemantics;
			else
			{
				HRIShaderInstanceDescGeneratorErrorDesc static const InvalidAccessTypeError("invalid access type of argument '%s'");
				self->RaiseError(InvalidAccessTypeError, &Argument->Name[0]);
				self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

				return nullptr;
			}

			if (Argument->Type->Class == GD_HLSL_TYPE_CLASS_STRUCT)
			{
				HLSLStruct const* const Struct = HLSLDynamicCast<HLSLStruct const*>(Argument->Type);
				for (auto const Definition : Struct->InnerDefinitions)
				{
					HLSLVariable const* const Variable = HLSLDynamicCast<HLSLVariable const*>(Definition);
					if (!self->ProcessEntryPointArgument(Variable, *ShaderSemantics))
						return nullptr;
				}
			}
			else if (!self->ProcessEntryPointArgument(Argument, *ShaderSemantics))
				return nullptr;
		}

		RefPtr<HRIShaderInstanceDesc> ShaderInstanceDesc(new HRIShaderInstanceDesc(Shader, ShaderInputSemantics, ShaderOutputSemantics));
		RefPtr<HRIShaderParamLocationDesc> ShaderResourcesLocation(new HRIShaderParamLocationDesc(ShaderInstanceDesc.GetPointer(), GD_HRI_SHADER_PARAM_LOCATION_DESC_TYPE_RESOURCES));
		for (auto const Definition : ParsedData->InnerDefinitions)
		{
			HLSLVariable const* const Variable = HLSLDynamicCast<HLSLVariable const*>(Definition);
			if (Variable != nullptr)
			{
				GD_NOT_IMPLEMENTED();
				/*if (Variable->Type->Class == GD_HLSL_TYPE_CLASS_TEXTURE2D)
				{
					continue;
				}

				if (Variable->Type->Class == GD_HLSL_TYPE_CLASS_SAMPLER)
				{
					if (Variable->Name.EndsWith("Sampler"))
					{
						HRIShaderInstanceDescGeneratorErrorDesc static const UnbecomingSamplerNameError("unbecoming sampler name '%s'. Name should end with 'Sampler' postfix.");
						self->RaiseError(UnbecomingSamplerNameError, &Variable->Name[0]);
						self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

						return nullptr;
					}

					size_t static const SamplerLettersCount = 7;
					HLSLVariable const* const AppropriateTexture = ParsedData->FindVariable(Variable->Name.GetSubstring(0, Variable->Name.GetSize() - SamplerLettersCount));
					if (AppropriateTexture == nullptr)
					{
						HRIShaderInstanceDescGeneratorErrorDesc static const NoAppropriateTextureError("no appropriate texture was found for sampler '%s'. Texture 'SomeTexture' should be paired to 'SomeTexture' sampler.");
						self->RaiseError(NoAppropriateTextureError, &Variable->Name[0]);
						self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

						return nullptr;
					}

					continue;
				}

				HRIShaderInstanceDescGeneratorErrorDesc static const UnallowedGlobalVariableType("unallowed type of a global variable '%s'. Only Texture/SamplerState types are allowed.");
				self->RaiseError(UnallowedGlobalVariableType, &Variable->Name[0]);
				self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);
				
				return nullptr;*/
			}

			HLSLCBuffer const* const ConstantBuffer = HLSLDynamicCast<HLSLCBuffer const*>(Definition);
			if (ConstantBuffer != nullptr)
			{
				RefPtr<HRIShaderParamLocationDesc> ConstantBufferLocationDesc(new HRIShaderParamLocationDesc(ShaderInstanceDesc.GetPointer(), GD_HRI_SHADER_PARAM_LOCATION_DESC_TYPE_CONSTANTBUFFER));
				for (auto const Definition : ConstantBuffer->InnerDefinitions)
				{
					HLSLVariable const* const Param = HLSLDynamicCast<HLSLVariable const*>(Definition);
					HRIShaderParamDescType    ParamType = GD_HRI_SHADER_PARAM_DESC_TYPE_UNKNOWN;
					Format                    ParamFormat = GD_FORMAT_UNKNOWN;

					/**/ if ((Param->Type->Class == GD_HLSL_TYPE_CLASS_SCALAR) || (Param->Type->Class == GD_HLSL_TYPE_CLASS_VECTOR))
					{
						size_t     ParamFormatComponentsCount = 0;
						size_t     ParamFormatSize = GD_FORMAT_SIZE_UNKNOWN;
						FormatType ParamFormatType = GD_FORMAT_TYPE_UNKNOWN;

						HLSLTypeDataType DataType = GD_HLSL_TYPE_DATA_TYPE_UNKNOWN;
						HLSLVectorType const* const VectorType = HLSLDynamicCast<HLSLVectorType const*>(Param->Type);
						if (VectorType != nullptr)
						{
							ParamFormatComponentsCount = VectorType->ComponentsNumber;
							DataType = VectorType->DataType;
						}
						else
						{
							ParamFormatComponentsCount = 1;
							DataType = HLSLDynamicCast<HLSLScalarType const*>(Param->Type)->DataType;
						}

						switch (DataType)
						{
						case GD_HLSL_TYPE_DATA_TYPE_bool:
							ParamFormatSize = GD_FORMAT_SIZE_8BITS;
							ParamFormatType = GD_FORMAT_TYPE_UINT;
							break;

						case GD_HLSL_TYPE_DATA_TYPE_int:
							ParamFormatSize = GD_FORMAT_SIZE_32BITS;
							ParamFormatType = GD_FORMAT_TYPE_SINT;
							break;

						case GD_HLSL_TYPE_DATA_TYPE_float:
							ParamFormatSize = GD_FORMAT_SIZE_32BITS;
							ParamFormatType = GD_FORMAT_TYPE_FLOAT;
							break;

						case GD_HLSL_TYPE_DATA_TYPE_double:
							ParamFormatSize = GD_FORMAT_SIZE_64BITS;
							ParamFormatType = GD_FORMAT_TYPE_FLOAT;
							break;

						case GD_HLSL_TYPE_DATA_TYPE_dword:
						case GD_HLSL_TYPE_DATA_TYPE_uint:
							ParamFormatSize = GD_FORMAT_SIZE_32BITS;
							ParamFormatType = GD_FORMAT_TYPE_UINT;
							break;

						default: 
							GD_NOT_IMPLEMENTED();
							break;
						}

						ParamType   = GD_HRI_SHADER_PARAM_DESC_TYPE_FORMATABLE;
						ParamFormat = GD_FORMAT_MAKE(ParamFormatType, ParamFormatSize, ParamFormatComponentsCount);
					}
					else if (Param->Type->Class == GD_HLSL_TYPE_CLASS_MATRIX)
					{
						HLSLMatrixType const* const MatrixType = HLSLDynamicCast<HLSLMatrixType const*>(Param->Type);
						/**/ if ((MatrixType->RowsNumber == 4) && (MatrixType->ColumnsNumber == 4))
							ParamType = GD_HRI_SHADER_PARAM_DESC_TYPE_MATRIX4X4;
						else if ((MatrixType->RowsNumber == 3) && (MatrixType->ColumnsNumber == 3))
							ParamType = GD_HRI_SHADER_PARAM_DESC_TYPE_MATRIX4X4;
						else
							GD_NOT_IMPLEMENTED();
					}

					RefPtr<HRIShaderParamDesc>(new HRIShaderParamDesc(ConstantBufferLocationDesc.GetPointer(), Param->Name, ParamType, ParamFormat));
				}
			}
		}

		return ShaderInstanceDesc.Release();
	}

GD_NAMESPACE_END
