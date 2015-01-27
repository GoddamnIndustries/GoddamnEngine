/// ==========================================================================================
/// SimpleScene.cpp - Goddamn Hello World Example.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.--.2012 - Originally written by James Jhuighuy.
///		* Today      - Created by You :)
/// ==========================================================================================

#include <GoddamnEngine/Engine/Application/Application.h>
#include <GoddamnEngine/Engine/Component/Static/Input/Input.h>			// To handle input.
#include <GoddamnEngine/Engine/Component/Static/Physics/Physics.h>		// To create collisions.
#include <GoddamnEngine/Engine/Component/Impl/Transform/Transform.h>			// To position objects in the world.
#include <GoddamnEngine/Engine/Component/Impl/MeshRenderer/MeshRenderer.h>	// To render meshes.

GD_USING_NAMESPACE;

/// Defines application runtime class.
class SimpleSceneSampleApp final : public Application
{
public:
	GDINL          SimpleSceneSampleApp(int const CMDArgsCount, char const* const* const CMDArgsList) : Application(CMDArgsCount, CMDArgsList) { }
	GDINL virtual ~SimpleSceneSampleApp() { }

	GDINT virtual void OnInitialize() final override;

	GDINL virtual String const& GetApplicationName   () const final override { String static const ApplicationName("GoddamnEngine Sample 01 - Simple Scene"); return ApplicationName; }
	GDINL virtual String const& GetApplicationVersion() const final override { String static const ApplicationVersion("v1.0.0.1");                            return ApplicationVersion; }
};	// class SimpleSceneSampleApp

/// Represents a simple DooM-like FPS controller.
class SimpleSceneFPSController final : public Component
{
	GD_CLASSINFO_DEFINITION(SimpleSceneFPSController, Component, GDINT);

	/// Player's moving speed.
	GD_DEFINE_FIELD(Float32, MovingSpeed) = 2.6f;

	/// Player's rotation speed.
	GD_DEFINE_FIELD(Float32, RotationSpeed) = 2.6f;

public/*Class API*/:
	GDINL SimpleSceneFPSController() { }
	GDINL virtual ~SimpleSceneFPSController() { }

	/// Is invoked each frame engine renders.
	virtual void OnUpdateSelf() final override;
};	// class SimpleSceneFPSController

GD_CLASSINFO_IMPLEMENTATION(SimpleSceneFPSController, Component, GDINT);

String MyFunction(String const& A, String const& B, size_t const C)
{
	String Result = A;
	for (size_t I = 0; I < C; ++I) {
		Result += B;
	}

	return Result + A;
}

/// @brief Sample enum.
GD_ENUMINFO_DEFINITION_BEGIN(MyEnum, UInt32, GDINT)
	GD_DEFINE_ENUM_VALUE_AUTO(Zero)
	GD_DEFINE_ENUM_VALUE(A, 123)
	GD_DEFINE_ENUM_VALUE_AUTO(Auto)
	GD_DEFINE_ENUM_VALUE(B, 1234)
GD_ENUMINFO_DEFINITION_END();

GD_ENUMINFO_IMPLEMENTATION(MyEnum, UInt32, GDINT);

//struct MyStruct 
//{
//	Float32 First;
//	UInt32 Second;
//	String Third;
//};	// struct MyStruct
//
//GD_STRUCTINFO_DEFINITION_EXTERNAL(MyStruct, GDINT);
//
//GD_STRUCTINFO_IMPLEMENTATION_EXTERNAL(MyStruct, GDINT);
////GD_IMPLEMENT_FIELD(First);
////GD_IMPLEMENT_FIELD(Second);
////GD_IMPLEMENT_FIELD(Third);

void SimpleSceneSampleApp::OnInitialize()
{	
	auto a = MyEnum::GetEnumInfo();

	// Initializing Application.
	this->Application::OnInitialize();

	// Creating a scene.
	/*RefPtr<Scene>*/Scene* const CurrentScene = new Scene(Scene::FlagsNone);
	{	// Creating a Player with Transform, camera and FPS controller and collider.
		RefPtr<GameObject> const PlayerObject = CurrentScene->CreateGameObject();
		RefPtr<Transform> const PlayerTransform(PlayerObject->AddComponent<Transform>());
		PlayerTransform->SetPosition(Vector3Fast(0.0f, 1.75f, -7.0f));
		PlayerTransform->SetRotation(Vector3Fast(0.0f, 180.0f, 0.0f));

		RefPtr<Camera> const PlayerCamera = PlayerObject->AddComponent<Camera>();
		RefPtr<SimpleSceneFPSController> const PlayerFPSController = PlayerObject->AddComponent<SimpleSceneFPSController>();
		
	//	RefPtr<Rigidbody> const PlayerRigidbody = PlayerObject->AddComponent<Rigidbody>();
	//	PlayerRigidbody->SetRigidbodyType(RigidbodyType::Character);
	//	PlayerRigidbody->SetColliderShape(GD_LOAD_RESOURCE("file://..\\Resources/labs_mesh", StaticMesh));
	}

	{	// Creating a level shape with Transform, collider and shape.
		RefPtr<GameObject> const LevelShapeObject = CurrentScene->CreateGameObject();
		RefPtr<Transform > const LevelShapeTransform = LevelShapeObject->AddComponent<Transform>();
		LevelShapeTransform->SetScale(Vector3Fast(25.0f));

		RefPtr<MeshRenderer> const LevelShapeRenderer = LevelShapeObject->AddComponent<MeshRenderer>();
		LevelShapeRenderer->SetStaticMesh(RSStreamer::GetInstance().LoadImmediately<StaticMesh>("file://Resources/LevelMesh.ProprietaryMeshFormat"));
		LevelShapeRenderer->SetMaterial(RSStreamer::GetInstance().LoadImmediately<Material>("file://Resources/LevelMeshMaterial.xml"));

	//	RefPtr<MeshCollider> const LevelShapeCollider = LevelShapeObject->AddComponent<MeshCollider>();
	}
}

#define Time 
constexpr Float32 GetDeltaTime() { return 0.0014f; } // Now executing at 650 - 700 FPS.

void SimpleSceneFPSController::OnUpdateSelf()
{	// Getting our Transform component.
	// Note: Unlike Unity, Transform is cached. So it can be used without user-caching.
	RefPtr<Transform> const MyTransform(this->GetGameObject()->GetTransform());

	// Handling WASD keys to move, E and R to rotate.
	Float32 const DeltaPosition = this->MovingSpeed * Time::GetDeltaTime();
	// Forward/Backward.
	if (Input::IsKeyDown(KeyCode::W)) {
		MyTransform->Translate(Vector3Fast(0.0f, 0.0f, DeltaPosition));
	} else if (Input::IsKeyDown(KeyCode::S)) {
		MyTransform->Translate(Vector3Fast(0.0f, 0.0f, -DeltaPosition));
	}
	
	// Right/Left.
	if (Input::IsKeyDown(KeyCode::D)) {
		MyTransform->Translate(Vector3Fast(DeltaPosition, 0.0f, 0.0f));
	} else if (Input::IsKeyDown(KeyCode::A)) {
		MyTransform->Translate(Vector3Fast(-DeltaPosition, 0.0f, 0.0f));
	}
	
	// Rotating Right/Left.
	if (Input::IsKeyDown(KeyCode::E)) {
		MyTransform->Rotate(Vector3Fast(0.0f, 5.0f * DeltaPosition, 0.0f));
	} else if (Input::IsKeyDown(KeyCode::Q)) {
		MyTransform->Rotate(Vector3Fast(0.0f, -5.0f * DeltaPosition, 0.0f));
	}
	
	// Handling space key to shoot.
	if (Input::IsKeyUp(KeyCode::Space)) {
		throw NotImplementedException("Shooting is not implemented!");
	}
}

// GD_SERIALIZABLE_IMPLEMENTATION(SimpleSceneFPSController, Component, GDINT);

int main(int const CMDArgsCount, char const* const* const CMDArgsList)
{
	SimpleSceneSampleApp SimpleSceneSampleAppInstance(CMDArgsCount, CMDArgsList);
	Application::Execute(SimpleSceneSampleAppInstance);
}
