/// ==========================================================================================
/// OGLIndexedShape.h: Renderable shape OpenGL implementation interface.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// 
/// History:
///		* 21.05.2014  - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_ENGINE_RENDERER_IMPL_OGL_INDEXED_SHAPE
#define GD_ENGINE_RENDERER_IMPL_OGL_INDEXED_SHAPE

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Renderer/Impl/OpenGL/OGLRenderer.h>
#include <GoddamnEngine/Engine/Renderer/Impl/OpenGL/OGLShader.h>

GD_NAMESPACE_BEGIN

	class HRIOGLIndexedShape final : public HRIIndexedShape
	{
	private:
	//	GD_TYPEINFORMATION_DEFINITION(HRIOGLIndexedShape, HRIIndexedShape, GDINT);

	public:
		GDINT virtual void SetTopologyType(HRITopologyType const TopologyType) { }
	};	// class HRID3D11IndexedShape

GD_NAMESPACE_END

#endif
