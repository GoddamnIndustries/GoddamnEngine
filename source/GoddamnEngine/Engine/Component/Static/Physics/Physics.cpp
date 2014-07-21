#include <GoddamnEngine/Engine/Component/Static/Physics/Physics.h>
#include <GoddamnEngine/Engine/Component/MeshRenderer/MeshRenderer.h>

#if 0

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>
#include <PxToolkit.h>
#include <PxRigidDynamic.h>
#include <PxScene.h>
#include <PxShape.h>

GD_NAMESPACE_BEGIN
	
	/// ==========================================================================================
	/// PhysicsInterface class
	/// ==========================================================================================

	GD_TYPEINFORMATION_IMPLEMENTATION(PhysicsInterface, StaticComponent, GDAPI);
	GD_SINGLETON_IMPLEMENTATION      (PhysicsInterface                         );

	PhysicsInterface::PhysicsInterface() : StaticComponent(StaticComponentPriority::Low)
	{
		using namespace physx;
		self->Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, 
			*(self->DefaultCallbackAllocator = new PxDefaultAllocator    ),
			*(self->DefaultCallbackError     = new PxDefaultErrorCallback)
		);

		self->PhysicsSdk = PxCreatePhysics(PX_PHYSICS_VERSION, *self->Foundation, PxTolerancesScale());
		self->Chief      = PxCreateCooking(PX_PHYSICS_VERSION, *self->Foundation, PxCookingParams(self->PhysicsSdk->getTolerancesScale()));
		
		PxInitExtensions(*self->PhysicsSdk);

		PxSceneDesc SceneDesc(self->PhysicsSdk->getTolerancesScale());
		SceneDesc.gravity		= PxVec3(0.0f, -9.81f, 0.0f);
		SceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
		SceneDesc.filterShader	= PxDefaultSimulationFilterShader;
		self->Scene = self->PhysicsSdk->createScene(SceneDesc);
		self->DefaultMaterial = self->PhysicsSdk->createMaterial(0.5f, 0.5f, 0.0f);

#if (defined(GD_DEBUG))
		if (self->PhysicsSdk->getPvdConnectionManager() != nullptr)
		{
			char const* const VisualDebuggerHost = "127.0.0.1";
			int const VisualDebuggerPort = 5425;
			int const VisualDebuggerTimeout = 100;
			self->VisualDebugger = PxVisualDebuggerExt::createConnection(
				self->PhysicsSdk->getPvdConnectionManager(),
				VisualDebuggerHost, VisualDebuggerPort, VisualDebuggerTimeout,
				PxVisualDebuggerExt::getAllConnectionFlags()
			);
		}
#endif	// if (defined(GD_DEBUG))
	}

	/// ==========================================================================================
	/// ==========================================================================================
	PhysicsInterface::~PhysicsInterface()
	{
#if (defined(GD_DEBUG))
		if (self->VisualDebugger != nullptr)
			self->VisualDebugger->release();
#endif	// if (defined(GD_DEBUG))

		self->Scene->release();
		self->PhysicsSdk->release();
		self->Foundation->release();

		delete self->DefaultCallbackAllocator;
		delete self->DefaultCallbackError;
	}

	/// ==========================================================================================
	/// ==========================================================================================
	void PhysicsInterface::OnUpdateSelf()
	{
		self->Scene->simulate(1 / 250.0f);
		while (!self->Scene->fetchResults());
	}

	/// ==========================================================================================
	/// ==========================================================================================
	GD_TYPEINFORMATION_IMPLEMENTATION(MeshCollider, Component, GDAPI);

	/// ==========================================================================================
	/// ==========================================================================================
	void MeshCollider::OnInitializeSelf()
	{
		using namespace physx;
		using namespace PxToolkit;

		MeshRenderer* const meshRenderer = self->GetGameObject()->GetComponent<MeshRenderer>();
		GD_ASSERT((meshRenderer != nullptr), "");
		HRIIndexedShape const* const meshShape = meshRenderer->GetStaticMesh();

		PxTriangleMeshDesc TriangleMeshDesc;
		auto const& meshShapeIndicesData  = meshShape->GetIndicesData();
		auto const& meshShapeVerticesData = meshShape->GetVerticesDataBySemantic(GD_HRI_SEMANTIC_POSITION);

		TriangleMeshDesc.triangles.count  = static_cast<PxU32>(meshShapeIndicesData.GetSize()) / 3;
		TriangleMeshDesc.points.count	  = static_cast<PxU32>(meshShapeVerticesData.GetSize() / 3);
		TriangleMeshDesc.triangles.stride = static_cast<PxU32>(3 * sizeof(meshShapeIndicesData[0]));
		TriangleMeshDesc.points.stride	  = static_cast<PxU32>(3 * sizeof(meshShapeVerticesData[0]));
		TriangleMeshDesc.triangles.data   = &meshShapeIndicesData[0];
		TriangleMeshDesc.points.data	  = &meshShapeVerticesData[0];
		TriangleMeshDesc.flags			  = PxMeshFlag::e16_BIT_INDICES;

		PxDefaultMemoryOutputStream TriangleMeshCooked;
		GD_ASSERT((TriangleMeshDesc.isValid()), "PhysX(R) Mesh check failed");
		GD_ASSERT((PhysicsInterface::GetInstance().Chief->cookTriangleMesh(TriangleMeshDesc, TriangleMeshCooked)),
					"PhysX(R) mesh cooking failed");

		PxDefaultMemoryInputData TriangleMeshData(TriangleMeshCooked.getData(), TriangleMeshCooked.getSize());
		PxTriangleMesh* const TriangleMesh = PhysicsInterface::GetInstance().PhysicsSdk->createTriangleMesh(TriangleMeshData);

		self->RigidbodyPtr = PhysicsInterface::GetInstance().PhysicsSdk->createRigidStatic(PxTransform(PxIdentity));
		self->RigidbodyPtr->createShape(PxTriangleMeshGeometry(TriangleMesh, PxMeshScale(25.0f)), *PhysicsInterface::GetInstance().DefaultMaterial);
		PhysicsInterface::GetInstance().Scene->addActor(*self->RigidbodyPtr);
	}

	/// ==========================================================================================
	/// ==========================================================================================
	void MeshCollider::OnDestroySelf(bool const isForceDestruction)
	{
		self->RigidbodyPtr->release();
	}

	/// ==========================================================================================
	/// ==========================================================================================
	GD_TYPEINFORMATION_IMPLEMENTATION(Rigidbody, Component, GDAPI);

	/// ==========================================================================================
	/// ==========================================================================================
	void Rigidbody::OnInitializeSelf()
	{
		using namespace physx;
		using namespace PxToolkit;

		self->GetGameObject()->GetTransform()->OnTransfromedEvent += self;
		self->RigidbodyPtr = PhysicsInterface::GetInstance().PhysicsSdk->createRigidDynamic(PxTransform(PxIdentity));
		self->RigidbodyPtr->createShape(PxBoxGeometry(0.5f, 0.9f, 0.5f), *PhysicsInterface::GetInstance().DefaultMaterial);
		PhysicsInterface::GetInstance().Scene->addActor(*self->RigidbodyPtr);
	}

	/// ==========================================================================================
	/// ==========================================================================================
	void Rigidbody::OnUpdateSelf()
	{
		using namespace physx;
		PxTransform const& RigidbodyTransform = self->RigidbodyPtr->getGlobalPose();
		self->GetGameObject()->GetTransform()->SetPosition(Vector3(RigidbodyTransform.p.x, RigidbodyTransform.p.y, RigidbodyTransform.p.z));
		self->GetGameObject()->GetTransform()->SetRotation(Quaternion(RigidbodyTransform.q.x, RigidbodyTransform.q.y, RigidbodyTransform.q.z, RigidbodyTransform.q.w));
	}

	/// ==========================================================================================
	/// ==========================================================================================
	void Rigidbody::OnDestroySelf(bool const isForceDestruction)
	{
		self->GetGameObject()->GetTransform()->OnTransfromedEvent -= self;
		self->RigidbodyPtr->release();
	}

	/// ==========================================================================================
	/// ==========================================================================================
	void Rigidbody::OnTransformed(Component* const transformer)
	{
		using namespace physx;
		PxTransform RigidbodyTransform = PxTransform(PxIdentity);
		memcpy(&RigidbodyTransform.p, &self->GetGameObject()->GetTransform()->GetPosition(), sizeof(RigidbodyTransform.p));
		memcpy(&RigidbodyTransform.q, &self->GetGameObject()->GetTransform()->GetRotation(), sizeof(RigidbodyTransform.q));
		self->RigidbodyPtr->setGlobalPose(RigidbodyTransform);
	}


GD_NAMESPACE_END

#endif