/// ==========================================================================================
/// D3D11Shader.h: Direct3D 11 shaders implementation interface. 
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.02.2014  - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_ENGINE_RENDERER_IMPL_D3D11_SHADER
#define GD_ENGINE_RENDERER_IMPL_D3D11_SHADER

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Renderer/Shader/Shader.h>
#include <GoddamnEngine/Engine/Renderer/Impl/Direct3D11/D3D11Renderer.h>
#include <GoddamnEngine/Engine/Renderer/Impl/Direct3D11/D3D11Texture2D.h>

GD_NAMESPACE_BEGIN

	/// Shader program D3D11 implementation. Does nothing.
	class HRID3D11ShaderProgram final : public HRIShaderProgram
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRID3D11ShaderProgram, HRIShaderProgram, GDINT);

	public:
		GDINL virtual ~HRID3D11ShaderProgram() { }
		GDINL explicit HRID3D11ShaderProgram() { }
	};	// class HRID3D11ShaderProgram

	/// Standard cbuffer location is quite enough.
	using HRID3D11ShaderParamLocationConstantBuffer = HRIShaderParamLocationConstantBufferNative;

	/// Resources location implementation for D3D11.
	class HRID3D11ShaderParamLocationResources final : public HRIShaderParamLocationResources
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRID3D11ShaderParamLocationResources, HRIShaderParamLocationResources, GDINT);
		typedef StackVector<ID3D11ShaderResourceView*, GD_HRI_SHADER_PARAM_MAX_LOCATION_COUNT> HRIShaderResourceViewsVector;
		typedef StackVector<ID3D11SamplerState   *, GD_HRI_SHADER_PARAM_MAX_LOCATION_COUNT> HRIShaderSampleStatesVector;
		HRIShaderResourceViewsVector ShaderResourceViews;
		HRIShaderSampleStatesVector  ShaderSampleStates;

	public:
		GDINT explicit HRID3D11ShaderParamLocationResources(HRIShaderInstance* const ShaderInstance, HRIShaderParamLocationDesc const* const LocationDesc);
		GDINL virtual ~HRID3D11ShaderParamLocationResources(                        ) { }

		GDINL HRIShaderResourceViewsVector const& GetShaderResourceViews() const { return this->ShaderResourceViews; }
		GDINL HRIShaderSampleStatesVector  const& GetShaderSampleStates () const { return this->ShaderSampleStates; }

		GDINT virtual void UploadAllParameters() override final;
	};	// class HRID3D11ShaderParamLocationResources

	/// Shader instance implementation for D3D11.
	class HRID3D11ShaderInstance : public HRIShaderInstance
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRID3D11ShaderInstance, HRIShaderInstance, GDINT);

	public:
		StackVector<ID3D11Buffer const*, GD_HRI_SHADER_PARAM_MAX_LOCATION_COUNT> ShaderConstantBuffers;

		GDINT explicit HRID3D11ShaderInstance(HRIShaderInstanceDesc const* const InstanceDesc);
		GDINL virtual ~HRID3D11ShaderInstance(              ) { }
	};	// class HRID3D11ShaderInstance

	/// Vertex shader implementation for D3D11.
	class HRID3D11VertexShader final : public HRIVertexShader
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRID3D11VertexShader, HRIVertexShader, GDINT);
		D3D11RefPtr<ID3D11VertexShader> VertexShader;
		D3D11RefPtr<ID3D11InputLayout > InputLayout;
		D3D11RefPtr<ID3DBlob    > CompiledData;

	private:
		GDINT void CreateShader  (HRIShaderCtorInfo const& CtorInfo);
		GDINT void CreateInputLayout(HRIShaderCtorInfo const& CtorInfo);

	public:
		GDINT explicit HRID3D11VertexShader(HRIShaderCtorInfo const& CtorInfo);
		GDINL virtual ~HRID3D11VertexShader(         ) { }
		GDINT virtual void BindShader  (HRIShaderInstance const* const ShaderInstance) const final;
		GDINT virtual void UnbindShader(            ) const final;
	};	// class HRID3D11VertexShader

	/// Hull shader implementation for D3D11.
	class HRID3D11HullShader final : public HRIHullShader
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRID3D11HullShader, HRIHullShader, GDINT);
		D3D11RefPtr<ID3D11HullShader> HullShader;
		D3D11RefPtr<ID3DBlob  > CompiledData;

	public:
		GDINT explicit HRID3D11HullShader(HRIShaderCtorInfo const& CtorInfo);
		GDINL virtual ~HRID3D11HullShader(         ) { }
		GDINT virtual void BindShader  (HRIShaderInstance const* const ShaderInstance) const final;
		GDINT virtual void UnbindShader(            ) const final;
	};	// class HRID3D11HullShader

	/// Domain shader implementation for D3D11.
	class HRID3D11DomainShader final : public HRIDomainShader
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRID3D11DomainShader, HRIDomainShader, GDINT);
		D3D11RefPtr<ID3D11DomainShader> DomainShader;
		D3D11RefPtr<ID3DBlob    > CompiledData;

	public:
		GDINT explicit HRID3D11DomainShader(HRIShaderCtorInfo const& CtorInfo);
		GDINL virtual ~HRID3D11DomainShader(         ) { }
		GDINT virtual void BindShader  (HRIShaderInstance const* const ShaderInstance) const final;
		GDINT virtual void UnbindShader(            ) const final;
	};	// class HRID3D11DomainShader

	/// Pixel shader implementation for D3D11.
	class HRID3D11PixelShader final : public HRIPixelShader
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRID3D11PixelShader, HRIPixelShader, GDINT);
		D3D11RefPtr<ID3D11PixelShader> PixelShader;
		D3D11RefPtr<ID3DBlob   > CompiledData;

	public:
		GDINT explicit HRID3D11PixelShader(HRIShaderCtorInfo const& CtorInfo);
		GDINL virtual ~HRID3D11PixelShader(         ) { }
		GDINT virtual void BindShader  (HRIShaderInstance const* const ShaderInstance) const final;
		GDINT virtual void UnbindShader(            ) const final;
	};	// class HRID3D11PixelShader

GD_NAMESPACE_END

#endif