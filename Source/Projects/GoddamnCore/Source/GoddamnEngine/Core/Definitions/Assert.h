// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Definitions/Assert.h
//! @note This file should be never directly included, please consider using <GoddamnEngine/Include.h> instead.
//! Contains Assert mechanism definitions.
// Current interface is partly inspired by LibSDL 2.0 implementation of asserts with modifications providing fatal failures reporting and etc.
#pragma once
#if !defined(GD_INSIDE_INCLUDE_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Include.h> instead.
#endif	// if !defined(GD_INSIDE_INCLUDE_H)

#include <GoddamnEngine/Include.h>

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Cross-platform debug break.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#if GD_DEBUG
#	if GD_PLATFORM_API_MICROSOFT
#		include <crtdbg.h>
#		define GD_DEBUG_BREAK() (::_CrtDbgBreak())
#	elif GD_PLATFORM_HTML5
#		include <csignal>
#		define GD_DEBUG_BREAK() (::raise(SIGTRAP))
#	elif GD_COMPILER_GCC_COMPATIBLE && GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_X86
#		define GD_DEBUG_BREAK() (__asm__ __volatile__ ("int $3\n\t"))
#	elif GD_COMPILER_GCC_COMPATIBLE && GD_ARCHITECTURE_ARM32 || GD_ARCHITECTURE_ARM64
#		define GD_DEBUG_BREAK() (__asm__ __volatile__ (".inst 0xE7F001F0\n\t"))
#	else	// *** Some other implementation ***
#		include <csignal>
#		define GD_DEBUG_BREAK() (::raise(SIGTRAP))
#	endif	// *** Some other implementation ***
#else	// if GD_DEBUG
#	define GD_DEBUG_BREAK() (static_cast<void>(false))
#endif	// if GD_DEBUG

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Fatal Assertation mechanisms.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ------------------------------------------------------------------------------------------
//! Defines a disabled fatal assertion. Does nothing.
//! @param Message Dummy message.
#define GD_DISABLED_FALSE_ASSERT(Message, ...) \
	do { \
		(static_cast<void>(false)); \
	} while (false)

// ------------------------------------------------------------------------------------------
//! Defines an enabled fatal assertion. Reports a failure via message box (if is available on target platform) and asks if user wants to send it to tracker.
//! @param Message Message string that describes what is going on, is formatable.
#define GD_ENABLED_FALSE_ASSERT(Message, ...) \
	do { \
		GD_USING_NAMESPACE; \
		FatalAssertData static const TheAssertData(Message, __FILE__, __FUNCTION__, __LINE__); \
		HandleFatalAssert(&TheAssertData, ##__VA_ARGS__);  \
	} while (false)

GD_NAMESPACE_BEGIN

	// ------------------------------------------------------------------------------------------
	//! Stores the whole assertion data in a single structure. Saves more stack space on failure reporter call.
	struct FatalAssertData : IUncopiable
	{
	public:
		CStr const Message;
		CStr const FileName;
		CStr const FunctionName;
		SizeTp const Line;

		GDINL FatalAssertData(CStr const Message, CStr const FileName, CStr const FunctionName, SizeTp const Line)
			: Message(Message), FileName(FileName), FunctionName(FunctionName), Line(Line)
		{
		}
	};	// struct FatalAssertData

	//! Provides inner functionality for handling fatal assertions. Should not be invoked directly.
	//! @param Data Pointer to assertion description data structure.
	//! @param Args Arguments for formating message string.
	//! @{
	GD_NORETURN GDAPI extern void HandleFatalAssertVa(FatalAssertData const* const Data, va_list const Args);
	GD_NORETURN GDINL extern void HandleFatalAssert(FatalAssertData const* const Data, ...)
	{
		va_list Args; va_start(Args, Data);
		HandleFatalAssertVa(Data, Args);
	}
	//! @}

GD_NAMESPACE_END

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Regular Assertation mechanisms.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ------------------------------------------------------------------------------------------
//! Defines a disabled assertion.
//! 	   Does nothing, just validates expression at compile-time.
//! @param Expression The expression that would be evaluated.
//! @param Message Message string that describes what is going on, is formatable.
#define GD_DISABLED_ASSERT(Expression, Message, ...) \
	do { \
		GD_USING_NAMESPACE; \
		static_cast<void>(sizeof(Expression)); \
	} while (false)

// ------------------------------------------------------------------------------------------
//! Defines a enabled assertion. 
//! 	   Reports a failure via message box (if is available on target platform) and asks if user wants 
//! 	   to abort application (control is redirected to failure reporter then with provide of feedback via tracker), 
//! 	   or to debug application, or to just to ignore this assertion once or forever during this launch.
//! @param Expression The expression that would be evaluated.
//! @param Message Message string that describes what is going on, is formatable.
#define GD_ENABLED_ASSERT(Expression, Message, ...) \
	do { \
		GD_USING_NAMESPACE; \
		RegularAssertData static TheAssertData(Message, __FILE__, __FUNCTION__, __LINE__, #Expression); \
		while ((!(Expression)) && (!TheAssertData.ShouldAlwaysIgnoreThisAssert)) { \
			AssertState const TheAssertState = HandleRegularAssert(&TheAssertData, ##__VA_ARGS__); \
			if (TheAssertState == AssertState::Break) { \
				GD_DEBUG_BREAK(); \
			} else if (TheAssertState != AssertState::Retry) { \
				break; \
			} \
		} \
	} while (false)

GD_NAMESPACE_BEGIN

	//! Storing the whole assertion data in a single structure. Saves more stack space on failure reporter call.
	struct RegularAssertData final : public FatalAssertData
	{
		CStr const Expression;
		bool ShouldAlwaysIgnoreThisAssert;

		GDINL RegularAssertData(CStr const Message, CStr const FileName, CStr const FunctionName, SizeTp const Line, CStr const Expression)
			: FatalAssertData(Message, FileName, FunctionName, Line)
			, Expression(Expression)
			, ShouldAlwaysIgnoreThisAssert(false)
		{
		}
	};	// struct AssertData

	//! Describes abilities that user can do with the failures.
	enum class AssertState : UInt8
	{
		Abort,
		Retry,
		Ignore,
		IgnoreAll,
		Report,
		Break,
		Invalid
	};	// enum class AssertState

	//! Provides inner functionality for handling regular assertions.
	//!        Should not be invoked directly.
	//! @param Expression The expression that would be evaluated.
	//! @param Message Message string that describes what is going on, is formatable.
	//! @{
	GDAPI extern AssertState HandleRegularAssertVa(RegularAssertData* const Data, va_list const Args);
	GDINL extern AssertState HandleRegularAssert(RegularAssertData* const Data, ...)
	{
		va_list Args; va_start(Args, Data);
		return HandleRegularAssertVa(Data, Args);
	}
	//! @}

GD_NAMESPACE_END

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Assert level handling.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Mostly assertion level 0 is actual in release builds where all bugs are checked and no
// "not implemented" places exist. Level 1 represents testing builds: code still contains bugs
// and testers can provide feedback for developers. Level 2 is for developers and creating new 
// code and catching fatal bugs and debug it.

#define GD_ASSERT_LEVEL_DEBUG	(2)
#define GD_ASSERT_LEVEL_TESTING	(1)
#define GD_ASSERT_LEVEL_RELEASE	(0)
#if (!defined(GD_ASSERT_LEVEL))
#	if GD_DEBUG
#		define GD_ASSERT_LEVEL GD_ASSERT_LEVEL_DEBUG
//	endif	// if GD_DEBUG
#	elif GD_RELEASE
#		define GD_ASSERT_LEVEL GD_ASSERT_LEVEL_TESTING
//	endif	// if GD_RELEASE
#	else	// *** Assert level selection ***.
#		define GD_ASSERT_LEVEL GD_ASSERT_LEVEL_RELEASE
#	endif	// *** Assert level selection ***.
#endif	// if (!defined(GD_ASSERT_LEVEL))

#if (GD_ASSERT_LEVEL == 0)
#	define GD_DEBUG_ASSERT_FALSE GD_DISABLED_FALSE_ASSERT
#	define GD_DEBUG_ASSERT GD_DISABLED_ASSERT
#	define GD_ASSERT_FALSE GD_DISABLED_FALSE_ASSERT
#	define GD_ASSERT GD_DISABLED_ASSERT
//endif	// if (GD_ASSERT_LEVEL == 0)
#elif (GD_ASSERT_LEVEL == 1)
#	define GD_DEBUG_ASSERT_FALSE GD_DISABLED_FALSE_ASSERT
#	define GD_DEBUG_ASSERT GD_DISABLED_ASSERT
#	define GD_ASSERT_FALSE GD_ENABLED_FALSE_ASSERT
#	define GD_ASSERT GD_ENABLED_ASSERT
//endif	// if (GD_ASSERT_LEVEL == 1)
#elif (GD_ASSERT_LEVEL == 2)
#	define GD_DEBUG_ASSERT_FALSE GD_ENABLED_FALSE_ASSERT
#	define GD_DEBUG_ASSERT GD_ENABLED_ASSERT
#	define GD_ASSERT_FALSE GD_ENABLED_FALSE_ASSERT
#	define GD_ASSERT GD_ENABLED_ASSERT
//endif	// if (GD_ASSERT_LEVEL == 2)
#else	// *** Switch on GD_ASSERT_LEVEL ***
#	error "Unknown assertion level specified"
#endif	// *** Switch on GD_ASSERT_LEVEL ***

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Common assertion checks.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Code block is not implemented.
#define GD_NOT_IMPLEMENTED() GD_DEBUG_ASSERT_FALSE("A part or whole function '%s' not implemented (and probably never would be :[).", __FUNCTION__)
#define GD_NOT_SUPPORTED() GD_DEBUG_ASSERT_FALSE("A part or whole function '%s' is not supported.", __FUNCTION__)

// Asserts without messages.
#define GD_ASSERT_DEFAULT_MESSAGE ("*** Someone is too lazy to write a message for every assert ***")
#define GD_DEBUG_ASSERT_FALSE_NO_MESSAGE() GD_DEBUG_ASSERT_FALSE(GD_ASSERT_DEFAULT_MESSAGE)
#define GD_DEBUG_ASSERT_NO_MESSAGE(Expression) GD_DEBUG_ASSERT(Expression, GD_ASSERT_DEFAULT_MESSAGE)
#define GD_ASSERT_NO_MESSAGE(Expression) GD_ASSERT(Expression, GD_ASSERT_DEFAULT_MESSAGE)

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Redefining standard C assertion.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if (!defined(GD_CORE_DIAGNOSTICS_ASSERT_CPP))
#	undef assert
#	define assert(Expression) GD_DEBUG_ASSERT(Expression, "*** STD-C ASSERT MACRO ***")
#endif	// if (!defined(GD_CORE_DIAGNOSTICS_ASSERT_CPP))
