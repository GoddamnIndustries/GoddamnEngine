// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
//
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file
 * Allocator implementation.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/Singleton.h>

#include <cstdlib>

#define GD_ASSERT_DIALOG_TITLE			  "Assert Failed"
#define GD_ASSERT_DIALOG_GROUP_BOX_TEXT	  "What the mess is going on?"
#define GD_ASSERT_DIALOG_DESCRIPTION_TEXT "Whoa, seems that you have just broken the world's most stable piece of code!\n" \
				                          "Seriously, some code assertion failed. This may cause unstable work and etc."

#define GD_ASSERT_DIALOG_BTN_ABORT_TEXT      "Abort"
#define GD_ASSERT_DIALOG_BTN_RETRY_TEXT      "Retry"
#define GD_ASSERT_DIALOG_BTN_IGNORE_TEXT     "Ignore"
#define GD_ASSERT_DIALOG_BTN_IGNORE_ALL_TEXT "Ignore All"
#define GD_ASSERT_DIALOG_BTN_REPORT_TEXT     "Report an Issue"
#define GD_ASSERT_DIALOG_BTN_BREAK_TEXT      "Break"

GD_NAMESPACE_BEGIN

	/*!
	 * Assertion information.
	 */
	struct AssertData final : public TNonCopyable
	{
	public:
		CStr   AssertMessage;
		CStr   AssertFileName;
		CStr   AssertFunctionName;
		UInt32 AssertLineNumber;
		CStr   AssertExpression;
		bool   AssertShouldBeAlwaysIgnored;
	public:
		GDINL AssertData(CStr const assertFileName, CStr const assertFunctionName, UInt32 const assertLineNumber, CStr const assertMessage = "Assert has failed", CStr const assertExpression = nullptr)
			: AssertMessage(assertMessage), AssertFileName(assertFileName), AssertFunctionName(assertFunctionName), AssertLineNumber(assertLineNumber), AssertExpression(assertExpression), AssertShouldBeAlwaysIgnored(false)
		{
		}
	public:
		GDINL static CStr WrapEmptyMessage(CStr const message = "Assert has failed")
		{
			return message;
		}
	};	// struct AssertData

	/*!
	 * User reactions on the assert.
	 */
	enum AssertDialogResult : UInt8
	{
		ASSERT_DIALOG_BTN_ABORT = 100,
		ASSERT_DIALOG_BTN_RETRY,
		ASSERT_DIALOG_BTN_IGNORE,
		ASSERT_DIALOG_BTN_IGNORE_ALL,
		ASSERT_DIALOG_BTN_REPORT,
		ASSERT_DIALOG_BTN_BREAK,
		ASSERT_DIALOG_BTN_UNKNOWN,
	};  // enum AssertDialogResult

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Runtime checks.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL IPlatformAssert : public Singleton<IPlatformAssert>
	{
	public:

		// ------------------------------------------------------------------------------------------
		// Assert utilities.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Runs an 'assert failed' dialog.
		 *
		 * @param assertData Assertion information.
		 * @returns Dialog result.
		 */
		GDINT virtual AssertDialogResult RunAssertDialog(AssertData const* const assertData) GD_PURE_VIRTUAL;

		/*!
		 * Reports that the assertion has failed and quits the application.
		 * @param assertData Assertion information.
		 */
		GD_NORETURN GDINT virtual void ReportAndExit(AssertData const* const assertData) GD_PURE_VIRTUAL;
	};	// class IPlatformAllocator

	template<>
	GDAPI IPlatformAssert& Singleton<IPlatformAssert>::Get();

	GD_NAMESPACE_END

/*!
 * @brief Defines an enabled fatal assertion.
 * Reports a failure via some UI window (if is available on target platform) and asks if user wants to send it to tracker.
 */
#define GD_ENABLED_FALSE_VERIFY(...) \
		do \
		{ \
			GD_USING_NAMESPACE; \
			AssertData static const fatalAssertData(__FILE__, __FUNCTION__, __LINE__, AssertData::WrapEmptyMessage(__VA_ARGS__), nullptr); \
			switch (IPlatformAssert::Get().RunAssertDialog(&fatalAssertData)) \
            { \
				case ASSERT_DIALOG_BTN_REPORT: \
					IPlatformAssert::Get().ReportAndExit(&fatalAssertData); \
				case ASSERT_DIALOG_BTN_BREAK:\
					__debugbreak(); \
					break; \
            } \
			abort(); \
		} while (false)

/*!
 * @brief Defines a enabled assertion.
 * Reports a failure via some UI (if is available on target platform) and asks if user wants
 * to abort application (control is redirected to failure reporter then with provide of feedback via tracker),
 * or to debug application, or to just to ignore this assertion once or forever during this launch.
 *
 * @param expression The Expression that would be evaluated.
 */
#define GD_ENABLED_VERIFY(expression, ...) \
		do \
		{ \
			GD_USING_NAMESPACE; \
			__analysis_assume(expression); \
			AssertData static assertData(__FILE__, __FUNCTION__, __LINE__, AssertData::WrapEmptyMessage(__VA_ARGS__), #expression); \
			while ((!(expression)) && (!assertData.AssertShouldBeAlwaysIgnored)) \
			{ \
				switch (IPlatformAssert::Get().RunAssertDialog(&assertData)) \
	            { \
					case ASSERT_DIALOG_BTN_ABORT: \
						abort(); \
					case ASSERT_DIALOG_BTN_IGNORE: \
						goto GD_GLUE(AssertBreakOuter, __LINE__); \
					case ASSERT_DIALOG_BTN_IGNORE_ALL: \
						assertData.AssertShouldBeAlwaysIgnored = true; \
						goto GD_GLUE(AssertBreakOuter, __LINE__); \
					case ASSERT_DIALOG_BTN_REPORT: \
						IPlatformAssert::Get().ReportAndExit(&assertData); \
					case ASSERT_DIALOG_BTN_BREAK:\
						__debugbreak(); \
						break; \
	            } \
			} \
		GD_GLUE(AssertBreakOuter, __LINE__):; \
		} while (false)

/*!
 * @brief Defines a disabled assertion.
 * Does nothing, just validates expression at compile-time.
 *
 * @param expression The Expression that would be evaluated.
 */
#define GD_DISABLED_VERIFY(expression, ...) \
		do \
		{ \
			GD_USING_NAMESPACE; \
			__analysis_assume(expression); \
			static_cast<void>(sizeof(expression)); \
		} while (false)

#if GD_DEBUG

	/*!
	 * Defines a fatal assertion that is enabled only in debug mode.
	 */
#	define GD_ASSERT_FALSE GD_ENABLED_FALSE_VERIFY
#   define GD_DEBUG_VERIFY_FALSE GD_ASSERT_FALSE

	/*!
	 * Defines a regular assertion that is enabled only in debug mode.
	 */
#	define GD_ASSERT GD_ENABLED_VERIFY
#   define GD_DEBUG_VERIFY GD_ASSERT

#else	// if GD_DEBUG

	/*!
	 * Defines a fatal assertion that is enabled only in debug mode.
	 */
#   define GD_ASSERT_FALSE GD_ENABLED_FALSE_VERIFY

	/*!
	 * Defines a regular assertion that is enabled only in debug mode.
	 */
#   define GD_ASSERT GD_DISABLED_VERIFY

#endif	// if GD_DEBUG

/*!
 * Defines a fatal assertion.
 */
#define GD_VERIFY_FALSE GD_ENABLED_FALSE_VERIFY

/*!
 * Defines a regular assertion.
 */
#define GD_VERIFY GD_ENABLED_VERIFY

/*!
 * Defines a fatal "Not Implemented" assertion.
 */
#define GD_NOT_IMPLEMENTED() GD_ASSERT_FALSE("A part or whole function is not implemented.")

/*!
 * Defines a fatal "Not Supported" assertion.
 */
#define GD_NOT_SUPPORTED() GD_ASSERT_FALSE("A part or whole function is not supported.")
