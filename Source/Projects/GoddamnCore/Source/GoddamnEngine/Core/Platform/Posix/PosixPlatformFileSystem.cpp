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
#include <stdio.h>

#include <GoddamnEngine/Core/Misc/StringConv.h>
#include "GoddamnEngine/Core/IO/Paths.h"

GD_NAMESPACE_BEGIN

    // **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
    //! File input stream on Posix platforms.
    // **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
    class GD_PLATFORM_KERNEL PosixFileInputStream final : public InputStream
    {
    private:
        Int32 m_FileHandle;
        
    public:
        GDINL explicit PosixFileInputStream(Int32 const fileHandle)
            : m_FileHandle(fileHandle)
        {
        }
        
        GDINT virtual ~PosixFileInputStream()
        {
            Close();
        }
        
    public:
        GDINT virtual bool IsValid() const override final
        {
            return m_FileHandle != -1;
        }
        
        GDINT virtual SizeTp GetPosition() const override final
        {
            GD_VERIFY(IsValid());
            return const_cast<PosixFileInputStream*>(this)->Seek(0, SeekOrigin::Current);
        }
        
        GDINT virtual void Close() override final
        {
            close(m_FileHandle);
            m_FileHandle = -1;
        }
        
        GDINT virtual SizeTp Seek(PtrDiffTp const offset, SeekOrigin const origin) override final
        {
            GD_VERIFY(IsValid());
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
            return lseek(m_FileHandle, offset, originSystem);
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
                auto const arrayBlock = static_cast<Byte*>(array) + i * size;
                if (read(m_FileHandle, arrayBlock, size) != size)
                {
                    return i;
                }
            }
            return count;
        }
    };    // class PosixFileInputStream

    // **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
    //! File output stream on Posix platforms.
    // **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
    class GD_PLATFORM_KERNEL PosixFileOutputStream : public OutputStream
    {
    private:
        Int32 m_FileHandle;
        
    public:
        GDINL explicit PosixFileOutputStream(Int32 const fileHandle)
            : m_FileHandle(fileHandle)
        {
        }
        
        GDINT virtual ~PosixFileOutputStream()
        {
            Close();
        }
        
    public:
        GDINT virtual bool IsValid() const override final
        {
            return m_FileHandle != -1;
        }
        
        GDINT virtual void Close() override final
        {
            close(m_FileHandle);
            m_FileHandle = -1;
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
                auto const arrayBlock = static_cast<Byte const*>(array) + i * size;
                if (write(m_FileHandle, arrayBlock, size) != size)
                {
                    return i;
                }
            }
            return count;
        }
    };    // class PosixFileOutputStream

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
     * @returns True if file size was succesfully obtained.
     */
    GDINT bool PosixPlatformDiskFileSystem::FileSize(WideString const& filename, UInt64& fileSize) const
    {
        struct stat fileAttributeData = {};
        auto const filenameSystem = StringConv::EncodeUTF8(Paths::Platformize(filename));
        if (stat(filenameSystem.CStr(), &fileAttributeData) == 0)
        {
            if (!S_ISDIR(fileAttributeData.st_mode))
            {
                fileSize = fileAttributeData.st_size;
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
        
    /*!
     * Opens a input stream for the specified file.
     *
     * @param filename Path to the file.
     * @returns Opened valid input stream or null pointer if operation has failed.
     */
    GDINT SharedPtr<InputStream> PosixPlatformDiskFileSystem::FileOpenRead(WideString const& filename) const
    {
        auto const filenameSystem = StringConv::EncodeUTF8(Paths::Platformize(filename));
        auto const fileHandle = open(filenameSystem.CStr(), O_RDONLY);
        if (fileHandle != -1)
        {
            return gd_new PosixFileInputStream(fileHandle);
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
    GDINT SharedPtr<OutputStream> PosixPlatformDiskFileSystem::FileOpenWrite(WideString const& filename, bool const doAppend) const
    {
        auto const filenameSystem = StringConv::EncodeUTF8(Paths::Platformize(filename));
        auto const fileHandle = open(filenameSystem.CStr(), O_CREAT | O_WRONLY | (doAppend ? O_APPEND : O_TRUNC));
        if (fileHandle != -1)
        {
            return gd_new PosixFileOutputStream(fileHandle);
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
        auto const directoryNameSystem = StringConv::EncodeUTF8(Paths::Platformize(directoryName));
        if (mkdir(directoryNameSystem.CStr(), 0755) != 0)
        {
			sleep(0);
			return mkdir(directoryNameSystem.CStr(), 0755) == 0;
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
                    auto const directoryEntryName = Paths::Combine(directoryName, StringConv::DecodeUTF8(directoryEntryHandle->d_name, directoryEntryHandle->d_namlen));
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

