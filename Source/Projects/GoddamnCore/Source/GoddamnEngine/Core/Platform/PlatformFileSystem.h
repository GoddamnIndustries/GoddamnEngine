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
#include <GoddamnEngine/Core/Platform/PlatformIO.h>
#include <GoddamnEngine/Core/Templates/Singleton.h>

GD_NAMESPACE_BEGIN
	
	/*!
	 * Directory iteration delegate.
	 */
	class IFileSystemDirectoryIterateDelegate : public ReferenceTarget
	{
	public:

		/*!
		 * Directory iteration callback.
		 * 
		 * @param path Path to the directory entry.
		 * @param isDirectory True if the entry itself is a subdirectory.
		 * 
		 * @returns If false was returned, iterations would stop.
		 */
		GDINT virtual bool OnVisitDirectoryEntry(WideString const& path, bool const isDirectory)
		{
			GD_NOT_USED_L(path, isDirectory);
			return true;
		}
	};	// class IFileSystemDirectoryIterateDelegate

	/*!
	 * File system watcher delegate.
	 */
	class IFileSystemWatcherDelegate : public ReferenceTarget
	{
	public:

		/*!
		 * Callback being triggered by the filesystem watcher when new file or directory was created
		 * or some existing file or directory is moved into the watch directory.
		 *
		 * @param path Path the entity that was created.
		 */
		GDINT virtual void OnFileOrDirectoryCreated(WideString const& path)
		{
			GD_NOT_USED_L(path);
		}

		/*!
		 * Callback being triggered by the filesystem watcher when new file or directory was removed
		 * or some existing file or directory is moved out from the watch directory.
		 *
		 * @param path Path the entity that was removed.
		 */
		GDINT virtual void OnFileOrDirectoryRemoved(WideString const& path)
		{
			GD_NOT_USED_L(path);
		}

		/*!
		 * Callback being triggered by the filesystem watcher when new file or directory was modified.
		 * @param path Path the entity that was removed.
		 */
		GDINT virtual void OnFileOrDirectoryModified(WideString const& path)
		{
			GD_NOT_USED_L(path);
		}

		/*!
		 * Callback being triggered by the filesystem watcher when new file or directory was moved.
		 * 
		 * @param srcPath Old path to the entity that was moved.
		 * @param dstPath New path to the entity that was moved.
		 */
		GDINT virtual void OnFileOrDirectoryMoved(WideString const& srcPath, WideString const& dstPath)
		{
			GD_NOT_USED_L(srcPath, dstPath);
		}
	};	// class IFileSystemWatcherDelegate

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! File system.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class IFileSystem : public IVirtuallyDestructible
	{
	public:

		// ------------------------------------------------------------------------------------------
		// File utilities.
		// ------------------------------------------------------------------------------------------

		/*!
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
		 * @returns True if file size was successfully obtained.
		 */
		GDINT virtual bool FileSize(WideString const& filename, UInt64& fileSize) const GD_PURE_VIRTUAL;

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
		GDINT virtual bool FileMove(WideString const& srcFilename, WideString const& dstFilename, bool const doOverwrite = false);

		/*!
		 * Copies file from source path to destination.
		 *
		 * @param srcFilename Path to the file.
		 * @param dstFilename Destination file path.
		 * @param doOverwrite Do overwrite destination file if it exists.
		 * 
		 * @returns True if file was successfully moved.
		 */
		GDINT virtual bool FileCopy(WideString const& srcFilename, WideString const& dstFilename, bool const doOverwrite = false);

		/*!
		 * Opens a input stream for the specified file.
		 * 
		 * @param filename Path to the file.
		 * @returns Opened valid input stream or null pointer if operation has failed.
		 */
		GDINT virtual SharedPtr<IInputStream> FileStreamOpenRead(WideString const& filename) const GD_PURE_VIRTUAL;

		/*!
		 * Opens a output stream for the specified file.
		 * 
		 * @param filename Path to the file.
		 * @param doAppend If true new data would be written to the end of file.
		 * 
		 * @returns Opened valid output stream or null pointer if operation has failed.
		 */
		GDINT virtual SharedPtr<IOutputStream> FileStreamOpenWrite(WideString const& filename, bool const doAppend = false) GD_PURE_VIRTUAL;

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

        /*!
         * Iterates through all entities of a directory.
         *
         * @param directoryName Path to the directory.
         * @param directoryIterateDelegate Delegate for the directory traversal.
         *
         * @returns True if directory exists or was successfully iterated.
         */
        GDINT virtual bool DirectoryIterate(WideString const& directoryName, IFileSystemDirectoryIterateDelegate& directoryIterateDelegate) const GD_PURE_VIRTUAL;

		/*!
         * Recursively iterates through all entities of a directory.
         *
         * @param directoryName Path to the directory.
         * @param directoryIterateDelegate Delegate for the directory traversal.
         *
         * @returns True if directory exists or was successfully iterated.
         */
        GDINT virtual bool DirectoryIterateRecursive(WideString const& directoryName, IFileSystemDirectoryIterateDelegate& directoryIterateDelegate) const;
	};	// class IFileSystem

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Disk file system.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL IPlatformDiskFileSystem : public IFileSystem, public Singleton<IPlatformDiskFileSystem>
	{
	public:
		GDINT virtual SharedPtr<IInputStream> FileStreamOpenRead(WideString const& filename) const override final;
		GDINT virtual SharedPtr<IOutputStream> FileStreamOpenWrite(WideString const& filename, bool const doAppend) override final;

	public:

		// ------------------------------------------------------------------------------------------
		// File IO utilities.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Opens an input handle for the specified file.
		 * 
		 * @param filename Path to the file.
		 * @param fileHandle File handle.
		 * 
		 * @returns True if file was successfully opened.
		 */
		GDINT virtual bool FileOpenRead(WideString const& filename, Handle& fileHandle) const GD_PURE_VIRTUAL;

		/*!
		 * Opens an output handle for the specified file.
		 * 
		 * @param filename Path to the file.
		 * @param fileHandle File handle.
		 * @param doAppend If true new data would be written to the end of file.
		 * 
		 * @returns True if file was successfully opened.
		 */
		GDINT virtual bool FileOpenWrite(WideString const& filename, Handle& fileHandle, bool const doAppend = false) GD_PURE_VIRTUAL;

		/*!
		 * Closes a file handle.
		 * 
		 * @param fileHandle File handle.
		 * @returns True if file was successfully closed.
		 */
		GDINT virtual bool FileClose(Handle const fileHandle) const GD_PURE_VIRTUAL;

		/*!
		 * Reposition this file handle to new specified position.
		 *
		 * @param fileHandle File handle.
		 * @param offset The offset in bytes from specified origin.
		 * @param origin Defines origin from which point make offset.
		 * @param newPosition New position in file.
		 * 
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool FileSeek(Handle const fileHandle, Int64 const offset, SeekOrigin const origin, UInt64* const newPosition = nullptr) const GD_PURE_VIRTUAL;

		/*!
		 * Reads data from file.
		 *
		 * @param fileHandle File handle.
		 * @param readBuffer Output memory to which data would be written.
		 * @param readBufferSizeBytes Length of the element in bytes.
		 * @param numBytesRead Amount of bytes that was read from file.
		 * 
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool FileRead(Handle const fileHandle, Handle const readBuffer, UInt32 const readBufferSizeBytes, UInt32* const numBytesRead = nullptr) const GD_PURE_VIRTUAL;

		/*!
		 * Writes data to file.
		 *
		 * @param fileHandle File handle.
		 * @param writeBuffer Input memory that would be written.
		 * @param writeBufferSizeBytes Length of the element in bytes.
		 * @param numBytesWritten Amount of bytes that were written to file.
		 * 
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool FileWrite(Handle const fileHandle, CHandle const writeBuffer, UInt32 const writeBufferSizeBytes, UInt32* const numBytesWritten = nullptr) GD_PURE_VIRTUAL;
	};	// class IPlatformDiskFileSystem

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Disk file system watcher.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL IPlatformDiskFileSystemWatcher : public Singleton<IPlatformDiskFileSystemWatcher>
	{
	public:

		// ------------------------------------------------------------------------------------------
		// Watch utilities.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Creates a new watch for the specified directory.
		 *
		 * @param directoryName Path to the directory.
		 * @param watchHandle Watch handle.
		 *
		 * @returns True if directory exists or new watch was successfully created.
		 */
		GDINT virtual bool WatchCreate(WideString const& directoryName, Handle& watchHandle) GD_PURE_VIRTUAL;

		/*!
		 * Closes a watch handle.
		 *
		 * @param watchHandle Watch handle.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool WatchDestroy(Handle const watchHandle) GD_PURE_VIRTUAL;

		/*!
		 * Reads all pending file system events of the specified watch.
		 *
		 * @param directoryName Path to the directory.
		 * @param directoryWatcherDelegate Watch event delegate.
		 * 
		 * @returns True if directory exists or new watch was successfully added.
		 */
		GDINT virtual bool WatchReadEvents(Handle const watchHandle, IFileSystemWatcherDelegate& directoryWatcherDelegate) GD_PURE_VIRTUAL;
	};	// class IPlatformDiskFileSystemWatcher

GD_NAMESPACE_END
