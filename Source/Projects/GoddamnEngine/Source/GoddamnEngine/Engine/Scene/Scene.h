#ifndef GD_COMPONENTS_SCENE
#define GD_COMPONENTS_SCENE

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/RefPtr.h>
#include <GoddamnEngine/Core/Templates/Singleton.h>
#include <GoddamnEngine/Engine/Component/Component.h>

GD_NAMESPACE_BEGIN

	class GameObject;

	class Scene : public Component, public Singleton<Scene>
	{
		friend class Application;
		typedef UInt8 SceneFlags;

	private:
		GD_CLASSINFO_DEFINITION(Scene, Object, GDAPI);

		RefPtr<GameObject> staticComponentsManager;

	public:
		enum : SceneFlags
		{
			FlagsUsingOwnStaticComponentsManager = GD_BIT(0),	///< This scene uses shared static components list
			FlagsNone = 0
		};

		GDAPI Scene(const SceneFlags sceneFlags = Scene::FlagsNone);

		/// @brief				Creates a new game object
		/// @returns			Newly created game object
		GDAPI GameObject* CreateGameObject();

		/// @brief				Removes a game object, component or asset.
		/// @param gameObject	Game object to destroy
		GDAPI void DestroyGameObject(GameObject* const gameObject);
		
		/// @brief				Destroys the object object immediately. It is strongly recommended to use Destroy instead.
		/// @param gameObject	Game object to destroy immediately
		GDAPI void DestroyGameObjectImmediate(GameObject* const gameObject);
	
		/// @brief				Clones the original game object and returns the clone.
		/// @param gameObject	Game object to clone
		/// @returns			Clone or original game object
		GDAPI GameObject* Instantiate(GameObject* const gameObject);
	};

	class SceneManager : public Object,
						 public Singleton<SceneManager>
	{
	private:
		GD_CLASSINFO_DEFINITION(SceneManager, Object, GDAPI);

	public:
		RefPtr<GameObject> const SharedStaticComponentManager;	///< Default static components manager used

		GDAPI SceneManager();
	};

GD_NAMESPACE_END

#include <GoddamnEngine/Engine/Scene/Scene.inl>

#endif