#include <GoddamnEngine/Engine/Component/Static/StaticComponent.h>
#include <GoddamnEngine/Engine/Component/Impl/Transform/Transform.h>

GD_NAMESPACE_BEGIN

	GD_SERIALIZABLE_IMPLEMENTATION_C(StaticComponent, Component, GDAPI, nullptr);

	StaticComponent::StaticComponent(const StaticComponentPriority priority) :
		Priority(priority)
	{
	}

	void StaticComponent::InitializeAsHandler(GameObject* const TheGameObject)
	{
		for (ITypeInformation const* TypeInfo = TypeInformation::GetLastRegistered(); TypeInfo != nullptr; TypeInfo = TypeInfo->PreviousType) {
			if ((TypeInfo->BaseType != StaticComponent::GetClassTypeInformation()) || (TypeInfo->VirtualConstructor == nullptr)) {
				continue;
			}

			RefPtr<StaticComponent> const TheStaticComponent = object_cast<RefPtr<StaticComponent>>(TheGameObject->AddComponent(TypeInfo));
		//	for (auto const TheStaticComponentIterator : IterateChildObjects<StaticComponent>(TheGameObject)) {
			for (StaticComponent*
				TheStaticComponentIterator = object_cast<StaticComponent*>(TheGameObject->GetTransform()->GetNextSiblingObject()); TheStaticComponent != TheStaticComponentIterator;
				TheStaticComponentIterator = object_cast<StaticComponent*>(TheStaticComponentIterator->GetNextSiblingObject())) {
				if (TheStaticComponentIterator->Priority <= TheStaticComponentIterator->Priority)	{
					TheStaticComponent->MoveToSiblingObject(TheStaticComponentIterator->GetPreviousSiblingObject());
					break;
				}
			}
		}
	}

GD_NAMESPACE_END
