/// ==========================================================================================
/// Component.h - Base compinent class interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.06.2012 - Created by James Jhuighuy.
/// ==========================================================================================

#pragma once
#ifndef GD_ENGINE_COMPONENT_COMPONENT
#define GD_ENGINE_COMPONENT_COMPONENT

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Events/Event.h>
#include <GoddamnEngine/Core/Object/Object.h>
#include <GoddamnEngine/Core/Containers/Pointer/RefPtr.h>

/// Legacy macros
#define Serializable Object
#define GD_SERIALIZATION_BEGIN(...)
#define GD_SERIALIZATION_END(...)
#define GD_EXTENDS_SERIALIZABLE_LIST(Serializable, ...) Serializable(__VA_ARGS__)
#define GD_EXTENDS_SERIALIZABLE(Serializable) GD_EXTENDS_SERIALIZABLE_LIST(Serializable)
#define GD_FIELD(Type, Name) Type Name

#define GD_SERIALIZABLE_DEFINITION			GD_TYPEINFORMATION_DEFINITION
#define GD_SERIALIZABLE_IMPLEMENTATION		GD_TYPEINFORMATION_IMPLEMENTATION
#define GD_SERIALIZABLE_IMPLEMENTATION_C	GD_TYPEINFORMATION_IMPLEMENTATION_C

GD_NAMESPACE_BEGIN

	class Component;
	class GameObject;
	
	class Camera;
	class Transform;

	/// Base class that is a simple part of each game object.
	$GD_REFLECTABLE()
	class Component : public Object
	{
		$GD_REFLECTABLE_BODY_GENERATED_CRAP();
		friend class Application;
		friend class GameObject;
		friend class Camera;
		friend class Scene;

	private:
		GD_SERIALIZABLE_DEFINITION(Component, Object, GDAPI);
		bool Enabled;

	protected:
		GDINL          Component() { }
		GDINL virtual ~Component() { }

	public /*Class API*/:
		/// Returns true if component is enabled. If component is disables then it does not takes part in scene update/rendering loop.
		/// @returns True if component is enabled.
		GDINL bool IsEnabled() const { return this->Enabled; }

		/// Enables / disables this component.
		/// @param Enabled Do we need to disable or enable the cmponent.
		GDINL void Enable(bool const Enabled = true) { this->Enabled = Enabled; }

		/// Returns GameObject this Component attached to.
		/// @returns GameObject this Component attached to.
		GDINL RefPtr<GameObject const> GetGameObject() const;
		GDINL RefPtr<GameObject      > GetGameObject();

	protected/*Class Messages Set*/:
		/// Method being called after component was constructed, 
		/// At this moment we can obtain game object this component is attached to and work with it.
		inline virtual void OnInitializeSelf();

		/// Method being called before destructor is called.
		///	If is normal destruction, at this moment no components are removed from this game object,
		/// otherwise this is force destruction. 
		inline virtual void OnDestroySelf(bool const IsForceDestruction);

		/// Method being called after before component`s first update
		inline virtual void OnStartSelf();

		/// Method being called each frame.
		inline virtual void OnUpdateSelf();

		/// Method each time engine renders this component.
		inline virtual void OnRenderSelf(RefPtr<Camera> const& TheCamera);
	};	// class Component

	/// Basic on-scene entity. Handles components.
	class GameObject final : public Component
	{
		friend class Application;
		friend class Transform;
		friend class Scene;

	private:
		GD_SERIALIZABLE_DEFINITION(GameObject, Component, GDAPI);
		RefPtr<Transform> mutable CachedTransform;

	public:
		String Name;
		String Tag;

		GDINL          GameObject();
		GDINL virtual ~GameObject() { }

		/// Disables all components that are attached to this enity.
		/// @param Enabled Do we need to disable or enable the cmponent.
		inline void Enable(bool const Enabled = true)
		{
			for (auto const AttachedComponent : IterateChildObjects<Component>(this)) {
				AttachedComponent->Enable(Enabled);
			}
		}

		/// @name Component Management
		/// @{

		/// Searches for attached components with specified type information
		/// @param typeinfo Type information for required component type
		/// @returns		Component attached to this object of this type or nullptr if nothing found
		GDAPI RefPtr<Component      > GetComponent(ITypeInformation const* const TypeInfo);
		GDINL RefPtr<Component const> GetComponent(ITypeInformation const* const TypeInfo) const
		{
			return const_cast<GameObject const*>(this)->GetComponent(TypeInfo);
		}

		GDINL RefPtr<Transform const> GetTransform() const;
		GDINL RefPtr<Transform      > GetTransform();

		template<typename ComponentType>
		GDINL RefPtr<ComponentType> GetComponent()
		{
			static_assert((TypeTraits::IsBaseType<Component, ComponentType>::Value), "'GameObject::GetComponent<T>()' error: 'T' should be derived from Component");
			return object_cast<RefPtr<ComponentType>>(this->GetComponent(ComponentType::GetClassTypeInformation()));
		}

		template<typename ComponentType>
		GDINL RefPtr<ComponentType const> GetComponent() const
		{
			return const_cast<RefPtr<ComponentType const>>(this->GetComponent<ComponentType>());
		}

		/// @brief			Adds component with specified type information.
		/// @param typeinfo Type information for required component type
		/// @returns		Newly created component of this type or existing
		GDAPI RefPtr<Component> AddComponent(ITypeInformation const* TypeInfo);

		template<typename ComponentType>
		GDINL RefPtr<ComponentType> AddComponent()
		{
			static_assert((TypeTraits::IsBaseType<Component, ComponentType>::Value), "'GameObject::AddComponent<T>()' error: 'T' should be derived from Component");
			return object_cast<RefPtr<ComponentType>>(this->AddComponent(ComponentType::GetClassTypeInformation()));
		}

		/// @brief			Removes component with specified type information (if exists in game object).
		/// @param typeinfo Type information for required component type
		GDAPI void RemoveComponent(ITypeInformation const* const TypeInfo);
		/// @see Component::RemoveComponent
		template<class ComponentType>
		GDINL void RemoveComponent()
		{
			static_assert((TypeTraits::IsBaseType<Component, ComponentType>::Value), "'GameObject::AddComponent<T>()' error: 'T' should be derived from Component");
			this->RemoveComponent(ComponentType::GetClassTypeInformation());
		}

		/// @}
	};	// class GameObject

GD_NAMESPACE_END

#include <GoddamnEngine/Engine/Component/Component.inl>

#endif
