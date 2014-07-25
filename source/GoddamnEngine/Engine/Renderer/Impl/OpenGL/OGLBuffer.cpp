/// ==========================================================================================
/// OGLBuffer.cpp - HRI Buffer OpenGL implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 05.06.2014 - Created by James Jhuighuy
/// ==========================================================================================

#include <GoddamnEngine/Engine/Renderer/Impl/OpenGL/OGLBuffer.h>

GD_NAMESPACE_BEGIN

	/// ==========================================================================================
	/// HRIOGLVertexBuffer class.
	/// ==========================================================================================

	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIOGLVertexBuffer, HRIVertexBuffer, GDINT, nullptr);
	HRIOGLVertexBuffer::HRIOGLVertexBuffer(Float32 const* const Data, size_t const Size) 
		: HRIVertexBuffer(Data, Size)
	{	
		auto const& GL = HROGLInterface::GetInstance().Driver;
		GLenum Result = GL_NO_ERROR;
		GL.GenBuffers(1, &self->VertexBufferObject);
		GD_HRI_OGL_CHECK_ERRORS("Failed to create Vertex buffer");
		self->BindBuffer();
		GL.BufferData(GL_ARRAY_BUFFER, Size * sizeof(Float32), Data, GL_STATIC_DRAW);
		GD_HRI_OGL_CHECK_ERRORS("Failed to pass Vertex buffer data");
		self->UnbindBuffer();
	}

	HRIOGLVertexBuffer::~HRIOGLVertexBuffer()
	{
		auto const& GL = HROGLInterface::GetInstance().Driver;
		GL.DeleteBuffers(1, &self->VertexBufferObject);
	}

	void HRIOGLVertexBuffer::BindBuffer() const
	{
		auto const& GL = HROGLInterface::GetInstance().Driver;
		GL.BindBuffer(GL_ARRAY_BUFFER, self->VertexBufferObject);
		GD_HRI_OGL_CHECK_ERRORS("Failed to bind Vertex buffer");
	}

	void HRIOGLVertexBuffer::UnbindBuffer() const
	{
		auto const& GL = HROGLInterface::GetInstance().Driver;
		GL.BindBuffer(GL_ARRAY_BUFFER, 0);
	}

	/// ==========================================================================================
	/// HRIOGLIndexBuffer class.
	/// ==========================================================================================

	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIOGLIndexBuffer, HRIIndexBuffer, GDINT, nullptr);
	HRIOGLIndexBuffer::HRIOGLIndexBuffer(chandle const Data, size_t const Size, size_t const Stride)
		: HRIIndexBuffer(Data, Size, Stride)
	{
		auto const& GL = HROGLInterface::GetInstance().Driver;
		GL.GenBuffers(1, &self->IndexBufferObject);
		GD_HRI_OGL_CHECK_ERRORS("Failed to create Index buffer");
		self->BindBuffer();
		GL.BufferData(GL_ELEMENT_ARRAY_BUFFER, Size * Stride, Data, GL_STATIC_DRAW);
		self->UnbindBuffer();
	}
	
	HRIOGLIndexBuffer::~HRIOGLIndexBuffer()
	{
		auto const& GL = HROGLInterface::GetInstance().Driver;
		GL.DeleteBuffers(1, &self->IndexBufferObject);
	}

	void HRIOGLIndexBuffer::BindBuffer() const
	{
		auto const& GL = HROGLInterface::GetInstance().Driver;
		GL.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->IndexBufferObject);
		GD_HRI_OGL_CHECK_ERRORS("Failed to bind Index buffer");
	}

	void HRIOGLIndexBuffer::UnbindBuffer() const
	{
		auto const& GL = HROGLInterface::GetInstance().Driver;
		GL.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	/// ==========================================================================================
	/// HRIOGLConstantBuffer class.
	/// ==========================================================================================

	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIOGLConstantBuffer, HRIConstantBuffer, GDINT, nullptr);
	HRIOGLConstantBuffer::HRIOGLConstantBuffer(size_t const Size)
		: HRIConstantBuffer(Size)
	{
		auto const& GL = HROGLInterface::GetInstance().Driver;
		GL.GenBuffers(1, &self->UniformBufferObject);
		GD_HRI_OGL_CHECK_ERRORS("Failed to create Uniform buffer");
	}

	HRIOGLConstantBuffer::~HRIOGLConstantBuffer()
	{
		auto const& GL = HROGLInterface::GetInstance().Driver;
		GL.DeleteBuffers(1, &self->UniformBufferObject);
	}

	void HRIOGLConstantBuffer::BindBuffer() const
	{
		auto const& GL = HROGLInterface::GetInstance().Driver;
		GL.BindBuffer(GL_UNIFORM_BUFFER, self->UniformBufferObject);
		GD_HRI_OGL_CHECK_ERRORS("Failed to bind Uniform buffer");
	}

	void HRIOGLConstantBuffer::UnbindBuffer() const
	{
		auto const& GL = HROGLInterface::GetInstance().Driver;
		GL.BindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void HRIOGLConstantBuffer::CopyDataFrom(chandle const Data)
	{
		auto const& GL = HROGLInterface::GetInstance().Driver;
		self->BindBuffer();
		GL.BufferData(GL_UNIFORM_BUFFER, self->Size, Data, GL_DYNAMIC_DRAW);
		GD_HRI_OGL_CHECK_ERRORS("Failed to pass Uniform buffer data");
		self->UnbindBuffer();
	}

	void HRIOGLConstantBuffer::CopyDataTo(handle const Data) const
	{
		GD_UNUSED(Data);
		GD_NOT_IMPLEMENTED();
	}

	HRIVertexBuffer  * HROGLInterface::CreateVertexBuffer  (Float32 const* const Data, size_t const Size                     ) { return new HRIOGLVertexBuffer  (Data, Size        ); }
	HRIIndexBuffer   * HROGLInterface::CreateIndexBuffer   (chandle const        Data, size_t const Size, size_t const Stride) { return new HRIOGLIndexBuffer   (Data, Size, Stride); } 
	HRIConstantBuffer* HROGLInterface::CreateConstantBuffer(                           size_t const Size                     ) 
	{ 
#if (!defined(GD_HRI_OGL_ES))
		return new HRIOGLConstantBuffer(Size);
#else	// if (!defined(GD_HRI_OGL_ES))
		if (HROGLInterface::GetInstance().Version == HRIOGLVersion::ES3) {
			return new HRIOGLConstantBuffer(Size);
		} else {
			return nullptr;
		}
#endif	// if (!defined(GD_HRI_OGL_ES))
	}

GD_NAMESPACE_END
