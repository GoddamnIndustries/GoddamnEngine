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
//#include "GoddamnEngine/Core/IO/Directory.h"
//#include "GoddamnEngine/Core/IO/Paths.h"

GD_NAMESPACE_BEGIN

    // ------------------------------------------------------------------------------------------
    // File utilities.
    // ------------------------------------------------------------------------------------------

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
            auto srcFileStream = FileOpenRead(srcFilename);
            auto dstFileStream = FileOpenWrite(dstFilename);
            if (srcFileStream != nullptr && dstFileStream != nullptr)
            {
                auto operationSucceeded = true;
                auto const sourceFileSize = srcFileStream->GetLength();
                
                // Copying file from source to the destination using 4KB blocks.
                SizeTp const bufferBlockSize = 4 * 1024 * 1024;
                auto const buffer = GD_MALLOC_ARRAY_T(Byte, Min(bufferBlockSize, sourceFileSize));
                for (SizeTp cnt = 0; cnt < sourceFileSize / bufferBlockSize && operationSucceeded; ++cnt)
                {
                    operationSucceeded = srcFileStream->Read(buffer, bufferBlockSize, 1) == 1 && dstFileStream->Write(buffer, bufferBlockSize, 1) == 1;
                }
                if (operationSucceeded)
                {
                    // Copying what is left in the source file.
                    auto const leftoverSize = sourceFileSize % bufferBlockSize;
                    if (leftoverSize != 0)
                    {
                        operationSucceeded = srcFileStream->Read(buffer, leftoverSize, 1) == 1 && dstFileStream->Write(buffer, leftoverSize, 1) == 1;
                    }
                }
                GD_FREE(buffer);
                
                if (!operationSucceeded)
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
		return false;
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
		public:
			IFileSystem* FileSystem;
		private:
			GDINT virtual void OnVisitDirectoryEntry(WideString const& path, bool const isDirectory) override final
			{
				if (isDirectory)
				{
					FileSystem->DirectoryRemoveRecursive(path);
				}
				else
				{
					FileSystem->FileRemove(path);
				}
			}
		};	// class DirectoryRemoveRecursiveDelegate

		//DirectoryRemoveRecursiveDelegate directoryRemoveRecursiveDelegate{this};
		//DirectoryIterateRecursive(directoryName, directoryRemoveRecursiveDelegate);

		/*Directory directory(directoryName.CStr());
		if (directory.IsValid())
		{
			for (auto directoryEntry = directory.ReadEntry(); directoryEntry.EntryName != nullptr; directoryEntry = directory.ReadEntry())
			{
				auto const directoryEntryPath = Paths::Combine(directoryName, directoryEntry.EntryName);
				if (directoryEntry.EntryIsDirectory)
				{
					DirectoryRemoveRecursive(directoryEntryPath);
				}
				else
				{
					FileRemove(directoryEntryPath);
				}
			}
			directory.Close();

			// If any internal removal has failed, attempt of removal outer directory will fail and false would be returned.
			return DirectoryRemove(directoryName);
		}*/
		return false;
	}

GD_NAMESPACE_END
