// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Misc/CStdio.cpp
//! Traits, helper functions and definitions for standard IO functions.
#include <GoddamnEngine/Core/Base/CStdlib/CStdio.h>
#if GD_PLATFORM_API_MICROSOFT

#include <dirent.h>

GD_NAMESPACE_BEGIN

	// ------------------------------------------------------------------------------------------
	//! @see @c "::opendir" function.
	GDAPI DIR* CStdio::OpenDir(CStr const directoryPath)
	{
		return ::opendir(directoryPath);
	}

	// ------------------------------------------------------------------------------------------
	//! @see @c "::closedir" function.
	GDAPI void CStdio::CloseDir(DIR* const directory)
	{
		::closedir(directory);
	}

	// ------------------------------------------------------------------------------------------
	//! @see @c "::closedir" function.
	GDAPI CStr CStdio::ReadDir(DIR* const directory)
	{
		return ::readdir(directory)->d_name;
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_API_MICROSOFT
