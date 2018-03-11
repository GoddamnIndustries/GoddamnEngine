// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
//
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file
 * Paths implementation.
 */
#include <GoddamnEngine/Core/Platform/PlatformPaths.h>
#if GD_PLATFORM_API_MICROSOFT

#include <Windows.h>
#include <ShlObj.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Paths utilities on Microsoft platforms.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL MicrosoftPlatformPaths : public IPlatformPaths
	{
	public:

		// ------------------------------------------------------------------------------------------
		// Paths utilities.
		// ------------------------------------------------------------------------------------------
		
		GDINT virtual WideChar PathGetSeparator() const override final
		{
			return GD_TEXT(WideChar, '\\');
		}

		GDINT virtual bool PathPlatformize(WideString const& path, WideString& pathSystem) const override final;
		GDINT virtual bool PathGetFull(WideString const& path, WideString& pathFull) const override final;
		GDINT virtual bool PathGetRelative(WideString const& pathFrom, WideString const& pathTo, WideString& pathRelative) const override final;
	};  // class MicrosoftPlatformPaths

	GD_IMPLEMENT_SINGLETON(IPlatformPaths, MicrosoftPlatformPaths)

	/*!
	 * Converts path to system-specific format.
	 *
	 * @param path Input path.
	 * @param pathSystem Full path in system format.
	 *
	 * @returns True if operation succeeded.
	 */
	GDINT bool MicrosoftPlatformPaths::PathPlatformize(WideString const& path, WideString& pathSystem) const
	{
		auto pathCopy(path);
		for (auto& character : pathCopy)
		{
			if (character == '/')
				character = '\\';
		}
		return PathGetFull(pathCopy, pathSystem);
	}

	/*!
	 * Calculates full path to the specified path string.
	 *
	 * @param path Input path.
	 * @param pathFull Full path.
	 *
	 * @returns True if operation succeeded.
	 */
	GDINT bool MicrosoftPlatformPaths::PathGetFull(WideString const& path, WideString& pathFull) const
	{
		WCHAR pathFullSystem[MAX_PATH] = {};
		if (GetFullPathNameW(path.CStr(), GetLength<DWORD>(pathFullSystem), pathFullSystem, nullptr) != 0)
		{
			pathFull = pathFullSystem;
			return true;
		}
		return false;
	}

	/*!
	 * Calculates relative path from one path to another.
	 *
	 * @param pathFrom Path to file, that start relative path.
	 * @param pathTo Path to file, which relative path we need to compute.
	 * @param pathRelative Relative path.
	 *
	 * @returns True if operation succeeded.
	 */
	GDINT bool MicrosoftPlatformPaths::PathGetRelative(WideString const& pathFrom, WideString const& pathTo, WideString& pathRelative) const
	{
		WCHAR pathRelativeSystem[MAX_PATH] = {};
		if (PathRelativePathToW(pathRelativeSystem, pathFrom.CStr(), FILE_ATTRIBUTE_DIRECTORY, pathTo.CStr(), FILE_ATTRIBUTE_DIRECTORY) != 0)
		{
			pathRelative = pathRelativeSystem;
			return true;
		}
		return false;
	}

GD_NAMESPACE_END

#endif  // if GD_PLATFORM_API_MICROSOFT
