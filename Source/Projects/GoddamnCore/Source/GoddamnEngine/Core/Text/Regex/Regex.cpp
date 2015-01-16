/// ==========================================================================================
/// Regex.cpp: Regular expressions implementation.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// 
/// History:
///		* --.01.2014  - Created by James Jhuighuy
/// ==========================================================================================

#include <GoddamnEngine/Core/Text/Regex/Regex.h>

#if 0
#include <pcre.h>

GD_NAMESPACE_BEGIN

	/// ------------------------------------------------------------------------------------------
	/// Forcing PCRE to use our memory managemet functions.
	/// ------------------------------------------------------------------------------------------

	struct RegexImplMemoryManager final
	{
		GDINT RegexImplMemoryManager()
		{
			::pcre_malloc = reinterpret_cast<decltype(::pcre_malloc)>(Allocator::AllocateMemory);
			::pcre_free = reinterpret_cast<decltype(::pcre_free)>(Allocator::DeallocateMemory);
		}
	};	// class RegexImplMemoryManager
	RegexImplMemoryManager static const TheRegexImplMemoryManager;

	class RegexImplHandle final
	{
	private:
		GD_CLASS_UNASSIGNABLE(RegexImplHandle);
		GD_CLASS_UNCOPIABLE(RegexImplHandle);

	public:
		pcre* const ThePCRE;
		GDINL RegexImplHandle(pcre* const ThePCRE) : ThePCRE(ThePCRE) { }
		GDINL ~RegexImplHandle() { ::pcre_free(this->ThePCRE); }
	};	// class RegexImplHandle

	Regex::Regex(Str const TheRegexString)
	{
	}

GD_NAMESPACE_END
#endif	// if 0
