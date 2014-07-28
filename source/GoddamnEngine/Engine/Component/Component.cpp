#include <GoddamnEngine/Engine/Component/Component.h>
#include <GoddamnEngine/Engine/Component/GameObject/GameObject.h>

#include <GoddamnEngine/Engine/Resource/Resource.h>

#include <GoddamnEngine/Core/Containers/Map/Map.h>
#include <GoddamnEngine/Core/Text/String/String.h>
#include <GoddamnEngine/Core/Containers/Vector/Vector.h>

GD_NAMESPACE_BEGIN

	GD_SERIALIZABLE_IMPLEMENTATION_C(Component, Serializable, GDAPI, nullptr);

	/// ==========================================================================================
	// 'Component's searchers
	/// ==========================================================================================

	Component::Component(ComponentAbilitiesFlags const abilitiesFlags /* = GD_COMPONENT_ABILITGDS_UNKNOWN */, 
		ObjectTreeLockingFlags const treeLockingFlags /* = Object::TreeLockingFlagsDefaultComponent */) :
		GD_EXTENDS_SERIALIZABLE(Serializable/*, treeLockingFlags*/),
		AbilitiesFlags(abilitiesFlags)
	{
		//// The Exies - What You Deserve
		//self->enabled = true;
		//self->gameObject = []() -> GameObject*
		//{
		//	ContextManager& contextManager = ContextManager::GetInstance();
		//	GameObjectsContext* gameObjectsContex = (GameObjectsContext*)contextManager.GetBindedInstance();

		//	return (gameObjectsContex != nullptr 
		//		? (GameObject*)gameObjectsContex->GetBindedInstance()
		//		: (GameObject*)nullptr
		//	);
		//}();

		//// Computing index of this component
		//const size_t componentIndex = [self, &name]() -> size_t
		//{
		//	const HashCode typeNameHash = name.GetHashCode();
		//	const size_t componentIndex = globalComponents->GetElementIndex(typeNameHash);

		//	if (componentIndex == -1)
		//	{
		//		globalComponents->AddElement(typeNameHash, ComponentsList());
		//		return globalComponents->GetSize() - 1;
		//	}

		//	return componentIndex;
		//}();

		//ComponentsList& 
		//	componentsList = globalComponents->GetElementAt(componentIndex).Value;
		//	componentsList.PushLast(self);

		//self->index = componentsList.GetSize() - 1;
	}

	// The Bloodhound Gang - I Hope You Die
	Component::~Component()
	{
		//// Decreasing other component's index and removing self from list
		//const HashCode typeNameHash = self->GetTypeInformation().GetName().GetHashCode();
		//const size_t componentIndex = globalComponents->GetElementIndex(typeNameHash);
		//if (componentIndex != -1)
		//{
		//	ComponentsList& componentsList = globalComponents->GetElementAt(componentIndex).Value;
		//	for (size_t cnt = self->GetComponentIndex() + 1; cnt < componentsList.GetSize(); cnt += 1)
		//	{
		//		Component*	
		//			component = componentsList[cnt];
		//			component->index -= 1;
		//	}

		//	componentsList.RemoveElementAt(index);
		//}
	}

GD_NAMESPACE_END
