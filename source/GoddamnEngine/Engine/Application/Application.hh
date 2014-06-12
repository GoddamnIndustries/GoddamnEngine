#ifndef GD_APPLICATION_APPLICATION
#define GD_APPLICATION_APPLICATION

#include <GoddamnEngine/Include.hh>

#include <GoddamnEngine/Core/Object/Object.hh>
#include <GoddamnEngine/Core/Object/RefPtr/RefPtr.hh>
#include <GoddamnEngine/Core/Object/Singleton/Singleton.hh>
#include <GoddamnEngine/Core/Containers/String/String.hh>
#include <GoddamnEngine/Core/Containers/Map/Map.hh>
#include <GoddamnEngine/Core/LowLevelSystem/LowLevelSystem.hh>

#include <GoddamnEngine/Engine/Resource/ResourceStreamer/ResourceStreamer.hh>
#include <GoddamnEngine/Engine/Plugin/Plugin.hh>
#include <GoddamnEngine/Engine/Scene/Scene.hh>

#define GD_EXIT_CODE_OFFSET 0x00008421	//(This value is binary identity Matrix4x4) 0xB2CB

GD_NAMESPACE_BEGIN

	/// Describes application state
	enum class ApplicationState : byte
	{
		Starting,	///< The 'Application' is starting now (before 'OnInit' and 'OnStart')
		Running,	///< The 'Application' is running now (invoking 'OnUpdate' and 'OnRender')
		Suspended,	///< The 'Application' is in background now (invoking 'OnUpdate' only)
		Exiting		///< 'Application::Exit(0)' was called
	};

	/// Describes reason of application termination
	enum class ExitCode : size_t
	{
		Normal		= GD_EXIT_CODE_OFFSET + 0,	///< Application is going to be terminated normally
		Unknown		= 0xFF,						///< Unknown termination reason.
	};

	class Application : public Object,
					    public Singleton<Application>
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(Application, Object, GDAPI);

		RefPtr<LowLevelSystem> lowLevelSystem;
		RefPtr<PluginManager> pluginManager;
		RefPtr<ResourceStreamer> resourceStreamer;
		// RefPtr<SceneManager> sceneManager;
		RefPtr<GameObject> staticComponentHandler;
		RefPtr<Object> gameObjectsHandler;

	protected:
		Map<HashSumm, String>	cmdArguments;
		ApplicationState state;
		size_t loadedScene;
		String path;

	public:
		int /*const*/ CommandLineArgumentsCount;
		char const* const* const CommandLineArgumentsList;

		/// @name Application termination
		/// @{

		/// @brief				Terminates application or lets it exiting normally
		/// @param exitCode		Application's exit code.
		GDAPI static void Exit(const ExitCode exitCode);

		/// @brief				Cancels exiting process if requested normal termination
		GDAPI static void CancelExit();

		/// @}

		/// @name Application Execution
		/// @{

		/// @brief				Executes specified application
		/// @param application	Application instance to execute
		/// @returns			Exit code
		GDAPI static int Execute(Application& application);

		/// @}

	protected:

		/// @name Constructor / Destructor
		/// @{

		/// @brief			Initializes application with command line arguments
		/// @param argc		Command arguments cout
		/// @param argv		Command arguments value
		GDAPI Application(int const argumentsCount, 
						  char const* const* const argumentsList);
		
		/// @brief			Deallocates application
		GDAPI virtual ~Application();

		/// @}

	public:

		/// @name Getters
		/// @{

		/// @brief			Returns 'Application'`s name
		/// @returns		'Application'`s name
		GDAPI virtual String GetApplicationName() const abstract;

		/// @brief			Returns 'Application'`s version
		/// @returns		'Application'`s version
		GDAPI virtual String GetApplicationVersion() const abstract;

		/// @brief			Returns 'Application'`s state
		/// @returns		'Application'`s state
		GDINL ApplicationState GetApplicationState() const;

		/// @}

	private:
		/// @name Messages for Application <-> OS exchange
		/// @{

		/// @brief			Overridable method to initialize application 
		GDINL virtual void OnInitialize();

		/// @brief			Overridable method to destroy application 
		GDINL virtual void OnDestroy();

		/// @brief			Overridable method to suspend application
		///					By default, application is not being rendered when is
		///					in background.
		GDINL virtual void OnSuspend();

		/// @brief			Overridable method to resume application
		GDINL virtual void OnResumed();

		/// @}
	};

GD_NAMESPACE_END

#include <GoddamnEngine/Engine/Application/Application.inl>

#endif
