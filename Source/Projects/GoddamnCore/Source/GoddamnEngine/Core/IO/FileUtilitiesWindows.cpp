// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/IO/FileUtilitiesWindows.cpp
 * File contains file and directory utilities interface.
 */
#include <GoddamnEngine/Core/IO/FileUtilities.h>
#include <GoddamnEngine/Core/IO/Paths.h>
#if GD_PLATFORM_WINDOWS 

#include <Windows.h>

GD_NAMESPACE_BEGIN

	// ------------------------------------------------------------------------------------------
	// File utilities.
	// ------------------------------------------------------------------------------------------

	/*
	 * Returns true if the specified file exists and is not directory.
	 * @param filename Path to the file.
	 */
	GDAPI bool FileUtilitiesWindows::FileExists(WideString const& filename)
	{
		auto const fileAttributes = GetFileAttributesW(Paths::Platformize(filename).CStr());
		return fileAttributes != INVALID_FILE_ATTRIBUTES && (fileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
	}

	/*!
	 * Returns size of the file in bytes, or -1 if it does not exist.
	 * @param filename Path to the file.
	 */
	GDAPI UInt64 FileUtilitiesWindows::FileSize(WideString const& filename)
	{
		WIN32_FILE_ATTRIBUTE_DATA fileAttributeData = {};
		if (GetFileAttributesExW(Paths::Platformize(filename).CStr(), GetFileExInfoStandard, &fileAttributeData) == TRUE)
		{
			if ((fileAttributeData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				LARGE_INTEGER fileSize;
				fileSize.HighPart = fileAttributeData.nFileSizeHigh;
				fileSize.LowPart = fileAttributeData.nFileSizeLow;
				return static_cast<UInt64>(fileSize.QuadPart);
			}
		}
		return UInt64Max;
	}

	/*!
	 * Creates an empty file with the specified file name.
	 * If file already exists, nothing is done.
	 *
	 * @param filename Path to the file.
	 * @returns True if file exists or was successfully created.
	 */
	GDAPI bool FileUtilitiesWindows::FileCreateEmpty(WideString const& filename)
	{
		auto const filenameSystem = Paths::Platformize(filename);
		auto const emptyOrExistingFile = CreateFileW(filenameSystem.CStr(), GENERIC_WRITE, 0, nullptr
			, CREATE_NEW | OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
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
	GDAPI bool FileUtilitiesWindows::FileRemove(WideString const& filename)
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
	 * @param sourceFilename Path to the file.
	 * @param destFilename Destination file path.
	 * @param doOverwrite Do overwrite destination file if it exists.
	 * @returns True if file was successfully moved.
	 */
	GDAPI bool FileUtilitiesWindows::FileMove(WideString const& sourceFilename, WideString const& destFilename, bool const doOverwrite /*= false*/)
	{
		if (FileExists(destFilename) && !doOverwrite)
		{
			auto const sourceFilenameSystem = Paths::Platformize(sourceFilename);
			auto const destFilenameSystem = Paths::Platformize(destFilename);
			if (MoveFileW(sourceFilenameSystem.CStr(), destFilenameSystem.CStr()) == FALSE)
			{
				Sleep(0);
				return MoveFileW(sourceFilenameSystem.CStr(), destFilenameSystem.CStr()) == TRUE;
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
	GDAPI bool FileUtilitiesWindows::FileCopy(WideString const& sourceFilename, WideString const& destFilename, bool const doOverwrite /*= false*/)
	{
		auto const sourceFilenameSystem = Paths::Platformize(sourceFilename);
		auto const destFilenameSystem = Paths::Platformize(destFilename);
		if (CopyFileW(sourceFilenameSystem.CStr(), destFilenameSystem.CStr(), !doOverwrite) == FALSE)
		{
			Sleep(0);
			return CopyFileW(sourceFilenameSystem.CStr(), destFilenameSystem.CStr(), !doOverwrite) == TRUE;
		}
		return true;
	}

	// ------------------------------------------------------------------------------------------
	// Directory utilities.
	// ------------------------------------------------------------------------------------------

	/*
	 * Returns true if the specified file exists and is a directory.
	 * @param directoryName Name to the directory.
	 */
	GDAPI bool FileUtilitiesWindows::DirectoryExists(WideString const& directoryName)
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
	GDAPI bool FileUtilitiesWindows::DirectoryCreateEmpty(WideString const& directoryName)
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
	GDAPI bool FileUtilitiesWindows::DirectoryRemove(WideString const& directoryName)
	{
		auto const directoryNameSystem = Paths::Platformize(directoryName);
		if (RemoveDirectoryW(directoryNameSystem.CStr()) == FALSE)
		{
			Sleep(0);
			return RemoveDirectoryW(directoryNameSystem.CStr()) == TRUE;
		}
		return false;
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_WINDOWS
