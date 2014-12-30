/// ==========================================================================================
/// Framework.cpp - Framework implementation selector (non-Cocoa) source file.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 10.06.2014 - Created by James Jhuighuy
/// ==========================================================================================

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Framework/Framework.h>

/// ==========================================================================================
/// Some defualt stuff.
/// ==========================================================================================

GD_NAMESPACE_BEGIN

	Str    const Window::DefaultWindowTitle  = "Goddamn motherfucken Window!";
	Rectangle const Window::DefaultWindowRectangle = Rectangle(100, 100, 1600, 900);

GD_NAMESPACE_END

/// ==========================================================================================
/// Including platform-dependant solutions.
/// ==========================================================================================

#if (defined(GD_PLATFORM_API_LIBSDL2))
#	include "Framework.SDL2.inl"
//endif	// if (defined(GD_PLATFORM_API_LIBSDL2))
#else	// *** Selecting best-suiting API ***
//#	error "Framework code is not implemented on target platform"
#endif	// *** Selecting best-suiting API ***
