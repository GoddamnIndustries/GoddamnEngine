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
#if GD_PLATFORM_API_POSIX

#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>

#include "GoddamnEngine/Core/IO/Paths.h"

GD_NAMESPACE_BEGIN

    // **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
    //! Disk file system on Posix platforms.
    // **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL PosixPlatformDiskFileSystem final : public IPlatformDiskFileSystem
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
	};	// class PosixPlatformDiskFileSystem

	GD_IMPLEMENT_SINGLETON(IPlatformDiskFileSystem, PosixPlatformDiskFileSystem)
        
    // ------------------------------------------------------------------------------------------
    // File utilities.
    // ------------------------------------------------------------------------------------------
        
    /*
     * Returns true if the specified file exists and is not directory.
     * @param filename Path to the file.
     */
    GDINT bool PosixPlatformDiskFileSystem::FileExists(WideString const& filename) const
    {
        struct stat fileAttributeData = {};
        auto const filenameSystem = StringConv::EncodeUTF8(Paths::Platformize(filename));
        if (stat(filenameSystem.CStr(), &fileAttributeData) == 0)
        {
            return S_ISREG(fileAttributeData.st_mode);
        }
        return false;
    }
        
    /*!
     * Gets size of the file in bytes, or -1 if it does not exist.
     *
     * @param filename Path to the file.
     * @param fileSize Output for file size.
     *
     * @returns True if file size was successfully obtained.
     */
    GDINT bool PosixPlatformDiskFileSystem::FileSize(WideString const& filename, UInt64& fileSize) const
    {
        struct stat fileAttributeData = {};
        auto const filenameSystem = StringConv::EncodeUTF8(Paths::Platformize(filename));
        if (stat(filenameSystem.CStr(), &fileAttributeData) == 0)
        {
            if (!S_ISDIR(fileAttributeData.st_mode))
            {
                fileSize = static_cast<UInt64>(fileAttributeData.st_size);
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
    GDINT bool PosixPlatformDiskFileSystem::FileCreateEmpty(WideString const& filename)
    {
        auto const filenameSystem = StringConv::EncodeUTF8(Paths::Platformize(filename));
        auto const emptyOrExistingFile = open(filenameSystem.CStr(), O_CREAT);
        if (emptyOrExistingFile != -1)
        {
            close(emptyOrExistingFile);
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
    GDINT bool PosixPlatformDiskFileSystem::FileRemove(WideString const& filename)
    {
        auto const filenameSystem = StringConv::EncodeUTF8(Paths::Platformize(filename));
        if (unlink(filenameSystem.CStr()) != 0)
        {
            sleep(0);
            return unlink(filenameSystem.CStr()) == 0;
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
    GDINT bool PosixPlatformDiskFileSystem::FileMove(WideString const& srcFilename, WideString const& dstFilename, bool const doOverwrite)
    {
	    if (DirectoryExists(srcFilename))
	    {
		    // Specified path is a path to file, this is an error.
		    return false;
	    }
        if (doOverwrite || !FileExists(dstFilename))
        {
            auto const srcFilenameSystem = StringConv::EncodeUTF8(Paths::Platformize(srcFilename));
            auto const dstFilenameSystem = StringConv::EncodeUTF8(Paths::Platformize(dstFilename));
            if (rename(srcFilenameSystem.CStr(), dstFilenameSystem.CStr()) != 0)
            {
                sleep(0);
                return rename(srcFilenameSystem.CStr(), dstFilenameSystem.CStr()) == 0;
            }
            return true;
        }
        return false;
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
    GDINT bool PosixPlatformDiskFileSystem::FileOpenRead(WideString const& filename, Handle& fileHandle) const
    {
	    if (DirectoryExists(filename))
	    {
		    // Specified path is a path to file, this is an error.
		    return false;
	    }
        auto const filenameSystem = StringConv::EncodeUTF8(Paths::Platformize(filename));
        auto const fileHandleSystem = open(filenameSystem.CStr(), O_RDONLY);
        if (fileHandleSystem != -1)
        {
            fileHandle = reinterpret_cast<Handle>(fileHandleSystem);
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
    GDINT bool PosixPlatformDiskFileSystem::FileOpenWrite(WideString const& filename, Handle& fileHandle, bool const doAppend)
    {
	    if (DirectoryExists(filename))
	    {
		    // Specified path is a path to file, this is an error.
		    return false;
	    }
        auto const filenameSystem = StringConv::EncodeUTF8(Paths::Platformize(filename));
        auto const fileHandleSystem = open(filenameSystem.CStr(), O_CREAT | O_WRONLY | (doAppend ? O_APPEND : O_TRUNC));
        if (fileHandleSystem != -1)
        {
            fileHandle = reinterpret_cast<Handle>(fileHandleSystem);
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
    GDINT bool PosixPlatformDiskFileSystem::FileClose(Handle const fileHandle) const
    {
        GD_DEBUG_VERIFY(fileHandle != nullptr);
        auto const fileHandleSystem = static_cast<Int32>(reinterpret_cast<IntPtr>(fileHandle));
        return close(fileHandleSystem) == 0;
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
    GDINT bool PosixPlatformDiskFileSystem::FileSeek(Handle const fileHandle, Int64 const offset, SeekOrigin const origin, UInt64* const newPosition) const
    {
        GD_DEBUG_VERIFY(fileHandle != nullptr);

        Int32 originSystem = 0;
        switch (origin)
        {
            case SeekOrigin::Beginning:
                originSystem = SEEK_SET;
                break;
            case SeekOrigin::Current:
                originSystem = SEEK_CUR;
                break;
            case SeekOrigin::End:
                originSystem = SEEK_END;
                break;
        }

        auto const fileHandleSystem = static_cast<Int32>(reinterpret_cast<IntPtr>(fileHandle));
        if (newPosition != nullptr)
        {
            auto const newFilePointerSystem = lseek64(fileHandleSystem, offset, originSystem);
            if (newFilePointerSystem != -1)
            {
                *newPosition = static_cast<UInt64>(newFilePointerSystem);
                return true;
            }
        }
        else
        {
            if (lseek64(fileHandleSystem, offset, originSystem) != -1)
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
    GDINT bool PosixPlatformDiskFileSystem::FileRead(Handle const fileHandle, Handle const readBuffer, UInt32 const readBufferSizeBytes, UInt32* const numBytesRead) const
    {
        GD_DEBUG_VERIFY(fileHandle != nullptr);
        auto const fileHandleSystem = static_cast<Int32>(reinterpret_cast<IntPtr>(fileHandle));
        if (numBytesRead != nullptr)
        {
            auto const numBytesReadSystem = read(fileHandleSystem, readBuffer, readBufferSizeBytes);
            if (numBytesReadSystem != -1)
            {
                *numBytesRead = static_cast<UInt32>(numBytesReadSystem);
                return true;
            }
        }
        else
        {
            if (read(fileHandleSystem, readBuffer, readBufferSizeBytes) != -1)
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
    GDINT bool PosixPlatformDiskFileSystem::FileWrite(Handle const fileHandle, CHandle const writeBuffer, UInt32 const writeBufferSizeBytes, UInt32* const numBytesWritten)
    {
        GD_DEBUG_VERIFY(fileHandle != nullptr);
        auto const fileHandleSystem = static_cast<Int32>(reinterpret_cast<IntPtr>(fileHandle));
        if (numBytesWritten != nullptr)
        {
            auto const numBytesWrittenSystem = write(fileHandleSystem, writeBuffer, writeBufferSizeBytes);
            if (numBytesWrittenSystem != -1)
            {
                *numBytesWritten = static_cast<UInt32>(numBytesWrittenSystem);
                return true;
            }
        }
        else
        {
            if (write(fileHandleSystem, writeBuffer, writeBufferSizeBytes) != -1)
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
    GDINT bool PosixPlatformDiskFileSystem::DirectoryExists(WideString const& directoryName) const
    {
        struct stat directoryAttributes = {};
        auto const directoryNameSystem = StringConv::EncodeUTF8(Paths::Platformize(directoryName));
        if (stat(directoryNameSystem.CStr(), &directoryAttributes) == 0)
        {
            return S_ISDIR(directoryAttributes.st_mode);
        }
        return false;
    }
        
    /*!
     * Creates an empty directory with the specified directory name.
     * If directory already exists, nothing is done.
     *
     * @param directoryName Path to the directory.
     * @returns True if directory exists or was successfully created.
     */
    GDINT bool PosixPlatformDiskFileSystem::DirectoryCreateEmpty(WideString const& directoryName) const
    {
	    if (!DirectoryExists(directoryName))
	    {
		    if (FileExists(directoryName))
		    {
			    // Specified path is a path to file, this is an error.
			    return false;
		    }
	        auto const directoryNameSystem = StringConv::EncodeUTF8(Paths::Platformize(directoryName));
	        if (mkdir(directoryNameSystem.CStr(), 0755) != 0)
	        {
				sleep(0);
				return mkdir(directoryNameSystem.CStr(), 0755) == 0;
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
    GDINT bool PosixPlatformDiskFileSystem::DirectoryRemove(WideString const& directoryName)
    {
        auto const directoryNameSystem = StringConv::EncodeUTF8(Paths::Platformize(directoryName));
        if (rmdir(directoryNameSystem.CStr()) != 0)
        {
            sleep(0);
            return rmdir(directoryNameSystem.CStr()) == 0;
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
    GDINT bool PosixPlatformDiskFileSystem::DirectoryIterate(WideString const& directoryName, IFileSystemDirectoryIterateDelegate& directoryIterateDelegate) const
    {
        auto const directoryNameSystem = StringConv::EncodeUTF8(Paths::Platformize(directoryName));
        auto const directoryHandle = opendir(directoryNameSystem.CStr());
        if (directoryHandle != nullptr)
        {
            while (true)
            {
                auto const directoryEntryHandle = readdir(directoryHandle);
                if (directoryEntryHandle == nullptr)
                {
                    break;
                }
                    
                if (CString::Strcmp(directoryEntryHandle->d_name, ".") != 0 && CString::Strcmp(directoryEntryHandle->d_name, "..") != 0
#if GD_PLATFORM_API_COCOA
                    && CString::Strcmp(directoryEntryHandle->d_name, ".DS_Store") != 0
#endif  // if GD_PLATFORM_API_COCOA
                    )
                {
                    auto const directoryEntryName = Paths::Combine(directoryName, StringConv::DecodeUTF8(directoryEntryHandle->d_name));
                    directoryIterateDelegate.OnVisitDirectoryEntry(directoryEntryName, directoryEntryHandle->d_type == DT_DIR);
                }
            }
            closedir(directoryHandle);
            return true;
        }
        return false;
    }

GD_NAMESPACE_END

#endif    // if GD_PLATFORM_API_MICROSOFT

