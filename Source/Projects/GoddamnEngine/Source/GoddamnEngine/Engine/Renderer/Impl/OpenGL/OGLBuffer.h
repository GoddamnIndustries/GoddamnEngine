/// ==========================================================================================
/// OGLBuffer.h - HRI Buffer OpenGL implementation interfaces.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 05.06.2014 - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_ENGINE_RENDERER_IMPL_OGL_BUFFER
#define GD_ENGINE_RENDERER_IMPL_OGL_BUFFER

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Renderer/Impl/OpenGL/OGLRenderer.h>

GD_NAMESPACE_BEGIN

	class HRIOGLVertexBuffer final : public HRIVertexBuffer
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRIOGLVertexBuffer, HRIVertexBuffer, GDINT);
		GLuint VertexBufferObject = 0;

	public:
		GDINT explicit HRIOGLVertexBuffer(Float32 const* const Data, size_t const Size);
		GDINT virtual ~HRIOGLVertexBuffer();

		GDINL virtual  handle GetNativePointer() const final { return const_cast<GLuint*>(&this->VertexBufferObject); }
		GDINL    GLuint GetBufferID  () const    { return       this->VertexBufferObject ; }
		GDINT void BindBuffer() const;
		GDINT void UnbindBuffer() const;
	};	// class HRIOGLVertexBuffer

	class HRIOGLIndexBuffer final : public HRIIndexBuffer
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRIOGLIndexBuffer, HRIIndexBuffer, GDINT);
		GLuint IndexBufferObject = 0;

	public:
		GDINT explicit HRIOGLIndexBuffer(chandle const Data, size_t const Size, size_t const Stride);
		GDINL virtual ~HRIOGLIndexBuffer();
		
		GDINL virtual  handle GetNativePointer() const final { return const_cast<GLuint*>(&this->IndexBufferObject); }
		GDINL    GLuint GetBufferID  () const    { return       this->IndexBufferObject ; }
		GDINT void BindBuffer() const;
		GDINT void UnbindBuffer() const;
	};	// class HRIOGLIndexBuffer

	class HRIOGLConstantBuffer final : public HRIConstantBuffer
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRIOGLConstantBuffer, HRIConstantBuffer, GDINT);
		GLuint UniformBufferObject = 0;

	public:
		GDINT explicit HRIOGLConstantBuffer(size_t const Size);
		GDINL virtual ~HRIOGLConstantBuffer();
		
		GDINT virtual void CopyDataTo  ( handle const Data) const final;
		GDINT virtual void CopyDataFrom(chandle const Data)    final;
		
		GDINL virtual  handle GetNativePointer() const final { return const_cast<GLuint*>(&this->UniformBufferObject); }
		GDINL    GLuint GetBufferID  () const    { return       this->UniformBufferObject ; }
		GDINT void BindBuffer() const;
		GDINT void UnbindBuffer() const;
	};	// class HRIOGLConstantBuffer

GD_NAMESPACE_END

#endif