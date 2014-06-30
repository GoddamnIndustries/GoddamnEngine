#ifndef GD_COMPONENT_GAME_OBJECT
#define GD_COMPONENT_GAME_OBJECT

#include <GoddamnEngine/Include.hh>

#include <GoddamnEngine/Engine/Component/Component.hh>
#include <GoddamnEngine/Engine/Component/Static/StaticComponent.hh>

GD_NAMESPACE_BEGIN

	/// @brief		Container for 'Component's. Stores and manager them.
	///	@see Component
	class GameObject : public Component
	{
		friend class Transform;
		friend class Application;
		friend class Scene;

		typedef UInt8 GameObjectState;

	private:
		GD_SERIALIZABLE_DEFINITION(GameObject, Component, GDAPI);

		GD_SERIALIZATION_BEGIN(GameObject, Component);
			GD_FIELD(String, name);
			GD_FIELD(String, tag);
		GD_SERIALIZATION_END();

		GameObjectState state;

	public:
		enum : GameObjectState
		{
			StateInitializing,	///< This game object is initializing
			StateDeleting,		///< This game object is going to be deleted 
			StateActive,		///< This game object is active and takes a part in update loop
			StateVisible,		///< This game object is visible by camera that is currently rendering 
		};

		/// @brief			Constructs new game object
		GDINT GameObject();

		GDINT virtual ~GameObject();

		/// @name Getters / Setters
		/// @{

		/// @brief			Gets 'GameObject'`s name
		///	@returns		'GameObject'`s name
		GDINL const String&	GetName() const;

		/// @brief			Renames 'GameObject'
		/// @param name		'GameObject'`s new name
		GDINL void SetName(const String& name);

		/// @brief			Gets 'GameObject'`s tag
		///	@returns		'GameObject'`s tag
		GDINL const String&	GetTag() const;

		/// @brief			Sets 'GameObjects'`s tag
		/// @param tag		'GameObjects'`s new tag
		GDINL void SetTag(const String& tag);

		/// @brief				Disables 'GameObject' and all 'Component's attached to it
		/// @param enabled		Enable or Disable
		/// @see Component
		GDINL void Enable(bool enabled);

		/// @}

		/// @name Component Management
		/// @{

		/// @brief			Searches for attached components with specified type information
		/// @param typeinfo Type information for required component type
		/// @returns		Component attached to this object of this type or nullptr if nothing found
		GDAPI Component* GetComponent(TypeInformation const* typeinfo) const;

		/// @see Component::GetComponent
		template<class ComponentType>
		GDINL ComponentType* GetComponent() const;

		/// @brief			Adds component with specified type information.
		/// @param typeinfo Type information for required component type
		/// @returns		Newly created component of this type or existing
		GDAPI Component* AddComponent(TypeInformation const* typeinfo);

		/// @see Component::AddComponent
		template<class ComponentType>
		GDINL ComponentType* AddComponent();

		/// @brief			Removes component with specified type information (if exists in game object).
		/// @param typeinfo Type information for required component type
		GDAPI void RemoveComponent(TypeInformation const* typeinfo);		

		/// @see Component::RemoveComponent
		template<class ComponentType>
		GDINL void RemoveComponent();

		/// @}

	private:
		GDINL virtual void OnInitializeSelf() 
		{ 
			for (Component* 
				component = ((Component*)self->GetLastChildObject()); component != nullptr; 
				component = ((Component*)component->GetPreviousSiblingObject()))
			{
				component->OnInitializeSelf();
				// component->InvokeEmited();
			}
		}

		GDINL virtual void OnStartSelf()	
		{ 
			for (Component* 
				component = ((Component*)self->GetLastChildObject()); component != nullptr; 
				component = ((Component*)component->GetPreviousSiblingObject()))
			{
				component->OnStartSelf();
				// component->InvokeEmited();
			}
		}

		GDINL virtual void OnUpdateSelf()	
		{ 
			for (Component* 
				component = ((Component*)self->GetLastChildObject()); component != nullptr; 
				component = ((Component*)component->GetPreviousSiblingObject()))
			{
				component->OnUpdateSelf();
				// component->InvokeEmited();
			}
		}

		GDINL virtual void OnRenderSelf(
			_In_ Camera const* const camera
		)
		{ 
			for (Component* 
				component = ((Component*)self->GetLastChildObject()); component != nullptr; 
				component = ((Component*)component->GetPreviousSiblingObject()))
			{
				component->OnRenderSelf(camera);
				// component->InvokeEmited();
			}
		}
	};
	

GD_NAMESPACE_END

#include <GoddamnEngine/Engine/Component/GameObject/GameObject.inl>

#endif
