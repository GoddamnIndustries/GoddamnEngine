/// ==========================================================================================
/// D3D11IndexedShape.cpp: Renderable shape D3D11 implementation.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// 
/// History:
///		* 22.05.2014  - Created by James Jhuighuy
/// ==========================================================================================

#include <GoddamnEngine/Engine/Renderer/Impl/Direct3D11/D3D11IndexedShape.h>
#include <GoddamnEngine/Engine/Renderer/Impl/Direct3D11/D3D11Buffer.h>

GD_NAMESPACE_BEGIN

	GD_CLASSINFO_IMPLEMENTATION(HRID3D11IndexedShape, HRIIndexedShape, GDINT);

	void HRID3D11IndexedShape::SetTopologyType(HRITopologyType const TopologyType)
	{
		static D3D_PRIMITIVE_TOPOLOGY const D3D11TopologyTranslationTable[] = {
			/* GD_HRI_TOPOLOGY_TYPE_TRIANGLE_LIST */ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		};

		GD_ASSERT(TopologyType < GD_ARRAY_SIZE(D3D11TopologyTranslationTable), "Invalid topology type specified.");
		this->D3D11TopologyType = D3D11TopologyTranslationTable[static_cast<size_t>(TopologyType)];
		this->HRIIndexedShape::SetTopologyType(TopologyType);
	}

	HRIIndexedShape* HRD3D11Interface::CreateIndexedShape()
	{
		return new HRID3D11IndexedShape();
	}

GD_NAMESPACE_END
