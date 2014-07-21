/// ==========================================================================================
/// D3D11Buffer.h - HRI Buffer Direct3D 11 implementation interfaces.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 16.05.2014 - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_ENGINE_RENDERER_IMPL_D3D11_BUFFER
#define GD_ENGINE_RENDERER_IMPL_D3D11_BUFFER

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Renderer/Impl/Direct3D11/D3D11Renderer.h>

GD_NAMESPACE_BEGIN

	class HRID3D11VertexBuffer final : public HRIVertexBuffer
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRID3D11VertexBuffer, HRIVertexBuffer, GDINT);
		D3D11RefPtr<ID3D11Buffer> Buffer;

	public:
		GDINT explicit HRID3D11VertexBuffer(Float32 const* const Data, size_t const Size);
		GDINL virtual ~HRID3D11VertexBuffer(                                            ) { }
		GDINL virtual  handle GetNativePointer() const final { return self->Buffer.GetPointer(); }
	};	// class HRID3D11VertexBuffer

	class HRID3D11IndexBuffer final : public HRIIndexBuffer
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRID3D11IndexBuffer, HRIIndexBuffer, GDINT);
		D3D11RefPtr<ID3D11Buffer> Buffer;
		DXGI_FORMAT IndexFormat = DXGI_FORMAT_UNKNOWN;

	public:
		GDINT explicit HRID3D11IndexBuffer(chandle const Data, size_t const Size, size_t const Stride);
		GDINL virtual ~HRID3D11IndexBuffer(                                                           ) { }
		GDINL virtual  handle GetNativePointer() const final { return self->GetBufferPointer(); }
		GDINL ID3D11Buffer*   GetBufferPointer() const       { return self->Buffer.GetPointer(); }
		GDINL DXGI_FORMAT     GetIndexFormat() const         { return self->IndexFormat; }
	};	// class HRID3D11IndexBuffer

	class HRID3D11ConstantBuffer final : public HRIConstantBuffer
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRID3D11ConstantBuffer, HRIConstantBuffer, GDINT);
		D3D11RefPtr<ID3D11Buffer> Buffer;

	public:
		GDINT explicit HRID3D11ConstantBuffer(size_t const Size);
		GDINL virtual ~HRID3D11ConstantBuffer(                 ) { }
		GDINL virtual  handle GetNativePointer() const final { return self->Buffer.GetPointer(); }
		GDINT virtual void CopyDataTo  ( handle const Data) const final;
		GDINT virtual void CopyDataFrom(chandle const Data)       final;
	};	// class HRID3D11ConstantBuffer

GD_NAMESPACE_END

#endif