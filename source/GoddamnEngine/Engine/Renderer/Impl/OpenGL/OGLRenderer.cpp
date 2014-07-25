#include <GoddamnEngine/Engine/Renderer/Impl/OpenGL/OGLRenderer.h>

#if (!defined(GD_HRI_OGL_ES))
#	if (defined(GD_PLATFORM_WINDOWS))
#		pragma comment(lib, "OpenGL32.lib")
#		pragma comment(lib, "GLU32.lib")
#		include <Windows.h>
#		include <gl/GL.h>
#		define GD_GL_GET_PROC_ADDRESS			(::wglGetProcAddress)
#		define WGL_DRAW_TO_WINDOW_ARB			0x2001
#		define WGL_ACCELERATION_ARB				0x2003
#		define WGL_SWAP_METHOD_ARB				0x2007
#		define WGL_SUPPORT_OPENGL_ARB			0x2010
#		define WGL_DOUBLE_BUFFER_ARB			0x2011
#		define WGL_PIXEL_TYPE_ARB				0x2013
#		define WGL_COLOR_BITS_ARB				0x2014
#		define WGL_DEPTH_BITS_ARB				0x2022
#		define WGL_STENCIL_BITS_ARB				0x2023
#		define WGL_FULL_ACCELERATION_ARB		0x2027
#		define WGL_SWAP_EXCHANGE_ARB			0x2028
#		define WGL_TYPE_RGBA_ARB				0x202B
#		define WGL_CONTEXT_MAJOR_VERSION_ARB	0x2091
#		define WGL_CONTEXT_MINOR_VERSION_ARB	0x2092
//	endif	//	if (defined(GD_PLATFORM_WINDOWS))
#	else	// *** Platform Select ***
#		error "'~~~Platform-Dependant-Includes~~~' error: Code for target platform is not implemented."
#	endif	// *** Platform Select ***
#endif	// if (!defined(GD_HRI_OGL_ES))
	
GD_NAMESPACE_BEGIN

	GD_TYPEINFORMATION_IMPLEMENTATION(HROGLInterface, HRInterface, GDINT);

	bool HROGLInterface::CreateContex()
	{
#define GD_DEFINE_OGL_METHOD(ReturnType, MethodName, ArgumentsDeclarations, ArgumentsPassing) \
		/**/self->Driver._##MethodName = reinterpret_cast<ReturnType (*)(ArgumentsDeclarations)>(GD_GL_GET_PROC_ADDRESS("gl"#MethodName)); \
		if (self->Driver._##MethodName == nullptr) { \
			throw HRIOGLException("Unable to map 'gl"#MethodName"' method."); \
		}
#if (!defined(GD_HRI_OGL_ES))
#	if (defined(GD_PLATFORM_WINDOWS))
		HDC const DeviceContext = HDC(LowLevelSystem::GetInstance().hDeviceContext);

		/*Loading extensions*/ {
			PIXELFORMATDESCRIPTOR PixelFormat;
			if (::SetPixelFormat(DeviceContext, 1, &PixelFormat) != TRUE) {
				throw HRIOGLException("'::SetPixelFormat' failed.");
			}
			// Creating temporary context to load OpenGL API.
			HGLRC const TemporaryRendererContext = ::wglCreateContext(DeviceContext);
			if (TemporaryRendererContext == nullptr) {
				throw HRIOGLException("Failed to create temporary OpenGL context ('::wglCreateContext' returned nullptr).");
			}
			if (::wglMakeCurrent(DeviceContext, TemporaryRendererContext) != TRUE) {
				throw HRIOGLException("Failed to create set temporary OpenGL context ('::wglMakeCurrent' failed).");
			}
			// Loading OpenGL methods using generated code and WGL manually.
#include <GoddamnEngine/Engine/Renderer/Impl/OpenGL/OGLRendererMethods.h>
			/**/self->Driver._WinChoosePixelFormatARB = reinterpret_cast<OpenGLDriver::PFNWGLCHOOSEPIXELFORMATARBPROC>(wglGetProcAddress("wglChoosePixelFormatARB"));
			if (self->Driver._WinChoosePixelFormatARB == nullptr) {
				throw HRIOGLException("Unable to map 'wglChoosePixelFormatARB' method.");
			}
			/**/self->Driver._WinCreateContextAttribsARB = reinterpret_cast<OpenGLDriver::PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
			if (self->Driver._WinCreateContextAttribsARB == nullptr) {
				throw HRIOGLException("Unable to map 'wglCreateContextAttribsARB' method.");
			}
			/**/self->Driver._WinSwapIntervalEXT = reinterpret_cast<OpenGLDriver::PFNWGLSwapIntervalEXTPROC>(wglGetProcAddress("wglSwapIntervalEXT"));
			if (self->Driver._WinSwapIntervalEXT == nullptr) {
				throw HRIOGLException("Unable to map 'wglSwapIntervalEXT' method.");
			}

			::wglMakeCurrent(nullptr, nullptr);
			::wglDeleteContext(TemporaryRendererContext);
		}

		/* Creating a real OpenGL 4.3 context: */ {
			int static const PixelFormatAttributes[] = {
				WGL_SUPPORT_OPENGL_ARB,    TRUE,
				WGL_DRAW_TO_WINDOW_ARB,    TRUE,
				WGL_ACCELERATION_ARB,      WGL_FULL_ACCELERATION_ARB,
				WGL_COLOR_BITS_ARB,        24,
				WGL_DEPTH_BITS_ARB,        24,
				WGL_STENCIL_BITS_ARB,      8,
				WGL_DOUBLE_BUFFER_ARB,     TRUE,
				WGL_SWAP_METHOD_ARB,       WGL_SWAP_EXCHANGE_ARB,
				WGL_PIXEL_TYPE_ARB,        WGL_TYPE_RGBA_ARB,
				0
			};

			int static const VersionAttributes[] = {
				WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
				WGL_CONTEXT_MINOR_VERSION_ARB, 2,
				0
			};

			/*Selecting pixel format*/ {
				int  ChosenPixelFormat = 0;
				UINT ChosenFormatCount = 0;
				if (self->Driver.WinChoosePixelFormatARB(DeviceContext, &PixelFormatAttributes[0], nullptr, 1, &ChosenPixelFormat, &ChosenFormatCount) != TRUE) {
					throw HRIOGLException("Failed to choose pixel format ('->wglChoosePixelFormatARB' failed).");
				}
			}

			if ((self->RenderingContext = self->Driver.WinCreateContextAttribsARB(DeviceContext, 0, &VersionAttributes[0])) == nullptr) {
				throw HRIOGLException(String::Format("Failed to create OpenGL %d.%d context ('->wglCreateContextAttribsARB' failed).", VersionAttributes[0], VersionAttributes[1]));
			}

			if(::wglMakeCurrent(DeviceContext, self->RenderingContext) != TRUE) {
				throw HRIOGLException("Failed to make generated context current ('::wglMakeCurrent' failed).");
			}
		}
		//	endif	// if (defined(GD_PLATFORM_WINDOWS))
#	else	// *** Platform Select ***
#		error "'HROGLInterface::CreateContex()' is not implemented for target platform (OpenGL desktop)."
#	endif	// *** Platform Select ***
#else	// if (!defined(GD_HRI_OGL_ES))
#	if 0
//	endif	// if 0
#	else	// *** Platform Select ***
#		error "'HROGLInterface::CreateContex()' is not implemented for target platform (OpenGL(ES))."
#	endif	// *** Platform Select ***
#endif	// if (!defined(GD_HRI_OGL_ES))
		
		::glEnable(GL_DEPTH_TEST);
		GD_HRI_OGL_CHECK_ERRORS("Failed to enable Depth Test");
		::glFrontFace (GL_CW);
		GD_HRI_OGL_CHECK_ERRORS("Failed to set Clock-Wise vertices mode");
		::glEnable(GL_CULL_FACE);
		GD_HRI_OGL_CHECK_ERRORS("Failed enable Face culling");
		::glCullFace(GL_BACK);
		GD_HRI_OGL_CHECK_ERRORS("Failed switch face culling to Back surface culling");
		::glClearDepth(1.0f);
		GD_HRI_OGL_CHECK_ERRORS("Failed to Clear depth to 1.0f");
		return true;
	}

	bool HROGLInterface::DestroyContext()
	{
#if (!defined(GD_HRI_OGL_ES))
#	define GD_DEFINE_OGL_METHOD(ReturnType, MethodName, ArgumentsDeclarations, ArgumentsPassing) self->Driver._##MethodName = nullptr;
#	include <GoddamnEngine/Engine/Renderer/Impl/OpenGL/OGLRendererMethods.h>
#	if (defined(GD_PLATFORM_WINDOWS))
		self->Driver._WinChoosePixelFormatARB    = nullptr;
		self->Driver._WinCreateContextAttribsARB = nullptr;
		self->Driver._WinSwapIntervalEXT         = nullptr;
		::wglMakeCurrent(nullptr, nullptr);
		::wglDeleteContext(self->RenderingContext);
		self->RenderingContext = nullptr;
//	endif	// if (defined(GD_PLATFORM_WINDOWS))
#	else	// *** Platform Select ***
#		error "'HROGLInterface::DestroyContext()' is not implemented for target platform (OpenGL desktop)."
#	endif	// *** Platform Select ***
#else	// if (!defined(GD_HRI_OGL_ES))
#	if 0
//	endif	// if 0
#	else	// *** Platform Select ***
#		error "'HROGLInterface::DestroyContext()' is not implemented for target platform (OpenGL(ES))."
#	endif	// *** Platform Select ***
#endif	// if (!defined(GD_HRI_OGL_ES))
		return true;
	}

	void HROGLInterface::ClearContext(Rectangle const& ClearingViewport, Color const& ClearColor, bool const DoClearDepth /* = true */)
	{
		::glClear(GL_COLOR_BUFFER_BIT | (DoClearDepth ? GL_DEPTH_BUFFER_BIT : 0));
		::glClearColor(ClearColor.r, ClearColor.g, ClearColor.b, ClearColor.a);
		::glViewport(static_cast<GLint>(self->ContextResolution.Width  * ClearingViewport.Left  ),
					 static_cast<GLint>(self->ContextResolution.Height * ClearingViewport.Top   ),
					 static_cast<GLint>(self->ContextResolution.Width  * ClearingViewport.Width ),
					 static_cast<GLint>(self->ContextResolution.Height * ClearingViewport.Height));
	}

	void HROGLInterface::SwapBuffers()
	{
#if (!defined(GD_HRI_OGL_ES))
#	if (defined(GD_PLATFORM_WINDOWS))
		if ((::SwapBuffers(HDC(LowLevelSystem::GetInstance().hDeviceContext))) != TRUE) {
			throw HRIOGLException("Failed to swap buffers ('::SwapBuffers' failed).");
		}
//	endif	// if (defined(GD_PLATFORM_WINDOWS))
#	else	// *** Platform Select ***
#		error "'HROGLInterface::SwapBuffers()' is not implemented for target platform (OpenGL desktop)."
#	endif	// *** Platform Select ***
#else	// if (!defined(GD_HRI_OGL_ES))
#	if 0
//	endif	// if 0
#	else	// *** Platform Select ***
#		error "'HROGLInterface::SwapBuffers()' is not implemented for target platform (OpenGL(ES))."
#	endif	// *** Platform Select ***
#endif	// if (!defined(GD_HRI_OGL_ES))
	}

GD_NAMESPACE_END

#if (!defined(GD_MONOLITHIC_ENGINE))
/// ==========================================================================================
GDEXP extern bool EnginePluginEntry(GD PluginDescription* const Description)
{
#if (defined(GD_NAMESPACE))
	using namespace GD_NAMESPACE;
#endif	// if (defined(GD_NAMESPACE))

	Description->Type = GD_PLUGIN_TYPE_RENDERER;
	Description->Description = "Default GoddamnEngine`s OpenGL Renderer";
	Description->PluginTypeInformation = HROGLInterface::GetClassTypeInformation();

	return true;
}
#endif	// if (!defined(GD_MONOLITHIC_ENGINE))
