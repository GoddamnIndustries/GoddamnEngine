// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Base/Architecture.h
 * @note This file should be never directly included, please consider using <GoddamnEngine/Include.h> instead.
 * Contains architecture definitions for GoddamnEngine.
 */
#pragma once
#if !defined(GD_INSIDE_INCLUDE_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Include.h> instead.
#endif	// if !defined(GD_INSIDE_INCLUDE_H)

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Platform Determination.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#pragma region Platform Determination
#if !GD_DOCUMENTATION

// http://sourceforge.net/p/predef/wiki/OperatingSystems/

// ------------------------------------------------------------------------------------------
// Microsoft Windows (R) Platforms family.
// ------------------------------------------------------------------------------------------
#if (defined(_WIN32)) && (!defined(GD_PLATFORM_NOT_WINDOWS)) 
#	define _USE_MATH_DEFINES    										GD_TRUE
#	define WIN32_LEAN_AND_MEAN											GD_TRUE
#	define VC_EXTRALEAN													GD_TRUE
//#	define _CRT_SECURE_NO_WARNINGS										GD_TRUE
#	if 0																// These enables heap debugging.
#	define _CRTDBG_MAP_ALLOC											GD_TRUE
#	define _CRTDBG_MAP_ALLOC_NEW										GD_TRUE
#	endif	// if 0
#	include <winapifamily.h>
#	define GD_PLATFORM_API_MICROSOFT									GD_TRUE
#	if (WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP))					//   Windows Desktop application.
#		define GD_PLATFORM_WINDOWS										GD_TRUE
#		define GD_PLATFORM_DESKTOP										GD_TRUE
#	elif (WINAPI_FAMILY_PARTITION(WINAPI_FAMILY_PHONE_APP))				//   Windows Phone 8 application.
#		define GD_PLATFORM_WINDOWS_PHONE								GD_TRUE
#		define GD_PLATFORM_MOBILE										GD_TRUE
#	elif (WINAPI_FAMILY_PARTITION(WINAPI_FAMILY_PC_APP))				//   Windows Store application (Windows Desktop or Windows RT).
#		if (defined(__WRL_WINRT_STRICT__))	|| 1						//   Windows RT application.
#			define GD_PLATFORM_WINDOWS_UAP								GD_TRUE
#			define GD_PLATFORM_DESKTOP									GD_TRUE
#		else	// if (defined(__WRL_WINRT_STRICT__))					//   Windows Desktop application.
#			define GD_PLATFORM_WINDOWS									GD_TRUE
#			define GD_PLATFORM_DESKTOP									GD_TRUE
#		endif	// if (defined(__WRL_WINRT_STRICT__))
#	else	// elif (WINAPI_FAMILY_PARTITION(WINAPI_FAMILY_***))
#		error Unsupported Windows target platform.
#	endif	// elif (WINAPI_FAMILY_PARTITION(WINAPI_FAMILY_***))
#endif	// if (defined(_WIN32)) && (!defined(GD_PLATFORM_NOT_WINDOWS))

// ------------------------------------------------------------------------------------------
// Apple (R) Platforms family.
// ------------------------------------------------------------------------------------------
#if (defined(__APPLE__))												
#	include <TargetConditionals.h>										// OS X or iOS application.
#	define GD_PLATFORM_API_COCOA										GD_TRUE
#	define GD_PLATFORM_API_POSIX										GD_TRUE
#	define GD_PLATFORM_API_LIBSDL2										GD_TRUE
#	define GD_PLATFORM_API_LIBSDL1										GD_TRUE
#	if (defined(TARGET_OS_MAC))											// OS X application.
#		define GD_PLATFORM_DESKTOP										GD_TRUE
#		define GD_PLATFORM_OS_X											GD_TRUE
#	elif defined(TARGET_IPAD_SIMULATOR) || defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IPHONE) || defined(TARGET_OS_IPAD)// iOS application.
#		define GD_PLATFORM_MOBILE										GD_TRUE
#		define GD_PLATFORM_IOS											GD_TRUE
#	else	// *** Apple Platform selection ***
#		error Unsupported Apple target platform.
#	endif	// *** Apple Platform selection ***
#endif	// if (defined(__APPLE__))

// ------------------------------------------------------------------------------------------
// GNU/Linux platforms family.
// ------------------------------------------------------------------------------------------
#if (defined(__linux__))
#	if (defined(__ANDROID__))											// Android application.
#		define GD_PLATFORM_MOBILE										GD_TRUE
#		define GD_PLATFORM_API_LIBSDL2									GD_TRUE
#		define GD_PLATFORM_API_LIBSDL1									GD_TRUE
#		define GD_PLATFORM_ANDROID										GD_TRUE
#	else	// if (defined(__ANDROID__))								// GNU/Linux application.
#		define GD_PLATFORM_DESKTOP										GD_TRUE
#		define GD_PLATFORM_API_LIBSDL2									GD_TRUE
#		define GD_PLATFORM_API_LIBSDL1									GD_TRUE
#		define GD_PLATFORM_GNU_LINUX									GD_TRUE
#	endif	// if (defined(__ANDROID__))
#	define GD_PLATFORM_API_POSIX										GD_TRUE
#endif	// if (defined(__linux__))

// ------------------------------------------------------------------------------------------
// Microsoft Xbox One Platform family.
// ------------------------------------------------------------------------------------------
#if (defined(_DURANGO))													// Xbox One application.
#	define GD_PLATFORM_API_MICROSOFT									GD_TRUE
#	define GD_PLATFORM_XBOX_ONE											GD_TRUE
#	define GD_PLATFORM_CONSOLE											GD_TRUE
#endif	// if (defined(_DURANGO))

// ------------------------------------------------------------------------------------------
// Sony PlayStation 4 platform family.
// ------------------------------------------------------------------------------------------
#if (defined(_ORBIS_))													// Sony PlayStation 4 game.
#	define GD_PLATFORM_API_ORBIS										GD_TRUE
#	define GD_PLATFORM_API_POSIX										GD_TRUE
#	define GD_PLATFORM_PLAYSTATION4										GD_TRUE
#	define GD_PLATFORM_CONSOLE											GD_TRUE
#endif	// if (defined(_ORBIS_))

// ------------------------------------------------------------------------------------------
// HTML5/Asm.js/Emscripten platforms family.
// ------------------------------------------------------------------------------------------
#if (defined(__EMSCRIPTEN__))											// HTML5/Asm.js/Emscripten webpage.
#	define GD_PLATFORM_WEB												GD_TRUE
#	define GD_PLATFORM_HTML5											GD_TRUE
#	define GD_PLATFORM_API_POSIX										GD_TRUE
#	define GD_PLATFORM_API_LIBSDL1										GD_TRUE
#endif	// if (defined(__EMSCRIPTEN__))

// Currently only HTML5 does not support multi-threading.
#if (!defined(GD_PLATFORM_HTML5)) 
#	define GD_PLATFORM_HAS_MULTITHREADING								GD_TRUE
#else	// if (!defined(GD_PLATFORM_HTML5))
#	define GD_PLATFORM_HAS_MULTITHREADING								GD_FALSE
#endif	// if (!defined(GD_PLATFORM_HTML5))

// Checking if there is actually a single platform selected.
#if (!defined(GD_PLATFORM_WINDOWS))
#	define GD_PLATFORM_WINDOWS											GD_FALSE
#endif	// if (!defined(GD_PLATFORM_WINDOWS))
#if (!defined(GD_PLATFORM_WINDOWS_PHONE))
#	define GD_PLATFORM_WINDOWS_PHONE									GD_FALSE
#endif	// if (!defined(GD_PLATFORM_WINDOWS_PHONE))
#if (!defined(GD_PLATFORM_WINDOWS_UAP))
#	define GD_PLATFORM_WINDOWS_UAP										GD_FALSE
#endif	// if (!defined(GD_PLATFORM_WINDOWS_UAP))
#if (!defined(GD_PLATFORM_OS_X))
#	define GD_PLATFORM_OS_X												GD_FALSE
#endif	// if (!defined(GD_PLATFORM_OS_X))
#if (!defined(GD_PLATFORM_IOS))
#	define GD_PLATFORM_IOS												GD_FALSE
#endif	// if (!defined(GD_PLATFORM_IOS))
#if (!defined(GD_PLATFORM_GNU_LINUX))
#	define GD_PLATFORM_GNU_LINUX										GD_FALSE
#endif	// if (!defined(GD_PLATFORM_GNU_LINUX))
#if (!defined(GD_PLATFORM_ANDROID))
#	define GD_PLATFORM_ANDROID											GD_FALSE
#endif	// if (!defined(GD_PLATFORM_ANDROID))
#if (!defined(GD_PLATFORM_XBOX_ONE))
#	define GD_PLATFORM_XBOX_ONE											GD_FALSE
#endif	// if (!defined(GD_PLATFORM_XBOX_ONE))
#if (!defined(GD_PLATFORM_PLAYSTATION4))
#	define GD_PLATFORM_PLAYSTATION4										GD_FALSE
#endif	// if (!defined(GD_PLATFORM_PLAYSTATION4))
#if (!defined(GD_PLATFORM_HTML5))
#	define GD_PLATFORM_HTML5											GD_FALSE
#endif	// if (!defined(GD_PLATFORM_HTML5))
#if (!(0 \
	^ GD_PLATFORM_WINDOWS \
	^ GD_PLATFORM_WINDOWS_PHONE \
	^ GD_PLATFORM_WINDOWS_UAP \
	^ GD_PLATFORM_OS_X \
	^ GD_PLATFORM_IOS \
	^ GD_PLATFORM_GNU_LINUX \
	^ GD_PLATFORM_ANDROID \
	^ GD_PLATFORM_XBOX_ONE \
	^ GD_PLATFORM_PLAYSTATION4 \
	^ GD_PLATFORM_HTML5))
#	error "No target platform was selected OR multiple target platforms were selected."
#endif	// *** All platforms XORed. ***

// Checking if there is actually a single platform-family selected.
#if (!defined(GD_PLATFORM_MOBILE))
#	define GD_PLATFORM_MOBILE 											GD_FALSE
#endif	// if (!defined(GD_PLATFORM_MOBILE))
#if (!defined(GD_PLATFORM_DESKTOP))
#	define GD_PLATFORM_DESKTOP 											GD_FALSE
#endif	// if (!defined(GD_PLATFORM_DESKTOP))
#if (!defined(GD_PLATFORM_CONSOLE))
#	define GD_PLATFORM_CONSOLE 											GD_FALSE
#endif	// if (!defined(GD_PLATFORM_CONSOLE))
#if (!defined(GD_PLATFORM_WEB))
#	define GD_PLATFORM_WEB												GD_FALSE
#endif	// if (!defined(GD_PLATFORM_WEB))
#if (!(0 \
	^ GD_PLATFORM_CONSOLE \
	^ GD_PLATFORM_MOBILE \
	^ GD_PLATFORM_DESKTOP \
	^ GD_PLATFORM_WEB))
#	error "No target platform family was selected OR multiple target platform families were selected."
#endif	// *** All platform families XORed ***

// Checking if there is actually a single platform API selected.
#if (!defined(GD_PLATFORM_API_MICROSOFT))
#	define GD_PLATFORM_API_MICROSOFT 									GD_FALSE
#endif	// if (!defined(GD_PLATFORM_API_MICROSOFT))
#if (!defined(GD_PLATFORM_API_COCOA))
#	define GD_PLATFORM_API_COCOA 										GD_FALSE
#endif	// if (!defined(GD_PLATFORM_API_MICROSOFT))
#if (!defined(GD_PLATFORM_API_POSIX))
#	define GD_PLATFORM_API_POSIX 										GD_FALSE
#endif	// if (!defined(GD_PLATFORM_API_POSIX))
#if (!defined(GD_PLATFORM_API_LIBSDL1))
#	define GD_PLATFORM_API_LIBSDL1 										GD_FALSE
#endif	// if (!defined(GD_PLATFORM_CONSOLE))
#if (!defined(GD_PLATFORM_API_LIBSDL2))
#	define GD_PLATFORM_API_LIBSDL2 										GD_FALSE
#else	// if (!defined(GD_PLATFORM_CONSOLE))
// Disabling support of libSDL1 if newer major version is supported.
#	if GD_PLATFORM_API_LIBSDL1
#		undef GD_PLATFORM_API_LIBSDL1
#		define GD_PLATFORM_API_LIBSDL1 									GD_FALSE
#	endif	// if GD_PLATFORM_API_LIBSDL1
#endif	// if (!defined(GD_PLATFORM_CONSOLE))
#if ((!(0 \
	^ GD_PLATFORM_API_MICROSOFT \
	^ GD_PLATFORM_API_COCOA \
	^ GD_PLATFORM_API_POSIX)) && (!(0 \
	^ GD_PLATFORM_API_LIBSDL1 \
	^ GD_PLATFORM_API_LIBSDL2)))
#	error "No target platform API was selected OR multiple target platform APIs were selected."
#endif	// *** All platform APIs XORed ***

#else	// if !GD_DOCUMENTATION

// ------------------------------------------------------------------------------------------
// Platform macros.
// ------------------------------------------------------------------------------------------

//! Engine is being built as a Windows Desktop application.
#define GD_PLATFORM_WINDOWS												Compile-Time Value

//! Engine is being built as a Microsoft Windows Phone 8 application.
#define GD_PLATFORM_WINDOWS_PHONE										Compile-Time Value

//! Engine is being built as a Microsoft Windows RT application.
#define GD_PLATFORM_WINDOWS_UAP											Compile-Time Value

//! Engine is being built as an Apple OS X application.
#define GD_PLATFORM_OS_X												Compile-Time Value

//! Engine is being built as an Apple iOS application.
#define GD_PLATFORM_IOS													Compile-Time Value

//! Engine is being built as a GNU/Linux application.
#define GD_PLATFORM_GNU_LINUX											Compile-Time Value

//! Engine is being built as a Google Android application.
#define GD_PLATFORM_ANDROID												Compile-Time Value

//! Engine is being built as a Microsoft Xbox One application.
#define GD_PLATFORM_XBOX_ONE											Compile-Time Value

//! Engine is being built as a Sony PlayStation 4 game.
#define GD_PLATFORM_PLAYSTATION4										Compile-Time Value

//! Engine is being built as a HTML5/Asm.js/Emscripten webpage.
#define GD_PLATFORM_HTML5												Compile-Time Value

// ------------------------------------------------------------------------------------------
// Platform Families macros.
// ------------------------------------------------------------------------------------------

//! Engine is being built as a desktop application.
#define GD_PLATFORM_DESKTOP 											Compile-Time Value

//! Engine is being built as a mobile application.
#define GD_PLATFORM_MOBILE 												Compile-Time Value

//! Engine is being built as a console application/game.
#define GD_PLATFORM_CONSOLE 											Compile-Time Value

//! Engine is being built as a webpage.
#define GD_PLATFORM_WEB													Compile-Time Value

// ------------------------------------------------------------------------------------------
// Platform API macros.
// ------------------------------------------------------------------------------------------

//! Engine is being built with a support of the Win32 API.
#define GD_PLATFORM_API_MICROSOFT 										Compile-Time Value

//! Engine is being built with a support of the Cocoa API.
#define GD_PLATFORM_API_COCOA 											Compile-Time Value

//! Engine is being built with a support of the Posix API.
#define GD_PLATFORM_API_POSIX 											Compile-Time Value

//! Engine is being built with a support of the LibSDL1 API.
#define GD_PLATFORM_API_LIBSDL1 										Compile-Time Value

//! Engine is being built with a support of the LibSDL2 API.
#define GD_PLATFORM_API_LIBSDL2 										Compile-Time Value

#endif	// if !GD_DOCUMENTATION
#pragma endregion Platform Determination

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Compiler Determination.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#pragma region Platform Determination
#if !GD_DOCUMENTATION

// ------------------------------------------------------------------------------------------
// Intel C++ Compiler. It actually masks into some other compiler, so processing it first.
// ------------------------------------------------------------------------------------------
#if (defined(__INTEL_COMPILER))
#	define GD_COMPILER_INTEL											GD_TRUE
#	if (defined(_MSC_VER))
#		define GD_COMPILER_MSVC_COMPATIBLE								GD_TRUE
#	elif (defined(__GNUC__))
#		define GD_COMPILER_GCC_COMPATIBLE								GD_TRUE
#	else	// *** Intel Compiler compatibility layer determination ***
#		error "Unable to determine Intel Compiler compatibilty layer."
#	endif	// *** Intel Compiler compatibility layer determination ***
#endif	// if (defined(__GNUC__))

// ------------------------------------------------------------------------------------------
// Microsoft Visual C++ compiler.
// ------------------------------------------------------------------------------------------
#if (defined(_MSC_VER)) && GD_PLATFORM_API_MICROSOFT
#	define GD_COMPILER_MSVC												GD_TRUE
#	define GD_COMPILER_MSVC_COMPATIBLE									GD_TRUE
#	if (_MSC_VER < 1800)												// 2013 CTP Compiler and above.
#		error "MSVC compiler version is lower than 1800. Please, use MSVC compiler version 2013 CTP and newer."
#	endif	// if (_MSC_VER >= 1800)
#endif	// if (defined(_MSC_VER)) && GD_PLATFORM_API_MICROSOFT

// ------------------------------------------------------------------------------------------
// Clang compiler.
// ------------------------------------------------------------------------------------------
#if (defined(__clang__))
#	define GD_COMPILER_CLANG											GD_TRUE
#	define GD_COMPILER_GCC_COMPATIBLE									GD_TRUE
#endif	// if (defined(__GNUC__))

// ------------------------------------------------------------------------------------------
// Generally-Not-Usable Compiler Collection.
// ------------------------------------------------------------------------------------------
#if (defined(__GNUC__)) && (!GD_COMPILER_CLANG)
#	define GD_COMPILER_GCC												GD_TRUE
#	define GD_COMPILER_GCC_COMPATIBLE									GD_TRUE
#endif	// if (defined(__GNUC__))

// Checking if there is actually a single compiler selected.
#if (!defined(GD_COMPILER_INTEL))
#	define GD_COMPILER_INTEL 											GD_FALSE
#endif	// if (!defined(GD_COMPILER_INTEL))
#if (!defined(GD_COMPILER_MSVC))
#	define GD_COMPILER_MSVC 											GD_FALSE
#endif	// if (!defined(GD_COMPILER_MSVC))
#if (!defined(GD_COMPILER_CLANG))
#	define GD_COMPILER_CLANG 											GD_FALSE
#endif	// if (!defined(GD_COMPILER_CLANG))
#if (!defined(GD_COMPILER_GCC))
#	define GD_COMPILER_GCC												GD_FALSE
#endif	// if (!defined(GD_COMPILER_GCC))

#if (!(0 \
	^ GD_COMPILER_INTEL \
	^ GD_COMPILER_MSVC \
	^ GD_COMPILER_CLANG \
	^ GD_COMPILER_GCC))
#	error "No known compiler was selected OR multiple compilers were detected."
#endif	// *** All default compilers XORed. ***

// Checking if there is actually a single compiler compatibility layer selected.
#if (!defined(GD_COMPILER_MSVC_COMPATIBLE))
#	define GD_COMPILER_MSVC_COMPATIBLE									GD_FALSE
#endif	// if (!defined(GD_COMPILER_MSVC_COMPATIBLE))
#if (!defined(GD_COMPILER_GCC_COMPATIBLE))
#	define GD_COMPILER_GCC_COMPATIBLE									GD_FALSE
#endif	// if (!defined(GD_COMPILER_GCC_COMPATIBLE))
#if (!(0 \
	^ GD_COMPILER_MSVC_COMPATIBLE \
	^ GD_COMPILER_GCC_COMPATIBLE))
#	error "No compiler compatibility layer specified."
#endif	// if (!(defined(GD_COMPILER_MSVC_COMPATIBLE) ^ defined(GD_COMPILER_GCC_COMPATIBLE)))

#else	// if !GD_DOCUMENTATION

// ------------------------------------------------------------------------------------------
// Compiler macros.
// ------------------------------------------------------------------------------------------

//! Engine is being built with a Intel C++ compiler.
#define GD_COMPILER_INTEL 												Compile-Time Value

//! Engine is being built with a Microsoft Visual C++ compiler.
#define GD_COMPILER_MSVC 												Compile-Time Value

//! Engine is being built with a Clang compiler.
#define GD_COMPILER_CLANG 												Compile-Time Value

//! Engine is being built with a GCC compiler.
#define GD_COMPILER_GCC													Compile-Time Value

// ------------------------------------------------------------------------------------------
// Compiler compatibility layer macros.
// ------------------------------------------------------------------------------------------

//! Engine is being built with a MSVC-compatible compiler.
#define GD_COMPILER_MSVC_COMPATIBLE										Compile-Time Value

//! Engine is being built with a GCC-compatible compiler.
#define GD_COMPILER_GCC_COMPATIBLE										Compile-Time Value

#endif	// if !GD_DOCUMENTATION
#pragma endregion Platform Determination

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Architecture Determination.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#pragma region Architecture Determination
#if !GD_DOCUMENTATION

// http://sourceforge.net/p/predef/wiki/Architectures/

#if GD_PLATFORM_HTML5
#	define GD_ARCHITECTURE_X86											GD_TRUE
#	define GD_ARCHITECTURE_ASMJS	    								GD_TRUE
#	define GD_ENDIANITY_LITTLE		    								GD_TRUE
#else	// if (defined(GD_PLATFORM_HTML5))
#	if GD_COMPILER_MSVC_COMPATIBLE
//! @todo Check for ARM64 processors in MSVC Compiler.
#		if (defined(_M_ARM))
#			define GD_ARCHITECTURE_ARM32								GD_TRUE
#			define GD_ENDIANITY_LITTLE									GD_TRUE
#		elif (defined(_M_X64))
#			define GD_ARCHITECTURE_X64									GD_TRUE
#			define GD_ENDIANITY_LITTLE									GD_TRUE
#		elif (defined(_M_IX86))
#			define GD_ARCHITECTURE_X86									GD_TRUE
#			define GD_ENDIANITY_LITTLE									GD_TRUE
#		else	// *** MSVC Architecture selection. ***
#			error "Unable to determine MSVC compilers target architecture."
#		endif	// *** MSVC Architecture selection. ***
#	elif GD_COMPILER_GCC_COMPATIBLE
#		if (defined(__aarch64__))
#			define GD_ARCHITECTURE_ARM64								GD_TRUE
#			define GD_ENDIANITY_LITTLE									GD_TRUE
#		elif (defined(__arm__))
#			define GD_ARCHITECTURE_ARM32								GD_TRUE
#			define GD_ENDIANITY_LITTLE									GD_TRUE
#		elif (defined(__amd64__))
#			define GD_ARCHITECTURE_X64									GD_TRUE
#			define GD_ENDIANITY_LITTLE									GD_TRUE
#		elif (defined(__i386__))
#			define GD_ARCHITECTURE_X86									GD_TRUE
#			define GD_ENDIANITY_LITTLE									GD_TRUE
#		else	// *** GCC Architecture selection. ***
#			error Unable to determine GCC compilers target architecture.
#		endif	// *** GCC Architecture selection. ***
#	endif	// *** Target architecture decls. ***
#endif	// if (defined(GD_PLATFORM_HTML5))

// Checking if there is actually a single CPU architecture selected.
#if (!defined(GD_ARCHITECTURE_X86))
#	define GD_ARCHITECTURE_X86 											GD_FALSE
#endif	// if (!defined(GD_ARCHITECTURE_X86))
#if (!defined(GD_ARCHITECTURE_X64))
#	define GD_ARCHITECTURE_X64  										GD_FALSE
#endif	// if (!defined(GD_ARCHITECTURE_X64))
#if (!defined(GD_ARCHITECTURE_ARM32))
#	define GD_ARCHITECTURE_ARM32  										GD_FALSE
#endif	// if (!defined(GD_ARCHITECTURE_ARM32))
#if (!defined(GD_ARCHITECTURE_ARM64))
#	define GD_ARCHITECTURE_ARM64  										GD_FALSE
#endif	// if (!defined(GD_ARCHITECTURE_ARM64))
#if (!(0 \
	^ GD_ARCHITECTURE_X86 \
	^ GD_ARCHITECTURE_X64 \
	^ GD_ARCHITECTURE_ARM32 \
	^ GD_ARCHITECTURE_ARM64))
#	error "No target CPU architecture was selected."
#endif	// *** All target architectures xored. ***

// Checking if there is actually a single CPU endianity selected.
#if (!defined(GD_ENDIANITY_BIG))
#	define GD_ENDIANITY_BIG  											GD_FALSE
#endif	// if (!defined(GD_ENDIANITY_BIG))
#if (!defined(GD_ENDIANITY_LITTLE))
#	define GD_ENDIANITY_LITTLE  										GD_FALSE
#endif	// if (!defined(GD_ENDIANITY_LITTLE))
#if (!(0 \
	^ GD_ENDIANITY_BIG \
	^ GD_ENDIANITY_LITTLE))
#	error "No target CPU endianity was selected."
#endif	// *** Big & Little endians xored. ***

// ------------------------------------------------------------------------------------------
// Compiler setup.
// ------------------------------------------------------------------------------------------
#if GD_COMPILER_MSVC_COMPATIBLE											//	Checking if C++ RTTI is disabled...
#	if (defined(__cplusplus_cli))										//	CLI requires RTTI been enabled...
#		if (!defined(_CPP_RTTI))
#			error "Standart C++/CLI RTTI should be enabled in MSVC compiler."
#		endif	// if (defined(_CPP_RTTI))
#	else	// if (defined(__cplusplus_cli))
#		if (defined(_CPP_RTTI))
#			error "Standart C++ RTTI should be disabled in MSVC compiler."
#		endif	// if (defined(_CPP_RTTI))
#	endif	// if (defined(__cplusplus_cli))
#elif GD_COMPILER_GCC_COMPATIBLE && 0
#	if (defined(__GXX_RTTI))
#		error "Standart C++ RTTI should be disabled in GCC compiler."
#	endif	// if (defined(__GXX_RTTI))
#endif	// *** C++ RTTI check. ***
#if GD_COMPILER_MSVC_COMPATIBLE											//	... And exceptions are enabled.
#	if (!(defined(_CPPUNWIND)))
#		error "Exceptions should be enabled in MSVC compiler."
#	endif	// if (!(defined(_CPPUNWIND)))
#elif GD_COMPILER_GCC_COMPATIBLE && 0
#	if !__EXCEPTIONS
#		error "Exceptions should be enabled in GCC compiler."
#	endif	// if (!(defined(__EXCEPTIONS)))
#endif	// *** C++ exceptions check. ***

#else	// if !GD_DOCUMENTATION

// ------------------------------------------------------------------------------------------
// Architecture macros.
// ------------------------------------------------------------------------------------------

//! Engine is being built to a x86 architecture.
#define GD_ARCHITECTURE_X86 											Compile-Time Value

//! Engine is being built to a x64 architecture.
#define GD_ARCHITECTURE_X64  											Compile-Time Value

//! Engine is being built to an ARM architecture.
#define GD_ARCHITECTURE_ARM32  											Compile-Time Value

//! Engine is being built to an ARM64 architecture.
#define GD_ARCHITECTURE_ARM64  											Compile-Time Value

// ------------------------------------------------------------------------------------------
// Architecture Endianity macros.
// ------------------------------------------------------------------------------------------

//! Engine is being built to a Big-Endian architecture.
#define GD_ENDIANITY_BIG  												Compile-Time Value

//! Engine is being built to a Little-Endian architecture.
#define GD_ENDIANITY_LITTLE				  								Compile-Time Value

#endif	// if !GD_DOCUMENTATION
#pragma endregion Platform Determination

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Target Configuration Determination.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#pragma region Target Configuration Determination
#if !GD_DOCUMENTATION

// If any configuration was predefined..
#if !(defined(GD_DEBUG) || defined(GD_TESTING) || defined(GD_DEVELOPMENT)  || defined(GD_RELEASE))
#	if GD_COMPILER_MSVC_COMPATIBLE
#		if (defined(_DEBUG))
#			define GD_DEBUG												GD_TRUE
#		elif (defined(NDEBUG))
#			define GD_RELEASE											GD_TRUE
#		else	// *** MSVC Debug specifications. ***
#			error "Unable to determine MSVC build configuration."
#		endif	// *** MSVC Debug specifications. ***
#	elif GD_COMPILER_GCC_COMPATIBLE
#		if (defined(__OPTIMIZE__))
#			define GD_RELEASE											GD_TRUE
#		else	// if (defined(__OPTIMIZE__))
#			define GD_DEBUG												GD_TRUE
#		endif	// if (defined(__OPTIMIZE__))
#	endif	// *** Deprecation specifications. ***
#endif	// if !(defined(GD_DEBUG) || defined(GD_TESTING) || defined(GD_DEVELOPMENT)  || defined(GD_RELEASE))

// Checking if there is actually a single target configuration selected.
#if (!defined(GD_DEBUG))
#	define GD_DEBUG														GD_FALSE
#endif	// if (!defined(GD_DEBUG))
#if (!defined(GD_TESTING))
#	define GD_TESTING													GD_FALSE
#endif	// if (!defined(GD_DEBUG))
#if (!defined(GD_DEVELOPMENT))
#	define GD_DEVELOPMENT												GD_FALSE
#endif	// if (!defined(GD_DEBUG))
#if (!defined(GD_RELEASE))
#	define GD_RELEASE													GD_FALSE
#endif	// if (!defined(GD_RELEASE))
#if (!(0 \
	^ GD_DEBUG \
	^ GD_TESTING \
	^ GD_DEVELOPMENT \
	^ GD_RELEASE))
#	error "Failed to determine target configuration."
#endif	// *** Debug & Release confirations XORed. ***

#else	// if !GD_DOCUMENTATION

//! Engine is being built in the Debug configuration.
#define GD_DEBUG														Compile-Time Value

//! Engine is being built for Testing.
#define GD_TESTING														Compile-Time Value

//! Engine is being built in the Development configuration.
#define GD_DEVELOPMENT													Compile-Time Value

//! Engine is being built in the Release configuration.
#define GD_RELEASE														Compile-Time Value

#endif	// if !GD_DOCUMENTATION
#pragma endregion Target Configuration Determination

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Compiler directives wrapping and etc.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#pragma region Compiler Directives

#if defined(__INTELLISENSE__)
#	define __attribute__(...)	// Wrapper to skip GCC directives while parsing with IntelliSense.
#endif	// if defined(__INTELLISENSE__)
#if (!defined(__has_builtin))
#	define __has_builtin(builtin) 0	// Compatibility with Clang...
#endif	// if (!defined(__has_builtin))
#if (!defined(__builtin_expect))
#	define __builtin_expect(expression, expected) (!!(expression) == expected)
#endif	// if (!defined(__builtin_expect))
#if !GD_COMPILER_MSVC_COMPATIBLE
#	define __analysis_assume(...)
#endif	// if !GD_COMPILER_MSVC_COMPATIBLE
#if (!defined(__COUNTER__))	// __COUNTER__ is required in several places.
#	error '__COUNTER__' macro is not defined. Check your compiler to support of it.
#endif	// if (!defined(__COUNTER__))
#if GD_COMPILER_GCC_COMPATIBLE	// __FUNCTION__ in GCC.
#	if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#		define __FUNCTION__ __func__
#	endif	// if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#endif	// if GD_COMPILER_GCC_COMPATIBLE

#if !GD_DOCUMENTATION
#	if GD_COMPILER_MSVC_COMPATIBLE
#		define GD_COMPILER_MSVC_COMPATIBLE_CODE(...) __VA_ARGS__
#		define GD_COMPILER_GCC_COMPATIBLE_CODE(...)
#	elif GD_COMPILER_GCC_COMPATIBLE
#		define GD_COMPILER_MSVC_COMPATIBLE_CODE(...) 
#		define GD_COMPILER_GCC_COMPATIBLE_CODE(...) __VA_ARGS__
#	else	// *** Compiler compatibility layers. ***
#		error No compiler compatibility layer was selected.
#	endif	// *** Compiler compatibility layers. ***
#else	// if !GD_DOCUMENTATION
#	define GD_COMPILER_MSVC_COMPATIBLE_CODE(...) 
#	define GD_COMPILER_GCC_COMPATIBLE_CODE(...)
#endif	// if !GD_DOCUMENTATION

// ------------------------------------------------------------------------------------------
// Definition declaration specifications.
// ------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------
//! Definition would be aligned in MSVC-compatible notation (if is supported).
//! @param Alignment Alignment value.
#define GD_ALIGN_MSVC(Alignment) \
	GD_COMPILER_GCC_COMPATIBLE_CODE() \
	GD_COMPILER_MSVC_COMPATIBLE_CODE(__declspec(align(Alignment)))

// ------------------------------------------------------------------------------------------
//! Definition would be aligned in GCC-compatible notation (if is supported).
//! @param Alignment Alignment value.
#define GD_ALIGN_GCC(Alignment) \
	GD_COMPILER_GCC_COMPATIBLE_CODE(__attribute__((aligned(Alignment)))) \
	GD_COMPILER_MSVC_COMPATIBLE_CODE()

// ------------------------------------------------------------------------------------------
//! Definition would be local to the thread.
//! @todo Erase this when MSVC would support 'thread_local'.
#define GD_THREAD_LOCAL \
	GD_COMPILER_GCC_COMPATIBLE_CODE(thread_local) \
	GD_COMPILER_MSVC_COMPATIBLE_CODE(__declspec(thread)) 

// ------------------------------------------------------------------------------------------
//! Definition would be defined deprecated.
//! @param Text Deprecation notation message.
//! @todo Erase this when compiler would support attributes.
#define GD_DEPRECATED(Text) \
	GD_COMPILER_GCC_COMPATIBLE_CODE(__attribute__((deprecated(Text)))) \
	GD_COMPILER_MSVC_COMPATIBLE_CODE(__declspec(deprecated(Text)))

// ------------------------------------------------------------------------------------------
// Method declaration specifications.
// ------------------------------------------------------------------------------------------

#if GD_COMPILER_MSVC_COMPATIBLE
#	if (defined(_WINDLL) && (!defined(GD_DLL_IMPORT)))
#		define GD_EXPORT(...) __VA_ARGS__
#		define GD_IMPORT(...) 
#	else	// if (defined(_WINDLL) && (!defined(GD_DLL_IMPORT)))
#		define GD_EXPORT(...) 
#		define GD_IMPORT(...) __VA_ARGS__
#	endif	// if (defined(_WINDLL) && (!defined(GD_DLL_IMPORT)))
#endif	// if GD_COMPILER_MSVC_COMPATIBLE

// ------------------------------------------------------------------------------------------
//! Defines an API method.
#define GDAPI \
	GD_COMPILER_GCC_COMPATIBLE_CODE(__attribute__((visibility("default")))) \
	GD_COMPILER_MSVC_COMPATIBLE_CODE(GD_EXPORT(__declspec(dllexport)) GD_IMPORT(__declspec(dllimport)))

// ------------------------------------------------------------------------------------------
//! Defines a method that stays internal for the Assembly.
#define GDINT \
	GD_COMPILER_GCC_COMPATIBLE_CODE(__attribute__((visibility("protected")))) \
	GD_COMPILER_MSVC_COMPATIBLE_CODE(__declspec()) 

// ------------------------------------------------------------------------------------------
//! Defines a method that would be exported from the Assembly.
#define GDEXP \
	GD_COMPILER_GCC_COMPATIBLE_CODE(__attribute__((visibility("default")))) \
	GD_COMPILER_MSVC_COMPATIBLE_CODE(__declspec(dllexport)) 

// ------------------------------------------------------------------------------------------
//! Defines a method that would be imported to the Assembly.
#define GDIMP \
	GD_COMPILER_GCC_COMPATIBLE_CODE(__attribute__((visibility("default")))) \
	GD_COMPILER_MSVC_COMPATIBLE_CODE(__declspec(dllimport)) 

// ------------------------------------------------------------------------------------------
//! Define this to disable force inlining of methods.
#define GD_NO_FORCE_INLINE 0
#if GD_NO_FORCE_INLINE
#	define GD_FORCE_INLINE(...) inline
#else	// if GD_NO_FORCE_INLINE
#	define GD_FORCE_INLINE(...) __VA_ARGS__
#endif	// if GD_NO_FORCE_INLINE

// ------------------------------------------------------------------------------------------
//! Defines a method that forcedly inlined into code.
#define GDINL GD_FORCE_INLINE( \
	GD_COMPILER_GCC_COMPATIBLE_CODE(__attribute__((always_inline)) inline) \
	GD_COMPILER_MSVC_COMPATIBLE_CODE(__forceinline))

// ------------------------------------------------------------------------------------------
//! Defines a method that would terminate the application.
#define GD_NORETURN \
	GD_COMPILER_GCC_COMPATIBLE_CODE(__attribute__((noreturn))) \
	GD_COMPILER_MSVC_COMPATIBLE_CODE(__declspec(noreturn)) 

// ------------------------------------------------------------------------------------------
//! Defines a function that smell like printf. Does some real static analysis on GCC-compatible compilers.
//! @param StringIndex m_Index of a format-string parameter.
//! @param FirstToCheck m_Index of first format-value parameter.
#define GD_PRINTF_LIKE(StringIndex, FirstToCheck) \
	GD_COMPILER_GCC_COMPATIBLE(__attribute__((format(printf, StringIndex, FirstToCheck)))) \
	GD_COMPILER_MSVC_COMPATIBLE_CODE() 

// ------------------------------------------------------------------------------------------
//! Defines a method that would be using a 'Vector Calling Convention'.
//! @see http://blogs.msdn.com/b/vcblog/archive/2013/07/12/introducing-vector-calling-convention.aspx
#define GD_VECTORCALL \
	GD_COMPILER_GCC_COMPATIBLE_CODE() \
	GD_COMPILER_MSVC_COMPATIBLE_CODE(__vectorcall) 

#pragma endregion Compiler Directives

// Disabled warnings.
#if GD_COMPILER_MSVC_COMPATIBLE
#	if !GD_RELEASE
#		pragma warning(disable : 4189)	// 'identifier' : local variable is initialized but not referenced
#	endif	// if !GD_RELEASE
#	pragma warning(disable : 4100)	// unreferenced formal parameter. Actually, this should not be disabled.
#	pragma warning(disable : 4127)	// conditional expression is constant
//#	pragma warning(disable : 4189)
#	pragma warning(disable : 4201)	// nonstandard extension used : nameless struct/union
#	pragma warning(disable : 4297)
#	pragma warning(disable : 4291)
//#	pragma warning(disable : 4301)
#	pragma warning(disable : 4324)	// structure was padded due to __declspec(align())
#	pragma warning(disable : 4373)	// virtual function overrides '___', previous versions of the compiler did not override when parameters only differed by const/volatile qualifiers
//#	pragma warning(disable : 4456)
//#	pragma warning(disable : 4458)
#	pragma warning(disable : 4505)	// unreferenced local function has been RemoveFromSelfd. Actually, this should not be disabled.
#	pragma warning(disable : 4592)	// symbol will be dynamically initialized (implementation limitation).
#	pragma warning(disable : 4595)	// warning C4595: 'operator delete': non-member operator new or delete functions may not be declared inline.
#	pragma warning(disable : 4714)	// function '...' marked as __forceinline not inlined. Actually, this should not be disabled.
#	pragma warning(disable : 4840)	
#elif GD_COMPILER_GCC_COMPATIBLE
#endif	// *** Warnings. ***

// ------------------------------------------------------------------------------------------
// System includes.
// ------------------------------------------------------------------------------------------
#if !GD_RESOURCE_COMPILER
#	include <cinttypes>
#	include <cwctype>
#	include <cassert>
#	include <clocale>
#	include <cstring>
#	include <cstdint>
#	include <cstddef>
#	include <cstdlib>
#	include <cstdarg>
#	include <cstdio>
#	include <cctype>
#	include <cwchar>
#	include <cfloat>
#	include <math.h>
#	include <time.h>
#	include <new>
#endif	// if !GD_RESOURCE_COMPILER
