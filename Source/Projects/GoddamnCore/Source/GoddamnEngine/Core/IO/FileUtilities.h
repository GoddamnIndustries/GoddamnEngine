// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/IO/FileUtilities.h
 * File contains file and directory utilities generic interface.
 */
#pragma once
#define GD_INSIDE_FILEUTILITIES_H

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/PlatformSpecificInclude.h>
#include <GoddamnEngine/Core/Containers/String.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Generic file and directory utilities interface.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class FileUtilitiesGeneric : public TNonCreatable
	{
	public:

		// ------------------------------------------------------------------------------------------
		// File utilities.
		// ------------------------------------------------------------------------------------------

		/*
		 * Returns true if the specified file exists and is not directory.
		 * @param filename Path to the file.
		 */
		GDAPI static bool FileExists(WideString const& filename);

		/*!
		 * Returns size of the file in bytes, or -1 if it does not exist.
		 * @param filename Path to the file.
		 */
		GDAPI static UInt64 FileSize(WideString const& filename);

		/*!
		 * Creates an empty file with the specified file name.
		 * If file already exists, nothing is done.
		 *
		 * @param filename Path to the file.
		 * @returns True if file exists or was successfully created.
		 */
		GDAPI static bool FileCreateEmpty(WideString const& filename);

		/*!
		 * Removes the existing file.
		 *
		 * @param filename Path to the file.
		 * @returns True if file was successfully deleted.
		 */
		GDAPI static bool FileRemove(WideString const& filename);

		/*!
		 * Moves file from source path to destination.
		 *
		 * @param sourceFilename Path to the file.
		 * @param destFilename Destination file path.
		 * @param doOverwrite Do overwrite destination file if it exists.
		 * @returns True if file was successfully moved.
		 */
		GDAPI static bool FileMove(WideString const& sourceFilename, WideString const& destFilename, bool const doOverwrite = false);

		/*!
		 * Copies file from source path to destination.
		 *
		 * @param sourceFilename Path to the file.
		 * @param destFilename Destination file path.
		 * @param doOverwrite Do overwrite destination file if it exists.
		 * @returns True if file was successfully moved.
		 */
		GDAPI static bool FileCopy(WideString const& sourceFilename, WideString const& destFilename, bool const doOverwrite = false);

		// ------------------------------------------------------------------------------------------
		// Directory utilities.
		// ------------------------------------------------------------------------------------------

		/*
		 * Returns true if the specified file exists and is a directory.
		 * @param directoryName Name to the directory.
		 */
		GDAPI static bool DirectoryExists(WideString const& directoryName);

		/*!
		 * Creates an empty directory with the specified directory name.
		 * If directory already exists, nothing is done.
		 *
		 * @param directoryName Path to the directory.
		 * @returns True if directory exists or was successfully created.
		 */
		GDAPI static bool DirectoryCreateEmpty(WideString const& directoryName);

		/*!
		 * Removes the existing empty directory.
		 *
		 * @param directoryName Path to the directory.
		 * @returns True if directory was successfully deleted.
		 */
		GDAPI static bool DirectoryRemove(WideString const& directoryName);

		/*!
		 * Removes the existing directory and everything inside it.
		 *
		 * @param directoryName Path to the directory.
		 * @returns True if directory was successfully deleted.
		 */
		GDAPI static bool DirectoryRemoveRecursive(WideString const& directoryName);

	};	// class FileUtilitiesGeneric
		
GD_NAMESPACE_END

// ReSharper disable once CppUnusedIncludeDirective
#include GD_PLATFORM_INCLUDE(GoddamnEngine/Core/IO, FileUtilities)
#undef GD_INSIDE_FILEUTILITIES_H
