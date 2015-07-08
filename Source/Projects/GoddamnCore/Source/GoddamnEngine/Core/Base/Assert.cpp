// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Definitions/Assert.cpp
//! Contains Assert mechanism implementation.

// Note: code inside this file needs to be asserted too. To do that use default assert macro.
#define GD_CORE_DIAGNOSTICS_ASSERT_CPP
#include <GoddamnEngine/Include.h>

//#include <GoddamnEngine/Core/Misc/Path.h>
//#include <GoddamnEngine/Core/Misc/CString.h>
#include <GoddamnEngine/Core/Containers/String.h>

// ------------------------------------------------------------------------------------------
// We are inside assertion implementation source, so we cannot use our asserts in order 
// of possible cycle dependencies.
#undef GD_ENABLED_ASSERT
#undef GD_ENABLED_FALSE_ASSERT
#define GD_ENABLED_ASSERT(Expression, Message, ...) assert((Expression) && (Message))
#define GD_ENABLED_FALSE_ASSERT(Message, ...) \
	do { \
		assert(false && (Message)); \
		std::abort(); /*Simulating "noreturn" behavior. */ \
	} while(false)

#if (defined(GD_PLATFORM_API_MICROSOFT))
#	define GD_PLATFORM_LINEBREAK "\r\n"
#	define GD_PLATFORM_SLASH  "\\"
#else	// if (defined(GD_PLATFORM_API_MICROSOFT))
#	define GD_PLATFORM_LINEBREAK "\n"
#	define GD_PLATFORM_SLASH  "/"
#endif	// if (defined(GD_PLATFORM_API_MICROSOFT))

// ------------------------------------------------------------------------------------------
// Some text for our assertion UI. Defined as macros to support widening.
#define GD_ASSERT_DIALOG_TITLE				"Assert Failed"
#define GD_ASSERT_DIALOG_BTNABORT_TEXT		"Abort"
#define GD_ASSERT_DIALOG_BTNRETRY_TEXT		"Retry"
#define GD_ASSERT_DIALOG_BTNIGNORE_TEXT		"Ignore"	
#define GD_ASSERT_DIALOG_BTNIGNOREALL_TEXT	"Ignore All"
#define GD_ASSERT_DIALOG_BTNREPORT_TEXT		"Report an Issue"
#define GD_ASSERT_DIALOG_BTNBREAK_TEXT		"Break"
#define GD_ASSERT_DIALOG_GROUPBOX_TEXT		"What the mess is going on?"
#define GD_ASSERT_DIALOG_DESCRIPTION_TEXT	"Whoa, seems that you have just broken the world's most stable piece of code!\nSeriosly, some code assertion failed. This may cause unstable work and etc."

GD_NAMESPACE_BEGIN

	// ------------------------------------------------------------------------------------------	
	// Platform-dependent methods for handling/reporting assertions. This needs to be implemented.
	// ------------------------------------------------------------------------------------------	

	struct AssertCache;

	//! Should show UI that notifies user about assertion failure and would request a decision from him.
	//! @param TheAssertCache Assertion data.
	//! @returns The assertion status, selected by user.
	GDINT static AssertState HandleAssertImpl(AssertCache const* const TheAssertCache);

	//! Should report about a bug to developers and return false on failure.
	//! @returns False on reporting failure.
	GDINT static bool ReportAssertImpl(AssertCache const* const TheAssertCache);

	// ------------------------------------------------------------------------------------------	
	// Location of the assertion in code.
	// ------------------------------------------------------------------------------------------

	enum class AssertLocation : UInt8
	{
		EngineCode,
		GameCode,
		DependencyCode,
		Invalid,
	};	// enum class AssertLocation

	//! Returns the approximate location of assertion in the source of the engine.
	//!        This information is crucial for determining whether bug is located inside engine code (and we made mistakes)
	//!        or is cause by crooked hands of some game developers (no offenses, all assertion are caused by crookedness).
	//! @param FileName Name of the specified file.
	//! @returns The approximate location of assertion in the source of the engine.
	GDINT static AssertLocation GetAssertLocationForSource(CStr const FileName)
	{
		CStr const EngineSDKLocation = CString::Strstr(FileName, GD_PLATFORM_SLASH "GoddamnEngine" GD_PLATFORM_SLASH "source");
		if (EngineSDKLocation != nullptr)
		{
			if (CString::Strstr(EngineSDKLocation, GD_PLATFORM_SLASH "Dependencies") != nullptr)
			{
				return AssertLocation::DependencyCode;
			}
			else
			{
				return AssertLocation::EngineCode;
			}
		}
		else
		{
			return AssertLocation::GameCode;
		}
	}

	// ------------------------------------------------------------------------------------------
	// Reporting issues mechanisms.
	// ------------------------------------------------------------------------------------------

	struct AssertCache final
	{
		AssertLocation TheAssertLocation = AssertLocation::Invalid;
		FatalAssertData const* TheAssertData = nullptr;
		bool IsFatal = false;
		String FormattedMessage;
	};	// struct AssertCache

	static const char ReportGithubURL [] = "https://github.com/QuaternionStudios/GoddamnEngine/issues/new";

	void ReportAssert(AssertCache const* const TheAssertCache)
	{
		GD_DEBUG_ASSERT(ReportAssertImpl(TheAssertCache), "Failed to report an error");
	}

	//! ------------------------------------------------------------------------------------------
	//! Fatal assertion mechanisms.
	//! ------------------------------------------------------------------------------------------

	void HandleFatalAssertVa(FatalAssertData const* const Data, va_list Args)
	{
		//! @todo Restore code here
		//CriticalSection static CriticalAssertCriticalSection;
		//CriticalAssertCriticalSection.Enter();

		AssertCache TheAssertCache;
		TheAssertCache.IsFatal = true;
		TheAssertCache.TheAssertData = Data;
		TheAssertCache.FormattedMessage = String::Format((
			"Fatal error occurred!" GD_PLATFORM_LINEBREAK
			"At: " GD_PLATFORM_LINEBREAK
			"[File]: %s" GD_PLATFORM_LINEBREAK
			"[Line]: %u" GD_PLATFORM_LINEBREAK
			"[Function]: %s" GD_PLATFORM_LINEBREAK GD_PLATFORM_LINEBREAK
			"%s"), Data->FileName, Data->Line, Data->FunctionName, String::FormatVa(Data->Message, Args).CStr());

		AssertState const TheAssertState = HandleAssertImpl(&TheAssertCache);
		if (TheAssertState == AssertState::Report)
		{
			ReportAssert(&TheAssertCache);
		}

		std::exit(-1);
	}

	//! ------------------------------------------------------------------------------------------
	//! Regular assertion mechanisms.
	//! ------------------------------------------------------------------------------------------

	AssertState HandleRegularAssertVa(RegularAssertData* const Data, va_list Args)
	{
		bool static ShouldIgnoreAllRegularAsserts = false;
		if (!ShouldIgnoreAllRegularAsserts)
		{
			//! @todo Restore code here
			//CriticalSection static RegularAssertCriticalSection;
			//ScopedLock RegularAssertLock(RegularAssertCriticalSection);

			AssertCache TheAssertCache;
			TheAssertCache.TheAssertData = Data;
			TheAssertCache.TheAssertLocation = GetAssertLocationForSource(Data->FileName);
			TheAssertCache.FormattedMessage = String::Format((
				"%s" GD_PLATFORM_LINEBREAK
				GD_PLATFORM_LINEBREAK
				"At: " GD_PLATFORM_LINEBREAK
				"[File]: %s" GD_PLATFORM_LINEBREAK
				"[Line]: %u" GD_PLATFORM_LINEBREAK
				"[Function]: %s" GD_PLATFORM_LINEBREAK
				"[Expression]: %s" GD_PLATFORM_LINEBREAK
				), String::FormatVa(Data->Message, Args).CStr(), Data->FileName, Data->Line, Data->FunctionName, Data->Expression);

			AssertState const TheAssertState = HandleAssertImpl(&TheAssertCache);
			switch (TheAssertState)
			{
				case AssertState::Abort: {
					::fprintf(stderr, GD_PLATFORM_LINEBREAK "Aborting the application...");
					::fflush(stderr);
					::abort();
				} break;
				case AssertState::Report: {
					ReportAssert(&TheAssertCache);
				} break;
				case AssertState::Ignore: {
					Data->ShouldAlwaysIgnoreThisAssert = true;
				} break;
				case AssertState::IgnoreAll: {
					ShouldIgnoreAllRegularAsserts = true;
				} break;
				case AssertState::Retry:
				case AssertState::Break: {
					// Macros Handle here.
				} break;
				default: {
					GD_DEBUG_ASSERT_FALSE("Invalid assert state returned.");
				} break;
			}

			return TheAssertState;
		}
		else
		{
			Data->ShouldAlwaysIgnoreThisAssert = true;
			return AssertState::Ignore;
		}
	}

GD_NAMESPACE_END

// ==========================================================================================
// Assert UI implementation.
// ==========================================================================================

// ==========================================================================================
// Win32 API Dialog implementation.
// Here comes real hardcore inlined dialog box programming. God is with us!
// ==========================================================================================

#if GD_PLATFORM_WINDOWS
#pragma region Win32 Assert UI implementation.
#define GD_ASSERT_UI_IMPLEMENTED 1

#include <Windows.h>

// ------------------------------------------------------------------------------------------
// Win32 ID classes.
// ------------------------------------------------------------------------------------------

#define GD_WINAPI_BUTTON		0x0080
#define GD_WINAPI_INFOPANEL		GD_WINAPI_BUTTON
#define GD_WINAPI_TEXTEDIT		0x0081
#define GD_WINAPI_STATIC_TEXT	0x0082

// ------------------------------------------------------------------------------------------
// Win32 Dialog builder helpers.
// ------------------------------------------------------------------------------------------

//! Defines initial dialog window header.
//! @param TheTitle The title string.
//! @param TheFont The font string.
#define GD_DEFINE_DIALOG_HEADER(TheTitle, TheFont) \
	DLGTEMPLATE Data; \
	WORD Menu; \
	WORD Class; \
	WCHAR Title[sizeof(TheTitle) / 2]; \
	WORD PointSize; \
	WCHAR Font[sizeof(TheFont) / 2] \

//! Defines some item inside dialog window.
//! @param Text Some text that would be defined inside dialog.
#define GD_DEFINE_DIALOG_ITEM(Text) \
	DialogItem<sizeof(Text) / 2> GD_GLUE(SomeDialogItemDefinedAtLine, __LINE__)

GD_NAMESPACE_BEGIN

template<SizeTp const TitleSize>
struct DialogItem final
{
	DWORD Style;
	DWORD StyleEx;
	SHORT X, Y, CX, CY;
	WORD ID;
	WORD SysClass;
	WORD IDClass;
	WCHAR Text[TitleSize];
	WORD CreationDummy;
};	// struct DialogItem 

GD_NAMESPACE_END

// ------------------------------------------------------------------------------------------
// Assert dialog's IDs.
// ------------------------------------------------------------------------------------------

//! Some Dialog-Box constant.
//! @{
#define GD_ASSERT_DLG_IDD				101
#define GD_ASSERT_DLG_TITLE				GD_WIDEN(GD_ASSERT_DIALOG_TITLE)
#define GD_ASSERT_DLG_FONT				L"MS Sans Serif"
#define GD_ASSERT_DLG_ITEMS_COUNT		9
#define GD_ASSERT_DLG_TEXTEDIT_IDC		1000	//	TextEdit into which the debug message is been written.
#define GD_ASSERT_DLG_TEXTEDIT_TEXT		L""
#define GD_ASSERT_DLG_BTNABORT_IDC		1001
#define GD_ASSERT_DLG_BTNABORT_TEXT		GD_WIDEN(GD_ASSERT_DIALOG_BTNABORT_TEXT)
#define GD_ASSERT_DLG_BTNRETRY_IDC		1002
#define GD_ASSERT_DLG_BTNRETRY_TEXT		GD_WIDEN(GD_ASSERT_DIALOG_BTNRETRY_TEXT)
#define GD_ASSERT_DLG_BTNIGNORE_IDC		1003
#define GD_ASSERT_DLG_BTNIGNORE_TEXT	GD_WIDEN(GD_ASSERT_DIALOG_BTNIGNORE_TEXT)
#define GD_ASSERT_DLG_BTNIGNOREALL_IDC	1004
#define GD_ASSERT_DLG_BTNIGNOREALL_TEXT	GD_WIDEN(GD_ASSERT_DIALOG_BTNIGNOREALL_TEXT)
#define GD_ASSERT_DLG_BTNREPORT_IDC		1005
#define GD_ASSERT_DLG_BTNREPORT_TEXT	GD_WIDEN(GD_ASSERT_DIALOG_BTNREPORT_TEXT)
#define GD_ASSERT_DLG_BTNBREAK_IDC		1006
#define GD_ASSERT_DLG_BTNBREAK_TEXT		GD_WIDEN(GD_ASSERT_DIALOG_BTNBREAK_TEXT)
#define GD_ASSERT_DLG_GROUPBOX_IDC		0		//	Groupbox to Handle text edit and label.
#define GD_ASSERT_DLG_GROUPBOX_TEXT		GD_WIDEN(GD_ASSERT_DIALOG_GROUPBOX_TEXT)
#define GD_ASSERT_DLG_DESCRIPTION_IDC	0		//	Description of a what mess is going on.
#define GD_ASSERT_DLG_DESCRIPTION_TEXT	GD_WIDEN(GD_ASSERT_DIALOG_DESCRIPTION_TEXT)
//! @}

GD_NAMESPACE_BEGIN

#pragma pack(push, 4) 
#define GD_TEXTEDIT_PROPS ES_MULTILINE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_READONLY
	struct AssertDialogDataType final
	{
		GD_DEFINE_DIALOG_HEADER(GD_ASSERT_DLG_TITLE, GD_ASSERT_DLG_FONT);
		GD_DEFINE_DIALOG_ITEM(GD_ASSERT_DLG_BTNABORT_TEXT);
		GD_DEFINE_DIALOG_ITEM(GD_ASSERT_DLG_BTNRETRY_TEXT);
		GD_DEFINE_DIALOG_ITEM(GD_ASSERT_DLG_BTNIGNORE_TEXT);
		GD_DEFINE_DIALOG_ITEM(GD_ASSERT_DLG_BTNIGNOREALL_TEXT);
		GD_DEFINE_DIALOG_ITEM(GD_ASSERT_DLG_BTNREPORT_TEXT);
		GD_DEFINE_DIALOG_ITEM(GD_ASSERT_DLG_BTNBREAK_TEXT);
		GD_DEFINE_DIALOG_ITEM(GD_ASSERT_DLG_TEXTEDIT_TEXT);
		GD_DEFINE_DIALOG_ITEM(GD_ASSERT_DLG_GROUPBOX_TEXT);
		GD_DEFINE_DIALOG_ITEM(GD_ASSERT_DLG_DESCRIPTION_TEXT);
	};	// struct AssertDialogDataType
	AssertDialogDataType static const AssertDialogData = {
		{ DS_SETFOREGROUND | DS_MODALFRAME | DS_3DLOOK | DS_SETFONT | DS_CENTER | WS_POPUP | WS_CAPTION | WS_VISIBLE, 0, GD_ASSERT_DLG_ITEMS_COUNT, 0, 0, 378, 173 }, 0, 0, GD_ASSERT_DLG_TITLE, 8, GD_ASSERT_DLG_FONT,
		{ WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, 0, 302, 7, 73, 14, GD_ASSERT_DLG_BTNABORT_IDC, 0xFFFF, GD_WINAPI_BUTTON, GD_ASSERT_DLG_BTNABORT_TEXT, 0 },
		{ WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, 0, 302, 27, 73, 14, GD_ASSERT_DLG_BTNRETRY_IDC, 0xFFFF, GD_WINAPI_BUTTON, GD_ASSERT_DLG_BTNRETRY_TEXT, 0 },
		{ WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, 0, 302, 47, 73, 14, GD_ASSERT_DLG_BTNIGNORE_IDC, 0xFFFF, GD_WINAPI_BUTTON, GD_ASSERT_DLG_BTNIGNORE_TEXT, 0 },
		{ WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, 0, 302, 67, 73, 14, GD_ASSERT_DLG_BTNIGNOREALL_IDC, 0xFFFF, GD_WINAPI_BUTTON, GD_ASSERT_DLG_BTNIGNOREALL_TEXT, 0 },
		{ WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, 0, 302, 87, 73, 14, GD_ASSERT_DLG_BTNREPORT_IDC, 0xFFFF, GD_WINAPI_BUTTON, GD_ASSERT_DLG_BTNREPORT_TEXT, 0 },
		{ WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, 0, 302, 107, 73, 14, GD_ASSERT_DLG_BTNBREAK_IDC, 0xFFFF, GD_WINAPI_BUTTON, GD_ASSERT_DLG_BTNBREAK_TEXT, 0 },
		{ WS_CHILD | WS_VISIBLE | GD_TEXTEDIT_PROPS, 0, 11, 41, 283, 124, GD_ASSERT_DLG_TEXTEDIT_IDC, 0xFFFF, GD_WINAPI_TEXTEDIT, GD_ASSERT_DLG_TEXTEDIT_TEXT, 0 },
		{ WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 0, 7, 4, 292, 165, GD_ASSERT_DLG_GROUPBOX_IDC, 0xFFFF, GD_WINAPI_INFOPANEL, GD_ASSERT_DLG_GROUPBOX_TEXT, 0 },
		{ WS_CHILD | WS_VISIBLE, 0, 13, 17, 279, 20, GD_ASSERT_DLG_DESCRIPTION_IDC, 0xFFFF, GD_WINAPI_STATIC_TEXT, GD_ASSERT_DLG_DESCRIPTION_TEXT, 0 },
	};
#undef GD_TEXTEDIT_PROPS
#pragma pack(pop)

	//! Defines a main procedure for an assertion dialog.
	//! @param HDialog Pointer to dialog window.
	//! @param Message Message sent by user/OS.
	//! @param WParam ID of the used widget of the dialog.
	//! @param LParam Pointer to the assertion cache.
	//! @returns 'FALSE'.
	GDINT INT_PTR CALLBACK AssertDialogProc(HWND const HDialog, UINT const Message, WPARAM const WParam, LPARAM const LParam)
	{
		static AssertCache* TheAssertCache = nullptr;
		switch (Message)
		{
			case WM_INITDIALOG: {
				TheAssertCache = reinterpret_cast<AssertCache*>(LParam);
				GD_DEBUG_ASSERT(TheAssertCache != nullptr, "No Assert Data specified");

#if (GD_ASSERT_LEVEL != GD_ASSERT_LEVEL_DEBUG)
				::EnableWindow(::GetDlgItem(HDialog, GD_ASSERT_DLG_BTNBREAK_IDC), FALSE);
				::SendMessageA(HDialog, DM_SETDEFID, static_cast<WPARAM>(GD_ASSERT_DLG_BTNBREAK_IDC), 0);
#else	// if (GD_ASSERT_LEVEL != GD_ASSERT_LEVEL_DEBUG)
				::SendMessageA(HDialog, DM_SETDEFID, static_cast<WPARAM>(GD_ASSERT_DLG_BTNABORT_IDC), 0);
#endif	// if (GD_ASSERT_LEVEL != GD_ASSERT_LEVEL_DEBUG)

				// We cannot ignore fatal expressions.
				if (TheAssertCache->IsFatal)
				{
					::EnableWindow(::GetDlgItem(HDialog, GD_ASSERT_DLG_BTNRETRY_IDC), FALSE);
					::EnableWindow(::GetDlgItem(HDialog, GD_ASSERT_DLG_BTNIGNORE_IDC), FALSE);
					::EnableWindow(::GetDlgItem(HDialog, GD_ASSERT_DLG_BTNIGNOREALL_IDC), FALSE);
				}
				// We cannot report bugs in external code.
				if (TheAssertCache->TheAssertLocation != AssertLocation::EngineCode)
				{
					::EnableWindow(::GetDlgItem(HDialog, GD_ASSERT_DLG_BTNREPORT_IDC), FALSE);
				}

				::SetWindowTextA(::GetDlgItem(HDialog, GD_ASSERT_DLG_TEXTEDIT_IDC), TheAssertCache->FormattedMessage.CStr());
				::Beep(750, 300);
			} break;
			case WM_COMMAND: {
				static_assert(sizeof(INT_PTR) >= sizeof(AssertState), "Size of 'AssertState' is not enough be Handled by WinAPI.");
				switch (LOWORD(WParam))
				{
					case GD_ASSERT_DLG_BTNABORT_IDC: {
						::EndDialog(HDialog, static_cast<INT_PTR>(AssertState::Abort));
					} break;
					case GD_ASSERT_DLG_BTNRETRY_IDC: {
						::EndDialog(HDialog, static_cast<INT_PTR>(AssertState::Retry));
					} break;
					case GD_ASSERT_DLG_BTNIGNORE_IDC: {
						::EndDialog(HDialog, static_cast<INT_PTR>(AssertState::Ignore));
					} break;
					case GD_ASSERT_DLG_BTNIGNOREALL_IDC: {
						::EndDialog(HDialog, static_cast<INT_PTR>(AssertState::IgnoreAll));
					} break;
					case GD_ASSERT_DLG_BTNREPORT_IDC: {
						///	Instructions how to open the URL on Win32 API.
						//! ::ShellExecuteA(NULL, "open", "http://pornhub.com", NULL, NULL, SW_SHOWMAXIMIZED);
						::EndDialog(HDialog, static_cast<INT_PTR>(AssertState::Report));
					} break;
					case GD_ASSERT_DLG_BTNBREAK_IDC: {
						::EndDialog(HDialog, static_cast<INT_PTR>(AssertState::Break));
					} break;
				}
			} break;
		}

		return INT_PTR(FALSE);
	}

	//! Should show UI that notifies user about assertion failure and would request a decision from him.
	//! @param TheAssertCache Assertion data.
	//! @returns The assertion status, selected by user.
	GDINT static AssertState HandleAssertImpl(AssertCache const* const TheAssertCache)
	{
		HINSTANCE static const HInstance = static_cast<HINSTANCE>(::GetModuleHandleA(nullptr));
		INT_PTR const DialogResult = ::DialogBoxIndirectParamW(HInstance
			, reinterpret_cast<LPCDLGTEMPLATEW>(const_cast<AssertDialogDataType*>(&AssertDialogData))
			, nullptr, &AssertDialogProc, reinterpret_cast<LPARAM>(TheAssertCache));

		GD_DEBUG_ASSERT(DialogResult != -1, "Failed to create a dialog (Win32 API implementation).");
		GD_DEBUG_ASSERT((DialogResult >= 0) && (DialogResult < static_cast<INT_PTR>(AssertState::Invalid)), "Dialog returned an invalid result (Win32 API implementation).");
		return static_cast<AssertState>(DialogResult);
	}

	//! Should report about a bug to developers and return false on failure.
	//! @returns False on reporting failure.
	GDINT static bool ReportAssertImpl(AssertCache const* const TheAssertCache)
	{
		GD_NOT_USED(TheAssertCache);
		GD_NOT_IMPLEMENTED();
	}

GD_NAMESPACE_END

#pragma endregion Win32 Assert UI implementation.
#endif	// if GD_PLATFORM_WINDOWS

// ==========================================================================================
// Emscripten implementation. 
// ==========================================================================================
#if GD_PLATFORM_HTML5
#pragma region Emscripten Assert UI implementation.
#define GD_ASSERT_UI_IMPLEMENTED 1

extern "C" void emscripten_log(int flags, ...);

GD_NAMESPACE_BEGIN

	//! Should show UI that notifies user about assertion failure and would request a decision from him.
	//! @param TheAssertCache Assertion data.
	//! @returns The assertion status, selected by user.
	GDINT static AssertState HandleAssertImpl(AssertCache const* const TheAssertCache)
	{
		::emscripten_log(255, "\n -------------------------------------------------------------------------------");
		::emscripten_log(255, "\n" GD_ASSERT_DIALOG_DESCRIPTION_TEXT);
		::emscripten_log(255, "\n-------------------------------------------------------------------------------");
		::emscripten_log(255, "\n%s", TheAssertCache->FormattedMessage.CStr());

		return AssertState::Abort;
	}

	//! Should report about a bug to developers and return false on failure.
	//! @returns False on reporting failure.
	GDINT static bool ReportAssertImpl(AssertCache const* const TheAssertCache)
	{
		GD_NOT_USED(TheAssertCache);
		GD_NOT_IMPLEMENTED();
	}

GD_NAMESPACE_END

#pragma endregion Emscripten Assert UI implementation.
#endif	// if GD_PLATFORM_HTML5

// ==========================================================================================
// SDL 2.0 MessageBox-driven implementation. 
// ==========================================================================================
#if GD_PLATFORM_API_LIBSDL2 && (!defined(GD_ASSERT_UI_IMPLEMENTED))
#pragma region SDL 2.0 Assert UI implementation.
#define GD_ASSERT_UI_IMPLEMENTED 1

#include <SDL2/SDL_messagebox.h>

GD_NAMESPACE_BEGIN

	static AssertState HandleAssertImpl(AssertCache const* const TheAssertCache)
	{
		SizeTp ButtonsSize = 0;
		SDL_MessageBoxButtonData ButtonsData[static_cast<SizeTp>(AssertState::Invalid)];
		CMemory::Memset(&ButtonsData[0], 0, sizeof(ButtonsData));

		// Abort button is always available.
		ButtonsData[ButtonsSize++] = { 0, static_cast<int>(AssertState::Abort), GD_ASSERT_DIALOG_BTNABORT_TEXT };

		// If this assert is not fatal, enabling Retry and Ignore[All] buttons.
		if (!TheAssertCache->IsFatal) {
			ButtonsData[ButtonsSize++] = { 0, static_cast<int>(AssertState::Retry), GD_ASSERT_DIALOG_BTNRETRY_TEXT };
			ButtonsData[ButtonsSize++] = { 0, static_cast<int>(AssertState::Ignore), GD_ASSERT_DIALOG_BTNIGNORE_TEXT };
			ButtonsData[ButtonsSize++] = { 0, static_cast<int>(AssertState::IgnoreAll), GD_ASSERT_DIALOG_BTNIGNOREALL_TEXT };
		}

		// If the assertion is inside engine code, enabling report button.
		if (TheAssertCache->TheAssertLocation == AssertLocation::EngineCode) {
			ButtonsData[ButtonsSize++] = { 0, static_cast<int>(AssertState::Report), GD_ASSERT_DIALOG_BTNREPORT_TEXT };
		}

		// If we can preform a debug break, then this option is default. Otherwise, abort is.
#if (GD_ASSERT_LEVEL == GD_ASSERT_LEVEL_DEBUG)
		ButtonsData[ButtonsSize++] = { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, static_cast<int>(AssertState::Break), GD_ASSERT_DIALOG_BTNBREAK_TEXT };
#else	// if (GD_ASSERT_LEVEL == GD_ASSERT_LEVEL_DEBUG)
		ButtonsData[0].flags |= SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
#endif	// if (GD_ASSERT_LEVEL == GD_ASSERT_LEVEL_DEBUG)
		GD_DEBUG_ASSERT(ButtonsSize <= GD_ARRAY_LENGTH(ButtonsData), "Running out of bounds");

		String ReformattedMessage(String::Format(GD_ASSERT_DIALOG_DESCRIPTION_TEXT GD_PLATFORM_LINEBREAK GD_PLATFORM_LINEBREAK "%s", TheAssertCache->FormattedMessage.CStr()));
		SDL_MessageBoxData const TheAssertMessageBoxDesc = {
			SDL_MESSAGEBOX_ERROR, nullptr,
			GD_ASSERT_DIALOG_TITLE, ReformattedMessage.CStr(),
			static_cast<int>(ButtonsSize), &ButtonsData[0],
			nullptr
		};

		int DialogResult = -1;
		int const MessageBoxResult = ::SDL_ShowMessageBox(&TheAssertMessageBoxDesc, &DialogResult);
		GD_DEBUG_ASSERT(MessageBoxResult != -1, "Failed to create a dialog (Win32 API implementation).");
		GD_DEBUG_ASSERT((DialogResult >= 0) && (DialogResult < static_cast<int>(AssertState::Invalid)), "Dialog returned an invalid result (LibSDL2 implementation).");

		return static_cast<AssertState>(DialogResult);
	}

	//! Should report about a bug to developers and return false on failure.
	//! @returns False on reporting failure.
	GDINT static bool ReportAssertImpl(AssertCache const* const TheAssertCache)
	{
		GD_NOT_USED(TheAssertCache);
		GD_NOT_IMPLEMENTED();
	}

GD_NAMESPACE_END

#pragma endregion SDL 2.0 Assert UI implementation.
#endif	// if GD_PLATFORM_API_LIBSDL2 && (!defined(GD_ASSERT_UI_IMPLEMENTED))

// ==========================================================================================
// Platforms that do not have any way to show debug message (or I do not know about it).
// ==========================================================================================
#if (!defined(GD_ASSERT_UI_IMPLEMENTED))
#pragma region Generic Assert "UI" implementation.

#define GD_ASSERT_UI_IMPLEMENTED 1
#include <cstdio>
#include <GoddamnEngine/Core/IO/Stream/FileStream/FileStream.h>

GD_NAMESPACE_BEGIN

	static AssertState HandleAssertImpl(AssertCache const* const TheAssertCache)
	{
		std::fprintf(stderr, GD_PLATFORM_LINEBREAK "-------------------------------------------------------------------------------");
		std::fprintf(stderr, GD_PLATFORM_LINEBREAK GD_ASSERT_DIALOG_DESCRIPTION_TEXT);
		std::fprintf(stderr, GD_PLATFORM_LINEBREAK "-------------------------------------------------------------------------------");
		std::fprintf(stderr, GD_PLATFORM_LINEBREAK "%s", TheAssertCache->FormattedMessage.CStr());
		std::fprintf(stderr, GD_PLATFORM_LINEBREAK "Abort (a)/");

		// If this assert is not fatal, enabling Retry and Ignore[All] buttons.
		if (!TheAssertCache->IsFatal) {
			std::fprintf(stderr, "Retry (r)/");
			std::fprintf(stderr, "Ignore (i)/");
			std::fprintf(stderr, "Ignore All (I)/");
		}

		// If the assertion is inside engine code, enabling report button.
		if (TheAssertCache->TheAssertLocation == AssertLocation::EngineCode) {
			std::fprintf(stderr, "Report (R)");
		}

#if (GD_ASSERT_LEVEL == GD_ASSERT_LEVEL_DEBUG)
		std::fprintf(stderr, "/Debug Break (b)?:");
#else	// if (GD_ASSERT_LEVEL == GD_ASSERT_LEVEL_DEBUG)
		std::fprintf(stderr, "?:");
#endif	// if (GD_ASSERT_LEVEL == GD_ASSERT_LEVEL_DEBUG)
		std::fflush(stderr);

		for (;;) {
			char Buffer[32] = { '\0' };
			std::fprintf(stderr, "\a");
			std::fflush(stderr);
			if (std::fgets(&Buffer[0], GD_ARRAY_LENGTH(Buffer), stdin) != nullptr) {
				if (std::strcmp(&Buffer[0], "a") == 0) {
					return AssertState::Abort;
				} else if (std::strcmp(&Buffer[0], "r\n") == 0) {
					return AssertState::Retry;
				} else if (std::strcmp(&Buffer[0], "i\n") == 0) {
					return AssertState::Ignore;
				} else if (std::strcmp(&Buffer[0], "I\n") == 0) {
					return AssertState::IgnoreAll;
				} else if (std::strcmp(&Buffer[0], "R\n") == 0) {
					return AssertState::Report;
				} else if (std::strcmp(&Buffer[0], "b\n") == 0) {
					return AssertState::Break;
				} 
			} else {
				std::fprintf(stderr, GD_PLATFORM_LINEBREAK "Invalid input. Please, try again:");
				std::fflush(stderr);
			}
		}
	}

	//! Should report about a bug to developers and return false on failure.
	//! @returns False on reporting failure.
	GDINT static bool ReportAssertImpl(AssertCache const* const TheAssertCache)
	{
		GD_NOT_USED(TheAssertCache);
		GD_NOT_IMPLEMENTED();
	}

GD_NAMESPACE_END

#pragma endregion Generic Assert "UI" implementation.
#endif	// if (!defined(GD_ASSERT_UI_IMPLEMENTED))

#if (!defined(GD_ASSERT_UI_IMPLEMENTED)) || (!GD_ASSERT_UI_IMPLEMENTED)
#	error No Assert UI was implemented.
#endif	// if (!defined(GD_ASSERT_UI_IMPLEMENTED)) || (!GD_ASSERT_UI_IMPLEMENTED)
