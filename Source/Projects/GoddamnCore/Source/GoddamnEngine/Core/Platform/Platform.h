// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Definitions/Platform.h
//! @note This file should be never directly included, please consider using <GoddamnEngine/Include.h> instead.
//! File contains platform-abstraction-layer definitions. It should be directly included
//! at the top of each header and source file in this directory.
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/Singleton.h>

// ------------------------------------------------------------------------------------------
// Platform-Dependent include wrapper.
// ------------------------------------------------------------------------------------------
#if !GD_DOCUMENTATION
#	if GD_PLATFORM_WINDOWS
#		define GD_PLATFORM_INCLUDE(Directory, Header)	<Directory/Windows/Windows ## Header>
#	endif	// if GD_PLATFORM_WINDOWS
#	if GD_PLATFORM_WINDOWS_PHONE8
#		define GD_PLATFORM_INCLUDE(Directory, Header)	<Directory/WindowsPhone8/WindowsPhone8 ## Header>
#	endif	// if GD_PLATFORM_WINDOWS_PHONE8
#	if GD_PLATFORM_WINDOWS_RT
#		define GD_PLATFORM_INCLUDE(Directory, Header)	<Directory/WindowsRT/WindowsRT ## Header>
#	endif	// if GD_PLATFORM_WINDOWS_RT
#	if GD_PLATFORM_OS_X
#		define GD_PLATFORM_INCLUDE(Directory, Header)	<Directory/OSX/OSX ## Header>
#	endif	// if GD_PLATFORM_OS_X
#	if GD_PLATFORM_IOS
#		define GD_PLATFORM_INCLUDE(Directory, Header)	<Directory/iOS/iOS ## Header>
#	endif	// if GD_PLATFORM_IOS
#	if GD_PLATFORM_GNU_LINUX
#		define GD_PLATFORM_INCLUDE(Directory, Header)	<Directory/GNULinux/GNULinux ## Header>
#	endif	// if GD_PLATFORM_GNU_LINUX
#	if GD_PLATFORM_ANDROID
#		define GD_PLATFORM_INCLUDE(Directory, Header)	<Directory/Android/Android ## Header>
#	endif	// if GD_PLATFORM_ANDROID
#	if GD_PLATFORM_XBOX_ONE
#		define GD_PLATFORM_INCLUDE(Directory, Header)	<Directory/XboxOne/XboxOne ## Header>
#	endif	// if GD_PLATFORM_XBOX_ONE
#	if GD_PLATFORM_PLAYSTATION4
#		define GD_PLATFORM_INCLUDE(Directory, Header)	<Directory/PlayStation4/PlayStation4 ## Header>
#	endif	// if GD_PLATFORM_PLAYSTATION4
#	if GD_PLATFORM_HTML5
#		define GD_PLATFORM_INCLUDE(Directory, Header)	<Directory/HTML5/HTML5 ## Header>
#	endif	// if GD_PLATFORM_HTML5
#	if GD_PLATFORM_INCLUDE
#		error GD_PLATFORM_INCLUDE: No platform was selected.
#	endif	// if GD_PLATFORM_INCLUDE
#else	// if !GD_DOCUMENTATION

//! Generates a path to header with implementation of some platform-dependent code.
//! @param Directory Path to directory with subdirectories that contain platform-dependent code.
//! @param Header Name of the header file.
#define GD_PLATFORM_INCLUDE(Directory, Header)	<Directory/CurrentPlatform/CurrentPlatform ## Header>

#endif	// if !GD_DOCUMENTATION

// ------------------------------------------------------------------------------------------
// Interface/Singleton methods wrappers.
// ------------------------------------------------------------------------------------------

//! @brief
//! @param MethodDecl
//! @param MethodReturnType
//! @param MethodName
//! @param MethodArgsDecls
//! @param MethodArgsValues
#define GD_PLATFORM_DEFINE_METHOD_IMPL_DECL(MethodDecl, MethodReturnType, MethodName, MethodArgsDecls, MethodArgsValues, ...) \
	MethodDecl MethodReturnType MethodName##_Inst(MethodArgsDecls) __VA_ARGS__ \

//! @brief
//! @param MethodReturnType
//! @param MethodName
//! @param MethodArgsDecls
//! @param MethodArgsValues
#define GD_PLATFORM_DEFINE_METHOD_IMPL(MethodReturnType, MethodName, MethodArgsDecls, MethodArgsValues, ...) \
	GD_PLATFORM_DEFINE_METHOD_IMPL_DECL(GDAPI virtual, MethodReturnType, MethodName, MethodArgsDecls, MethodArgsValues, __VA_ARGS__); \
	GDINL static MethodReturnType MethodName(MethodArgsDecls) \
	{ \
		return GetInstance().MethodName##_Inst(MethodArgsValues); \
	} \

//! @brief
//! @param MethodReturnType
//! @param MethodName
//! @param MethodArgsDecls
//! @param MethodArgsValues
#define GD_PLATFORM_SHORTCUT_METHOD_IMPL(MethodReturnType, MethodName, MethodArgsDecls, MethodArgsValues, ...) \
	GD_PLATFORM_DEFINE_METHOD_IMPL_DECL(GDINL, MethodReturnType, MethodName, MethodArgsDecls, MethodArgsValues, __VA_ARGS__); \
	GDINL static MethodReturnType MethodName(MethodArgsDecls) \
	{ \
		return GetInstance().MethodName##_Inst(MethodArgsValues); \
	} \

//! @brief
//! @param MethodReturnType
//! @param MethodName
//! @param MethodArgsDecls
//! @param MethodArgsValues
#define GD_PLATFORM_OVERRIDE_METHOD_IMPL(MethodReturnType, MethodName, MethodArgsDecls, MethodArgsValues, ...) \
	GD_PLATFORM_DEFINE_METHOD_IMPL_DECL(GDAPI virtual, MethodReturnType, MethodName, MethodArgsDecls, MethodArgsValues, __VA_ARGS__); \

#define GD_PLATFORM_DEFINE_METHOD0(MethodReturnType, MethodName, ...) GD_PASS(GD_PLATFORM_DEFINE_METHOD_IMPL(MethodReturnType, MethodName, GD_PASS(), GD_PASS(), __VA_ARGS__))
#define GD_PLATFORM_DEFINE_METHOD1(MethodReturnType, MethodName, Type0, Name0, ...) GD_PASS(GD_PLATFORM_DEFINE_METHOD_IMPL(MethodReturnType, MethodName, GD_PASS(Type0 Name0), GD_PASS(Name0), __VA_ARGS__))
#define GD_PLATFORM_DEFINE_METHOD2(MethodReturnType, MethodName, Type0, Name0, Type1, Name1, ...) GD_PASS(GD_PLATFORM_DEFINE_METHOD_IMPL(MethodReturnType, MethodName, GD_PASS(Type0 Name0, Type1 Name1), GD_PASS(Name0, Name1), __VA_ARGS__))
#define GD_PLATFORM_DEFINE_METHOD3(MethodReturnType, MethodName, Type0, Name0, Type1, Name1, Type2, Name2, ...) GD_PASS(GD_PLATFORM_DEFINE_METHOD_IMPL(MethodReturnType, MethodName, GD_PASS(Type0 Name0, Type1 Name1, Type2 Name2), GD_PASS(Name0, Name1, Name2), __VA_ARGS__))
#define GD_PLATFORM_DEFINE_METHOD4(MethodReturnType, MethodName, Type0, Name0, Type1, Name1, Type2, Name2, Type3, Name3, ...) GD_PASS(GD_PLATFORM_DEFINE_METHOD_IMPL(MethodReturnType, MethodName, GD_PASS(Type0 Name0, Type1 Name1, Type2 Name2, Type3 Name3), GD_PASS(Name0, Name1, Name2, Name3), __VA_ARGS__))
#define GD_PLATFORM_DEFINE_METHOD5(MethodReturnType, MethodName, Type0, Name0, Type1, Name1, Type2, Name2, Type3, Name3, Type4, Name4, ...) GD_PASS(GD_PLATFORM_DEFINE_METHOD_IMPL(MethodReturnType, MethodName, GD_PASS(Type0 Name0, Type1 Name1, Type2 Name2, Type3 Name3, Type4 Name4), GD_PASS(Name0, Name1, Name2, Name3, Name4), __VA_ARGS__))

#define GD_PLATFORM_SHORTCUT_METHOD0(MethodReturnType, MethodName, ...) GD_PASS(GD_PLATFORM_SHORTCUT_METHOD_IMPL(MethodReturnType, MethodName, GD_PASS(), GD_PASS(), __VA_ARGS__))
#define GD_PLATFORM_SHORTCUT_METHOD1(MethodReturnType, MethodName, Type0, Name0, ...) GD_PASS(GD_PLATFORM_SHORTCUT_METHOD_IMPL(MethodReturnType, MethodName, GD_PASS(Type0 Name0), GD_PASS(Name0), __VA_ARGS__))
#define GD_PLATFORM_SHORTCUT_METHOD2(MethodReturnType, MethodName, Type0, Name0, Type1, Name1, ...) GD_PASS(GD_PLATFORM_SHORTCUT_METHOD_IMPL(MethodReturnType, MethodName, GD_PASS(Type0 Name0, Type1 Name1), GD_PASS(Name0, Name1), __VA_ARGS__))
#define GD_PLATFORM_SHORTCUT_METHOD3(MethodReturnType, MethodName, Type0, Name0, Type1, Name1, Type2, Name2, ...) GD_PASS(GD_PLATFORM_SHORTCUT_METHOD_IMPL(MethodReturnType, MethodName, GD_PASS(Type0 Name0, Type1 Name1, Type2 Name2), GD_PASS(Name0, Name1, Name2), __VA_ARGS__))
#define GD_PLATFORM_SHORTCUT_METHOD4(MethodReturnType, MethodName, Type0, Name0, Type1, Name1, Type2, Name2, Type3, Name3, ...) GD_PASS(GD_PLATFORM_SHORTCUT_METHOD_IMPL(MethodReturnType, MethodName, GD_PASS(Type0 Name0, Type1 Name1, Type2 Name2, Type3 Name3), GD_PASS(Name0, Name1, Name2, Name3), __VA_ARGS__))
#define GD_PLATFORM_SHORTCUT_METHOD5(MethodReturnType, MethodName, Type0, Name0, Type1, Name1, Type2, Name2, Type3, Name3, Type4, Name4, ...) GD_PASS(GD_PLATFORM_SHORTCUT_METHOD_IMPL(MethodReturnType, MethodName, GD_PASS(Type0 Name0, Type1 Name1, Type2 Name2, Type3 Name3, Type4 Name4), GD_PASS(Name0, Name1, Name2, Name3, Name4), __VA_ARGS__))

#define GD_PLATFORM_OVERRIDE_METHOD0(MethodReturnType, MethodName, ...) GD_PASS(GD_PLATFORM_OVERRIDE_METHOD_IMPL(MethodReturnType, MethodName, GD_PASS(), GD_PASS(), __VA_ARGS__))
#define GD_PLATFORM_OVERRIDE_METHOD1(MethodReturnType, MethodName, Type0, Name0, ...) GD_PASS(GD_PLATFORM_OVERRIDE_METHOD_IMPL(MethodReturnType, MethodName, GD_PASS(Type0 Name0), GD_PASS(Name0), __VA_ARGS__))
#define GD_PLATFORM_OVERRIDE_METHOD2(MethodReturnType, MethodName, Type0, Name0, Type1, Name1, ...) GD_PASS(GD_PLATFORM_OVERRIDE_METHOD_IMPL(MethodReturnType, MethodName, GD_PASS(Type0 Name0, Type1 Name1), GD_PASS(Name0, Name1), __VA_ARGS__))
#define GD_PLATFORM_OVERRIDE_METHOD3(MethodReturnType, MethodName, Type0, Name0, Type1, Name1, Type2, Name2, ...) GD_PASS(GD_PLATFORM_OVERRIDE_METHOD_IMPL(MethodReturnType, MethodName, GD_PASS(Type0 Name0, Type1 Name1, Type2 Name2), GD_PASS(Name0, Name1, Name2), __VA_ARGS__))
#define GD_PLATFORM_OVERRIDE_METHOD4(MethodReturnType, MethodName, Type0, Name0, Type1, Name1, Type2, Name2, Type3, Name3, ...) GD_PASS(GD_PLATFORM_OVERRIDE_METHOD_IMPL(MethodReturnType, MethodName, GD_PASS(Type0 Name0, Type1 Name1, Type2 Name2, Type3 Name3), GD_PASS(Name0, Name1, Name2, Name3), __VA_ARGS__))
#define GD_PLATFORM_OVERRIDE_METHOD5(MethodReturnType, MethodName, Type0, Name0, Type1, Name1, Type2, Name2, Type3, Name3, Type4, Name4, ...) GD_PASS(GD_PLATFORM_OVERRIDE_METHOD_IMPL(MethodReturnType, MethodName, GD_PASS(Type0 Name0, Type1 Name1, Type2 Name2, Type3 Name3, Type4 Name4), GD_PASS(Name0, Name1, Name2, Name3, Name4), __VA_ARGS__))

GD_NAMESPACE_BEGIN

	//! Describes compile-time flags of a platform type.
	//! @{
	struct PlatformObjectFlags
	{
		typedef UInt8 Type;
		Type static const FlagsNone						= 0;
		Type static const FlagsGenericInterfaceIsDummy	= GD_BIT(0);	//!< Indicates that generic interface is useless without concrete implementation.
		Type static const FlagsLiquidSignleton			= GD_BIT(1);	//!< Indicates that this class is not technically a singleton.
		Type static const FlagsHasNoTickFunction		= GD_BIT(2);	//!< Indicates that this class is not required to be updated every tick.
	};	// struct PlatformObjectFlags
	typedef PlatformObjectFlags::Type PlatformObjectFlagsTp;
	//! @}

	//! Interface to an platform-dependent object.
	class IPlatformObjectBase
	{
	protected:
		GDINL IPlatformObjectBase() { }
		GDINL virtual ~IPlatformObjectBase() { }

	public:

		//! Initializes a platform-dependent object.
		//! @{
		GDAPI virtual void InitializeObject_Inst() {}
		GDINL void InitializeObject()
		{
			this->InitializeObject_Inst();
		}
		//! @}

		//! Deinitializes a platform-dependent object.
		//! @{
		GDAPI virtual void DestroyObject_Inst() {}
		GDINL void DestroyObject()
		{
			this->DestroyObject_Inst();
		}
		//! @}

		//! Ticks a platform-dependent object.
		//! @{
		GDAPI virtual void TickObject_Inst() {}
		GDINL void TickObject()
		{
			this->TickObject_Inst();
		}
		//! @}
	};	// class IPlatformObjectBase

	//! Interface and wrapper to an platform-dependent object.
	//! @tparam IPlatformObjectTp Type of an platform-dependent object.
	//! @tparam PlatformObjectFlagsValue Flags of a platform-dependent object.
	template<typename IPlatformObjectTp, PlatformObjectFlagsTp PlatformObjectFlagsValue>
	class IPlatformObject : public IPlatformObjectBase	//, public Singleton<IPlatformObjectTp>, public IUncopiable
	{
	public:
		GDAPI static IPlatformObjectTp& GetInstance() { return *reinterpret_cast<IPlatformObjectTp*>(nullptr); };
	};	// class IPlatformObject

GD_NAMESPACE_END
