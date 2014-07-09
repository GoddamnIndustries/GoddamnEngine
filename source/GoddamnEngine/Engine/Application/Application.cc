#include <GoddamnEngine/Engine/Application/Application.hh>
#include <GoddamnEngine/Engine/Component/GameObject/GameObject.hh>
#include <GoddamnEngine/Engine/Resource/Resource.hh>

#include <GoddamnEngine/Engine/HUD/HUD.hh>
#include <GoddamnEngine/Engine/Plugin/Plugin.hh>
#include <GoddamnEngine/Engine/Renderer/Renderer.hh>
// #include <GoddamnEngine/Engine/Renderer/Impl/OpenGL/OGLRenderer.hh>
#include <GoddamnEngine/Engine/Renderer/Impl/Direct3D11/D3D11Renderer.hh>
#include <GoddamnEngine/Engine/Resource/Impl/ShaderProgram/ShaderProgramResource.hh>

#include <conio.h>

#include <GoddamnEngine/Engine/Resource/Impl/StaticMesh/StaticMesh.hh>

GD_NAMESPACE_BEGIN

	GD_TYPEINFORMATION_IMPLEMENTATION_C(Application, Object, GDAPI, nullptr);
	GD_SINGLETON_IMPLEMENTATION(Application);

	GDINL void OnEvent(chandle const argument)
	{
		Application::Exit(ExitCode::Normal);
	}

	/// ==========================================================================================
	// Constructor / Destructor
	/// ==========================================================================================

	Application::Application(int const argumentsCount, 
							 char const* const* const argumentsList) :
		CommandLineArgumentsCount(argumentsCount),
		CommandLineArgumentsList(argumentsList),
		lowLevelSystem(new LowLevelSystem()),
		pluginManager(new PluginManager()),
		resourceStreamer(new ResourceStreamer()),
		staticComponentHandler(new GameObject()),
		gameObjectsHandler(new /*Object*/GameObject())
	{
		/*atexit([]() -> void
		{
			// Registering hook for some libraries like Mono
			// termination on-error process: they just print message and terminate app.
			Debug::Log("Press any key to terminate...");
			
			#pragma warning(suppress: 6031)
			_getch();
		});*/

		new HRD3D11Interface();
		self->staticComponentHandler.GetPointer()->AttachToObject(self);
		StaticComponent::InitializeAsHandler(self->staticComponentHandler.GetPointer());
		HRInterface::GetInstance().CreateContex();
	}

	Application::~Application()
	{
	}

	/// ==========================================================================================
	// Application termination
	/// ==========================================================================================

	void Application::Exit(const ExitCode exitCode)
	{
		Application& 
			application = Application::GetInstance();
			application.state = ApplicationState::Exiting;

		if (exitCode != ExitCode::Normal)	// We not terminating in unusual way
		{
			exit((int)exitCode);
		}

		// Temporary force exit
		exit(0);
	}

	void Application::CancelExit()
	{
		Application::GetInstance().state = ((Application::GetInstance().state != ApplicationState::Exiting)
			? Application::GetInstance().state
			: ApplicationState::Running);
	}

	/// ==========================================================================================
	// Application Execution
	/// ==========================================================================================

	int Application::Execute(Application& application)
	{
		//JobSystem::StartJobThread();
		application.state = ApplicationState::Starting;

		application.staticComponentHandler->OnInitializeSelf();
		application.staticComponentHandler->OnStartSelf();
		application.OnInitialize();
		
		Scene::GetInstance().OnInit();
		Scene::GetInstance().OnStart();
		
		// Beginning initialization phase:
		LowLevelSystem::GetInstance().SetEventCallBack(GD_LL_EVENT_ON_WINDOW_CLOSED, OnEvent);
		ResourceStreamer::GetInstance().WaitForLoading();

		// All resources are now loaded, we a ready for final start 
		// And rendering! God is with us!
		application.state = ApplicationState::Running;
		for (/*size_t fpsCount = 0*/; application.state != ApplicationState::Exiting;)
		{
			LowLevelSystem::GetInstance().UpdateWindow();
			Scene::GetInstance().OnUpdate();
			application.staticComponentHandler->OnUpdateSelf();
			application.staticComponentHandler->OnRenderSelf(nullptr);
			
			/*if ((++fpsCount % 60 == 0 && application.state != ApplicationState::Suspended)
				|| (DeviceConfiguration::IsVerticalSyncronizationEnabled()))*/
			{
			}
		}

		application.OnDestroy();

		return 0;
	}

GD_NAMESPACE_END
