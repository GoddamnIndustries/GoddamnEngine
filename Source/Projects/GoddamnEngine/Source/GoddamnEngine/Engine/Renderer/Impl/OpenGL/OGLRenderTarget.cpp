/// ==========================================================================================
/// OGLRenderTarget.cpp: Render target Direct3D11 implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 17.02.2014  - Created by James Jhuighuy
/// ==========================================================================================

#include <GoddamnEngine/Engine/Renderer/Impl/OpenGL/OGLRenderTarget.h>
#include <GoddamnEngine/Engine/Renderer/Impl/OpenGL/OGLTexture2D.h>

GD_NAMESPACE_BEGIN

	HRIRenderTarget* HROGLInterface::CreateRenderTarget(HRIRenderTargetCtorInfo const& CtorInfo)
	{
		throw 0;
	//	return new HRIOGLRenderTarget(CtorInfo);
	}

GD_NAMESPACE_END
