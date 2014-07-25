/// ==========================================================================================
/// OGLRenderer.h - OpenGL(ES) Hardware renderer implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 05.06.2014 - Created by James Jhuighuy
/// ==========================================================================================

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
#	include <GL/GL.h>
#	include <GL/GLU.h>
#	include <GoddamnEngine/Engine/Renderer/Impl/OpenGL/OGLRendererDefines.h>
#endif	// if (defined(GD_HRI_OGL_ES))

#define GD_HRI_OGL_CHECK_ERRORS(Message) \
	do { \
		GLenum const Result = ::glGetError(); \
		if (Result != GL_NO_ERROR) { \
			Str const ResultLog = reinterpret_cast<Str>(::gluErrorString(Result)); \
			if (ResultLog != nullptr) { \
				throw HRIOGLException(String(Message" with following log: ") + ResultLog); \
			} else { \
				throw HRIOGLException(Message); \
			} \
		} \
	} while(false)

GD_NAMESPACE_BEGIN

	/// Indicates about runtime error caused by renderer interface
	class HRIOGLException : public HRIException
	{
	public /*Public API*/:
		GDINL explicit HRIOGLException(String const& Message) : HRIException(Message.CStr()) { }
		GDINL virtual ~HRIOGLException() { }
	};	// class HRIOGLException

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
			friend class HROGLInterface;// bool HROGLInterface::CreateContex();

		public:
			GDINL  OpenGLDriver() { }
			GDINL ~OpenGLDriver() { }

		public /*Class API*/:
#if ((!defined(GD_HRI_OGL_ES)) && (defined(GD_PLATFORM_WINDOWS)))
		private /*WGL methods dynamic linkage*/:
			typedef BOOL (WINAPI * PFNWGLCHOOSEPIXELFORMATARBPROC   ) (HDC const hDC, int const* const piAttribIList, const FLOAT* const pfAttribFList, UINT const nMaxFormats, int* const piFormats, UINT* const nNumFormats);
			typedef HGLRC(WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC const hDC, HGLRC const hShareContext, int const* attribList);
			typedef BOOL (WINAPI * PFNWGLSwapIntervalEXTPROC        ) (int const interval);
			PFNWGLCHOOSEPIXELFORMATARBPROC    _WinChoosePixelFormatARB    = nullptr;
			PFNWGLCREATECONTEXTATTRIBSARBPROC _WinCreateContextAttribsARB = nullptr;
			PFNWGLSwapIntervalEXTPROC         _WinSwapIntervalEXT         = nullptr;
		public /*WGL API*/:
			GDINL BOOL WinChoosePixelFormatARB(HDC const hDC, int const* const piAttribIList, const FLOAT* const pfAttribFList, UINT const nMaxFormats, int* const piFormats, UINT* const nNumFormats) const
			{
				return self->_WinChoosePixelFormatARB(hDC, piAttribIList, pfAttribFList, nMaxFormats, piFormats, nNumFormats);
			}
			GDINL HGLRC WinCreateContextAttribsARB(HDC const hDC, HGLRC const hShareContext, int const* attribList) const
			{
				return self->_WinCreateContextAttribsARB(hDC, hShareContext, attribList);
			}
			GDINL BOOL WinSwapIntervalEXT(int const interval)
			{
				return self->_WinSwapIntervalEXT(interval);
			}
#endif	// if ((!defined(GD_HRI_OGL_ES)) && (defined(GD_PLATFORM_WINDOWS)))
#include <GoddamnEngine/Engine/Renderer/Impl/OpenGL/OGLRendererMethods.h>
		};	// struct DriverMethodsType
		
		OpenGLDriver Driver;
#if ((!defined(GD_HRI_OGL_ES)) && (defined(GD_PLATFORM_WINDOWS)))
		HGLRC RenderingContext;
#endif	// if ((!defined(GD_HRI_OGL_ES)) && (defined(GD_PLATFORM_WINDOWS)))

#define GD_HRI_API  GDINT virtual
#define GD_HRI_SPEC       override final
#include <GoddamnEngine/Engine/Renderer/RendererMethods.h>
	};	// class HROGLInterface

GD_NAMESPACE_END

#endif
