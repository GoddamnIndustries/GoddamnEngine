/// ==========================================================================================
/// OGLIndexedShape.cpp: Renderable shape OpenGL implementation.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// 
/// History:
///		* 22.05.2014  - Created by James Jhuighuy
/// ==========================================================================================

#include <GoddamnEngine/Engine/Renderer/Impl/OpenGL/OGLIndexedShape.h>
#include <GoddamnEngine/Engine/Renderer/Impl/OpenGL/OGLBuffer.h>

GD_NAMESPACE_BEGIN

	HRIIndexedShape* HROGLInterface::CreateIndexedShape()
	{
		return new HRIOGLIndexedShape();
	}

GD_NAMESPACE_END
