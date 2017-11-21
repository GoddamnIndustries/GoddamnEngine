// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Engine/Resources/Resource.h
 * File contains base class for resources subsystem.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Object/Serialization/Serialization.h>

#define GD_RESOURCE_KERNEL
#define GD_RESOURCE_HELPER

GD_NAMESPACE_BEGIN

	// **------------------------------------------------------------------------------------------**
	//! Declares a base class for every resource.
	// **------------------------------------------------------------------------------------------**
	GD_RESOURCE_KERNEL class Resource : public Object
	{
		GD_DECLARE_OBJECT_INTRINSIC(Resource, Object);


	};	// class Resource

GD_NAMESPACE_END
