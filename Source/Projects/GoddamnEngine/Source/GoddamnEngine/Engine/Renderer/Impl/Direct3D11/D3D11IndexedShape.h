/// ==========================================================================================
/// D3D11IndexedShape.h: Renderable shape D3D11 implementation interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 21.05.2014  - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_ENGINE_RENDERER_IMPL_D3D11_INDEXED_SHAPE
#define GD_ENGINE_RENDERER_IMPL_D3D11_INDEXED_SHAPE

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Renderer/Impl/Direct3D11/D3D11Renderer.h>
#include <GoddamnEngine/Engine/Renderer/Impl/Direct3D11/D3D11Shader.h>

GD_NAMESPACE_BEGIN
	
	class HRID3D11IndexedShape final : public HRIIndexedShape
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRID3D11IndexedShape, HRIIndexedShape, GDINT);
		D3D_PRIMITIVE_TOPOLOGY D3D11TopologyType = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;

	public:
		GDINT virtual void SetTopologyType(HRITopologyType const TopologyType);
	};	// class HRID3D11IndexedShape

GD_NAMESPACE_END

#endif