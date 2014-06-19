//////////////////////////////////////////////////////////////////////////
/// HRIShader.cc: shader implementation
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.01.2014 - Created by James Jhuighuy
///     * 05.05.2014 - Rewritten to match cross compiler
//////////////////////////////////////////////////////////////////////////

#include <GoddamnEngine/Engine/Renderer/Renderer.hh>
#include <GoddamnEngine/Engine/Renderer/Shader/Shader.hh>

GD_NAMESPACE_BEGIN
	
	extern Str HRIShaderTypeToStr(HRIShaderType const ShaderType)
	{
		Str static const HRIShaderTypeToStrTable[] = {
			/* GD_HRI_SHADER_TYPE_COMPUTE  = */ "compute",
			/* GD_HRI_SHADER_TYPE_GEOMETRY = */ "geometry",
			/* GD_HRI_SHADER_TYPE_VERTEX   = */ "vertex",
			/* GD_HRI_SHADER_TYPE_HULL     = */ "hull",
			/* GD_HRI_SHADER_TYPE_DOMAIN   = */ "domain",
			/* GD_HRI_SHADER_TYPE_PIXEL    = */ "pixel",
		};

		GD_DEBUG_ASSERT(ShaderType < GD_HRI_SHADER_TYPES_COUNT, "Invalid shader type specified");
		return HRIShaderTypeToStrTable[ShaderType];
	}

	extern HRIShaderType HRIShaderTypeFromString(String const& ShaderType)
	{
		if (strncmp(ShaderType.CStr(), "compute",   sizeof("compute")  ) == 0) return GD_HRI_SHADER_TYPE_COMPUTE;
		if (strncmp(ShaderType.CStr(), "geometry",  sizeof("geometry") ) == 0) return GD_HRI_SHADER_TYPE_GEOMETRY;
		if (strncmp(ShaderType.CStr(), "vertex",    sizeof("vertex")   ) == 0) return GD_HRI_SHADER_TYPE_VERTEX;
		if (strncmp(ShaderType.CStr(), "hull",      sizeof("hull")     ) == 0) return GD_HRI_SHADER_TYPE_HULL;
		if (strncmp(ShaderType.CStr(), "domain",    sizeof("domain")   ) == 0) return GD_HRI_SHADER_TYPE_DOMAIN;
		if (strncmp(ShaderType.CStr(), "pixel",     sizeof("pixel")    ) == 0) return GD_HRI_SHADER_TYPE_PIXEL;
#if (defined(GD_HRI_SHADER_OPENGL_ALISASING))
		if (strncmp(ShaderType.CStr(), "tess_ctrl", sizeof("tess_ctrl")) == 0) return GD_HRI_SHADER_TYPE_HULL;
		if (strncmp(ShaderType.CStr(), "tess_eval", sizeof("tess_eval")) == 0) return GD_HRI_SHADER_TYPE_DOMAIN;
		if (strncmp(ShaderType.CStr(), "fragment",  sizeof("fragment") ) == 0) return GD_HRI_SHADER_TYPE_PIXEL;
#endif	// if (defined(GD_HRI_SHADER_OPENGL_ALISASING))

		GD_ASSERT_FALSE("Invalid shader type string specified");
		return GD_HRI_SHADER_TYPE_UNKNOWN;
	}

	//////////////////////////////////////////////////////////////////////////
	/// HRIShaderParamDesc
	//////////////////////////////////////////////////////////////////////////
	
	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIShaderParamDesc, HRIObject, GDAPI, nullptr);

	//////////////////////////////////////////////////////////////////////////
	/// HRIShaderParam
	//////////////////////////////////////////////////////////////////////////
	
	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIShaderParam, HRIObject, GDAPI, nullptr);

	//////////////////////////////////////////////////////////////////////////
	HRIShaderParam::HRIShaderParam(HRIShaderParamLocation* const ParamLocation, HRIShaderParamDesc const* const ParamDesc)
		: HRIObject(HRIObject::TreeLockingFlagsAll, ParamLocation), ParamDesc(ParamDesc)
	{
		GD_DEBUG_ASSERT((self->GetParentObject() != nullptr), "Invalid param location specified");
		GD_DEBUG_ASSERT((self->ParamDesc != nullptr), "Invalid param desc specified");

		memset(&self->ParamValue, 0, sizeof(self->ParamValue));
		self->ParamDesc->AddReference();
	}

	HRIShaderParam::~HRIShaderParam()
	{
		self->SetValue(nullptr);
		self->ParamDesc->RemoveReference();
	}

	void HRIShaderParam::SetValue(chandle const ParamValue)
	{
		self->GetShaderParamLocation()->DidAnyParamValueChanged = true;
		self->DidValueChanged = true;

		if (ParamValue == nullptr)
		{	// Parameter unspecified. So we need to clear all data.
			if (   (self->ParamDesc->ParamType == GD_HRI_SHADER_PARAM_DESC_TYPE_TEXTURE2D  )
				|| (self->ParamDesc->ParamType == GD_HRI_SHADER_PARAM_DESC_TYPE_TEXTURECUBE))
				SafeRelease(reinterpret_cast<HRIObject*>(self->ParamValue));
			else
				memset(&self->ParamPointer, 0, self->ParamDesc->GetParamSize());
		}

		switch (self->ParamDesc->ParamType)
		{	// Parameter specified. So we need to copy new data.
		case GD_HRI_SHADER_PARAM_DESC_TYPE_TEXTURE2D:
		case GD_HRI_SHADER_PARAM_DESC_TYPE_TEXTURECUBE:
			{
				HRIObject*  const NewValue = *reinterpret_cast<HRIObject* const*>(ParamValue);
				HRIObject*&       OldValue = *reinterpret_cast<HRIObject*      *>(&self->ParamValue);
				SafeRelease(OldValue);
				SafeObtain (OldValue = NewValue);
			}	return;

		case GD_HRI_SHADER_PARAM_DESC_TYPE_MATRIX4X4:
		case GD_HRI_SHADER_PARAM_DESC_TYPE_MATRIX3X3:
		case GD_HRI_SHADER_PARAM_DESC_TYPE_FORMATABLE:
			{
				GD_DEBUG_ASSERT(self->ParamPointer != nullptr, "Param points to nullptr data");
				memcpy(self->ParamPointer, ParamValue, self->ParamDesc->GetParamSize());
			}	return;

		case GD_HRI_SHADER_PARAM_DESC_TYPE_STRING:
			{
				GD_DEBUG_ASSERT(self->ParamPointer != nullptr, "Param points to nullptr data");
				Str            NewString = reinterpret_cast<Str>(ParamValue) - 1;
				HRIShaderChar* OldString = reinterpret_cast<HRIShaderChar*>(self->ParamPointer) - 1;
				memset(OldString, 0, self->ParamDesc->GetParamSize());
				while ((*(++OldString) = HRIShaderChar(*(++NewString))) != HRIShaderChar('\0'));
			}	return;

		default: GD_ASSERT_FALSE("Unhandled param type"); return;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	/// HRIShaderParamLocationDesc
	//////////////////////////////////////////////////////////////////////////

	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIShaderParamLocationDesc, HRIObject, GDAPI, nullptr);

	//////////////////////////////////////////////////////////////////////////
	/// HRIShaderParamLocation
	//////////////////////////////////////////////////////////////////////////

	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIShaderParamLocation, HRIObject, GDAPI, nullptr);

	HRIShaderParamLocation::HRIShaderParamLocation(
		HRIShaderInstance               * const HRIShaderInstance,
		HRIShaderParamLocationDesc const* const LocationDesc)
		: HRIObject(HRIObject::TreeLockingFlagsAll, HRIShaderInstance)
		, LocationDesc(LocationDesc)
	{
		GD_DEBUG_ASSERT((self->LocationDesc != nullptr), "Invalid location desc specified");
		GD_DEBUG_ASSERT((self->GetLocationInstance() != nullptr), "Invalid instance specified");
		GD_DEBUG_ASSERT((object_cast<HRIShaderParamLocation const*>(self->GetLocationInstance()->GetLastChildObject())->LocationDesc->LocationType <= self->LocationDesc->LocationType), "Invalid instance specified");

		if (self->GetLocationInstance()->InstanceFirstLocations[self->LocationDesc->LocationType] == nullptr)
			self->GetLocationInstance()->InstanceFirstLocations[self->LocationDesc->LocationType] = self;

		for (auto const ShaderParamDesc : IterateChildObjects<HRIShaderParamDesc const>(self->LocationDesc))
			new HRIShaderParam(self, ShaderParamDesc);
	}

	HRIShaderParamLocation::~HRIShaderParamLocation()
	{
		if (self->GetLocationInstance()->InstanceFirstLocations[self->LocationDesc->LocationType] == self)
			self->GetLocationInstance()->InstanceFirstLocations[self->LocationDesc->LocationType] = object_cast<HRIShaderParamLocation*>(self->GetNextSiblingObject());
	}

	//////////////////////////////////////////////////////////////////////////
	/// HRIShaderParamLocationConstantBuffer class
	//////////////////////////////////////////////////////////////////////////

	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIShaderParamLocationConstantBufferNative  , HRIShaderParamLocation, GDAPI, nullptr);
	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIShaderParamLocationConstantBufferEmulated, HRIShaderParamLocation, GDAPI, nullptr);

	HRIShaderParamLocationConstantBufferBase::HRIShaderParamLocationConstantBufferBase(HRIShaderInstance* const ShaderInstance, HRIShaderParamLocationDesc const* const LocationDesc)
		: HRIShaderParamLocation(ShaderInstance, LocationDesc)
		, LocationStorage(new UInt8[self->LocationDesc->GetLocationInstancesSize()])
	{
		GD_DEBUG_ASSERT(self->LocationDesc->LocationType == GD_HRI_SHADER_PARAM_LOCATION_DESC_TYPE_CONSTANTBUFFER, "Invalid shader location type specified");
		ZeroMemory(self->LocationStorage.GetPointer(), self->LocationDesc->GetLocationInstancesSize());

		size_t Offset = 0;
		for (auto const ShaderParam : IterateChildObjects<HRIShaderParam>(self))
		{	// Pointing all params's memory to allocated block.
			ShaderParam->ParamPointer = self->LocationStorage.GetPointer() + Offset;
			Offset += ShaderParam->ParamDesc->GetParamSize();
		}
	}

	HRIShaderParamLocationConstantBufferNative::HRIShaderParamLocationConstantBufferNative(HRIShaderInstance* const ShaderInstance, HRIShaderParamLocationDesc const* const LocationDesc)
		: HRIShaderParamLocationConstantBufferBase(ShaderInstance, LocationDesc)
		, LocationBuffer(HRInterface::GetInstance().CreateConstantBuffer(self->LocationDesc->GetLocationInstancesSize()))
	{
	}

	HRIShaderParamLocationConstantBufferNative::~HRIShaderParamLocationConstantBufferNative()
	{
		self->LocationBuffer->RemoveReference();
	}

	void HRIShaderParamLocationConstantBufferNative::UploadAllParameters()
	{
		if (self->DidAnyParamValueChanged)
		{	// Uploading all data to GPU. 
			self->DidAnyParamValueChanged = false;
			self->LocationBuffer->CopyDataFrom(self->GetStorage());
		}
	}

	//////////////////////////////////////////////////////////////////////////
	/// HRIShaderParamLocationResources
	//////////////////////////////////////////////////////////////////////////
	
	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIShaderParamLocationResources, HRIShaderParamLocation, GDAPI, nullptr);

	//////////////////////////////////////////////////////////////////////////
	/// HRIShaderInstanceDesc
	//////////////////////////////////////////////////////////////////////////

	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIShaderInstanceDesc, HRIObject, GDAPI, nullptr);

	//////////////////////////////////////////////////////////////////////////
	/// HRIShaderInstance
	//////////////////////////////////////////////////////////////////////////

	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIShaderInstance, HRIObject, GDAPI, nullptr);

	HRIShaderInstance::HRIShaderInstance(HRIShaderInstanceDesc const* const InstanceDesc)
	 : HRIObject(HRIObject::TreeLockingFlagsNone), InstanceDesc(InstanceDesc)
	{
		GD_DEBUG_ASSERT((self->InstanceDesc != nullptr), "Invalid desc specified");
		ZeroMemory(&self->InstanceFirstLocations, sizeof(self->InstanceFirstLocations));
		for (auto const ShaderParamLocationDesc : IterateChildObjects<HRIShaderParamLocationDesc const>(self->InstanceDesc))
			/**/ if (ShaderParamLocationDesc->LocationType == GD_HRI_SHADER_PARAM_LOCATION_DESC_TYPE_CONSTANTBUFFER)
				HRInterface::GetInstance().CreateShaderParamLocationConstantBuffer(self, ShaderParamLocationDesc);
			else if (ShaderParamLocationDesc->LocationType == GD_HRI_SHADER_PARAM_LOCATION_DESC_TYPE_RESOURCES)
				HRInterface::GetInstance().CreateShaderParamLocationResources(self, ShaderParamLocationDesc);
			else { GD_ASSERT_FALSE("Invalid location type"); }
	}

	//////////////////////////////////////////////////////////////////////////
	/// HRIShader
	//////////////////////////////////////////////////////////////////////////

	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIShader, HRIObject, GDAPI, nullptr);

	//////////////////////////////////////////////////////////////////////////
	/// HRIShaderProgram
	//////////////////////////////////////////////////////////////////////////

	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIShaderProgram, HRIObject, GDAPI, nullptr);

GD_NAMESPACE_END
