#ifndef GD_CORE_COMPONENTS_STATIC_PHYSICS_INTERFACE
#define GD_CORE_COMPONENTS_STATIC_PHYSICS_INTERFACE

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Component/Static/StaticComponent.h>
#include <GoddamnEngine/Engine/Component/Transform/Transform.h>

#if (!defined(GD_DOCUMENTATION))
namespace physx
{
	class PxFoundation;
	class PxPhysics;
	class PxScene;
	class PxFoundation;
	class PxDefaultErrorCallback;
	class PxDefaultAllocator;
	class PxTransform;
	class PxActor;
	class PxShape;
	class PxRigidActor;
	class PxRigidDynamic;
	class PxMaterial;
	class PxCooking;
#	if (defined(GD_DEBUG))
	namespace debugger { namespace comm { class PvdConnection; } }
#	endif	// if (defined(GD_DEBUG))
}		// namespace physx
#endif	// if (!defined(GD_DOCUMENTATION))

GD_NAMESPACE_BEGIN

	/// ==========================================================================================
	/// ==========================================================================================
	class PhysicsInterface : public StaticComponent,
							 public Singleton<PhysicsInterface>
	{
		friend class MeshCollider;

	private:
		GD_TYPEINFORMATION_DEFINITION(PhysicsInterface, StaticComponent, GDAPI);
		
		physx::PxFoundation* Foundation = nullptr;
		physx::PxPhysics* PhysicsSdk = nullptr;
		physx::PxScene* Scene = nullptr;
		physx::PxCooking* Chief = nullptr;
		physx::PxMaterial* DefaultMaterial = nullptr;
		physx::PxDefaultAllocator* DefaultCallbackAllocator = nullptr;
		physx::PxDefaultErrorCallback* DefaultCallbackError = nullptr;
#if (defined(GD_DEBUG))
		physx::debugger::comm::PvdConnection* VisualDebugger = nullptr;
#endif	// if (defined(GD_DEBUG))

	protected:
		GDINT          PhysicsInterface();
		GDINT virtual ~PhysicsInterface();

		//GDINT virtual void OnInitializeSelf();
		GDINT virtual void OnUpdateSelf();
	};

	/// ==========================================================================================
	/// ==========================================================================================
	class MeshCollider : public Component
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(MeshCollider, Component, GDAPI);

		physx::PxTransform*	 RigidbodyTransform;
		physx::PxRigidActor* RigidbodyPtr = nullptr;

	public:
		GDINT MeshCollider() { }
		GDINT virtual ~MeshCollider() { }

	protected:
		GDINT virtual void OnInitializeSelf();
		GDINT virtual void OnDestroySelf(bool const isForceDestruction);
	};

	/// ==========================================================================================
	/// ==========================================================================================
	class Rigidbody : public Component,
					  public IOnTransformedListener
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(Rigidbody, Component, GDAPI);

		physx::PxTransform*	 RigidbodyTransform;
		physx::PxRigidDynamic* RigidbodyPtr = nullptr;

	public:
		GDINT Rigidbody() { }
		GDINT virtual ~Rigidbody() { }

	protected:
		GDINT virtual void OnUpdateSelf();
		GDINT virtual void OnInitializeSelf();
		GDINT virtual void OnDestroySelf(bool const isForceDestruction);
		GDINT virtual void OnTransformed(Component* const transformer);
	};

GD_NAMESPACE_END

#include <GoddamnEngine/Engine/Component/Static/Physics/Physics.inl>

#endif