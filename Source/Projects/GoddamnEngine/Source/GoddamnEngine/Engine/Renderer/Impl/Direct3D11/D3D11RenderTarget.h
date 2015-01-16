/// ==========================================================================================
/// D3D11RenderTarget.h: Render target Direct3D11 interface.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// 
/// History:
///		* 17.02.2014  - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_ENGINE_RENDERER_IMPL_D3D11_RENDER_TARGET
#define GD_ENGINE_RENDERER_IMPL_D3D11_RENDER_TARGET

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Renderer/Impl/Direct3D11/D3D11Renderer.h>

GD_NAMESPACE_BEGIN

	class HRID3D11Texture2D;

	class HRID3D11RenderTarget : public HRIRenderTarget
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRID3D11RenderTarget, HRIRenderTarget, GDINT);
		StaticVector<ID3D11RenderTargetView*, GD_HRI_MAX_RENDER_TARGETS> RenderTargetRenderTargets;

	public:
		GDINT explicit HRID3D11RenderTarget(HRIRenderTargetCtorInfo const& gpuRenderTargetCtorInfo);
		GDINT virtual ~HRID3D11RenderTarget();
		GDINT virtual handle GetNativePointer() const final { return const_cast<ID3D11RenderTargetView**>(&this->RenderTargetRenderTargets[0]); }
		GDINT virtual void   BindRenderTarget () const final;
		GDINT virtual void UnbindRenderTarget () const final;
	};	// class HRID3D11RenderTarget

GD_NAMESPACE_END

#endif