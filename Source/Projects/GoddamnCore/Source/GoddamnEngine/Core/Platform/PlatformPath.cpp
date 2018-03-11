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
#include <GoddamnEngine/Core/Containers/StringBuilder.h>

GD_NAMESPACE_BEGIN

	GDINL static UInt32 FindLastChar(WideString const& filename, WideChar const query)
	{
		auto const ptr = CString::Strrchr(filename.CStr(), query);
		return ptr != nullptr ? static_cast<UInt32>(*ptr) : UInt32Max;
	}
	GDINL static UInt32 FindLastChar(WideString const& filename, WideChar const query1, WideChar const query2)
	{
		auto const iter = Algo::FindLastIf(filename, [=](WideChar const character) { return character == query1 || character == query2; });
		return iter != filename.End() ? static_cast<UInt32>(&*iter - filename.CStr()) : UInt32Max;
	}

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
	GDAPI WideString Paths::Combine(WideString const& firstPath, WideString const& secondPath)
	{
		WideStringBuilder combinedPath;
		combinedPath.Append(firstPath);
		if (firstPath.GetLast() != GD_TEXT(WideChar, '/') && firstPath.GetLast() != GD_TEXT(WideChar, '\\'))
		{
			combinedPath.Append(IPlatformPaths::Get().PathGetSeparator());
		}
		combinedPath.Append(secondPath[0] == GD_TEXT(WideChar, '/') || secondPath[0] == GD_TEXT(WideChar, '\\') ? secondPath.CStr() + 1 : secondPath.CStr());
		return combinedPath.ToString();
	}
	GDAPI WideString Paths::Combine(WideString const& firstPath, WideString const& secondPath, WideString const& thirdPath)
	{
		return Paths::Combine(Paths::Combine(firstPath, secondPath), thirdPath);
	}
	//! @}

	/*!
	 * Converts path to system-specific format.
	 *
	 * @param path Path to the file.
	 * @param resultPtr Pointer to the function call result. If no parameter presented and function fails it will throw.
	 */
	GDAPI WideString Paths::Platformize(WideString const& path, bool* const resultPtr)
	{
		WideString pathSystem;
		auto const result = IPlatformPaths::Get().PathPlatformize(path, pathSystem);
		if (resultPtr != nullptr)
		{
			*resultPtr = result;
		}
		else if (!result)
		{
			GD_VERIFY_FALSE("Unhandled IO error: failed to calculate full path.");
		}
		return pathSystem;
	}

	/*!
	 * Returns a full path to the specified path string.
	 *
	 * @param path Path to the file.
	 * @param resultPtr Pointer to the function call result. If no parameter presented and function fails it will throw.
	 */
	GDAPI WideString Paths::GetFullPath(WideString const& path, bool* const resultPtr)
	{
		WideString pathSystem;
		auto const result = IPlatformPaths::Get().PathGetFull(path, pathSystem);
		if (resultPtr != nullptr)
		{
			*resultPtr = result;
		}
		else if (!result)
		{
			GD_VERIFY_FALSE("Unhandled IO error: failed to calculate system path.");
		}
		return pathSystem;
	}

	/*!
	 * Returns relative path from one path to another.
	 *
	 * @param pathFrom Path to file, that start relative path.
	 * @param pathFrom Path to file, which relative path we need to compute.
	 * @param resultPtr Pointer to the function call result. If no parameter presented and function fails it will throw.
	 */
	GDAPI WideString Paths::GetRelativePath(WideString const& pathFrom, WideString const& pathTo, bool* const resultPtr)
	{
		WideString pathRelative;
		auto const result = IPlatformPaths::Get().PathGetRelative(pathFrom, pathTo, pathRelative);
		if (resultPtr != nullptr)
		{
			*resultPtr = result;
		}
		else if (!result)
		{
			GD_VERIFY_FALSE("Unhandled IO error: failed to calculate relative path.");
		}
		return pathRelative;
	}

	/*!
	 * Returns extension information for specified path string.
	 * E.g. "directory/file.ext" -> ".ext"
	 *
	 * @param path Path to the file.
	 */
	GDAPI WideString Paths::GetExtension(WideString const& path)
	{
		auto const lastDot = FindLastChar(path, '.');
		return lastDot == UInt32Max ? path : path.Substring(lastDot);
	}

	/*!
	 * Returns file and extension information for specified path string.
	 * E.g. "directory/file.ext" -> "file.ext"
	 *
	 * @param path Path to the file.
	 */
	GDAPI WideString Paths::GetFileName(WideString const& path)
	{
		auto const lastSlashOrBackslash = FindLastChar(path, '/', '\\');
		return lastSlashOrBackslash == UInt32Max ? path : path.Substring(lastSlashOrBackslash + 1);
	}

	/*!
	 * Returns file information for specified path string (without extension).
	 * E.g. "directory/file.ext" -> "file"
	 *
	 * @param path Path to the file.
	 */
	GDAPI WideString Paths::GetFileNameWithoutExtension(WideString const& path)
	{
		return Paths::GetFileName(Paths::GetDirectoryAndFileNameWithoutExtension(path));
	}

	/*!
	 * Returns directory information for specified path string.
	 * E.g. "directory/file.ext" -> "directory/"
	 *
	 * @param path Path to the file.
	 */
	GDAPI WideString Paths::GetDirectoryName(WideString const& path)
	{
		auto const lastSlashOrBackslash = FindLastChar(path, '/', '\\');
		return lastSlashOrBackslash == UInt32Max ? path : path.Substring(0, lastSlashOrBackslash);
	}

	/*!
	 * Returns directory file information for specified path string (without extension).
	 * E.g. "directory/file.ext" -> "directory/file"
	 *
	 * @param path Path to the file.
	 */
	GDAPI WideString Paths::GetDirectoryAndFileNameWithoutExtension(WideString const& path)
	{
		auto const lastDot = FindLastChar(path, '.');
		return lastDot == UInt32Max ? path : path.Substring(0, lastDot);
	}

GD_NAMESPACE_END
