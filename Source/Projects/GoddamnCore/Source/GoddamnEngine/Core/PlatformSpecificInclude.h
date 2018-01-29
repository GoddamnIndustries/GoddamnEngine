// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/System/Platform.h
 * File contains platform-abstraction-layer definitions. It should be directly included
 * at the top of each header and source file in this directory.
 */
#pragma once

#include <GoddamnEngine/Include.h>

// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
// ******                       Platform-Dependent include wrapper.                        ******
// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

#if !GD_DOCUMENTATION
#	if GD_PLATFORM_API_MICROSOFT && !defined(GD_PLATFORM_API_INCLUDE) 
#		define GD_PLATFORM_API_INCLUDE(Directory, Header)	<Directory/Header ## Microsoft.h>
#	endif	// GD_PLATFORM_API_MICROSOFT && !defined(GD_PLATFORM_API_INCLUDE)
#	if GD_PLATFORM_API_COCOA && !defined(GD_PLATFORM_API_INCLUDE)
#		define GD_PLATFORM_API_INCLUDE(Directory, Header)	<Directory/Header ## Cocoa.h>
#	endif	// GD_PLATFORM_API_COCOA && !defined(GD_PLATFORM_API_INCLUDE)
#	if GD_PLATFORM_API_POSIX && !defined(GD_PLATFORM_API_INCLUDE)
#		define GD_PLATFORM_API_INCLUDE(Directory, Header)	<Directory/Header ## Posix.h>
#	endif	// GD_PLATFORM_API_POSIX && !defined(GD_PLATFORM_API_INCLUDE)
#	if GD_PLATFORM_API_LIBSDL2 && !defined(GD_PLATFORM_API_INCLUDE)
#		define GD_PLATFORM_API_INCLUDE(Directory, Header)	<Directory/Header ## SDL2.h>
#	endif	// GD_PLATFORM_API_LIBSDL2 && !defined(GD_PLATFORM_API_INCLUDE)
#	if GD_PLATFORM_API_LIBSDL1 && !defined(GD_PLATFORM_API_INCLUDE)
#		define GD_PLATFORM_API_INCLUDE(Directory, Header)	<Directory/Header ## SDL1.h>
#	endif	// GD_PLATFORM_API_LIBSDL1 && !defined(GD_PLATFORM_API_INCLUDE)
#	ifndef GD_PLATFORM_API_INCLUDE
#		error GD_PLATFORM_API_INCLUDE: No platform API was selected.
#	endif	// if GD_PLATFORM_INCLUDE
#endif	// if !GD_DOCUMENTATION

#if !GD_DOCUMENTATION
#	if GD_PLATFORM_WINDOWS
#		define GD_PLATFORM_INCLUDE(Directory, Header)	<Directory/Header ## Windows.h>
#	endif	// if GD_PLATFORM_WINDOWS
#	if GD_PLATFORM_WINDOWS_PHONE
#		define GD_PLATFORM_INCLUDE(Directory, Header)	<Directory/Header ## WindowsPhone.h>
#	endif	// if GD_PLATFORM_WINDOWS_PHONE
#	if GD_PLATFORM_WINDOWS_UAP
#		define GD_PLATFORM_INCLUDE(Directory, Header)	<Directory/Header ## WindowsUAP.h>
#	endif	// if GD_PLATFORM_WINDOWS_RT
#	if GD_PLATFORM_OS_X
#		define GD_PLATFORM_INCLUDE(Directory, Header)	<Directory/Header ## OSX.h>
#	endif	// if GD_PLATFORM_OS_X
#	if GD_PLATFORM_IOS
#		define GD_PLATFORM_INCLUDE(Directory, Header)	<Directory/Header ## iOS.h>
#	endif	// if GD_PLATFORM_IOS
#	if GD_PLATFORM_GNU_LINUX
#		define GD_PLATFORM_INCLUDE(Directory, Header)	<Directory/Header ## GNULinux.h>
#	endif	// if GD_PLATFORM_GNU_LINUX
#	if GD_PLATFORM_ANDROID
#		define GD_PLATFORM_INCLUDE(Directory, Header)	<Directory/Header ## Android.h>
#	endif	// if GD_PLATFORM_ANDROID
#	if GD_PLATFORM_XBOX_ONE
#		define GD_PLATFORM_INCLUDE(Directory, Header)	<Directory/Header ## XboxOne.h>
#	endif	// if GD_PLATFORM_XBOX_ONE
#	if GD_PLATFORM_PLAYSTATION4
#		define GD_PLATFORM_INCLUDE(Directory, Header)	<Directory/Header ## PlayStation4.h>
#	endif	// if GD_PLATFORM_PLAYSTATION4
#	if GD_PLATFORM_HTML5
#		define GD_PLATFORM_INCLUDE(Directory, Header)	<Directory/Header ## HTML5.h>
#	endif	// if GD_PLATFORM_HTML5
#	ifndef GD_PLATFORM_INCLUDE
#		error GD_PLATFORM_INCLUDE: No platform was selected.
#	endif	// if GD_PLATFORM_INCLUDE
#endif	// if !GD_DOCUMENTATION

#if !GD_DOCUMENTATION
#	if GD_ARCHITECTURE_X86 && !defined(GD_ARCHITECTURE_INCLUDE)
#		define GD_ARCHITECTURE_INCLUDE(Directory, Header)	<Directory/Header ## x86.h>
#	endif	// if GD_ARCHITECTURE_X86 && !defined(GD_ARCHITECTURE_INCLUDE)
#	if GD_ARCHITECTURE_X64 && !defined(GD_ARCHITECTURE_INCLUDE)
#		define GD_ARCHITECTURE_INCLUDE(Directory, Header)	<Directory/Header ## x64.h>
#	endif	// if GD_ARCHITECTURE_X64 && !defined(GD_ARCHITECTURE_INCLUDE)
#	if GD_ARCHITECTURE_ARM32 && !defined(GD_ARCHITECTURE_INCLUDE)
#		define GD_ARCHITECTURE_INCLUDE(Directory, Header)	<Directory/Header ## xARM32.h>
#	endif	// if GD_ARCHITECTURE_ARM32 && !defined(GD_ARCHITECTURE_INCLUDE)
#	if GD_ARCHITECTURE_ARM64 && !defined(GD_ARCHITECTURE_INCLUDE)
#		define GD_ARCHITECTURE_INCLUDE(Directory, Header)	<Directory/Header ## xARM64.h>
#	endif	// if GD_ARCHITECTURE_ARM64 && !defined(GD_ARCHITECTURE_INCLUDE)
#	ifndef GD_ARCHITECTURE_INCLUDE
#		error GD_ARCHITECTURE_INCLUDE: No architecture was selected.
#	endif	// if GD_ARCHITECTURE_INCLUDE
#endif	// if !GD_DOCUMENTATION

#if GD_DOCUMENTATION

/*!
 * Generates a path to header with implementation of some platform API-dependent code.
 * 
 * @param Directory Path to directory with subdirectories that contain platform API-dependent code.
 * @param Header Name of the header file.
 */
#define GD_PLATFORM_API_INCLUDE(Directory, Header)	<Directory/Header ## CurrentPlatformAPI.h>

/*!
 * Generates a path to header with implementation of some platform-dependent code.
 * 
 * @param Directory Path to directory with subdirectories that contain platform-dependent code.
 * @param Header Name of the header file.
 */
#define GD_PLATFORM_INCLUDE(Directory, Header)	<Directory/Header ## CurrentPlatform.h>

/*!
 * Generates a path to header with implementation of some architecture-dependent code.
 * 
 * @param Directory Path to directory with subdirectories that contain architecture-dependent code.
 * @param Header Name of the header file.
 */
#define GD_ARCHITECTURE_INCLUDE(Directory, Header)	<Directory/Header ## CurrentArchitecture.h>

#endif	// if GD_DOCUMENTATION
