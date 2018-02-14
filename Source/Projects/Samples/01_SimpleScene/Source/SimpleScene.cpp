#define GD_DLL_IMPORT 1

#include <GoddamnEngine/Core/Platform/PlatformApplication.h>
#include <GoddamnEngine/Core/Platform/PlatformWindow.h>
#include "GoddamnEngine/Core/Platform/PlatformFileSystem.h"
#include "GoddamnEngine/Core/Platform/PlatformProcess.h"

using namespace GD_NAMESPACE;

class TestAppDelegate final : public IPlatformApplicationDelegate
{
	GDINT virtual void OnAppUpdate() override final
	{
	}

	GDINT virtual void OnKeyboardButtonUp(KeyboardButton const keyboardButton) override final
	{
		printf("%c\n", keyboardButton);
	}
};	// class TestAppDelegate

class DesktopWatcherDelegate : public IFileSystemWatcherDelegate
{
};	// class DesktopWatcherDelegate

#if GD_PLATFORM_WINDOWS_UWP
[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^ args)
#else
int main()
#endif
{
	Handle pipe[2];
	IPlatformProcess::Get().PipeCreate(pipe[0], pipe[1]);

	Handle p;
	ProcessCreateInfo pci = { L"cmd.exe /c dir" };
	pci.ProcessStandardOutput = pipe[1];
	IPlatformProcess::Get().ProcessCreate(pci, p);

	WideString c;
	while(!IPlatformProcess::Get().PipeReadPendingContent(pipe[0], c));
	IPlatformProcess::Get().ProcessTerminateRecursive(p);

	DesktopWatcherDelegate watcherDelegate;
	IPlatformDiskFileSystemWatcher::Get().AddWatch(L"C:\\Users\\Oleg\\Desktop", watcherDelegate);

	TestAppDelegate appDelegate;
	IPlatformApplication::Get().Run(appDelegate);
	return 0;
}
