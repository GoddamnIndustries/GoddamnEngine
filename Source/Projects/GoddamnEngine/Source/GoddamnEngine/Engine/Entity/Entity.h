// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Engine/Entity/Entity.h
 * File contains entity class for the entity-component system.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Entity/Component.h>

GD_NAMESPACE_BEGIN

	// **------------------------------------------------------------------------------------------**
	//! Declares a base class for everything that can be attached to an entity.
	// **------------------------------------------------------------------------------------------**
	GD_ENTITY_KERNEL class Entity : public Behaviour
	{
	private:
		GD_DECLARE_OBJECT_INTRINSIC(Entity, Behaviour);
		
		GD_PROPERTY(private, Vector<RefPtr<Component>>, m_Components);
		//RefPtr<Transform> m_Transform;

	public:

		/*!
		 * Returns a transform attached to this component's entity.
		 */
		/*GDINL RefPtr<Transform> GetTransform() const
		{
			return m_Transform;
		}*/

		/*!
		 * Finds a component of a specified class and returns it.
		 * 
		 * @param klass Desired component class.
		 * @returns Inner component of a specified class, nullptr if nothing was found.
		 */
		GDAPI RefPtr<Component> GetComponent(ObjectClassPtr const klass) const;
		template<typename TComponent>
		GDINL RefPtr<TComponent> GetComponent() const
		{
			return static_cast<RefPtr<TComponent>>(this->GetComponent(TComponent::GetStaticClass()));
		}

		/*!
		 * Adds a component of a specified type to this entity.
		 * 
		 * @param klass Desired component class.
		 * @returns Newly created component of a specified class.
		 */
		GDAPI RefPtr<Component> AddComponent(ObjectClassPtr const klass);
		template<typename TComponent>
		GDINL RefPtr<TComponent> AddComponent() const
		{
			return static_cast<RefPtr<TComponent>>(this->AddComponent(TComponent::GetStaticClass()));
		}

	public:
		GDAPI void OnInit() override;
		GDAPI void OnStart() override;
		GDAPI void OnPreUpdate() override;
		GDAPI void OnUpdate() override;
		GDAPI void OnPostUpdate() override;
		GDAPI void OnRender() override;

	};	// class Entity

GD_NAMESPACE_END
