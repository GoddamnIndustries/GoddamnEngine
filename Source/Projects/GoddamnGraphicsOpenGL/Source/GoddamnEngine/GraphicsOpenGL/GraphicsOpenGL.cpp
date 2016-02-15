// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#define GD_IGRAPHICS_OPENGL_IMPL 1

/*!
 * @file GoddamnEngine/GraphicsOpenGL/GraphicsOpenGL.cpp
 * File contains Implementation for OpenGL 4.3 Implementation of the graphics interface.
 */
#include <GoddamnEngine/GraphicsOpenGL/GraphicsOpenGL.h>
#include <GoddamnEngine/Core/System/OutputDevice/OutputDevice.h>
#define GD_DLOG_CAT "GFX device (OpenGL)"

GD_NAMESPACE_BEGIN

	GD_IMPLEMENT_CLASS_INFORMATION(IGraphicsOpenGL);
	GDEXP extern IGraphics* CreateIGraphicsInstance()
	{
		return GD_NEW(IGraphicsOpenGL);
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Runtime..
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Function would be called on the global initialization step.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsOpenGL::OnRuntimeInitialize()
	{
	//	_CheckNotInitialized();
		ConsoleDevice->Log(GD_DLOG_CAT ": going to initialize graphics devices...");

		IResult const _BaseResult = IGraphicsOpenGLPlatform::OnRuntimeInitialize();
		if (IFailed(_BaseResult))
			return _BaseResult;

		// Loading OpenGL core profile methods.
		glewExperimental = GL_TRUE;
		glewInit();

#if GD_DEBUG
		if (GLEW_ARB_debug_output)
		{
			// We have some good debugging extension running, so we do not need to check for OpenGL
			// errors after every single OpenGL API function call.
			// http://www.opengl.org/registry/specs/ARB/debug_output.txt
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
			glDebugMessageCallbackARB([](GLenum const Source, GLenum const Type, GLuint const ID, GLenum const Severity
				, GLsizei const Length, GLchar const* const Message, CHandle const UserParam)
			{
				CStr glDebugSeverity;
				switch (Severity)
				{
				default: glDebugSeverity = "Unknown severity"; break;
				case GL_DEBUG_SEVERITY_HIGH_ARB:   glDebugSeverity = "High priority"; break;
				case GL_DEBUG_SEVERITY_MEDIUM_ARB: glDebugSeverity = "Medium priority"; break;
				case GL_DEBUG_SEVERITY_LOW_ARB:    glDebugSeverity = "Low priority"; break;
				}

				CStr glDebugErrorType;
				switch (Type)
				{
				default: glDebugErrorType = "something"; break;
				case GL_DEBUG_TYPE_OTHER_ARB: glDebugErrorType = "other issue"; break;
				case GL_DEBUG_TYPE_ERROR_ARB: glDebugErrorType = "error"; break;
				case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:	glDebugErrorType = "deprecated behavior issue"; break;
				case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:  glDebugErrorType = "undefined behavior issue"; break;
				case GL_DEBUG_TYPE_PORTABILITY_ARB:	glDebugErrorType = "portability issue"; break;
				case GL_DEBUG_TYPE_PERFORMANCE_ARB:	glDebugErrorType = "performance issue"; break;
				}

				CStr glDebugErrorSource;
				switch (Source)
				{
				default: glDebugErrorSource = "unknown location"; break;
				case GL_DEBUG_SOURCE_API_ARB: glDebugErrorSource = "API call"; break;
				case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:   glDebugErrorSource = "window system API all"; break;
				case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB: glDebugErrorSource = "shader compiler"; break;
				case GL_DEBUG_SOURCE_THIRD_PARTY_ARB: glDebugErrorSource = "third party API"; break;
				case GL_DEBUG_SOURCE_APPLICATION_ARB: glDebugErrorSource = "application"; break;
				case GL_DEBUG_SOURCE_OTHER_ARB: glDebugErrorSource = "other"; break;
				}
				
				GD_NOT_USED_L(UserParam, Length);
				switch (Severity)
				{
				default: 
				case GL_DEBUG_SEVERITY_LOW_ARB:	// Possibly, this is not an error: e.g. a log from NVidia driver.
					ConsoleDevice->LogFormat(GD_DLOG_CAT ": ... debug callback:\n\t%s %s #%x in %s:\n\t%s"
						, glDebugSeverity, glDebugErrorType, ID, glDebugErrorSource, Message
						);
					break;

				case GL_DEBUG_SEVERITY_HIGH_ARB:
				case GL_DEBUG_SEVERITY_MEDIUM_ARB:	// This is some kind of warning or error.
					ConsoleDevice->LogErrorFormat(GD_DLOG_CAT ": ... debug callback:\n\t%s %s #%x in %s:\n\t%s"
						, glDebugSeverity, glDebugErrorType, ID, glDebugErrorSource, Message
						);
					GD_ASSERT_FALSE("Some issue inside OpenGL code.");
				//	break;
				}
			}, nullptr);
		}
		else
		{
			ConsoleDevice->LogError(GD_DLOG_CAT ": ... no 'ARB_debug_output' extension was found. No log or error checking for OpenGL code "
				"would be provided.");
		}
#endif	// if GD_DEBUG

		// Enabling the back-face cooling.
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		// We are using m_Left-handed math and clockwise triangles.
		glFrontFace(GL_CW);

		// Initially setting up the view-port and depth.
		glEnable(GL_DEPTH_TEST);
		glClearDepth(1.0f);
		glClearColor(0.3f, 0.275f, 1.0f, 1.0f);
		glViewport(0, 0, GfxResolutionSelected->Width, GfxResolutionSelected->Height);

		ConsoleDevice->Log(GD_DLOG_CAT ": ... initialized.");
		return IResult::Ok;
	}

GD_NAMESPACE_END
	