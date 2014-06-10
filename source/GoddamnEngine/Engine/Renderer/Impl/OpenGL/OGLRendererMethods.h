//////////////////////////////////////////////////////////////////////////
/// OGLRendererMethods.h - OpenGL methods declarations.
/// Copyright (C) $(GD_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 05.06.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#if (!defined(GD_ENGINE_RENDERER_IMPL_OGL_RENDERER))
#	error 'This header should be included inside "OGLRenderer.h"'
#endif	// if (!defined(GD_ENGINE_RENDERER_IMPL_OGL_RENDERER))

#define GD_DEFINE_OGL_METHOD_GLUE_ARGS(...) \
	__VA_ARGS__

#define GD_DEFINE_OGL_METHOD_DEFINITION(ReturnType, MethodName, ArgumentsDeclarations, ArgumentsPassing) \
	private: \
		ReturnType (* _##MethodName)(ArgumentsDeclarations) = nullptr; \
	public: \
		GDINL ReturnType MethodName(ArgumentsDeclarations) const { \
			return self->_##MethodName(ArgumentsPassing); \
		}

#define GD_DEFINE_OGL_METHOD_LINKED(ReturnType, MethodName, ArgumentsDeclarations, ArgumentsPassing) \
	public: \
		GDINL static ReturnType MethodName(ArgumentsDeclarations) { \
			return ::gl##MethodName(ArgumentsPassing); \
		}

#if (!defined(GD_DEFINE_OGL_METHOD))
#	if (!defined(GD_HRI_OGL_ES)) 
#		define GD_DEFINE_OGL_METHOD(ReturnType, MethodName, ArgumentsDeclarations, ArgumentsPassing) GD_DEFINE_OGL_METHOD_DEFINITION(ReturnType, MethodName, ArgumentsDeclarations, ArgumentsPassing)
#	else	// if (!defined(IE_HRI_OGL_ES))
#		define GD_DEFINE_OGL_METHOD(ReturnType, MethodName, ArgumentsDeclarations, ArgumentsPassing) GD_DEFINE_OGL_METHOD_LINKED(ReturnType, MethodName, ArgumentsDeclarations, ArgumentsPassing)
#	endif	// if (!defined(IE_HRI_OGL_ES))
#endif	// if (!defined(GD_HRI_OGL_METHOD_DECL))

#define GD_DEFINE_OGL_METHOD_0(ReturnType, MethodName) \
	GD_DEFINE_OGL_METHOD(ReturnType, MethodName, \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(), \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS() \
		)

#define GD_DEFINE_OGL_METHOD_1(ReturnType, MethodName, T0, A0) \
	GD_DEFINE_OGL_METHOD(ReturnType, MethodName, \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(T0 A0), \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(A0) \
		)

#define GD_DEFINE_OGL_METHOD_2(ReturnType, MethodName, T0, A0, T1, A1) \
	GD_DEFINE_OGL_METHOD(ReturnType, MethodName, \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(T0 A0, T1 A1), \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(A0, A1) \
		)

#define GD_DEFINE_OGL_METHOD_3(ReturnType, MethodName, T0, A0, T1, A1, T2, A2) \
	GD_DEFINE_OGL_METHOD(ReturnType, MethodName, \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(T0 A0, T1 A1, T2 A2), \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(A0, A1, A2) \
		)

#define GD_DEFINE_OGL_METHOD_4(ReturnType, MethodName, T0, A0, T1, A1, T2, A2, T3, A3) \
	GD_DEFINE_OGL_METHOD(ReturnType, MethodName, \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(T0 A0, T1 A1, T2 A2, T3 A3), \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(A0, A1, A2, A3) \
		)

#define GD_DEFINE_OGL_METHOD_5(ReturnType, MethodName, T0, A0, T1, A1, T2, A2, T3, A3, T4, A4) \
	GD_DEFINE_OGL_METHOD(ReturnType, MethodName, \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(T0 A0, T1 A1, T2 A2, T3 A3, T4 A4), \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(A0, A1, A2, A3, A4) \
		)

#define GD_DEFINE_OGL_METHOD_6(ReturnType, MethodName, T0, A0, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5) \
	GD_DEFINE_OGL_METHOD(ReturnType, MethodName, \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(T0 A0, T1 A1, T2 A2, T3 A3, T4 A4, T5 A5), \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(A0, A1, A2, A3, A4, A5) \
		)

#define GD_DEFINE_OGL_METHOD_7(ReturnType, MethodName, T0, A0, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6) \
	GD_DEFINE_OGL_METHOD(ReturnType, MethodName, \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(T0 A0, T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6), \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(A0, A1, A2, A3, A4, A5, A6) \
		)

#define GD_DEFINE_OGL_METHOD_8(ReturnType, MethodName, T0, A0, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7) \
	GD_DEFINE_OGL_METHOD(ReturnType, MethodName, \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(T0 A0, T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7), \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(A0, A1, A2, A3, A4, A5, A6, A7) \
		)

#define GD_DEFINE_OGL_METHOD_9(ReturnType, MethodName, T0, A0, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7, T8, A8) \
	GD_DEFINE_OGL_METHOD(ReturnType, MethodName, \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(T0 A0, T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8), \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(A0, A1, A2, A3, A4, A5, A6, A7, A8) \
		)

#define GD_DEFINE_OGL_METHOD_10(ReturnType, MethodName, T0, A0, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7, T8, A8, T9, A9) \
	GD_DEFINE_OGL_METHOD(ReturnType, MethodName, \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(T0 A0, T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8, T9 A9), \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) \
		)

//////////////////////////////////////////////////////////////////////////
// Some OpenGL <-> Goddamn coding style conversions.
//////////////////////////////////////////////////////////////////////////

#ifndef GD_ENGINE_RENDERER_IMPL_OGL
#define GD_ENGINE_RENDERER_IMPL_OGL
#	define GL_GetError glGetError
#endif

//////////////////////////////////////////////////////////////////////////
// Buffers
//////////////////////////////////////////////////////////////////////////

// void glGenBuffers(GLsizei n, GLuint* buffers);
GD_DEFINE_OGL_METHOD_2(
	void, GenBuffers,
	GLsizei const, n, 
	GLuint* const, buffers
	)

// void glDeleteBuffers(GLsizei n, GLuint* buffers);
GD_DEFINE_OGL_METHOD_2(
	void, DeleteBuffers,
	GLsizei const, n,
	GLuint* const, buffers
	)

// void glBindBuffer(GLenum target, GLuint buffer);
GD_DEFINE_OGL_METHOD_2(
	void, BindBuffer,
	GLenum const, target,
	GLuint const, buffer
	)

// void glBufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);
GD_DEFINE_OGL_METHOD_4(
	void, BufferData,
	GLenum         const, target, 
	GLsizeiptr     const, size, 
	GLvoid const*  const, data,
	GLenum         const, usage
	)

#undef GD_DEFINE_OGL_METHOD_0
#undef GD_DEFINE_OGL_METHOD_1
#undef GD_DEFINE_OGL_METHOD_2
#undef GD_DEFINE_OGL_METHOD_3
#undef GD_DEFINE_OGL_METHOD_4
#undef GD_DEFINE_OGL_METHOD_5
#undef GD_DEFINE_OGL_METHOD_6
#undef GD_DEFINE_OGL_METHOD_7
#undef GD_DEFINE_OGL_METHOD_8
#undef GD_DEFINE_OGL_METHOD_9
#undef GD_DEFINE_OGL_METHOD_10
#undef GD_DEFINE_OGL_METHOD_GLUE_ARGS
#undef GD_DEFINE_OGL_METHOD
