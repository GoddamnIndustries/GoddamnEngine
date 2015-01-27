/// ==========================================================================================
/// Window.h - Generic-platform window class.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_PLATFORM_WINDOW
#define GD_CORE_PLATFORM_WINDOW

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Math/Vector4.h>

GD_NAMESPACE_BEGIN

	class IGenericWindow
	{
	protected:
		bool HasBorder = true;
		bool SupportsTransparency = false;
		bool AppearsInTaskbar = true;
		bool IsTopmostWindow = false;
		bool AcceptsInput = false;

	public:

		GDINL IGenericWindow() 
		{
		}

		GDINL virtual ~IGenericWindow()
		{
		}

		/// @brief Changes window position and size.
		/// @param Rect The new window rectangle.
		GDAPI virtual void ReshapeWindow(Rectangle const& Rect)
		{
		}

		/// @brief Returns the rectangle of the window.
		/// @param Rect Output for rectangle.
		/// @returns True on success, false otherwise.
		GDAPI virtual bool GetRectangle(Rectangle& Rect) const
		{
			return false;
		}

	};	// class IGenericWindow

GD_NAMESPACE_END

#endif	// ifndef GD_CORE_PLATFORM_WINDOW
