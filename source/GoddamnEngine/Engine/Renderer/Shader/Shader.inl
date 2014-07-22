/// ==========================================================================================
/// HRIShader.inl: shader inline implementation
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.01.2014 - Created by James Jhuighuy
///		* 06.02.2014 - Rewritten from scratch by James Jhuighuy
///     * 05.05.2014 - Rewritten to match cross compiler
/// ==========================================================================================

#include <GoddamnEngine/Core/IO/Stream/Stream.h>
#include <GoddamnEngine/Core/Format/FormatFromString.h>
#include <GoddamnEngine/Core/Format/FormatSelector.h>

GD_NAMESPACE_BEGIN

	/// ==========================================================================================
	/// HRIShaderParamDesc
	/// ==========================================================================================

	GDINL HRIShaderParamDesc::HRIShaderParamDesc(
		HRIShaderParamLocationDesc* const  ParamLocationDesc,
		String                      const& ParamName,
		HRIShaderParamDescType      const  ParamType,
		Format                      const  ParamFormat      /* = GD_FORMAT_UNKNOWN                   */,
		size_t                      const  ParamArrayLength /* = 1                                   */,
		HRIShaderParamID            const  ParamID          /* = GD_HRI_SHADER_PARAM_DESC_ID_UNKNOWN */)
		: HRIObject(HRIObject::TreeLockingFlagsAll, ParamLocationDesc)
		, ParamName(ParamName), ParamHash  (ParamName.GetHashSumm())
		, ParamType(ParamType), ParamFormat(ParamFormat), ParamArrayLength(ParamArrayLength)
		, ParamID(ParamID)
	{
		GD_DEBUG_ASSERT(  (self->GetParentObject() != nullptr), "Invalid instance desc specified");
		GD_DEBUG_ASSERT(  (self->ParamType  < GD_HRI_SHADER_PARAM_DESC_TYPE_UNKNOWN), "Invalid type specified");
		GD_DEBUG_ASSERT(!((self->ParamType == GD_HRI_SHADER_PARAM_DESC_TYPE_FORMATABLE) ^ (self->ParamFormat != GD_FORMAT_UNKNOWN)), "Invalid format specified");

		ParamLocationDesc->LocationInstancesSize  += self->GetParamSize();
		ParamLocationDesc->LocationInstancesCount += 1;

		GD_DEBUG_ASSERT((ParamLocationDesc->LocationInstancesSize  < GD_HRI_SHADER_PARAM_MAX_LOCATION_SIZE),  "Max location parametrs size reached" );
		GD_DEBUG_ASSERT((ParamLocationDesc->LocationInstancesCount < GD_HRI_SHADER_PARAM_MAX_LOCATION_COUNT), "Max location parametrs count reached");
	}

	GDINL HRIShaderParamDesc::~HRIShaderParamDesc()
	{
		auto const ParamLocationDesc = const_cast<HRIShaderParamLocationDesc*>(self->GetLocationDesc());
		ParamLocationDesc->LocationInstancesSize -= self->GetParamSize();
		ParamLocationDesc->LocationInstancesCount -= 1;
	}

	GDINL size_t HRIShaderParamDesc::GetParamSize() const
	{
		if (self->ParamCachedSize == SIZE_MAX) {	// Precaching param size.
			switch (self->ParamType) {
				case GD_HRI_SHADER_PARAM_DESC_TYPE_TEXTURE2D:
				case GD_HRI_SHADER_PARAM_DESC_TYPE_TEXTURECUBE:	return (self->ParamCachedSize = self->ParamArrayLength * sizeof(HRIObject const*));
				case GD_HRI_SHADER_PARAM_DESC_TYPE_MATRIX4X4:	return (self->ParamCachedSize = self->ParamArrayLength * sizeof(Matrix4x4));
				case GD_HRI_SHADER_PARAM_DESC_TYPE_MATRIX3X3:	return (self->ParamCachedSize = self->ParamArrayLength * sizeof(Matrix3));
				case GD_HRI_SHADER_PARAM_DESC_TYPE_STRING:		return (self->ParamCachedSize = self->ParamArrayLength * sizeof(HRIShaderChar) * GD_HRI_SHADER_PARAM_MAX_STRING_LENGTH);
				case GD_HRI_SHADER_PARAM_DESC_TYPE_FORMATABLE:	return (self->ParamCachedSize = self->ParamArrayLength * GD_FORMAT_SIZEOF(self->ParamFormat));
			}

			return (self->ParamCachedSize = 0);
		}

		return self->ParamCachedSize;
	}

	GDINL HRIShaderParamLocationDesc const* HRIShaderParamDesc::GetLocationDesc() const 
	{
		return object_cast<HRIShaderParamLocationDesc const*>(self->GetParentObject());
	}

	GDINL HRIShaderParamLocationDesc* HRIShaderParamDesc::GetLocationDesc()
	{
		return object_cast<HRIShaderParamLocationDesc*>(self->GetParentObject());
	}

	/// ==========================================================================================
	/// HRIShaderParam
	/// ==========================================================================================

	GDINL HRIShaderParamLocation const* HRIShaderParam::GetShaderParamLocation() const
	{
		return object_cast<HRIShaderParamLocation const*>(self->GetParentObject());
	}

	GDINL HRIShaderParamLocation* HRIShaderParam::GetShaderParamLocation()
	{
		return object_cast<HRIShaderParamLocation*>(self->GetParentObject());
	}

	GDINL HRIShaderParam const* HRIShaderParam::GetNextSameParam() const
	{
		return object_cast<HRIShaderParam const*>(self->GetNextSiblingObject());
	}

	GDINL HRIShaderParam* HRIShaderParam::GetNextSameParam()
	{
		return object_cast<HRIShaderParam*>(self->GetNextSiblingObject());
	}

	template<class HRIShaderParamValueType>
	GDINL void HRIShaderParam::SetValue(HRIShaderParamValueType const& ParamValue)
	{
		GD_DEBUG_ASSERT((FormatTypeSelector<HRIShaderParamValueType>::SpecifiedFormat == self->ParamDesc->ParamFormat), "Invalid format parameter");
		self->SetValue(chandle(&ParamValue));
	}

	/// ==========================================================================================
	/// HRIShaderParamLocationDesc
	/// ==========================================================================================

	GDINL HRIShaderParamLocationDesc::HRIShaderParamLocationDesc(HRIShaderInstanceDesc* const LocationShaderInstanceDesc, HRIShaderParamLocationDescType const LocationType)
		: HRIObject(HRIObject::TreeLockingFlagsAll, LocationShaderInstanceDesc), LocationType(LocationType)
	{
		GD_DEBUG_ASSERT((self->GetParentObject() != nullptr), "Invalid instance desc specified");
		GD_DEBUG_ASSERT((self->LocationType < GD_HRI_SHADER_PARAM_DESC_TYPES_COUNT), "Invalid location type specified");
	}

	/// ==========================================================================================
	/// HRIShaderParamLocation
	/// ==========================================================================================

	GDINL HRIShaderInstance const* HRIShaderParamLocation::GetLocationInstance() const
	{
		return object_cast<HRIShaderInstance const*>(self->GetParentObject());
	}

	GDINL HRIShaderInstance* HRIShaderParamLocation::GetLocationInstance() 
	{
		return object_cast<HRIShaderInstance*>(self->GetParentObject());
	}

	GDINL HRIShaderParam const* HRIShaderParamLocation::GetParamByName(String const& ParamName) const
	{
		HashSumm const ParamNameHash = ParamName.GetHashSumm();
		return self->FindFirstChildObject<HRIShaderParam const>([ParamNameHash](HRIShaderParam const* const LocationShaderParam) -> bool { 
			return (LocationShaderParam->ParamDesc->ParamHash == ParamNameHash); 
		});
	}

	GDINL HRIShaderParam* HRIShaderParamLocation::GetParamByName(String const& ParamName)
	{
		return const_cast<HRIShaderParam*>(const_cast<HRIShaderParamLocation const*>(self)->GetParamByName(ParamName));
	}
	
	HRIShaderParamLocationConstantBufferEmulated::HRIShaderParamLocationConstantBufferEmulated(HRIShaderInstance* const ShaderInstance, HRIShaderParamLocationDesc const* const LocationDesc)
		: HRIShaderParamLocationConstantBufferBase(ShaderInstance, LocationDesc)
	{
	}

	void HRIShaderParamLocationConstantBufferEmulated::UploadAllParameters()
	{
		if (self->DidAnyParamValueChanged) {
			for (auto Param : IterateChildObjects<HRIShaderParam const>(self)) {
				if (HRIShaderParamLocation::QueryAndResetDidParamValueChanged(Param)) {
					self->UploadSingleParameter(Param);
				}
			}
			self->DidAnyParamValueChanged = false;
		}
	}

	HRIShaderParamLocationResources::HRIShaderParamLocationResources(HRIShaderInstance* const ShaderInstance, HRIShaderParamLocationDesc const* const LocationDesc)
		: HRIShaderParamLocation(ShaderInstance, LocationDesc)
	{
	}


	/// ==========================================================================================
	/// HRIShaderInstanceDesc
	/// ==========================================================================================

	GDINL HRIShaderInstanceDesc::HRIShaderInstanceDesc(UInt64 const InstanceInputFormat, UInt64 const InstanceOutputFormat) 
		: InstanceInputFormat (InstanceInputFormat )
		, InstanceOutputFormat(InstanceOutputFormat)
	{
		// GD_DEBUG_ASSERT((self->GetInstanceShader() != nullptr), "Invalid instance shader specified");
		// GD_DEBUG_ASSERT((self->InstanceVertexFormat >= VertexFormat()), "Invalid vertex format specified");
	}

	GDINL HRIShader const* HRIShaderInstanceDesc::GetInstanceShader() const
	{
		return object_cast<HRIShader const*>(self->GetParentObject());
	}

	GDINL HRIShader* HRIShaderInstanceDesc::GetInstanceShader() 
	{
		return object_cast<HRIShader*>(self->GetParentObject());
	}

	GDINL HRIShaderParamLocationDesc const* HRIShaderInstanceDesc::GetFirstLocationDesc(HRIShaderParamLocationDescType const InstanceLocationDescType) const
	{
		return self->FindFirstChildObject<HRIShaderParamLocationDesc const>(
			[InstanceLocationDescType](HRIShaderParamLocationDesc const* HRIShaderParamLocationDesc) -> bool { return (HRIShaderParamLocationDesc->LocationType == InstanceLocationDescType); }
		);
	}

	/// @todo Determine what this originlly does.
	/*GDINL HRIShaderParamLocationDesc const* HRIShaderInstanceDesc::GetNextLocationDesc(HRIShaderParamLocationDescType const InstanceLocationDescType) const
	{
		auto const NextShaderParamLocationDesc = object_cast<HRIShaderParamLocationDesc const*>(self->GetNextSiblingObject());
		if (NextShaderParamLocationDesc != nullptr)
		{
			if (NextShaderParamLocationDesc->LocationType == InstanceLocationDescType)
			{
				return NextShaderParamLocationDesc;
			}
		}

		return nullptr;
	}*/

	/// ==========================================================================================
	/// HRIShaderInstance
	/// ==========================================================================================

	GDINL HRIShaderParam const* HRIShaderInstance::GetParamByName(String const& ParamName) const
	{
		HRIShaderParam const* HRIShaderParam = nullptr;
		self->FindFirstChildObject<HRIShaderParamLocation const>(
			[&HRIShaderParam, &ParamName](HRIShaderParamLocation const* shaderLocation) -> bool { return (HRIShaderParam = shaderLocation->GetParamByName(ParamName)) != nullptr; }
		);

		return HRIShaderParam;
	}

	GDINL HRIShaderParam* HRIShaderInstance::GetParamByName(String const& ParamName)
	{
		return const_cast<HRIShaderParam*>(const_cast<HRIShaderInstance const*>(self)->GetParamByName(ParamName));
	}

	/// ==========================================================================================
	/// HRIShaderCtorInfo
	/// ==========================================================================================

	GDINL HRIShaderCtorInfo::HRIShaderCtorInfo(
		HRIShaderProgram*     const  CreationShaderProgram,
		chandle               const  CreationData,
		size_t                const  CreationDataSize,
		HRIShaderInstanceDesc const* CreationInstanceDesc)
		: CreationShaderProgram(CreationShaderProgram)
		, CreationData         (CreationData         )
		, CreationDataSize     (CreationDataSize     )
		, CreationInstanceDesc (CreationInstanceDesc )
	{	
	}

	GDINL HRIShaderCtorInfo::HRIShaderCtorInfo(HRIShaderCtorInfo const& CtorInfo) 
		: CreationShaderProgram(CtorInfo.CreationShaderProgram)
		, CreationData         (CtorInfo.CreationData         )
		, CreationDataSize     (CtorInfo.CreationDataSize     )
		, CreationInstanceDesc (CtorInfo.CreationInstanceDesc )
	{
	}

	/// ==========================================================================================
	/// HRIShader
	/// ==========================================================================================

	GDINL HRIShader::HRIShader(HRIShaderCtorInfo const& CtorInfo, HRIShaderType const HRIShaderType)
		: HRIObject(HRIObject::TreeLockingFlagsAll, CtorInfo.CreationShaderProgram)
		, ShaderDesc(CtorInfo.CreationInstanceDesc), Type(HRIShaderType)
	{
		GD_DEBUG_ASSERT((self->ShaderDesc != nullptr), "Invalid shader desc specified");
		GD_DEBUG_ASSERT((self->Type < GD_HRI_SHADER_TYPE_UNKNOWN), "Invalid shader type specified");
		GD_DEBUG_ASSERT((self->GetShaderProgram() != nullptr), "Invalid shader program specified");
		GD_DEBUG_ASSERT((self->GetShaderProgram()->ProgramShaders[self->Type] == nullptr), "Shader of this type is already attached to this shader program");

		self->GetShaderProgram()->ProgramShaders[self->Type] = self;
	}

	GDINL HRIShader::~HRIShader()
	{
		self->GetShaderProgram()->ProgramShaders[self->Type] = nullptr;
	}

	GDINL HRIShaderProgram const* HRIShader::GetShaderProgram() const
	{
		return object_cast<HRIShaderProgram const*>(self->GetParentObject());
	}

	GDINL HRIShaderProgram* HRIShader::GetShaderProgram() 
	{
		return object_cast<HRIShaderProgram*>(self->GetParentObject());
	}

	/// ==========================================================================================
	/// HRIShaderProgram
	/// ==========================================================================================

	GDINL HRIShaderProgram::HRIShaderProgram()
	{
		ZeroMemory(&self->ProgramShaders, sizeof(self->ProgramShaders));
	}

	GDINL HRIShader const* HRIShaderProgram::GetProgramShader(HRIShaderType const Type) const
	{
		GD_DEBUG_ASSERT((Type < GD_HRI_SHADER_TYPES_COUNT), "Invalid shader type specified");
		return self->ProgramShaders[Type];
	}

	GDINL HRIShader* HRIShaderProgram::GetProgramShader(HRIShaderType const Type) 
	{
		GD_DEBUG_ASSERT((Type < GD_HRI_SHADER_TYPES_COUNT), "Invalid shader type specified");
		return self->ProgramShaders[Type];
	}

GD_NAMESPACE_END
