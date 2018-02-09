// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file
 * File system implementation.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/IO/Stream.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Templates/Singleton.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! File directory visitor.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class IFileSystemDirectoryVistor : public IVirtuallyDestructible
	{
	public:

		GDINT virtual void Visit(WideString const& path, bool const isDirectory) GD_PURE_VIRTUAL;

	};	// class IFileSystemDirectoryVistor

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! File system.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL IFileSystem : public IVirtuallyDestructible
	{
	public:

		// ------------------------------------------------------------------------------------------
		// File utilities.
		// ------------------------------------------------------------------------------------------

		/*
		 * Returns true if the specified file exists and is not directory.
		 * @param filename Path to the file.
		 */
		GDINT virtual bool FileExists(WideString const& filename) const GD_PURE_VIRTUAL;

		/*!
		 * Gets size of the file in bytes, or -1 if it does not exist.
		 * 
		 * @param filename Path to the file.
		 * @param fileSize Output for file size.
		 * 
		 * @returns True if file size was succesfully obtained.
		 */
		GDINT virtual bool FileSize(WideString const& filename, UINT64& fileSize) const GD_PURE_VIRTUAL;

		/*!
		 * Creates an empty file with the specified file name.
		 * If file already exists, nothing is done.
		 *
		 * @param filename Path to the file.
		 * @returns True if file exists or was successfully created.
		 */
		GDINT virtual bool FileCreateEmpty(WideString const& filename) GD_PURE_VIRTUAL;

		/*!
		 * Removes the existing file.
		 *
		 * @param filename Path to the file.
		 * @returns True if file was successfully deleted.
		 */
		GDINT virtual bool FileRemove(WideString const& filename) GD_PURE_VIRTUAL;

		/*!
		 * Moves file from source path to destination.
		 *
		 * @param srcFilename Path to the file.
		 * @param dstFilename Destination file path.
		 * @param doOverwrite Do overwrite destination file if it exists.
		 * 
		 * @returns True if file was successfully moved.
		 */
		GDINT virtual bool FileMove(WideString const& srcFilename, WideString const& dstFilename, bool const doOverwrite = false) GD_PURE_VIRTUAL;

		/*!
		 * Copies file from source path to destination.
		 *
		 * @param srcFilename Path to the file.
		 * @param dstFilename Destination file path.
		 * @param doOverwrite Do overwrite destination file if it exists.
		 * 
		 * @returns True if file was successfully moved.
		 */
		GDINT virtual bool FileCopy(WideString const& srcFilename, WideString const& dstFilename, bool const doOverwrite = false) GD_PURE_VIRTUAL;

		/*!
		 * Opens a input stream for the specified file.
		 * 
		 * @param filename Path to the file.
		 * @returns Opened valid input stream or null pointer if operation has failed.
		 */
		GDINT virtual SharedPtr<InputStream> FileOpenRead(WideString const& filename) const GD_PURE_VIRTUAL;

		/*!
		 * Opens a output stream for the specified file.
		 * 
		 * @param filename Path to the file.
		 * @param doAppend If true new data would be written to the end of file.
		 * 
		 * @returns Opened valid output stream or null pointer if operation has failed.
		 */
		GDINT virtual SharedPtr<OutputStream> FileOpenWrite(WideString const& filename, bool const doAppend) const GD_PURE_VIRTUAL;

		// ------------------------------------------------------------------------------------------
		// Directory utilities.
		// ------------------------------------------------------------------------------------------

		/*
		 * Returns true if the specified file exists and is a directory.
		 * @param directoryName Name to the directory.
		 */
		GDINT virtual bool DirectoryExists(WideString const& directoryName) const GD_PURE_VIRTUAL;

		/*!
		 * Creates an empty directory with the specified directory name.
		 * If directory already exists, nothing is done.
		 *
		 * @param directoryName Path to the directory.
		 * @returns True if directory exists or was successfully created.
		 */
		GDINT virtual bool DirectoryCreateEmpty(WideString const& directoryName) const GD_PURE_VIRTUAL;

		/*!
		 *
		 * @returns True if directory exists or was successfully traversed.
		 */
		//GDINT virtual bool DirectoryVisit() const GD_PURE_VIRTUAL;

		/*!
		 * Removes the existing empty directory.
		 *
		 * @param directoryName Path to the directory.
		 * @returns True if directory was successfully deleted.
		 */
		GDINT virtual bool DirectoryRemove(WideString const& directoryName) GD_PURE_VIRTUAL;

		/*!
		 * Removes the existing directory and everything inside it.
		 *
		 * @param directoryName Path to the directory.
		 * @returns True if directory was successfully deleted.
		 */
		GDINT virtual bool DirectoryRemoveRecursive(WideString const& directoryName);

	};	// class IFileSystem

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Disk file system.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL IPlatformDiskFileSystem : public IFileSystem, public Singleton<IPlatformDiskFileSystem>
	{
	};	// class IPlatformDiskFileSystem

GD_NAMESPACE_END
