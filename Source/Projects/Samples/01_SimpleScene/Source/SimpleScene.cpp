#define GD_DLL_IMPORT 1

#include <GoddamnEngine/Core/Platform/PlatformApplication.h>
#include <GoddamnEngine/Core/Platform/PlatformWindow.h>

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

#if GD_PLATFORM_WINDOWS_UWP
[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^ args)
#else
int main()
#endif
{
	TestAppDelegate appDelegate;
	IPlatformApplication::Get().Run(appDelegate);
	return 0;
}
