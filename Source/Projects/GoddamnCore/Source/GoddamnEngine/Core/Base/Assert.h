// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Base/Assert.h
 * @note This file should be never directly included, please consider using <GoddamnEngine/Include.h> instead.
 * Contains error-checking mechanism definitions.
 */
#pragma once
#if !defined(GD_INSIDE_INCLUDE_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Include.h> instead.
#endif	// if !defined(GD_INSIDE_INCLUDE_H)

#include <cstdarg>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**	
	// ******                            Common Assert macros.                                 ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**	

	/*! 
	 * Holds all assertion information.
	 */
	struct DebugAssertData final : public TNonCopyable
	{
	public:
		CStr    AssertUnformattedMessage;
		va_list AssertMessageFormat;
		CStr    AssertFileName;
		CStr    AssertFunctionName;
		UInt32  AssertLineNumber;
		CStr    AssertExpression;
		bool    AssertShouldBeAlwaysIgnored;

	public:
		GDINL DebugAssertData(CStr const unformattedMessage, CStr const assertFileName, CStr const assertFunctionName, UInt32 const assertLineNumber, CStr const assertExpression = nullptr)
			: AssertUnformattedMessage(unformattedMessage), AssertMessageFormat{}
			, AssertFileName(assertFileName), AssertFunctionName(assertFunctionName), AssertLineNumber(assertLineNumber)
			, AssertExpression(assertExpression), AssertShouldBeAlwaysIgnored(false)
		{
		}
	};	// struct PlatformAssertData

	/*! 
	 * Describes all possible user reactions on the assert.
	 */
	enum DebugAssertDialogResult : UInt8
	{
		#define GD_VERIFY_DIALOG_TITLE			  "Assert Failed"
		#define GD_VERIFY_DIALOG_GROUPBOX_TEXT	  "What the mess is going on?"
		#define GD_VERIFY_DIALOG_DESCRIPTION_TEXT "Whoa, seems that you have just broken the world's most stable piece of code!\n" \
				"Seriously, some code assertion failed. This may cause unstable work and etc."

		VERIFY_DIALOG_BTN_ABORT,
		#define GD_VERIFY_DIALOG_BTN_ABORT_TEXT "Abort"

		VERIFY_DIALOG_BTN_RETRY,
		#define GD_VERIFY_DIALOG_BTN_RETRY_TEXT "Retry"

		VERIFY_DIALOG_BTN_IGNORE,
		#define GD_VERIFY_DIALOG_BTN_IGNORE_TEXT "Ignore"	

		VERIFY_DIALOG_BTN_IGNORE_ALL,
		#define GD_VERIFY_DIALOG_BTN_IGNORE_ALL_TEXT "Ignore All"

		VERIFY_DIALOG_BTN_REPORT,
		#define GD_VERIFY_DIALOG_BTN_REPORT_TEXT "Report an Issue"

		VERIFY_DIALOG_BTN_BREAK,
		#define GD_VERIFY_DIALOG_BTN_BREAK_TEXT "Break"

		VERIFY_DIALOG_BTN_UNKNOWN,
		VERIFY_DIALOG_BTNS_COUNT = VERIFY_DIALOG_BTN_UNKNOWN,
	};	// enum DebugAssertDialogResult

	/*!
	 * Provides inner functionality for handling fatal asserts. Should not be invoked directly.
	 *
	 * @param data Pointer to assertion description data structure.
	 * @param args Arguments for formatting Message string.
	 */
	//! @{
	GD_NORETURN GDAPI extern void DebugHandleFatalAssertDataVa(DebugAssertData const* const data, va_list const args);
	GD_NORETURN GDINL extern void DebugHandleFatalAssertData(DebugAssertData const* const data, ...)
	{
		va_list args; 
		va_start(args, data);
		DebugHandleFatalAssertDataVa(data, args);
	//	va_end(args);
	}
	//! @}

	/*!
	 * Provides inner functionality for handling regular asserts. Should not be invoked directly.
	 *
	 * @param data Pointer to assertion description data structure.
	 * @param args Arguments for formatting Message string.
	 */
	//! @{
	GDAPI extern DebugAssertDialogResult DebugHandleAssertDataVa(DebugAssertData const* const data, va_list const args);
	GDINL extern DebugAssertDialogResult DebugHandleAssertData(DebugAssertData const* const data, ...)
	{
		va_list args; 
		va_start(args, data);
		auto const assertDialogResult = DebugHandleAssertDataVa(data, args);
		va_end(args);
		return assertDialogResult;
	}
	//! @}

#if GD_TESTING_ENABLED

	#define GD_ENABLED_FALSE_VERIFY gd_testing_assert_false
	#define GD_ENABLED_VERIFY gd_testing_assert

#else

	/*!
	 * @brief Defines an enabled fatal assertion. 
	 * Reports a failure via Message box (if is available on target platform) and asks if user wants to send it to tracker.
	 *
	 * @param message A formatable string that describes what is going on.
	 */
	#define GD_ENABLED_FALSE_VERIFY(message, ...) \
		do { \
			GD_USING_NAMESPACE; \
			DebugAssertData static const fatalAssertData(message, __FILE__, __FUNCTION__, __LINE__); \
			DebugHandleFatalAssertData(&fatalAssertData, ##__VA_ARGS__);  \
		} while (false)

	/*!
	 * @brief Defines a enabled assertion. 
	 * Reports a failure via Message box (if is available on target platform) and asks if user wants 
	 * to abort application (control is redirected to failure reporter then with provide of feedback via tracker), 
	 * or to debug application, or to just to ignore this assertion once or forever during this launch.
	 *
	 * @param expression The Expression that would be evaluated.
	 * @param message A formatable string that describes what is going on.
	 */
	#define GD_ENABLED_VERIFY(expression, message, ...) \
		do { \
			GD_USING_NAMESPACE; \
			__analysis_assume(expression); \
			DebugAssertData static const assertData(message, __FILE__, __FUNCTION__, __LINE__, #expression); \
			while ((!(expression)) && (!assertData.AssertShouldBeAlwaysIgnored)) \
			{ \
				auto const assertState = DebugHandleAssertData(&assertData, ##__VA_ARGS__); \
				if (assertState == VERIFY_DIALOG_BTN_BREAK) \
				{ \
					__debugbreak(); \
				} \
				else if (assertState != VERIFY_DIALOG_BTN_RETRY) \
				{ \
					break; \
				} \
			} \
		} while (false)

#endif	// if GD_TESTING_

	/*!
	 * @brief Defines a disabled fatal assertion. 
	 * Does nothing.
	 *
	 * @param message Dummy Message.
	 */
	#define GD_DISABLED_FALSE_VERIFY(...) \
		do { \
			static_cast<void>(false); \
			abort(); \
		} while (false)

	/*!
	 * @brief Defines a disabled assertion.
	 * Does nothing, just validates expression at compile-time.
	 *
	 * @param expression The Expression that would be evaluated.
	 * @param message A formatable string that describes what is going on.
	 */
	#define GD_DISABLED_VERIFY(expression, ...) \
		do { \
			GD_USING_NAMESPACE; \
			__analysis_assume(expression); \
			static_cast<void>(sizeof(expression)); \
		} while (false)

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**	
	// ******                             Debug Assert macros.                                 ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**	

#if GD_DEBUG

	/*!
	 * Defines a fatal assertion that is enabled only in debug mode. 
	 * @param message A formatable string that describes what is going on.
	 */
	#define GD_DEBUG_VERIFY_FALSE GD_ENABLED_FALSE_VERIFY
	
	/*!
	 * Defines a regular assertion that is enabled only in debug mode. 
	 *
	 * @param expression The Expression that would be evaluated.
	 * @param message A formatable string that describes what is going on.
	 */
	#define GD_DEBUG_VERIFY GD_ENABLED_VERIFY

#else	// if GD_DEBUG

	/*!
	 * Defines a fatal assertion that is enabled only in debug mode. 
	 * @param message A formatable string that describes what is going on.
	 */
	#define GD_DEBUG_VERIFY_FALSE GD_DISABLED_FALSE_VERIFY
	
	/*!
	 * Defines a regular assertion that is enabled only in debug mode. 
	 *
	 * @param expression The Expression that would be evaluated.
	 * @param message A formatable string that describes what is going on.
	 */
	#define GD_DEBUG_VERIFY GD_DISABLED_VERIFY

#endif	// if GD_DEBUG

	/*!
	 * Defines a fatal assertion. 
	 * @param message A formatable string that describes what is going on.
	 */
	#define GD_VERIFY_FALSE GD_ENABLED_FALSE_VERIFY
	
	/*!
	 * Defines a regular assertion.
	 *
	 * @param expression The Expression that would be evaluated.
	 * @param message A formatable string that describes what is going on.
	 */
	#define GD_VERIFY GD_ENABLED_VERIFY

	/*!
	 * Defines a fatal "Not Implemented" assertion. 
	 */
	#define GD_NOT_IMPLEMENTED() GD_DEBUG_VERIFY_FALSE("A part or whole function '%s' not implemented.", __FUNCTION__)

	/*!
	 * Defines a fatal "Not Supported" assertion. 
	 */
	#define GD_NOT_SUPPORTED() GD_DEBUG_VERIFY_FALSE("A part or whole function '%s' is not supported.", __FUNCTION__)

GD_NAMESPACE_END
