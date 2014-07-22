#ifndef GD_APPLICATION_APPLICATION
#define GD_APPLICATION_APPLICATION

#include <GoddamnEngine/Include.h>

#include <GoddamnEngine/Core/Object/Object.h>
#include <GoddamnEngine/Core/Object/Singleton/Singleton.h>
#include <GoddamnEngine/Core/Text/String/String.h>
#include <GoddamnEngine/Core/Containers/Map/Map.h>
#include <GoddamnEngine/Core/Containers/Pointer/RefPtr.h>
#include <GoddamnEngine/Core/LowLevelSystem/LowLevelSystem.h>
#include <GoddamnEngine/Engine/Plugin/Plugin.h>
#include <GoddamnEngine/Engine/Scene/Scene.h>

#define GD_EXIT_CODE_OFFSET 0x00008421	//(This value is binary identity Matrix4x4) 0xB2CB

GD_NAMESPACE_BEGIN

	class RSStreamer;

	/// Describes application state
	enum class ApplicationState : UInt8
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

	class Application : public Object, public Singleton<Application>
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(Application, Object, GDAPI);
		String EnvironmentPath;

		UniquePtr<PluginManager   > ThePluginManager;
		UniquePtr<RSStreamer> TheResourceStreamer;

		RefPtr<LowLevelSystem> lowLevelSystem;
		// RefPtr<SceneManager> sceneManager;
		RefPtr<GameObject> staticComponentHandler;
		RefPtr<Object> gameObjectsHandler;

	protected:
		Map<HashSumm, String>	cmdArguments;
		ApplicationState State;
		size_t LoadedScene;

	public:
		int                const CMDArgsCount;
		char const* const* const CMDArgsList;

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

	protected/*Constructor / Destructor*/:
		/// Initializes application with command line arguments
		/// @param CMDArgsCount Command arguments cout
		/// @param CMDArgsList  Command arguments value
		GDAPI Application(int const CMDArgsCount, char const* const* const CMDArgsList);
		GDAPI virtual ~Application();

	public:
		/// Returns 'Application'`s name
		GDAPI virtual String const& GetApplicationName() const abstract;

		/// Returns 'Application'`s version
		GDAPI virtual String const& GetApplicationVersion() const abstract;

		/// Returns 'Application'`s state
		GDINL ApplicationState GetApplicationState() const { return self->State; }

		/// Returns path to application`s executable.
		GDINL String const& GetEnvironmentPath() const { return self->EnvironmentPath; }

	protected:
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
