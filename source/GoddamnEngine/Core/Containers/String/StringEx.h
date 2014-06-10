//////////////////////////////////////////////////////////////////////////
/// StringEx.h - Extensions for strings
/// Copyright (C) $(GD_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 10.02.2014  - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#ifndef GD_CORE_CONTAINERS_STRING_EX
#define GD_CORE_CONTAINERS_STRING_EX

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/String/String.h>

GD_NAMESPACE_BEGIN

#if 0
	//////////////////////////////////////////////////////////////////////////
	constexpr size_t __StringLenth(
		_In_ Char const* const string
	)
	{
		return ((*string != Char('\0')) 
			? (__StringLenth(string + 1) + 1) 
			: (0)
		);
	}


	//////////////////////////////////////////////////////////////////////////
	constexpr UInt32 __StringHashPrecompute(
		_In_ Char const* const string
	)
	{
		return ((*string != Char('\0')) 
			? (__StringHashPrecompute(string + 1) * 65599 + (*string))
			: (0)
		);
	}

	//////////////////////////////////////////////////////////////////////////
	constexpr UInt32 __StringHashCompute(
		_In_ UInt32 const hashPrecomputed
	)
	{
		return ((hashPrecomputed >> 16) ^ hashPrecomputed);
	}

	//////////////////////////////////////////////////////////////////////////
	constexpr UInt32 __StringHash(
		_In_ Char const* const string
	)
	{
		return (__StringHashCompute(__StringHashPrecompute(string)));
	}
#endif

GD_NAMESPACE_END

#endif