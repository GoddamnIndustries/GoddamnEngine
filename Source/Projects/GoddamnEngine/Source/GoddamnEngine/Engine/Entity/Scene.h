// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Engine/Entity/Scene.h
 * File contains scene class for the entity-component system.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Entity/Entity.h>

GD_NAMESPACE_BEGIN

	// **------------------------------------------------------------------------------------------**
	//! Declares a base class for everything that can be attached to an entity.
	// **------------------------------------------------------------------------------------------**
	GD_ENTITY_KERNEL class Scene : public Behaviour
	{
	private:
		GD_DECLARE_OBJECT_INTRINSIC(Scene, Behaviour);
		
		GD_PROPERTY(private, Vector<RefPtr<Entity>>, m_Entities);

	public:
		
		/*!
		 * Adds an entity onto the scene.
		 * @returns Newly created entity.
		 */
		GDAPI RefPtr<Entity> AddEntity();

	public:
		GDAPI void OnInit() override;
		GDAPI void OnStart() override;
		GDAPI void OnPreUpdate() override;
		GDAPI void OnUpdate() override;
		GDAPI void OnPostUpdate() override;
		GDAPI void OnRender() override;

	};	// class Scene

GD_NAMESPACE_END
