/// ==========================================================================================
/// Assertion.h: Assertation mechanism definition.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.01.2014  - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_DIAGNOSTICS_ASSERTION
#define GD_CORE_DIAGNOSTICS_ASSERTION

#include <GoddamnEngine/Include.h>

/// Current interface is partly inspired by LibSDL 2.0 implementation of asserts
/// with modifications providing fatal failures reporting and etc.

#include <cstdlib>
#include <cstdarg>
#include <cassert>

/// ------------------------------------------------------------------------------------------
/// Cross-platform debug break.
/// ------------------------------------------------------------------------------------------

#if (defined(GD_DEBUG))
#	if (defined(GD_PLATFORM_API_WINAPI))
#		include <crtdbg.h>
#		define GD_DEBUG_BREAK() (::_CrtDbgBreak())
//	endif	// if (defined(GD_PLATFORM_API_WINAPI))
#	elif (defined(GD_COMPILER_GCC_COMPATIBLE) && (defined(GD_ARCHITECTURE_X64) || defined(GD_ARCHITECTURE_X86))
#		define GD_DEBUG_BREAK() (__asm__ __volatile__ ("int $3\n\t"))
//	endif	// if (defined(GD_COMPILER_GCC_COMPATIBLE) && (defined(GD_ARCHITECTURE_X64) || defined(GD_ARCHITECTURE_X86))
#	elif (defined(GD_COMPILER_GCC_COMPATIBLE) && (defined(GD_ARCHITECTURE_ARM32) || defined(GD_ARCHITECTURE_ARM64))
#		define GD_DEBUG_BREAK() (__asm__ __volatile__ (".inst 0xE7F001F0\n\t"))
//	endif	// if (defined(GD_COMPILER_GCC_COMPATIBLE) && (defined(GD_ARCHITECTURE_ARM32) || defined(GD_ARCHITECTURE_ARM64))
#	else	// *** Some other implementation ***
#		include <csignal>
#		define GD_DEBUG_BREAK() (::raise(SIGTRAP))
#	endif	// *** Some other implementation ***
#else	// if (defined(GD_DEBUG))
#	define GD_DEBUG_BREAK() (static_cast<void>(false))
#endif	// if (defined(GD_DEBUG))

/// ------------------------------------------------------------------------------------------
/// Fatal Assertation mechanisms.
/// ------------------------------------------------------------------------------------------

/// Defines a disabled fatal assertion.
/// Does nothing.
#define GD_DISABLED_FALSE_ASSERT(Message, ...) \
	do { \
		(static_cast<void>(false)); \
	} while (false)

/// Defines an enabled fatal assertion.
/// Reports a failure via message box (if is available on target platform) and asks if user wants
/// to send it to tracker.
#define GD_ENABLED_FALSE_ASSERT(Message, ...) \
	do { \
		GD_USING_NAMESPACE; \
		FatalAssertionData static const TheAssertionData(__FILE__, __FUNCTION__, __LINE__); \
		HandleFatalAssertion(&TheAssertionData, __VA_ARGS__);  \
	} while (false)

#if (!defined(GD_DOCUMENTATION))
GD_NAMESPACE_BEGIN

	/// Storing the whole assertion data in a single structure. Saving more stack space on 
	/// failure reporter call.
	struct FatalAssertionData
	{
		GD_CLASS_UNASSIGNABLE(FatalAssertionData);

	public:
		Str const FileName;
		Str const FunctionName;
		size_t const Line;

		GDINL FatalAssertionData(Str const FileName, Str const FunctionName, size_t const Line)
			: FileName(FileName), FunctionName(FunctionName), Line(Line)
		{
		}
	};	// struct FatalAssertionData

	/// Provides inner functionality for handling fatal assertions.
	/// Should not be invoked directly.
	GD_NORETURN GDAPI extern void HandleFatalAssertionVa(FatalAssertionData const* const Data, va_list const Args);
	GD_NORETURN GDINL extern void HandleFatalAssertion(FatalAssertionData const* const Data, ...)
	{
		va_list Args; va_start(Args, Data);
		HandleFatalAssertionVa(Data, Args);
	}

GD_NAMESPACE_END
#endif	// if (!defined(GD_DOCUMENTATION))

/// ------------------------------------------------------------------------------------------
/// Regular Assertation mechanisms.
/// ------------------------------------------------------------------------------------------

/// Defines a disabled assertion.
/// Does nothing, just validates expression at compile-time.
#define GD_DISABLED_ASSERT(Expression, Message, ...) \
	do { \
		GD_USING_NAMESPACE; \
		static_cast<void>(sizeof(Expression)); \
	} while (false)

/// Defines a enabled assertion.
/// Reports a failure via message box (if is available on target platform) and asks if user wants
/// to abort application (control is redirected to failure reporter then with provide of feedback via tracker),
/// or to debug application, or to just to ignore this assertion once or forever during this launch.
#define GD_ENABLED_ASSERT(Expression, Message, ...) \
	do { \
		GD_USING_NAMESPACE; \
		RegularAssertionData static TheAssertionData(__FILE__, __FUNCTION__, __LINE__, #Expression, Message); \
		while ((!(Expression)) && (!TheAssertionData.ShouldAlwaysIgnoreThisAssertion)) { \
			AssertionState const TheAssertionState = HandleRegularAssertion(&TheAssertionData, __VA_ARGS__); \
			if (TheAssertionState == AssertionState::Break) { \
				GD_DEBUG_BREAK(); \
			} else if (TheAssertionState != AssertionState::Retry) { \
				break; \
			} \
		} \
	} while (false)

#if (!defined(GD_DOCUMENTATION))
GD_NAMESPACE_BEGIN

	/// Storing the whole assertion data in a single structure. Saving more stack space on 
	/// failure reporter call.
	struct RegularAssertionData final : public FatalAssertionData
	{
		Str const Expression;
		Str const Message;
		bool ShouldAlwaysIgnoreThisAssertion;

		GDINL RegularAssertionData(Str const FileName, Str const FunctionName, size_t const Line, Str const Expression, Str const Message)
			: FatalAssertionData(FileName, FunctionName, Line)
			, Expression(Expression), Message(Message)
			, ShouldAlwaysIgnoreThisAssertion(false)
		{
		}
	};	// struct AssertionData

	/// Describes abilities that user can do with the failures.
	enum class AssertionState : UInt8
	{
		Abort,
		Retry,
		Ignore,
		IgnoreAll,
		Report,
		Break,
		Invalid
	};	// enum class AssertionState

	/// Provides inner functionality for handling regular assertions.
	/// Should not be invoked directly.
	GDAPI extern AssertionState HandleRegularAssertionVa(RegularAssertionData* const Data, va_list const Args);
	GDINL extern AssertionState HandleRegularAssertion(RegularAssertionData* const Data, ...)
	{
		va_list Args; va_start(Args, Data);
		return HandleRegularAssertionVa(Data, Args);
	}

GD_NAMESPACE_END
#endif	// if (!defined(GD_DOCUMENTATION))

/// ------------------------------------------------------------------------------------------
/// Assertion level handling.
/// ------------------------------------------------------------------------------------------

/// Mostly assertion level 0 is actual in release builds where all bugs are checked and no
/// "not implemented" places exist. Level 1 represents testing builds: code still contains bugs
/// and testers can provide feedback for developers. Level 2 is for developers and creating new 
/// code and catching fatal bugs and debug it.

#define GD_ASSERTION_LEVEL_DEBUG	(2)
#define GD_ASSERTION_LEVEL_TESTING	(1)
#define GD_ASSERTION_LEVEL_RELEASE	(0)
#if (!defined(GD_ASSERTION_LEVEL))
#	if (defined(GD_DEBUG))
#		define GD_ASSERTION_LEVEL GD_ASSERTION_LEVEL_DEBUG
//	endif	// if (defined(GD_DEBUG))
#	elif (defined(GD_RELEASE))
#		define GD_ASSERTION_LEVEL GD_ASSERTION_LEVEL_TESTING
//	endif	// if (defined(GD_RELEASE))
#	else	// *** Assertion level selection ***.
#		define GD_ASSERTION_LEVEL GD_ASSERTION_LEVEL_RELEASE
#	endif	// *** Assertion level selection ***.
#endif	// if (!defined(GD_ASSERTION_LEVEL))

#if (GD_ASSERTION_LEVEL == 0)
#	define GD_DEBUG_ASSERT_FALSE GD_DISABLED_FALSE_ASSERT
#	define GD_DEBUG_ASSERT GD_DISABLED_ASSERT
#	define GD_ASSERT GD_DISABLED_ASSERT
//endif	// if (GD_ASSERTION_LEVEL == 0)
#elif (GD_ASSERTION_LEVEL == 1)
#	define GD_DEBUG_ASSERT_FALSE GD_ENABLED_FALSE_ASSERT
#	define GD_DEBUG_ASSERT GD_ENABLED_ASSERT
#	define GD_ASSERT GD_DISABLED_ASSERT
//endif	// if (GD_ASSERTION_LEVEL == 1)
#elif (GD_ASSERTION_LEVEL == 2)
#	define GD_DEBUG_ASSERT_FALSE GD_ENABLED_FALSE_ASSERT
#	define GD_DEBUG_ASSERT GD_ENABLED_ASSERT
#	define GD_ASSERT GD_ENABLED_ASSERT
//endif	// if (GD_ASSERTION_LEVEL == 2)
#else	// *** Switch on GD_ASSERTION_LEVEL ***
#	error "Unknown assertion level specified"
#endif	// *** Switch on GD_ASSERTION_LEVEL ***

/// ------------------------------------------------------------------------------------------
/// Common assertion checks.
/// ------------------------------------------------------------------------------------------

// Code block is not implemented.
#define GD_NOT_IMPLEMENTED() GD_DEBUG_ASSERT_FALSE("A part or whole '"__FUNCTION__"' not implemented (and probably never would be :[)")

// Messegeless assertions.
#define GD_ASSERTION_DEFAULT_MESSAGE ("*** Someone is too lazy to write a message for every assert ***")
#define GD_DEBUG_ASSERT_FALSE_NO_MESSAGE() GD_DEBUG_ASSERT_FALSE(GD_ASSERTION_DEFAULT_MESSAGE)
#define GD_DEBUG_ASSERT_NO_MESSAGE(Expression) GD_DEBUG_ASSERT(Expression, GD_ASSERTION_DEFAULT_MESSAGE)
#define GD_ASSERT_NO_MESSAGE(Expression) GD_ASSERT(Expression, GD_ASSERTION_DEFAULT_MESSAGE)
#undef GD_ASSERTION_DEFAULT_MESSAGE

/// ------------------------------------------------------------------------------------------
/// Redefining standard C assertion.
/// ------------------------------------------------------------------------------------------

#if (!defined(GD_CORE_DIAGNOSTICS_ASSERTION_CPP))
#	undef assert
#	define assert(Expression) GD_DEBUG_ASSERT(Expression, "*** STD-C ASSERT MACRO ***")
#endif	// if (!defined(GD_CORE_DIAGNOSTICS_ASSERTION_CPP))

#endif	// ifndef GD_CORE_DIAGNOSTICS_ASSERTION
