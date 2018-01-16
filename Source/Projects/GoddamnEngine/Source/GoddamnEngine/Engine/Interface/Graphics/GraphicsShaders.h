// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================


/*!
 * @file GoddamnEngine/Engine/Interface/Graphics/GraphicsShaders.h
 * File contains export interface for the engine shaders subsystem.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Interface/Graphics/Graphics.h>

GD_NAMESPACE_BEGIN

	// ------------------------------------------------------------------------------------------
	//! GPU buffer interface. Engine's graphics abstraction layer provides no separation for
	//! buffers of different type.
	GD_DINTERFACE()
	uinterface IGraphicsShaderReflection : public IGraphicsInterface 
	{
	};	// uinterface IGraphicsShaderReflection

GD_NAMESPACE_END
