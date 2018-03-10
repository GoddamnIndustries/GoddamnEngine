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
//! @todo Fix include paths.
//#include <GoddamnEngine/Core/Platform/PlatformAssert.h>
#include <GoddamnEngine/Include.h>
#include <cassert>
#if GD_PLATFORM_WINDOWS

#include <Windows.h>

GD_NAMESPACE_BEGIN

	GD_MSVC_PRAGMA(pack(push, 4))
	template<SizeTp TDialogItemTextLength>
	struct WindowsDialogItem
	{
		DWORD Style;
		DWORD StyleEx;
		SHORT X, Y, CX, CY;
		WORD  ID;
		WORD  SysClass;
		WORD  IDClass;
		WCHAR Text[TDialogItemTextLength];
		WORD  Dummy;
	};	// struct WindowsDialogItem
	GD_MSVC_PRAGMA(pack(pop))

#define GD_WINDOWS_DIALOG_ITEM_CREATE(style, x, y, cx, cy, id, idClass, text) \
		WindowsDialogItem<GD_ARRAY_LENGTH(text)> GD_GLUE(DialogItem, __LINE__) \
			= { WS_CHILD | WS_VISIBLE | style, 0, x, y, cx, cy, id, 0xFFFF, idClass, GD_WIDEN(text), 0 }

#define GD_WINDOWS_DIALOG_BTN_CREATE(x, y, cx, cy, id, text) \
		GD_WINDOWS_DIALOG_ITEM_CREATE(BS_PUSHBUTTON | WS_TABSTOP, x, y, cx, cy, id, 0x0080, text)

#define GD_WINDOWS_DIALOG_TXE_CREATE(x, y, cx, cy, id, text) \
		GD_WINDOWS_DIALOG_ITEM_CREATE(ES_MULTILINE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_READONLY, x, y, cx, cy, id, 0x0081, text)

#define GD_WINDOWS_DIALOG_INF_CREATE(x, y, cx, cy, id, text) \
		GD_WINDOWS_DIALOG_ITEM_CREATE(BS_GROUPBOX, x, y, cx, cy, id, 0x0080, text)

#define GD_WINDOWS_DIALOG_LBL_CREATE(x, y, cx, cy, id, text) \
		GD_WINDOWS_DIALOG_ITEM_CREATE(0, x, y, cx, cy, id, 0x0082, text)

	enum
	{
		ASSERT_DIALOG_TEXTEDIT = 10,
	};	// enum

	GD_MSVC_PRAGMA(pack(push, 4))
	struct AssertWindowsDialog final : public TNonCopyable
	{
		DLGTEMPLATE DialogTemplate = { DS_SETFOREGROUND | DS_MODALFRAME | DS_3DLOOK | DS_SETFONT | DS_CENTER | WS_POPUP | WS_CAPTION | WS_VISIBLE, 0, 9, 0, 0, 378, 173 };
		WORD        DialogMenu = 0;
		WORD        DialogClass = 0;
		WCHAR       DialogTitle[sizeof(GD_ASSERT_DIALOG_TITLE)] = GD_WIDEN(GD_ASSERT_DIALOG_TITLE);
		WORD        DialogFontSize = 8;
		WCHAR       DialogFont[sizeof "Verdana"] = L"Verdana";
		struct {
			GD_WINDOWS_DIALOG_BTN_CREATE(302,   7,  73,  14, ASSERT_DIALOG_BTN_ABORT,      GD_ASSERT_DIALOG_BTN_ABORT_TEXT);
			GD_WINDOWS_DIALOG_BTN_CREATE(302,  27,  73,  14, ASSERT_DIALOG_BTN_RETRY,      GD_ASSERT_DIALOG_BTN_RETRY_TEXT);
			GD_WINDOWS_DIALOG_BTN_CREATE(302,  47,  73,  14, ASSERT_DIALOG_BTN_IGNORE,     GD_ASSERT_DIALOG_BTN_IGNORE_TEXT);
			GD_WINDOWS_DIALOG_BTN_CREATE(302,  67,  73,  14, ASSERT_DIALOG_BTN_IGNORE_ALL, GD_ASSERT_DIALOG_BTN_IGNORE_ALL_TEXT);
			GD_WINDOWS_DIALOG_BTN_CREATE(302,  87,  73,  14, ASSERT_DIALOG_BTN_REPORT,     GD_ASSERT_DIALOG_BTN_REPORT_TEXT);
			GD_WINDOWS_DIALOG_BTN_CREATE(302, 107,  73,  14, ASSERT_DIALOG_BTN_BREAK,      GD_ASSERT_DIALOG_BTN_BREAK_TEXT);
			GD_WINDOWS_DIALOG_TXE_CREATE( 11,  41, 283, 124, ASSERT_DIALOG_TEXTEDIT,       "");
			GD_WINDOWS_DIALOG_INF_CREATE(  7,   4, 292, 165, 0,                            GD_ASSERT_DIALOG_GROUP_BOX_TEXT);
			GD_WINDOWS_DIALOG_LBL_CREATE( 13,  17, 279,  20, 0,                            GD_ASSERT_DIALOG_DESCRIPTION_TEXT);
		};
	};	// struct AssertWindowsDialog
	GD_MSVC_PRAGMA(pack(pop))

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Runtime checks.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL WindowsPlatformAssert final : public IPlatformAssert
	{
	private:
		GDINT virtual AssertDialogResult RunAssertDialog(AssertData const *const data) override final;
		GDINT virtual void ReportAndExit(AssertData const* const assertData) override final;
	private:
		GDINT static INT_PTR CALLBACK AssertDialogProc(HWND const dlgHandle, UINT const dlgMessage, WPARAM const wParam, LPARAM const lParam);
	};	// class WindowsPlatformAssert

	GD_IMPLEMENT_SINGLETON(IPlatformAssert, WindowsPlatformAssert)

	// ------------------------------------------------------------------------------------------
	// Assert utilities.
	// ------------------------------------------------------------------------------------------

	/*!
	 * Runs an 'assert failed' dialog.
	 *
	 * @param assertData Assertion information.
	 * @returns Dialog result.
	 */
	GDINT AssertDialogResult WindowsPlatformAssert::RunAssertDialog(AssertData const* const assertData)
	{
		AssertWindowsDialog const assertWindowsDialogInstance = {};
		auto const assertDialogResult = DialogBoxIndirectParamW(GetModuleHandleA(nullptr), &assertWindowsDialogInstance.DialogTemplate, nullptr, &AssertDialogProc, reinterpret_cast<LPARAM>(assertData));
		if (assertDialogResult != -1)
		{
			return static_cast<AssertDialogResult>(assertDialogResult);
		}
		return ASSERT_DIALOG_BTN_ABORT;
	}

	/*!
	 * Reports that the assertion has failed and quits the application.
	 * @param assertData Assertion information.
 	 */
	GDINT void WindowsPlatformAssert::ReportAndExit(AssertData const* const assertData)
	{
		GD_NOT_USED_L(assertData);
		exit(-1);
	}

	/*!
	 * Handles messages for dialog window by OS.
	 */
	GDINT INT_PTR CALLBACK WindowsPlatformAssert::AssertDialogProc(HWND const dlgHandle, UINT const dlgMessage, WPARAM const wParam, LPARAM const lParam)
	{
		static_assert(sizeof(AssertDialogResult) <= sizeof LOWORD(wParam), "");
		static_assert(sizeof(AssertDialogResult) <= sizeof(INT_PTR), "");
		static AssertData const* assertData = nullptr;
		switch (dlgMessage)
		{
			case WM_INITDIALOG:
				{
					// Obtaining the assert data from the main application's code.
					assertData = reinterpret_cast<AssertData*>(lParam);
					if (assertData == nullptr)
					{
						return FALSE;
					}

					// Disabling some buttons n case of false assert.
					if (assertData->AssertExpression == nullptr)
					{
						auto const dlgButtonRetry = GetDlgItem(dlgHandle, ASSERT_DIALOG_BTN_RETRY);
						if (dlgButtonRetry != nullptr)
						{
							EnableWindow(dlgButtonRetry, FALSE);
						}
						auto const dlgButtonIgnore = GetDlgItem(dlgHandle, ASSERT_DIALOG_BTN_IGNORE);
						if (dlgButtonIgnore != nullptr)
						{
							EnableWindow(dlgButtonIgnore, FALSE);
						}
						auto const dlgButtonIgnoreAll = GetDlgItem(dlgHandle, ASSERT_DIALOG_BTN_IGNORE_ALL);
						if (dlgButtonIgnoreAll != nullptr)
						{
							EnableWindow(dlgButtonIgnoreAll, FALSE);
						}
					}
#if !GD_DEBUG 
					auto const dlgButtonBreak = GetDlgItem(dlgHandle, ASSERT_DIALOG_BTN_IGNORE_ALL);
					if (dlgButtonBreak != nullptr)
					{
						EnableWindow(dlgButtonBreak, FALSE);
					}
#endif	// if !GD_DEBUG 

					// Setting default button.
#if GD_DEBUG 
					SendMessageA(dlgHandle, DM_SETDEFID, static_cast<WPARAM>(ASSERT_DIALOG_BTN_BREAK), 0);
#else	// if GD_DEBUG
					SendMessageA(dlgHandle, DM_SETDEFID, static_cast<WPARAM>(ASSERT_DIALOG_BTN_REPORT), 0);
#endif	// if GD_DEBUG

					// Setting error message.
					CHAR assertMessage[16 * 1024] = {};
					if (assertData->AssertExpression != nullptr)
					{
						sprintf_s(assertMessage, "%s\r\n\r\nAt: \r\n[File]: %s\r\n[Line]: %u\r\n[Function]: %s\r\n[Expression]: %s\r\n"
							, assertData->AssertMessage, assertData->AssertFileName, assertData->AssertLineNumber, assertData->AssertFunctionName, assertData->AssertExpression);
					}
					else
					{
						sprintf_s(assertMessage, "%s\r\n\r\nAt: \r\n[File]: %s\r\n[Line]: %u\r\n[Function]: %s\r\n"
							, assertData->AssertMessage, assertData->AssertFileName, assertData->AssertLineNumber, assertData->AssertFunctionName);
					}
					auto const dlgTextEdit = GetDlgItem(dlgHandle, ASSERT_DIALOG_TEXTEDIT);
					if (dlgTextEdit != nullptr)
					{
						SetWindowTextA(dlgTextEdit, assertMessage);
					}

					Beep(750, 300);
					return FALSE;
				}
			case WM_COMMAND:
				{
					if (assertData == nullptr)
					{
						return FALSE;
					}
					auto const dlgItemID = LOWORD(wParam);
					if (dlgItemID >= ASSERT_DIALOG_BTN_ABORT && dlgItemID <= ASSERT_DIALOG_BTN_BREAK)
					{
						EndDialog(dlgHandle, static_cast<INT_PTR>(dlgItemID));
						assertData = nullptr;
					}
					return FALSE;
				}
			default:
				return FALSE;
		}
	}

GD_NAMESPACE_END

#endif  // if GD_PLATFORM_WINDOWS
