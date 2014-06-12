#ifndef GD_ENGINE_COMPONENT_COMPONENT
#define GD_ENGINE_COMPONENT_COMPONENT

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/Events/Event.hh>
#include <GoddamnEngine/Core/Object/Object.hh>

/// Legacy macroes
#define Serializable Object
#define GD_SERIALIZATION_BEGIN(...)
#define GD_SERIALIZATION_END(...)
#define GD_EXTENDS_SERIALIZABLE_LIST(Serializable, ...) Serializable(__VA_ARGS__)
#define GD_EXTENDS_SERIALIZABLE(Serializable) GD_EXTENDS_SERIALIZABLE_LIST(Serializable)
#define GD_FGDLD(Type, Name) Type Name

#define GD_SERIALIZABLE_DEFINITION			GD_TYPEINFORMATION_DEFINITION
#define GD_SERIALIZABLE_IMPLEMENTATION		GD_TYPEINFORMATION_IMPLEMENTATION
#define GD_SERIALIZABLE_IMPLEMENTATION_C	GD_TYPEINFORMATION_IMPLEMENTATION_C

GD_NAMESPACE_BEGIN

	class Component;
	class Transform;
	class Camera;
	class GameObject;

	enum ComponentState : UInt8
	{
		GD_COMPONENT_STATE_CREATED,
		GD_COMPONENT_STATE_STARTED,
		GD_COMPONENT_STATE_UPDATED
	};

	typedef UInt32 ComponentAbilitiesFlags;
	enum ComponentAbilitiesFlagsEnumeration : ComponentAbilitiesFlags
	{
		GD_COMPONENT_ABILITGDS_DO_RENDER				= GD_BIT(0),	///< This component is renderable
		GD_COMPONENT_ABILITGDS_DO_RENDER_IF_INVISIBLE	= GD_BIT(1),	///< This component is rendered even if it is invisible
		GD_COMPONENT_ABILITGDS_DO_UPDATE_IF_INVISIBLE	= GD_BIT(2),	///< This component is not updated when is invisible
		GD_COMPONENT_ABILITGDS_DO_DESTROY_IF_INVISIBLE	= GD_BIT(3),	///< This component is destroyed when is invisible
		GD_COMPONENT_ABILITGDS_DO_DISABLE_IF_INVISIBLE	= GD_BIT(3),	///< This component is disabled when becomes invisible
		GD_COMPONENT_ABILITGDS_DO_ENABLE_IF_INVISIBLE	= GD_BIT(4),	///< This component is enabled when becomes visible
		GD_COMPONENT_ABILITGDS_UNKNOWN					= (0),
	};

	/// Component class.
	class Component : public Serializable
	{
		friend class GameObject;

	private:
		GD_SERIALIZABLE_DEFINITION(Component, Serializable, GDAPI);

		GD_SERIALIZATION_BEGIN(Component, Serializable);
			GD_FGDLD(bool, enabled);
		GD_SERIALIZATION_END();

		Transform* transform;	

	public:
		ComponentAbilitiesFlags const AbilitiesFlags;		///< Flags describing abilities of component

	protected:
		GDAPI Component(ComponentAbilitiesFlags const abilitiesFlags = GD_COMPONENT_ABILITGDS_UNKNOWN,
			ObjectTreeLockingFlags const treeLockingFlags = Object::TreeLockingFlagsDefaultComponent);
		
		GDAPI virtual ~Component();

	public:
		/// Returns boolean for this component is enabled.
		/// If component is disabled it does not updates
		GDINL bool IsEnabled() const;

		/// Enables / disables this component.
		GDINL void Enable(bool const enabled = true); 

		/// Returns GameObject this Component attached to
		GDINL GameObject* GetGameObject() const;

		/// Returns Transform of the game object this component attached to
		GDINL Transform* GetTransform() const;

	protected:

		/// Method being called after component was constructed, 
		/// At this moment we can obtain game object this component is attached to and work with it.
		/// @note Base classes`s method should be called
		GDINT virtual void OnInitializeSelf() { }

		/// Method being called before destructor is called.
		///	If is normal destruction, at this moment no components are removed from this game object,
		/// otherwise this is force destruction. 
		/// @note Base classes`s method should be called
		GDINT virtual void OnDestroySelf(bool const isForceDestruction) { }

		/// Method being called after before component`s first update
		/// @note Base classes`s method should be called
		GDINT virtual void OnStartSelf() { }

		/// Method being called each frame.
		/// @note Base classes`s method should be called
		GDINT virtual void OnUpdateSelf() { }

		/// Method each time engine renders this component.
		/// @note Base classes`s method should be called
		GDINT virtual void OnRenderSelf(
			_In_ Camera const* const camera
		) { }
	};

GD_NAMESPACE_END

#include <GoddamnEngine/Engine/Component/Component.inl>

#endif
