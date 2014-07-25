/// ==========================================================================================
/// D3D11Buffer.cpp - HRI Buffer Direct3D 11 implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 16.05.2014 - Created by James Jhuighuy
/// ==========================================================================================

#include <GoddamnEngine/Engine/Renderer/Impl/Direct3D11/D3D11Buffer.h>

GD_NAMESPACE_BEGIN

	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRID3D11VertexBuffer, HRIVertexBuffer, GDINT, nullptr);
	HRID3D11VertexBuffer::HRID3D11VertexBuffer(Float32 const* const Data, size_t const Size) 
		: HRIVertexBuffer(Data, Size)
	{	
		D3D11_BUFFER_DESC VertexBufferDescription;
		ZeroMemory(&VertexBufferDescription, sizeof(VertexBufferDescription));
		VertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		VertexBufferDescription.Usage     = D3D11_USAGE_IMMUTABLE;
		VertexBufferDescription.ByteWidth = static_cast<UINT>(Size * sizeof(Data[0]));
		VertexBufferDescription.StructureByteStride = 0;
		VertexBufferDescription.CPUAccessFlags = 0;
		VertexBufferDescription.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA VertexBufferData;
		ZeroMemory(&VertexBufferData, sizeof(VertexBufferData));
		VertexBufferData.pSysMem = Data;
		VertexBufferData.SysMemPitch = 0;
		VertexBufferData.SysMemSlicePitch = 0;

		HRESULT Result = E_FAIL;
		if (FAILED(Result = HRD3D11Interface::GetInstance().Device->CreateBuffer(&VertexBufferDescription, &VertexBufferData, &self->Buffer))) {
			throw HRID3D11Exception("Vertex buffer creation failed.");
		}
	}

	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRID3D11IndexBuffer, HRIIndexBuffer, GDINT, nullptr);
	HRID3D11IndexBuffer::HRID3D11IndexBuffer(chandle const Data, size_t const Size, size_t const Stride)
		: HRIIndexBuffer(Data, Size, Stride)
	{
		switch (self->Stride)
		{	// Setting up index format of our buffer.
		case 1: self->IndexFormat = DXGI_FORMAT_R8_UINT;   break;
		case 2: self->IndexFormat = DXGI_FORMAT_R16_UINT;   break;
		case 4: self->IndexFormat = DXGI_FORMAT_R32_UINT;   break;
		default: GD_ASSERT_FALSE("Unhandled stride size."); break;
		}

		D3D11_BUFFER_DESC IndexBufferDescription;
		ZeroMemory(&IndexBufferDescription, sizeof(IndexBufferDescription));
		IndexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
		IndexBufferDescription.Usage     = D3D11_USAGE_IMMUTABLE;
		IndexBufferDescription.ByteWidth = static_cast<UINT>(Size * Stride);
		IndexBufferDescription.StructureByteStride = 0;
		IndexBufferDescription.CPUAccessFlags = 0;
		IndexBufferDescription.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA IndexBufferData;
		ZeroMemory(&IndexBufferData, sizeof(IndexBufferData));
		IndexBufferData.pSysMem = Data;
		IndexBufferData.SysMemPitch = 0;
		IndexBufferData.SysMemSlicePitch = 0;

		HRESULT Result = E_FAIL;
		if (FAILED(Result = HRD3D11Interface::GetInstance().Device->CreateBuffer(&IndexBufferDescription, &IndexBufferData, &self->Buffer))) {
			throw HRID3D11Exception("Index buffer creation failed.");
		}
	}
	
	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRID3D11ConstantBuffer, HRIConstantBuffer, GDINT, nullptr);
	HRID3D11ConstantBuffer::HRID3D11ConstantBuffer(size_t const Size)
		: HRIConstantBuffer(Size - (Size % 16) + (((Size % 16) != 0) ? 16 : 0))	// Constant buffer's memory is aligned by 16 bytes.
	{
		D3D11_BUFFER_DESC ConstantBufferDescription;
		ZeroMemory(&ConstantBufferDescription, sizeof(ConstantBufferDescription));
		ConstantBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		ConstantBufferDescription.Usage     = D3D11_USAGE_DYNAMIC;
		ConstantBufferDescription.ByteWidth = static_cast<UINT>(self->GetSize());
		ConstantBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;// | D3D11_CPU_ACCESS_READ;
		ConstantBufferDescription.StructureByteStride = 0;
		ConstantBufferDescription.MiscFlags = 0;

		HRESULT Result = E_FAIL;
		if (FAILED(Result = HRD3D11Interface::GetInstance().Device->CreateBuffer(&ConstantBufferDescription, nullptr, &self->Buffer))) {
			throw HRID3D11Exception("Constant buffer creation failed.");
		}
	}

	void HRID3D11ConstantBuffer::CopyDataTo(handle const Data) const
	{
		GD_NOT_IMPLEMENTED();
		D3D11_MAPPED_SUBRESOURCE ConstantBufferData;
		ZeroMemory(&ConstantBufferData, sizeof(ConstantBufferData));

		HRESULT Result = E_FAIL;
		if (FAILED(Result = HRD3D11Interface::GetInstance().Context->Map(self->Buffer.Get(), 0, D3D11_MAP_READ, 0, &ConstantBufferData))) {
			throw HRID3D11Exception("Constant buffer data mapping failed.");
		}

		memcpy(Data, ConstantBufferData.pData, self->GetSize());
		HRD3D11Interface::GetInstance().Context->Unmap(self->Buffer.Get(), 0);
	}

	void HRID3D11ConstantBuffer::CopyDataFrom(chandle const Data)
	{
		D3D11_MAPPED_SUBRESOURCE ConstantBufferData;
		ZeroMemory(&ConstantBufferData, sizeof(ConstantBufferData));

		HRESULT Result = E_FAIL;
		if (FAILED(Result = HRD3D11Interface::GetInstance().Context->Map(self->Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ConstantBufferData))) {
			throw HRID3D11Exception("Constant buffer data mapping failed.");
		}

		memcpy(ConstantBufferData.pData, Data, self->GetSize());
		HRD3D11Interface::GetInstance().Context->Unmap(self->Buffer.Get(), 0);
	}

	HRIVertexBuffer  * HRD3D11Interface::CreateVertexBuffer  (Float32 const* const Data, size_t const Size                     ) { return new HRID3D11VertexBuffer  (Data, Size        ); }
	HRIIndexBuffer   * HRD3D11Interface::CreateIndexBuffer   (chandle const        Data, size_t const Size, size_t const Stride) { return new HRID3D11IndexBuffer   (Data, Size, Stride); } 
	HRIConstantBuffer* HRD3D11Interface::CreateConstantBuffer(                           size_t const Size                     ) { return new HRID3D11ConstantBuffer(      Size        ); }

GD_NAMESPACE_END
