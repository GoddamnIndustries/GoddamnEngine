#include <GoddamnEngine/Engine/Component/Static/Screen/Screen.h>
#include <GoddamnEngine/Engine/Component/Static/DeviceConfiguration/DeviceConfiguration.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

GD_NAMESPACE_BEGIN

	/// ==========================================================================================
	// Specifying Null-renderer's screen
	/// ==========================================================================================

	GD_SERIALIZABLE_IMPLEMENTATION(Screen, StaticComponent, GDAPI);
	GD_SINGLETON_IMPLEMENTATION(Screen);

	/// ==========================================================================================
	// Constructor/Destructor
	/// ==========================================================================================

	Screen::Screen() : StaticComponent(StaticComponentPriority::High)
	{
	}

	Screen::~Screen()
	{
	}

	/// ==========================================================================================
	// Screen resolution management
	/// ==========================================================================================

	const Resolution& Screen::SetResolution(const Resolution& screenResolution)
	{
		return GD_REAPPLY_RESOLUTION;
	}

	/// ==========================================================================================
	// Window position management
	/// ==========================================================================================

	void Screen::GetWindowPosition(Position& position) const
	{
	}

	void Screen::SetWindowPosition(const Position& position)
	{
	}

	/// ==========================================================================================
	// Windowed/Fullscreen management
	/// ==========================================================================================

	bool Screen::GetIsFullscreen() const
	{
		return false;
	}

	void Screen::SetIsFullscreen(const bool fullscreen)
	{
	}

	/// ==========================================================================================
	// Title management
	/// ==========================================================================================

	void Screen::GetWindowTitle(String& title) const
	{
	}

	void Screen::SetWindowTitle(const String& title)
	{
	}

	/// ==========================================================================================
	// VSync management
	/// ==========================================================================================

	bool Screen::GetVsyncEnabled() const
	{
		return false;
	}

	void Screen::SetVsyncEnabled(const bool isVsyncEnabled)
	{
	}

	/// ==========================================================================================
	// Cursor management
	/// ==========================================================================================

	bool Screen::GetCursorShowing() const
	{
		return false;
	}

	void Screen::SetCursorShowing(const bool showCursor)
	{
	}

GD_NAMESPACE_END
