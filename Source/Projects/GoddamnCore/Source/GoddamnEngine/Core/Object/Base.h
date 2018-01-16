// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Object/Base.h
 * File contains forward declaration for entities system.
 */
#pragma once

#include <GoddamnEngine/Include.h>

GD_NAMESPACE_BEGIN

	#define GD_OBJECT_KERNEL	//!< Use this macro to mark declarations inside Object system, that couldn't be implemented outside Object.cpp
	#define GD_OBJECT_HELPER	//!< Use this macro to mark declarations inside Object system, that use kernel API and relate to the Object system.
	#define GD_OBJECT_GENERATED	//!< Use this macro to mark declarations inside Object system, that would be generated.

	struct Struct;
	class  Object;
	class  ObjectClass;
	struct ObjectVisitor;
	struct ObjectRegistry;
	struct PropertyMetaInfo;

	/*!
	 * Represents pointer to class information.
	 */
	GD_OBJECT_HELPER using ObjectClassPtr = ObjectClass*;

	/*!
	 * Represents constructor procedure pointer for objects.
	 */
	GD_OBJECT_KERNEL using ObjectCtorProc = Object*(*)();

	template<typename TObjectDest, typename TObjectSource>
	GDINL GD_OBJECT_KERNEL static TObjectDest object_cast(TObjectSource const sourceObject);

GD_NAMESPACE_END
