#include <GoddamnEngine/Engine/Renderer/Impl/OpenGL/OGLRenderer.h>

#if (!defined(GD_HRI_OGL_ES))
#	if (defined(GD_PLATFORM_WINDOWS))
#		define  GD_FIND_OGL_METHOD wglGetProcAddress
#		include <Windows.h>
#		include <gl/GL.h>
#	else
#		error 'Unimplemented platform'
#	endif
#endif

GD_NAMESPACE_BEGIN

	GD_TYPEINFORMATION_IMPLEMENTATION(HROGLInterface, HRInterface, GDINT);

	bool HROGLInterface::CreateContex()
	{
		// Loading OpenGL methods.
#if (!defined(GD_HRI_OGL_ES))
#	  define  GD_DEFINE_OGL_METHOD(ReturnType, MethodName, ArgumentsDeclarations, ArgumentsPassing) \
		self->Driver->_##MethodName = static_cast<ReturnType (*)(ArgumentsDeclarations)>(GD_FIND_OGL_METHOD(#MethodName)); \
		GD_ASSERT(self->Driver->_##MethodName != nullptr, "Unable to map 'gl"#MethodName"' method.");
#	  include <GoddamnEngine/Engine/Renderer/RendererMethods.h>
#endif	// if (!defined(GD_HRI_OGL_ES))

		return true;
	}

GD_NAMESPACE_END
