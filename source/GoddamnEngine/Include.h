/// ==========================================================================================
/// Include.h: Common definitions for all GD projects
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.--.2012 - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GODDAMN_ENGINE_INCLUDE
#define GODDAMN_ENGINE_INCLUDE

/// ==========================================================================================
/// Version definitions
#define ENGINE				GoddamnEngine	
#define ENGINE_VERSION		0x05000001ui32
#define ENGINE_VERSION_FULL	"5.0.0.1"

#define GD_ENGINE_NAME		"GoddamnEngine"
#define GD_ENGINE_VERSION	"1/2"
/// ==========================================================================================

#define GD_BUILDTYPE_STANDALONE_ENGINE
// #define GD_BUILDTYPE_EDITOR
#if !(defined(GD_BUILDTYPE_EDITOR) ^ defined(GD_BUILDTYPE_STANDALONE_ENGINE))
#	error 'GD_BUILDTYPE_EDITOR' and 'GD_BUILDTYPE_STANDALONE_ENGINE' are both (un)defined, \
		   please use just one of them to specify project build type
#endif

/// ==========================================================================================
/// Supported platforms: 
///	Desktop	 :	OS X (Mavericks onwards - TBA), Windows (7 onward, tested on 8.1 only, does not supports RT)
/// Consoles :	PlayStation 4, XBox One, SteamBox
/// Mobile	 :	iOS (iPhone 5S, iPad Air / Mini Retina and further generations - TBA), Windows Phone 8 (after first x64 devices - TBA)
#if (defined(_WIN32))
#	define _USE_MATH_DEFINES       // <| M_PI and others
#	define _CRT_SECURE_NO_WARNINGS // <| Using printf and others
#	define  WIN32_LEAN_AND_MEAN    // <| We do not need whole WinAPI header.
#	define  VC_EXTRALEAN		   //  |
#	include <Windows.h>
#	define GD_PLATFORM_WINAPI			  (1)
//	Platform arch, name, desktop/mobile type.
#	if (defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP))	//   Windows Desktop application.
#		define GD_PLATFORM_ARCH_X86		  (1)									///< x86 platform 
#		define GD_PLATFORM_WINDOWS		  (1)									///< Macro to detect Windows-specific code
#		define GD_PLATFORM_DESKTOP		  (1)			        				///< Building for desktop platform 
//  endif	// if (defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP))
#	elif (defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP))	//   Windows Phone 8 application.
#		define GD_PLATFORM_ARCH_ARM  	  (1)									///< ARM Platform.
#		define GD_PLATFORM_WINDOWS_PHONE8 (1)									///< Macro to detect WindowsPhone 8-specific code
#		define GD_PLATFORM_MOBILE		  (1)									///< Building for mobile platform 
//	endif	// elif (defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP))
#	elif (defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_PC_APP))	//   Windows Store application (Windows Desktop or Windows RT)
#		if (defined(__WRL_WINRT_STRICT__))										//   Windows RT application.
#			define GD_PLATFORM_ARCH_ARM   (1)									///< ARM Platform.
#			define GD_PLATFORM_WINDOWS_RT (1)									///< Macro to detect Windows RT-specific code
#			define GD_PLATFORM_MOBILE	  (1)									///< Building for mobile platform 
#		else	// if (defined(__WRL_WINRT_STRICT__))							//   Windows Desktop application.
#			define GD_PLATFORM_ARCH		("x86")									///< x86 Platform.
#			define GD_PLATFORM_NAME		("Windows")								///< Building for Windows-Powered machine
#			define GD_PLATFORM_WINDOWS	(GD_PLATFORM_NAME)						///< Macro to detect Windows-specific code
#			define GD_PLATFORM_MOBILE	(GD_PLATFORM_WINDOWS)					///< Building for mobile platform 
#		endif	// if (defined(__WRL_WINRT_STRICT__))
//	endif	// elif (defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP))
#	else	// elif (defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_***))
#		error "Unsupported Windows platform.""
#	endif	// elif (defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_***))
//	Debug/Release builds.
#	if (defined(_DEBUG))
#		define GD_DEBUG 1
#	endif	// if defined(_DEBUG)
//	Bit depth of processor.
#	if (defined(_M_AMD64))
#		define GD_PLATFORM_64BIT (GD_PLATFORM_ARCH)
#	else	// if (defined(_M_AMD64))
#		define GD_PLATFORM_32BIT (GD_PLATFORM_ARCH)
#	endif	// if (defined(_M_AMD64))
//endif	// #if (defined(_WIN32))

#elif defined(__APPLE__)
#	include "TargetConditionals.h"
#	  if (defined(__APPLE__) && defined(TARGET_OS_MAC))
#		define GD_PLATFORM_ARCH			("x86")
#		define GD_PLATFORM_NAME			("OS X")
#		define GD_PLATFORM_OSX			(GD_PLATFORM_NAME)
#		define GD_PLATFORM_DESKTOP		(GD_PLATFORM_OSX)
//	elif	// if (defined(__APPLE__) && defined(TARGET_OS_MAC))
#	elif (defined(__APPLE__) && (defined(TARGET_IPAD_SIMULATOR) || defined(TARGET_IPHONE_SIMULATOR)))
#		define GD_PLATFORM_ARCH			("x86")
#		define GD_PLATFORM_NAME			("iOS")
#		define GD_PLATFORM_IOS			(GD_PLATFORM_NAME)
#		define GD_PLATFORM_MOBILE		(GD_PLATFORM_IOS)
//	elif	// elif (defined(__APPLE__) && (defined(TARGET_IPAD_SIMULATOR) || defined(TARGET_IPHONE_SIMULATOR)))
#	elif (defined(__APPLE__) && (defined(TARGET_OS_IPHONE) defined(TARGET_OS_IPAD)))
#		define GD_PLATFORM_ARCH			("ARM")
#		define GD_PLATFORM_NAME			("iOS")
#		define GD_PLATFORM_IOS			(GD_PLATFORM_NAME)
#		define GD_PLATFORM_MOBILE		(GD_PLATFORM_IOS)
//	elif	// elif (defined(__APPLE__) && (defined(TARGET_OS_IPHONE) || defined(TARGET_IPAD_SIMULATOR) || defined(TARGET_OS_IPAD) || defined(TARGET_IPHONE_SIMULATOR)))
#	else	// elif (defined(__APPLE__) && (defined(TARGET_***) ... ))
#		error 'Unsupported Apple device!'
#	endif
#	if (defined(__LP64__))
#		define GD_PLATFORM_64BIT		GD_PLATFORM_ARCH
#	else	// if (defined(__LP64__))
#		define GD_PLATFORM_32BIT		GD_PLATFORM_ARCH
#	endif	// if (defined(__LP64__))
#elif defined(GD_DOCUMENTATION)
#	define GD_PLATFORM_DESKTOP	GD_DOCUMENTATION
#	define GD_PLATFORM_MOBILE	GD_DOCUMENTATION
#	define GD_PLATFORM_CONSOLE	GD_DOCUMENTATION
#else
#	error 'Unsupported platform''
#endif

#if defined(GD_PLATFORM_32BIT)
#	pragma warning("Deprecated 32-bit build")
#endif

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/// Export specifications
#if (defined(_MSC_VER))
#	define GD_COMPILER_MSC
#	if(!defined(_CRT_SECURE_NO_WARNINGS))
#		define _CRT_SECURE_NO_WARNINGS
#	endif
#	define GD_WARNING_SUPPRESS(Warning) __pragma(warning(suppress: Warning))
#	define GD_GCC_ALIGN(Alignment)	
#	define GD_MSC_ALIGN(Alignment)	__declspec(align(Alignment))
#	define GD_DEPRECATED			__declspec(deprecated)
#	define GDINT					__declspec(/*No export needed*/)
#	define GDEXP					__declspec(dllexport)
#	define GDIMP					__declspec(dllimport)
#	define GDINL					__forceinline
#	if (defined(_WINDLL) && !defined(GD_DLL_IMPORT))
#		define GDAPI GDEXP
#	else
#		define GDAPI GDIMP
#	endif
#else // Assuming this is GCC-Compatible compiler
#	include <stdint.h>
#	include <stddef.h>
#	define GD_COMPILER_GCC
#	define GD_WARNING_SUPPRESS(Warning)
#	define GD_ALIGN(Alignment)	__attribute__((aligned(Alignment)))
#	define GD_DEPRECATED		__attribute__((deprecated))
#	define GDINL				__attribute__((always_inline)) inline
#	define GDINT				__attribute__(())
#	define GDEXP				__attribute__((visibility("default")))
#	define GDIMP				/*No export needed*/
#	define GDAPI				/*No export needed*/
#endif

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/// Code re-usage
#define abstract		= 0
#define impl_abstract	abstract
#define throws			throw
#define throws_nothing	throws()
#define self			this
#define Min(a, b)		(((a) < (b)) ? (a) : (b)) 
#define Max(a, b)		(((a) > (b)) ? (a) : (b)) 
#define GD_UNUSED(Argument) ((void) Argument)

#if (defined(_CPP_RTTI) && defined(GD_DEBUG))
#	define object_cast dynamic_cast
#else
#	define object_cast static_cast
#endif

/// @todo Check CPU endianity here
#define GD_BIT(Bit) (TypeTraits::RemoveAllModifiers<decltype((Bit))>::Type(1) << (Bit))
#define GD_MONOLITHIC_ENGINE

#if defined(__cplusplus)
#	define GD_PLATFORM_HAS_MOVE_SUPPORT		(true)
#	define GD_NAMESPACE						Goddamn 
#	if(defined(GD_NAMESPACE))
#		define GD_NAMESPACE_BEGIN				namespace GD_NAMESPACE { 
#		define GD_NAMESPACE_END					} // namespace GD_NAMESPACE { 
#		define GD								::GD_NAMESPACE::
#	else	// if(defined(GD_NAMESPACE))
#		define GD_NAMESPACE_BEGIN
#		define GD_NAMESPACE_END
#		define GD
#	endif	// if(defined(GD_NAMESPACE))
#	define GD_ARRAY_SIZE(Array)				(sizeof(ArraySizeHelper(Array))) //	Using safe macro for array length
#else
#	define GD_NAMESPACE 
#	define GD_NAMESPACE_BEGIN					
#	define GD_NAMESPACE_END					
#	define GD 
#	define GD_ARRAY_SIZE(Array)				(sizeof(Array) / sizeof(Array[0]))
#endif

#if defined(__cplusplus)
#	define GD_CLASS_UNASSIGNABLE(Class)		private: GDINT Class& operator= (Class const&)  = delete; \
											private: GDINT Class& operator= (Class     &&)  = delete;
#	define GD_CLASS_UNSWAPPABLE(Class)		///@todo: GCC does not compiles this: private: GDINT friend void Swap(Class&, Class&) = delete;
#	define GD_CLASS_UNCOPIABLE(Class)		private: GDINT Class(Class const&) = delete;
#	define GD_CLASS_UNMOVABLE(Class)		private: GDINT Class(Class     &&) = delete;
GD_NAMESPACE_BEGIN

	template <typename Type, size_t const Count>
	char (&ArraySizeHelper(Type (&Array)[Count]))[Count];

#if ((defined(GD_PLATFORM_WINDOWS) || defined(GD_PLATFORM_XBOX_ONE) || defined(GD_PLATFORM_WINPHONE8)))
	typedef   signed __int8	  Int8;  static_assert((sizeof(Int8 ) == 1), "Invalid atomic type size");
	typedef   signed __int16  Int16; static_assert((sizeof(Int16) == 2), "Invalid atomic type size");
	typedef   signed __int32  Int32; static_assert((sizeof(Int32) == 4), "Invalid atomic type size");
	typedef   signed __int64  Int64; static_assert((sizeof(Int64) == 8), "Invalid atomic type size");

	typedef unsigned __int8  UInt8;  static_assert((sizeof(UInt8 ) == 1), "Invalid atomic type size");
	typedef unsigned __int16 UInt16; static_assert((sizeof(UInt16) == 2), "Invalid atomic type size");
	typedef unsigned __int32 UInt32; static_assert((sizeof(UInt32) == 4), "Invalid atomic type size");
	typedef unsigned __int64 UInt64; static_assert((sizeof(UInt64) == 8), "Invalid atomic type size");
#else
	typedef  int8_t   Int8;  static_assert((sizeof(Int8 ) == 1), "Invalid atomic type size");
	typedef  int16_t  Int16; static_assert((sizeof(Int16) == 2), "Invalid atomic type size");
	typedef  int32_t  Int32; static_assert((sizeof(Int32) == 4), "Invalid atomic type size");
	typedef  int64_t  Int64; static_assert((sizeof(Int64) == 8), "Invalid atomic type size");

	typedef uint8_t  UInt8;  static_assert((sizeof(UInt8 ) == 1), "Invalid atomic type size");
	typedef uint16_t UInt16; static_assert((sizeof(UInt16) == 2), "Invalid atomic type size");
	typedef uint32_t UInt32; static_assert((sizeof(UInt32) == 4), "Invalid atomic type size");
	typedef uint64_t UInt64; static_assert((sizeof(UInt64) == 8), "Invalid atomic type size");
#endif

	typedef Int8   Float8[1];  static_assert((sizeof(Float8 ) == 1), "Invalid atomic type size"); ///@todo Add  8-bit floating point class type
	typedef Int8   Float16[2]; static_assert((sizeof(Float16) == 2), "Invalid atomic type size"); ///@todo Add 16-bit floating point class type
	typedef float  Float32;    static_assert((sizeof(Float32) == 4), "Invalid atomic type size");
	typedef double Float64;    static_assert((sizeof(Float64) == 8), "Invalid atomic type size");

	typedef char const*	Str;
	typedef void const*	chandle; 
	typedef void	  *	handle; 

	/*GD_DEPRECATED*/ typedef UInt32 uint;		///@todo Define this ones as deprecated
	/*GD_DEPRECATED*/ typedef UInt16 ushort;
	/*GD_DEPRECATED*/ typedef UInt64 ulong;
	/*GD_DEPRECATED*/ typedef UInt8  byte;

GD_NAMESPACE_END

#include <GoddamnEngine/Core/Allocator/Allocator.h>

#define _USE_MATH_DEFINES
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

#endif

#endif
