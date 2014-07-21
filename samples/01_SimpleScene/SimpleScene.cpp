/// ==========================================================================================
/// SimpleScene.cpp - Goddamn Hello World Example.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.--.2012 - Created by James Jhuighuy
/// ==========================================================================================

// Icluding common engine stuff.
#include <GoddamnEngine/Engine/Engine.h>
#include <GoddamnEngine/Engine/Application/Application.h>
#include <GoddamnEngine/Engine/Component/GameObject/GameObject.h>
#include <GoddamnEngine/Engine/Component/Transform/Transform.h>
#include <GoddamnEngine/Engine/Component/MeshRenderer/MeshRenderer.h>
#include <GoddamnEngine/Engine/Component/Static/Input/Input.h>
#include <GoddamnEngine/Engine/Component/Static/Physics/Physics.h>

using namespace GD_NAMESPACE;

class TestController final : public Component
{
private:
	GD_SERIALIZABLE_DEFINITION(TestController, Component, GDINT);

	GD_SERIALIZATION_BEGIN(TestController, Component);
	GD_FIELD(float, s);
	//GD_FIELD(Light*, l);
	//GD_FIELD_ARRAY(String, j);
	GD_SERIALIZATION_END();

	bool b;

public:
	TestController() :
		GD_EXTENDS_SERIALIZABLE(Component)
	{
		s = 0.02f;
		b = true;
	}

	virtual void OnInitializeSelf()
	{
	}

	virtual void OnUpdateSelf()
	{
		if (Input::IsKeyDown(KeyCode::R))
		{
			b = !b;
		}

		Transform* t = (self)->GetGameObject()->GetTransform();

		if (Input::IsKeyDown(KeyCode::W))
		{
			t->Translate(Vector3Fast(0, 0, s));
		}
		if (Input::IsKeyDown(KeyCode::S))
		{
			t->Translate(Vector3Fast(0, 0, -s));
		}
		if (Input::IsKeyDown(KeyCode::A))
		{
			t->Translate(Vector3Fast(-s, 0, 0));
		}
		if (Input::IsKeyDown(KeyCode::D))
		{
			t->Translate(Vector3Fast(+s, 0, 0));
		}

		/**/ if (Input::IsKeyDown(KeyCode::Q))
		{
			t->Rotate(Vector3Fast(0, -s * 5, 0));
			auto a = t->GetRotation();
			int i = 0;
		}
		else if (Input::IsKeyDown(KeyCode::E))
		{
			t->Rotate(Vector3Fast(0, s * 5, 0));
		}
	}
};

class DebugRuntime final : public Application
{
public:
	GDINT DebugRuntime(int const argumentsCount, char const* const* const argumentsList) :
		Application(argumentsCount, argumentsList)
	{}

	GDINT virtual void OnInitialize();

	GDINT virtual String GetApplicationName() const { return "Debug Runtime"; }
	GDINT virtual String GetApplicationVersion() const { return "v5.0.0.1"; }
};

void DebugRuntime::OnInitialize()
{
	Scene* const scene = new Scene(Scene::FlagsNone);

	GameObject* object0 = scene->CreateGameObject();
	GameObject* object1 = scene->CreateGameObject();
	GameObject* object2 = scene->CreateGameObject();
	GameObject* object3 = scene->CreateGameObject();

	//Object0
	auto 
		transform0 = object0->AddComponent<Transform>();
		transform0->SetPosition(Vector3Fast(0.0f, 1.75f, -7.0f));

	auto camera0 = object0->AddComponent<Camera>();
	auto testController0 = object0->AddComponent<TestController>();
	//auto rigidbody0 = object0->AddComponent<Rigidbody>();

	/*auto
		rigidbody0 = object0->AddComponent<Rigidbody>();
		rigidbody0->SetRigidbodyType(RigidbodyType::Character);
		rigidbody0->SetColliderShape(GD_LOAD_RESOURCE("file://..\\Resources/labs_mesh", StaticMesh));*/

	//Object1
	auto 
		transform1 = object1->AddComponent<Transform>();
		transform1->SetScale(Vector3Fast(25.0f));
		transform1->SetPosition(Vector3Fast(0.0f));

	auto 
		meshRenderer1 = object1->AddComponent<MeshRenderer>();
		meshRenderer1->SetStaticMesh(GD_LOAD_RESOURCE(R"(file://D:\Engine\IncredibleEngine\resources\labs_mesh)", StaticMesh));
		meshRenderer1->SetMaterial(GD_LOAD_RESOURCE(R"(file://D:\Engine\IncredibleEngine\bin.x64\test_material.xml)", Material));
		//auto rigidbody1 = object1->AddComponent<MeshCollider>();

	//auto
	//	meshCollider1 = object1->AddComponent<MeshCollider>();
	//	//meshCollider1->SwitchToTrigger();
	//	object1->AddComponent<TriggerTest>();

	//Object2
	auto 
		transform2 = object2->AddComponent<Transform>();
		transform2->SetPosition(Vector3Fast(0.0f, 1.75f, -2.0f));
		transform2->SetScale(Vector3Fast(0.25f));
		transform2->SetRotation(Vector3Fast(-90.0f, 0.0f, 0.0f));

		/*auto 
		light2 = object2->AddComponent<Light>();
		light2->SetColor(Color(1.0f,  1.0f, 1.0f, 1.0f));
		light2->SetIntensivity(1.0f);*/

	/*auto 
		meshRenderer2 = object2->AddComponent<MeshRenderer>();
		meshRenderer2->SetStaticMesh(GD_LOAD_RESOURCE("file://..\\Resources//sphere.shk", StaticMesh));
		meshRenderer2->SetMaterial(GD_LOAD_RESOURCE("file://..\\Resources/labs_material2",	Material));*/
			
	/*auto
		boxCollider2 = object2->AddComponent<SphereCollider>();*/
		
		/*auto
		rigidbody2 = object2->AddComponent<Rigidbody>();
		rigidbody2->Enable(false);*/

	auto 
		transform3 = object3->AddComponent<Transform>();
		transform3->SetPosition(Vector3Fast(0.0f, 0.5f, -3.0f));
}

GD_SERIALIZABLE_IMPLEMENTATION(TestController, Component, GDINT);

#include <time.h>

int main(
	_In_ int const ArgumentsCount, 
	_In_ char const* const* const ArgumentsList
)
{
	time_t const StartTime = clock();

	/*{
		Vector3Fast Vec3(1.0f, 20.0f, 300.0f);
		for (size_t cnt = 0; cnt < USHRT_MAX; cnt += 1)
		{
			Vec3 *= Vector3Fast(1.0f, 20.0f, 300.0f);
			Vec3 = Vec3.Cross(Vector3Fast(Vec3.Dot(Vec3--)));
			Vec3 /= Vec3.Normalize()++;
		}
	}	time_t const SSETime = clock() - StartTime;*/

	/*{
		Vector3 Vec3(1.0f, 20.0f, 300.0f);
		for (size_t cnt = 0; cnt < USHRT_MAX; cnt += 1)
		{
			Vec3 *= Vector3(1.0f, 20.0f, 300.0f);
			Vec3 = Vec3.Cross(Vector3(Vec3.Dot(Vec3--)));
			Vec3 /= Vec3.Normalize()++;
		}
	}	time_t const NOSSETime = clock() - SSETime;*/

	/*{
		Matrix4x4 Matrix;
		for (size_t cnt = 0; cnt < USHRT_MAX; cnt += 1)
		{
			Matrix4x4 A = Matrix4x4().Identity(3.0f);
			A *= Matrix4x4().PerspectiveLh(60.0f, 16.0f/9.0f, 0.1f, 3000.0f);
			A.Inverse().Transpose().Inverse().Rotate(Quaternion().SetEulerDegrees(Vector3Fast(30.0f, 60.0f, 90.0f)));
		}
	} time_t const SSETime = clock() - StartTime;*/

    DebugRuntime DebugRuntimeInstance(ArgumentsCount, ArgumentsList);
    Application::Execute(DebugRuntimeInstance);
}
