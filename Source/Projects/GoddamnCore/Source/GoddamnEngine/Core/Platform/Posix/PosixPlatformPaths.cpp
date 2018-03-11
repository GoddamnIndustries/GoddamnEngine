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
#if GD_PLATFORM_API_POSIX

#include <GoddamnEngine/Core/Platform/PlatformEncoding.h>

#include <stdlib.h>
#include <limits.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Paths utilities on Posix platforms.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL PosixPlatformPaths : public IPlatformPaths
	{
	public:

		// ------------------------------------------------------------------------------------------
		// Paths utilities.
		// ------------------------------------------------------------------------------------------

		GDINT virtual bool PathPlatformize(WideString const& path, WideString& pathSystem) const override final;
		GDINT virtual bool PathGetFull(WideString const& path, WideString& pathFull) const override final;
		GDINT virtual bool PathGetRelative(WideString const& pathFrom, WideString const& pathTo, WideString& pathRelative) const override final;
	};  // class PosixPlatformPaths

	GD_IMPLEMENT_SINGLETON(IPlatformPaths, PosixPlatformPaths)

	/*!
	 * Converts path to system-specific format.
	 *
	 * @param path Input path.
	 * @param pathSystem Full path in system format.
	 *
	 * @returns True if operation succeeded.
	 */
	GDINT bool PosixPlatformPaths::PathPlatformize(WideString const& path, WideString& pathSystem) const
	{
		pathSystem = path;
		return true;
	}

	/*!
	 * Calculates full path to the specified path string.
	 *
	 * @param path Input path.
	 * @param pathFull Full path.
	 *
	 * @returns True if operation succeeded.
	 */
	GDINT bool PosixPlatformPaths::PathGetFull(WideString const& path, WideString& pathFull) const
	{
		auto const pathSystem = StringConv::EncodeUTF8(path);
		Char pathFullSystem[4 * PATH_MAX] = {};
		if (realpath(pathSystem.CStr(), pathFullSystem) != nullptr)
		{
			pathFull = StringConv::DecodeUTF8(pathFullSystem);
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
	GDINT bool PosixPlatformPaths::PathGetRelative(WideString const& pathFrom, WideString const& pathTo, WideString& pathRelative) const
	{
		GD_NOT_IMPLEMENTED();
	}

GD_NAMESPACE_END

#endif  // if GD_PLATFORM_API_POSIX
