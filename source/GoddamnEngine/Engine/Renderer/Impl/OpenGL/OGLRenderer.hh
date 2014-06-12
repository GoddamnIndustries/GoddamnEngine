//////////////////////////////////////////////////////////////////////////
/// OGLRenderer.h - OpenGL(ES) Hardware renderer implementation.
/// Copyright (C) $(GD_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 05.06.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_ENGINE_RENDERER_IMPL_OGL_RENDERER
#define GD_ENGINE_RENDERER_IMPL_OGL_RENDERER

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Renderer/Renderer.h>

/// Undefine this to focre use OpenGL ES on current platform.
// #define GD_HRI_OGL_FORCE_ES

#if (defined(GD_PLATFORM_MOBILE) || defined(GD_HRI_OGL_FORCE_ES))
#	define GD_HRI_OGL_ES
#endif	// if (defined(IE_PLATFORM_MOBILE) || defined(IE_HRI_OGL_FORCE_ES))

#if (defined(GD_HRI_OGL_ES))
#	include <EGL/egl.h>
#	include <GLES3/gl3.h>
#else	// if (defined(GD_HRI_OGL_ES))
#	include <GL/gl.h>
#	include <GoddamnEngine/Engine/Renderer/Impl/OpenGL/OGLRendererDefines.h>
#endif	// if (defined(GD_HRI_OGL_ES))

#define GD_HRI_OGLASSERT(Message) \
	do { \
		GLenum Error = GL_GetError(); \
		GD_ASSERT(Error == GL_NO_ERROR, "OpenGL error: %s" Message); \
	} while(false)

GD_NAMESPACE_BEGIN

	class HROGLInterface final : public HRInterface
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HROGLInterface, HRInterface, GDINT);
		GD_SINGLETON_OVERRIDE(HROGLInterface, HRInterface);

	public:
		GDINL explicit HROGLInterface() { }
		GDINL virtual ~HROGLInterface() { }

		struct OpenGLDriver final
		{
		private:
			GD_CLASS_UNASSIGNABLE(OpenGLDriver);
			GD_CLASS_UNCOPIABLE  (OpenGLDriver);
			friend bool HROGLInterface::CreateContex();

		public:
			GDINL  OpenGLDriver() { }
			GDINL ~OpenGLDriver() { }

		public /*Class API*/:
#include <GoddamnEngine/Engine/Renderer/Impl/OpenGL/OGLRendererMethods.h>
		};	// struct DriverMethodsType
		
		OpenGLDriver Driver;

#define GD_HRI_API  GDINT virtual
#define GD_HRI_SPEC       override final
#include <GoddamnEngine/Engine/Renderer/RendererMethods.h>
	};	// class HROGLInterface

GD_NAMESPACE_END

#endif
