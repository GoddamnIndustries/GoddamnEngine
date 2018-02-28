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

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! File input stream.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_WRAPPER FileInputStream final : public IInputStream
	{
	private:
		IPlatformDiskFileSystem const& m_FileSystem;
		Handle m_FileHandle;
	public:
		GDINL FileInputStream(IPlatformDiskFileSystem const& fileSystem, Handle const fileHandle)
			: m_FileSystem(fileSystem), m_FileHandle(fileHandle)
		{
		}
		GDINT virtual ~FileInputStream()
		{
			Close(nullptr);
		}
	private:
		GDINT virtual void Close(bool* const resultPtr) override final
		{
			auto const result = m_FileSystem.FileClose(m_FileHandle);
			if (resultPtr != nullptr)
			{
				*resultPtr = result;
			}
			else if (!result)
			{
				GD_VERIFY_FALSE("Unhandled IO error: failed to close a file.");
			}
			else
			{
				m_FileHandle = nullptr;
			}
		}
		GDINT virtual UInt32 Read(Handle const readBuffer, UInt32 const readBufferSizeBytes, bool* const resultPtr) override final
		{
			UInt32 numBytesRead = 0;
			auto const result = m_FileSystem.FileRead(m_FileHandle, readBuffer, readBufferSizeBytes, &numBytesRead);
			if (resultPtr != nullptr)
			{
				*resultPtr = result;
			}
			else if (!result)
			{
				GD_VERIFY_FALSE("Unhandled IO error: reading from file failed.");
			}
			return numBytesRead;
		}
		GDINT virtual UInt64 Seek(Int64 const offset, SeekOrigin const origin, bool* const resultPtr) override final
		{
			UInt64 newPosition = 0;
			auto const result = m_FileSystem.FileSeek(m_FileHandle, offset, origin, &newPosition);
			if (resultPtr != nullptr)
			{
				*resultPtr = result;
			}
			else if (!result)
			{
				GD_VERIFY_FALSE("Unhandled IO error: repositioning file failed.");
			}
			return newPosition;
		}
	};	// class FileInputStream

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! File output stream.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_WRAPPER FileOutputStream final : public IOutputStream
	{
	private:
		IPlatformDiskFileSystem& m_FileSystem;
		Handle m_FileHandle;

	public:
		GDINL FileOutputStream(IPlatformDiskFileSystem& fileSystem, Handle const fileHandle)
			: m_FileSystem(fileSystem), m_FileHandle(fileHandle)
		{
		}
		GDINT virtual ~FileOutputStream()
		{
			Close(nullptr);
		}
	private:
		GDINT virtual void Close(bool* const resultPtr) override final
		{
			auto const result = m_FileSystem.FileClose(m_FileHandle);
			if (resultPtr != nullptr)
			{
				*resultPtr = result;
			}
			else if (!result)
			{
				GD_VERIFY_FALSE("Unhandled IO error: failed to close a file.");
			}
			else
			{
				m_FileHandle = nullptr;
			}
		}
		GDINT virtual UInt64 Seek(Int64 const offset, SeekOrigin const origin, bool* const resultPtr) override final
		{
			UInt64 newPosition = 0;
			auto const result = m_FileSystem.FileSeek(m_FileHandle, offset, origin, &newPosition);
			if (resultPtr != nullptr)
			{
				*resultPtr = result;
			}
			else if (!result)
			{
				GD_VERIFY_FALSE("Unhandled IO error: repositioning file failed.");
			}
			return newPosition;
		}
		GDINT virtual UInt32 Write(CHandle const writeBuffer, UInt32 const writeBufferSizeBytes, bool* const resultPtr) override final
		{
			UInt32 numBytesWritten = 0;
			auto const result = m_FileSystem.FileWrite(m_FileHandle, writeBuffer, writeBufferSizeBytes, &numBytesWritten);
			if (resultPtr != nullptr)
			{
				*resultPtr = result;
			}
			else if (!result)
			{
				GD_VERIFY_FALSE("Unhandled IO error: writing to file failed.");
			}
			return numBytesWritten;
		}
	};	// class FileOutputStream

    // ------------------------------------------------------------------------------------------
    // File utilities.
    // ------------------------------------------------------------------------------------------

	/*!
	 * Moves file from source path to destination.
	 *
	 * @param srcFilename Path to the file.
	 * @param dstFilename Destination file path.
	 * @param doOverwrite Do overwrite destination file if it exists.
	 * 
	 * @returns True if file was successfully moved.
	 */
	GDINT bool IFileSystem::FileMove(WideString const& srcFilename, WideString const& dstFilename, bool const doOverwrite)
	{
		if (FileCopy(srcFilename, dstFilename, doOverwrite))
		{
			if (!FileRemove(srcFilename))
			{
				// If failed, removing destination file as this function was never called.
				FileRemove(dstFilename);
				return false;
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
    GDINT bool IFileSystem::FileCopy(WideString const& srcFilename, WideString const& dstFilename, bool const doOverwrite /*= false*/)
    {
        if (FileExists(srcFilename) && (doOverwrite || FileExists(dstFilename)))
        {
            auto srcFileStream = FileStreamOpenRead(srcFilename);
            auto dstFileStream = FileStreamOpenWrite(dstFilename);
            if (srcFileStream != nullptr && dstFileStream != nullptr)
            {
                // Copying file from source to the destination using 4KB blocks.
                auto const bufferSize = 4 * 1024 * 1024;
                auto const buffer = GD_MALLOC_ARRAY_T(Byte, bufferSize);
				
				auto result = true;
				while(result)
				{
					auto const numBytesRead = srcFileStream->Read(buffer, bufferSize, &result);
					if (result)
					{
						if (numBytesRead == 0)
						{
							break;
						}
						auto const numBytesWritten = dstFileStream->Write(buffer, numBytesRead, &result);
						if (result)
						{
							result = numBytesRead == numBytesWritten;
						}
					}
				}

				GD_FREE(buffer);

                if (!result)
                {
                    // If failed, removing destination file as this function was never called.
                    dstFileStream->Close();
                    FileRemove(dstFilename);
                }
                else
                {
                    return true;
                }
            }
        }
        return false;
    }

	/*!
	 * Opens a input stream for the specified file.
	 * 
	 * @param filename Path to the file.
	 * @returns Opened valid input stream or null pointer if operation has failed.
	 */
	GDINT SharedPtr<IInputStream> IPlatformDiskFileSystem::FileStreamOpenRead(WideString const& filename) const
	{
		Handle fileHandle = nullptr;
		if (FileOpenRead(filename, fileHandle))
		{
			gd_new FileInputStream(*this, fileHandle);
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
	GDINT SharedPtr<IOutputStream> IPlatformDiskFileSystem::FileStreamOpenWrite(WideString const& filename, bool const doAppend)
	{
		Handle fileHandle = nullptr;
		if (FileOpenWrite(filename, fileHandle, doAppend))
		{
			gd_new FileOutputStream(*this, fileHandle);
		}
		return nullptr;
	}

    // ------------------------------------------------------------------------------------------
    // Directory utilities.
    // ------------------------------------------------------------------------------------------

	/*!
     * Recursively iterates through all entities of a directory.
     *
     * @param directoryName Path to the directory.
     * @param directoryIterateDelegate Delegate for the directory traversal.
     *
     * @returns True if directory exists or was successfully iterated.
     */
    GDINT bool IFileSystem::DirectoryIterateRecursive(WideString const& directoryName, IFileSystemDirectoryIterateDelegate& directoryIterateDelegate) const
	{
		class DirectoryIterateRecursiveDelegate final : public IFileSystemDirectoryIterateDelegate
		{
		private:
			IFileSystem const& m_FileSystem;
			IFileSystemDirectoryIterateDelegate& m_Delegate;
		public:
			GDINL DirectoryIterateRecursiveDelegate(IFileSystem const& fileSystem, IFileSystemDirectoryIterateDelegate& delegate)
				: m_FileSystem(fileSystem), m_Delegate(delegate) {}
		public:
			GDINT virtual bool OnVisitDirectoryEntry(WideString const& path, bool const isDirectory) override final
			{
				if (isDirectory)
				{
					if (!m_FileSystem.DirectoryIterateRecursive(path, m_Delegate))
					{
						return false;
					}
				}
				return m_Delegate.OnVisitDirectoryEntry(path, isDirectory);
			}
		};	// class DirectoryIterateRecursiveDelegate

		DirectoryIterateRecursiveDelegate directoryIterateRecursiveDelegate(*this, directoryIterateDelegate);
		return DirectoryIterate(directoryName, directoryIterateRecursiveDelegate);
	}

	/*!
	 * Removes the existing directory and everything inside it.
	 *
	 * @param directoryName Path to the directory.
	 * @returns True if directory was successfully deleted.
	 */
	GDINT bool IFileSystem::DirectoryRemoveRecursive(WideString const& directoryName)
	{
		class DirectoryRemoveRecursiveDelegate final : public IFileSystemDirectoryIterateDelegate
		{
		private:
			IFileSystem& m_FileSystem;
		public:
			GDINL explicit DirectoryRemoveRecursiveDelegate(IFileSystem& fileSystem)
				: m_FileSystem(fileSystem) {}
		private:
			GDINT virtual bool OnVisitDirectoryEntry(WideString const& path, bool const isDirectory) override final
			{
				if (isDirectory)
				{
					if (!m_FileSystem.DirectoryRemoveRecursive(path))
					{
						return false;
					}
				}
				else
				{
					if (!m_FileSystem.FileRemove(path))
					{
						return false;
					}
				}
				return true;
			}
		};	// class DirectoryRemoveRecursiveDelegate

		DirectoryRemoveRecursiveDelegate directoryIterateRecursiveDelegate(*this);
		return DirectoryIterate(directoryName, directoryIterateRecursiveDelegate);
	}

GD_NAMESPACE_END
