#include <GoddamnEngine/Engine/Component/Component.h>
#include <GoddamnEngine/Engine/Resource/Resource.h>
#include <GoddamnEngine/Core/Containers/Map.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Containers/Vector.h>

GD_NAMESPACE_BEGIN


	/// ==========================================================================================
	/// Component class.		
	/// Base class that is a simple part of each game object.
	/// ==========================================================================================

	GD_SERIALIZABLE_IMPLEMENTATION(Component, Object, GDAPI);

	/// ==========================================================================================
	/// GameObject class.
	/// ==========================================================================================

	GD_SERIALIZABLE_IMPLEMENTATION(GameObject, Component, GDAPI);

	RefPtr<Component> GameObject::GetComponent(IClassInfo const* const TypeInfo)
	{
		for (auto const AttachedComponent : IterateChildObjects<Component>(this)) {
			IClassInfo const* const AttachedComponentTypeInfo = AttachedComponent->GetClassInfo();
			if (AttachedComponentTypeInfo->IsDerivedFrom(TypeInfo)) {
				AttachedComponent->AddReference();
				return AttachedComponent;
			}
		}

		return nullptr;
	}

	RefPtr<Component> GameObject::AddComponent(IClassInfo const* const TypeInfo)
	{
		RefPtr<Component> const FoundAttachedComponent(this->GetComponent(TypeInfo));
		if (FoundAttachedComponent != nullptr) {
			return FoundAttachedComponent;
		}
		else {
			/*GD_DEBUG_ASSERT(TypeInfo->VirtualConstructor != nullptr, "Failed to create a instance of a abstract class.");
			RefPtr<Component> const CreatedComponent = object_cast<Component*>(TypeInfo->VirtualConstructor(nullptr, nullptr));
			CreatedComponent->AttachToObject(this);
			return CreatedComponent;*/
			return nullptr;
		}
	}

	void GameObject::RemoveComponent(IClassInfo const* const TypeInfo)
	{
		RefPtr<Component> const FoundAttachedComponent = this->GetComponent(TypeInfo);
		if (FoundAttachedComponent != nullptr) {
			FoundAttachedComponent->DetachFromParentObject();
		}
	}

GD_NAMESPACE_END
