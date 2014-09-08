#include <GoddamnEngine/Engine/Scene/Scene.h>
#include <GoddamnEngine/Engine/Component/Static/StaticComponent.h>

GD_NAMESPACE_BEGIN

	GD_TYPEINFORMATION_IMPLEMENTATION(Scene, Object, GDAPI);
	GD_SINGLETON_IMPLEMENTATION(Scene);

	GD_TYPEINFORMATION_IMPLEMENTATION(SceneManager, Object, GDAPI);
	GD_SINGLETON_IMPLEMENTATION(SceneManager);

	Scene::Scene(const SceneFlags sceneFlags /* = Scene::UsingSharedStaticComponentsManager */)
	{
		//if ((sceneFlags & Scene::UsingOwnStaticComponentsManager) != 0)
		//{
		//	GD_ASSERT(false, 
		//		"'Scene::Scene' internal error: "
		//		"using 'UsingOwnStaticComponentsManager' is not allowed");

		//	this->staticComponentsManager = new GameObject();
		//	StaticComponent::InitializeAsHandler(this->staticComponentsManager.GetPointer());
		//}
		//else
		//{
		//	//this->staticComponentsManager = SceneManager::GetInstance().SharedStaticComponentManager;
		//}
	}

	GameObject* Scene::CreateGameObject()
	{
		GameObject* const 
			gameObject = new GameObject();
			gameObject->AttachToObject(this);

		return gameObject;
	}

	void Scene::DestroyGameObject(GameObject* const gameObject)
	{
		
	}

	void Scene::DestroyGameObjectImmediate(GameObject* const gameObject)
	{
		gameObject->RemoveReference();
	}

	SceneManager::SceneManager() :
		SharedStaticComponentManager(new GameObject())
	{
		//RefPtr<Scene> scene = this->AdoptChildObject(new Scene());
		//scene->AdoptChildObject(this->SharedStaticComponentManager.GetPointer());


	}

GD_NAMESPACE_END
