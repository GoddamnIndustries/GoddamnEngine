#include <GoddamnEngine/Core/LowLevelSystem/LowLevelSystem.hh>

GD_NAMESPACE_BEGIN

	GDINL void Screen::GetResolution(Resolution& resolution) const
	{
		resolution = LowLevelSystem::GetInstance().WindowResolution;
	}

	GDINL Dimension Screen::GetResolutionWidth()
	{
		Resolution resolution;
		Screen::GetInstance().GetResolution(resolution);

		return resolution.Width;
	}

	GDINL Dimension Screen::GetResolutionHeight()
	{
		Resolution resolution;
		Screen::GetInstance().GetResolution(resolution);

		return resolution.Height;
	}

	GDINL void Screen::ShowCursor() 
	{ 
		Screen::GetInstance().SetCursorShowing(true); 
	}

	GDINL void Screen::HideCursor() 
	{ 
		Screen::GetInstance().SetCursorShowing(false); 
	}

GD_NAMESPACE_END
