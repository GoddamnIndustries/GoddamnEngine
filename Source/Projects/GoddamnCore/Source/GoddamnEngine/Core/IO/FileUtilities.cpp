// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/IO/FileUtilities.cpp
 * File contains file and directory utilities generic interface.
 */
#include <GoddamnEngine/Core/IO/FileUtilities.h>
#include <GoddamnEngine/Core/IO/FileStream.h>
#include <GoddamnEngine/Core/IO/Directory.h>
#include <GoddamnEngine/Core/IO/Paths.h>
#include <GoddamnEngine/Core/Misc/Misc.h>

GD_NAMESPACE_BEGIN

	// ReSharper disable CppRedundantQualifier

	// ------------------------------------------------------------------------------------------
	// File utilities.
	// ------------------------------------------------------------------------------------------

	/*
	 * Returns true if the specified file exists and is not directory.
	 * @param filename Path to the file.
	 */
	GDAPI bool FileUtilitiesGeneric::FileExists(WideString const& filename)
	{
		FileInputStream const fileStream(filename.CStr());
		return fileStream.IsValid();
	}

	/*!
	 * Returns size of the file in bytes, or -1 if it does not exist.
	 * @param filename Path to the file.
	 */
	GDAPI UInt64 FileUtilitiesGeneric::FileSize(WideString const& filename)
	{
		FileInputStream const fileStream(filename.CStr());
		return fileStream.IsValid() ? fileStream.GetLength() : UInt64Max;
	}

	/*!
	 * Creates an empty file with the specified file name.
	 * If file already exists, nothing is done.
	 *
	 * @param filename Path to the file.
	 * @returns True if file exists or was successfully created.
	 */
	GDAPI bool FileUtilitiesGeneric::FileCreateEmpty(WideString const& filename)
	{
		FileOutputStream const fileStream(filename.CStr(), true);
		return fileStream.IsValid();
	}

	/*!
	 * Removes the existing file.
	 *
	 * @param filename Path to the file.
	 * @returns True if file was successfully deleted.
	 */
	GDAPI bool FileUtilitiesGeneric::FileRemove(WideString const& filename)
	{
		auto const filenameSystem = Paths::Platformize(filename);
		if (CStdio::Remove(filenameSystem.CStr()) != 0)
		{
			PlatformMisc::Sleep(0);
			return CStdio::Remove(filenameSystem.CStr()) == 0;
		}
		return true;
	}

	/*!
	 * Moves file from source path to destination.
	 *
	 * @param sourceFilename Path to the file.
	 * @param destFilename Destination file path.
	 * @param doOverwrite Do overwrite destination file if it exists.
	 * @returns True if file was successfully moved.
	 */
	GDAPI bool FileUtilitiesGeneric::FileMove(WideString const& sourceFilename, WideString const& destFilename, bool const doOverwrite /*= false*/)
	{
		if (FileUtilities::FileExists(destFilename) && !doOverwrite)
		{
			auto const sourceFilenameSystem = Paths::Platformize(sourceFilename);
			auto const destFilenameSystem = Paths::Platformize(destFilename);
			if (CStdio::Rename(sourceFilenameSystem.CStr(), destFilenameSystem.CStr()) != 0)
			{
				PlatformMisc::Sleep(0);
				return CStdio::Rename(sourceFilenameSystem.CStr(), destFilenameSystem.CStr()) == 0;
			}
			return true;
		}
		return false;
	}

	/*!
	 * Copies file from source path to destination.
	 *
	 * @param sourceFilename Path to the file.
	 * @param destFilename Destination file path.
	 * @param doOverwrite Do overwrite destination file if it exists.
	 * @returns True if file was successfully moved.
	 */
	GDAPI bool FileUtilitiesGeneric::FileCopy(WideString const& sourceFilename, WideString const& destFilename, bool const doOverwrite /*= false*/)
	{
		if (FileUtilities::FileExists(sourceFilename) && FileUtilities::FileExists(destFilename) && !doOverwrite)
		{
			FileInputStream sourceFileStream(sourceFilename.CStr());
			FileOutputStream destFileStream(destFilename.CStr());
			if (sourceFileStream.IsValid() && destFileStream.IsValid())
			{
				auto operationSucceeded = true;
				auto const sourceFileSize = sourceFileStream.GetLength();

				// Copying file from source to the destination using 4KB blocks.
				SizeTp const bufferBlockSize = 4 * 1024 * 1024;
				auto const buffer = GD_MALLOC_ARRAY_T(Byte, Min(bufferBlockSize, sourceFileSize));
				for (SizeTp cnt = 0; cnt < sourceFileSize / bufferBlockSize && operationSucceeded; ++cnt)
				{
					operationSucceeded = sourceFileStream.Read(buffer, bufferBlockSize, 1) == 1;
					if (operationSucceeded)
						operationSucceeded = destFileStream.Write(buffer, bufferBlockSize, 1) == 1;
				}
				if (operationSucceeded)
				{
					// Copying what is left in the source file.
					auto const leftoverSize = sourceFileSize % bufferBlockSize;
					if (leftoverSize != 0)
					{
						operationSucceeded = sourceFileStream.Read(buffer, leftoverSize, 1) == 1;
						if (operationSucceeded)
							operationSucceeded = destFileStream.Write(buffer, leftoverSize, 1) == 1;
					}
				}
				GD_FREE(buffer);

				if (!operationSucceeded)
				{
					// If failed, removing destination file as this function was never called. 
					destFileStream.Close();
					FileUtilities::FileRemove(destFilename);
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

	/*
	 * Returns true if the specified file exists and is a directory.
	 * @param directoryName Name to the directory.
	 */
	GDAPI bool FileUtilitiesGeneric::DirectoryExists(WideString const& directoryName)
	{
		Directory directory(directoryName.CStr());
		return directory.IsValid();
	}

	/*!
	 * Creates an empty directory with the specified directory name.
	 * If directory already exists, nothing is done.
	 *
	 * @param directoryName Path to the directory.
	 * @returns True if directory exists or was successfully created.
	 */
	GDAPI bool FileUtilitiesGeneric::DirectoryCreateEmpty(WideString const& directoryName)
	{
		if (!FileUtilities::DirectoryExists(directoryName))
		{
			// Could not find any other cross-platform ways for generating a directory.
			GD_STUBBED(_wsystem);
			WideChar mkdirCommand[2048] = L"mkdir ";
			WideCString::Strcat_s(mkdirCommand, GetLength(mkdirCommand), Paths::Platformize(directoryName).CStr());
			return _wsystem(mkdirCommand) == 0;
		}
		return true;
	}

	/*!
	 * Removes the existing empty directory.
	 *
	 * @param directoryName Path to the directory.
	 * @returns True if directory was successfully deleted.
	 */
	GDAPI bool FileUtilitiesGeneric::DirectoryRemove(WideString const& directoryName)
	{
		if (!FileUtilities::DirectoryExists(directoryName))
		{
			// Could not find any other cross-platform ways for removing a directory.
			GD_STUBBED(_wsystem);
			WideChar rmdirCommand[2048] = L"rmdir ";
			WideCString::Strcat_s(rmdirCommand, GetLength(rmdirCommand), Paths::Platformize(directoryName).CStr());
			if (_wsystem(rmdirCommand) != 0)
			{
				PlatformMisc::Sleep(0);
				return _wsystem(rmdirCommand) == 0;
			}
		}
		return true;
	}

	/*!
	 * Removes the existing directory and everything inside it.
	 *
	 * @param directoryName Path to the directory.
	 * @returns True if directory was successfully deleted.
	 */
	GDAPI bool FileUtilitiesGeneric::DirectoryRemoveRecursive(WideString const& directoryName)
	{
		Directory directory(directoryName.CStr());
		if (directory.IsValid())
		{
			for (auto directoryEntry = directory.ReadEntry(); directoryEntry.EntryName != nullptr; directoryEntry = directory.ReadEntry())
			{
				auto const directoryEntryPath = Paths::Combine(directoryName, directoryEntry.EntryName);
				if (directoryEntry.EntryIsDirectory)
				{
					FileUtilities::DirectoryRemoveRecursive(directoryEntryPath);
				}
				else
				{
					FileUtilities::FileRemove(directoryEntryPath);
				}
			}
			directory.Close();

			// If any internal removal has failed, attempt of removal outer directory will fail and false would be returned.
			return FileUtilities::DirectoryRemove(directoryName);
		}
		return false;
	}

	// ReSharper restore CppRedundantQualifier

GD_NAMESPACE_END
