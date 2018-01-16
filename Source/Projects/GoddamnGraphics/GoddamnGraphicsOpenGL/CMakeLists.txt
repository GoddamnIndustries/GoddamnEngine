cmake_minimum_required (VERSION 2.8)
project(GoddamnGraphicsOpenGL)
include_directories(H:/GoddamnEngine/source/Projects/GoddamnGraphics/GoddamnGraphicsOpenGL/Source
	H:/GoddamnEngine/source/Projects/GoddamnGraphics/GoddamnGraphicsOpenGL/Dependencies/glew/include
	H:/GoddamnEngine/source/Projects/GoddamnGraphics/GoddamnGraphicsOpenGL/Dependencies/glslang/include
	H:/GoddamnEngine/Source/Projects/GoddamnCore/Source
	H:/GoddamnEngine/Source/Projects/GoddamnEngine/Source
	)
add_library(GoddamnGraphicsOpenGL SHARED
	H:/GoddamnEngine/source/Projects/GoddamnGraphics/GoddamnGraphicsOpenGL/Source/GoddamnEngine/GraphicsOpenGL/GraphicsOpenGL.cpp
	H:/GoddamnEngine/source/Projects/GoddamnGraphics/GoddamnGraphicsOpenGL/Source/GoddamnEngine/GraphicsOpenGL/GraphicsOpenGL.h
	H:/GoddamnEngine/source/Projects/GoddamnGraphics/GoddamnGraphicsOpenGL/Source/GoddamnEngine/GraphicsOpenGL/GraphicsOpenGLBuffer.cpp
	H:/GoddamnEngine/source/Projects/GoddamnGraphics/GoddamnGraphicsOpenGL/Source/GoddamnEngine/GraphicsOpenGL/GraphicsOpenGLCommandList.cpp
	H:/GoddamnEngine/source/Projects/GoddamnGraphics/GoddamnGraphicsOpenGL/Source/GoddamnEngine/GraphicsOpenGL/GraphicsOpenGLFormat.h
	H:/GoddamnEngine/source/Projects/GoddamnGraphics/GoddamnGraphicsOpenGL/Source/GoddamnEngine/GraphicsOpenGL/GraphicsOpenGLPipelineState.cpp
	H:/GoddamnEngine/source/Projects/GoddamnGraphics/GoddamnGraphicsOpenGL/Source/GoddamnEngine/GraphicsOpenGL/GraphicsOpenGLRenderTarget.cpp
	H:/GoddamnEngine/source/Projects/GoddamnGraphics/GoddamnGraphicsOpenGL/Source/GoddamnEngine/GraphicsOpenGL/GraphicsOpenGLSampler.cpp
	H:/GoddamnEngine/source/Projects/GoddamnGraphics/GoddamnGraphicsOpenGL/Source/GoddamnEngine/GraphicsOpenGL/GraphicsOpenGLShaders.cpp
	H:/GoddamnEngine/source/Projects/GoddamnGraphics/GoddamnGraphicsOpenGL/Source/GoddamnEngine/GraphicsOpenGL/GraphicsOpenGLTextures.cpp
	H:/GoddamnEngine/source/Projects/GoddamnGraphics/GoddamnGraphicsOpenGL/Source/GoddamnEngine/GraphicsOpenGL/GraphicsOpenGLVertexArray.cpp
	H:/GoddamnEngine/source/Projects/GoddamnGraphics/GoddamnGraphicsOpenGL/Source/GoddamnEngine/GraphicsOpenGL/GraphicsOpenGLWindows.cpp
	H:/GoddamnEngine/source/Projects/GoddamnGraphics/GoddamnGraphicsOpenGL/Source/GoddamnEngine/GraphicsOpenGL/GraphicsOpenGLWindows.h
	)
