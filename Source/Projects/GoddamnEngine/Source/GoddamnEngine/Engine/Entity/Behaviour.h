// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Engine/Entity/Behaviour.h
 * File contains base classes for the entity-component system.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Object/Serialization/Serialization.h>

#define GD_ENTITY_KERNEL	//!< Use this macro to mark declarations inside Object system, that couldn't be implemented outside Object.cpp
#define GD_ENTITY_HELPER	//!< Use this macro to mark declarations inside Object system, that use kernel API and relate to the Object system.
#define GD_ENTITY_GENERATED	//!< Use this macro to mark declarations inside Object system, that would be generated.

GD_NAMESPACE_BEGIN

	class Behaviour;
	class Component;
	class Transform;
	class Entity;
	
	// **------------------------------------------------------------------------------------------**
	//! Declares a behaviour for entity-component system objects.
	// **------------------------------------------------------------------------------------------**
	class Behaviour : public SerializableObject
	{
		GD_DECLARE_OBJECT_INTRINSIC_ENGINE(Behaviour, SerializableObject);

	public:

		/*!
		 * @brief Callback being triggered after this object is initialized (or reset).
		 * This function may be useful for assigning properties default values. 
		 */
		GDAPI virtual void OnInit() {}

		/*!
		 * @brief Callback being triggered after all object were initialized (or reset).
		 * This function may be useful for assigning properties default values. 
		 */
		GDAPI virtual void OnStart() {}

		/*!
		 * @brief Callback being triggered each frame, before all objects been updated.
		 * This function may be useful for implementing gameplay-related logic. 
		 */
		GDAPI virtual void OnPreUpdate() {}

		/*!
		 * @brief Callback being triggered each frame.
		 * This function may be useful for implementing gameplay-related logic. 
		 */
		GDAPI virtual void OnUpdate() {}
		
		/*!
		 * @brief Callback being triggered each frame, after all objects been updated.
		 * This function may be useful for implementing gameplay-related logic. 
		 */
		GDAPI virtual void OnPostUpdate() {}

		/*!
		 * @brief Callback being triggered each frame from the renderer thread.
		 * This function may be useful for implementing rendering-related logic. 
		 */
		GDAPI virtual void OnRender() {}

	};	// class Behaviour

GD_NAMESPACE_END
