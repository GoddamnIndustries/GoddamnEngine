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
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Templates/Singleton.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Paths utilities.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL IPlatformPaths : public Singleton<IPlatformPaths>
	{
	public:

		// ------------------------------------------------------------------------------------------
		// Paths utilities.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Returns path separator character.
		 */
		GDINT virtual WideChar PathGetSeparator() const
		{
			return GD_TEXT(WideChar, '/');
		}

		/*!
		 * Converts path to system-specific format.
		 *
		 * @param path Input path.
		 * @param pathSystem Full path in system format.
		 *
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool PathPlatformize(WideString const& path, WideString& pathSystem) const GD_PURE_VIRTUAL;

		/*!
		 * Calculates full path to the specified path string.
		 *
		 * @param path Input path.
		 * @param pathFull Full path.
		 *
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool PathGetFull(WideString const& path, WideString& pathFull) const GD_PURE_VIRTUAL;

		/*!
		 * Calculates relative path from one path to another.
		 *
		 * @param pathFrom Path to file, that start relative path.
		 * @param pathTo Path to file, which relative path we need to compute.
		 * @param pathRelative Relative path.
		 *
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool PathGetRelative(WideString const& pathFrom, WideString const& pathTo, WideString& pathRelative) const GD_PURE_VIRTUAL;
	};  // class IPlatformPaths

	template<>
	GDAPI IPlatformPaths& Singleton<IPlatformPaths>::Get();

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// Handles generic global paths and contains helpers to work with paths.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_WRAPPER Paths : public TNonCreatable
	{
	public:

		// ------------------------------------------------------------------------------------------
		// Helper functions.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Combines to paths.
		 *
		 * @param firstPath First path to combine.
		 * @param secondPath Second path to combine.
		 *
		 * @returns Combination of two paths.
		 */
		//! @{
		GDAPI static WideString Combine(WideString const& firstPath, WideString const& secondPath);
		GDAPI static WideString Combine(WideString const& firstPath, WideString const& secondPath, WideString const& thirdPath);
		//! @}

		/*!
		 * Converts path to system-specific format.
		 *
		 * @param path Path to the file.
		 * @param resultPtr Pointer to the function call result. If no parameter presented and function fails it will throw.
		 */
		GDAPI static WideString Platformize(WideString const& path, bool* const resultPtr = nullptr);

		/*!
		 * Returns a full path to the specified path string.
		 *
		 * @param path Path to the file.
		 * @param resultPtr Pointer to the function call result. If no parameter presented and function fails it will throw.
		 */
		GDAPI static WideString GetFullPath(WideString const& path, bool* const resultPtr = nullptr);

		/*!
		 * Returns relative path from one path to another.
		 *
		 * @param pathFrom Path to file, that start relative path.
		 * @param pathFrom Path to file, which relative path we need to compute.
		 * @param resultPtr Pointer to the function call result. If no parameter presented and function fails it will throw.
		 */
		GDAPI static WideString GetRelativePath(WideString const& pathFrom, WideString const& pathTo, bool* const resultPtr = nullptr);

		/*!
		 * Returns extension information for specified path string.
		 * E.g. "directory/file.ext" -> ".ext"
		 * @param path Path to the file.
		 */
		GDAPI static WideString GetExtension(WideString const& path);

		/*!
		 * Returns file and extension information for specified path string.
		 * E.g. "directory/file.ext" -> "file.ext"
		 * @param path Path to the file.
		 */
		GDAPI static WideString GetFileName(WideString const& path);

		/*!
		 * Returns file information for specified path string (without extension).
		 * E.g. "directory/file.ext" -> "file"
		 *
		 * @param path Path to the file.
		 */
		GDAPI static WideString GetFileNameWithoutExtension(WideString const& path);

		/*!
		 * Returns directory information for specified path string.
		 * E.g. "directory/file.ext" -> "directory/"
		 *
		 * @param path Path to the file.
		 */
		GDAPI static WideString GetDirectoryName(WideString const& path);

		/*!
		 * Returns directory file information for specified path string (without extension).
		 * E.g. "directory/file.ext" -> "directory/file"
		 *
		 * @param path Path to the file.
		 */
		GDAPI static WideString GetDirectoryAndFileNameWithoutExtension(WideString const& path);
	};	// class PathsGeneric

GD_NAMESPACE_END
