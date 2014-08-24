/// ==========================================================================================
/// Assertion.h: Assertation mechanism implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.01.2014  - Created by James Jhuighuy
/// ==========================================================================================

/// Note: code inside this file needs to be asserted too. To do that use defult assert macro.
#define GD_CORE_DIAGNOSTICS_ASSERTION_CPP
#include <GoddamnEngine/Include.h>

#include <GoddamnEngine/Core/Diagnostics/Assertion/Assertion.h>

/// ------------------------------------------------------------------------------------------
/// We are inside assertion implementation source, so we cannot use our asserts in order 
/// of possible cycle dependencies.
#undef GD_DEBUG_ASSERT_FALSE
#undef GD_DEBUG_ASSERT
#undef GD_ASSERT
#define GD_ASSERT(Expression, Message, ...) assert((Expression) && (Message))
#define GD_DEBUG_ASSERT(Expression, Message, ...) assert((Expression) && (Message))
#define GD_DEBUG_ASSERT_FALSE(Message, ...) \
	do { \
		assert(false && (Message)); \
		::abort(); \
	} while(false)

#if ((!defined(GD_PLATFORM_API_COCOA)) || (defined(GD_CORE_DIAGNOSTICS_ASSERTION_MM)))

#include <GoddamnEngine/Core/Text/String/String.h>

/// ==========================================================================================
/// Cross-platform assertion cache.
/// ==========================================================================================

GD_NAMESPACE_BEGIN

	struct AssertionCache final
	{
		FatalAssertionData* TheAssertionData;
		Str FormattedMessage;
		bool IsEngineAssertion;
	};	// struct AssertionCache

GD_NAMESPACE_END

/// ==========================================================================================
/// Here comes real hardcore inlined dialog box programming.
/// God is with us!
/// ==========================================================================================
#if (defined(GD_PLATFORM_WINDOWS))
#	include <Windows.h>
#	include <tchar.h>

/// ------------------------------------------------------------------------------------------
/// Win32 ID classes.
/// ------------------------------------------------------------------------------------------

#	define GD_WINAPI_BUTTON			0x0080
#	define GD_WINAPI_INFOPANEL		GD_WINAPI_BUTTON
#	define GD_WINAPI_TEXTEDIT		0x0081
#	define GD_WINAPI_STATIC_TEXT	0x0082

/// ------------------------------------------------------------------------------------------
/// Win32 Dialog builder helpers.
/// ------------------------------------------------------------------------------------------

/// Some glue magic.
#define GD_GLUE2(A, B) A ## B
#define GD_GLUE(A, B) GD_GLUE2(A, B)

/// Defines initial dialog window header.
#define GD_DEFINE_DIALOG_HEADER(TheTitle, TheFont) \
	DLGTEMPLATE Data; \
	WORD Menu; \
	WORD Class; \
	WCHAR Title[sizeof(TheTitle) / 2]; \
	WORD PointSize; \
	WCHAR Font[sizeof(TheFont) / 2] \

/// Defines some item inside dialog window.
#define GD_DEFINE_DIALOG_ITEM(Text) DialogItem<sizeof(Text) / 2> GD_GLUE(SomeDialogItemDefinedAtLine, __LINE__)

GD_NAMESPACE_BEGIN

	template<size_t const TitleSize>
	struct DialogItem final
	{
		DWORD Style;
		DWORD StyleEx;
		short x, y, cx, cy;
		WORD ID;
		WORD SysClass;
		WORD IDClass;
		WCHAR Text[TitleSize];
		WORD CreationDummy;
	};	// struct DialogItem 

GD_NAMESPACE_END

/// ------------------------------------------------------------------------------------------
/// Assertion dialog's IDs.
/// ------------------------------------------------------------------------------------------

#define GD_ASSERTION_DLG_IDD				101
#define GD_ASSERTION_DLG_TITLE				L"Assertion Failed"
#define GD_ASSERTION_DLG_FONT				L"MS Sans Serif"
#define GD_ASSERTION_DLG_ITEMS_COUNT		9
#define GD_ASSERTION_DLG_TEXTEDIT_IDC		1000	//	TextEdit into which the debug message is been written.
#define GD_ASSERTION_DLG_TEXTEDIT_TEXT		L""
#define GD_ASSERTION_DLG_BTNABORT_IDC		1001	//	"Abort" Button.
#define GD_ASSERTION_DLG_BTNABORT_TEXT		L"Abort"
#define GD_ASSERTION_DLG_BTNRETRY_IDC		1002	//	"Retry" Button.
#define GD_ASSERTION_DLG_BTNRETRY_TEXT		L"Retry"
#define GD_ASSERTION_DLG_BTNIGNORE_IDC		1003	//	"Ignore" Button.
#define GD_ASSERTION_DLG_BTNIGNORE_TEXT		L"Ignore"	
#define GD_ASSERTION_DLG_BTNIGNOREALL_IDC	1004	//	"Ignore All" Button.
#define GD_ASSERTION_DLG_BTNIGNOREALL_TEXT	L"Ignore All"
#define GD_ASSERTION_DLG_BTNREPORT_IDC		1005	//	"Ignore" Button.
#define GD_ASSERTION_DLG_BTNREPORT_TEXT		L"Report to BugTracker"
#define GD_ASSERTION_DLG_BTNBREAK_IDC		1006	//	"Break" Button.
#define GD_ASSERTION_DLG_BTNBREAK_TEXT		L"Break"
#define GD_ASSERTION_DLG_GROUPBOX_IDC		0		//	Groupbox to handle text edit and label.
#define GD_ASSERTION_DLG_GROUPBOX_TEXT		L"What the mess is going on?"
#define GD_ASSERTION_DLG_DESCRIPTION_IDC	0		//	Description of a what mess is going on.
#define GD_ASSERTION_DLG_DESCRIPTION_TEXT	L"Whoa, seems that you have just broken the world's most stable piece of code!\nSeriosly, some code assertion failed. This may cause unstable work and etc."

/// ------------------------------------------------------------------------------------------
/// Assertion dialog's data.
/// ------------------------------------------------------------------------------------------

GD_NAMESPACE_BEGIN

#pragma pack(push, 4) 
#define GD_TEXTEDIT_PROPS ES_MULTILINE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_READONLY
	struct AssertionDialogDataType final
	{
		GD_DEFINE_DIALOG_HEADER(GD_ASSERTION_DLG_TITLE, GD_ASSERTION_DLG_FONT);
		GD_DEFINE_DIALOG_ITEM(GD_ASSERTION_DLG_BTNABORT_TEXT);
		GD_DEFINE_DIALOG_ITEM(GD_ASSERTION_DLG_BTNRETRY_TEXT);
		GD_DEFINE_DIALOG_ITEM(GD_ASSERTION_DLG_BTNIGNORE_TEXT);
		GD_DEFINE_DIALOG_ITEM(GD_ASSERTION_DLG_BTNIGNOREALL_TEXT);
		GD_DEFINE_DIALOG_ITEM(GD_ASSERTION_DLG_BTNREPORT_TEXT);
		GD_DEFINE_DIALOG_ITEM(GD_ASSERTION_DLG_BTNBREAK_TEXT);
		GD_DEFINE_DIALOG_ITEM(GD_ASSERTION_DLG_TEXTEDIT_TEXT);
		GD_DEFINE_DIALOG_ITEM(GD_ASSERTION_DLG_GROUPBOX_TEXT);
		GD_DEFINE_DIALOG_ITEM(GD_ASSERTION_DLG_DESCRIPTION_TEXT);
	};	// struct AssertionDialogDataType
	AssertionDialogDataType static const AssertionDialogData = {
		{ DS_SETFOREGROUND | DS_MODALFRAME | DS_3DLOOK | DS_SETFONT | DS_CENTER | WS_POPUP | WS_CAPTION | WS_VISIBLE, 0, GD_ASSERTION_DLG_ITEMS_COUNT, 0, 0, 378, 173 }, 0, 0, GD_ASSERTION_DLG_TITLE, 8, GD_ASSERTION_DLG_FONT,
		{ WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, 0, 302,   7,  73,  14, GD_ASSERTION_DLG_BTNABORT_IDC    , 0xFFFF, GD_WINAPI_BUTTON     , GD_ASSERTION_DLG_BTNABORT_TEXT    , 0 },
		{ WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, 0, 302,  27,  73,  14, GD_ASSERTION_DLG_BTNRETRY_IDC    , 0xFFFF, GD_WINAPI_BUTTON     , GD_ASSERTION_DLG_BTNRETRY_TEXT    , 0 },
		{ WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, 0, 302,  47,  73,  14, GD_ASSERTION_DLG_BTNIGNORE_IDC   , 0xFFFF, GD_WINAPI_BUTTON     , GD_ASSERTION_DLG_BTNIGNORE_TEXT   , 0 },
		{ WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, 0, 302,  67,  73,  14, GD_ASSERTION_DLG_BTNIGNOREALL_IDC, 0xFFFF, GD_WINAPI_BUTTON     , GD_ASSERTION_DLG_BTNIGNOREALL_TEXT, 0 },
		{ WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, 0, 302,  87,  73,  14, GD_ASSERTION_DLG_BTNREPORT_IDC   , 0xFFFF, GD_WINAPI_BUTTON     , GD_ASSERTION_DLG_BTNREPORT_TEXT   , 0 },
		{ WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, 0, 302, 107,  73,  14, GD_ASSERTION_DLG_BTNBREAK_IDC    , 0xFFFF, GD_WINAPI_BUTTON     , GD_ASSERTION_DLG_BTNBREAK_TEXT    , 0 },
		{ WS_CHILD | WS_VISIBLE | GD_TEXTEDIT_PROPS         , 0,  11,  41, 283, 124, GD_ASSERTION_DLG_TEXTEDIT_IDC    , 0xFFFF, GD_WINAPI_TEXTEDIT   , GD_ASSERTION_DLG_TEXTEDIT_TEXT    , 0 },
		{ WS_CHILD | WS_VISIBLE | BS_GROUPBOX               , 0,   7,   4, 292, 165, GD_ASSERTION_DLG_GROUPBOX_IDC    , 0xFFFF, GD_WINAPI_INFOPANEL  , GD_ASSERTION_DLG_GROUPBOX_TEXT    , 0 },
		{ WS_CHILD | WS_VISIBLE                             , 0,  13,  17, 279,  20, GD_ASSERTION_DLG_DESCRIPTION_IDC , 0xFFFF, GD_WINAPI_STATIC_TEXT, GD_ASSERTION_DLG_DESCRIPTION_TEXT , 0 },
	};
#undef GD_TEXTEDIT_PROPS
#pragma pack(pop)

	INT_PTR CALLBACK AssertionDialogProc(HWND const HWindow, UINT const Message, WPARAM const WParam, LPARAM const LParam)
	{
		static RegularAssertionData* TheAssertionData = nullptr;
		switch (Message) {
			case WM_INITDIALOG: {
				TheAssertionData = reinterpret_cast<RegularAssertionData*>(LParam);
				GD_DEBUG_ASSERT(TheAssertionData != nullptr, "No Assertion Data specified");
				::SetWindowTextA(::GetDlgItem(HWindow, GD_ASSERTION_DLG_TEXTEDIT_IDC), TheAssertionData->FileName);
				::SendMessageA(HWindow, DM_SETDEFID, static_cast<WPARAM>(GD_ASSERTION_DLG_BTNBREAK_IDC), 0);
				::EnableWindow(::GetDlgItem(HWindow, GD_ASSERTION_DLG_BTNREPORT_IDC), FALSE);
			} break;
			case WM_COMMAND: {
				switch (LOWORD(WParam)) {
					case GD_ASSERTION_DLG_BTNABORT_IDC:
					case GD_ASSERTION_DLG_BTNRETRY_IDC:
					case GD_ASSERTION_DLG_BTNIGNORE_IDC:
					case GD_ASSERTION_DLG_BTNIGNOREALL_IDC:
					case GD_ASSERTION_DLG_BTNBREAK_IDC:
					case GD_ASSERTION_DLG_BTNREPORT_IDC: {
						GD_NOT_IMPLEMENTED();
					} break;
				}
			} break;
		}

		return INT_PTR(FALSE);
	}

GD_NAMESPACE_END
#endif	// if (defined(GD_PLATFORM_WINDOWS))

GD_NAMESPACE_BEGIN

	/// ------------------------------------------------------------------------------------------
	/// Fatal assertation mechanisms.
	/// ------------------------------------------------------------------------------------------

	void HandleFatalAssertionVa(FatalAssertionData const* const Data, va_list const Args)
	{
		HINSTANCE const HInstance = static_cast<HINSTANCE>(::GetModuleHandle(nullptr));
		::DialogBoxIndirectParamW(HInstance
			, reinterpret_cast<LPCDLGTEMPLATEW>(const_cast<AssertionDialogDataType*>(&AssertionDialogData))
			, nullptr, &AssertionDialogProc, reinterpret_cast<LPARAM>(Data));
		::abort();
	}

	/// ------------------------------------------------------------------------------------------
	/// Regular assertation mechanisms.
	/// ------------------------------------------------------------------------------------------

	AssertionState HandleRegularAssertionVa(RegularAssertionData* const Data, va_list const Args)
	{
	//	AssertionCache TheAssertionCache;
	//	TheAssertionCache.IsEngineAssertion = false;
	//	TheAssertionCache.

	//	::MessageBoxA(nullptr, "Some text here", "Some title here", 0);
		HandleFatalAssertionVa(Data, Args);
	//	return AssertionState::Break;
	}

GD_NAMESPACE_END

#endif	// if ((!defined(GD_PLATFORM_API_COCOA)) || (defined(GD_CORE_DIAGNOSTICS_ASSERTION_MM)))
