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

#include "GoddamnEngine/Core/IO/Paths.h"

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! File input stream on Microsoft platforms.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL MicrosoftFileInputStream final : public InputStream
	{
	private:
		HANDLE m_FileHandle;

	public:
		GDINL explicit MicrosoftFileInputStream(HANDLE const fileHandle)
			: m_FileHandle(fileHandle)
		{
		}

		GDINT virtual ~MicrosoftFileInputStream()
		{
			Close();
		}

	public:
		GDINT virtual bool IsValid() const override final
		{
			return m_FileHandle != INVALID_HANDLE_VALUE && m_FileHandle != nullptr;
		}

		GDINT virtual SizeTp GetPosition() const override final
		{
			GD_VERIFY(IsValid());
			return const_cast<MicrosoftFileInputStream*>(this)->Seek(0, SeekOrigin::Current);
		}

		GDINT virtual void Close() override final
		{
			CloseHandle(m_FileHandle);
			m_FileHandle = nullptr;
		}

		GDINT virtual SizeTp Seek(PtrDiffTp const offset, SeekOrigin const origin) override final
		{
			GD_VERIFY(IsValid());
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
			LARGE_INTEGER offsetSystem, newFilePointerSystem;
			offsetSystem.QuadPart = offset;
			if (SetFilePointerEx(m_FileHandle, offsetSystem, &newFilePointerSystem, originSystem) == FALSE)
			{
				return SizeTpMax;
			}
			return newFilePointerSystem.QuadPart;
		}

		GDINT virtual Int16 Read() override final
		{
			GD_VERIFY(IsValid());
			Byte byte;
			if (Read(&byte, sizeof(byte), 1) != 1)
			{
				return -1;
			}
			return byte;
		}

		GDINT virtual SizeTp Read(Handle const array, SizeTp const size, SizeTp const count) override final
		{
			GD_VERIFY(IsValid());
			for (SizeTp i = 0; i < count; ++i)
			{
				DWORD numBytesRead = 0;
				auto const arrayBlock = static_cast<Byte*>(array) + i * size;
				auto const arrayBlockSize = static_cast<DWORD>(size);
				if (ReadFile(m_FileHandle, arrayBlock, arrayBlockSize, &numBytesRead, nullptr) == FALSE || numBytesRead != arrayBlockSize)
				{
					return i;
				}
			}
			return count;
		}
	};	// class MicrosoftFileInputStream

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! File output stream on Microsoft platforms.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL MicrosoftFileOutputStream : public OutputStream
	{
	private:
		HANDLE m_FileHandle;

	public:
		GDINL explicit MicrosoftFileOutputStream(HANDLE const fileHandle)
			: m_FileHandle(fileHandle)
		{
		}

		GDINT virtual ~MicrosoftFileOutputStream()
		{
			Close();
		}

	public:
		GDINT virtual bool IsValid() const override final
		{
			return m_FileHandle != INVALID_HANDLE_VALUE && m_FileHandle != nullptr;
		}

		GDINT virtual void Close() override final
		{
			CloseHandle(m_FileHandle);
			m_FileHandle = nullptr;
		}

		GDINT virtual void Flush() override final
		{
		}

		GDINT virtual bool Write(Byte const byte) override final
		{
			return Write(&byte, sizeof(byte), 1) == 1;
		}

		GDINT virtual SizeTp Write(CHandle const array, SizeTp const size, SizeTp const count) override final
		{
			GD_VERIFY(IsValid(), "Writing to invalid stream.");
			for (SizeTp i = 0; i < count; ++i)
			{
				DWORD numBytesWritten = 0;
				auto const arrayBlock = static_cast<Byte const*>(array) + i * size;
				auto const arrayBlockSize = static_cast<DWORD>(size);
				if (WriteFile(m_FileHandle, arrayBlock, arrayBlockSize, &numBytesWritten, nullptr) == FALSE || numBytesWritten != arrayBlockSize)
				{
					return i;
				}
			}
			return count;
		}
	};	// class MicrosoftFileOutputStream

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
		GDINT virtual SharedPtr<InputStream> FileOpenRead(WideString const& filename) const override final;
		GDINT virtual SharedPtr<OutputStream> FileOpenWrite(WideString const& filename, bool const doAppend) const override final;

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
	 * @returns True if file size was succesfully obtained.
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
		auto const filenameSystem = Paths::Platformize(filename);
		auto const emptyOrExistingFile = CreateFile2(filenameSystem.CStr(), GENERIC_WRITE, 0, CREATE_NEW | OPEN_EXISTING, nullptr);
		if (emptyOrExistingFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(emptyOrExistingFile);
			return true;
		}
		return false;
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
		if (doOverwrite || !FileExists(dstFilename))
		{
			auto const srcFilenameSystem = Paths::Platformize(srcFilename);
			auto const dstFilenameSystem = Paths::Platformize(dstFilename);
			if (MoveFileExW(srcFilenameSystem.CStr(), dstFilenameSystem.CStr(), 0) == FALSE)
			{
				Sleep(0);
				return MoveFileExW(srcFilenameSystem.CStr(), dstFilenameSystem.CStr(), 0) == TRUE;
			}
			return true;
		}
		return false;
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
		if (doOverwrite || !FileExists(dstFilename))
		{
			auto const srcFilenameSystem = Paths::Platformize(srcFilename);
			auto const dstFilenameSystem = Paths::Platformize(dstFilename);
			if (CopyFileW(srcFilenameSystem.CStr(), dstFilenameSystem.CStr(), !doOverwrite) == FALSE)
			{
				Sleep(0);
				return CopyFileW(srcFilenameSystem.CStr(), dstFilenameSystem.CStr(), !doOverwrite) == TRUE;
			}
		}
		return true;
	}
		
	/*!
	 * Opens a input stream for the specified file.
	 * 
	 * @param filename Path to the file.
	 * @returns Opened valid input stream or null pointer if operation has failed.
	 */
	GDINT SharedPtr<InputStream> MicrosoftPlatformDiskFileSystem::FileOpenRead(WideString const& filename) const
	{
		auto const filenameSystem = Paths::Platformize(filename);
		auto const fileHandle = CreateFile2(filenameSystem.CStr(), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr);
		if (fileHandle != INVALID_HANDLE_VALUE)
		{
			return gd_new MicrosoftFileInputStream(fileHandle);
		}
		return nullptr;
	}

	/*!
	 * Opens a output stream for the specified file.
	 * 
	 * @param filename Path to the file.
	 * @param doAppend If true new data would be written to the end of file.
	 * 
	 * @returns Opened valid output stream or null pointer if operation has failed.
	 */
	GDINT SharedPtr<OutputStream> MicrosoftPlatformDiskFileSystem::FileOpenWrite(WideString const& filename, bool const doAppend) const
	{
		auto const filenameSystem = Paths::Platformize(filename);
		auto const fileHandle = CreateFile2(filenameSystem.CStr(), GENERIC_WRITE, FILE_SHARE_READ, doAppend ? OPEN_ALWAYS : CREATE_ALWAYS, nullptr);
		if (fileHandle != INVALID_HANDLE_VALUE)
		{
			return gd_new MicrosoftFileOutputStream(fileHandle);
		}
		return nullptr;
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
		if (CreateDirectoryW(directoryNameSystem.CStr(), nullptr) == FALSE && GetLastError() != ERROR_ALREADY_EXISTS)
		{
			Sleep(0);
			return CreateDirectoryW(directoryNameSystem.CStr(), nullptr) == TRUE;
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
					directoryIterateDelegate.OnVisitDirectoryEntry(directoryEntryName.CStr(), directoryEntryIsDirectory);
				}
			} while (FindNextFileW(directoryHandle, &directoryEntry));
			return FindClose(directoryHandle) == TRUE;
		}
		return false;
	}

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Disk file system watcher.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL MicrosoftPlatformDiskFileSystemWatcher : public IPlatformDiskFileSystemWatcher
	{
	private:

		/*!
		 * Adds a watch on the specified directory.
		 *
		 * @param directoryName Path to the directory.
		 * @param directoryWatcherDelegate Wacth event delegate.
		 * 
		 * @returns True if directory exists or new watch was succesfully added.
		 */
		GDINT virtual bool AddWatch(WideString const& directoryName, IFileSystemWatcherDelegate& directoryWatcherDelegate) override final
		{
			return AddWatch(directoryName, directoryWatcherDelegate, false);
		}

		/*!
		 * Adds a watch on the specified directory and all it's subdirectories.
		 *
		 * @param directoryName Path to the directory.
		 * @param directoryWatcherDelegate Wacth event delegate.
		 * 
		 * @returns True if directory exists or new watch was succesfully added.
		 */
		GDINT virtual bool AddWatchRecursive(WideString const& directoryName, IFileSystemWatcherDelegate& directoryWatcherDelegate) override final
		{
			return AddWatch(directoryName, directoryWatcherDelegate, true);
		}

	private:
		GDINT bool AddWatch(WideString const& directoryName, IFileSystemWatcherDelegate& directoryWatcherDelegate, bool const recursive);
	};	// class MicrosoftPlatformDiskFileSystemWatcher

	GD_IMPLEMENT_SINGLETON(IPlatformDiskFileSystemWatcher, MicrosoftPlatformDiskFileSystemWatcher);

	/*!
	 * Adds a watch on the specified directory and all it's subdirectories.
	 *
	 * @param directoryName Path to the directory.
	 * @param directoryWatcherDelegate Wacth event delegate.
	 * @param recursive 
	 * 
	 * @returns True if directory exists or new watch was succesfully added.
	 */
	GDINT bool MicrosoftPlatformDiskFileSystemWatcher::AddWatch(WideString const& directoryName, IFileSystemWatcherDelegate& directoryWatcherDelegate, bool const recursive)
	{
		auto const directoryNameSystem = Paths::Platformize(directoryName);
		auto const directoryHandle = CreateFileW(directoryNameSystem.CStr(), FILE_LIST_DIRECTORY
			, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
		if (directoryHandle != INVALID_HANDLE_VALUE)
		{
			DWORD directoryChangesLength;
			Byte directoryChangesBuffer[1024 * 128];
			while (ReadDirectoryChangesW(directoryHandle
				, directoryChangesBuffer, sizeof(directoryChangesBuffer), recursive
				, FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_FILE_NAME
				, &directoryChangesLength, nullptr, nullptr))
			{
				DWORD directoryChangeOffset = 0;
				PFILE_NOTIFY_INFORMATION directoryChange;
				do
				{
					directoryChange = reinterpret_cast<PFILE_NOTIFY_INFORMATION>(directoryChangesBuffer + directoryChangeOffset);

					WideString const directoryChangePath(directoryChange->FileName, directoryChange->FileNameLength);
					switch (directoryChange->Action)
					{
						case FILE_ACTION_ADDED:
							directoryWatcherDelegate.OnFileOrDirectoryAdded(directoryChangePath);
							break;
						case FILE_ACTION_REMOVED:
							directoryWatcherDelegate.OnFileOrDirectoryRemoved(directoryChangePath);
							break;
						case FILE_ACTION_MODIFIED:
							directoryWatcherDelegate.OnFileOrDirectoryModified(directoryChangePath);
							break;
						case FILE_ACTION_RENAMED_OLD_NAME:
							break;
					}

					directoryChangeOffset += directoryChange->NextEntryOffset;
				} while (directoryChange->NextEntryOffset != 0);
			}
			CloseHandle(directoryHandle);
			return true;
		}
		return false;
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_API_MICROSOFT
