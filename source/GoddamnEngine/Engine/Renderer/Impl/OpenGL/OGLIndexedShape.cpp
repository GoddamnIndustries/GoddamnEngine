/// ==========================================================================================
/// OGLIndexedShape.cpp: Renderable shape OpenGL implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
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
