/// ==========================================================================================
/// HRIShader.cpp: shader implementation
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// 
/// History:
///		* --.01.2014 - Created by James Jhuighuy
///  * 05.05.2014 - Rewritten to match cross compiler
/// ==========================================================================================

#include <GoddamnEngine/Engine/Renderer/Renderer.h>
#include <GoddamnEngine/Engine/Renderer/Shader/Shader.h>

GD_NAMESPACE_BEGIN
	
	extern Str HRIShaderTypeToStr(HRIShaderType const ShaderType)
	{
		Str static const HRIShaderTypeToStrTable[] = {
			/* GD_HRI_SHADER_TYPE_VERTEX   = */ "vertex",
			/* GD_HRI_SHADER_TYPE_HULL  = */ "hull",
			/* GD_HRI_SHADER_TYPE_DOMAIN   = */ "domain",
			/* GD_HRI_SHADER_TYPE_PIXEL = */ "pixel",
			/* GD_HRI_SHADER_TYPE_COMPUTE  = */ "compute",
			/* GD_HRI_SHADER_TYPE_GEOMETRY = */ "geometry",
		};

		GD_DEBUG_ASSERT(ShaderType < GD_HRI_SHADER_TYPES_COUNT, "Invalid shader type specified");
		return HRIShaderTypeToStrTable[ShaderType];
	}

	extern HRIShaderType HRIShaderTypeFromString(String const& ShaderType)
	{
		if (strncmp(ShaderType.CStr(), "compute",   sizeof("compute")  ) == 0) return GD_HRI_SHADER_TYPE_COMPUTE;
		if (strncmp(ShaderType.CStr(), "geometry",  sizeof("geometry") ) == 0) return GD_HRI_SHADER_TYPE_GEOMETRY;
		if (strncmp(ShaderType.CStr(), "vertex", sizeof("vertex")   ) == 0) return GD_HRI_SHADER_TYPE_VERTEX;
		if (strncmp(ShaderType.CStr(), "hull",   sizeof("hull")  ) == 0) return GD_HRI_SHADER_TYPE_HULL;
		if (strncmp(ShaderType.CStr(), "domain", sizeof("domain")   ) == 0) return GD_HRI_SHADER_TYPE_DOMAIN;
		if (strncmp(ShaderType.CStr(), "pixel",  sizeof("pixel") ) == 0) return GD_HRI_SHADER_TYPE_PIXEL;
#if (defined(GD_HRI_SHADER_OPENGL_ALISASING))
		if (strncmp(ShaderType.CStr(), "tess_ctrl", sizeof("tess_ctrl")) == 0) return GD_HRI_SHADER_TYPE_HULL;
		if (strncmp(ShaderType.CStr(), "tess_eval", sizeof("tess_eval")) == 0) return GD_HRI_SHADER_TYPE_DOMAIN;
		if (strncmp(ShaderType.CStr(), "fragment",  sizeof("fragment") ) == 0) return GD_HRI_SHADER_TYPE_PIXEL;
#endif	// if (defined(GD_HRI_SHADER_OPENGL_ALISASING))

		GD_DEBUG_ASSERT_FALSE("Invalid shader type string specified");
	}

	/// ==========================================================================================
	/// HRIShaderParamDesc
	/// ==========================================================================================
	
//	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIShaderParamDesc, HRIObject, GDAPI, nullptr);

	/// ==========================================================================================
	/// HRIShaderParam
	/// ==========================================================================================
	
//	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIShaderParam, HRIObject, GDAPI, nullptr);

	/// ==========================================================================================
	HRIShaderParam::HRIShaderParam(HRIShaderParamLocation* const ParamLocation, HRIShaderParamDesc const* const ParamDesc)
		: HRIObject(HRIObject::TreeLockingFlagsAll, ParamLocation), ParamDesc(ParamDesc)
	{
		GD_DEBUG_ASSERT((this->GetParentObject() != nullptr), "Invalid param location specified");
		GD_DEBUG_ASSERT((this->ParamDesc != nullptr), "Invalid param desc specified");
	}

	HRIShaderParam::~HRIShaderParam()
	{
		this->SetValue(nullptr);
	}

	void HRIShaderParam::SetValue(chandle const ParamValue)
	{
		this->GetShaderParamLocation()->DidAnyParamValueChanged = true;
		this->DidValueChanged = true;

		if (ParamValue != nullptr) {	// Parameter specified. So we need to copy new data.
			switch (this->ParamDesc->ParamType)	{	
				case GD_HRI_SHADER_PARAM_DESC_TYPE_TEXTURE2D:
				case GD_HRI_SHADER_PARAM_DESC_TYPE_TEXTURECUBE:	{
					HRIObject*  const NewValue = *reinterpret_cast<HRIObject* const*>(ParamValue);
					HRIObject*&    OldValue = *reinterpret_cast<HRIObject*   *>(&this->ParamValue);
					SafeRelease(OldValue);
					SafeObtain (OldValue = NewValue);
				} break;

				case GD_HRI_SHADER_PARAM_DESC_TYPE_MATRIX4X4:
				case GD_HRI_SHADER_PARAM_DESC_TYPE_MATRIX3X3:
				case GD_HRI_SHADER_PARAM_DESC_TYPE_FORMATABLE: {
					GD_DEBUG_ASSERT(this->ParamPointer != nullptr, "Param points to nullptr data");
					::memcpy(this->ParamPointer, ParamValue, this->ParamDesc->GetParamSize());
				} break;

				case GD_HRI_SHADER_PARAM_DESC_TYPE_STRING: {
					GD_DEBUG_ASSERT(this->ParamPointer != nullptr, "Param points to nullptr data");
					Str   NewString = reinterpret_cast<Str>(ParamValue) - 1;
					HRIShaderChar* OldString = reinterpret_cast<HRIShaderChar*>(this->ParamPointer) - 1;
					::memset(OldString, 0, this->ParamDesc->GetParamSize());
					while ((*(++OldString) = HRIShaderChar(*(++NewString))) != HRIShaderChar('\0'));
				} break;

				default: {
					GD_DEBUG_ASSERT_FALSE("Unhandled param type");
				} break;
			}
		} else { // Parameter unspecified. So we need to clear all data.
			if (   (this->ParamDesc->ParamType == GD_HRI_SHADER_PARAM_DESC_TYPE_TEXTURE2D  )
				|| (this->ParamDesc->ParamType == GD_HRI_SHADER_PARAM_DESC_TYPE_TEXTURECUBE)) {
				SafeRelease(reinterpret_cast<HRIObject*>(this->ParamValue));
			} else {
				memset(&this->ParamPointer, 0, this->ParamDesc->GetParamSize());
			}
		}
	}

	/// ==========================================================================================
	/// HRIShaderParamLocationDesc
	/// ==========================================================================================

//	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIShaderParamLocationDesc, HRIObject, GDAPI, nullptr);

	/// ==========================================================================================
	/// HRIShaderParamLocation
	/// ==========================================================================================

//	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIShaderParamLocation, HRIObject, GDAPI, nullptr);

	HRIShaderParamLocation::HRIShaderParamLocation(
		HRIShaderInstance      * const HRIShaderInstance,
		HRIShaderParamLocationDesc const* const LocationDesc)
		: HRIObject(HRIObject::TreeLockingFlagsAll, HRIShaderInstance)
		, LocationDesc(LocationDesc)
	{
		GD_DEBUG_ASSERT((this->LocationDesc != nullptr), "Invalid location desc specified");
		GD_DEBUG_ASSERT((this->GetLocationInstance() != nullptr), "Invalid instance specified");
		GD_DEBUG_ASSERT((object_cast<HRIShaderParamLocation const*>(this->GetLocationInstance()->GetLastChildObject())->LocationDesc->LocationType <= this->LocationDesc->LocationType), "Invalid instance specified");

		if (this->GetLocationInstance()->InstanceFirstLocations[this->LocationDesc->LocationType] == nullptr) {
			this->GetLocationInstance()->InstanceFirstLocations[this->LocationDesc->LocationType] = this;
		}

		for (auto const ShaderParamDesc : IterateChildObjects<HRIShaderParamDesc const>(this->LocationDesc)) {
			new HRIShaderParam(this, ShaderParamDesc);
		}
	}

	HRIShaderParamLocation::~HRIShaderParamLocation()
	{
		if (this->GetLocationInstance()->InstanceFirstLocations[this->LocationDesc->LocationType] == this) {
			this->GetLocationInstance()->InstanceFirstLocations[this->LocationDesc->LocationType] = object_cast<HRIShaderParamLocation*>(this->GetNextSiblingObject());
		}
	}

	/// ==========================================================================================
	/// HRIShaderParamLocationConstantBuffer class
	/// ==========================================================================================

	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIShaderParamLocationConstantBufferNative  , HRIShaderParamLocation, GDAPI, nullptr);
	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIShaderParamLocationConstantBufferEmulated, HRIShaderParamLocation, GDAPI, nullptr);

	HRIShaderParamLocationConstantBufferBase::HRIShaderParamLocationConstantBufferBase(HRIShaderInstance* const ShaderInstance, HRIShaderParamLocationDesc const* const LocationDesc)
		: HRIShaderParamLocation(ShaderInstance, LocationDesc)
		, LocationStorage(new UInt8[this->LocationDesc->GetLocationInstancesSize()])
	{
		GD_DEBUG_ASSERT(this->LocationDesc->LocationType == GD_HRI_SHADER_PARAM_LOCATION_DESC_TYPE_CONSTANTBUFFER, "Invalid shader location type specified");
		ZeroMemory(this->LocationStorage.GetPointer(), this->LocationDesc->GetLocationInstancesSize());

		size_t Offset = 0;
		for (auto const ShaderParam : IterateChildObjects<HRIShaderParam>(this)) {	// Pointing all params's memory to allocated block.
			HRIShaderParamLocation::SetParamPointer(ShaderParam, this->LocationStorage.GetPointer() + Offset);
			Offset += ShaderParam->ParamDesc->GetParamSize();
		}
	}

	HRIShaderParamLocationConstantBufferNative::HRIShaderParamLocationConstantBufferNative(HRIShaderInstance* const ShaderInstance, HRIShaderParamLocationDesc const* const LocationDesc)
		: HRIShaderParamLocationConstantBufferBase(ShaderInstance, LocationDesc)
		, LocationBuffer(HRInterface::GetInstance().CreateConstantBuffer(this->LocationDesc->GetLocationInstancesSize()))
	{
	}

	HRIShaderParamLocationConstantBufferNative::~HRIShaderParamLocationConstantBufferNative()
	{
		this->LocationBuffer->RemoveReference();
	}

	void HRIShaderParamLocationConstantBufferNative::UploadAllParameters()
	{
		if (this->DidAnyParamValueChanged) {	// Uploading all data to GPU. 
			this->DidAnyParamValueChanged = false;
			this->LocationBuffer->CopyDataFrom(this->GetStorage());
		}
	}

	/// ==========================================================================================
	/// HRIShaderParamLocationResources
	/// ==========================================================================================
	
	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIShaderParamLocationResources, HRIShaderParamLocation, GDAPI, nullptr);

	/// ==========================================================================================
	/// HRIShaderInstanceDesc
	/// ==========================================================================================

	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIShaderInstanceDesc, HRIObject, GDAPI, nullptr);

	/// ==========================================================================================
	/// HRIShaderInstance
	/// ==========================================================================================

	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIShaderInstance, HRIObject, GDAPI, nullptr);

	HRIShaderInstance::HRIShaderInstance(HRIShaderInstanceDesc const* const InstanceDesc)
		: HRIObject(HRIObject::TreeLockingFlagsNone), InstanceDesc(InstanceDesc)
	{
		GD_DEBUG_ASSERT((this->InstanceDesc != nullptr), "Invalid desc specified");
		ZeroMemory(&this->InstanceFirstLocations, sizeof(this->InstanceFirstLocations));
		for (auto const ShaderParamLocationDesc : IterateChildObjects<HRIShaderParamLocationDesc const>(this->InstanceDesc)) {
			switch (ShaderParamLocationDesc->LocationType) {
				case GD_HRI_SHADER_PARAM_LOCATION_DESC_TYPE_CONSTANTBUFFER: {
					HRInterface::GetInstance().CreateShaderParamLocationConstantBuffer(this, ShaderParamLocationDesc);
				} break;
				case GD_HRI_SHADER_PARAM_LOCATION_DESC_TYPE_RESOURCES: {
					HRInterface::GetInstance().CreateShaderParamLocationResources(this, ShaderParamLocationDesc);
				} break;
				default: {
					GD_DEBUG_ASSERT_FALSE("Invalid location type");
				} break;
			}
		}
	}

	/// ==========================================================================================
	/// HRIShader
	/// ==========================================================================================

	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIShader, HRIObject, GDAPI, nullptr);

	/// ==========================================================================================
	/// HRIShaderProgram
	/// ==========================================================================================

	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIShaderProgram, HRIObject, GDAPI, nullptr);

GD_NAMESPACE_END
