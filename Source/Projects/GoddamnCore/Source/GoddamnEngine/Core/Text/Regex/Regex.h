/// ==========================================================================================
/// Regex.h: Regular expressions interface.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// 
/// History:
///		* --.01.2014  - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_TEXT_REGEX
#define GD_CORE_TEXT_REGEX

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/SharedPtr.h>

GD_NAMESPACE_BEGIN

	class Regex final
	{
	private:
		SharedPtr<class RegexImplHandle> Implementation = nullptr;

	public:
		GDAPI Regex(Str const TheRegexString);
		GDAPI ~Regex();
	};	// class Regex

GD_NAMESPACE_END

#endif	// ifndef GD_CORE_TEXT_REGEX
