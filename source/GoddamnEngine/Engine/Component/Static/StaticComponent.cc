#include <GoddamnEngine/Engine/Component/Static/StaticComponent.hh>
#include <GoddamnEngine/Engine/Component/Transform/Transform.hh>

GD_NAMESPACE_BEGIN

	GD_SERIALIZABLE_IMPLEMENTATION_C(StaticComponent, Component, GDAPI, nullptr);

	StaticComponent::StaticComponent(const StaticComponentPriority priority) :
		Priority(priority)
	{
	}

	void StaticComponent::InitializeAsHandler(GameObject* const gameObject)
	{
		for (TypeInformation const* 
			typeInformation = TypeInformation::GetLastRegistered(); typeInformation != nullptr;
			typeInformation = typeInformation->PreviousType)
		{
			if ((typeInformation->BaseType != StaticComponent::GetClassTypeInformation())
				|| (typeInformation->VirtualConstructor == nullptr))
			{
				continue;
			}

			StaticComponent* const staticComponent = (StaticComponent*)(gameObject->AddComponent(typeInformation));
			for (StaticComponent* 
				staticComponentIterator = (StaticComponent*)gameObject->GetTransform()->GetNextSiblingObject();
				staticComponentIterator != staticComponent; 
				staticComponentIterator = (StaticComponent*)staticComponentIterator->GetNextSiblingObject())
			{
				if (staticComponentIterator->Priority <= staticComponent->Priority)
				{
					staticComponent->MoveToSiblingObject(staticComponentIterator->GetPreviousSiblingObject());
					break;
				}
			}
		}
	}

GD_NAMESPACE_END
