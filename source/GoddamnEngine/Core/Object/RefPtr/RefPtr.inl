#if !defined(GD_CORE_OBJECT_REFPTR)
#	pragma error("Attempting to include 'RefPtr.inl' file. Please, use 'RefPtr.h' instead")
#endif

GD_NAMESPACE_BEGIN

	template<class ObjectType>
	GDINL RefPtr<ObjectType>::RefPtr(ObjectType* const object)
	{
		self->object = object;
	}

	template<class ObjectType>
	GDINL RefPtr<ObjectType>::RefPtr(const RefPtr<ObjectType>& refPtr)
	{
		SafeObtain(self->object = refPtr.object);
	}

	template<class ObjectType>
	GDINL RefPtr<ObjectType>::~RefPtr()
	{
		SafeRelease(self->object);
	}

	template<class ObjectType>
	GDINL const ObjectType* RefPtr<ObjectType>::GetPointer() const
	{
		return self->object;
	}

	template<class ObjectType>
	GDINL ObjectType* RefPtr<ObjectType>::GetPointer()
	{
		return self->object;
	}

	template<class ObjectType>
	GDINL bool RefPtr<ObjectType>::operator== (const RefPtr<ObjectType>& refPtr) const
	{
		return ((handle)self->object == (handle)refPtr->object);
	}

	template<class ObjectType>
	GDINL RefPtr<ObjectType>& RefPtr<ObjectType>::operator= (const RefPtr<ObjectType>& refPtr)
	{
		if (self != &refPtr)
		{
			SafeRelease(self->object);
			SafeObtain (self->object = refPtr.object);
		}

		return *self;
	}

	template<class ObjectType>
	GDINL RefPtr<ObjectType>& RefPtr<ObjectType>::operator= (ObjectType* const ptr)
	{
		SafeRelease(self->object);
		if (self->object != nullptr)
			SafeObtain(self->object = ptr);
		else
			self->object = ptr;
		return *self;
	}

	template<class ObjectType>
	GDINL const ObjectType* RefPtr<ObjectType>::operator-> () const
	{
		return self->GetPointer();
	}

	template<class ObjectType>
	GDINL ObjectType* RefPtr<ObjectType>::operator-> ()
	{
		return self->GetPointer();
	}

	template<class ObjectType>
	GDINL const ObjectType& RefPtr<ObjectType>::operator* () const
	{
		return (*self->GetPointer());
	}

	template<class ObjectType>
	GDINL ObjectType& RefPtr<ObjectType>::operator* ()
	{
		return (*self->GetPointer());
	}

	template<class ObjectType>
	GDINL RefPtr<ObjectType>& RefPtr<ObjectType>::operator++ ()
	{
		SafeObtain(self->object);
		return *self;
	}

	template<class ObjectType>
	GDINL RefPtr<ObjectType>& RefPtr<ObjectType>::operator-- ()
	{
		SafeRelease(self->object);
		return *self;
	}

GD_NAMESPACE_END