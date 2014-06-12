//////////////////////////////////////////////////////////////////////////
/// OGLBuffer.cpp - HRI Buffer OpenGL implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 05.06.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#include <GoddamnEngine/Engine/Renderer/Impl/OpenGL/OGLBuffer.h>

GD_NAMESPACE_BEGIN

	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIOGLVertexBuffer, HRIVertexBuffer, GDINT, nullptr);
	HRIOGLVertexBuffer::HRIOGLVertexBuffer(Float32 const* const Data, size_t const Size) 
		: HRIVertexBuffer(Data, Size)
	{	
		auto const& GL = HROGLInterface::GetInstance().Driver;
		GL.GenBuffers(1, &self->VertexBufferObject);
		GL.BindBuffer(GL_ARRAY_BUFFER, self->VertexBufferObject);
		GL.BufferData(GL_ARRAY_BUFFER, Size, Data, GL_STATIC_DRAW);
		GL.BindBuffer(GL_ARRAY_BUFFER, 0);
	}

	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIOGLIndexBuffer, HRIIndexBuffer, GDINT, nullptr);
	HRIOGLIndexBuffer::HRIOGLIndexBuffer(chandle const Data, size_t const Size, size_t const Stride)
		: HRIIndexBuffer(Data, Size, Stride)
	{
		auto const& GL = HROGLInterface::GetInstance().Driver;
		GL.GenBuffers(1, &self->IndexBufferObject);
		GL.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->IndexBufferObject);
		GL.BufferData(GL_ELEMENT_ARRAY_BUFFER, Size, Data, GL_STATIC_DRAW);
		GL.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	
	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIOGLConstantBuffer, HRIConstantBuffer, GDINT, nullptr);
	HRIOGLConstantBuffer::HRIOGLConstantBuffer(size_t const Size)
		: HRIConstantBuffer(Size)
	{
		auto const& GL = HROGLInterface::GetInstance().Driver;
		GL.GenBuffers(1, &self->UniformBufferObject);
	}

	void HRIOGLConstantBuffer::CopyDataTo(handle const Data) const
	{
		auto const& GL = HROGLInterface::GetInstance().Driver;
		GL.BindBuffer(GL_UNIFORM_BUFFER, self->UniformBufferObject);
		GL.BufferData(GL_UNIFORM_BUFFER, self->Size, Data, GL_DYNAMIC_DRAW);
		GL.BindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void HRIOGLConstantBuffer::CopyDataFrom(chandle const Data)
	{
		GD_UNUSED(Data);
		GD_NOT_IMPLEMENTED();
	}

	HRIVertexBuffer  * HROGLInterface::CreateVertexBuffer  (Float32 const* const Data, size_t const Size                     ) { return new HRIOGLVertexBuffer  (Data, Size        ); }
	HRIIndexBuffer   * HROGLInterface::CreateIndexBuffer   (chandle const        Data, size_t const Size, size_t const Stride) { return new HRIOGLIndexBuffer   (Data, Size, Stride); } 
	HRIConstantBuffer* HROGLInterface::CreateConstantBuffer(                           size_t const Size                     ) { return new HRIOGLConstantBuffer(      Size        ); }

GD_NAMESPACE_END
