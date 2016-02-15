// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/System/Interaction/Assert.h
 * Cross-platform error handling algorithms.
 */
#pragma once
#define GD_INSIDE_ASSERT_H

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/System/Platform.h>

// ------------------------------------------------------------------------------------------
//! Text for Assert dialog UI. 
//! @note Due to some platform-dependent API these should be defined as macros.  
//! @{
#define GD_ASSERT_DIALOG_TITLE				 "Assert Failed"
#define GD_ASSERT_DIALOG_BTN_ABORT_TEXT		 "Abort"
#define GD_ASSERT_DIALOG_BTN_RETRY_TEXT		 "Retry"
#define GD_ASSERT_DIALOG_BTN_IGNORE_TEXT	 "Ignore"	
#define GD_ASSERT_DIALOG_BTN_IGNORE_ALL_TEXT "Ignore All"
#define GD_ASSERT_DIALOG_BTN_REPORT_TEXT	 "Report an Issue"
#define GD_ASSERT_DIALOG_BTN_BREAK_TEXT		 "Break"
#define GD_ASSERT_DIALOG_GROUPBOX_TEXT		 "What the mess is going on?"
#define GD_ASSERT_DIALOG_DESCRIPTION_TEXT	 "Whoa, seems that you have just broken the world's most stable piece of code!\n" \
	"Seriosly, some code assertion failed. This may cause unstable work and etc."
//! @}

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Describes all possible user reactions on the assert.
	enum DebugAssertDialogResult : UInt8
	{
		ASSERT_DIALOG_BTN_ABORT,
		ASSERT_DIALOG_BTN_RETRY,
		ASSERT_DIALOG_BTN_IGNORE,
		ASSERT_DIALOG_BTN_IGNORE_ALL,
		ASSERT_DIALOG_BTN_REPORT,
		ASSERT_DIALOG_BTN_BREAK,
		ASSERT_DIALOG_BTN_UNKNOWN,
		ASSERT_DIALOG_BTNS_COUNT = ASSERT_DIALOG_BTN_UNKNOWN,
	};	// enum PlatformAssertUIResult

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Holds all assertion information.
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

		GDINL DebugAssertData(CStr const unformattedMessage, CStr const assertFileName, CStr const assertFunctionName, UInt32 const assertLineNumber, CStr const assertExpression)
			: AssertUnformattedMessage(unformattedMessage), AssertMessageFormat(nullptr)
			, AssertFileName(assertFileName), AssertFunctionName(assertFunctionName), AssertLineNumber(assertLineNumber)
			, AssertExpression(assertExpression), AssertShouldBeAlwaysIgnored(false)
		{
		}
	};	// struct PlatformAssertData

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//! Shows assert diagnostics UI to user to determine what to do next with it.
	//! @param assertData Assert description.
	//! @returns What user decided to do with the error.
	GDAPI extern DebugAssertDialogResult AssertPlatformHandle(DebugAssertData const* const assertData);

	GDAPI extern bool AssertPlatformReport();

GD_NAMESPACE_END

#include GD_PLATFORM_INCLUDE(GoddamnEngine/Core/System/Interaction, Assert)
#undef GD_INSIDE_ASSERT_H
