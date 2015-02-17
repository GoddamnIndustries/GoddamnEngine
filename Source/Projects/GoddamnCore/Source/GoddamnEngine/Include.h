/// ==========================================================================================
/// Include.h: Common definitions for all GoddamnEngine inner projects.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// 
/// ------------------------------------------------------------------------------------------
///	My life is just a collection of goals dubious reachability. This project is one of those.
/// Oleg Butakov (James Jhuighuy, the engine master).
/// ------------------------------------------------------------------------------------------
/// 
/// History:
///		* Long time ago in galaxy far away - Created by James Jhuighuy.
/// ==========================================================================================

#pragma once
#ifndef ___GODDAMN_ENGINE_INCLUDE___
#define ___GODDAMN_ENGINE_INCLUDE___

/// ==========================================================================================
/// Version definitions
/// ==========================================================================================

#define GD_ENGINE_NAME					L"Goddamn Engine"
#define GD_ENGINE_TITLE					L"Goddamn Engine"
#define GD_ENGINE_ASSEMBLY_COMPANY		L"(C) Goddamn Industries"
#define GD_ENGINE_ASSEMBLY_PRODUCT		L"Goddamn Engine"
#define GD_ENGINE_ASSEMBLY_COPYRIGHT	L"Copyright \0xA9 2011 - 2015"##GD_ENGINE_ASSEMBLY_COMPANY
#define GD_ENGINE_ASSEMBLY_TRADEMARK	L"Goddamn Engine"
#define GD_ENGINE_ASSEMBLY_GUID			L"D2D6C4B5-41CD-4986-8CAA-538591C82A0C"
#define GD_ENGINE_ASSEMBLY_VERSION		L"1.0.0.0"
#define GD_ENGINE_ASSEMBLY_FILEVERSION	L"1.0.0.0"

// Code below should be stripped be the resource compiler.
#if (!defined(RC_INVOKED))

/// ==========================================================================================
/// Platform Determination.
/// Supported platforms:
///		@li Windows Desktop (Tested on 8.1) defined as GD_PLATFORM_WINDOWS.
/// Planned to support:
///		@li Microsoft Windows Store Application (Under construction) defined as GD_PLATFORM_WINDOWS.
///		@li Microsoft Windows Phone 8 (Under construction) defined as GD_PLATFORM_WINDOWS_PHONE8.
///		@li Microsoft Windows RT Application (Under construction) defined as GD_PLATFORM_WINDOWS_RT.
///		@li Microsoft XBox One Game (Under construction) defined as GD_PLATFORM_XBOX_ONE.
///		@li Apple OS X (Maverics and newer, under construction) defined as GD_PLATFORM_OS_X.
///		@li Apple iOS (8 and newer, under construction) defined as GD_PLATFORM_IOS.
///		@li Sony PlayStation 4 (Under construction) defined as GD_PLATFORM_PLAYSTATION4.
///		@li Google Android (Under construction) defined as GD_PLATFORM_ANDROID.
///		@li GNU/Linux including Valve SteamBox (Under construction) defined as GD_PLATFORM_GNU_LINUX.
///		@li HTML5/Asm.js/Emscripten (Under construction) defined as GD_PLATFORM_HTML5.
/// Thanks to this (http://sourceforge.net/p/predef/wiki/OperatingSystems/) article.
/// ==========================================================================================

/// ------------------------------------------------------------------------------------------
/// Microsoft Windows (R) Platforms family.
/// ------------------------------------------------------------------------------------------

#if (defined(_WIN32)) && (!defined(GD_PLATFORM_NOT_WINDOWS))
#	define _CRT_SECURE_NO_WARNINGS		  1
#	define _USE_MATH_DEFINES    		  1								// <| M_PI and others
#	define  WIN32_LEAN_AND_MEAN			  1								// <| We do not need whole WinAPI header.
#	define  VC_EXTRALEAN				  1								//  |
#	include <winapifamily.h>
#	define GD_PLATFORM_API_WINAPI		  1  							///< Obviously Windows platforms run on WinAPI.
#	if (WINAPI_FAMILY_PARTITION(WINAPI_FAMILY_DESKTOP_APP))				//   Windows Desktop application.
#		define GD_PLATFORM_API_LIBSDL2	  1  							///< LibSDL2 supports target platform.
#		define GD_PLATFORM_API_LIBSDL1	  1  							///< LibSDL1 supports target platform.
#		define GD_PLATFORM_WINDOWS		  1  							///< Macro to detect Windows-specific code
#		define GD_PLATFORM_DESKTOP		  1  			  				///< Building for desktop platform 
#	elif (WINAPI_FAMILY_PARTITION(WINAPI_FAMILY_PHONE_APP))				//   Windows Phone 8 application.
#		define GD_PLATFORM_WINDOWS_PHONE8 1  							///< Macro to detect WindowsPhone 8-specific code
#		define GD_PLATFORM_MOBILE		  1  							///< Building for mobile platform.
#	elif (WINAPI_FAMILY_PARTITION(WINAPI_FAMILY_PC_APP))				//   Windows Store application (Windows Desktop or Windows RT)
#		if (defined(__WRL_WINRT_STRICT__))								//   Windows RT application.
#			define GD_PLATFORM_WINDOWS_RT 1  							///< Macro to detect Windows RT-specific code
#			define GD_PLATFORM_MOBILE	  1  							///< Building for mobile platform 
#		else	// if (defined(__WRL_WINRT_STRICT__))					//   Windows Desktop application.
#			define GD_PLATFORM_WINDOWS	  1  							///< Macro to detect Windows-specific code
#			define GD_PLATFORM_DESKTOP	  1  			  				///< Building for desktop platform 
#		endif	// if (defined(__WRL_WINRT_STRICT__))
#	else	// elif (WINAPI_FAMILY_PARTITION(WINAPI_FAMILY_***))
#		error "Unsupported Windows target platform."
#	endif	// elif (WINAPI_FAMILY_PARTITION(WINAPI_FAMILY_***))
#endif	// if (defined(_WIN32)) && (!defined(GD_PLATFORM_NOT_WINDOWS))

/// ------------------------------------------------------------------------------------------
/// Microsoft XBox One Platform family.
/// ------------------------------------------------------------------------------------------

/// @todo Identify XBox One here.
#if (defined(___SOME_VERY_SPECIFIC_XBOX_ONE_MACRO___))
#	define GD_PLATFORM_API_WINAPI		  1  							///< Obviously Windows platforms run on WinAPI.
#	define GD_PLATFORM_XBOX_ONE			  1  							///< Building the XBox One game.
#	define GD_PLATFORM_CONSOLE			  1  							///< Building the console game.
#endif	// if (defined(___SOME_VERY_SPECIFIC_XBOX_ONE_MACRO___))

/// ------------------------------------------------------------------------------------------
/// Apple (R) Platforms family.
/// ------------------------------------------------------------------------------------------

#if (defined(__APPLE__))
#	include <TargetConditionals.h>
#	define GD_PLATFORM_API_COCOA		  1  							///< Apple operating systems use Cocoa API.
#	define GD_PLATFORM_API_POSIX		  1  							///< And also are POSIX-compatible.
#	define GD_PLATFORM_API_LIBSDL2		  1  							///< LibSDL2 supports target platform.
#	define GD_PLATFORM_API_LIBSDL1		  1  							///< LibSDL1 supports target platform.
#	if (defined(TARGET_OS_MAC))
#		define GD_PLATFORM_DESKTOP		  1  							///< Building for desktop platform 
#		define GD_PLATFORM_OSX			  1  							///< OS X application.
#	elif defined(TARGET_IPAD_SIMULATOR) || defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IPHONE) || defined(TARGET_OS_IPAD)
#		define GD_PLATFORM_MOBILE		  1  							///< Building for mobile platform 
#		define GD_PLATFORM_IOS			  1  							///< iOS application.
#	else	// *** Apple Platform selection ***
#		error "Unsupported Apple target platform."
#	endif	// *** Apple Platform selection ***
#endif	// if (defined(__APPLE__))

/// ------------------------------------------------------------------------------------------
/// Sony PlayStation 4 platform family.
/// ------------------------------------------------------------------------------------------

/// @todo Identify Sony PlayStation 4 here.
#if (defined(___SOME_VERY_SPECIFIC_PLAYSTATION4_MACRO___))
#	define GD_PLATFORM_API_ORBIS		  1  							///< PlayStation 4 runs Orbis OS.
#	define GD_PLATFORM_API_POSIX		  1  							///< And also is POSIX-compatible.
#	define GD_PLATFORM_PLAYSTATION4		  1  							///< Building the XBox One game.
#	define GD_PLATFORM_CONSOLE			  1  							///< Building the console game.
#endif	// if (defined(___SOME_VERY_SPECIFIC_PLAY_STATION4_MACRO___))

/// ------------------------------------------------------------------------------------------
/// Google Android platforms family.
/// ------------------------------------------------------------------------------------------

#if (defined(__ANDROID__))
#	define GD_PLATFORM_MOBILE			  1  							///< Building for mobile platform 
#	define GD_PLATFORM_API_POSIX		  1  							///< Android runs on GNU/Linux which POSIX-compatible.
#	define GD_PLATFORM_API_LIBSDL2		  1  							///< LibSDL2 supports target platform.
#	define GD_PLATFORM_API_LIBSDL1		  1  							///< LibSDL1 supports target platform.
#	define GD_PLATFORM_ANDROID			  1  							///< Android application.
#endif	// if (defined(__ANDROID__))

/// ------------------------------------------------------------------------------------------
/// GNU/Linux platforms family.
/// ------------------------------------------------------------------------------------------

#if (defined(__linux__))
#	define GD_PLATFORM_DESKTOP			  1  			  		///< Building for desktop platform 
#	define GD_PLATFORM_API_POSIX		  1  					///< GNU/Linux is POSIX-compatible.
#	define GD_PLATFORM_API_LIBSDL2		  1  					///< LibSDL2 supports target platform.
#	define GD_PLATFORM_API_LIBSDL1		  1  					///< LibSDL1 supports target platform.
#	define GD_PLATFORM_GNU_LINUX		  1  					///< GNU/Linux application.
#endif	// if (defined(__linux__))

/// ------------------------------------------------------------------------------------------
/// HTML5/Asm.js/Emscripten platforms family.
/// ------------------------------------------------------------------------------------------

#if (defined(__EMSCRIPTEN__))
#	define GD_PLATFORM_WEB				  1  			  		///< Building a web-application. 
#	define GD_PLATFORM_HTML5			  1  					///< Building an HTML5 web-application.
#	define GD_PLATFORM_API_POSIX		  1  					///< Emscripten has a POSIX-compatible layer.
#	define GD_PLATFORM_API_LIBSDL1		  1  					///< LibSDL1 supports target platform.
#endif	// if (defined(__EMSCRIPTEN__))

/// Currently only HTML5 does not support multithreading.
#if (!defined(GD_PLATFORM_HTML5))
#	define GD_PLATFORM_HAS_MULTITHREADING 1
#else	// if (!defined(GD_PLATFORM_HTML5))
#	define GD_PLATFORM_HAS_MULTITHREADING 0  
#endif	// if (!defined(GD_PLATFORM_HTML5))

// Checking if there is actually a single platform selected.
#if (!defined(GD_PLATFORM_WINDOWS))
#	define GD_PLATFORM_WINDOWS 0
#endif	// if (!defined(GD_PLATFORM_WINDOWS))
#if (!defined(GD_PLATFORM_WINDOWS))
#	define GD_PLATFORM_WINDOWS_PHONE8 0
#endif	// if (!defined(GD_PLATFORM_WINDOWS_PHONE8))
#if (!defined(GD_PLATFORM_WINDOWS_RT))
#	define GD_PLATFORM_WINDOWS_RT 0
#endif	// if (!defined(GD_PLATFORM_WINDOWS_RT))
#if (!defined(GD_PLATFORM_XBOX_ONE))
#	define GD_PLATFORM_XBOX_ONE 0
#endif	// if (!defined(GD_PLATFORM_XBOX_ONE))
#if (!defined(GD_PLATFORM_OS_X))
#	define GD_PLATFORM_OS_X 0
#endif	// if (!defined(GD_PLATFORM_OS_X))
#if (!defined(GD_PLATFORM_IOS))
#	define GD_PLATFORM_IOS 0
#endif	// if (!defined(GD_PLATFORM_IOS))
#if (!defined(GD_PLATFORM_PLAYSTATION4))
#	define GD_PLATFORM_PLAYSTATION4 0
#endif	// if (!defined(GD_PLATFORM_PLAYSTATION4))
#if (!defined(GD_PLATFORM_ANDROID))
#	define GD_PLATFORM_ANDROID 0
#endif	// if (!defined(GD_PLATFORM_ANDROID))
#if (!defined(GD_PLATFORM_GNU_LINUX))
#	define GD_PLATFORM_GNU_LINUX 0
#endif	// if (!defined(GD_PLATFORM_GNU_LINUX))
#if (!defined(GD_PLATFORM_HTML5))
#	define GD_PLATFORM_HTML5 0
#endif	// if (!defined(GD_PLATFORM_HTML5))
#if (!(0 \
	^ GD_PLATFORM_WINDOWS \
	^ GD_PLATFORM_WINDOWS_PHONE8 \
	^ GD_PLATFORM_WINDOWS_RT \
	^ GD_PLATFORM_XBOX_ONE \
	^ GD_PLATFORM_OS_X \
	^ GD_PLATFORM_IOS \
	^ GD_PLATFORM_PLAYSTATION4 \
	^ GD_PLATFORM_ANDROID \
	^ GD_PLATFORM_GNU_LINUX \
	^ GD_PLATFORM_HTML5))
#	error "No target platform was selected OR multiple target platforms were selected."
#endif	// *** All platforms XORed. ***

// Checking if there is actually a single platform-family selected.
#if (!defined(GD_PLATFORM_WEB))
#	define GD_PLATFORM_WEB 0
#endif	// if (!defined(GD_PLATFORM_WEB))
#if (!defined(GD_PLATFORM_MOBILE))
#	define GD_PLATFORM_MOBILE 0
#endif	// if (!defined(GD_PLATFORM_MOBILE))
#if (!defined(GD_PLATFORM_DESKTOP))
#	define GD_PLATFORM_DESKTOP 0
#endif	// if (!defined(GD_PLATFORM_DESKTOP))
#if (!defined(GD_PLATFORM_CONSOLE))
#	define GD_PLATFORM_CONSOLE 0
#endif	// if (!defined(GD_PLATFORM_CONSOLE))
#if (!(0 \
	^ GD_PLATFORM_WEB \
	^ GD_PLATFORM_MOBILE \
	^ GD_PLATFORM_DESKTOP \
	^ GD_PLATFORM_CONSOLE))
#	error "No target platform family was selected OR multiple target platform families were selected."
#endif	// *** All platform families XORed ***

// Checking if there is actually a single platform API selected.
#if (!defined(GD_PLATFORM_API_WINAPI))
#	define GD_PLATFORM_API_WINAPI 0
#endif	// if (!defined(GD_PLATFORM_API_WINAPI))
#if (!defined(GD_PLATFORM_API_COCOA))
#	define GD_PLATFORM_API_COCOA 0
#endif	// if (!defined(GD_PLATFORM_API_WINAPI))
#if (!defined(GD_PLATFORM_API_POSIX))
#	define GD_PLATFORM_API_POSIX 0
#endif	// if (!defined(GD_PLATFORM_API_POSIX))
#if (!defined(GD_PLATFORM_API_LIBSDL1))
#	define GD_PLATFORM_API_LIBSDL1 0
#endif	// if (!defined(GD_PLATFORM_CONSOLE))
#if (!defined(GD_PLATFORM_API_LIBSDL2))
#	define GD_PLATFORM_API_LIBSDL2 0
#else	// if (!defined(GD_PLATFORM_CONSOLE))
// Disabling support of libSDL1 if newer major version is supported.
#	if GD_PLATFORM_API_LIBSDL1
#		undef GD_PLATFORM_API_LIBSDL1
#		define GD_PLATFORM_API_LIBSDL1 0
#	endif	// if GD_PLATFORM_API_LIBSDL1
#endif	// if (!defined(GD_PLATFORM_CONSOLE))
#if ((!(0 \
	^ GD_PLATFORM_API_WINAPI \
	^ GD_PLATFORM_API_COCOA \
	^ GD_PLATFORM_API_POSIX)) && (!(0 \
	^ GD_PLATFORM_API_LIBSDL1 \
	^ GD_PLATFORM_API_LIBSDL2)))
#	error "No target platform API was selected OR multiple target platform APIs were selected."
#endif	// *** All platform APIs XORed ***

/// ==========================================================================================
/// Compiler Determination.
/// Supported compilers:
///		@li Microsoft Visual C++ compiler version 2013 CTP and above - Windows and XBox One, defined as GD_COMPILER_MSVC.
///		@li Apple's Clang compiler - Apple platforms, HTML5, defined as GD_COMPILER_CLANG.
/// Planned to support:
///		@li GNU Compilers Collection's C++ compiler - Windows, GNU/Linux, Android, PlayStation 4, defined as GD_COMPILER_GCC.
///		@li Intel C Compiler - Windows, OS X and GNU/Linux, defined as GD_COMPILER_INTEL + default platform compiler.
/// ==========================================================================================

/// ------------------------------------------------------------------------------------------
/// Intel C++ Compiler. It actually masks into some other compiler, so processing it first.
/// ------------------------------------------------------------------------------------------

#if (defined(__INTEL_COMPILER))
#	define GD_COMPILER_INTEL				1   ///< Code for target platform is going to be compiled using GNU Compiler collection. 
#	if (defined(_MSC_VER))
#		define GD_COMPILER_MSVC_COMPATIBLE	1  	///< Target compiler is compatible with the MSVC compiler.
#	elif (defined(__GNUC__))
#		define GD_COMPILER_GCC_COMPATIBLE	1  	///< Target compiler is compatible with the GCC compiler.
#	else	// *** Intel Compiler compatibility layer determination ***
#		error "Unable to determine Intel Compiler compatibilty layer."
#	endif	// *** Intel Compiler compatibility layer determination ***
#endif	// if (defined(__GNUC__))

/// ------------------------------------------------------------------------------------------
/// Microsoft Visual C++ compiler.
/// ------------------------------------------------------------------------------------------

#if (defined(_MSC_VER)) && (!defined(GD_PLATFORM_NOT_WINDOWS))
#	if (_MSC_VER >= 1800)						//   2013 CTP Compiler and above.
#		define GD_COMPILER_MSVC				1  	///< Code for target platform is going to be compiled using MSVC.
#		define GD_COMPILER_MSVC_COMPATIBLE	1  	///< Target compiler is compatible with the MSVC compiler.
#	else	// if (_MSC_VER >= 1800)
#		error "MSVC compiler version is lower than 1800. Please, use MSVC compiler version 2013 CTP and newer."
#	endif	// if (_MSC_VER >= 1800)
#endif	// if (defined(_MSC_VER)) && (!defined(GD_PLATFORM_NOT_WINDOWS))

/// ------------------------------------------------------------------------------------------
/// Apple (R) Clang compiler.
/// ------------------------------------------------------------------------------------------

#if (defined(__clang__))
#	define GD_COMPILER_CLANG				1  	///< Code for target platform is going to be compiled using Clang compiler. 
#	define GD_COMPILER_GCC_COMPATIBLE		1  	///< Target compiler is compatible with the GCC compiler.
#endif	// if (defined(__GNUC__))

/// ------------------------------------------------------------------------------------------
/// Generally-Not-Usable Compiler Collection.
/// ------------------------------------------------------------------------------------------

#if (defined(__GNUC__)) && (!defined(GD_COMPILER_CLANG))
#	define GD_COMPILER_GCC					1   ///< Code for target platform is going to be compiled using GNU Compiler collection. 
#	define GD_COMPILER_GCC_COMPATIBLE		1  	///< Target compiler is compatible with the GCC compiler.
#endif	// if (defined(__GNUC__))

// Checking if there is actually a single compiler selected.
#if (!defined(GD_COMPILER_GCC))
#	define GD_COMPILER_GCC 0
#endif	// if (!defined(GD_COMPILER_GCC))
#if (!defined(GD_COMPILER_MSVC))
#	define GD_COMPILER_MSVC 0
#endif	// if (!defined(GD_COMPILER_MSVC))
#if (!defined(GD_COMPILER_CLANG))
#	define GD_COMPILER_CLANG 0
#endif	// if (!defined(GD_COMPILER_CLANG))
#if (!defined(GD_COMPILER_INTEL))
#	define GD_COMPILER_INTEL 0
#endif	// if (!defined(GD_COMPILER_INTEL))
#if (!(0 \
	^ GD_COMPILER_GCC \
	^ GD_COMPILER_MSVC \
	^ GD_COMPILER_CLANG \
	^ GD_COMPILER_INTEL))
#	error "No known compiler was selected OR multiple compilers were detected."
#endif	// *** All default compilers XORed. ***

// Checking if there is actually a single compiler compatibility layer selected.
#if (!defined(GD_COMPILER_MSVC_COMPATIBLE))
#	define GD_COMPILER_MSVC_COMPATIBLE 0
#endif	// if (!defined(GD_COMPILER_MSVC_COMPATIBLE))
#if (!defined(GD_COMPILER_GCC_COMPATIBLE))
#	define GD_COMPILER_GCC_COMPATIBLE 0
#endif	// if (!defined(GD_COMPILER_GCC_COMPATIBLE))
#if (!(0 \
	^ GD_COMPILER_MSVC_COMPATIBLE \
	^ GD_COMPILER_GCC_COMPATIBLE))
#	error "No compiler compatibility layer specified."
#endif	// if (!(defined(GD_COMPILER_MSVC_COMPATIBLE) ^ defined(GD_COMPILER_GCC_COMPATIBLE)))

/// ==========================================================================================
/// Compiler abstraction layer and etc.
/// ==========================================================================================

/// ------------------------------------------------------------------------------------------
/// Target architecture declarations.
/// Thanks to this (http://sourceforge.net/p/predef/wiki/Architectures/) article.
/// ------------------------------------------------------------------------------------------

#if (defined(GD_PLATFORM_HTML5))
#		define GD_ARCHITECTURE_X86		    1  					///< Target processor is simulated to be x86.
#		define GD_ARCHITECTURE_ASMJS	    1  					///< Target processor is JavaScript Virtual machine.
#		define GD_ENDIANITY_LITTLE		    1  					///< Target processor is little-endian.
#else	// if (defined(GD_PLATFORM_HTML5))
#	if GD_COMPILER_MSVC_COMPATIBLE
/// @todo Check for ARM64 processors in MSVC Compiler.
#		if (defined(_M_ARM))
#			define GD_ARCHITECTURE_ARM32	1  					///< Target processor is ARM-32 bits.
#			define GD_ENDIANITY_LITTLE		1  					///< Target processor is little-endian.
#		elif (defined(_M_X64))
#			define GD_ARCHITECTURE_X64		1  					///< Target processor is x64.
#			define GD_ENDIANITY_LITTLE		1  					///< Target processor is little-endian.
#		elif (defined(_M_IX86))
#			define GD_ARCHITECTURE_X86		1  					///< Target processor is x86.
#			define GD_ENDIANITY_LITTLE		1  					///< Target processor is little-endian.
#		else	// *** MSVC Architecture selection. ***
#			error "Unable to determine MSVC compilers target architecture."
#		endif	// *** MSVC Architecture selection. ***
#	elif GD_COMPILER_GCC_COMPATIBLE
#		if (defined(__aarch64__))
#			define GD_ARCHITECTURE_ARM64	1  					///< Target processor is ARM-64 bits.
#			define GD_ENDIANITY_LITTLE		1  					///< Target processor is little-endian.
#		elif (defined(__arm__))
#			define GD_ARCHITECTURE_ARM32	1  					///< Target processor is ARM-32 bits.
#			define GD_ENDIANITY_LITTLE		1  					///< Target processor is little-endian.
#		elif (defined(__amd64__))
#			define GD_ARCHITECTURE_X64		1  					///< Target processor is x64.
#			define GD_ENDIANITY_LITTLE		1  					///< Target processor is little-endian.
#		elif (defined(__i386__))
#			define GD_ARCHITECTURE_X86		1  					///< Target processor is x86.
#			define GD_ENDIANITY_LITTLE		1  					///< Target processor is little-endian.
#		else	// *** GCC Architecture selection. ***
#			error "Unable to determine GCC compilers target architecture."
#		endif	// *** GCC Architecture selection. ***
#	endif	// *** Target architecture decls. ***
#endif	// if (defined(GD_PLATFORM_HTML5))

// Checking if there is actually a single CPU architecture selected.
#if (!defined(GD_ARCHITECTURE_X86))
#	define GD_ARCHITECTURE_X86 0
#endif	// if (!defined(GD_ARCHITECTURE_X86))
#if (!defined(GD_ARCHITECTURE_X64))
#	define GD_ARCHITECTURE_X64 0
#endif	// if (!defined(GD_ARCHITECTURE_X64))
#if (!defined(GD_ARCHITECTURE_ARM32))
#	define GD_ARCHITECTURE_ARM32 0
#endif	// if (!defined(GD_ARCHITECTURE_ARM32))
#if (!defined(GD_ARCHITECTURE_ARM64))
#	define GD_ARCHITECTURE_ARM64 0
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
#	define GD_ENDIANITY_BIG 0
#endif	// if (!defined(GD_ENDIANITY_BIG))
#if (!defined(GD_ENDIANITY_LITTLE))
#	define GD_ENDIANITY_LITTLE 0
#endif	// if (!defined(GD_ENDIANITY_LITTLE))
#if (!(0 \
	^ GD_ENDIANITY_BIG \
	^ GD_ENDIANITY_LITTLE))
#	error "No target CPU endianity was selected."
#endif	// *** Big & Little endians xored. ***

/// ------------------------------------------------------------------------------------------
/// Export specifications and etc.
/// ------------------------------------------------------------------------------------------

// Exports...
#if GD_COMPILER_MSVC_COMPATIBLE
#	define GDINT __declspec(/*Nothing*/)						///< Assembly internal name.
#	define GDEXP __declspec(dllexport)							///< Method would be exported from Assembly.
#	define GDIMP __declspec(dllimport)							///< Method would be imported from external Assembly.
#	if (defined(_WINDLL) && (!defined(GD_DLL_IMPORT)))
#		define GDAPI GDEXP										///< Method would be exported/imported from DLL.
#	else	// if (defined(_WINDLL) && (!defined(GD_DLL_IMPORT)))
#		define GDAPI GDIMP										///< Method would be exported/imported from DLL.
#	endif	// if (defined(_WINDLL) && (!defined(GD_DLL_IMPORT)))
#elif GD_COMPILER_GCC_COMPATIBLE
#	define GDINT __attribute__((visibility("protected")))		///< Assembly internal name.
#	define GDAPI __attribute__((visibility("default")))			///< Method would be exported/imported from DLL.
#	define GDEXP GDAPI											///< Method would be exported/imported from DLL.
#	define GDIMP GDAPI											///< Method would be imported from external Assembly.
#endif	// *** Export specification. ***

/// @brief Define this to disable force inlining of methods.
#define GD_NO_FORCE_INLINE 0

// Inlining...
#if GD_NO_FORCE_INLINE
#	define GDINL inline											///< Inline method.
#else	// if (defined(GD_NO_FORCE_INLINE))
#	if GD_COMPILER_MSVC_COMPATIBLE
#		define GDINL __forceinline								///< Method would be forcedly inlined.
#	elif GD_COMPILER_GCC_COMPATIBLE
#		define GDINL __attribute__((always_inline)) inline		///< Method would be forcedly inlined.
#	endif	// *** Force inline specification. ***
#endif	// if GD_NO_FORCE_INLINE

// Alignment...
#if GD_COMPILER_MSVC_COMPATIBLE
#	define GD_ALIGN_MSVC(Alignment) __declspec(align(Alignment))		///< Class would be aligned in MSVS compatible notation.
#	define GD_ALIGN_GCC(Alignment)										///< Dummy class alignment  for abstraction.
#elif GD_COMPILER_GCC_COMPATIBLE
#	define GD_ALIGN_MSVC(Alignment)										///< Dummy class alignment  for abstraction.
#	define GD_ALIGN_GCC(Alignment) __attribute__((aligned(Alignment)))	///< Class would be aligned in MSVS compatible notation.
#endif	// *** Aligment specifications. ***

// Noreturn...
#if GD_COMPILER_MSVC_COMPATIBLE
#	define GD_NORETURN __declspec(noreturn)				///< Method would terminate the application.
#elif GD_COMPILER_GCC_COMPATIBLE
#	define GD_NORETURN __attribute__((noreturn))		///< Method would terminate the application.
#endif	// *** Noreturn specifications. ***

// Thread-local...
/// @todo Remove this when MSVC supports thread_local
#if GD_COMPILER_MSVC_COMPATIBLE
#	define GD_THREAD_LOCAL __declspec(thread)
#elif GD_COMPILER_GCC_COMPATIBLE
#	define GD_THREAD_LOCAL thread_local
#endif	// *** Thread-local specifications. ***

// Deprecation...
#if GD_COMPILER_MSVC_COMPATIBLE
#	define GD_DEPRECATED(Text) __declspec(deprecated(Text))			///< Following definitions is declared deprecated.
#elif GD_COMPILER_GCC_COMPATIBLE
#	define GD_DEPRECATED(Text) __attribute__((deprecated(Text)))	///< Following definitions is declared deprecated.
#endif	// *** Deprecation specifications. ***

// Printf-like functions...
#if GD_COMPILER_MSVC_COMPATIBLE
#	define GD_PRINTF_LIKE(StringIndex, FirstToCheck)	///< Following functions smells like printf. Does some real static analysis on GCC-compatible compilers.
#elif GD_COMPILER_GCC_COMPATIBLE
#	define GD_PRINTF_LIKE(StringIndex, FirstToCheck) __attribute__((format(printf, StringIndex, FirstToCheck)))	///< Following functions smells like printf. Does some real static analysis on GCC-compatible compilers.
#endif	// *** Printf-like functions. ***

// Vector calling convention...
#if GD_COMPILER_MSVC_COMPATIBLE
#	define GD_VECTORCALL __vectorcall	///< Per-register calling convection.
#else
#	define GD_VECTORCALL				///< Per-register calling convection.
#endif	// *** Vector calling convention. ***

// Compatibility with Clang...
#if (!defined(GD_DOCUMENTATION))
#	if (!defined(__has_builtin))
#		define __has_builtin(builtin) 0
#	endif	// if (!defined(__has_builtin))
#endif	// if (!defined(GD_DOCUMENTATION))

// // For future expansion...
// #if GD_COMPILER_MSVC_COMPATIBLE
// #elif GD_COMPILER_GCC_COMPATIBLE
// #endif	// *** Future expansion specifications. ***

/// ------------------------------------------------------------------------------------------
/// Compiler setup.
/// ------------------------------------------------------------------------------------------

// __COUNTER__ is required in several places.
#if (!defined(__COUNTER__))
#	error '__COUNTER__' macro is not defined. Check your compiler to support of it.
#endif	// if (!defined(__COUNTER__))

// __FUNCTION__ in GCC.
#if GD_COMPILER_GCC_COMPATIBLE
#	if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#		define __FUNCTION__ __func__
#	endif	// if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#endif	// if GD_COMPILER_GCC_COMPATIBLE

#if defined(__cplusplus)
//	Checking if C++ RTTI is disabled...
#	if GD_COMPILER_MSVC_COMPATIBLE
//		CLI requires RTTI been enabled...
#		if (defined(__cplusplus_cli))
#			if (!defined(_CPP_RTTI))
#				error "Standart C++/CLI RTTI should be enabled in MSVC compiler."
#			endif	// if (defined(_CPP_RTTI))
#		else	// if (defined(__cplusplus_cli))
#			if (defined(_CPP_RTTI))
#				error "Standart C++ RTTI should be disabled in MSVC compiler."
#			endif	// if (defined(_CPP_RTTI))
#		endif	// if (defined(__cplusplus_cli))
#	elif GD_COMPILER_GCC_COMPATIBLE
#		if (defined(__GXX_RTTI))
#			error "Standart C++ RTTI should be disabled in GCC compiler."
#		endif	// if (defined(__GXX_RTTI))
#	endif	// *** C++ RTTI check. ***

//	And exceptions are enabled.
#	if GD_COMPILER_MSVC_COMPATIBLE
#		if (!(defined(_CPPUNWIND)))
#			error "Exceptions should be enabled in MSVC compiler."
#		endif	// if (!(defined(_CPPUNWIND)))
#	elif GD_COMPILER_GCC_COMPATIBLE
#		if (!(defined(__EXCEPTIONS)))
#			error "Exceptions should be enabled in GCC compiler."
#		endif	// if (!(defined(__EXCEPTIONS)))
#	endif	// *** C++ exceptions check. ***
#endif	// if defined(__cplusplus)

// Disabled warnings.
#if GD_COMPILER_MSVC_COMPATIBLE
#	pragma warning(disable : 4100)	// unreferenced formal parameter. Actually, this should not be disabled.
#	pragma warning(disable : 4127)	// conditional expression is constant
//#	pragma warning(disable : 4189)
#	pragma warning(disable : 4201)	// nonstandard extension used : nameless struct/union
//#	pragma warning(disable : 4301)
#	pragma warning(disable : 4324)	// structure was padded due to __declspec(align())
//#	pragma warning(disable : 4456)
//#	pragma warning(disable : 4458)
#	pragma warning(disable : 4505)	// unreferenced local function has been removed. Actually, this should not be disabled.
#	pragma warning(disable : 4714)	// function '...' marked as __forceinline not inlined. Actually, this should not be disabled.
#elif GD_COMPILER_GCC_COMPATIBLE
#endif	// *** Warning. ***

/// ------------------------------------------------------------------------------------------
/// Target configuration.
/// ------------------------------------------------------------------------------------------

#if GD_COMPILER_MSVC_COMPATIBLE
#	if (defined(_DEBUG))
#		define GD_DEBUG		1  							///< Building a debug version of the engine.
#	elif (defined(NDEBUG))
#		define GD_RELEASE	1  							///< Building an optimized version of the engine.
#	else	// *** MSVC Debug specifications. ***
#		error "Unable to determine MSVC build configuration."
#	endif	// *** MSVC Debug specifications. ***
#elif GD_COMPILER_GCC_COMPATIBLE
#	if (defined(__OPTIMIZE__))
#		define GD_RELEASE	1  							///< Building an optimized version of the engine.
#	else	// if (defined(__OPTIMIZE__))
#		define GD_DEBUG		1  							///< Building a debug version of the engine.
#	endif	// if (defined(__OPTIMIZE__))
#endif	// *** Deprecation specifications. ***

#if (!defined(GD_RELEASE))
#	define GD_RELEASE 0
#endif	// if (!definedGD_RELEASE)
#if (!defined(GD_DEBUG))
#	define GD_DEBUG 0
#endif	// if (!definedGD_DEBUG)
#if (!(0 \
	^ GD_DEBUG \
	^ GD_RELEASE))
#	error "Failed to determine target configuration."
#endif	// *** Debug & Release confirations XORed. ***

/// ==========================================================================================
/// Most common definitions.
/// ==========================================================================================

#if defined(__cplusplus)
#	include <cstddef>
#	include <cstdint>
#	include <cmath>
#else	// if defined(__cplusplus)
#	include <stddef.h>
#	include <stdint.h>
#	include <math.h>
#endif	// if defined(__cplusplus)

/// ==========================================================================================
/// Engine namespaces.
/// ==========================================================================================

#if defined(__cplusplus)
/// Redefine this to move entire engine code to custom namespace.
#	define GD_NAMESPACE GoddamnEngine
#	if (defined(GD_NAMESPACE))
#		define GD_NAMESPACE_BEGIN namespace GD_NAMESPACE {		///< Declaration of the namespace where entire API is located.
#		define GD_NAMESPACE_END   }	// namespace GD_NAMESPACE  	///< End of main namespace declaration.
#		define GD				  ::GD_NAMESPACE
#		define GD_USING_NAMESPACE using namespace GD
#	else	// if (defined(GD_NAMESPACE))
#		define GD_NAMESPACE_BEGIN								///< Dummy for namespace begin.
#		define GD_NAMESPACE_END									///< Dummy for namespace end.
#		define GD::::
#		define GD_USING_NAMESPACE
#	endif	// if (defined(GD_NAMESPACE))
#endif	// if defined(__cplusplus)

/// ------------------------------------------------------------------------------------------
/// C++ syntax "improvements" & useful macros.
/// ------------------------------------------------------------------------------------------

#define GD_MONOLITHIC_ENGINE

#define fallthrough
#define abstract		= 0
#define impl_abstract	abstract
#define object_cast		static_cast
#define GD_NOT_USED(Argument) (static_cast<void>(Argument))

/// Some glue magic.
#define GD_GLUE_(A, B) A ## B
#define GD_GLUE(A, B) GD_GLUE_(A, B)

/// Some widening magic.
#define GD_WIDEN_(String) L ## String
#define GD_WIDEN(String) GD_WIDEN_(String)

#if defined(__cplusplus)
#	define GD_BIT(Bit)						(TypeTraits::RemoveAllModifiers<decltype((Bit))>::Type1   << (Bit))
#	define GD_CLASS_UNASSIGNABLE(Class)		private: GDINT Class& operator= (Class const&) = delete; \
											private: GDINT Class& operator= (Class  &&) = delete;
#	define GD_CLASS_UNSWAPPABLE(Class)		/// @todo: GCC does not compiles this: private: GDINT friend void Swap(Class&, Class&) = delete;
#	define GD_CLASS_UNCOPIABLE(Class)		private: GDINT Class(Class const&) = delete;
#	define GD_CLASS_UNMOVABLE(Class)		private: GDINT Class(Class  &&) = delete;
#endif	// if defined(__cplusplus)

/// ------------------------------------------------------------------------------------------
/// Array size.
/// ------------------------------------------------------------------------------------------

#if defined(__cplusplus)
GD_NAMESPACE_BEGIN
	template <typename Type, size_t const Count>
	GDINT char(&ArraySizeHelper(Type(&Array)[Count]))[Count];
GD_NAMESPACE_END
#	define GD_ARRAY_SIZE(Array) (sizeof(ArraySizeHelper(Array)))				///< Using safe macro for array length.
#else	// if defined(__cplusplus)
#	if GD_DEBUG
GDINL static size_t gd_array_size_failure() { GD_DEBUG_ASSERT_FALSE("Non-array was passed to 'GD_ARRAY_SIZE' macro."); }
///		Using safe macro for array length.
#		define GD_ARRAY_SIZE(Array) ((sizeof(Array) != sizeof(&Array[0])) ? (sizeof(Array) / sizeof(*Array)) : (SIZE_MAX + (0 & gd_array_size_failure())))
#	else	// if GD_DEBUG
#		define GD_ARRAY_SIZE(Array) (sizeof(Array) / sizeof(Array[0]))			///< Using UNSAFE macro for array length in C release code.
#	endif	// if GD_DEBUG
#endif	// if defined(__cplusplus)

/// ------------------------------------------------------------------------------------------
/// Basic types.
/// ------------------------------------------------------------------------------------------

#if defined(__cplusplus)
GD_NAMESPACE_BEGIN

	typedef decltype(nullptr) nullptr_t;
	using ::size_t;

	typedef ::uint8_t UInt8;
	typedef ::int8_t  Int8;
	static_assert((sizeof(UInt8) == sizeof(Int8)) && (sizeof(UInt8) == 1), "Error in 8-bit integer sizes");

	typedef ::uint16_t UInt16;
	typedef ::int16_t  Int16;
	static_assert((sizeof(UInt16) == sizeof(Int16)) && (sizeof(UInt16) == 2), "Error in 16-bit integer sizes");

	typedef ::uint32_t UInt32;
	typedef ::int32_t  Int32;
	static_assert((sizeof(UInt32) == sizeof(Int32)) && (sizeof(UInt32) == 4), "Error in 32-bit integer sizes");

	// In JavaScript we do not have any 64-bit integer types. 
//#if (!defined(GD_ARCHITECTURE_ASMJS))
	typedef ::uint64_t UInt64;
	typedef ::int64_t  Int64;
	static_assert((sizeof(UInt64) == sizeof(Int64)) && (sizeof(UInt64) == 8), "Error in 64-bit integer sizes");
//#endif	// if (!defined(GD_ARCHITECTURE_ASMJS))

	typedef UInt8 Float8[1];
	typedef UInt8 Float16[2];

	typedef float Float32;
	static_assert(sizeof(Float32) == 4, "Error in 32-bit float sizes");

	typedef double Float64;
	static_assert(sizeof(Float64) == 8, "Error in 64-bit float sizes");

	typedef char const* CStr;
	typedef void const* CHandle;
	typedef void      *  Handle;

GD_NAMESPACE_END
#	include <GoddamnEngine/Core/Diagnostics/Assertion/Assertion.h>
#	include <GoddamnEngine/Core/Allocator/Allocator.h>
#endif	// if defined(__cplusplus)

#endif	// if (defined(RC_INVOKED))
#endif	// ifndef ___GODDAMN_ENGINE_INCLUDE___
