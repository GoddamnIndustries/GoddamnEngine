// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/System/IO/Paths.h
 * File contains generic paths helpers interface.
 */
#pragma once
#define GD_INSIDE_PATHS_H

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/System/Platform.h>
#include <GoddamnEngine/Core/Containers/String.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// Handles generic global paths and contains helpers to work with paths.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class PathsGeneric : public TNonCreatable
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
		 * @returns Combination of two paths.
		 */
		//! @{
		GDAPI static WideString Combine(WideString const& firstPath, WideString const& secondPath);
		GDAPI static WideString Combine(WideString const& firstPath, WideString const& secondPath, WideString const& thirdPath);
		//! @}

		/*!
		 * Normalizes path string - converts all backslashes to slashes, replaces '~' chars and etc.
		 * @param filename Path to the file.
		 */
		GDAPI static WideString Normalize(WideString const& filename)
		{
			GD_NOT_USED(filename);
			GD_NOT_IMPLEMENTED();
		}

		/*!
		 * 'Platformizes' path string - converts path to system-specific path.
		 * @param filename Path to the file.
		 */
		GDAPI static WideString Platformize(WideString const& filename)
		{
			GD_NOT_USED(filename);
			GD_NOT_IMPLEMENTED();
		}

		/*!
		 * Returns extension information for specified path string.
		 * E.g. "directory/file.ext" -> ".ext"
		 * @param filename Path to the file.
		 */
		GDAPI static WideString GetExtension(WideString const& filename);

		/*!
		 * Returns file and extension information for specified path string.
		 * E.g. "directory/file.ext" -> "file.ext"
		 * @param filename Path to the file.
		 */
		GDAPI static WideString GetFileName(WideString const& filename);

		/*!
		 * Returns file information for specified path string (without extension).
		 * E.g. "directory/file.ext" -> "file"
		 *
		 * @param filename Path to the file.
		 */
		GDAPI static WideString GetFileNameWithoutExtension(WideString const& filename);

		/*!
		 * Returns directory information for specified path string.
		 * E.g. "directory/file.ext" -> "directory/"
		 *
		 * @param filename Path to the file.
		 */
		GDAPI static WideString GetDirectoryName(WideString const& filename);

		/*!
		 * Returns directory file information for specified path string (without extension).
		 * E.g. "directory/file.ext" -> "directory/file"
		 *
		 * @param filename Path to the file.
		 */
		GDAPI static WideString GetDirectoryAndFileNameWithoutExtension(WideString const& filename);

		/*!
		 * Returns a full path to the specified path string.
		 * @param filename Path to the file.
		 */
		GDAPI static WideString GetFullPath(WideString const& filename)
		{
			GD_NOT_USED(filename);
			GD_NOT_IMPLEMENTED();
		}

		/*!
		 * Returns relative path from one path to another.
		 *
		 * @param filenameFrom Path to file, that start relative path.
		 * @param filenameTo Path to file, which relative path we need to compute.
		 */
		GDAPI static WideString GetRelativePath(WideString const& filenameFrom, WideString const& filenameTo)
		{
			GD_NOT_USED(filenameFrom);
			GD_NOT_USED(filenameTo);
			GD_NOT_IMPLEMENTED();
		}

		// ------------------------------------------------------------------------------------------
		// Engine specific paths.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Returns path to the directory, where engine core is located.
		 * This maybe useful for reusing single engine core for several applications.
		 */
		GDAPI static WideString const& GetEngineRootDirectory()
		{
			GD_NOT_IMPLEMENTED();
		}

		/*!
		 * Returns path to the directory, where engine binaries are located.
		 */
		GDAPI static WideString const& GetEngineBinariesDirectory();

		/*!
		 * Returns path to the directory, where engine content files are located.
		 */
		GDAPI static WideString const& GetEngineContentDirectory();

		/*!
		 * Returns path to the directory, where engine configuration files are located.
		 */
		GDAPI static WideString const& GetEngineConfigDirectory();

		/*!
		 * Returns path to the directory, where engine can store engine-related files.
		 */
		GDAPI static WideString const& GetEngineSavedDataDirectory();

		/*!
		 * Returns path to the directory, where engine can store user-related files.
		 */
		GDAPI static WideString const& GetEngineSavedDataUserDirectory()
		{
			GD_NOT_IMPLEMENTED();
		}

		// ------------------------------------------------------------------------------------------
		// Application specific paths.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Returns path to the file, where running executable is located.
		 */
		GDAPI static WideString const& GetApplicationExecutableFile()
		{
			GD_NOT_IMPLEMENTED();
		}

		/*!
		 * Returns path to the directory, where application is located.
		 * This maybe useful for reusing single application for several applications.
		 */
		GDAPI static WideString const& GetApplicationRootDirectory();

		/*!
		 * Returns path to the directory, where application binaries are located.
		 */
		GDAPI static WideString const& GetApplicationBinariesDirectory();

		/*!
		 * Returns path to the directory, where application content files are located.
		 */
		GDAPI static WideString const& GetApplicationContentDirectory();

		/*!
		 * Returns path to the directory, where application configuration files are located.
		 */
		GDAPI static WideString const& GetApplicationConfigDirectory();

		/*!
		 * Returns path to the directory, where application can store application-related files.
		 */
		GDAPI static WideString const& GetApplicationSavedDataDirectory();

		/*!
		 * Returns path to the directory, where application can store user-related files.
		 */
		GDAPI static WideString const& GetApplicationSavedDataUserDirectory();

	};	// class PathsGeneric

GD_NAMESPACE_END

#include GD_PLATFORM_INCLUDE(GoddamnEngine/Core/System/IO, Paths)
#undef GD_INSIDE_DIRECTORY_H
