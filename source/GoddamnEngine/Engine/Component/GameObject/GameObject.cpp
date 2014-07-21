#include <GoddamnEngine/Engine/Component/GameObject/GameObject.h>
#include <GoddamnEngine/Engine/Component/Transform/Transform.h>

GD_NAMESPACE_BEGIN

	GD_SERIALIZABLE_IMPLEMENTATION(GameObject, Component, GDAPI);

	/// ==========================================================================================
	// Constructors / Destructor
	/// ==========================================================================================

	GameObject::GameObject() :
		GD_EXTENDS_SERIALIZABLE(Component)
	{
		self->transform = self->AddComponent<Transform>();
		self->name = "GameObject";
		self->tag = "Untagged";
	}

	GameObject::~GameObject()
	{
	}

	Component* GameObject::GetComponent(TypeInformation const* typeinfo) const
	{
		for (Component* 
			component = (Component*)self->GetLastChildObject(); component != nullptr;
			component = (Component*)component->GetPreviousSiblingObject())
		{
			if (component->GetTypeInformation()->IsDerivedFrom(typeinfo))
			{
				return component;
			}
		}

		return (Component*)nullptr;
	}

	Component* GameObject::AddComponent(TypeInformation const* typeinfo)
	{
		GD_ASSERT((typeinfo->IsDerivedFrom(Component::GetClassTypeInformation())), 
			"'Component::AddComponent' error: invalid type information specified, type is not component-derived");

		GD_ASSERT((typeinfo->VirtualConstructor != nullptr),
			"'Component::AddComponent' error: invalid type information specified, type is non constructable");

		Component* const existingComponent = self->GetComponent(typeinfo);
		if (existingComponent != nullptr)
		{
			return existingComponent;
		}
		
		Component* const 
			component = (Component*)(typeinfo->VirtualConstructor(self, nullptr));
			component->AttachToObject(self);

		return (Component*)component;
	}

	void GameObject::RemoveComponent(TypeInformation const* typeinfo)
	{
		Component* const existingComponent = self->GetComponent(typeinfo);
		if (existingComponent != nullptr)
		{
			existingComponent->DetachFromParentObject();
		}
	}

GD_NAMESPACE_END