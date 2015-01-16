/// ==========================================================================================
/// D3D11LinkagePoint.h: Linkage point D3D11 implementation interface.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// 
/// History:
///		* 21.05.2014  - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_ENGINE_RENDERER_IMPL_D3D11_LINKAGE_POINT
#define GD_ENGINE_RENDERER_IMPL_D3D11_LINKAGE_POINT

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Renderer/Impl/Direct3D11/D3D11IndexedShape.h>
#include <GoddamnEngine/Engine/Renderer/Impl/Direct3D11/D3D11Renderer.h>
#include <GoddamnEngine/Engine/Renderer/Impl/Direct3D11/D3D11Shader.h>

GD_NAMESPACE_BEGIN

	/// Provides dynamic linkage of shader program and shape.
	class HRID3D11LinkagePoint final : public HRILinkagePoint
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRID3D11LinkagePoint, HRILinkagePoint, GDINT);
		RefPtr<HRID3D11ShaderProgram const> Effect;
		RefPtr<HRID3D11IndexedShape  const> IndexedShape;
		mutable bool IsRelinkingRequired = false;
		mutable struct LinkageCache final {
			UINT SlotsTotalCount = 0;
			UINT VerticesCount   = UINT_MAX;
			StaticVector<ID3D11Buffer*, GD_HRI_SEMANTICS_COUNT> Buffers;
			StaticVector<UINT,    GD_HRI_SEMANTICS_COUNT> Offsets;
			StaticVector<UINT,    GD_HRI_SEMANTICS_COUNT> Strides;
		} LinkingCache;

	public:
		GDINL explicit HRID3D11LinkagePoint() { }
		GDINL virtual ~HRID3D11LinkagePoint() { }
		
		GDINL virtual HRIShaderProgram const* GetShaderProgram() const final { return this->Effect.GetPointer(); }
		GDINL virtual HRIIndexedShape  const* GetIndexedShape () const final { return this->IndexedShape.GetPointer(); }
		GDINL virtual void SetShaderProgram(HRIShaderProgram const* const Effect) final
		{
			this->Effect = object_cast<HRID3D11ShaderProgram const*>(Effect);
			this->IsRelinkingRequired = true;
		}

		GDINL virtual void SetIndexedShape(HRIIndexedShape  const* const IndexedShape) final
		{
			this->IndexedShape = object_cast<HRID3D11IndexedShape const*>(IndexedShape);
			this->IsRelinkingRequired = true;
		}

		GDINT virtual void RenderSelf() const final;
	};	// class HRID3D11LinkagePoint

GD_NAMESPACE_END

#endif