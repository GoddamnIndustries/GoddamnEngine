// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Assert/AssertWindows.cpp
 * Desktop Windows error handling algorithms.
 */
//#include <GoddamnEngine/Core/Interaction/Assert.h>
#if GD_PLATFORM_WINDOWS 

#include <Windows.h>
#include <assert.h>

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Assert-handling Dialog Box UI Items.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	/*!
	 * Describes a single item of the Dialog window.
	 * @warning Internal layout of following fields should not be changed. 
	 */
	__pragma(pack(push, 4))
	template<SizeTp TDialogItemTextLength>
	struct AssertWindowsDialogItem
	{
		DWORD m_Style;
		DWORD m_StyleEx;
		SHORT m_X, m_Y, m_CX, m_CY;
		WORD  m_ID;
		WORD  m_SysClass;
		WORD  m_IDClass;
		WCHAR m_Text[TDialogItemTextLength];
		WORD  m_Dummy;
	};	// struct AssertWindowsDialogItem
	__pragma(pack(pop))

	enum
	{
		GD_VERIFY_WINDOWS_DIALOG_ITEM_ID = 1001,
		GD_VERIFY_WINDOWS_DIALOG_TEXTEDIT_ID = 10,
	};	// enum

	#define GD_VERIFY_WINDOWS_DIALOG_ITEM_CREATE(style, x, y, cx, cy, id, idClass, text) \
		AssertWindowsDialogItem<GD_ARRAY_LENGTH(text)> GD_GLUE(m_DialogItem, __LINE__) \
			= { WS_CHILD | WS_VISIBLE | style, 0, x, y, cx, cy, id != 0 ? GD_VERIFY_WINDOWS_DIALOG_ITEM_ID + id : 0, 0xFFFF, idClass, GD_WIDEN(text), 0 };

	#define GD_VERIFY_WINDOWS_DIALOG_BUTTON_CREATE(x, y, cx, cy, id, text) \
		GD_VERIFY_WINDOWS_DIALOG_ITEM_CREATE(BS_PUSHBUTTON | WS_TABSTOP, x, y, cx, cy, id, 0x0080, text)

	#define GD_VERIFY_WINDOWS_DIALOG_TEXTEDIT_CREATE(x, y, cx, cy, id, text) \
		GD_VERIFY_WINDOWS_DIALOG_ITEM_CREATE(ES_MULTILINE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_READONLY, x, y, cx, cy, id, 0x0081, text)

	#define GD_VERIFY_WINDOWS_DIALOG_INFOPANEL_CREATE(x, y, cx, cy, id, text) \
		GD_VERIFY_WINDOWS_DIALOG_ITEM_CREATE(BS_GROUPBOX, x, y, cx, cy, id, 0x0080, text)

	#define GD_VERIFY_WINDOWS_DIALOG_STATICTEXT_CREATE(x, y, cx, cy, id, text) \
		GD_VERIFY_WINDOWS_DIALOG_ITEM_CREATE(0, x, y, cx, cy, id, 0x0082, text)

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Assert-handling Dialog Box UI.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#pragma pack(push, 4)

	// ------------------------------------------------------------------------------------------
	//! Defines the dialog box for the assert-handling UI.
	//! UI contains 6 action buttons, panel with error information and labels. 
	struct AssertWindowsDialog final : public TNonCopyable
	{
		DLGTEMPLATE m_DialogTemplate = { DS_SETFOREGROUND | DS_MODALFRAME | DS_3DLOOK | DS_SETFONT | DS_CENTER | WS_POPUP | WS_CAPTION | WS_VISIBLE, 0, 9, 0, 0, 378, 173 };
		WORD        m_DialogMenu     = 0;
		WORD        m_DialogClass    = 0;
		WCHAR       m_DialogTitle[sizeof GD_VERIFY_DIALOG_TITLE] = GD_WIDEN(GD_VERIFY_DIALOG_TITLE);
		WORD        m_DialogFontSize = 8;
		WCHAR       m_DialogFont[sizeof "Verdana"] = L"Verdana";
		struct {
			GD_VERIFY_WINDOWS_DIALOG_BUTTON_CREATE( 302,    7,  73,  14, VERIFY_DIALOG_BTN_ABORT,      GD_VERIFY_DIALOG_BTN_ABORT_TEXT);
			GD_VERIFY_WINDOWS_DIALOG_BUTTON_CREATE( 302,   27,  73,  14, VERIFY_DIALOG_BTN_RETRY,      GD_VERIFY_DIALOG_BTN_RETRY_TEXT);
			GD_VERIFY_WINDOWS_DIALOG_BUTTON_CREATE( 302,   47,  73,  14, VERIFY_DIALOG_BTN_IGNORE,     GD_VERIFY_DIALOG_BTN_IGNORE_TEXT);
			GD_VERIFY_WINDOWS_DIALOG_BUTTON_CREATE( 302,   67,  73,  14, VERIFY_DIALOG_BTN_IGNORE_ALL, GD_VERIFY_DIALOG_BTN_IGNORE_ALL_TEXT);
			GD_VERIFY_WINDOWS_DIALOG_BUTTON_CREATE( 302,   87,  73,  14, VERIFY_DIALOG_BTN_REPORT,     GD_VERIFY_DIALOG_BTN_REPORT_TEXT);
			GD_VERIFY_WINDOWS_DIALOG_BUTTON_CREATE( 302,  107,  73,  14, VERIFY_DIALOG_BTN_BREAK,      GD_VERIFY_DIALOG_BTN_BREAK_TEXT);
			GD_VERIFY_WINDOWS_DIALOG_TEXTEDIT_CREATE(11,   41, 283, 124, GD_VERIFY_WINDOWS_DIALOG_TEXTEDIT_ID, ""); // Would be set dynamically.
			GD_VERIFY_WINDOWS_DIALOG_INFOPANEL_CREATE(7,    4, 292, 165, /*ID, unused*/0,              GD_VERIFY_DIALOG_GROUPBOX_TEXT);
			GD_VERIFY_WINDOWS_DIALOG_STATICTEXT_CREATE(13, 17, 279,  20, /*ID, unused*/0,              GD_VERIFY_DIALOG_DESCRIPTION_TEXT);
		};
	};	// struct AssertWindowsDialog
	
#pragma pack(pop)

	// ------------------------------------------------------------------------------------------
	//! Handles messages for the Dialog box from OS.
	//! @returns The dialog result.
	GDINT static INT_PTR CALLBACK AssertWindowsDialogProc(HWND const dlgHandle, UINT const dlgMessage, WPARAM const wParam, LPARAM const lParam)
	{
		static DebugAssertData const* assertData = nullptr;
		if (dlgMessage == WM_INITDIALOG)	// Dialog initialization.
		{
			// Obtaining the assert data from the main application's code.
			assertData = reinterpret_cast<DebugAssertData*>(lParam);
			assert(assertData != nullptr && "No assert data could specified to the Assert-Handling dialog.");

			if (assertData->AssertExpression == nullptr)
			{
				// Semantically, fatal asserts could not be ignored nor retried.
				// So we need to disable 'Ignore' and 'Ignore All' buttons for this kinds of asserts.
				HWND const dlgButtonRetry = GetDlgItem(dlgHandle, GD_VERIFY_WINDOWS_DIALOG_ITEM_ID + VERIFY_DIALOG_BTN_RETRY);
				assert(dlgButtonRetry != nullptr && "'GetDlgItem' failed to find the 'Ignore' button by ID.");
				assert(EnableWindow(dlgButtonRetry, FALSE) == FALSE && "'EnableWindow' failed to disable the 'Ignore' button.");

				HWND const dlgButtonIgnore = GetDlgItem(dlgHandle, GD_VERIFY_WINDOWS_DIALOG_ITEM_ID + VERIFY_DIALOG_BTN_IGNORE);
				assert(dlgButtonIgnore != nullptr && "'GetDlgItem' failed to find the 'Ignore' button by ID.");
				assert(EnableWindow(dlgButtonIgnore, FALSE) == FALSE && "'EnableWindow' failed to disable the 'Ignore' button.");

				HWND const dlgButtonIgnoreAll = GetDlgItem(dlgHandle, GD_VERIFY_WINDOWS_DIALOG_ITEM_ID + VERIFY_DIALOG_BTN_IGNORE_ALL);
				assert(dlgButtonIgnoreAll != nullptr && "'GetDlgItem' failed to find the 'Ignore All' button by ID.");
				assert(EnableWindow(dlgButtonIgnoreAll, FALSE) == FALSE && "'EnableWindow' failed to disable the 'Ignore All' button.");
			}

			{
#if GD_DEBUG 
				// Setting 'Break' default action for our the dialog window in debug mode.
				assert(SendMessageA(dlgHandle, DM_SETDEFID, static_cast<WPARAM>(GD_VERIFY_WINDOWS_DIALOG_ITEM_ID + VERIFY_DIALOG_BTN_BREAK), 0));
#else	// if GD_DEBUG
				// In non-debug modes, debugger is not connected and we technically are unable to processed the
				// 'Break' button.
				HWND const dlgButtonBreak = GetDlgItem(dlgHandle, GD_VERIFY_WINDOWS_DIALOG_ITEM_ID + VERIFY_DIALOG_BTN_BREAK);
				assert(dlgButtonBreak != nullptr && "'GetDlgItem' failed to find the 'Break' button by ID.");
				assert(EnableWindow(dlgButtonBreak, FALSE) == FALSE && "'EnableWindow' failed to disable the 'Break' button.");

				// Setting 'Report' default action for our the dialog window.
				assert(SendMessageA(dlgHandle, DM_SETDEFID, static_cast<WPARAM>(GD_VERIFY_WINDOWS_DIALOG_ITEM_ID + VERIFY_DIALOG_BTN_REPORT), 0));
#endif	// if GD_DEBUG
			}

			// Safely reformatting the error message.
			CHAR assertMessage[16384] = "<<< ERROR DESCRIPTION HERE >>>";
			auto const assertMessageLength = vsprintf_s(assertMessage, assertData->AssertUnformattedMessage, assertData->AssertMessageFormat);
			assert(assertMessageLength >= 0 && "'vsprintf_s' Failed to reformat the assert message.");

			// Adding information about the location of the assert statement.
			auto const assertMessageDesc = assertMessage + assertMessageLength;
			auto const assertMessageDescLength = GD_ARRAY_LENGTH(assertMessage) - assertMessageLength;
			assert(sprintf_s(assertMessageDesc, assertMessageDescLength
				, "\r\n\r\nAt: \r\n[File]: %s\r\n[Line]: %u\r\n[Function]: %s"
				, assertData->AssertFileName, assertData->AssertLineNumber, assertData->AssertFunctionName) != 0);
			if (assertData->AssertExpression != nullptr)
			{
				assert(strcat_s(assertMessageDesc, assertMessageDescLength, "\r\n[Expression]: ") == 0);
				assert(strcat_s(assertMessageDesc, assertMessageDescLength, assertData->AssertExpression) == 0);
			}

			// Printing the error message to the dialog window and making some noise to make user shit with bricks.
			HWND const dlgTextEditWithMessage = GetDlgItem(dlgHandle, GD_VERIFY_WINDOWS_DIALOG_ITEM_ID + GD_VERIFY_WINDOWS_DIALOG_TEXTEDIT_ID);
			assert(dlgTextEditWithMessage != nullptr && "'GetDlgItem' failed to find the TextEdit by ID.");
			assert(SetWindowTextA(dlgTextEditWithMessage, assertMessage));
			assert(Beep(750, 300));
		}
		else if (dlgMessage == WM_COMMAND)	// Dialog clicks handling.
		{
			// Any command should be handled after Dialog UI was initialized.
			assert(assertData != nullptr && "'WM_COMMAND' message was received before 'WM_INITDIALOG'.");
			static_assert(sizeof(AssertDialogResult) <= sizeof LOWORD(wParam), "'AssertDialogResult' enum could not be carried by Win32 API.");
			static_assert(sizeof(AssertDialogResult) <= sizeof(INT_PTR), "'AssertDialogResult' enum could not be carried by Win32 API.");

			// User has clicked on some item of dialog. We need to process only button-clicks.
			auto const dlgItemID = LOWORD(wParam) - GD_VERIFY_WINDOWS_DIALOG_ITEM_ID;
			if (dlgItemID >= 0 && dlgItemID <= VERIFY_DIALOG_BTNS_COUNT)
			{
				assert(EndDialog(dlgHandle, static_cast<INT_PTR>(dlgItemID)) && "'EndDialog' failed.");
				assertData = nullptr;
			}
		}
		return static_cast<INT_PTR>(FALSE);
	}

	GDINL static CRITICAL_SECTION AssertWindowsDialogGetLock()
	{
		CRITICAL_SECTION assertDialogMutex = {};
		InitializeCriticalSection(&assertDialogMutex);
		return assertDialogMutex;
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Desktop Windows error handling algorithms.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//! Shows assert diagnostics UI to user to determine what to do next with it.
	//! @param assertData Assert description.
	//! @returns What user decided to do with the error.
	GDAPI extern AssertDialogResult AssertPlatformHandle(DebugAssertData const* const assertData)
	{
		static auto assertDialogMutex = AssertWindowsDialogGetLock();
		EnterCriticalSection(&assertDialogMutex);

		// Creating the dialog window and awaiting for user to get result..
		AssertWindowsDialog static const assertWindowsDialogInstance = {};
		auto const assertDialogResult = DialogBoxIndirectParamW(GetModuleHandleA(nullptr), &assertWindowsDialogInstance.m_DialogTemplate
			, nullptr, &AssertWindowsDialogProc, reinterpret_cast<LPARAM>(assertData));
		assert(assertDialogResult != -1 && "Failed to create the Assert-Handling dialog window.");

		LeaveCriticalSection(&assertDialogMutex);

		return static_cast<AssertDialogResult>(assertDialogResult);
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_WINDOWS
