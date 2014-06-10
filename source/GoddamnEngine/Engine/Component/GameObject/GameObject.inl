#if !defined(GD_COMPONENT_GAME_OBJECT)
#	pragma error("Attempting to include 'GameObject.inl' file. Please, use 'GameObject.h' instead")
#endif

GD_NAMESPACE_BEGIN

	//////////////////////////////////////////////////////////////////////////
	// Getters / Setters
	//////////////////////////////////////////////////////////////////////////

	GDINL const String&	GameObject::GetName() const
	{ 
		return self->name; 
	}

	GDINL void GameObject::SetName(const String& name)	
	{ 
		self->name = name; 
	}

	GDINL const String&	GameObject::GetTag() const
	{ 
		return self->tag; 
	}

	GDINL void GameObject::SetTag(const String& tag)	
	{ 
		self->tag = tag; 
	}

	GDINL void GameObject::Enable(bool enabled)
	{
		self->Component::Enable(enabled);
		for (Component* 
			component = (Component*)self->GetLastChildObject();
			component = (Component*)component->GetPreviousSiblingObject();)
		{
			component->Enable(enabled);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Component Management
	//////////////////////////////////////////////////////////////////////////

	template<class ComponentType>
	GDINL ComponentType* GameObject::GetComponent() const
	{
		static_assert((TypeTraits::IsBaseType<Component, ComponentType>::Value),
			"'GameObject::GetComponent' error: type 'ComponentType' should be derived from 'Component'");

		return (ComponentType*)self->GetComponent(ComponentType::GetClassTypeInformation());
	}

	template<class ComponentType>
	GDINL ComponentType* GameObject::AddComponent()
	{
		static_assert((TypeTraits::IsBaseType<Component, ComponentType>::Value),
			"'GameObject::AddComponent' error: type 'ComponentType' should be derived from 'Component'");

		return (ComponentType*)self->AddComponent(ComponentType::GetClassTypeInformation());
	}

	template<class ComponentType>
	GDINL void GameObject::RemoveComponent()
	{
		static_assert((TypeTraits::IsBaseType<Component, ComponentType>::Value),
			"'GameObject::RemoveComponent' error: type 'ComponentType' should be derived from 'Component'");

		return self->RemoveComponent(ComponentType::GetClassTypeInformation());
	}

GD_NAMESPACE_END
