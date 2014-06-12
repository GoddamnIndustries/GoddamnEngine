//////////////////////////////////////////////////////////////////////////
/// HRIRenderTarget.h: Frame-buffer Direct3D11 interface
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 17.02.2014  - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_ENGINE_RENDERER_IMPL_D3D11_HRI_BUFFER
#define GD_ENGINE_RENDERER_IMPL_D3D11_HRI_BUFFER

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Engine/Renderer/Impl/Direct3D11/D3D11Renderer.hh>

GD_NAMESPACE_BEGIN

	class HRID3D11Texture2D;

	class HRID3D11RenderTarget : public HRIRenderTarget
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRID3D11RenderTarget, HRIRenderTarget, GDINT);
		StackVector<ID3D11RenderTargetView*, GD_HRI_MAX_RENDER_TARGETS> RenderTargetRenderTargets;

	public:
		GDINT explicit HRID3D11RenderTarget(HRIRenderTargetCtorInfo const& gpuRenderTargetCtorInfo);
		GDINT virtual ~HRID3D11RenderTarget();
		GDINT virtual handle GetNativePointer() const final { return const_cast<ID3D11RenderTargetView**>(&self->RenderTargetRenderTargets[0]); }
		GDINT virtual void   BindRenderTarget () const final;
		GDINT virtual void UnbindRenderTarget () const final;
	};	// class HRID3D11RenderTarget

GD_NAMESPACE_END

#endif