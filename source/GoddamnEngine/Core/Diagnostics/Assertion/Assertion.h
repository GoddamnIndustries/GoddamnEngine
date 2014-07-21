/// ==========================================================================================
/// Assertion.h: Assertation mechanism definition
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.01.2014  - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_DIAGNOSTICS_ASSERTION
#define GD_CORE_DIAGNOSTICS_ASSERTION

#include <GoddamnEngine/Include.h>

#if (defined(GD_PLATFORM_DESKTOP))
#	define   GD_USE_CUSTOM_ASSERTS 1
#endif	// if (defined(GD_PLATFORM_DESKTOP))

#if (defined(GD_USE_CUSTOM_ASSERTS))
#	if (defined(GD_PLATFORM_WINDOWS) || defined(GD_PLATFORM_XBOX_ONE) || defined(GD_PLATFORM_WINPHONE))
#		include <crtdbg.h>
#	else	// if (defined(GD_PLATFORM_WINDOWS) || defined(GD_PLATFORM_XBOX_ONE) || defined(GD_PLATFORM_WINPHONE))
#		define _CrtDbgBreak abort 
#	endif	// if (defined(GD_PLATFORM_WINDOWS) || defined(GD_PLATFORM_XBOX_ONE) || defined(GD_PLATFORM_WINPHONE))
#	if (defined(GD_DEBUG))
#		define  GD_ASSERT(Expression, Message, ...) \
			do { \
				if (!(Expression)) \
					if (Assert(__FILE__ , __FUNCTION__, __LINE__ , #Expression, Message)) _CrtDbgBreak(); \
			} while (false)
#	else	// if (defined(GD_DEBUG))	// We do not need to store debug information in release executables
#		define  GD_ASSERT(Expression, Message, ...) \
			do { \
				if (!(Expression)) \
					if (Assert(""/*File*/,  ""/*Func*/ , 0/*Line*/,  ""/*Expr*/, Message)) abort(); \
			} while (false)
#	endif	// if (defined(GD_DEBUG))
#	undef  assert
#	define assert(Expression) GD_ASSERT(Expression, "Debug assertation failed!")
#else	// if (defined(GD_USE_CUSTOM_ASSERTS))
#	include <assert.h>
#	pragma warning(push)
#	pragma warning(disable : 4130)
#	define GD_ASSERT(Expression, Message) assert((Expression) && ((Message ": at " __FILE__ __FUNCTION__)))
#	pragma warning(pop)
#endif	// if (defined(GD_USE_CUSTOM_ASSERTS))

#if defined(GD_DEBUG)
#	define GD_DEBUG_ASSERT GD_ASSERT
#else 
#	define GD_DEBUG_ASSERT(...) 
#endif

#define GD_ASSERT_FALSE(Message, ...)		GD_ASSERT(false, Message, __VA_ARGS__)
#define GD_DEBUG_ASSERT_FALSE(Message, ...)	GD_DEBUG_ASSERT(false, Message, __VA_ARGS__)
#if (defined(GD_COMPILER_MSC))
#   define GD_NOT_IMPLEMENTED()           GD_ASSERT_FALSE(__FUNCTION__ " is not implemented.")
#else   // if (defined(GD_COMPILER_MSC))
#   define GD_NOT_IMPLEMENTED()           GD_ASSERT_FALSE("Some function is not unimplemented.")
#endif	// if (defined(GD_COMPILER_MSC))

GD_NAMESPACE_BEGIN

#if (defined(GD_USE_CUSTOM_ASSERTS) || defined(GD_DOCUMENTATION))  
	/// Throws debug assertional message
	/// @param FileName			Name of file assertion accrues (@c __FILE__ in common)
	/// @param LineNumber		Number of line in file assertion accrues (@c __LINE__ in common)
	/// @param ExpressionString	String for assertion expression (@c #Expression in common)
	/// @param Message			Message to display on assertion accrues
	/// @returns				True if debug break required
	GDAPI extern bool Assert(
		_In_ Str    const FileName,
		_In_ Str    const FunctionName,
		_In_ size_t const LineNumber,
		_In_ Str    const ExpressionString,
		_In_ Str    const Message
	);
#endif	// if (defined(GD_USE_CUSTOM_ASSERTS) || defined(GD_DOCUMENTATION))  

GD_NAMESPACE_END


#endif
