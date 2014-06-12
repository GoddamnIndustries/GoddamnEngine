#include <GoddamnEngine/Core/Diagnostics/Assertion/Assertion.hh>
#include <GoddamnEngine/Core/Text/String/String.hh>
#include <GoddamnEngine/Core/LowLevelSystem/LowLevelSystem.hh>

#if (defined(GD_USE_CUSTOM_ASSERTS) && (defined(GD_PLATFORM_WINDOWS) || defined(GD_PLATFORM_XBOX_ONE))) 
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>

GD_NAMESPACE_BEGIN
	
	//////////////////////////////////////////////////////////////////////////
	extern bool Assert(		
		_In_ Str    const FileName,
		_In_ Str    const FunctionName,
		_In_ size_t const LineNumber,
		_In_ Str    const ExpressionString,
		_In_ Str    const Message
	)
	{
		String const FormatedString = String::Format(
			"%s\n\n\nExpression: \"%s\";\n(In File \"%s\", Function \"%s\", Line #%d).\n"
			"\nPress\t[Abort]\tto terminate application;"
			"\nPress\t[Retry]\tto debug application;"
			"\nPress\t[Ignore]\tto ignore the Assertation.",
			Message, ExpressionString, FileName, FunctionName, static_cast<int>(LineNumber)
		);

		switch (MessageBoxA(
            nullptr/*reinterpret_cast<HWND>(LowLevelSystem::GetInstance().hWindow)*/,
			&FormatedString[0], "Debug Assertation failed!",
			MB_ABORTRETRYIGNORE | MB_ICONERROR
		))
		{
		case IDABORT:	abort(); return false;
		case IDIGNORE:			 return false;
		case IDRETRY:			 return true;
		}

		return true;
	}

GD_NAMESPACE_END

#endif
