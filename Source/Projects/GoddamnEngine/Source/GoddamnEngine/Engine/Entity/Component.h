// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Engine/Entity/Component.h
 * File contains component class for the entity-component system.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Entity/Behaviour.h>

GD_NAMESPACE_BEGIN

	// **------------------------------------------------------------------------------------------**
	//! Declares a base class for everything that can be attached to an entity.
	// **------------------------------------------------------------------------------------------**
	GD_ENTITY_KERNEL class Component : public Behaviour
	{
	private:
		GD_DECLARE_OBJECT_INTRINSIC(Component, Behaviour);
		
	//	GD_PROPERTY(private, RefPtr<Entity>, m_Entity);

	public:

		/*!
		 * Returns an entity this component is attached to.
		 */
		/*GDINL RefPtr<Entity> GetEntity() const
		{
			return m_Entity;
		}*/

		/*!
		 * Returns a transform attached to this component's entity.
		 */
	//	GDAPI RefPtr<Transform> GetTransform() const;

	};	// class Component

GD_NAMESPACE_END
