
GD_NAMESPACE_BEGIN

	GDINL Resource::Resource(String const& identifier) :
		Object(Object::TreeLockingFlagsDefaultResource),
		State(ResourceState::Unknown),
		Identifier(identifier)
	{
	}

	GDINL Resource::~Resource()
	{
	}

GD_NAMESPACE_END
