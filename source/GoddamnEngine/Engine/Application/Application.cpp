#include <GoddamnEngine/Engine/Application/Application.h>
#include <GoddamnEngine/Engine/Component/GameObject/GameObject.h>
#include <GoddamnEngine/Engine/Resource/Resource.h>

#include <GoddamnEngine/Core/IO/Path/Path.h>

#include <GoddamnEngine/Engine/HUD/HUD.h>
#include <GoddamnEngine/Engine/Plugin/Plugin.h>
#include <GoddamnEngine/Engine/Renderer/Renderer.h>
// #include <GoddamnEngine/Engine/Renderer/Impl/OpenGL/OGLRenderer.h>
#include <GoddamnEngine/Engine/Renderer/Impl/Direct3D11/D3D11Renderer.h>
#include <GoddamnEngine/Engine/Resource/Impl/Effect/Effect.h>

#include <conio.h>

#include <GoddamnEngine/Engine/Resource/Impl/StaticMesh/StaticMesh.h>

GD_NAMESPACE_BEGIN

	GD_TYPEINFORMATION_IMPLEMENTATION_C(Application, Object, GDAPI, nullptr);
	GD_SINGLETON_IMPLEMENTATION(Application);

	GDINL void OnEvent(chandle const argument)
	{
		Application::Exit(ExitCode::Normal);
	}

	/// ==========================================================================================
	/// Constructor / Destructor
	/// ==========================================================================================

	Application::Application(int const CMDArgsCount, char const* const* const CMDArgsList) 
		: CMDArgsCount(CMDArgsCount)
		, CMDArgsList(CMDArgsList)
		, TheResourceStreamer(new RSStreamer())
		, ThePluginManager(new PluginManager())
		, lowLevelSystem(new LowLevelSystem())
		, staticComponentHandler(new GameObject())
		, gameObjectsHandler(new /*Object*/GameObject())
	{	// Some libraries, like Mono just terminate process if some error accures. 
		// For debug purposes we setup a callback.
#if (defined(GD_PLATFORM_DESKTOP))
		int const ExitCallbackSetupResult = std::atexit([]() -> void {
#	if (defined(GD_PLATFORM_WINDOWS))
			DWORD ExitCode = MAXDWORD;
			::GetExitCodeProcess(::GetCurrentProcess(), &ExitCode);
			if (ExitCode == 0)
				return;
#	endif	// if (defined(GD_PLATFORM_WINDOWS))
			Debug::Log("Press any key to terminate...");
			GD_WARNING_SUPPRESS(6031)
			_getch();
		});
		GD_DEBUG_ASSERT(ExitCallbackSetupResult == 0, "Failed to setup exit callback.");
#endif	// if (defined(GD_PLATFORM_DESKTOP))

		// Handling command line arguments.
		GD_DEBUG_ASSERT((self->CMDArgsCount >= 1) && (self->CMDArgsList != nullptr), "Invalid command line arguments specified");
		self->EnvironmentPath = Path::GetDirectoryName(self->CMDArgsList[0]);

		new HRD3D11Interface();
		self->staticComponentHandler.GetPointer()->AttachToObject(self);
		StaticComponent::InitializeAsHandler(self->staticComponentHandler.GetPointer());
		HRInterface::GetInstance().CreateContex();
	}

	Application::~Application()
	{
	}

	/// ==========================================================================================
	/// Application termination
	/// ==========================================================================================

	void Application::Exit(ExitCode const TheExitCode)
	{
		Application::GetInstance().State = ApplicationState::Exiting;
		if (TheExitCode != ExitCode::Normal)	// We not terminating in unusual way
			std::exit(static_cast<int>(TheExitCode));

		// Temporary force exit
		exit(0);
	}

	void Application::CancelExit()
	{
		ApplicationState& State = Application::GetInstance().State;
		State = ((State != ApplicationState::Exiting) ? Application::GetInstance().State : ApplicationState::Running);
	}

	/// ==========================================================================================
	// Application Execution
	/// ==========================================================================================

	int Application::Execute(Application& application)
	{
		//JobSystem::StartJobThread();
		application.State = ApplicationState::Starting;

		application.staticComponentHandler->OnInitializeSelf();
		application.staticComponentHandler->OnStartSelf();
		application.OnInitialize();
		
		Scene::GetInstance().OnInit();
		Scene::GetInstance().OnStart();
		
		// Beginning initialization phase:
		LowLevelSystem::GetInstance().SetEventCallBack(GD_LL_EVENT_ON_WINDOW_CLOSED, OnEvent);
		RSStreamer::GetInstance().WaitForLoading();

		// All resources are now loaded, we a ready for final start 
		// And rendering! God is with us!
		application.State = ApplicationState::Running;
		for (/*size_t fpsCount = 0*/; application.State != ApplicationState::Exiting;)
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
