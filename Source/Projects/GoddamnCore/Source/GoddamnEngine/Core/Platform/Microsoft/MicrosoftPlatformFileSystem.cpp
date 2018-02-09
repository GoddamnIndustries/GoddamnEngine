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
#include <GoddamnEngine/Core/Platform/FileSystem.h>
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

		GDINT virtual SizeTp GetLength() const override final
		{
			GD_VERIFY(IsValid());
			auto const fileSavedPos = GetPosition();
			if (fileSavedPos == SizeTpMax)
			{
				return SizeTpMax;
			}
			auto const mutableThis = const_cast<MicrosoftFileInputStream*>(this);
			mutableThis->Seek(0, SeekOrigin::End);
			auto const fileEndPos = GetPosition();
			mutableThis->Seek(fileSavedPos, SeekOrigin::Beginning);
			return fileEndPos;
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
	class GD_PLATFORM_KERNEL MicrosoftPlatformDiskFileSystem : public IPlatformDiskFileSystem
	{
	private:

		// ------------------------------------------------------------------------------------------
		// File utilities.
		// ------------------------------------------------------------------------------------------

		/*
		 * Returns true if the specified file exists and is not directory.
		 * @param filename Path to the file.
		 */
		GDINT virtual bool FileExists(WideString const& filename) const override final
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
		GDINT virtual bool FileSize(WideString const& filename, UInt64& fileSize) const override final
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
		GDINT virtual bool FileCreateEmpty(WideString const& filename) override final
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
		GDINT virtual bool FileRemove(WideString const& filename) override final
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
		GDINT virtual bool FileMove(WideString const& srcFilename, WideString const& dstFilename, bool const doOverwrite) override final
		{
			if (doOverwrite || !FileExists(dstFilename))
			{
				auto const srcFilenameSystem = Paths::Platformize(srcFilename);
				auto const dstFilenameSystem = Paths::Platformize(dstFilename);
				if (MoveFileW(srcFilenameSystem.CStr(), dstFilenameSystem.CStr()) == FALSE)
				{
					Sleep(0);
					return MoveFileW(srcFilenameSystem.CStr(), dstFilenameSystem.CStr()) == TRUE;
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
		GDINT virtual bool FileCopy(WideString const& srcFilename, WideString const& dstFilename, bool const doOverwrite) override final
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
		GDINT virtual SharedPtr<InputStream> FileOpenRead(WideString const& filename) const override final
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
		GDINT virtual SharedPtr<OutputStream> FileOpenWrite(WideString const& filename, bool const doAppend) const override final
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
		GDINT virtual bool DirectoryExists(WideString const& directoryName) const override final
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
		GDINT virtual bool DirectoryCreateEmpty(WideString const& directoryName) const override final
		{
			auto const directoryNameSystem = Paths::Platformize(directoryName);
			return CreateDirectoryW(directoryNameSystem.CStr(), nullptr) == TRUE || GetLastError() == ERROR_ALREADY_EXISTS; 
		}

		/*!
		 * Removes the existing empty directory.
		 *
		 * @param directoryName Path to the directory.
		 * @returns True if directory was successfully deleted.
		 */
		GDINT virtual bool DirectoryRemove(WideString const& directoryName) override final
		{
			auto const directoryNameSystem = Paths::Platformize(directoryName);
			if (RemoveDirectoryW(directoryNameSystem.CStr()) == FALSE)
			{
				Sleep(0);
				return RemoveDirectoryW(directoryNameSystem.CStr()) == TRUE;
			}
			return false;
		}
	};	// class MicrosoftPlatformDiskFileSystem

	GD_IMPLEMENT_SINGLETON(IPlatformDiskFileSystem, MicrosoftPlatformDiskFileSystem);

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_API_MICROSOFT
