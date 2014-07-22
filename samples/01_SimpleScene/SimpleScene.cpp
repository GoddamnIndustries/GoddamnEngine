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
#include <GoddamnEngine/Engine/Component/Transform/Transform.h>			// To position objects in the world.
#include <GoddamnEngine/Engine/Component/MeshRenderer/MeshRenderer.h>	// To render meshes.

#if (defined(GD_NAMESPACE))
using namespace GD_NAMESPACE;
#endif	// if (defined(GD_NAMESPACE))

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
$GD_REFLECTABLE()
class SimpleSceneFPSController final : public Component
{
	$GD_REFLECTABLE_BODY_GENERATED_CRAP()
	 GD_TYPEINFORMATION_DEFINITION(SimpleSceneFPSController, Component, GDINT)

public/*Component properties*/:
	/// Player's moving speed.
	$GD_PROPERTY(ReadWrite, DefinedAs = Field)
	float MovingSpeed = 1.3f;

public/*Class API*/:
	GDINL SimpleSceneFPSController() { }
	GDINL virtual ~SimpleSceneFPSController() { }

	/// Is invoked each frame engine renders.
	virtual void OnUpdateSelf() final override;
};	// class SimpleSceneFPSController

GD_TYPEINFORMATION_IMPLEMENTATION(SimpleSceneFPSController, Component, GDINT);

void SimpleSceneSampleApp::OnInitialize()
{	// Initializing Application.
	self->Application::OnInitialize();

	// Creating a scene.
	/*RefPtr<Scene>*/Scene* const CurrentScene = new Scene(Scene::FlagsNone);
	{	// Creating a Player with Transform, camera and FPS controller and collider.
		RefPtr<GameObject> const PlayerObject = CurrentScene->CreateGameObject();
		RefPtr<Transform> const PlayerTransform = PlayerObject->AddComponent<Transform>();
		PlayerTransform->SetPosition(Vector3Fast(0.0f, 1.75f, -7.0f));

		RefPtr<Camera> const PlayerCamera = PlayerObject->AddComponent<Camera>();
		RefPtr<SimpleSceneFPSController> const PlayerFPSController = PlayerObject->AddComponent<SimpleSceneFPSController>();
		
	//	RefPtr<Rigidbody> const PlayerRigidbody = PlayerObject->AddComponent<Rigidbody>();
	//	PlayerRigidbody->SetRigidbodyType(RigidbodyType::Character);
	//	PlayerRigidbody->SetColliderShape(GD_LOAD_RESOURCE("file://..\\Resources/labs_mesh", StaticMesh));
	}

	{	// Creating a level shape with Transform, collider and shape.
		RefPtr<GameObject> const LevelShapeObject = CurrentScene->CreateGameObject();
		RefPtr<Transform> const LevelShapeTransform = LevelShapeObject->AddComponent<Transform>();
		LevelShapeTransform->SetScale(Vector3Fast(25.0f));

		RefPtr<MeshRenderer> const LevelShapeRenderer = LevelShapeObject->AddComponent<MeshRenderer>();
		LevelShapeRenderer->SetStaticMesh(RSStreamer::GetInstance().LoadImmediately<StaticMesh>("file://Resources/LevelMesh.ProprietaryMeshFormat"));
		LevelShapeRenderer->SetMaterial(RSStreamer::GetInstance().LoadImmediately<Material>("file://Resources/LevelMeshMaterial.xml"));

	//	RefPtr<MeshCollider> const LevelShapeCollider = LevelShapeObject->AddComponent<MeshCollider>();
	}
}

void SimpleSceneFPSController::OnUpdateSelf()
{	// Getting our Transform component.
	Transform* const MyTransform = self->GetGameObject()->GetTransform();

	// Handling WASD keys to move, E and R to rotate.
	/**/ if (Input::IsKeyDown(KeyCode::W)) MyTransform->Translate(Vector3Fast(0.0f, 0.0f, +self->MovingSpeed));
	else if (Input::IsKeyDown(KeyCode::S)) MyTransform->Translate(Vector3Fast(0.0f, 0.0f, -self->MovingSpeed));
	/**/ if (Input::IsKeyDown(KeyCode::A)) MyTransform->Translate(Vector3Fast(-self->MovingSpeed, 0.0f, 0.0f));
	else if (Input::IsKeyDown(KeyCode::D)) MyTransform->Translate(Vector3Fast(+self->MovingSpeed, 0.0f, 0.0f));
	/**/ if (Input::IsKeyDown(KeyCode::Q)) MyTransform->Rotate(Vector3Fast(0.0f, -self->MovingSpeed * 5.0f, 0.0f));
	else if (Input::IsKeyDown(KeyCode::E)) MyTransform->Rotate(Vector3Fast(0.0f, +self->MovingSpeed * 5.0f, 0.0f));

	// Handling space key to shoot.
	if (Input::IsKeyUp(KeyCode::Space)) {

	}
}

// GD_SERIALIZABLE_IMPLEMENTATION(SimpleSceneFPSController, Component, GDINT);

int main(int const CMDArgsCount, char const* const* const CMDArgsList)
{
	SimpleSceneSampleApp SimpleSceneSampleAppInstance(CMDArgsCount, CMDArgsList);
	Application::Execute(SimpleSceneSampleAppInstance);
}
