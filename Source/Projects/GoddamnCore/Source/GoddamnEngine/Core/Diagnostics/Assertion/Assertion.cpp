/// ==========================================================================================
/// Assertion.cpp: Assertion mechanism implementation.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// ==========================================================================================

/// Note: code inside this file needs to be asserted too. To do that use default assert macro.
#define GD_CORE_DIAGNOSTICS_ASSERTION_CPP
#include <GoddamnEngine/Include.h>

// We should not compile this file on Cocoa-driven systems cause they are compiling dummy Obj-C++ source, that 
// includes this one.
#if (!GD_PLATFORM_API_COCOA) || (defined(GD_CORE_DIAGNOSTICS_ASSERTION_MM))

#include <GoddamnEngine/Core/Diagnostics/Assertion/Assertion.h>
#include <GoddamnEngine/Core/Containers/String.h>

#include <cstdio>

/// ------------------------------------------------------------------------------------------
/// We are inside assertion implementation source, so we cannot use our asserts in order 
/// of possible cycle dependencies.
#undef GD_ENABLED_ASSERT
#undef GD_ENABLED_FALSE_ASSERT
#define GD_ENABLED_ASSERT(Expression, Message, ...) assert((Expression) && (Message))
#define GD_ENABLED_FALSE_ASSERT(Message, ...) \
	do { \
		assert(false && (Message)); \
		std::abort(); /*Simulating "noreturn" behavior. */ \
	} while(false)

#if (defined(GD_PLATFORM_API_WINAPI))
#	define GD_PLATFORM_LINEBREAK "\r\n"
#	define GD_PLATFORM_SLASH  "\\"
#else	// if (defined(GD_PLATFORM_API_WINAPI))
#	define GD_PLATFORM_LINEBREAK "\n"
#	define GD_PLATFORM_SLASH  "/"
#endif	// if (defined(GD_PLATFORM_API_WINAPI))

/// ------------------------------------------------------------------------------------------
/// Some text for our assertion UI. Defined as macros to support widening.
#define GD_ASSERTION_DIALOG_TITLE				"Assertion Failed"
#define GD_ASSERTION_DIALOG_BTNABORT_TEXT		"Abort"
#define GD_ASSERTION_DIALOG_BTNRETRY_TEXT		"Retry"
#define GD_ASSERTION_DIALOG_BTNIGNORE_TEXT		"Ignore"	
#define GD_ASSERTION_DIALOG_BTNIGNOREALL_TEXT	"Ignore All"
#define GD_ASSERTION_DIALOG_BTNREPORT_TEXT		"Report an Issue"
#define GD_ASSERTION_DIALOG_BTNBREAK_TEXT		"Break"
#define GD_ASSERTION_DIALOG_GROUPBOX_TEXT		"What the mess is going on?"
#define GD_ASSERTION_DIALOG_DESCRIPTION_TEXT	"Whoa, seems that you have just broken the world's most stable piece of code!\nSeriosly, some code assertion failed. This may cause unstable work and etc."

GD_NAMESPACE_BEGIN

	/// ------------------------------------------------------------------------------------------	
	/// Platform-dependent methods for handling/reporting assertions.
	/// This needs to be implemented.
	/// ------------------------------------------------------------------------------------------	

	struct AssertionCache;

	/// @brief Should show UI that notifies user about assertion failure and would request a decision from him.
	GDINT static AssertionState HandleAssertionImpl(AssertionCache const* const TheAssertionCache);

	/// @brief Should report about a bug to developers and return false on failure.
	/// @returns False on reporting failure.
	GDINT static bool ReportAssertionImpl(AssertionCache const* const TheAssertionCache);

	/// ------------------------------------------------------------------------------------------	
	/// Location of the assertion in code.
	/// ------------------------------------------------------------------------------------------

	enum class AssertionLocation : UInt8
	{
		EngineCode,
		GameCode,
		DependencyCode,
		Invalid,
	};	// enum class AssertionLocation

	/// Returns the approximate location of assertion in the source of the engine.
	/// This information is crucial for determining whether bug is located inside engine code (and we made mistakes)
	/// or is cause by crooked hands of some game developers (no offenses, all assertion are caused by crookedness).
	GDINT static AssertionLocation GetAssertionLocationForSource(Str const FileName)
	{
		Str const EngineSDKLocation = std::strstr(FileName, GD_PLATFORM_SLASH "GoddamnEngine" GD_PLATFORM_SLASH "source");
		if (EngineSDKLocation != nullptr) {
			if (std::strstr(EngineSDKLocation, GD_PLATFORM_SLASH "Dependencies") != nullptr) {
				return AssertionLocation::DependencyCode;
			} else {
				return AssertionLocation::EngineCode;
			}
		} else {
			return AssertionLocation::GameCode;
		}
	}

	/// ------------------------------------------------------------------------------------------
	/// Reporting issues mechanisms.
	/// ------------------------------------------------------------------------------------------

	struct AssertionCache final
	{
		AssertionLocation TheAssertionLocation = AssertionLocation::Invalid;
		FatalAssertionData const* TheAssertionData = nullptr;
		bool IsFatal = false;
		String FormattedMessage;
	};	// struct AssertionCache

	static const char ReportGithubURL [] = "https://github.com/QuaternionStudios/GoddamnEngine/issues/new";

	void ReportAssertion(AssertionCache const* const TheAssertionCache)
	{
		GD_DEBUG_ASSERT(ReportAssertionImpl(TheAssertionCache), "Failed to report an error");
	}

	/// ------------------------------------------------------------------------------------------
	/// Fatal assertion mechanisms.
	/// ------------------------------------------------------------------------------------------

	void HandleFatalAssertionVa(FatalAssertionData const* const Data, va_list Args)
	{
		/// @todo Restore code here
		//CriticalSection static CriticalAssertionCriticalSection;
		//CriticalAssertionCriticalSection.Enter();

		AssertionCache TheAssertionCache;
		TheAssertionCache.IsFatal = true;
		TheAssertionCache.TheAssertionData = Data;
		TheAssertionCache.FormattedMessage = String::Format((
			"Fatal error occurred!" GD_PLATFORM_LINEBREAK
			"At: " GD_PLATFORM_LINEBREAK
			"[File]: %s" GD_PLATFORM_LINEBREAK
			"[Line]: %u" GD_PLATFORM_LINEBREAK
			"[Function]: %s" GD_PLATFORM_LINEBREAK GD_PLATFORM_LINEBREAK
			"%s"), Data->FileName, Data->Line, Data->FunctionName, String::FormatVa(Data->Message, Args).CStr());

		AssertionState const TheAssertionState = HandleAssertionImpl(&TheAssertionCache);
		if (TheAssertionState == AssertionState::Report) {
			ReportAssertion(&TheAssertionCache);
		}

		std::exit(-1);
	}

	/// ------------------------------------------------------------------------------------------
	/// Regular assertion mechanisms.
	/// ------------------------------------------------------------------------------------------

	AssertionState HandleRegularAssertionVa(RegularAssertionData* const Data, va_list Args)
	{
		bool static ShouldIgnoreAllRegularAssertions = false;
		if (!ShouldIgnoreAllRegularAssertions) {
			/// @todo Restore code here
			//CriticalSection static RegularAssertionCriticalSection;
			//ScopedLock RegularAssertionLock(RegularAssertionCriticalSection);

			AssertionCache TheAssertionCache;
			TheAssertionCache.TheAssertionData = Data;
			TheAssertionCache.TheAssertionLocation = GetAssertionLocationForSource(Data->FileName);
			TheAssertionCache.FormattedMessage = String::Format((
				"%s" GD_PLATFORM_LINEBREAK
				GD_PLATFORM_LINEBREAK
				"At: " GD_PLATFORM_LINEBREAK
				"[File]: %s" GD_PLATFORM_LINEBREAK
				"[Line]: %u" GD_PLATFORM_LINEBREAK
				"[Function]: %s" GD_PLATFORM_LINEBREAK
				"[Expression]: %s" GD_PLATFORM_LINEBREAK
				), String::FormatVa(Data->Message, Args).CStr(), Data->FileName, Data->Line, Data->FunctionName, Data->Expression);

			AssertionState const TheAssertionState = HandleAssertionImpl(&TheAssertionCache);
			switch (TheAssertionState) {
				case AssertionState::Abort: {
					::fprintf(stderr, GD_PLATFORM_LINEBREAK "Aborting the application...");
					::fflush(stderr);
					::abort();
				} break;
				case AssertionState::Report: {
					ReportAssertion(&TheAssertionCache);
				} break;
				case AssertionState::Ignore: {
					Data->ShouldAlwaysIgnoreThisAssertion = true;
				} break;
				case AssertionState::IgnoreAll: {
					ShouldIgnoreAllRegularAssertions = true;
				} break;
				case AssertionState::Retry:
				case AssertionState::Break: {
					// Macros Handle here.
				} break;
			}

			return TheAssertionState;
		} else {
			Data->ShouldAlwaysIgnoreThisAssertion = true;
			return AssertionState::Ignore;
		}
	}

GD_NAMESPACE_END

/// ==========================================================================================
/// Assertion UI implementation.
/// ==========================================================================================

/// ==========================================================================================
/// Win32 API Dialog implementation.
/// Here comes real hardcore inlined dialog box programming. God is with us!
/// ==========================================================================================

#if (defined(GD_PLATFORM_WINDOWS))
#include <Windows.h>

/// ------------------------------------------------------------------------------------------
/// Win32 ID classes.
/// ------------------------------------------------------------------------------------------

#define GD_WINAPI_BUTTON		0x0080
#define GD_WINAPI_INFOPANEL		GD_WINAPI_BUTTON
#define GD_WINAPI_TEXTEDIT		0x0081
#define GD_WINAPI_STATIC_TEXT	0x0082

/// ------------------------------------------------------------------------------------------
/// Win32 Dialog builder helpers.
/// ------------------------------------------------------------------------------------------

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
		SHORT X, Y, CX, CY;
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
#define GD_ASSERTION_DLG_TITLE				GD_WIDEN(GD_ASSERTION_DIALOG_TITLE)
#define GD_ASSERTION_DLG_FONT				L"MS Sans Serif"
#define GD_ASSERTION_DLG_ITEMS_COUNT		9
#define GD_ASSERTION_DLG_TEXTEDIT_IDC		1000	//	TextEdit into which the debug message is been written.
#define GD_ASSERTION_DLG_TEXTEDIT_TEXT		L""
#define GD_ASSERTION_DLG_BTNABORT_IDC		1001
#define GD_ASSERTION_DLG_BTNABORT_TEXT		GD_WIDEN(GD_ASSERTION_DIALOG_BTNABORT_TEXT)
#define GD_ASSERTION_DLG_BTNRETRY_IDC		1002
#define GD_ASSERTION_DLG_BTNRETRY_TEXT		GD_WIDEN(GD_ASSERTION_DIALOG_BTNRETRY_TEXT)
#define GD_ASSERTION_DLG_BTNIGNORE_IDC		1003
#define GD_ASSERTION_DLG_BTNIGNORE_TEXT		GD_WIDEN(GD_ASSERTION_DIALOG_BTNIGNORE_TEXT)
#define GD_ASSERTION_DLG_BTNIGNOREALL_IDC	1004
#define GD_ASSERTION_DLG_BTNIGNOREALL_TEXT	GD_WIDEN(GD_ASSERTION_DIALOG_BTNIGNOREALL_TEXT)
#define GD_ASSERTION_DLG_BTNREPORT_IDC		1005
#define GD_ASSERTION_DLG_BTNREPORT_TEXT		GD_WIDEN(GD_ASSERTION_DIALOG_BTNREPORT_TEXT)
#define GD_ASSERTION_DLG_BTNBREAK_IDC		1006
#define GD_ASSERTION_DLG_BTNBREAK_TEXT		GD_WIDEN(GD_ASSERTION_DIALOG_BTNBREAK_TEXT)
#define GD_ASSERTION_DLG_GROUPBOX_IDC		0		//	Groupbox to Handle text edit and label.
#define GD_ASSERTION_DLG_GROUPBOX_TEXT		GD_WIDEN(GD_ASSERTION_DIALOG_GROUPBOX_TEXT)
#define GD_ASSERTION_DLG_DESCRIPTION_IDC	0		//	Description of a what mess is going on.
#define GD_ASSERTION_DLG_DESCRIPTION_TEXT	GD_WIDEN(GD_ASSERTION_DIALOG_DESCRIPTION_TEXT)

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
		{ WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, 0, 302, 7, 73, 14, GD_ASSERTION_DLG_BTNABORT_IDC, 0xFFFF, GD_WINAPI_BUTTON, GD_ASSERTION_DLG_BTNABORT_TEXT, 0 },
		{ WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, 0, 302, 27, 73, 14, GD_ASSERTION_DLG_BTNRETRY_IDC, 0xFFFF, GD_WINAPI_BUTTON, GD_ASSERTION_DLG_BTNRETRY_TEXT, 0 },
		{ WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, 0, 302, 47, 73, 14, GD_ASSERTION_DLG_BTNIGNORE_IDC, 0xFFFF, GD_WINAPI_BUTTON, GD_ASSERTION_DLG_BTNIGNORE_TEXT, 0 },
		{ WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, 0, 302, 67, 73, 14, GD_ASSERTION_DLG_BTNIGNOREALL_IDC, 0xFFFF, GD_WINAPI_BUTTON, GD_ASSERTION_DLG_BTNIGNOREALL_TEXT, 0 },
		{ WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, 0, 302, 87, 73, 14, GD_ASSERTION_DLG_BTNREPORT_IDC, 0xFFFF, GD_WINAPI_BUTTON, GD_ASSERTION_DLG_BTNREPORT_TEXT, 0 },
		{ WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, 0, 302, 107, 73, 14, GD_ASSERTION_DLG_BTNBREAK_IDC, 0xFFFF, GD_WINAPI_BUTTON, GD_ASSERTION_DLG_BTNBREAK_TEXT, 0 },
		{ WS_CHILD | WS_VISIBLE | GD_TEXTEDIT_PROPS, 0, 11, 41, 283, 124, GD_ASSERTION_DLG_TEXTEDIT_IDC, 0xFFFF, GD_WINAPI_TEXTEDIT, GD_ASSERTION_DLG_TEXTEDIT_TEXT, 0 },
		{ WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 0, 7, 4, 292, 165, GD_ASSERTION_DLG_GROUPBOX_IDC, 0xFFFF, GD_WINAPI_INFOPANEL, GD_ASSERTION_DLG_GROUPBOX_TEXT, 0 },
		{ WS_CHILD | WS_VISIBLE, 0, 13, 17, 279, 20, GD_ASSERTION_DLG_DESCRIPTION_IDC, 0xFFFF, GD_WINAPI_STATIC_TEXT, GD_ASSERTION_DLG_DESCRIPTION_TEXT, 0 },
	};
#undef GD_TEXTEDIT_PROPS
#pragma pack(pop)

	INT_PTR CALLBACK AssertionDialogProc(HWND const HDialog, UINT const Message, WPARAM const WParam, LPARAM const LParam)
	{
		static AssertionCache* TheAssertionCache = nullptr;
		switch (Message) {
			case WM_INITDIALOG: {
				TheAssertionCache = reinterpret_cast<AssertionCache*>(LParam);
				GD_DEBUG_ASSERT(TheAssertionCache != nullptr, "No Assertion Data specified");

#if (GD_ASSERTION_LEVEL != GD_ASSERTION_LEVEL_DEBUG)
				::EnableWindow(::GetDlgItem(HDialog, GD_ASSERTION_DLG_BTNBREAK_IDC), FALSE);
				::SendMessageA(HDialog, DM_SETDEFID, static_cast<WPARAM>(GD_ASSERTION_DLG_BTNBREAK_IDC), 0);
#else	// if (GD_ASSERTION_LEVEL != GD_ASSERTION_LEVEL_DEBUG)
				::SendMessageA(HDialog, DM_SETDEFID, static_cast<WPARAM>(GD_ASSERTION_DLG_BTNABORT_IDC), 0);
#endif	// if (GD_ASSERTION_LEVEL != GD_ASSERTION_LEVEL_DEBUG)

				// We cannot ignore fatal expressions.
				if (TheAssertionCache->IsFatal) {
					::EnableWindow(::GetDlgItem(HDialog, GD_ASSERTION_DLG_BTNRETRY_IDC), FALSE);
					::EnableWindow(::GetDlgItem(HDialog, GD_ASSERTION_DLG_BTNIGNORE_IDC), FALSE);
					::EnableWindow(::GetDlgItem(HDialog, GD_ASSERTION_DLG_BTNIGNOREALL_IDC), FALSE);
				}
				// We cannot report bugs in external code.
				if (TheAssertionCache->TheAssertionLocation != AssertionLocation::EngineCode) {
					::EnableWindow(::GetDlgItem(HDialog, GD_ASSERTION_DLG_BTNREPORT_IDC), FALSE);
				}

				::SetWindowTextA(::GetDlgItem(HDialog, GD_ASSERTION_DLG_TEXTEDIT_IDC), TheAssertionCache->FormattedMessage.CStr());
				::Beep(750, 300);
			} break;
			case WM_COMMAND: {
				static_assert(sizeof(INT_PTR) >= sizeof(AssertionState), "Size of 'AssertionState' is not enough be Handled by WinAPI.");
				switch (LOWORD(WParam)) {
					case GD_ASSERTION_DLG_BTNABORT_IDC: {
						::EndDialog(HDialog, static_cast<INT_PTR>(AssertionState::Abort));
					} break;
					case GD_ASSERTION_DLG_BTNRETRY_IDC: {
						::EndDialog(HDialog, static_cast<INT_PTR>(AssertionState::Retry));
					} break;
					case GD_ASSERTION_DLG_BTNIGNORE_IDC: {
						::EndDialog(HDialog, static_cast<INT_PTR>(AssertionState::Ignore));
					} break;
					case GD_ASSERTION_DLG_BTNIGNOREALL_IDC: {
						::EndDialog(HDialog, static_cast<INT_PTR>(AssertionState::IgnoreAll));
					} break;
					case GD_ASSERTION_DLG_BTNREPORT_IDC: {
						///	Instructions how to open the URL on Win32 API.
						/// ::ShellExecuteA(NULL, "open", "http://pornhub.com", NULL, NULL, SW_SHOWMAXIMIZED);
						::EndDialog(HDialog, static_cast<INT_PTR>(AssertionState::Report));
					} break;
					case GD_ASSERTION_DLG_BTNBREAK_IDC: {
						::EndDialog(HDialog, static_cast<INT_PTR>(AssertionState::Break));
					} break;
				}
			} break;
		}

		return INT_PTR(FALSE);
	}

	static AssertionState HandleAssertionImpl(AssertionCache const* const TheAssertionCache)
	{
		HINSTANCE static const HInstance = static_cast<HINSTANCE>(::GetModuleHandle(nullptr));
		INT_PTR const DialogResult = ::DialogBoxIndirectParamW(HInstance
			, reinterpret_cast<LPCDLGTEMPLATEW>(const_cast<AssertionDialogDataType*>(&AssertionDialogData))
			, nullptr, &AssertionDialogProc, reinterpret_cast<LPARAM>(TheAssertionCache));

		GD_DEBUG_ASSERT(DialogResult != -1, "Failed to create a dialog (Win32 API implementation).");
		GD_DEBUG_ASSERT((DialogResult >= 0) && (DialogResult < static_cast<INT_PTR>(AssertionState::Invalid)), "Dialog returned an invalid result (Win32 API implementation).");
		return static_cast<AssertionState>(DialogResult);
	}

GD_NAMESPACE_END

/// ==========================================================================================
/// SDL 2.0 MessageBox-driven implementation. 
/// ==========================================================================================

#elif (defined(GD_PLATFORM_API_LIBSDL2))
#include <SDL2/SDL_messagebox.h>

GD_NAMESPACE_BEGIN

	static AssertionState HandleAssertionImpl(AssertionCache const* const TheAssertionCache)
	{
		size_t ButtonsSize = 0;
		SDL_MessageBoxButtonData ButtonsData[static_cast<size_t>(AssertionState::Invalid)];
		std::memset(&ButtonsData[0], 0, sizeof(ButtonsData));

		// Abort button is always available.
		ButtonsData[ButtonsSize++] = { 0, static_cast<int>(AssertionState::Abort), GD_ASSERTION_DIALOG_BTNABORT_TEXT };

		// If this assert is not fatal, enabling Retry and Ignore[All] buttons.
		if (!TheAssertionCache->IsFatal) {
			ButtonsData[ButtonsSize++] = { 0, static_cast<int>(AssertionState::Retry), GD_ASSERTION_DIALOG_BTNRETRY_TEXT };
			ButtonsData[ButtonsSize++] = { 0, static_cast<int>(AssertionState::Ignore), GD_ASSERTION_DIALOG_BTNIGNORE_TEXT };
			ButtonsData[ButtonsSize++] = { 0, static_cast<int>(AssertionState::IgnoreAll), GD_ASSERTION_DIALOG_BTNIGNOREALL_TEXT };
		}

		// If the assertion is inside engine code, enabling report button.
		if (TheAssertionCache->TheAssertionLocation == AssertionLocation::EngineCode) {
			ButtonsData[ButtonsSize++] = { 0, static_cast<int>(AssertionState::Report), GD_ASSERTION_DIALOG_BTNREPORT_TEXT };
		}

		// If we can preform a debug break, then this option is default. Otherwise, abort is.
#if (GD_ASSERTION_LEVEL == GD_ASSERTION_LEVEL_DEBUG)
		ButtonsData[ButtonsSize++] = { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, static_cast<int>(AssertionState::Break), GD_ASSERTION_DIALOG_BTNBREAK_TEXT };
#else	// if (GD_ASSERTION_LEVEL == GD_ASSERTION_LEVEL_DEBUG)
		ButtonsData[0].flags |= SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
#endif	// if (GD_ASSERTION_LEVEL == GD_ASSERTION_LEVEL_DEBUG)
		GD_DEBUG_ASSERT(ButtonsSize <= GD_ARRAY_SIZE(ButtonsData), "Running out of bounds");

		String ReformattedMessage(String::Format(GD_ASSERTION_DIALOG_DESCRIPTION_TEXT GD_PLATFORM_LINEBREAK GD_PLATFORM_LINEBREAK "%s", TheAssertionCache->FormattedMessage.CStr()));
		SDL_MessageBoxData const TheAssertionMessageBoxDesc = {
			SDL_MESSAGEBOX_ERROR, nullptr,
			GD_ASSERTION_DIALOG_TITLE, ReformattedMessage.CStr(),
			static_cast<int>(ButtonsSize), &ButtonsData[0],
			nullptr
		};

		int DialogResult = -1;
		int const MessageBoxResult = ::SDL_ShowMessageBox(&TheAssertionMessageBoxDesc, &DialogResult);
		GD_DEBUG_ASSERT(MessageBoxResult != -1, "Failed to create a dialog (Win32 API implementation).");
		GD_DEBUG_ASSERT((DialogResult >= 0) && (DialogResult < static_cast<int>(AssertionState::Invalid)), "Dialog returned an invalid result (LibSDL2 implementation).");

		return static_cast<AssertionState>(DialogResult);
	}

GD_NAMESPACE_END

/// ==========================================================================================
/// Platforms that do not have any way to show debug message (or I do not know about it).
/// ==========================================================================================
#else	// *** Selecting best-suited API ***
#include <cstdio>
#include <GoddamnEngine/Core/IO/Stream/FileStream/FileStream.h>

GD_NAMESPACE_BEGIN

	static AssertionState HandleAssertionImpl(AssertionCache const* const TheAssertionCache)
	{
		std::fprintf(stderr, GD_PLATFORM_LINEBREAK "-------------------------------------------------------------------------------");
		std::fprintf(stderr, GD_PLATFORM_LINEBREAK GD_ASSERTION_DIALOG_DESCRIPTION_TEXT);
		std::fprintf(stderr, GD_PLATFORM_LINEBREAK "-------------------------------------------------------------------------------");
		std::fprintf(stderr, GD_PLATFORM_LINEBREAK "%s", TheAssertionCache->FormattedMessage.CStr());
		std::fprintf(stderr, GD_PLATFORM_LINEBREAK "Abort (a)/");

		// If this assert is not fatal, enabling Retry and Ignore[All] buttons.
		if (!TheAssertionCache->IsFatal) {
			std::fprintf(stderr, "Retry (r)/");
			std::fprintf(stderr, "Ignore (i)/");
			std::fprintf(stderr, "Ignore All (I)/");
		}

		// If the assertion is inside engine code, enabling report button.
		if (TheAssertionCache->TheAssertionLocation == AssertionLocation::EngineCode) {
			std::fprintf(stderr, "Report (R)");
		}

#if (GD_ASSERTION_LEVEL == GD_ASSERTION_LEVEL_DEBUG)
		std::fprintf(stderr, "/Debug Break (b)?:");
#else	// if (GD_ASSERTION_LEVEL == GD_ASSERTION_LEVEL_DEBUG)
		std::fprintf(stderr, "?:");
#endif	// if (GD_ASSERTION_LEVEL == GD_ASSERTION_LEVEL_DEBUG)
		std::fflush(stderr);

		for (;;) {
			char Buffer[32] = { '\0' };
			std::fprintf(stderr, "\a");
			std::fflush(stderr);
			if (std::fgets(&Buffer[0], GD_ARRAY_SIZE(Buffer), stdin) != nullptr) {
				if (std::strcmp(&Buffer[0], "a") == 0) {
					return AssertionState::Abort;
				} else if (std::strcmp(&Buffer[0], "r\n") == 0) {
					return AssertionState::Retry;
				} else if (std::strcmp(&Buffer[0], "i\n") == 0) {
					return AssertionState::Ignore;
				} else if (std::strcmp(&Buffer[0], "I\n") == 0) {
					return AssertionState::IgnoreAll;
				} else if (std::strcmp(&Buffer[0], "R\n") == 0) {
					return AssertionState::Report;
				} else if (std::strcmp(&Buffer[0], "b\n") == 0) {
					return AssertionState::Break;
				} 
			} else {
				std::fprintf(stderr, GD_PLATFORM_LINEBREAK "Invalid input. Please, try again:");
				std::fflush(stderr);
			}
		}
	}

GD_NAMESPACE_END

#endif	// *** Selecting best-suited API ***

/// ==========================================================================================
/// Assertion Issue-reporting system.
/// ==========================================================================================

GD_NAMESPACE_BEGIN

	static bool ReportAssertionImpl(AssertionCache const* const TheAssertionCache)
	{
		GD_NOT_USED(TheAssertionCache);
		GD_NOT_IMPLEMENTED();
		//	static const char ReportFileName[] = "BugReport.txt";
		//	::fprintf(stdout, GD_PLATFORM_LINEBREAK "Hey, Bro! You are using a very specific system, so we are unable to send a report message back here.");
		//	::fprintf(stdout, GD_PLATFORM_LINEBREAK "Now we will save the message into file '%s', so be so kind and send it as an issue to our GitHub tracker, located here ('%s').", &ReportFileName[0], &ReportGithubURL[0]);
		//	return true;
	}

GD_NAMESPACE_END

#endif	// if ((!defined(GD_PLATFORM_API_COCOA)) || (defined(GD_CORE_DIAGNOSTICS_ASSERTION_MM)))
