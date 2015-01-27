/// ==========================================================================================
/// Effect.h - Shader program resource interface.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// 
/// History:
///		* --.01.2012 - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_RESOURCE_IMPL_EFFECT
#define GD_RESOURCE_IMPL_EFFECT

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Resource/Resource.h>
#include <GoddamnEngine/Engine/Renderer/Shader/Shader.h>

GD_NAMESPACE_BEGIN
	
	/// Class handles shader programs uploading to system
	class RSEffect final : public Resource
	{
	private:
		GD_CLASSINFO_DEFINITION(RSEffect, Resource, GDAPI);
		RefPtr<HRIShaderProgram> EffectShaderProgram = nullptr;

		GDINL explicit RSEffect(String const& ID) : Resource(ID) { }
		GDINL virtual ~RSEffect() { }

	public:
		/// Returns shader program that was loaded from resource
		GDINL RefPtr<HRIShaderProgram> GetEffectShaderProgram() const { return this->EffectShaderProgram; }

	protected:
		GDAPI virtual void OnResourceLoad(UniquePtr<InputStream> const& InputResourceData) override final;
		GDAPI virtual void OnResourceUnload() override final;
	};	// class RSEffect

GD_NAMESPACE_END

#endif