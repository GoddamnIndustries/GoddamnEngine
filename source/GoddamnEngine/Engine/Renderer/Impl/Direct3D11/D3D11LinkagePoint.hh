//////////////////////////////////////////////////////////////////////////
/// D3D11LinkagePoint.hh: Linkage point D3D11 implementation interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 21.05.2014  - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_ENGINE_RENDERER_IMPL_D3D11_LINKAGE_POINT
#define GD_ENGINE_RENDERER_IMPL_D3D11_LINKAGE_POINT

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Engine/Renderer/Impl/Direct3D11/D3D11IndexedShape.hh>
#include <GoddamnEngine/Engine/Renderer/Impl/Direct3D11/D3D11Renderer.hh>
#include <GoddamnEngine/Engine/Renderer/Impl/Direct3D11/D3D11Shader.hh>

GD_NAMESPACE_BEGIN

	/// Provides dynamic linkage of shader program and shape.
	class HRID3D11LinkagePoint final : public HRILinkagePoint
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRID3D11LinkagePoint, HRILinkagePoint, GDINT);
		RefPtr<HRID3D11ShaderProgram const> ShaderProgram;
		RefPtr<HRID3D11IndexedShape  const> IndexedShape;
		mutable bool IsRelinkingRequired = false;
		mutable struct LinkageCache final {
			UINT SlotsTotalCount = 0;
			UINT VerticesCount   = UINT_MAX;
			StackVector<ID3D11Buffer*, GD_HRI_SEMANTICS_COUNT> Buffers;
			StackVector<UINT,          GD_HRI_SEMANTICS_COUNT> Offsets;
			StackVector<UINT,          GD_HRI_SEMANTICS_COUNT> Strides;
		} LinkingCache;

	public:
		GDINL explicit HRID3D11LinkagePoint() { }
		GDINL virtual ~HRID3D11LinkagePoint() { }
		
		GDINL virtual HRIShaderProgram const* GetShaderProgram() const final { return self->ShaderProgram.GetPointer(); }
		GDINL virtual HRIIndexedShape  const* GetIndexedShape () const final { return self->IndexedShape.GetPointer(); }
		GDINL virtual void SetShaderProgram(HRIShaderProgram const* const ShaderProgram) final
		{
			self->ShaderProgram = object_cast<HRID3D11ShaderProgram const*>(ShaderProgram);
			self->IsRelinkingRequired = true;
		}

		GDINL virtual void SetIndexedShape(HRIIndexedShape  const* const IndexedShape) final
		{
			self->IndexedShape = object_cast<HRID3D11IndexedShape const*>(IndexedShape);
			self->IsRelinkingRequired = true;
		}

		GDINT virtual void RenderSelf() const final;
	};	// class HRID3D11LinkagePoint

GD_NAMESPACE_END

#endif