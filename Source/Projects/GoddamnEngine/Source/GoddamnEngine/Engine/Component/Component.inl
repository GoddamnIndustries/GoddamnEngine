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
		for (auto const AttachedComponent : IterateChildObjects<Component>(this)) {
			AttachedComponent->OnInitializeSelf();
		}
	}

	inline void Component::OnDestroySelf(bool const IsForceDestruction)
	{ 
		for (auto const AttachedComponent : IterateChildObjects<Component>(this)) {
			AttachedComponent->OnDestroySelf(IsForceDestruction);
		}
	}

	inline void Component::OnStartSelf()
	{ 
		for (auto const AttachedComponent : IterateChildObjects<Component>(this)) {
			AttachedComponent->OnStartSelf();
		}
	}

	inline void Component::OnUpdateSelf()
	{ 
		for (auto const AttachedComponent : IterateChildObjects<Component>(this)) {
			AttachedComponent->OnUpdateSelf();
		}
	}

	inline void Component::OnRenderSelf(RefPtr<Camera> const& TheCamera)
	{ 
		for (auto const AttachedComponent : IterateChildObjects<Component>(this)) {
			AttachedComponent->OnRenderSelf(TheCamera);
		}
	}

	GDINL RefPtr<GameObject> Component::GetGameObject()
	{
		return object_cast<GameObject*>(this->GetParentObject());
	}

	GDINL RefPtr<GameObject const> Component::GetGameObject() const
	{
		return object_cast<GameObject const*>(this->GetParentObject());
	}

	/// ==========================================================================================
	/// GameObject class.
	/// ==========================================================================================

	GDINL GameObject::GameObject()
	{
		this->AddComponent<Transform>();
	}

	GDINL RefPtr<Transform> GameObject::GetTransform()
	{
		if (this->CachedTransform == nullptr) {
			this->CachedTransform = this->GetComponent<Transform>();
			GD_DEBUG_ASSERT(this->CachedTransform != nullptr, "No Transform component was found.");
		}

		return this->CachedTransform;
	}

	GDINL RefPtr<Transform const> GameObject::GetTransform() const
	{
		return const_cast<GameObject*>(this)->GetTransform();
	}

GD_NAMESPACE_END
