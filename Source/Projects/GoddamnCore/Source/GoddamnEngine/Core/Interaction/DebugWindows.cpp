// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Interaction/DebugWindows.cpp
 * Windows output device definitions.
 */
#include <GoddamnEngine/Core/Interaction/Debug.h>
#if GD_PLATFORM_WINDOWS

GD_NAMESPACE_BEGIN

	// ------------------------------------------------------------------------------------------
	// Error handling functions.
	// ------------------------------------------------------------------------------------------

	/*!
	 * Formats the assert message. Should not be invoked directly.
	 *
	 * @param data Pointer to assertion description data structure.
	 * @param args Arguments for formatting Message string.
	 */
	GDAPI CStr DebugWindows::FormatAssertMessage(DebugAssertData const* const data, va_list const args)
	{
		return DebugGeneric::FormatAssertMessage(data, args);
	}

	/*!
	 * Provides inner functionality for handling fatal asserts. Should not be invoked directly.
	 *
	 * @param data Pointer to assertion description data structure.
	 * @param args Arguments for formatting Message string.
	 */
	GDAPI void DebugWindows::HandleFatalAssertData(DebugAssertData const* const data, va_list const args)
	{
		HandleAssertData(data, args);
		ExitProcess(255);
	}

	__pragma(pack(push, 4))
	struct DebugAssertDialog
	{
		template<SizeTp TDialogItemTextLength>
		struct DialogItem final 
		{
			DWORD m_Style;
			DWORD m_StyleEx;
			SHORT m_X, m_Y, m_CX, m_CY;
			WORD  m_ID;
			WORD  m_SysClass;
			WORD  m_IDClass;
			WCHAR m_Text[TDialogItemTextLength];
			WORD  m_Dummy;
		};	// struct DialogItem

		enum
		{
			GD_VERIFY_WINDOWS_DIALOG_ITEM_ID = 1001,
			GD_VERIFY_WINDOWS_DIALOG_TEXTEDIT_ID = 10,
		};	// enum

		#define GD_VERIFY_WINDOWS_DIALOG_ITEM_CREATE(style, x, y, cx, cy, id, idClass, text) \
			DialogItem<GD_ARRAY_LENGTH(text)> GD_GLUE(m_DialogItem, __LINE__) \
				= { WS_CHILD | WS_VISIBLE | style, 0, x, y, cx, cy, id != 0 ? GD_VERIFY_WINDOWS_DIALOG_ITEM_ID + id : 0, 0xFFFF, idClass, GD_WIDEN(text), 0 };

		#define GD_VERIFY_WINDOWS_DIALOG_BUTTON_CREATE(x, y, cx, cy, id, text) \
			GD_VERIFY_WINDOWS_DIALOG_ITEM_CREATE(BS_PUSHBUTTON | WS_TABSTOP, x, y, cx, cy, id, 0x0080, text)

		#define GD_VERIFY_WINDOWS_DIALOG_TEXTEDIT_CREATE(x, y, cx, cy, id, text) \
			GD_VERIFY_WINDOWS_DIALOG_ITEM_CREATE(ES_MULTILINE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_READONLY, x, y, cx, cy, id, 0x0081, text)

		#define GD_VERIFY_WINDOWS_DIALOG_INFOPANEL_CREATE(x, y, cx, cy, id, text) \
			GD_VERIFY_WINDOWS_DIALOG_ITEM_CREATE(BS_GROUPBOX, x, y, cx, cy, id, 0x0080, text)

		#define GD_VERIFY_WINDOWS_DIALOG_STATICTEXT_CREATE(x, y, cx, cy, id, text) \
			GD_VERIFY_WINDOWS_DIALOG_ITEM_CREATE(0, x, y, cx, cy, id, 0x0082, text)

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

	};	// struct DebugAssertDialog
	__pragma(pack(pop))

	/*!
	 * Provides inner functionality for handling regular asserts. Should not be invoked directly.
	 *
	 * @param data Pointer to assertion description data structure.
	 * @param args Arguments for formatting Message string.
	 */
	GDAPI DebugAssertDialogResult DebugWindows::HandleAssertData(DebugAssertData const* const data, va_list const args)
	{
		return VERIFY_DIALOG_BTN_BREAK;
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_WINDOWS
