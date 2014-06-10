#if !defined(GD_ENGINE_COMPONENT_COMPONENT)
#	error Attempting to include 'Component.inl' file. Please, use 'Component.h' instead
#endif

GD_NAMESPACE_BEGIN
	
	GDINL bool Component::IsEnabled() const
	{
		return self->enabled; 
	}

	GDINL void Component::Enable(bool enabled /* = true */)
	{
		self->enabled = enabled; 
	}

	GDINL GameObject* Component::GetGameObject() const 
	{ 
		return ((GameObject*)self->GetParentObject()); 
	}

	GDINL Transform* Component::GetTransform() const
	{
		return self->transform;
	}

GD_NAMESPACE_END
