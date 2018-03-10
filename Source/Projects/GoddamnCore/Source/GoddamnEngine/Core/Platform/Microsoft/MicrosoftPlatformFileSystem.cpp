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
#include <GoddamnEngine/Core/Platform/PlatformFileSystem.h>
#if GD_PLATFORM_API_MICROSOFT

#include <GoddamnEngine/Core/Platform/PlatformApplication.h>
#include <GoddamnEngine/Core/Templates/UniquePtr.h>
#include "GoddamnEngine/Core/IO/Paths.h"

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Disk file system on Microsoft platforms.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL MicrosoftPlatformDiskFileSystem final : public IPlatformDiskFileSystem
	{
	private:

		// ------------------------------------------------------------------------------------------
		// File utilities.
		// ------------------------------------------------------------------------------------------

		GDINT virtual bool FileExists(WideString const& filename) const override final;
		GDINT virtual bool FileSize(WideString const& filename, UInt64& fileSize) const override final;
		GDINT virtual bool FileCreateEmpty(WideString const& filename) override final;
		GDINT virtual bool FileRemove(WideString const& filename) override final;
		GDINT virtual bool FileMove(WideString const& srcFilename, WideString const& dstFilename, bool const doOverwrite) override final;
		GDINT virtual bool FileCopy(WideString const& srcFilename, WideString const& dstFilename, bool const doOverwrite) override final;

		// ------------------------------------------------------------------------------------------
		// File IO utilities.
		// ------------------------------------------------------------------------------------------

		GDINT virtual bool FileOpenRead(WideString const& filename, Handle& fileHandle) const override final;
		GDINT virtual bool FileOpenWrite(WideString const& filename, Handle& fileHandle, bool const doAppend) override final;
		GDINT virtual bool FileClose(Handle const fileHandle) const override final;
		GDINT virtual bool FileSeek(Handle const fileHandle, Int64 const offset, SeekOrigin const origin, UInt64* const newPosition) const override final;
		GDINT virtual bool FileRead(Handle const fileHandle, Handle const readBuffer, UInt32 const readBufferSizeBytes, UInt32* const numBytesRead) const override final;
		GDINT virtual bool FileWrite(Handle const fileHandle, CHandle const readBuffer, UInt32 const readBufferSizeBytes, UInt32* const numBytesRead) override final;

		// ------------------------------------------------------------------------------------------
		// Directory utilities.
		// ------------------------------------------------------------------------------------------

		GDINT virtual bool DirectoryExists(WideString const& directoryName) const override final;
		GDINT virtual bool DirectoryCreateEmpty(WideString const& directoryName) const override final;
		GDINT virtual bool DirectoryRemove(WideString const& directoryName) override final;
		GDINT virtual bool DirectoryIterate(WideString const& directoryName, IFileSystemDirectoryIterateDelegate& directoryIterateDelegate) const override final;
	};	// class MicrosoftPlatformDiskFileSystem

	GD_IMPLEMENT_SINGLETON(IPlatformDiskFileSystem, MicrosoftPlatformDiskFileSystem)

	// ------------------------------------------------------------------------------------------
	// File utilities.
	// ------------------------------------------------------------------------------------------

	/*!
	 * Returns true if the specified file exists and is not directory.
	 * @param filename Path to the file.
	 */
	GDINT bool MicrosoftPlatformDiskFileSystem::FileExists(WideString const& filename) const
	{
		auto const filenameSystem = Paths::Platformize(filename);
		auto const fileAttributes = GetFileAttributesW(filenameSystem.CStr());
		return fileAttributes != INVALID_FILE_ATTRIBUTES && (fileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
	}

	/*!
	 * Gets size of the file in bytes, or -1 if it does not exist.
	 * 
	 * @param filename Path to the file.
	 * @param fileSize Output for file size.
	 * 
	 * @returns True if file size was successfully obtained.
	 */
	GDINT bool MicrosoftPlatformDiskFileSystem::FileSize(WideString const& filename, UInt64& fileSize) const
	{
		WIN32_FILE_ATTRIBUTE_DATA fileAttributeData = {};
		auto const filenameSystem = Paths::Platformize(filename);
		if (GetFileAttributesExW(filenameSystem.CStr(), GetFileExInfoStandard, &fileAttributeData) == TRUE)
		{
			if ((fileAttributeData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				LARGE_INTEGER fileSizeSystem;
				fileSizeSystem.HighPart = fileAttributeData.nFileSizeHigh;
				fileSizeSystem.LowPart = fileAttributeData.nFileSizeLow;
				fileSize = fileSizeSystem.QuadPart;
				return true;
			}
		}
		return false;
	}

	/*!
	 * Creates an empty file with the specified file name.
	 * If file already exists, nothing is done.
	 *
	 * @param filename Path to the file.
	 * @returns True if file exists or was successfully created.
	 */
	GDINT bool MicrosoftPlatformDiskFileSystem::FileCreateEmpty(WideString const& filename)
	{
		if (!FileExists(filename))
		{
			auto const filenameSystem = Paths::Platformize(filename);
			auto const emptyFile = CreateFile2(filenameSystem.CStr(), GENERIC_WRITE, FILE_SHARE_READ, CREATE_NEW, nullptr);
			if (emptyFile != INVALID_HANDLE_VALUE)
			{
				CloseHandle(emptyFile);
				return true;
			}
			return false;
		}
		return true;
	}

	/*!
	 * Removes the existing file.
	 *
	 * @param filename Path to the file.
	 * @returns True if file was successfully deleted.
	 */
	GDINT bool MicrosoftPlatformDiskFileSystem::FileRemove(WideString const& filename)
	{
		auto const filenameSystem = Paths::Platformize(filename);
		if (DeleteFileW(filenameSystem.CStr()) == FALSE)
		{
			Sleep(0);
			return DeleteFileW(filenameSystem.CStr()) == TRUE;
		}
		return true;
	}

	/*!
	 * Moves file from source path to destination.
	 *
	 * @param srcFilename Path to the file.
	 * @param dstFilename Destination file path.
	 * @param doOverwrite Do overwrite destination file if it exists.
	 * 
	 * @returns True if file was successfully moved.
	 */
	GDINT bool MicrosoftPlatformDiskFileSystem::FileMove(WideString const& srcFilename, WideString const& dstFilename, bool const doOverwrite)
	{
		if (DirectoryExists(srcFilename))
		{
			// Specified path is a path to file, this is an error.
			return false;
		}
		auto const srcFilenameSystem = Paths::Platformize(srcFilename);
		auto const dstFilenameSystem = Paths::Platformize(dstFilename);
		if (MoveFileExW(srcFilenameSystem.CStr(), dstFilenameSystem.CStr(), doOverwrite ? MOVEFILE_REPLACE_EXISTING : 0) == FALSE)
		{
			Sleep(0);
			return MoveFileExW(srcFilenameSystem.CStr(), dstFilenameSystem.CStr(), doOverwrite ? MOVEFILE_REPLACE_EXISTING : 0) == TRUE;
		}
		return true;
	}

	/*!
	 * Copies file from source path to destination.
	 *
	 * @param srcFilename Path to the file.
	 * @param dstFilename Destination file path.
	 * @param doOverwrite Do overwrite destination file if it exists.
	 * 
	 * @returns True if file was successfully moved.
	 */
	GDINT bool MicrosoftPlatformDiskFileSystem::FileCopy(WideString const& srcFilename, WideString const& dstFilename, bool const doOverwrite)
	{
		if (DirectoryExists(srcFilename))
		{
			// Specified path is a path to file, this is an error.
			return false;
		}
		auto const srcFilenameSystem = Paths::Platformize(srcFilename);
		auto const dstFilenameSystem = Paths::Platformize(dstFilename);
		if (CopyFileW(srcFilenameSystem.CStr(), dstFilenameSystem.CStr(), !doOverwrite) == FALSE)
		{
			Sleep(0);
			return CopyFileW(srcFilenameSystem.CStr(), dstFilenameSystem.CStr(), !doOverwrite) == TRUE;
		}
		return true;
	}

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
	GDINT bool MicrosoftPlatformDiskFileSystem::FileOpenRead(WideString const& filename, Handle& fileHandle) const
	{
		auto const filenameSystem = Paths::Platformize(filename);
		auto const fileHandleSystem = CreateFile2(filenameSystem.CStr(), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr);
		if (fileHandleSystem != INVALID_HANDLE_VALUE)
		{
			fileHandle = fileHandleSystem;
			return true;
		}
		return false;
	}

	/*!
	 * Opens an output handle for the specified file.
	 * 
	 * @param filename Path to the file.
	 * @param fileHandle File handle.
	 * @param doAppend If true new data would be written to the end of file.
	 * 
	 * @returns True if file was successfully opened.
	 */
	GDINT bool MicrosoftPlatformDiskFileSystem::FileOpenWrite(WideString const& filename, Handle& fileHandle, bool const doAppend)
	{
		auto const filenameSystem = Paths::Platformize(filename);
		auto const fileHandleSystem = CreateFile2(filenameSystem.CStr(), doAppend ? FILE_APPEND_DATA : GENERIC_WRITE, FILE_SHARE_READ, doAppend ? OPEN_ALWAYS : CREATE_ALWAYS, nullptr);
		if (fileHandleSystem != INVALID_HANDLE_VALUE)
		{
			fileHandle = fileHandleSystem;
			return true;
		}
		return false;
	}

	/*!
	 * Closes a file handle.
	 * 
	 * @param fileHandle File handle.
	 * @returns True if file was successfully closed.
	 */
	GDINT bool MicrosoftPlatformDiskFileSystem::FileClose(Handle const fileHandle) const
	{
		GD_ASSERT(fileHandle != nullptr);
		return CloseHandle(fileHandle) == TRUE;
	}

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
	GDINT bool MicrosoftPlatformDiskFileSystem::FileSeek(Handle const fileHandle, Int64 const offset, SeekOrigin const origin, UInt64* const newPosition) const
	{
		GD_ASSERT(fileHandle != nullptr);

		DWORD originSystem = 0;
		switch (origin)
		{
			case SeekOrigin::Beginning:
				originSystem = FILE_BEGIN;
				break;
			case SeekOrigin::Current:
				originSystem = FILE_CURRENT;
				break;
			case SeekOrigin::End:
				originSystem = FILE_END;
				break;
		}

		LARGE_INTEGER offsetSystem;
		offsetSystem.QuadPart = offset;
		if (newPosition != nullptr)
		{
			LARGE_INTEGER newFilePointerSystem;
			if (SetFilePointerEx(fileHandle, offsetSystem, &newFilePointerSystem, originSystem) == TRUE)
			{
				*newPosition = newFilePointerSystem.QuadPart;
				return true;
			}
		}
		else
		{
			if (SetFilePointerEx(fileHandle, offsetSystem, nullptr, originSystem) == TRUE)
			{
				return true;
			}
		}
		return false;
	}

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
	GDINT bool MicrosoftPlatformDiskFileSystem::FileRead(Handle const fileHandle, Handle const readBuffer, UInt32 const readBufferSizeBytes, UInt32* const numBytesRead) const
	{
		GD_ASSERT(fileHandle != nullptr);
		if (numBytesRead != nullptr)
		{
			DWORD numBytesReadSystem = 0;
			if (ReadFile(fileHandle, readBuffer, readBufferSizeBytes, &numBytesReadSystem, nullptr) == TRUE)
			{
				*numBytesRead = numBytesReadSystem;
				return true;
			}
		}
		else
		{
			if (ReadFile(fileHandle, readBuffer, readBufferSizeBytes, nullptr, nullptr) == TRUE)
			{
				return true;
			}
		}
		return false;
	}

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
	GDINT bool MicrosoftPlatformDiskFileSystem::FileWrite(Handle const fileHandle, CHandle const writeBuffer, UInt32 const writeBufferSizeBytes, UInt32* const numBytesWritten)
	{
		GD_ASSERT(fileHandle != nullptr);
		if (numBytesWritten != nullptr)
		{
			DWORD numBytesReadSystem = 0;
			if (WriteFile(fileHandle, writeBuffer, writeBufferSizeBytes, &numBytesReadSystem, nullptr) == TRUE)
			{
				*numBytesWritten = numBytesReadSystem;
				return true;
			}
		}
		else
		{
			if (WriteFile(fileHandle, writeBuffer, writeBufferSizeBytes, nullptr, nullptr) == TRUE)
			{
				return true;
			}
		}
		return false;
	}

	// ------------------------------------------------------------------------------------------
	// Directory utilities.
	// ------------------------------------------------------------------------------------------

	/*
	 * Returns true if the specified file exists and is a directory.
	 * @param directoryName Name to the directory.
	 */
	GDINT bool MicrosoftPlatformDiskFileSystem::DirectoryExists(WideString const& directoryName) const
	{
		auto const directoryNameSystem = Paths::Platformize(directoryName);
		auto const directoryAttributes = GetFileAttributesW(directoryNameSystem.CStr());
		return directoryAttributes != INVALID_FILE_ATTRIBUTES && (directoryAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
	}

	/*!
	 * Creates an empty directory with the specified directory name.
	 * If directory already exists, nothing is done.
	 *
	 * @param directoryName Path to the directory.
	 * @returns True if directory exists or was successfully created.
	 */
	GDINT bool MicrosoftPlatformDiskFileSystem::DirectoryCreateEmpty(WideString const& directoryName) const
	{
		auto const directoryNameSystem = Paths::Platformize(directoryName);
		if (CreateDirectoryW(directoryNameSystem.CStr(), nullptr) == FALSE)
		{
			if (GetLastError() != ERROR_ALREADY_EXISTS)
			{
				Sleep(0);
				return CreateDirectoryW(directoryNameSystem.CStr(), nullptr) == TRUE;
			}
			if (FileExists(directoryName))
			{
				// Specified path is a path to file, this is an error.
				return false;
			}
		}
		return true;
	}

	/*!
	 * Removes the existing empty directory.
	 *
	 * @param directoryName Path to the directory.
	 * @returns True if directory was successfully deleted.
	 */
	GDINT bool MicrosoftPlatformDiskFileSystem::DirectoryRemove(WideString const& directoryName)
	{
		auto const directoryNameSystem = Paths::Platformize(directoryName);
		if (RemoveDirectoryW(directoryNameSystem.CStr()) == FALSE)
		{
			Sleep(0);
			return RemoveDirectoryW(directoryNameSystem.CStr()) == TRUE;
		}
		return true;
	}

	/*!
	 * Iterates through all entities of a directory.
	 *
	 * @param directoryName Path to the directory.
	 * @param directoryIterateDelegate Delegate for the directory traversal.
	 *
	 * @returns True if directory exists or was successfully iterated.
	 */
	GDINT bool MicrosoftPlatformDiskFileSystem::DirectoryIterate(WideString const& directoryName, IFileSystemDirectoryIterateDelegate& directoryIterateDelegate) const
	{
		WIN32_FIND_DATAW directoryEntry = {};
		auto const directoryNameSystem = Paths::Platformize(Paths::Combine(directoryName, L"*"));
		auto const directoryHandle = FindFirstFileW(directoryNameSystem.CStr(), &directoryEntry);
		if (directoryHandle != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (CString::Strcmp(directoryEntry.cFileName, L".") != 0 && CString::Strcmp(directoryEntry.cFileName, L"..") != 0)
				{
					auto const directoryEntryName = Paths::Combine(directoryName, directoryEntry.cFileName);
					auto const directoryEntryIsDirectory = (directoryEntry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
					if (!directoryIterateDelegate.OnVisitDirectoryEntry(directoryEntryName.CStr(), directoryEntryIsDirectory))
					{
						return false;
					}
				}
			} while (FindNextFileW(directoryHandle, &directoryEntry));
			return FindClose(directoryHandle) == TRUE;
		}
		return false;
	}

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! ReadDirectoryChangesW event iterator.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL ReadDirectoryChangesEventIterator final
	{
	private:
		Byte const* m_BufferCurrent;
		Byte const* m_BufferEnd;
	public:
		GDINL explicit ReadDirectoryChangesEventIterator(Byte const* const bufferCurrent, Byte const* const bufferEnd)
			: m_BufferCurrent(bufferCurrent), m_BufferEnd(bufferEnd)
		{
		}
	public:
		GDINL bool IsEnd() const
		{
			return m_BufferCurrent < m_BufferEnd;
		}
	public:
		// *iterator
		GDINL FILE_NOTIFY_EXTENDED_INFORMATION const& operator* () const
		{
			GD_ASSERT(IsEnd());
			return *reinterpret_cast<FILE_NOTIFY_EXTENDED_INFORMATION const*>(m_BufferCurrent);
		}
		// iterator->
		GDINL FILE_NOTIFY_EXTENDED_INFORMATION const* operator-> () const
		{
			GD_ASSERT(IsEnd());
			return reinterpret_cast<FILE_NOTIFY_EXTENDED_INFORMATION const*>(m_BufferCurrent);
		}
		// iterator++
		GDINL ReadDirectoryChangesEventIterator& operator++ ()
		{
			auto const nextEntryOffset = (**this).NextEntryOffset;
			if (nextEntryOffset != 0)
			{
				m_BufferCurrent += nextEntryOffset;
			}
			else
			{
				m_BufferCurrent = m_BufferEnd;
			}
			return *this;
		}
	};  // class ReadDirectoryChangesEventIterator

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! ReadDirectoryChangesW event queue.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL ReadDirectoryChangesEventQueue final : public TNonCopyable
	{
	private:
		UniquePtr<Byte[]> m_QueueBuffer;
		DWORD m_QueueLength;
	public:
		GDINL explicit ReadDirectoryChangesEventQueue()
			: m_QueueBuffer(gd_new Byte[GetCapacity()]), m_QueueLength(0)
		{
		}
	public:
		GDINL static DWORD GetCapacity()
		{
			return 1024 * (sizeof(FILE_NOTIFY_INFORMATION) + FILENAME_MAX);
		}
		GDINL Byte* GetData() const
		{
			return m_QueueBuffer.Get();
		}
		GDINL DWORD GetLength() const
		{
			return m_QueueLength;
		}
		GDINL void Resize(DWORD const newLength)
		{
			m_QueueLength = newLength;
		}
	public:
		GDINL ReadDirectoryChangesEventIterator Begin() const
		{
			return ReadDirectoryChangesEventIterator(m_QueueBuffer.Get(), m_QueueBuffer.Get() + GetLength());
		}
	};  // class ReadDirectoryChangesEventQueue

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Disk file system watcher on Microsoft platforms.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL MicrosoftPlatformDiskFileSystemWatcher : public IPlatformDiskFileSystemWatcher
	{
	private:
		GDINT virtual bool WatchDirectoryRecursive(WideString const& directoryName, IFileSystemWatcherDelegate& directoryWatcherDelegate) override final;
	};	// class MicrosoftPlatformDiskFileSystemWatcher

	GD_IMPLEMENT_SINGLETON(IPlatformDiskFileSystemWatcher, MicrosoftPlatformDiskFileSystemWatcher)

	// ------------------------------------------------------------------------------------------
	// Watch utilities.
	// ------------------------------------------------------------------------------------------

	/*!
	 * Watches all filesystem events on the specified directory.
	 * This function returns on application exit.
	 *
	 * @param directoryName Path to the directory.
	 * @param directoryWatcherDelegate Watch event delegate.
	 *
	 * @returns True if operation succeeded.
	 */
	GDINT bool MicrosoftPlatformDiskFileSystemWatcher::WatchDirectoryRecursive(WideString const& directoryName, IFileSystemWatcherDelegate& directoryWatcherDelegate)
	{
		auto const directoryNameSystem = Paths::Platformize(directoryName);
		auto const directoryHandle = CreateFileW(directoryNameSystem.CStr(), FILE_LIST_DIRECTORY
			, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
		if (directoryHandle == INVALID_HANDLE_VALUE)
		{
			return false;
		}

		ReadDirectoryChangesEventQueue directoryEventQueue;
		while (IPlatformApplication::Get().IsRunning())
		{
			DWORD lengthOfEvents = 0;
			if (ReadDirectoryChangesExW(directoryHandle
				, directoryEventQueue.GetData(), directoryEventQueue.GetCapacity(), TRUE
				, FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_FILE_NAME
				, &lengthOfEvents, nullptr, nullptr, ReadDirectoryNotifyExtendedInformation) == FALSE)
			{
				CloseHandle(directoryHandle);
				return false;
			}

			directoryEventQueue.Resize(lengthOfEvents);
			for (auto watchEventIterator = directoryEventQueue.Begin(); watchEventIterator.IsEnd(); ++watchEventIterator)
			{
				switch (watchEventIterator->Action)
				{
					// File or directory was created or moved into watch directory.
					case FILE_ACTION_ADDED:
						{
							auto const watchEventIsDir = (watchEventIterator->FileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
							auto const watchEventPath = Paths::Combine(directoryName, WideString(watchEventIterator->FileName, watchEventIterator->FileNameLength / sizeof(watchEventIterator->FileName[0])));
							if (!directoryWatcherDelegate.OnFileOrDirectoryCreated(watchEventPath, watchEventIsDir))
							{
								return false;
							}
							break;
						}
					// File or directory was removed or moved from watch directory.
					case FILE_ACTION_REMOVED:
						{
							auto const watchEventIsDir = (watchEventIterator->FileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
							auto const watchEventPath = Paths::Combine(directoryName, WideString(watchEventIterator->FileName, watchEventIterator->FileNameLength / sizeof(watchEventIterator->FileName[0])));
							if (!directoryWatcherDelegate.OnFileOrDirectoryRemoved(watchEventPath, watchEventIsDir))
							{
								return false;
							}
							break;
						}
					// File or directory was modified.
					case FILE_ACTION_MODIFIED:
						{
							auto const watchEventIsDir = (watchEventIterator->FileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
							auto const watchEventPath = Paths::Combine(directoryName, WideString(watchEventIterator->FileName, watchEventIterator->FileNameLength / sizeof(watchEventIterator->FileName[0])));
							if (!directoryWatcherDelegate.OnFileOrDirectoryModified(watchEventPath, watchEventIsDir))
							{
								return false;
							}
							break;
						}
					// File or directory was renamed.
					case FILE_ACTION_RENAMED_OLD_NAME:
						{
							auto const newNameEventIterator = ++ReadDirectoryChangesEventIterator(watchEventIterator);
							if (newNameEventIterator.IsEnd() && newNameEventIterator->Action == FILE_ACTION_RENAMED_NEW_NAME)
							{
								auto const watchEventIsDir = (watchEventIterator->FileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
								auto const watchRenamedOldNameEventPath = Paths::Combine(directoryName, WideString(watchEventIterator->FileName, watchEventIterator->FileNameLength / sizeof(watchEventIterator->FileName[0])));
								auto const watchRenamedNewNameEventPath = Paths::Combine(directoryName, WideString(newNameEventIterator->FileName, newNameEventIterator->FileNameLength / sizeof(watchEventIterator->FileName[0])));
								if (!directoryWatcherDelegate.OnFileOrDirectoryMoved(watchRenamedOldNameEventPath, watchRenamedNewNameEventPath, watchEventIsDir))
								{
									return false;
								}
							}
							else
							{
								return false;
							}
							break;
						}
					default:
						break;
				}
			}
		}

		CloseHandle(directoryHandle);
		return true;
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_API_MICROSOFT
