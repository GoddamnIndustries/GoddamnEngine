/// ==========================================================================================
/// Framework.cpp - Framework implementation selector (non-Cocoa) source file.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 10.06.2014 - Created by James Jhuighuy
/// ==========================================================================================

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Framework/Framework.h>

GD_NAMESPACE_BEGIN

	Str       const Window::DefaultWindowTitle     = "Goddamn motherfucken Window!";
	Rectangle const Window::DefaultWindowRectangle = Rectangle(100, 100, 1600, 900);

GD_NAMESPACE_END

#if  ((!defined(GD_PLATFORM_OSX)) && (!defined(GD_PLATFORM_IOS)))
#	if (defined(GD_PLATFORM_WINDOWS)) && 0
#		include <GoddamnEngine/Core/Framework/Framework.WinApi.inl>
#	else
#		include <GoddamnEngine/Core/Framework/Framework.SDL2.inl>
#	endif	// if (defined(GD_PLATFORM_WINDOWS))
#endif	// if ((!defined(GD_PLATFORM_OSX)) && (!defined(GD_PLATFORM_IOS)))
