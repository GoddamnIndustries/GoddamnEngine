/// ==========================================================================================
/// D3D11Shader.cpp: Direct3D 11 shaders implementation. 
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.02.2014  - Created by James Jhuighuy
/// ==========================================================================================

#include <GoddamnEngine/Engine/Renderer/Impl/Direct3D11/D3D11Shader.h>

GD_NAMESPACE_BEGIN

	GD_TYPEINFORMATION_IMPLEMENTATION(HRID3D11ShaderProgram, HRIShaderProgram, GDINT);

	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRID3D11ShaderParamLocationResources, HRIShaderParamLocationResources, GDINT, nullptr);
	HRID3D11ShaderParamLocationResources::HRID3D11ShaderParamLocationResources(HRIShaderInstance* const ShaderInstance, HRIShaderParamLocationDesc const* const LocationDesc) 
		: HRIShaderParamLocationResources(ShaderInstance, LocationDesc)
	{
		GD_DEBUG_ASSERT(self->LocationDesc->LocationType == GD_HRI_SHADER_PARAM_LOCATION_DESC_TYPE_RESOURCES, "Invalid shader location type specified");
		self->ShaderResourceViews.Resize(self->LocationDesc->GetLocationInstancesCount());
		self->ShaderSampleStates .Resize(self->LocationDesc->GetLocationInstancesCount());
	}

	void HRID3D11ShaderParamLocationResources::UploadAllParameters()
	{
		if (self->DidAnyParamValueChanged) {
			self->DidAnyParamValueChanged = false;
			for (auto const ShaderParam : IterateChildObjects<HRIShaderParam const>(self)) {
				if (HRIShaderParamLocation::QueryAndResetDidParamValueChanged(ShaderParam))	{
					HRID3D11Texture2D const* const Texture2D = object_cast<HRID3D11Texture2D const*>(ShaderParam->GetValue());
					if (Texture2D != nullptr) {
						self->ShaderResourceViews[ShaderParam->ParamDesc->GetParamID()] = Texture2D->GetShaderResource();
						self->ShaderSampleStates[ShaderParam->ParamDesc->GetParamID()] = Texture2D->GetSample();
					}
				}
			}
		}
	}

	/// ==========================================================================================
	/// HRID3D11ShaderInstance class
	/// ==========================================================================================

	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRID3D11ShaderInstance, HRIShaderInstance, GDINT, nullptr);

	HRID3D11ShaderInstance::HRID3D11ShaderInstance(HRIShaderInstanceDesc const* const InstanceDesc) : HRIShaderInstance(InstanceDesc)
	{	// Caching all inner constant buffer.
		for (auto const Location : IterateChildObjects<HRIShaderParamLocation const>(self)) {
			if (Location->LocationDesc->LocationType == GD_HRI_SHADER_PARAM_LOCATION_DESC_TYPE_CONSTANTBUFFER) {
				HRID3D11ShaderParamLocationConstantBuffer const* const ConstantBufferLocation = object_cast<HRID3D11ShaderParamLocationConstantBuffer const*>(Location);
				self->ShaderConstantBuffers.PushLast(reinterpret_cast<ID3D11Buffer const*>(ConstantBufferLocation->GetBuffer()->GetNativePointer()));
			}
		}
	}

	/// ==========================================================================================
	/// HRID3D11VertexShader class
	/// ==========================================================================================

	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRID3D11VertexShader, HRIVertexShader, GDINT, nullptr);
	HRID3D11VertexShader::HRID3D11VertexShader(HRIShaderCtorInfo const& CtorInfo) : HRIVertexShader(CtorInfo)
	{
		HRESULT Result = E_FAIL;
		ID3D11Device* const Device = HRD3D11Interface::GetInstance().Device.Get();	
		if (FAILED(Result = Device->CreateVertexShader(CtorInfo.CreationData, CtorInfo.CreationDataSize, nullptr, &self->VertexShader))) {
			throw D3D11Exception("Failed to create vertex shader");
		}
		
		UINT                     InputLayoutInputSlot = 0;
		D3D11_INPUT_ELEMENT_DESC InputLayoutElements[15];
		for (size_t SemanticIter = GD_HRI_SEMANTIC_FIRST; SemanticIter < GD_HRI_SEMANTIC_UNKNOWN; SemanticIter += 1) {	// Setting up all upcoming semantic from shader.
			if ((self->ShaderDesc->InstanceInputFormat & GD_BIT(UInt64(SemanticIter + 1))) == 0) {	// This semantic is not used, we not need to mention it in layout.
				continue;
			}

			HRISemantic     const  Semantic = static_cast<HRISemantic>(SemanticIter);
			HRISemanticDesc const& SemanticDesc = HRISemanticGetDesc(Semantic);

			D3D11_INPUT_ELEMENT_DESC& InputLayoutElement = InputLayoutElements[InputLayoutInputSlot];
			ZeroMemory(&InputLayoutElement, sizeof(InputLayoutElement));
			InputLayoutElement.SemanticName         = SemanticDesc.Name;
			InputLayoutElement.SemanticIndex        = 0;
			InputLayoutElement.Format               = ToDxgiFormat(SemanticDesc.SlotFormat);
			InputLayoutElement.InputSlot            = InputLayoutInputSlot;
			InputLayoutElement.AlignedByteOffset    = 0;
			InputLayoutElement.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
			InputLayoutElement.InstanceDataStepRate = 0;
			InputLayoutInputSlot += 1;
		}

		if (FAILED(Result = Device->CreateInputLayout(&InputLayoutElements[0], InputLayoutInputSlot, CtorInfo.CreationData, CtorInfo.CreationDataSize, &self->InputLayout))) {
			throw D3D11Exception("Failed to create input layout");
		}
	}

	void HRID3D11VertexShader::BindShader(HRIShaderInstance const* const TheShaderInstance) const
	{
		ID3D11DeviceContext* const Context = HRD3D11Interface::GetInstance().Context.Get();
		Context->IASetInputLayout(self->InputLayout .Get()            );
		Context->VSSetShader     (self->VertexShader.Get(), nullptr, 0);
		if (TheShaderInstance != nullptr) {	// Instance is specified, it may contain constant buffer and some other resources that are required to be sent to GPU.
			HRID3D11ShaderInstance               const* const ShaderInstance  = object_cast<HRID3D11ShaderInstance const*>(TheShaderInstance);
			HRID3D11ShaderParamLocationResources const* const ShaderResources = object_cast<HRID3D11ShaderParamLocationResources const*>(ShaderInstance->GetInstanceResourcesLocation());
			
			UINT const LocationConstantBuffersCount = static_cast<UINT>(ShaderInstance->ShaderConstantBuffers.GetSize());
			if (LocationConstantBuffersCount != 0) {
				Context->VSSetConstantBuffers(0, LocationConstantBuffersCount, const_cast<ID3D11Buffer* const*>(&ShaderInstance->ShaderConstantBuffers[0]));
			}
			
			if (ShaderResources != nullptr)	{	// Our shader also cointains resources.
				UINT const LocationResourcesCount = static_cast<UINT>(ShaderResources->LocationDesc->GetLocationInstancesCount());
				if (LocationResourcesCount != 0) {
					Context->VSSetShaderResources(0, LocationResourcesCount, const_cast<ID3D11ShaderResourceView* const*>(&ShaderResources->ShaderResourceViews[0]));
					Context->VSSetSamplers       (0, LocationResourcesCount, const_cast<ID3D11SamplerState      * const*>(&ShaderResources->ShaderSampleStates [0]));
				}
			}
		}
	}

	void HRID3D11VertexShader::UnbindShader() const
	{
		ID3D11DeviceContext* const Context = HRD3D11Interface::GetInstance().Context.Get();
		Context->IASetInputLayout    (      nullptr            );
		Context->VSSetShaderResources(0, 0, nullptr            );
		Context->VSSetSamplers       (0, 0, nullptr            );
		Context->VSSetConstantBuffers(0, 0, nullptr            );
		Context->VSSetShader         (      nullptr, nullptr, 0);
	}

	/// ==========================================================================================
	/// HRID3D11HullShader class
	/// ==========================================================================================

	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRID3D11HullShader, HRIHullShader, GDINT, nullptr);
	HRID3D11HullShader::HRID3D11HullShader(HRIShaderCtorInfo const& CtorInfo) : HRIHullShader(CtorInfo)
	{
		HRESULT Result = E_FAIL;
		if (FAILED(Result = HRD3D11Interface::GetInstance().Device->CreateHullShader(CtorInfo.CreationData, CtorInfo.CreationDataSize, nullptr, &self->HullShader))) {
			throw D3D11Exception("Failed to create Hull shader");
		}
	}

	void HRID3D11HullShader::BindShader(HRIShaderInstance const* const TheShaderInstance) const
	{
		ID3D11DeviceContext* const Context = HRD3D11Interface::GetInstance().Context.Get();
		Context->HSSetShader(self->HullShader.Get(), nullptr, 0);
		if (TheShaderInstance != nullptr) {	// Instance is specified, it may contain constant buffer and some other resources that are required to be sent to GPU.
			HRID3D11ShaderInstance               const* const ShaderInstance  = object_cast<HRID3D11ShaderInstance const*>(TheShaderInstance);
			HRID3D11ShaderParamLocationResources const* const ShaderResources = object_cast<HRID3D11ShaderParamLocationResources const*>(ShaderInstance->GetInstanceResourcesLocation());
			
			UINT const LocationConstantBuffersCount = static_cast<UINT>(ShaderInstance->ShaderConstantBuffers.GetSize());
			if (LocationConstantBuffersCount != 0) {
				Context->HSSetConstantBuffers(0, LocationConstantBuffersCount, const_cast<ID3D11Buffer* const*>(&ShaderInstance->ShaderConstantBuffers[0]));
			}
			
			if (ShaderResources != nullptr)	{	// Our shader also cointains resources.
				UINT const LocationResourcesCount = static_cast<UINT>(ShaderResources->LocationDesc->GetLocationInstancesCount());
				if (LocationResourcesCount != 0) {
					Context->HSSetShaderResources(0, LocationResourcesCount, const_cast<ID3D11ShaderResourceView* const*>(&ShaderResources->ShaderResourceViews[0]));
					Context->HSSetSamplers       (0, LocationResourcesCount, const_cast<ID3D11SamplerState      * const*>(&ShaderResources->ShaderSampleStates [0]));
				}
			}
		}
	}

	void HRID3D11HullShader::UnbindShader() const
	{
		ID3D11DeviceContext* const Context = HRD3D11Interface::GetInstance().Context.Get();
		Context->HSSetShaderResources(0, 0, nullptr            );
		Context->HSSetSamplers       (0, 0, nullptr            );
		Context->HSSetConstantBuffers(0, 0, nullptr            );
		Context->HSSetShader         (      nullptr, nullptr, 0);
	}

	/// ==========================================================================================
	/// HRID3D11DomainShader class
	/// ==========================================================================================

	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRID3D11DomainShader, HRIDomainShader, GDINT, nullptr);
	HRID3D11DomainShader::HRID3D11DomainShader(HRIShaderCtorInfo const& CtorInfo) : HRIDomainShader(CtorInfo)
	{
		HRESULT Result = E_FAIL;
		if (FAILED(Result = HRD3D11Interface::GetInstance().Device->CreateDomainShader(CtorInfo.CreationData, CtorInfo.CreationDataSize, nullptr, &self->DomainShader))) {
			throw D3D11Exception("Failed to create Domain shader");
		}
	}

	void HRID3D11DomainShader::BindShader(HRIShaderInstance const* const TheShaderInstance) const
	{
		ID3D11DeviceContext* const Context = HRD3D11Interface::GetInstance().Context.Get();
		Context->DSSetShader(self->DomainShader.Get(), nullptr, 0);
		if (TheShaderInstance != nullptr) {	// Instance is specified, it may contain constant buffer and some other resources that are required to be sent to GPU.
			HRID3D11ShaderInstance               const* const ShaderInstance  = object_cast<HRID3D11ShaderInstance const*>(TheShaderInstance);
			HRID3D11ShaderParamLocationResources const* const ShaderResources = object_cast<HRID3D11ShaderParamLocationResources const*>(ShaderInstance->GetInstanceResourcesLocation());
			
			UINT const LocationConstantBuffersCount = static_cast<UINT>(ShaderInstance->ShaderConstantBuffers.GetSize());
			if (LocationConstantBuffersCount != 0) {
				Context->DSSetConstantBuffers(0, LocationConstantBuffersCount, const_cast<ID3D11Buffer* const*>(&ShaderInstance->ShaderConstantBuffers[0]));
			}
			
			if (ShaderResources != nullptr) {	// Our shader also cointains resources.
				UINT const LocationResourcesCount = static_cast<UINT>(ShaderResources->LocationDesc->GetLocationInstancesCount());
				if (LocationResourcesCount != 0) {
					Context->DSSetShaderResources(0, LocationResourcesCount, const_cast<ID3D11ShaderResourceView* const*>(&ShaderResources->ShaderResourceViews[0]));
					Context->DSSetSamplers       (0, LocationResourcesCount, const_cast<ID3D11SamplerState      * const*>(&ShaderResources->ShaderSampleStates [0]));
				}
			}
		}
	}

	void HRID3D11DomainShader::UnbindShader() const
	{
		ID3D11DeviceContext* const Context = HRD3D11Interface::GetInstance().Context.Get();
		Context->DSSetShaderResources(0, 0, nullptr            );
		Context->DSSetSamplers       (0, 0, nullptr            );
		Context->DSSetConstantBuffers(0, 0, nullptr            );
		Context->DSSetShader         (      nullptr, nullptr, 0);
	}

	/// ==========================================================================================
	/// HRID3D11PixelShader class
	/// ==========================================================================================

	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRID3D11PixelShader, HRIPixelShader, GDINT, nullptr);
	HRID3D11PixelShader::HRID3D11PixelShader(HRIShaderCtorInfo const& CtorInfo) : HRIPixelShader(CtorInfo)
	{
		HRESULT Result = E_FAIL;
		if (FAILED(Result = HRD3D11Interface::GetInstance().Device->CreatePixelShader(CtorInfo.CreationData, CtorInfo.CreationDataSize, nullptr, &self->PixelShader))) {
			throw D3D11Exception("Failed to create Pixel shader");
		}
	}

	void HRID3D11PixelShader::BindShader(HRIShaderInstance const* const TheShaderInstance) const
	{
		ID3D11DeviceContext* const Context = HRD3D11Interface::GetInstance().Context.Get();
		Context->PSSetShader(self->PixelShader.Get(), nullptr, 0);
		if (TheShaderInstance != nullptr) {	// Instance is specified, it may contain constant buffer and some other resources that are required to be sent to GPU.
			HRID3D11ShaderInstance               const* const ShaderInstance  = object_cast<HRID3D11ShaderInstance const*>(TheShaderInstance);
			HRID3D11ShaderParamLocationResources const* const ShaderResources = object_cast<HRID3D11ShaderParamLocationResources const*>(ShaderInstance->GetInstanceResourcesLocation());
			UINT const LocationConstantBuffersCount = static_cast<UINT>(ShaderInstance->ShaderConstantBuffers.GetSize());
			if (LocationConstantBuffersCount != 0) {
				Context->PSSetConstantBuffers(0, LocationConstantBuffersCount, const_cast<ID3D11Buffer* const*>(&ShaderInstance->ShaderConstantBuffers[0]));
			}
			if (ShaderResources != nullptr) {	// Our shader also cointains shader resources.
				UINT const LocationResourcesCount = static_cast<UINT>(ShaderResources->LocationDesc->GetLocationInstancesCount());
				if (LocationResourcesCount != 0) {
					Context->PSSetShaderResources(0, LocationResourcesCount, const_cast<ID3D11ShaderResourceView* const*>(&ShaderResources->ShaderResourceViews[0]));
					Context->PSSetSamplers       (0, LocationResourcesCount, const_cast<ID3D11SamplerState      * const*>(&ShaderResources->ShaderSampleStates [0]));
				}
			}
		}
	}

	void HRID3D11PixelShader::UnbindShader() const
	{
		ID3D11DeviceContext* const Context = HRD3D11Interface::GetInstance().Context.Get();
		Context->PSSetShaderResources(0, 0, nullptr            );
		Context->PSSetSamplers       (0, 0, nullptr            );
		Context->PSSetConstantBuffers(0, 0, nullptr            );
		Context->PSSetShader         (      nullptr, nullptr, 0);
	}

	/// ==========================================================================================
	/// HRD3D11Interface class
	/// ==========================================================================================

	HRIShaderParamLocation* HRD3D11Interface::CreateShaderParamLocationConstantBuffer(HRIShaderInstance* const ShaderInstance, HRIShaderParamLocationDesc const* const LocationDesc)
	{
		return new HRID3D11ShaderParamLocationConstantBuffer(ShaderInstance, LocationDesc);
	}

	HRIShaderParamLocation* HRD3D11Interface::CreateShaderParamLocationResources(HRIShaderInstance* const ShaderInstance, HRIShaderParamLocationDesc const* const LocationDesc)
	{
		return new HRID3D11ShaderParamLocationResources(ShaderInstance, LocationDesc);
	}

	HRIShaderInstance* HRD3D11Interface::CreateShaderInstance(HRIShaderInstanceDesc const* const InstanceDesc)
	{
		return new HRID3D11ShaderInstance(InstanceDesc);
	}

	HRIVertexShader* HRD3D11Interface::CreateVertexShader(HRIShaderCtorInfo const& CtorInfo) { return new HRID3D11VertexShader(CtorInfo); }
	HRIHullShader  * HRD3D11Interface::CreateHullShader  (HRIShaderCtorInfo const& CtorInfo) { return new HRID3D11HullShader  (CtorInfo); }
	HRIDomainShader* HRD3D11Interface::CreateDomainShader(HRIShaderCtorInfo const& CtorInfo) { return new HRID3D11DomainShader(CtorInfo); }
	HRIPixelShader * HRD3D11Interface::CreatePixelShader (HRIShaderCtorInfo const& CtorInfo) { return new HRID3D11PixelShader (CtorInfo); }

	HRIShaderProgram* HRD3D11Interface::CreateShaderProgram()
	{
		return new HRID3D11ShaderProgram();
	}

GD_NAMESPACE_END
