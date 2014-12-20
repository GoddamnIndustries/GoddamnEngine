#include <GoddamnEngine/Engine/Component/Static/Physics/Physics.h>
#include <GoddamnEngine/Engine/Component/Impl/MeshRenderer/MeshRenderer.h>

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
		this->Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, 
			*(this->DefaultCallbackAllocator = new PxDefaultAllocator    ),
			*(this->DefaultCallbackError     = new PxDefaultErrorCallback)
		);

		this->PhysicsSdk = PxCreatePhysics(PX_PHYSICS_VERSION, *this->Foundation, PxTolerancesScale());
		this->Chief      = PxCreateCooking(PX_PHYSICS_VERSION, *this->Foundation, PxCookingParams(this->PhysicsSdk->getTolerancesScale()));
		
		PxInitExtensions(*this->PhysicsSdk);

		PxSceneDesc SceneDesc(this->PhysicsSdk->getTolerancesScale());
		SceneDesc.gravity		= PxVec3(0.0f, -9.81f, 0.0f);
		SceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
		SceneDesc.filterShader	= PxDefaultSimulationFilterShader;
		this->Scene = this->PhysicsSdk->createScene(SceneDesc);
		this->DefaultMaterial = this->PhysicsSdk->createMaterial(0.5f, 0.5f, 0.0f);

#if (defined(GD_DEBUG))
		if (this->PhysicsSdk->getPvdConnectionManager() != nullptr)
		{
			char const* const VisualDebuggeRHost = "127.0.0.1";
			int const VisualDebuggerPort = 5425;
			int const VisualDebuggerTimeout = 100;
			this->VisualDebugger = PxVisualDebuggerExt::createConnection(
				this->PhysicsSdk->getPvdConnectionManager(),
				VisualDebuggeRHost, VisualDebuggerPort, VisualDebuggerTimeout,
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
		if (this->VisualDebugger != nullptr)
			this->VisualDebugger->release();
#endif	// if (defined(GD_DEBUG))

		this->Scene->release();
		this->PhysicsSdk->release();
		this->Foundation->release();

		delete this->DefaultCallbackAllocator;
		delete this->DefaultCallbackError;
	}

	/// ==========================================================================================
	/// ==========================================================================================
	void PhysicsInterface::OnUpdateSelf()
	{
		this->Scene->simulate(1 / 250.0f);
		while (!this->Scene->fetchResults());
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

		MeshRenderer* const meshRenderer = this->GetGameObject()->GetComponent<MeshRenderer>();
		GD_ASSERT((meshRenderer != nullptr), "");
		HRIIndexedShape const* const meshShape = meshRenderer->GetStaticMesh();

		PxTriangleMeshDesc TriangleMeshDesc;
		auto const& meshShapeIndicesData  = meshShape->GetIndicesData();
		auto const& meshShapeVerticesData = meshShape->GetVerticesDataBySemantic(GD_HRI_SEMANTIC_POSITION);

		TriangleMeshDesc.triangles.count  = static_cast<PxU32>(meshShapeIndicesData.GetLength()) / 3;
		TriangleMeshDesc.points.count	  = static_cast<PxU32>(meshShapeVerticesData.GetLength() / 3);
		TriangleMeshDesc.triangles.stride = static_cast<PxU32>(3 * sizeof(meshShapeIndicesData[0]));
		TriangleMeshDesc.points.stride	  = static_cast<PxU32>(3 * sizeof(meshShapeVerticesData[0]));
		TriangleMeshDesc.triangles.data   = &meshShapeIndicesData[0];
		TriangleMeshDesc.points.data	  = &meshShapeVerticesData[0];
		TriangleMeshDesc.flags			  = PxMeshFlag::e16_BIT_INDICES;

		PxDefaultMemoryOutputStream TriangleMeshCooked;
		GD_ASSERT((TriangleMeshDesc.isValid()), "PhysX(R) Mesh check failed");
		GD_ASSERT((PhysicsInterface::GetInstance().Chief->cookTriangleMesh(TriangleMeshDesc, TriangleMeshCooked)),
					"PhysX(R) mesh cooking failed");

		PxDefaultMemoryInputData TriangleMeshData(TriangleMeshCooked.getData(), TriangleMeshCooked.GetLength());
		PxTriangleMesh* const TriangleMesh = PhysicsInterface::GetInstance().PhysicsSdk->createTriangleMesh(TriangleMeshData);

		this->RigidbodyPtr = PhysicsInterface::GetInstance().PhysicsSdk->createRigidStatic(PxTransform(PxIdentity));
		this->RigidbodyPtr->createShape(PxTriangleMeshGeometry(TriangleMesh, PxMeshScale(25.0f)), *PhysicsInterface::GetInstance().DefaultMaterial);
		PhysicsInterface::GetInstance().Scene->addActor(*this->RigidbodyPtr);
	}

	/// ==========================================================================================
	/// ==========================================================================================
	void MeshCollider::OnDestroySelf(bool const isForceDestruction)
	{
		this->RigidbodyPtr->release();
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

		this->GetGameObject()->GetTransform()->OnTransfromedEvent += this;
		this->RigidbodyPtr = PhysicsInterface::GetInstance().PhysicsSdk->createRigidDynamic(PxTransform(PxIdentity));
		this->RigidbodyPtr->createShape(PxBoxGeometry(0.5f, 0.9f, 0.5f), *PhysicsInterface::GetInstance().DefaultMaterial);
		PhysicsInterface::GetInstance().Scene->addActor(*this->RigidbodyPtr);
	}

	/// ==========================================================================================
	/// ==========================================================================================
	void Rigidbody::OnUpdateSelf()
	{
		using namespace physx;
		PxTransform const& RigidbodyTransform = this->RigidbodyPtr->getGlobalPose();
		this->GetGameObject()->GetTransform()->SetPosition(Vector3(RigidbodyTransform.p.x, RigidbodyTransform.p.y, RigidbodyTransform.p.z));
		this->GetGameObject()->GetTransform()->SetRotation(Quaternion(RigidbodyTransform.q.x, RigidbodyTransform.q.y, RigidbodyTransform.q.z, RigidbodyTransform.q.w));
	}

	/// ==========================================================================================
	/// ==========================================================================================
	void Rigidbody::OnDestroySelf(bool const isForceDestruction)
	{
		this->GetGameObject()->GetTransform()->OnTransfromedEvent -= this;
		this->RigidbodyPtr->release();
	}

	/// ==========================================================================================
	/// ==========================================================================================
	void Rigidbody::OnTransformed(Component* const transformer)
	{
		using namespace physx;
		PxTransform RigidbodyTransform = PxTransform(PxIdentity);
		memcpy(&RigidbodyTransform.p, &this->GetGameObject()->GetTransform()->GetPosition(), sizeof(RigidbodyTransform.p));
		memcpy(&RigidbodyTransform.q, &this->GetGameObject()->GetTransform()->GetRotation(), sizeof(RigidbodyTransform.q));
		this->RigidbodyPtr->setGlobalPose(RigidbodyTransform);
	}


GD_NAMESPACE_END

#endif