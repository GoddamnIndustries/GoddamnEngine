/// ==========================================================================================
/// Assertion.h: Assertation mechanism implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.01.2014  - Created by James Jhuighuy
/// ==========================================================================================

#include <GoddamnEngine/Include.h>
#if ((!defined(GD_PLATFORM_API_COCOA)) || (defined(GD_CORE_DIAGNOSTICS_ASSERTION_MM)))

#include <GoddamnEngine/Core/Diagnostics/Assertion/Assertion.h>
#include <GoddamnEngine/Core/Text/String/String.h>

#include <Windows.h>

GD_NAMESPACE_BEGIN

	/// Whoa, seems that you have just broken the world's most stable piece of code!

	/// ------------------------------------------------------------------------------------------
	/// Fatal assertation mechanisms.
	/// ------------------------------------------------------------------------------------------

	void HandleFatalAssertionVa(FatalAssertionData const* const Data, va_list const Args)
	{
		::abort();
	}

	/// ------------------------------------------------------------------------------------------
	/// Regular assertation mechanisms.
	/// ------------------------------------------------------------------------------------------

	AssertionState HandleRegularAssertionVa(RegularAssertionData* const Data, va_list const Args)
	{
	//	::MessageBoxA(nullptr, "Some text here", "Some title here", 0);
		return AssertionState::Break;
	}

GD_NAMESPACE_END

#endif	// if ((!defined(GD_PLATFORM_API_COCOA)) || (defined(GD_CORE_DIAGNOSTICS_ASSERTION_MM)))
