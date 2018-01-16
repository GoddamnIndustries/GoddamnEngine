// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Engine/Entity/Entity.cpp
 * File contains entity class for the entity-component system.
 */
#include <GoddamnEngine/Engine/Entity/Entity.h>

GD_NAMESPACE_BEGIN

	GD_IMPLEMENT_OBJECT_INTRINSIC(Behaviour);
GD_IMPLEMENT_OBJECT_INTRINSIC(Entity);
	GD_IMPLEMENT_OBJECT_INTRINSIC(Component);

	/*!
	 * Finds a component of a specified class and returns it.
	 * 
	 * @param klass Desired component class.
	 * @returns Inner component of a specified class, nullptr if nothing was found.
	 */
	GDAPI RefPtr<Component> Entity::GetComponent(ObjectClassPtr const klass) const
	{
		GD_DEBUG_VERIFY(klass != nullptr, "Null pointer class was specified.");
		auto const componentIter = Algo::FindFirstIf(m_Components, [&klass](auto const& component)
		{
			return component->GetClass() == klass;
		});
		return componentIter != m_Components.End() ? *componentIter : nullptr;
	}

	/*!
	 * Adds a component of a specified type to this entity.
	 * 
	 * @param klass Desired component class.
	 * @returns Newly created component of a specified class.
	 */
	GDAPI RefPtr<Component> Entity::AddComponent(ObjectClassPtr const klass)
	{
		GD_DEBUG_VERIFY(klass != nullptr, "Null pointer class was specified.");
		
		auto const component = object_cast<RefPtr<Component>>(CreateOrFindClassRelatedObjectByGUID(EmptyGUID, klass));
		m_Components.InsertLast(component);
		return component;
	}

	/*!
	 * @brief Callback being triggered after this object is initialized (or reset).
	 * This function may be useful for assigning properties default values. 
	 */
	GDAPI void Entity::OnInit() 
	{
		for (auto& component : m_Components)
		{
			component->OnInit();
		}
	}

	/*!
	 * @brief Callback being triggered after all object were initialized (or reset).
	 * This function may be useful for assigning properties default values. 
	 */
	GDAPI void Entity::OnStart() 
	{
		for (auto& component : m_Components)
		{
			component->OnStart();
		}
	}

	/*!
	 * @brief Callback being triggered each frame, before all objects been updated.
	 * This function may be useful for implementing gameplay-related logic. 
	 */
	GDAPI void Entity::OnPreUpdate() 
	{
		for (auto& component : m_Components)
		{
			component->OnPreUpdate();
		}
	}

	/*!
	 * @brief Callback being triggered each frame.
	 * This function may be useful for implementing gameplay-related logic. 
	 */
	GDAPI void Entity::OnUpdate() 
	{
		for (auto& component : m_Components)
		{
			component->OnUpdate();
		}
	}
		
	/*!
	 * @brief Callback being triggered each frame, after all objects been updated.
	 * This function may be useful for implementing gameplay-related logic. 
	 */
	GDAPI void Entity::OnPostUpdate() 
	{
		for (auto& component : m_Components)
		{
			component->OnPostUpdate();
		}
	}

	/*!
	 * @brief Callback being triggered each frame from the renderer thread.
	 * This function may be useful for implementing rendering-related logic. 
	 */
	GDAPI void Entity::OnRender()
	{
		for (auto& component : m_Components)
		{
			component->OnRender();
		}
	}

GD_NAMESPACE_END
