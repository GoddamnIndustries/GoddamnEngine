/// ==========================================================================================
/// HRIShader.inl: shader inline implementation
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.01.2014 - Created by James Jhuighuy
///		* 06.02.2014 - Rewritten from scratch by James Jhuighuy
///  * 05.05.2014 - Rewritten to match cross compiler
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
		String       const& ParamName,
		HRIShaderParamDescType   const  ParamType,
		Format       const  ParamFormat   /* = GD_FORMAT_UNKNOWN       */,
		size_t       const  ParamArrayLength /* = 1           */,
		HRIShaderParamID   const  ParamID    /* = GD_HRI_SHADER_PARAM_DESC_ID_UNKNOWN */)
		: HRIObject(HRIObject::TreeLockingFlagsAll, ParamLocationDesc)
		, ParamName(ParamName), ParamHash  (ParamName.GetHashCode())
		, ParamType(ParamType), ParamFormat(ParamFormat), ParamArrayLength(ParamArrayLength)
		, ParamID(ParamID)
	{
		GD_DEBUG_ASSERT(  (this->GetParentObject() != nullptr), "Invalid instance desc specified");
		GD_DEBUG_ASSERT(  (this->ParamType  < GD_HRI_SHADER_PARAM_DESC_TYPE_UNKNOWN), "Invalid type specified");
		GD_DEBUG_ASSERT(!((this->ParamType == GD_HRI_SHADER_PARAM_DESC_TYPE_FORMATABLE) ^ (this->ParamFormat != GD_FORMAT_UNKNOWN)), "Invalid format specified");

		ParamLocationDesc->LocationInstancesSize  += this->GetParamSize();
		ParamLocationDesc->LocationInstancesCount += 1;

		GD_DEBUG_ASSERT((ParamLocationDesc->LocationInstancesSize  < GD_HRI_SHADER_PARAM_MAX_LOCATION_SIZE),  "Max location parametrs size reached" );
		GD_DEBUG_ASSERT((ParamLocationDesc->LocationInstancesCount < GD_HRI_SHADER_PARAM_MAX_LOCATION_COUNT), "Max location parametrs count reached");
	}

	GDINL HRIShaderParamDesc::~HRIShaderParamDesc()
	{
		auto const ParamLocationDesc = const_cast<HRIShaderParamLocationDesc*>(this->GetLocationDesc());
		ParamLocationDesc->LocationInstancesSize -= this->GetParamSize();
		ParamLocationDesc->LocationInstancesCount -= 1;
	}

	GDINL size_t HRIShaderParamDesc::GetParamSize() const
	{
		if (this->ParamCachedSize == SIZE_MAX) {	// Precaching param size.
			switch (this->ParamType) {
				case GD_HRI_SHADER_PARAM_DESC_TYPE_TEXTURE2D:
				case GD_HRI_SHADER_PARAM_DESC_TYPE_TEXTURECUBE:	return (this->ParamCachedSize = this->ParamArrayLength * sizeof(HRIObject const*));
				case GD_HRI_SHADER_PARAM_DESC_TYPE_MATRIX4X4:	return (this->ParamCachedSize = this->ParamArrayLength * sizeof(Matrix4x4));
				case GD_HRI_SHADER_PARAM_DESC_TYPE_MATRIX3X3:	return (this->ParamCachedSize = this->ParamArrayLength * sizeof(Matrix3));
				case GD_HRI_SHADER_PARAM_DESC_TYPE_STRING:		return (this->ParamCachedSize = this->ParamArrayLength * sizeof(HRIShaderChar) * GD_HRI_SHADER_PARAM_MAX_STRING_LENGTH);
				case GD_HRI_SHADER_PARAM_DESC_TYPE_FORMATABLE:	return (this->ParamCachedSize = this->ParamArrayLength * GD_FORMAT_SIZEOF(this->ParamFormat));
			}

			return (this->ParamCachedSize = 0);
		}

		return this->ParamCachedSize;
	}

	GDINL HRIShaderParamLocationDesc const* HRIShaderParamDesc::GetLocationDesc() const 
	{
		return object_cast<HRIShaderParamLocationDesc const*>(this->GetParentObject());
	}

	GDINL HRIShaderParamLocationDesc* HRIShaderParamDesc::GetLocationDesc()
	{
		return object_cast<HRIShaderParamLocationDesc*>(this->GetParentObject());
	}

	/// ==========================================================================================
	/// HRIShaderParam
	/// ==========================================================================================

	GDINL HRIShaderParamLocation const* HRIShaderParam::GetShaderParamLocation() const
	{
		return object_cast<HRIShaderParamLocation const*>(this->GetParentObject());
	}

	GDINL HRIShaderParamLocation* HRIShaderParam::GetShaderParamLocation()
	{
		return object_cast<HRIShaderParamLocation*>(this->GetParentObject());
	}

	GDINL HRIShaderParam const* HRIShaderParam::GetNextSameParam() const
	{
		return object_cast<HRIShaderParam const*>(this->GetNextSiblingObject());
	}

	GDINL HRIShaderParam* HRIShaderParam::GetNextSameParam()
	{
		return object_cast<HRIShaderParam*>(this->GetNextSiblingObject());
	}

	template<class HRIShaderParamValueType>
	GDINL void HRIShaderParam::SetValue(HRIShaderParamValueType const& ParamValue)
	{
		GD_DEBUG_ASSERT((FormatTypeSelector<HRIShaderParamValueType>::SpecifiedFormat == this->ParamDesc->ParamFormat), "Invalid format parameter");
		this->SetValue(chandle(&ParamValue));
	}

	/// ==========================================================================================
	/// HRIShaderParamLocationDesc
	/// ==========================================================================================

	GDINL HRIShaderParamLocationDesc::HRIShaderParamLocationDesc(HRIShaderInstanceDesc* const LocationShaderInstanceDesc, HRIShaderParamLocationDescType const LocationType)
		: HRIObject(HRIObject::TreeLockingFlagsAll, LocationShaderInstanceDesc), LocationType(LocationType)
	{
		GD_DEBUG_ASSERT((this->GetParentObject() != nullptr), "Invalid instance desc specified");
		GD_DEBUG_ASSERT((this->LocationType < GD_HRI_SHADER_PARAM_DESC_TYPES_COUNT), "Invalid location type specified");
	}

	/// ==========================================================================================
	/// HRIShaderParamLocation
	/// ==========================================================================================

	GDINL HRIShaderInstance const* HRIShaderParamLocation::GetLocationInstance() const
	{
		return object_cast<HRIShaderInstance const*>(this->GetParentObject());
	}

	GDINL HRIShaderInstance* HRIShaderParamLocation::GetLocationInstance() 
	{
		return object_cast<HRIShaderInstance*>(this->GetParentObject());
	}

	GDINL HRIShaderParam const* HRIShaderParamLocation::GetParamByName(String const& ParamName) const
	{
		HashCode const ParamNameHash = ParamName.GetHashCode();
		return this->FindFirstChildObject<HRIShaderParam const>([ParamNameHash](HRIShaderParam const* const LocationShaderParam) -> bool { 
			return (LocationShaderParam->ParamDesc->ParamHash == ParamNameHash); 
		});
	}

	GDINL HRIShaderParam* HRIShaderParamLocation::GetParamByName(String const& ParamName)
	{
		return const_cast<HRIShaderParam*>(const_cast<HRIShaderParamLocation const*>(this)->GetParamByName(ParamName));
	}
	
	HRIShaderParamLocationConstantBufferEmulated::HRIShaderParamLocationConstantBufferEmulated(HRIShaderInstance* const ShaderInstance, HRIShaderParamLocationDesc const* const LocationDesc)
		: HRIShaderParamLocationConstantBufferBase(ShaderInstance, LocationDesc)
	{
	}

	void HRIShaderParamLocationConstantBufferEmulated::UploadAllParameters()
	{
		if (this->DidAnyParamValueChanged) {
			for (auto Param : IterateChildObjects<HRIShaderParam const>(this)) {
				if (HRIShaderParamLocation::QueryAndResetDidParamValueChanged(Param)) {
					this->UploadSingleParameter(Param);
				}
			}
			this->DidAnyParamValueChanged = false;
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
		// GD_DEBUG_ASSERT((this->GetInstanceShader() != nullptr), "Invalid instance shader specified");
		// GD_DEBUG_ASSERT((this->InstanceVertexFormat >= VertexFormat()), "Invalid vertex format specified");
	}

	GDINL HRIShader const* HRIShaderInstanceDesc::GetInstanceShader() const
	{
		return object_cast<HRIShader const*>(this->GetParentObject());
	}

	GDINL HRIShader* HRIShaderInstanceDesc::GetInstanceShader() 
	{
		return object_cast<HRIShader*>(this->GetParentObject());
	}

	GDINL HRIShaderParamLocationDesc const* HRIShaderInstanceDesc::GetFirstLocationDesc(HRIShaderParamLocationDescType const InstanceLocationDescType) const
	{
		return this->FindFirstChildObject<HRIShaderParamLocationDesc const>(
			[InstanceLocationDescType](HRIShaderParamLocationDesc const* HRIShaderParamLocationDesc) -> bool { return (HRIShaderParamLocationDesc->LocationType == InstanceLocationDescType); }
		);
	}

	/// @todo Determine what this originlly does.
	/*GDINL HRIShaderParamLocationDesc const* HRIShaderInstanceDesc::GetNextLocationDesc(HRIShaderParamLocationDescType const InstanceLocationDescType) const
	{
		auto const NextShaderParamLocationDesc = object_cast<HRIShaderParamLocationDesc const*>(this->GetNextSiblingObject());
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
		this->FindFirstChildObject<HRIShaderParamLocation const>(
			[&HRIShaderParam, &ParamName](HRIShaderParamLocation const* shaderLocation) -> bool { return (HRIShaderParam = shaderLocation->GetParamByName(ParamName)) != nullptr; }
		);

		return HRIShaderParam;
	}

	GDINL HRIShaderParam* HRIShaderInstance::GetParamByName(String const& ParamName)
	{
		return const_cast<HRIShaderParam*>(const_cast<HRIShaderInstance const*>(this)->GetParamByName(ParamName));
	}

	/// ==========================================================================================
	/// HRIShaderCtorInfo
	/// ==========================================================================================

	GDINL HRIShaderCtorInfo::HRIShaderCtorInfo(
		HRIShaderProgram*  const  CreationShaderProgram,
		chandle      const  CreationData,
		size_t    const  CreationDataSize,
		HRIShaderInstanceDesc const* CreationInstanceDesc)
		: CreationShaderProgram(CreationShaderProgram)
		, CreationData   (CreationData   )
		, CreationDataSize  (CreationDataSize  )
		, CreationInstanceDesc (CreationInstanceDesc )
	{	
	}

	GDINL HRIShaderCtorInfo::HRIShaderCtorInfo(HRIShaderCtorInfo const& CtorInfo) 
		: CreationShaderProgram(CtorInfo.CreationShaderProgram)
		, CreationData   (CtorInfo.CreationData   )
		, CreationDataSize  (CtorInfo.CreationDataSize  )
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
		GD_DEBUG_ASSERT((this->ShaderDesc != nullptr), "Invalid shader desc specified");
		GD_DEBUG_ASSERT((this->Type < GD_HRI_SHADER_TYPE_UNKNOWN), "Invalid shader type specified");
		GD_DEBUG_ASSERT((this->GetShaderProgram() != nullptr), "Invalid shader program specified");
		GD_DEBUG_ASSERT((this->GetShaderProgram()->ProgramShaders[this->Type] == nullptr), "Shader of this type is already attached to this shader program");

		this->GetShaderProgram()->ProgramShaders[this->Type] = this;
	}

	GDINL HRIShader::~HRIShader()
	{
		this->GetShaderProgram()->ProgramShaders[this->Type] = nullptr;
	}

	GDINL HRIShaderProgram const* HRIShader::GetShaderProgram() const
	{
		return object_cast<HRIShaderProgram const*>(this->GetParentObject());
	}

	GDINL HRIShaderProgram* HRIShader::GetShaderProgram() 
	{
		return object_cast<HRIShaderProgram*>(this->GetParentObject());
	}

	/// ==========================================================================================
	/// HRIShaderProgram
	/// ==========================================================================================

	GDINL HRIShaderProgram::HRIShaderProgram()
	{
		ZeroMemory(&this->ProgramShaders, sizeof(this->ProgramShaders));
	}

	GDINL HRIShader const* HRIShaderProgram::GetProgramShader(HRIShaderType const Type) const
	{
		GD_DEBUG_ASSERT((Type < GD_HRI_SHADER_TYPES_COUNT), "Invalid shader type specified");
		return this->ProgramShaders[Type];
	}

	GDINL HRIShader* HRIShaderProgram::GetProgramShader(HRIShaderType const Type) 
	{
		GD_DEBUG_ASSERT((Type < GD_HRI_SHADER_TYPES_COUNT), "Invalid shader type specified");
		return this->ProgramShaders[Type];
	}

GD_NAMESPACE_END
