/// ==========================================================================================
/// Include.h: Common definitions for all GoddamnEngine inner projects.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// ------------------------------------------------------------------------------------------
///	My life is just a collection of goals dubious reachability. This project is one of them.
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
#define ENGINE				GoddamnEngine	
#define ENGINE_VERSION		0x06000001ui32
#define ENGINE_VERSION_FULL	"6.0.0.1"

#define GD_ENGINE_NAME		"GoddamnEngine"
#define GD_ENGINE_VERSION	"1/2"
/// ==========================================================================================

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
///		@li GNU/Linux including Valve Steambox (Under construction) defined as GD_PLATFORM_GNU_LINUX.
///		@li HTML5/Asm.js/Emscripten (Under construction) defined as GD_PLATFORM_HTML5.
/// Thanks to this (http://sourceforge.net/p/predef/wiki/OperatingSystems/) article.
/// ==========================================================================================

/// ------------------------------------------------------------------------------------------
/// Microsoft Windows (R) Platforms family.
/// ------------------------------------------------------------------------------------------

#if (defined(_WIN32))
#	define _USE_MATH_DEFINES       // <| M_PI and others
#	define _CRT_SECURE_NO_WARNINGS // <| Using printf and others
#	define  WIN32_LEAN_AND_MEAN    // <| We do not need whole WinAPI header.
#	define  VC_EXTRALEAN		   //  |
#	include <winapifamily.h>
#	define GD_PLATFORM_API_WINAPI		  (1)							///< Obviously Windows platforms run on WinAPI.
#	if (WINAPI_FAMILY_PARTITION(WINAPI_FAMILY_DESKTOP_APP))				//   Windows Desktop application.
#		define GD_PLATFORM_API_LIBSDL2	  (1)							///< LibSDL2 supports target platform.
#		define GD_PLATFORM_WINDOWS		  (1)							///< Macro to detect Windows-specific code
#		define GD_PLATFORM_DESKTOP		  (1)			        		///< Building for desktop platform 
//  endif	// if (WINAPI_FAMILY_PARTITION(WINAPI_FAMILY_DESKTOP_APP))
#	elif (WINAPI_FAMILY_PARTITION(WINAPI_FAMILY_PHONE_APP))				//   Windows Phone 8 application.
#		define GD_PLATFORM_WINDOWS_PHONE8 (1)							///< Macro to detect WindowsPhone 8-specific code
#		define GD_PLATFORM_MOBILE		  (1)							///< Building for mobile platform.
#		error "Building for this platform is not implemented."
//	endif	// elif (WINAPI_FAMILY_PARTITION(WINAPI_FAMILY_PHONE_APP))
#	elif (WINAPI_FAMILY_PARTITION(WINAPI_FAMILY_PC_APP))				//   Windows Store application (Windows Desktop or Windows RT)
#		if (defined(__WRL_WINRT_STRICT__))								//   Windows RT application.
#			define GD_PLATFORM_WINDOWS_RT (1)							///< Macro to detect Windows RT-specific code
#			define GD_PLATFORM_MOBILE	  (1)							///< Building for mobile platform 
#			error "Building for this platform is not implemented."
#		else	// if (defined(__WRL_WINRT_STRICT__))					//   Windows Desktop application.
#			define GD_PLATFORM_WINDOWS	  (1)							///< Macro to detect Windows-specific code
#			define GD_PLATFORM_DESKTOP	  (1)			        		///< Building for desktop platform 
#			error "Building for this platform is not implemented."
#		endif	// if (defined(__WRL_WINRT_STRICT__))
//	endif	// elif (WINAPI_FAMILY_PARTITION(WINAPI_FAMILY_DESKTOP_APP))
#	else	// elif (WINAPI_FAMILY_PARTITION(WINAPI_FAMILY_***))
#		error "Unsupported Windows target platform."
#	endif	// elif (WINAPI_FAMILY_PARTITION(WINAPI_FAMILY_***))
//endif	// if (defined(_WIN32))

/// ------------------------------------------------------------------------------------------
/// Microsoft XBox One Platform family.
/// ------------------------------------------------------------------------------------------

/// @todo Identify XBox One here.
#elif (defined(___SOME_VERY_SPECIFIC_XBOX_ONE_MACRO___))
#	define GD_PLATFORM_API_WINAPI		  (1)							///< Obviously Windows platforms run on WinAPI.
#	define GD_PLATFORM_XBOX_ONE			  (1)							///< Building the XBox One game.
#	define GD_PLATFORM_CONSOLE			  (1)							///< Building the console game.
#	error "Building for this platform is not implemented."
//endif	// if (defined(___SOME_VERY_SPECIFIC_XBOX_ONE_MACRO___))

/// ------------------------------------------------------------------------------------------
/// Apple (R) Platforms family.
/// ------------------------------------------------------------------------------------------

#elif (defined(__APPLE__))
#	include <TargetConditionals.h>
#	define GD_PLATFORM_API_COCOA		  (1)							///< Apple operating systems use Cocoa API.
#	define GD_PLATFORM_API_POSIX		  (1)							///< And also are POSIX-compatible.
#	define GD_PLATFORM_API_LIBSDL2		  (1)							///< LibSDL2 supports target platform.
#	if (defined(TARGET_OS_MAC))
#		define GD_PLATFORM_DESKTOP		  (1)			        		///< Building for desktop platform 
#		define GD_PLATFORM_OSX			  (1)							///< OS X application.
#		error "Building for this platform is not implemented."
//	elif	// if (defined(TARGET_OS_MAC))
#	elif (defined(TARGET_IPAD_SIMULATOR) || defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IPHONE) || defined(TARGET_OS_IPAD))
#		define GD_PLATFORM_MOBILE		  (1)							///< Building for mobile platform 
#		define GD_PLATFORM_IOS			  (1)							///< iOS application.
#		error "Building for this platform is not implemented."
//	endif	// elif (defined(TARGET_IPAD_SIMULATOR) || defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IPHONE) || defined(TARGET_OS_IPAD))
#	else	// *** Apple Platform selection ***
#		error "Unsupported Apple target platform."
#	endif	// *** Apple Platform selection ***
//endif	// if (defined(__APPLE__))

/// ------------------------------------------------------------------------------------------
/// Sony PlayStation 4 platform family.
/// ------------------------------------------------------------------------------------------

/// @todo Identify Sony PlayStation 4 here.
#elif (defined(___SOME_VERY_SPECIFIC_PLAYSTATION4_MACRO___))
#	define GD_PLATFORM_API_ORBIS		  (1)							///< PlayStation 4 runs orbis OS.
#	define GD_PLATFORM_API_POSIX		  (1)							///< And also is POSIX-compatible.
#	define GD_PLATFORM_PLAYSTATION4		  (1)							///< Building the XBox One game.
#	define GD_PLATFORM_CONSOLE			  (1)							///< Building the console game.
#	error "Building for this platform is not implemented."
//endif	// if (defined(___SOME_VERY_SPECIFIC_PLAY_STATION4_MACRO___))

/// ------------------------------------------------------------------------------------------
/// Google Android platforms family.
/// ------------------------------------------------------------------------------------------

#elif (defined(__ANDROID__))
#	define GD_PLATFORM_MOBILE			  (1)							///< Building for mobile platform 
#	define GD_PLATFORM_API_POSIX		  (1)							///< Android runs on GNU/Linux which POSIX-compatible.
#	define GD_PLATFORM_API_LIBSDL2		  (1)							///< LibSDL2 supports target platform.
#	define GD_PLATFORM_ANDROID			  (1)							///< Android application.
#	error "Building for this platform is not implemented."
//endif	// if (defined(__ANDROID__))

/// ------------------------------------------------------------------------------------------
/// GNU/Linux platforms family.
/// ------------------------------------------------------------------------------------------

#elif (defined(__linux__))
#	define GD_PLATFORM_DESKTOP			  (1)			        		///< Building for desktop platform 
#	define GD_PLATFORM_API_POSIX		  (1)							///< GNU/Linux is POSIX-compatible.
#	define GD_PLATFORM_API_LIBSDL2		  (1)							///< LibSDL2 supports target platform.
#	define GD_PLATFORM_GNU_LINUX		  (1)							///< GNU/Linux application.
#	error "Building for this platform is not implemented."
//endif	// if (defined(__linux__))

/// ------------------------------------------------------------------------------------------
/// HTML5/Asm.js/Emscripten platforms family.
/// ------------------------------------------------------------------------------------------

#elif (defined(__EMSCRIPTEN__))
#	define GD_PLATFORM_WEB				  (1)			        		///< Building a web-application. 
#	define GD_PLATFORM_HTML5			  (1)							///< Building an HTML5 web-application.
#	error "Building for this platform is not implemented."
//endif	// if (defined(__EMSCRIPTEN__))

/// ------------------------------------------------------------------------------------------
/// Some unknown platform.
/// ------------------------------------------------------------------------------------------

#else	// *** Target platform family determination. ***
#	error "Unsupported engine's target platform family."
#endif	// *** Target platform family determination. ***

// Checking if there is actualy a single platform selected.
#if (!(0 \
	^ defined(GD_PLATFORM_WINDOWS) \
	^ defined(GD_PLATFORM_WINDOWS_PHONE8) \
	^ defined(GD_PLATFORM_WINDOWS_RT) \
	^ defined(GD_PLATFORM_XBOX_ONE) \
	^ defined(GD_PLATFORM_OS_X) \
	^ defined(GD_PLATFORM_IOS) \
	^ defined(GD_PLATFORM_PLAYSTATION4) \
	^ defined(GD_PLATFORM_ANDROID) \
	^ defined(GD_PLATFORM_GNU_LINUX) \
	^ defined(GD_PLATFORM_HTML5)))
#	error "No target platform was selected OR multiple target platforms were selected."
#endif	// *** All platforms XORed. ***
#if (!(0 \
	^ defined(GD_PLATFORM_WEB) \
	^ defined(GD_PLATFORM_MOBILE) \
	^ defined(GD_PLATFORM_DESKTOP) \
	^ defined(GD_PLATFORM_CONSOLE)))
#	error "No target platform family was selected OR multiple target platform families were selected."
#endif	// *** All platform familis XORed ***

/// ==========================================================================================
/// Compiler Determination.
/// Supported compilers:
///		@li Microsoft Visual C++ compiler version 2012 CTP and above - Windowses and XBox One, defined as GD_COMPILER_MSVC.
/// Planned to support:
///		@li Apple's Clang compiler - Apple platforms, HTML5, defined as GD_COMPILER_CLANG.
///		@li GNU Compilers Collections's C++ compiler - Windows, GNU/Linux, Android, PlayStation 4, defined as GD_COMPILER_GCC.
///		@li Intel C Compiler - Windows, OS X and GNU/Linux, defined as GD_COMPILER_INTEL + default platform compiler.
/// ==========================================================================================

/// ------------------------------------------------------------------------------------------
/// Intel C++ Compiler. It actually masks into some other compiler, so processing it first.
/// ------------------------------------------------------------------------------------------

#if (defined(__INTEL_COMPILER))
#	define GD_COMPILER_INTEL				(1) ///< Code for target platform is going to be compiled using GNU Compiler collection. 
#	if (defined(_MSC_VER))
#		define GD_COMPILER_MSVC_COMPATIBLE	(1)	///< Target compiler is compatible with the MSVC compiler.
//	endif	// if (defined(_MSC_VER))
#	elif (defined(__GNUC__))
#		define GD_COMPILER_GCC_COMPATIBLE	(1)	///< Target compiler is compatible with the GCC compiler.
//	endif	// elif (defined(__GNUC__))
#	else	// *** Intel Compiler compatibility layer determination ***
#		error "Unable to determine Intel Compiler compatibilty layer."
#	endif	// *** Intel Compiler compatibility layer determination ***
//endif	// if (defined(__GNUC__))

/// ------------------------------------------------------------------------------------------
/// Microsoft Visual C++ compiler.
/// ------------------------------------------------------------------------------------------

#elif (defined(_MSC_VER))
#	if (_MSC_VER >= 1800)	// 2012 CTP Compiler and above.
#		define GD_COMPILER_MSVC				(1)	///< Code for target platform is going to be compiled using MSVC.
#		define GD_COMPILER_MSVC_COMPATIBLE	(1)	///< Target compiler is compatible with the MSVC compiler.
#	else	// if (_MSC_VER >= 1800)
#		error "MSVC compiler version is lower than 1800. Please, use MSVC compiler version 2012 CTP and newer."
#	endif	// if (_MSC_VER >= 1800)
//endif	// if (defined(_MSC_VER))

/// ------------------------------------------------------------------------------------------
/// Apple (R) Clang compiler.
/// ------------------------------------------------------------------------------------------

#elif (defined(__clang__))
#	define GD_COMPILER_GLANG				(1)	///< Code for target platform is going to be compiled using Clang compiler. 
#	define GD_COMPILER_GCC_COMPATIBLE		(1)	///< Target compiler is compatible with the GCC compiler.
//endif	// if (defined(__GNUC__))

/// ------------------------------------------------------------------------------------------
/// Generally-Not-Usable Compiler Collection.
/// ------------------------------------------------------------------------------------------

#elif (defined(__GNUC__))
#	define GD_COMPILER_GCC					(1) ///< Code for target platform is going to be compiled using GNU Compiler collection. 
#	define GD_COMPILER_GCC_COMPATIBLE		(1)	///< Target compiler is compatible with the GCC compiler.
//endif	// if (defined(__GNUC__))

/// ------------------------------------------------------------------------------------------
/// Some unknown C++ compiler.
/// ------------------------------------------------------------------------------------------

#else	// *** Compiler determination. ***
#	error "Unsupported engine's compiler."
#endif	// *** Compiler determination. ***

#if (!(0 \
	^ defined(GD_COMPILER_GCC) \
	^ defined(GD_COMPILER_MSVC) \
	^ defined(GD_COMPILER_CLANG) \
	^ defined(GD_COMPILER_INTEL)))
#	error "No known compiler was selected OR multiple compilers were detected."
#endif	// *** All default compilers XORed. ***
#if (!(0 \
	^ defined(GD_COMPILER_MSVC_COMPATIBLE) \
	^ defined(GD_COMPILER_GCC_COMPATIBLE)))
#	error "No compiler compatibilty layer specified."
#endif	// if (!(defined(GD_COMPILER_MSVC_COMPATIBLE) ^ defined(GD_COMPILER_GCC_COMPATIBLE)))

/// ==========================================================================================
/// Compiler abstraction layer and etc.
/// ==========================================================================================

/// ------------------------------------------------------------------------------------------
/// Target architecture decls.
/// Thanks to this (http://sourceforge.net/p/predef/wiki/Architectures/) article.
/// ------------------------------------------------------------------------------------------

#if (defined(GD_PLATFORM_HTML5))
#		define GD_ARCHITECTURE_X86		    (1)					///< Target processor is simulated to be x86.
#		define GD_ARCHITECTURE_ASMJS	    (1)					///< Target processor is JavaScript Virtual machine.
#		define GD_ENDIANITY_LITTLE		    (1)					///< Target processor is little-endian.
#else	// if (defined(GD_PLATFORM_HTML5))
#	if (defined(GD_COMPILER_MSVC_COMPATIBLE))
/// @todo Check for ARM64 processors in MSVC Compiler.
#		if (defined(_M_ARM))
#			define GD_ARCHITECTURE_ARM32	(1)					///< Target processor is ARM-32 bits.
#			define GD_ENDIANITY_LITTLE		(1)					///< Target processor is little-endian.
//		endif	// if (defined(_M_ARM))
#		elif (defined(_M_X64))
#			define GD_ARCHITECTURE_X64		(1)					///< Target processor is x64.
#			define GD_ENDIANITY_LITTLE		(1)					///< Target processor is little-endian.
//		endif	// if (defined(_M_X64))
#		elif (defined(_M_IX86))
#			define GD_ARCHITECTURE_X86		(1)					///< Target processor is x86.
#			define GD_ENDIANITY_LITTLE		(1)					///< Target processor is little-endian.
//		endif	// if (defined(_M_IX86))
#		else	// *** MSVC Architecture selection. ***
#			error "Unable to determine MSVC compilers target architecture."
#		endif	// *** MSVC Architecture selection. ***
//	endif	// if (defined(GD_COMPILER_MSVC_COMPATIBLE))
#	elif (defined(GD_COMPILER_GCC_COMPATIBLE))
#		if (defined(__aarch64__))
#			define GD_ARCHITECTURE_ARM64	(1)					///< Target processor is ARM-64 bits.
#			define GD_ENDIANITY_LITTLE		(1)					///< Target processor is little-endian.
//		endif	// if (defined(__aarch64__))
#		elif (defined(__arm__))
#			define GD_ARCHITECTURE_ARM32	(1)					///< Target processor is ARM-32 bits.
#			define GD_ENDIANITY_LITTLE		(1)					///< Target processor is little-endian.
//		endif	// if (defined(__arm__))
#		elif (defined(__amd64__))
#			define GD_ARCHITECTURE_X64		(1)					///< Target processor is x64.
#			define GD_ENDIANITY_LITTLE		(1)					///< Target processor is little-endian.
//		endif	// if (defined(__amd64__))
#		elif (defined(__i386__))
#			define GD_ARCHITECTURE_X86		(1)					///< Target processor is x86.
#			define GD_ENDIANITY_LITTLE		(1)					///< Target processor is little-endian.
//		endif	// if (defined(__i386__))
#		else	// *** GCC Architecture selection. ***
#			error "Unable to determine GCC compilers target architecture."
#		endif	// *** GCC Architecture selection. ***
//	endif	// if (defined(GD_COMPILER_GCC_COMPATIBLE))
#	endif	// *** Target architecture decls. ***
#endif	// if (defined(GD_PLATFORM_HTML5))

#if (!(0 \
	^ defined(GD_ARCHITECTURE_X86) \
	^ defined(GD_ARCHITECTURE_X64) \
	^ defined(GD_ARCHITECTURE_ARM32) \
	^ defined(GD_ARCHITECTURE_ARM64)))
#	error "No target CPU architecture was selected."
#endif	// *** All target architectures xored. ***
#if (!(0 \
	^ defined(GD_ENDIANITY_BIG) \
	^ defined(GD_ENDIANITY_LITTLE)))
#	error "No target CPU endianity was selected."
#endif	// *** Big & Little endians xored. ***

/// ------------------------------------------------------------------------------------------
/// Export specifications and etc.
/// ------------------------------------------------------------------------------------------

// Exports...
#if (defined(GD_COMPILER_MSVC_COMPATIBLE))
#	define GDINT __declspec(/*Nothing*/)						///< Assembly internal name.
#	define GDEXP __declspec(dllexport)							///< Method would be exported from Assembly.
#	define GDIMP __declspec(dllimport)							///< Method would be imported from external Assembly.
#	if (defined(_WINDLL) && (!defined(GD_DLL_IMPORT)))
#		define GDAPI GDEXP										///< Method would be exported/imported from DLL.
#	else	// if (defined(_WINDLL) && (!defined(GD_DLL_IMPORT)))
#		define GDAPI GDIMP										///< Method would be exported/imported from DLL.
#	endif	// if (defined(_WINDLL) && (!defined(GD_DLL_IMPORT)))
//endif	// if (defined(GD_COMPILER_MSVC_COMPATIBLE))
#elif (defined(GD_COMPILER_GCC_COMPATIBLE))
#	define GDINT __attribute__((visibility("protected")))		///< Assembly internal name.
#	define GDAPI __attribute__((visibility("default")))			///< Method would be exported/imported from DLL.
#	define GDEXP GDAPI											///< Method would be exported/imported from DLL.
#	define GDIMP GDAPI											///< Method would be imported from external Assembly.
//endif // if (defined(GD_COMPILER_GCC_COMPATIBLE))
#endif	// *** Export specification. ***

/// Define this to disable force inlining of methods.
// #define GD_NO_FORCE_INLINE

// Inlining...
#if (defined(GD_NO_FORCE_INLINE))
#	define GDINL inline											///< Inline method.
#else	// if (defined(GD_NO_FORCE_INLINE))
#	if (defined(GD_COMPILER_MSVC_COMPATIBLE))
#		define GDINL __forceinline								///< Method would be forcely inlined.
//	endif	// if (defined(GD_COMPILER_MSVC_COMPATIBLE))
#	elif (defined(GD_COMPILER_GCC_COMPATIBLE))
#		define GDINL __attribute__((always_inline)) inline		///< Method would be forcely inlined.
//	elif (defined(GD_COMPILER_GCC_COMPATIBLE))			
#	endif	// *** Force inline specification. ***
#endif	// if (defined(GD_NO_FORCE_INLINE))

// Aligment...
#if (defined(GD_COMPILER_MSVC_COMPATIBLE))
#	define GD_ALIGN_MSVC(Alignment) __declspec(align(Alignment))		///< Class would be aligned in MSVS compatible notation.
#	define GD_ALIGN_GCC(Alignment)										///< Dummy class alignment  for abstraction.
//endif	// if (defined(GD_COMPILER_MSVC_COMPATIBLE))
#elif (defined(GD_COMPILER_GCC_COMPATIBLE))
#	define GD_ALIGN_MSVC(Alignment)										///< Dummy class alignment  for abstraction.
#	define GD_ALIGN_GCC(Alignment) __attribute__((aligned(Alignment)))	///< Class would be aligned in MSVS compatible notation.
//endif	// if (defined(GD_COMPILER_GCC_COMPATIBLE))
#endif	// *** Aligment specifications. ***

// Noreturn...
#if (defined(GD_COMPILER_MSVC_COMPATIBLE))
#	define GD_NORETURN __declspec(noreturn)				///< Method would terminate the application.
//endif	// if (defined(GD_COMPILER_MSVC_COMPATIBLE))
#elif (defined(GD_COMPILER_GCC_COMPATIBLE))
#	define GD_NORETURN __attribute__((noreturn))		///< Method would terminate the application.
//endif	// if  (defined(GD_COMPILER_GCC_COMPATIBLE))
#endif	// *** Noreturn specifications. ***

// Deprecation...
#if (defined(GD_COMPILER_MSVC_COMPATIBLE))
#	define GD_DEPRECATED __declspec(deprecated)			///< Following definitions is declared deprecated.
//endif	// if (defined(GD_COMPILER_MSVC_COMPATIBLE))
#elif (defined(GD_COMPILER_GCC_COMPATIBLE))
#	define GD_DEPRECATED __attribute__((deprecated))	///< Following definitions is declared deprecated.
//endif // if (defined(GD_COMPILER_GCC_COMPATIBLE))
#endif	// *** Deprecation specifications. ***

// Printf-like functions...
#if (defined(GD_COMPILER_MSVC_COMPATIBLE))
#	define GD_PRINTF_LIKE(StringIndex, FirstToCheck)	///< Following functions smells like printf. Does some real static analyzis on GCC-compatible compilers.
//endif	// if (defined(GD_COMPILER_MSVC_COMPATIBLE))
#elif (defined(GD_COMPILER_GCC_COMPATIBLE))
#	define GD_PRINTF_LIKE(StringIndex, FirstToCheck) __attribute__((format (printf, StringIndex, FirstToCheck)))	///< Following functions smells like printf. Does some real static analyzis on GCC-compatible compilers.
//endif // if (defined(GD_COMPILER_GCC_COMPATIBLE))
#endif	// *** Printf-like functions. ***

// // For future expansion...
// #if (defined(GD_COMPILER_MSVC_COMPATIBLE))
// //endif	// if (defined(GD_COMPILER_MSVC_COMPATIBLE))
// #elif (defined(GD_COMPILER_GCC_COMPATIBLE))
// //endif // if (defined(GD_COMPILER_GCC_COMPATIBLE))
// #endif	// *** Future expansion specifications. ***

/// ------------------------------------------------------------------------------------------
/// Compiler setup.
/// ------------------------------------------------------------------------------------------

// __FUNCTION__ in GCC.
#if (defined(GD_COMPILER_GCC_COMPATIBLE))
#	if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#		define __FUNCTION__ __func__
#	endif	// if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#endif	// if (defined(GD_COMPILER_GCC_COMPATIBLE))

#if defined(__cplusplus)
	// Checking if C++ RTTI is disabled...
#	if (defined(GD_COMPILER_MSVC_COMPATIBLE))
#		if (defined(_CPP_RTTI))
#			error "Standart C++ RTTI should be disabled in MSVC compiler."
#		endif	// if (defined(_CPP_RTTI))
//	endif	// if (defined(GD_COMPILER_MSVC_COMPATIBLE))
#	elif (defined(GD_COMPILER_GCC_COMPATIBLE))
#		if (defined(__GXX_RTTI))
#			error "Standart C++ RTTI should be disabled in GCC compiler."
#		endif	// if (defined(__GXX_RTTI))
//	endif // if (defined(GD_COMPILER_GCC_COMPATIBLE))
#	endif	// *** C++ RTTI check. ***

	// And exceptions are enabled.
#	if (defined(GD_COMPILER_MSVC_COMPATIBLE))
#		if (!(defined(_CPPUNWIND)))
#			error "Exceptions should be enabled in MSVC compiler."
#		endif	// if (!(defined(_CPPUNWIND)))
//	endif	// if (defined(GD_COMPILER_MSVC_COMPATIBLE))
#	elif (defined(GD_COMPILER_GCC_COMPATIBLE))
#		if (!(defined(__EXCEPTIONS)))
#			error "Exceptions should be enabled in GCC compiler."
#		endif	// if (!(defined(__EXCEPTIONS)))
//	endif // if (defined(GD_COMPILER_GCC_COMPATIBLE))
#	endif	// *** C++ exceptions check. ***
#endif	// if defined(__cplusplus)

/// ------------------------------------------------------------------------------------------
/// Target configuration.
/// ------------------------------------------------------------------------------------------

#if (defined(GD_COMPILER_MSVC_COMPATIBLE))
#	if (defined(_DEBUG))
#		define GD_DEBUG		(1)							///< Building a debuggable version of the engine.
//	endif	// if (defined(_DEBUG))
#	elif (defined(NDEBUG))
#		define GD_RELEASE	(1)							///< Building an optimized version of the engine.
//	endif	// if (defined(NDEBUG))
#	else	// *** MSVC Debug specifications. ***
#		error "Unable to determine MSVC build configuration."
#	endif	// *** MSVC Debug specifications. ***
//endif	// if (defined(GD_COMPILER_MSVC_COMPATIBLE))
#elif (defined(GD_COMPILER_GCC_COMPATIBLE))
#	if (defined(__OPTIMIZE__))
#		define GD_RELEASE	(1)							///< Building an optimized version of the engine.
#	else	// if (defined(__OPTIMIZE__))
#		define GD_DEBUG		(1)							///< Building a debuggable version of the engine.
#	endif	// if (defined(__OPTIMIZE__))
//endif // if (defined(GD_COMPILER_GCC_COMPATIBLE))
#endif	// *** Deprecation specifications. ***

#if (!(0 \
	^ defined(GD_DEBUG) \
	^ defined(GD_RELEASE)))
#	error "Failed to determine target configuration."
#endif	// *** Debug & Release confirations XORed. ***

/// ==========================================================================================
/// Engine namepaces.
/// ==========================================================================================

#if defined(__cplusplus)
/// Redefine this to move entire engine code to custom namespace.
#	define GD_NAMESPACE Goddamn
#	if (defined(GD_NAMESPACE))
#		define GD_NAMESPACE_BEGIN namespace GD_NAMESPACE {		///< Declaration of the namespace where entire API is located.
#		define GD_NAMESPACE_END   }	// namespace GD_NAMESPACE  	///< End of main namespace delcaration.
#		define GD				  ::GD_NAMESPACE::
#		define GD_USING_NAMESPACE using namespace ::GD_NAMESPACE
#	else	// if (defined(GD_NAMESPACE))
#		define GD_NAMESPACE_BEGIN								///< Dummy for namespace begin.
#		define GD_NAMESPACE_END									///< Dummy for namespace end.
#		define GD ::
#		define GD_USING_NAMESPACE
#	endif	// if (defined(GD_NAMESPACE))
#endif	// if defined(__cplusplus)

/// ==========================================================================================
/// Most common definitions.
/// ==========================================================================================

#if defined(__cplusplus)
#	include <cstddef>
#	include <cstdint>
#else	// if defined(__cplusplus)
#	include <stddef.h>
#	include <stdint.h>
#endif	// if defined(__cplusplus)

/// ------------------------------------------------------------------------------------------
/// C++ syntax "improvements" & useful macros.
/// ------------------------------------------------------------------------------------------

#define GD_MONOLITHIC_ENGINE

#define fallthrough
#define abstract		= 0
#define impl_abstract	abstract
#define object_cast		static_cast
#define Min(a, b)		(((a) < (b)) ? (a) : (b)) 
#define Max(a, b)		(((a) > (b)) ? (a) : (b)) 
#define GD_UNUSED(Argument) (static_cast<void>(Argument))

/// Some glue magic.
#define GD_GLUE_(A, B) A ## B
#define GD_GLUE(A, B) GD_GLUE_(A, B)

/// Some widening magic.
#define GD_WIDEN_(String) L ## String
#define GD_WIDEN(String) GD_WIDEN_(String)

#if defined(__cplusplus)
#	define GD_BIT(Bit)		(TypeTraits::RemoveAllModifiers<decltype((Bit))>::Type(1) << (Bit))
#	define GD_CLASS_UNASSIGNABLE(Class)		private: GDINT Class& operator= (Class const&) = delete; \
											private: GDINT Class& operator= (Class     &&) = delete;
#	define GD_CLASS_UNSWAPPABLE(Class)		///@todo: GCC does not compiles this: private: GDINT friend void Swap(Class&, Class&) = delete;
#	define GD_CLASS_UNCOPIABLE(Class)		private: GDINT Class(Class const&) = delete;
#	define GD_CLASS_UNMOVABLE(Class)		private: GDINT Class(Class     &&) = delete;
#endif	// if defined(__cplusplus)

/// ------------------------------------------------------------------------------------------
/// Array size.
/// ------------------------------------------------------------------------------------------

#if defined(__cplusplus)
GD_NAMESPACE_BEGIN
	template <typename Type, size_t const Count>
	GDINT char (&ArraySizeHelper(Type (&Array)[Count]))[Count];
GD_NAMESPACE_END
#	define GD_ARRAY_SIZE(Array) (sizeof(ArraySizeHelper(Array)))				///< Using safe macro for array length.
#else	// if defined(__cplusplus)
#	if (defined(GD_DEBUG))
		GDINL static size_t gd_array_size_failure() { GD_DEBUG_ASSERT_FALSE("Non-array was passed to 'GD_ARRAY_SIZE' macro."); }
		/// Using safe macro for array length.
#		define GD_ARRAY_SIZE(Array) ((sizeof(Array) != sizeof(&Array[0])) ? (sizeof(Array) / sizeof(*Array)) : (SIZE_MAX + (0 & gd_array_size_failure())))
#	else	// if (defined(GD_DEBUG))
#		define GD_ARRAY_SIZE(Array) (sizeof(Array) / sizeof(Array[0]))			///< Using UNSAFE macro for array length in C relaese code.
#	endif	// if (defined(GD_DEBUG))
#endif	// if defined(__cplusplus)

/// ------------------------------------------------------------------------------------------
/// Basic types.
/// ------------------------------------------------------------------------------------------

#if defined(__cplusplus)
GD_NAMESPACE_BEGIN
	typedef std::uint8_t UInt8;
	typedef std:: int8_t  Int8;
	static_assert((sizeof(UInt8) == sizeof(Int8)) && (sizeof(UInt8) == 1), "Error in 8-bit integer sizes");

	typedef std::uint16_t UInt16;
	typedef std:: int16_t  Int16;
	static_assert((sizeof(UInt16) == sizeof(Int16)) && (sizeof(UInt16) == 2), "Error in 16-bit integer sizes");

	typedef std::uint32_t UInt32;
	typedef std:: int32_t  Int32;
	static_assert((sizeof(UInt32) == sizeof(Int32)) && (sizeof(UInt32) == 4), "Error in 32-bit integer sizes");

	// In JavaScript we do not have any 64-bit integer types. 
#if (!defined(GD_ARCHITECTURE_ASMJS))
	typedef std::uint64_t UInt64;
	typedef std:: int64_t  Int64;
	static_assert((sizeof(UInt64) == sizeof(Int64)) && (sizeof(UInt64) == 8), "Error in 64-bit integer sizes");
#endif	// if (!defined(GD_ARCHITECTURE_ASMJS))

	typedef UInt8 Float8[1];
	typedef UInt8 Float16[2];

	typedef float Float32;
	static_assert(sizeof(Float32) == 4, "Error in 32-bit float sizes");

	typedef double Float64;
	static_assert(sizeof(Float64) == 8, "Error in 64-bit float sizes");

	typedef char const* Str;
	typedef void const* chandle;
	typedef void      *  handle;

GD_NAMESPACE_END
#	include <GoddamnEngine/Core/Diagnostics/Assertion/Assertion.h>
#	include <GoddamnEngine/Core/Allocator/Allocator.h>
#endif	// if defined(__cplusplus)

#include <cmath>
#define SquareRoot (::sqrt)

//////////////////////////////////////////////////////////////////////////
// Temporary debug class implementation
#define Debug
#if defined(GD_DEBUG_OUTPUT_VS)
#	include <Windows.h>
#	define Log(message)	OutputDebugString(&String::Format("\n[-]\t %s", &message[0])[0]) 
#else
#	include <stdio.h>
#	define Log(message)	printf("\n[-]\t %s", (&message[0]))
#endif
#define Warning			Log
#define Error			Log
//////////////////////////////////////////////////////////////////////////

#endif	// ifndef ___GODDAMN_ENGINE_INCLUDE___
