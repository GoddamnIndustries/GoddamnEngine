// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/System/IO/PathsWindows.cpp
 * File contains generic paths helpers interface.
 */
#include <GoddamnEngine/Core/System/IO/Paths.h>
#if GD_PLATFORM_WINDOWS

#include <Windows.h>
#include <ShlObj.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

GD_NAMESPACE_BEGIN

	// ReSharper disable CppRedundantQualifier

	//! @todo Move this out of here.
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#pragma warning(disable: 4047)
HINSTANCE g_GoddamnCoreHinstance = reinterpret_cast<HINSTANCE>(&__ImageBase);
#pragma warning(default: 4047)

	// ------------------------------------------------------------------------------------------
	// Helper functions.
	// ------------------------------------------------------------------------------------------

	/*!
	 * Normalizes path string - converts all backslashes to slashes, replaces '~' chars and etc.
	 * @param filename Path to the file.
	 */
	GDAPI WideString PathsWindows::Normalize(WideString const& filename)
	{
		auto filenameCopy(filename);
		for (auto& character : filenameCopy)
		{
			if (character == '\\')
				character = '/';
		}
		return filenameCopy;
	}

	/*!
	 * 'Platformizes' path string - converts path to system-specific path.
	 * @param filename Path to the file.
	 */
	GDAPI WideString PathsWindows::Platformize(WideString const& filename)
	{
		auto filenameCopy(filename);
		for (auto& character : filenameCopy)
		{
			if (character == '/')
				character = '\\';
		}
		return Paths::GetFullPath(filenameCopy);
	}

	/*!
	 * Returns a full path to the specified path string.
	 * @param filename Path to the file.
	 */
	GDAPI WideString PathsWindows::GetFullPath(WideString const& filename)
	{
		WCHAR filenameFullPath[MAX_PATH] = {};
		GD_ASSERT(GetFullPathNameW(filename.CStr(), GD_ARRAY_LENGTH(filenameFullPath), filenameFullPath, nullptr) != 0
			, "'GetFullPathName' function has failed.");

		return filenameFullPath;
	}

	/*!
	 * Returns relative path from one path to another.
	 *
	 * @param filenameFrom Path to file, that start relative path.
	 * @param filenameTo Path to file, which relative path we need to compute.
	 */
	GDAPI WideString PathsWindows::GetRelativePath(WideString const& filenameFrom, WideString const& filenameTo)
	{
		// 'PathRelativePathTo' function works correctly with backslashes only.
		WCHAR filenameRelativePath[MAX_PATH] = {};
		if (PathRelativePathToW(filenameRelativePath
			, Platformize(GetFullPath(filenameFrom)).CStr(), FILE_ATTRIBUTE_DIRECTORY
			, Platformize(GetFullPath(filenameTo)).CStr(), FILE_ATTRIBUTE_DIRECTORY) == TRUE)
		{
			return Paths::Normalize(filenameRelativePath + 2 /*skipping '.\\'*/);
		}
		return filenameFrom;
	}

	// ------------------------------------------------------------------------------------------
	// Engine specific paths.
	// ------------------------------------------------------------------------------------------

	/*!
	 * Returns path to the directory, where engine core is located.
	 * This maybe useful for reusing single engine core for several applications.
	 */
	GDAPI WideString const& PathsWindows::GetEngineRootDirectory()
	{
		WideString static engineRootDirectory;
		if (engineRootDirectory.IsEmpty())
		{
			WCHAR engineCoreLibraryPath[MAX_PATH] = {};
			GD_ASSERT(GetModuleFileNameW(g_GoddamnCoreHinstance, engineCoreLibraryPath, GD_ARRAY_LENGTH(engineCoreLibraryPath) - 1) != 0
				, "'GetModuleFileName' function has failed.");

			engineRootDirectory = Paths::Normalize(
				Paths::GetFullPath(
					Paths::Combine(
						Paths::GetDirectoryName(engineCoreLibraryPath), 
						L"../"
						)
					)
				);
		}
		return engineRootDirectory;
	}

	/*!
	 * Returns path to the directory, where engine can store user-related files.
	 */
	GDAPI WideString const& PathsWindows::GetEngineSavedDataUserDirectory()
	{
		WideString static engineSavedDataUserDirectory;
		if (engineSavedDataUserDirectory.IsEmpty())
		{
			// Disclaimer: originally, this engine core was designed to be used not only for games.
			// But we ironically save user data inside the 'Saved Games' directory.
			PWSTR windowsSavedGamedDirectory = nullptr;
			if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_SavedGames, CSIDL_FLAG_CREATE | CSIDL_FLAG_PER_USER_INIT, nullptr, &windowsSavedGamedDirectory)))
			{
				engineSavedDataUserDirectory = windowsSavedGamedDirectory;
				CoTaskMemFree(windowsSavedGamedDirectory);
			}
			else
			{
				// If something fails, we will try to save data inside user's home directory.
				WCHAR engineSavedDataUserDirectoryBuilder[MAX_PATH] = {};
				GD_ASSERT(SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_PERSONAL | CSIDL_FLAG_CREATE, nullptr, SHGFP_TYPE_CURRENT, engineSavedDataUserDirectoryBuilder))
					, "'SHGetFolderPath' function has failed.");
				engineSavedDataUserDirectory = Paths::Combine(
					engineSavedDataUserDirectoryBuilder,
					L"My Games/"
					);
			}
			engineSavedDataUserDirectory = Paths::Normalize(
				Paths::Combine(
					engineSavedDataUserDirectory, 
					GD_ENGINE_NAME L"/"
					)
				);
		}
		return engineSavedDataUserDirectory;
	}

	// ------------------------------------------------------------------------------------------
	// Application specific paths.
	// ------------------------------------------------------------------------------------------

	/*!
	 * Returns path to the file, where running executable is located.
	 */
	GDAPI WideString const& PathsWindows::GetApplicationExecutableFile()
	{
		WideString static applicationExecutableFile;
		if (applicationExecutableFile.IsEmpty())
		{
			WCHAR applicationExecutablePath[MAX_PATH] = {};
			GD_ASSERT(GetModuleFileNameW(nullptr, applicationExecutablePath, GD_ARRAY_LENGTH(applicationExecutablePath) - 1) != 0
				, "'GetModuleFileName' function has failed.");

			applicationExecutableFile = Paths::GetFileName(applicationExecutablePath);
		}
		return applicationExecutableFile;
	}

	// ReSharper restore CppRedundantQualifier

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_WINDOWS
