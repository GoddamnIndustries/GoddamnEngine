// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Engine/Interface/Graphics/GraphicsHLSLCrossCompiler/HLSLValidator.cpp
 * File HLSL validator implementation for the shader cross compiler.
 */
#include <GoddamnEngine/Include.h>
#if GD_PLATFORM_DESKTOP

#include <GoddamnEngine/Engine/Interface/Graphics/GraphicsHLSLCrossCompiler/HLSLCrossCompiler.h>
#include <GoddamnEngine/Engine/Interface/Graphics/GraphicsHLSLCrossCompiler/HLSLValidator.h>
#include <GoddamnEngine/Engine/Interface/Graphics/GraphicsHLSLCrossCompiler/HLSLTokenizer.h>
#include <GoddamnEngine/Engine/Interface/Graphics/GraphicsHLSLCrossCompiler/HLSLParser.h>

#include "Format.h"

// ------------------------------------------------------------------------------------------
//! Define this to make validator throw warnings on stuff unrecommended to
//! used in GoddamnEngine coding standards.
#define GD_SHADERCC_VALIDATOR_VALIDATE_STYLE GD_TRUE

GD_NAMESPACE_BEGIN

	typedef ToolchainException HLSLValidatorErrorException;

	// ==========================================================================================
	// HLSLValidator class.
	// ==========================================================================================

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Internal class API:
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Validates all static resource parameters.
	//! @param ParsedData Parsed HLSL code data.
	void HLSLValidator::ValidateResourceParameters(HLSLScope const* const ParsedData, HRIShaderInstanceDesc* const ShaderInstanceDesc)
	{
		SizeTp TextureObjectNewRegister = 0;
#if 0
		RefPtr<HRIShaderParamLocationDesc> ShaderParamLocationResources(new HRIShaderParamLocationDesc(ShaderInstanceDesc, GD_HRI_SHADER_PARAM_LOCATION_DESC_TYPE_RESOURCES));
#endif	// if 0
		for (auto const& Definition : ParsedData->InnerDefinitions)
		{
			auto const TextureObject = HLSLDynamicCast<HLSLVariable const*>(Definition);
			if (TextureObject == nullptr)
				continue;

#if 0
			HRIShaderParamDescType TextureObjectType = GD_HRI_SHADER_PARAM_DESC_TYPE_UNKNOWN;
#endif	// if 0
			if (TextureObject->Type->Class == GD_HLSL_TYPE_CLASS_TEXTURE2D)
			{
#if 0
				TextureObjectType = GD_HRI_SHADER_PARAM_DESC_TYPE_TEXTURE2D;
#endif	// if 0
			}
			else if (TextureObject->Type->Class == GD_HLSL_TYPE_CLASS_TEXTURECUBE)
			{
#if 0
				TextureObjectType = GD_HRI_SHADER_PARAM_DESC_TYPE_TEXTURECUBE;
#endif	// if 0
			}
			else
			{
				if (TextureObject->Type->Class != GD_HLSL_TYPE_CLASS_SAMPLER)
				{
					auto const StaticVariable = TextureObject;
					if (StaticVariable->Binding != nullptr)
					{ 
						// Some static variable contains after-colon expression.
						throw HLSLValidatorErrorException("static variable '%s' cannot have after-colon-expression.", TextureObject->Name.CStr());
					}
				}
				continue;
			}

			// Lets check if this texture object also contains sampler.
			auto const TextureSampler = ParsedData->FindVariable(TextureObject->Name + "Sampler");
			if (TextureSampler == nullptr)
			{ 
				// No sampler was found.
				throw HLSLValidatorErrorException("appropriate sampler for texture object named '%s' does not exists.", TextureObject->Name.CStr());
			}

			auto const TextureObjectRegister = HLSLDynamicCast<HLSLRegister const*>(TextureObject->Binding);
			if (TextureObjectRegister != nullptr)
			{
				if (TextureObjectRegister->Register != GD_HLSL_REGISTER_T)
				{ 
					// Texture is located outside 'T' registers group.
					throw HLSLValidatorErrorException("texture '%s' is located outside of 'T' registers group.", TextureObject->Name.CStr());
				}
			}

			auto const TextureSamplerRegister = HLSLDynamicCast<HLSLRegister const*>(TextureSampler->Binding);
			if (TextureSamplerRegister != nullptr)
			{
				if (TextureSamplerRegister->Register != GD_HLSL_REGISTER_S)
				{ 
					// Sampler is located outside 'S' registers group.
					throw HLSLValidatorErrorException("sampler '%s' is located outside of 'S' registers group.", TextureObject->Name.CStr());
				}
			}

			SizeTp TextureObjectUsedRegister = SizeTpMax;
			if (TextureObjectRegister != nullptr && TextureSamplerRegister != nullptr)
			{ 
				// Both texture object and sampler contain register information.
				if (TextureObjectRegister->RegisterID != TextureSamplerRegister->RegisterID)
				{ 
					// Texture and it`s sampler should be located in same registers
					throw HLSLValidatorErrorException("texture '%s' and appropriate sampler should be located in same registers", TextureObject->Name.CStr());
				}

				TextureObjectUsedRegister = TextureObjectRegister->RegisterID;
				TextureObjectNewRegister  = TextureObjectUsedRegister + 1;
			}
			else if (TextureObjectRegister == nullptr && TextureSamplerRegister != nullptr)
			{ // Only texture sampler contains register information.
#if GD_SHADERCC_VALIDATOR_VALIDATE_STYLE
				throw HLSLValidatorErrorException("texture sampler '%s' contains explicit register information, but object does not. Consider adding it.", TextureObject->Name.CStr());
#else // if GD_SHADERCC_VALIDATOR_VALIDATE_STYLE
				TextureObjectUsedRegister = TextureSamplerRegister->RegisterID;
				TextureObjectNewRegister = TextureObjectUsedRegister + 1;
#endif // if GD_SHADERCC_VALIDATOR_VALIDATE_STYLE
			}
			else if (TextureObjectRegister != nullptr && TextureSamplerRegister == nullptr)
			{ // Only texture object contains register information.
#if GD_SHADERCC_VALIDATOR_VALIDATE_STYLE
				throw HLSLValidatorErrorException("texture object '%s' contains explicit register information, but sampler does not. Consider adding it.", TextureObject->Name.CStr());
#else // if GD_SHADERCC_VALIDATOR_VALIDATE_STYLE
				TextureObjectUsedRegister = TextureObjectRegister->RegisterID;
				TextureObjectNewRegister  = TextureObjectUsedRegister + 1;
#endif // if GD_SHADERCC_VALIDATOR_VALIDATE_STYLE
			}
			else
			{
#if GD_SHADERCC_VALIDATOR_VALIDATE_STYLE
				throw HLSLValidatorErrorException("texture object '%s' and appropriate sampler does not contain explicit register information. Consider adding it.", TextureObject->Name.CStr());
#else // if GD_SHADERCC_VALIDATOR_VALIDATE_STYLE
				TextureObjectUsedRegister = TextureObjectNewRegister;
				TextureObjectNewRegister += 1;
#endif // if GD_SHADERCC_VALIDATOR_VALIDATE_STYLE

			}

#if 0		// Now we are sure that texture parameter is valid and ready to create descriptor.
			RefPtr<HRIShaderParamDesc>(new HRIShaderParamDesc(ShaderParamLocationResources.GetPointer(), TextureObject->Name, TextureObjectType, GD_FORMAT_UNKNOWN, TextureObjectUsedRegister));
#endif	// if 0
		}
	}

	// ------------------------------------------------------------------------------------------
	//! Validates all constant buffers parameters.
	//! @param ParsedData Parsed HLSL code data.
	void HLSLValidator::ValidateConstantBuffersParameters(HLSLScope const* const ParsedData, HRIShaderInstanceDesc* const ShaderInstanceDesc)
	{
		SizeTp ConstantBufferNewRegister = 0;
		for (auto const& Definition : ParsedData->InnerDefinitions)
		{
			auto const ConstantBuffer = HLSLDynamicCast<HLSLCBuffer const*>(Definition);
			if (ConstantBuffer == nullptr)
				continue;

			auto const ConstantBufferRegister = ConstantBuffer->Register;
			auto ConstantBufferUsedRegister = SizeTpMax;
			if (ConstantBufferRegister != nullptr)
			{
				if (ConstantBufferRegister->Register != GD_HLSL_REGISTER_B)
				{ 
					// Constant buffer is located outside 'B' registers group
					throw HLSLValidatorErrorException("constant buffer '%s' is located outside of 'B' registers group.", ConstantBuffer->Name.CStr());
				}

				ConstantBufferUsedRegister = ConstantBufferRegister->RegisterID;
				ConstantBufferNewRegister  = ConstantBufferUsedRegister + 1;
			}
			else
			{
				throw HLSLValidatorErrorException("constant buffer '%s' does not contain explicit register information. Consider adding it.", ConstantBuffer->Name.CStr());
			}
			
#if 0		// Now we are sure that constant buffer is valid and ready to create descriptor.
			RefPtr<HRIShaderParamLocationDesc> ShaderParamConstantBuffersLocationDesc(new HRIShaderParamLocationDesc(ShaderInstanceDesc, GD_HRI_SHADER_PARAM_LOCATION_DESC_TYPE_CONSTANTBUFFER));
#endif	// if 0
			for (auto const& InnerDefinition : ConstantBuffer->InnerDefinitions)
			{
				HLSLVariable const* const ConstantBufferParam = HLSLDynamicCast<HLSLVariable const*>(InnerDefinition);
				if (ConstantBufferParam == nullptr)
					continue;

				Format ParamFormat = GD_FORMAT_UNKNOWN;
			//	HRIShaderParamDescType ParamType = GD_HRI_SHADER_PARAM_DESC_TYPE_UNKNOWN;
				if (ConstantBufferParam->Type->Class == GD_HLSL_TYPE_CLASS_SCALAR || ConstantBufferParam->Type->Class == GD_HLSL_TYPE_CLASS_VECTOR)
				{
					auto const VectorType = HLSLDynamicCast<HLSLVectorType const*>(ConstantBufferParam->Type);
					auto const DataType = VectorType != nullptr ? VectorType->DataType : static_cast<HLSLScalarType const*>(ConstantBufferParam->Type)->DataType;

					SizeTp ParamFormatSize = GD_FORMAT_SIZE_UNKNOWN;
					FormatType ParamFormatType = GD_FORMAT_TYPE_UNKNOWN;
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
					}

				//	ParamType = GD_HRI_SHADER_PARAM_DESC_TYPE_format-able;
					ParamFormat = GD_FORMAT_MAKE(ParamFormatType, ParamFormatSize, ((VectorType != nullptr) ? VectorType->ComponentsNumber : 1));
				}
				else if (ConstantBufferParam->Type->Class == GD_HLSL_TYPE_CLASS_MATRIX)
				{
					HLSLMatrixType const* const MatrixType = static_cast<HLSLMatrixType const*>(ConstantBufferParam->Type);
					if (MatrixType->RowsNumber == 4 && MatrixType->ColumnsNumber == 4)
						; // ParamType = GD_HRI_SHADER_PARAM_DESC_TYPE_MATRIX4X4;
					else if (MatrixType->RowsNumber == 3 && MatrixType->ColumnsNumber == 3)
						; // ParamType = GD_HRI_SHADER_PARAM_DESC_TYPE_MATRIX3X3;
					else
						GD_NOT_IMPLEMENTED();
				}

#if 0			//! @todo Uncomment this.
				RefPtr<HRIShaderParamDesc>(new HRIShaderParamDesc(
					ShaderParamConstantBuffersLocationDesc.GetPointer(), ConstantBufferParam->Name, ParamType, ParamFormat,
					((ConstantBufferParam->ArrayRank != 0) ? ConstantBufferParam->ArrayRank : 1)));
#endif	// if 0
			}
		}
	}

	// ------------------------------------------------------------------------------------------
	//! Validates entry point of the shader.
	//! @param ParsedData Parsed HLSL code data.
	void HLSLValidator::ValidateEntryPoint(HLSLScope const* const ParsedData, String const& EntryPointName/*, RefPtr<HRIShaderInstanceDesc>& ShaderInstanceDesc*/)
	{
		HLSLFunction const* const EntryPoint = ParsedData->FindFunction(EntryPointName);
		if (EntryPoint == nullptr)
		{ 
			// No entry point was found.
			throw HLSLValidatorErrorException("entry point function named '%s' was not found.", EntryPointName.CStr());
		}

		UInt64 ShaderInputSemantics  = 0;
		UInt64 ShaderOutputSemantics = 0;

		// Utilizing entry point return value.
		if (EntryPoint->Type->Class != GD_HLSL_TYPE_CLASS_VOID)
		{
#if GD_SHADERCC_VALIDATOR_VALIDATE_STYLE
			throw HLSLValidatorErrorException("style warning: entry point function '%s' should return void.", EntryPointName.CStr());
#else // if GD_SHADERCC_VALIDATOR_VALIDATE_STYLE
			if (EntryPoint->Type->Class == GD_HLSL_TYPE_CLASS_STRUCT)
			{	
				// Return value of entry point is structure that may contain some semantics.
				HLSLStruct const* const EntryPointReturnStruct = static_cast<HLSLStruct const*>(EntryPoint->Type);
				for (auto const& Definition : EntryPointReturnStruct->InnerDefinitions) 
				{
					HLSLVariable const* const EntryPointReturnStructField = static_cast<HLSLVariable const*>(Definition);
					if (EntryPointReturnStructField->Binding == nullptr) 
					{
						throw HLSLValidatorErrorException("entry point function '%s' returns structure that contains unutilized field '%s'.", EntryPointName.CStr()
							, EntryPointReturnStructField->Name.CStr());
					}
					ValidateEntryPointArgumentBinding(EntryPointReturnStructField->Binding, ShaderInputSemantics);
				}
			} 
			else if (EntryPoint->Type->Class == GD_HLSL_TYPE_CLASS_SCALAR || EntryPoint->Type->Class == GD_HLSL_TYPE_CLASS_VECTOR) 
			{	
				// Entry point returns scalar or vector value.
				HLSLSemantic const* const EntryPointSemantic = EntryPoint->Semantic;
				if (EntryPointSemantic == nullptr) 
				{
					// Entry point returns value that is not utilized with any semantic
					throw HLSLValidatorErrorException("entry point function '%s' returns value that is not utilized with any semantic.", EntryPointName.CStr());
				}
				ValidateEntryPointArgumentBinding(EntryPointSemantic, ShaderOutputSemantics);
			} 
			else 
			{	
				// Entry point returns something really strange.
				throw HLSLValidatorErrorException("entry point function '%s' returns value that cannot be utilized.", EntryPointName.CStr());
			}
#endif // if GD_SHADERCC_VALIDATOR_VALIDATE_STYLE
		}

		// Utilizing entry point arguments.
		for (auto const& EntryPointArgument : EntryPoint->Arguments)
		{
			UInt64* CurrentShaderSemantics = nullptr;
			if (EntryPointArgument->AccsessType == GD_HLSL_ARGUMENT_IN) 
				CurrentShaderSemantics = &ShaderInputSemantics;
			else if (EntryPointArgument->AccsessType == GD_HLSL_ARGUMENT_OUT) 
				CurrentShaderSemantics = &ShaderOutputSemantics;
			else
			{
				throw HLSLValidatorErrorException("invalid access type of argument '%s'", EntryPointArgument->Name.CStr());
			}

			auto const EntryPointArgumentStruct = HLSLDynamicCast<HLSLStruct const*>(EntryPointArgument->Type);
			if (EntryPointArgumentStruct != nullptr)
			{
				for (auto const& Definition : EntryPointArgumentStruct->InnerDefinitions)
				{
					auto const EntryPointArgumentStructField = static_cast<HLSLVariable const*>(Definition);
					ValidateEntryPointArgumentBinding(EntryPointArgumentStructField->Binding, *CurrentShaderSemantics);
				}
			}
			else
			{
				ValidateEntryPointArgumentBinding(EntryPointArgument->Binding, *CurrentShaderSemantics);
			}
		}
		
#if 0
		ShaderInstanceDesc = new HRIShaderInstanceDesc(ShaderInputSemantics, ShaderOutputSemantics);
#endif	// if 0
	}

	// ------------------------------------------------------------------------------------------
	//! Validates entry point arguments bindings.
	//! @param ParsedData Parsed HLSL code data.
	//! @param ShaderSemanticsList
	//! @todo Document me.
	void HLSLValidator::ValidateEntryPointArgumentBinding(HLSLBinding const* const Binding, UInt64& ShaderSemanticsList)
	{
		HLSLSemantic const* const ArgumentSemanticHLSL = static_cast<HLSLSemantic const*>(Binding);
		HRISemantic const ArgumentSemanticHRI = HLSLSemanticToHRI(ArgumentSemanticHLSL->Semantic);
		if (ArgumentSemanticHRI == GD_HRI_SEMANTIC_UNKNOWN)
		{ 
			//! @todo Do something here.
	//		HLSLValidatorErrorDesc static const UntranslatableSemanticError("Untranslatable semantic '%s' in argument '%s'");
	//		throw HLSLValidatorErrorException(UntranslatableSemanticError, HLSLSemanticToStr(ArgumentHLSLSemantic->Semantic), &Argument->Name[0]);
	//		RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);
	//	
	//		return false;
			return;
		}

	//	if ((ShaderSemanticsList & GD_BIT(UInt64(ArgumentSemanticHRI + 1))) != 0) {
	//		HLSLValidatorErrorDesc static const InvalidAccessTypeError("entry point function already contains (in/out)put for '%s' semantic");
	//		throw HLSLValidatorErrorException(InvalidAccessTypeError, HLSLSemanticToStr(ArgumentSemanticHLSL->Semantic));
	//		RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);
	//
	//		return false;
	//	}

		ShaderSemanticsList |= GD_BIT(UInt64(ArgumentSemanticHRI + 1));
	}

	// ------------------------------------------------------------------------------------------
	//! Generates shader instance description using data generated by HLSLParser.
	//! @param Shader  Shader for which description would be generated.
	//! @param ParsedData Parsed data of specified shader.
	//! @param EntryName  Name of entry point function
	//! @returns Generated description from parsed shader data or nullptr if error occurs.
	HRIShaderInstanceDesc* HLSLValidator::ValidateAndGenerateDescription(HLSLScope const* const ParsedData, String const& EntryPointName)
	{
		//! @todo Uncomment all these..
	//	RefPtr<HRIShaderInstanceDesc> ShaderInstanceDesc(nullptr);
		ValidateEntryPoint(ParsedData, EntryPointName/*, ShaderInstanceDesc*/);
		ValidateConstantBuffersParameters(ParsedData, /*ShaderInstanceDesc.GetPointer()*/nullptr);
		ValidateResourceParameters(ParsedData, /*ShaderInstanceDesc.GetPointer()*/nullptr);

		return /*ShaderInstanceDesc.Release()*/ nullptr;
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_DESKTOP
