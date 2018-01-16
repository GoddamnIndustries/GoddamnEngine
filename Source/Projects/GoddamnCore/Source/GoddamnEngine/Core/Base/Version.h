// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Base/Version.h
 * @note This file should be never directly included, please consider using <GoddamnEngine/Include.h> instead.
 * Contains version definitions for GoddamnEngine.
 */
#pragma once
#if !defined(GD_INSIDE_INCLUDE_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Include.h> instead.
#endif	// if !defined(GD_INSIDE_INCLUDE_H)

#include <GoddamnEngine/Include.h>

#define GD_ENGINE_VERSION_MAJOR					2018
#define GD_ENGINE_VERSION_MINOR					1
#define GD_ENGINE_VERSION_PATCH					0
#define GD_ENGINE_VERSION						GD_STRIGIFY(GD_ENGINE_VERSION_MAJOR) "." GD_STRIGIFY(GD_ENGINE_VERSION_MINOR) "." GD_STRIGIFY(GD_ENGINE_VERSION_PATCH) 

#define GD_ENGINE_NAME							"Goddamn Engine"
#define GD_ENGINE_TITLE							"Goddamn Engine"
#define GD_ENGINE_ASSEMBLY_COMPANY				"(C) Goddamn Industries"
#define GD_ENGINE_ASSEMBLY_PRODUCT				"Goddamn Engine"
#define GD_ENGINE_ASSEMBLY_COPYRIGHT			"Copyright \0xA9 2011 - 2018"##GD_ENGINE_ASSEMBLY_COMPANY
#define GD_ENGINE_ASSEMBLY_TRADEMARK			"Goddamn Engine"
#define GD_ENGINE_ASSEMBLY_GUID					"D2D6C4B5-41CD-4986-8CAA-538591C82A0C"
#define GD_ENGINE_ASSEMBLY_VERSION				"1.0.0.0"
#define GD_ENGINE_ASSEMBLY_FILEVERSION			"1.0.0.0"

#if !GD_RESOURCE_COMPILER
#	define GD_NAMESPACE Goddamn
#	if (defined(GD_NAMESPACE))
#		define GD_NAMESPACE_BEGIN namespace GD_NAMESPACE {		//!< Declaration of the namespace where entire API is located.
#		define GD_NAMESPACE_END   }	// namespace GD_NAMESPACE  	//!< End of main namespace declaration.
#		define GD				  ::GD_NAMESPACE
#		define GD_USING_NAMESPACE using namespace GD
#	else	// if (defined(GD_NAMESPACE))
#		define GD_NAMESPACE_BEGIN								//!< Dummy for namespace begin.
#		define GD_NAMESPACE_END									//!< Dummy for namespace end.
#		define GD ::
#		define GD_USING_NAMESPACE
#	endif	// if (defined(GD_NAMESPACE))
#endif	// if !GD_RESOURCE_COMPILER
