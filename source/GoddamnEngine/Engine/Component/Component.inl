/// ==========================================================================================
/// Component.inl - Base compinent class inline implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.06.2012 - Created by James Jhuighuy.
/// ==========================================================================================

#include <GoddamnEngine/Engine/Component/Impl/Transform/Transform.h>

GD_NAMESPACE_BEGIN

	/// ==========================================================================================
	/// Component class.
	/// ==========================================================================================

	inline void Component::OnInitializeSelf()
	{ 
		for (auto const AttachedComponent : IterateChildObjects<Component>(self)) {
			AttachedComponent->OnInitializeSelf();
		}
	}

	inline void Component::OnDestroySelf(bool const IsForceDestruction)
	{ 
		for (auto const AttachedComponent : IterateChildObjects<Component>(self)) {
			AttachedComponent->OnDestroySelf(IsForceDestruction);
		}
	}

	inline void Component::OnStartSelf()
	{ 
		for (auto const AttachedComponent : IterateChildObjects<Component>(self)) {
			AttachedComponent->OnStartSelf();
		}
	}

	inline void Component::OnUpdateSelf()
	{ 
		for (auto const AttachedComponent : IterateChildObjects<Component>(self)) {
			AttachedComponent->OnUpdateSelf();
		}
	}

	inline void Component::OnRenderSelf(RefPtr<Camera> const& TheCamera)
	{ 
		for (auto const AttachedComponent : IterateChildObjects<Component>(self)) {
			AttachedComponent->OnRenderSelf(TheCamera);
		}
	}

	GDINL RefPtr<GameObject> Component::GetGameObject()
	{
		return object_cast<GameObject*>(self->GetParentObject());
	}

	GDINL RefPtr<GameObject const> Component::GetGameObject() const
	{
		return object_cast<GameObject const*>(self->GetParentObject());
	}

	/// ==========================================================================================
	/// GameObject class.
	/// ==========================================================================================

	GDINL GameObject::GameObject()
	{
		self->AddComponent<Transform>();
	}

	GDINL RefPtr<Transform> GameObject::GetTransform()
	{
		if (self->CachedTransform == nullptr) {
			self->CachedTransform = self->GetComponent<Transform>();
			GD_DEBUG_ASSERT(self->CachedTransform != nullptr, "No Transform component was found.");
		}

		return self->CachedTransform;
	}

	GDINL RefPtr<Transform const> GameObject::GetTransform() const
	{
		return const_cast<GameObject*>(self)->GetTransform();
	}

GD_NAMESPACE_END
