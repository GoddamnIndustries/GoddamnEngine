#if !defined(GD_CORE_OBJECT)
#	pragma error("Attempting to include 'Object.inl' file. Please, use 'Object.h' instead")
#endif

GD_NAMESPACE_BEGIN

	template<typename ObjectType, typename SearchingPredicateType>
	GDINL ObjectType const* Object::SearchForFirstChildObject(SearchingPredicateType const& Predicate) const
	{
		for (auto const ChildObject : IterateChildObjects<ObjectType>(self))
		{	// Iterating all child objects in direct order.
			if (Predicate(ChildObject)) return ChildObject;
		}

		return nullptr;
	}

	template<typename ObjectType, typename SearchingPredicateType>
	GDINL ObjectType const* Object::SearchForLastChildObject(SearchingPredicateType const& Predicate) const
	{
		for (auto const ChildObject : Reverse(IterateChildObjects<ObjectType>(self)))
		{	// Iterating all child objects in indirect order.
			if (Predicate(ChildObject)) return ChildObject;
		}

		return nullptr;
	}

	template<typename ObjectType, typename SearchingPredicateType>
	GDINL ObjectType* Object::SearchForFirstChildObject(SearchingPredicateType const& Predicate)
	{	 
		return const_cast<ObjectType*>(const_cast<Object const*>(self)->SearchForFirstChildObject<ObjectType>(Predicate));
	}

	template<typename ObjectType, typename SearchingPredicateType>
	GDINL ObjectType* Object::SearchForLastChildObject(SearchingPredicateType const& Predicate)
	{
		return const_cast<ObjectType*>(const_cast<Object const*>(self)->SearchForLastChildObject<ObjectType>(Predicate));
	}

GD_NAMESPACE_END
