#include <GoddamnEngine/Engine/Renderer/Impl/OpenGL/OGLRenderer.h>

#if (!defined(GD_HRI_OGL_ES))
#	if (defined(GD_PLATFORM_WINDOWS))
#		pragma comment(lib, "OpenGL32.lib")
#		pragma comment(lib, "GLU32.lib")
#		include <Windows.h>
#		include <gl/GL.h>
#		define GD_GL_GET_PROC_ADDRESS			(::wglGetProcAddress)
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
		HWND const Window = HWND(LowLevelSystem::GetInstance().hWindow);
		HDC  const DeviceContext = GetDC(Window);

		PIXELFORMATDESCRIPTOR PixelFormatDescriptor;
		ZeroMemory(&PixelFormatDescriptor, sizeof(PIXELFORMATDESCRIPTOR));
		PixelFormatDescriptor.nSize      = sizeof(PIXELFORMATDESCRIPTOR);
		PixelFormatDescriptor.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
		PixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
		PixelFormatDescriptor.cColorBits = 32;
		PixelFormatDescriptor.cDepthBits = 32;
		PixelFormatDescriptor.iLayerType = PFD_MAIN_PLANE;

		int const PixelFormat = ::ChoosePixelFormat(DeviceContext, &PixelFormatDescriptor);
		if (PixelFormat == 0) {
			throw HRIOGLException("Failed to choose pixel format ('::ChoosePixelFormat' returned 0)");
		}
		if (::SetPixelFormat(DeviceContext, PixelFormat, &PixelFormatDescriptor) != TRUE) {
			throw HRIOGLException("Failed to set pixel format ('::SetPixelFormat' failed)");
		}

		// Creating temporary OpenGL 2.1 context to load extensions.
		HGLRC const TemporaryContext = ::wglCreateContext(DeviceContext);
		if (TemporaryContext == nullptr) {
			throw HRIOGLException("Failed to create temporary OpenGL context ('::wglCreateContext' returned nullptr)");
		}
		if (wglMakeCurrent(DeviceContext, TemporaryContext) != TRUE) {
			throw HRIOGLException("Failed to setup temorary OpenGL context ('::wglMakeCurrent' failed)");
		}

		// Loading OpenGL methods using generated code and WGL manually.
#if (!defined(__INTELLISENSE__))	// Intelli Sence goes crazy while parsing code below.
#	include <GoddamnEngine/Engine/Renderer/Impl/OpenGL/OGLRendererMethods.h>
#endif	// if (!defined(__INTELLISENSE__))

		// Loading API that allowes creation of avanced context.
		typedef HGLRC(APIENTRY* PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC const hDC, HGLRC const hGLRC, int const* const Attributes);
		auto const WinCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(::wglGetProcAddress("wglCreateContextAttribsARB"));
		if (WinCreateContextAttribsARB == nullptr) {
			throw HRIOGLException("Unable to map 'wglCreateContextAttribsARB' method.");
		}

		::wglMakeCurrent(nullptr, nullptr);
		::wglDeleteContext(TemporaryContext);

		int static const VersionAttributes[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
			WGL_CONTEXT_MINOR_VERSION_ARB, 2,
			0
		};

		if ((self->RenderingContext = WinCreateContextAttribsARB(DeviceContext, nullptr, VersionAttributes)) == nullptr) {
			throw HRIOGLException(String::Format("Failed to create OpenGL %d.%d context ('->wglCreateContextAttribsARB' failed).", VersionAttributes[0], VersionAttributes[1]));
		}
		if (::wglMakeCurrent(DeviceContext, self->RenderingContext) != TRUE) {
			throw HRIOGLException("Failed to make generated context current ('::wglMakeCurrent' failed).");
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
#	if (!defined(__INTELLISENSE__))	// Intelli Sence goes crazy while parsing code below.
#		include <GoddamnEngine/Engine/Renderer/Impl/OpenGL/OGLRendererMethods.h>
#	endif	// if (!defined(__INTELLISENSE__))
#	if (defined(GD_PLATFORM_WINDOWS))
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