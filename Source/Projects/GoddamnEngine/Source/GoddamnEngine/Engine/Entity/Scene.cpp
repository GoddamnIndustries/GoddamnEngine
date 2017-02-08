// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Engine/Entity/Scene.cpp
 * File contains scene class for the entity-entity system.
 */
#include <GoddamnEngine/Engine/Entity/Scene.h>

GD_NAMESPACE_BEGIN

	GD_IMPLEMENT_OBJECT(GDAPI, Scene);

	GDAPI Scene::~Scene()
	{
	}

	/*!
	 * Adds an entity onto the scene.
	 * @returns Newly created entity.
	 */
	GDAPI RefPtr<Entity> Scene::AddEntity()
	{
		auto const entity = FindOrCreateObject<Entity>(EmptyGUID);
		m_Entities.InsertLast(entity);
		return entity;
	}

	/*!
	 * @brief Callback being triggered after this object is initialized (or reset).
	 * This function may be useful for assigning properties default values. 
	 */
	GDAPI void Scene::OnInit() 
	{
		for (auto& entity : m_Entities)
		{
			entity->OnInit();
		}
	}

	/*!
	 * @brief Callback being triggered after all object were initialized (or reset).
	 * This function may be useful for assigning properties default values. 
	 */
	GDAPI void Scene::OnStart() 
	{
		for (auto& entity : m_Entities)
		{
			entity->OnStart();
		}
	}

	/*!
	 * @brief Callback being triggered each frame, before all objects been updated.
	 * This function may be useful for implementing gameplay-related logic. 
	 */
	GDAPI void Scene::OnPreUpdate() 
	{
		for (auto& entity : m_Entities)
		{
			entity->OnPreUpdate();
		}
	}

	/*!
	 * @brief Callback being triggered each frame.
	 * This function may be useful for implementing gameplay-related logic. 
	 */
	GDAPI void Scene::OnUpdate() 
	{
		for (auto& entity : m_Entities)
		{
			entity->OnUpdate();
		}
	}
		
	/*!
	 * @brief Callback being triggered each frame, after all objects been updated.
	 * This function may be useful for implementing gameplay-related logic. 
	 */
	GDAPI void Scene::OnPostUpdate() 
	{
		for (auto& entity : m_Entities)
		{
			entity->OnPostUpdate();
		}
	}

	/*!
	 * @brief Callback being triggered each frame from the renderer thread.
	 * This function may be useful for implementing rendering-related logic. 
	 */
	GDAPI void Scene::OnRender()
	{
		for (auto& entity : m_Entities)
		{
			entity->OnRender();
		}
	}

GD_NAMESPACE_END
