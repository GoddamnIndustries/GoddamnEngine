/// ==========================================================================================
/// OGLLinkagePoint.h: Linkage point OpenGL implementation interface.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// 
/// History:
///		* 21.05.2014  - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_ENGINE_RENDERER_IMPL_OGL_LINKAGE_POINT
#define GD_ENGINE_RENDERER_IMPL_OGL_LINKAGE_POINT

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Renderer/Impl/OpenGL/OGLIndexedShape.h>
#include <GoddamnEngine/Engine/Renderer/Impl/OpenGL/OGLRenderer.h>
#include <GoddamnEngine/Engine/Renderer/Impl/OpenGL/OGLShader.h>

GD_NAMESPACE_BEGIN
	
	/// Provides dynamic linkage of shader program and shape.
	class HRIOGLLinkagePoint final : public HRILinkagePoint
	{
	private:
		GD_CLASSINFO_DEFINITION(HRIOGLLinkagePoint, HRILinkagePoint, GDINT);
		RefPtr<HRIOGLShaderProgram const> ShaderProgram;
		RefPtr<HRIOGLIndexedShape  const> IndexedShape;
		mutable bool  IsRelinkingRequired = true;
		mutable struct LinkageCache final {
			GLuint  VertexArrayObject =  0;
			GLsizei VerticesCount	  = -1;
			GLenum  IndexType   = GL_FALSE;
		} LinkingCache;
	public:
		GDINL explicit HRIOGLLinkagePoint() { }
		GDINL virtual ~HRIOGLLinkagePoint() { }
		
		GDINL virtual HRIShaderProgram const* GetShaderProgram() const final { return this->ShaderProgram.GetPointer(); }
		GDINL virtual HRIIndexedShape  const* GetIndexedShape () const final { return this->IndexedShape.GetPointer(); }
		GDINL virtual void SetShaderProgram(HRIShaderProgram const* const ShaderProgram) final
		{
			this->ShaderProgram = object_cast<HRIOGLShaderProgram const*>(ShaderProgram);
			this->IsRelinkingRequired = true;
		}

		GDINL virtual void SetIndexedShape(HRIIndexedShape  const* const IndexedShape) final
		{
			this->IndexedShape = object_cast<HRIOGLIndexedShape const*>(IndexedShape);
			this->IsRelinkingRequired = true;
		}

		GDINT virtual void RenderSelf() const override final;
	};	// class HRIOGLLinkagePoint


GD_NAMESPACE_END

#endif
