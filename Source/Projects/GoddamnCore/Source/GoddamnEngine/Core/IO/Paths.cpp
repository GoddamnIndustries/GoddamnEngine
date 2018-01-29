// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/IO/Paths.cpp
 * File contains generic paths helpers interface.
 */
#include <GoddamnEngine/Core/IO/Paths.h>
#include <GoddamnEngine/Core/Containers/StringBuilder.h>

GD_NAMESPACE_BEGIN

	// ReSharper disable CppRedundantQualifier

	// ------------------------------------------------------------------------------------------
	// Helper functions.
	// ------------------------------------------------------------------------------------------

	/*!
	 * Combines to paths.
	 *
	 * @param firstPath First path to combine.
	 * @param secondPath Second path to combine.
	 * @returns Combination of two paths.
	 */
	 //! @{
	GDAPI WideString PathsGeneric::Combine(WideString const& firstPath, WideString const& secondPath)
	{
		WideStringBuilder combinedPath;
		combinedPath.Append(firstPath);
		if (firstPath.GetLast() != '/' && firstPath.GetLast() != '\\')
		{
			combinedPath.Append('/');
		}
		combinedPath.Append(secondPath[0] == L'/' || secondPath[0] == L'\\' ? secondPath.CStr() + 1 : secondPath.CStr());
		return combinedPath.ToString();
	}
	GDAPI WideString PathsGeneric::Combine(WideString const& firstPath, WideString const& secondPath, WideString const& thirdPath)
	{
		return Paths::Combine(Paths::Combine(firstPath, secondPath), thirdPath);
	}
	//! @}

	GDINL static UInt32 FindLastChar(WideString const& filename, WideChar const query)
	{
		auto const ptr = CString::Strrchr(filename.CStr(), query);
		return ptr != nullptr ? *ptr : UInt32Max;
	}
	GDINL static UInt32 FindLastChar(WideString const& filename, WideChar const query1, WideChar const query2)
	{
		auto const iter = Algo::FindLastIf(filename, [=](WideChar const character) { return character == query1 || character == query2; });
		return iter != filename.End() ? static_cast<UInt32>(&*iter - filename.CStr()) : UInt32Max;
	}

	/*!
	 * Returns extension information for specified path string.
	 * E.g. "directory/file.ext" -> ".ext"
	 *
	 * @param filename Path to the file.
	 */
	GDAPI WideString PathsGeneric::GetExtension(WideString const& filename)
	{
		auto const lastDot = FindLastChar(filename, '.');
		return lastDot == UInt32Max ? filename : filename.Substring(lastDot);
	}

	/*!
	 * Returns file and extension information for specified path string.
	 * E.g. "directory/file.ext" -> "file.ext"
	 *
	 * @param filename Path to the file.
	 */
	GDAPI WideString PathsGeneric::GetFileName(WideString const& filename)
	{
		auto const lastSlashOrBackslash = FindLastChar(filename, '/', '\\');
		return lastSlashOrBackslash == UInt32Max ? filename : filename.Substring(lastSlashOrBackslash + 1);
	}

	/*!
	 * Returns file information for specified path string (without extension).
	 * E.g. "directory/file.ext" -> "file"
	 *
	 * @param filename Path to the file.
	 */
	GDAPI WideString PathsGeneric::GetFileNameWithoutExtension(WideString const& filename)
	{
		return Paths::GetFileName(Paths::GetDirectoryAndFileNameWithoutExtension(filename));
	}

	/*!
	 * Returns directory information for specified path string.
	 * E.g. "directory/file.ext" -> "directory/"
	 *
	 * @param filename Path to the file.
	 */
	GDAPI WideString PathsGeneric::GetDirectoryName(WideString const& filename)
	{
		auto const lastSlashOrBackslash = FindLastChar(filename, '/', '\\');
		return lastSlashOrBackslash == UInt32Max ? filename : filename.Substring(0, lastSlashOrBackslash);
	}

	/*!
	 * Returns directory file information for specified path string (without extension).
	 * E.g. "directory/file.ext" -> "directory/file"
	 *
	 * @param filename Path to the file.
	 */
	GDAPI WideString PathsGeneric::GetDirectoryAndFileNameWithoutExtension(WideString const& filename)
	{
		auto const lastDot = FindLastChar(filename, '.');
		return lastDot == UInt32Max ? filename : filename.Substring(0, lastDot);
	}

	// ------------------------------------------------------------------------------------------
	// Engine specific paths.
	// ------------------------------------------------------------------------------------------

	/*!
	 * Returns path to the directory, where engine binaries are located.
	 */
	GDAPI WideString const& PathsGeneric::GetEngineBinariesDirectory()
	{
		auto static const engineBinariesDirectory = Paths::Normalize(
			Paths::Combine(
				Paths::GetEngineRootDirectory(),
				L"Bin/"
				)
			);
		return engineBinariesDirectory;
	}

	/*!
	 * Returns path to the directory, where engine content files are located.
	 */
	GDAPI WideString const& PathsGeneric::GetEngineContentDirectory()
	{
		auto static const engineContentDirectory = Paths::Normalize(
			Paths::Combine(
				Paths::GetEngineRootDirectory(),
				L"Content/"
				)
			);
		return engineContentDirectory;
	}
	
	/*!
	 * Returns path to the directory, where engine configuration files are located.
	 */
	GDAPI WideString const& PathsGeneric::GetEngineConfigDirectory()
	{
		auto static const engineConfigDirectory = Paths::Normalize(
			Paths::Combine(
				Paths::GetEngineRootDirectory(),
				L"Config/"
				)
			);
		return engineConfigDirectory;
	}

	/*!
	 * Returns path to the directory, where engine can store engine-related files.
	 */
	GDAPI WideString const& PathsGeneric::GetEngineSavedDataDirectory()
	{
		auto static const engineConfigDirectory = Paths::Normalize(
			Paths::Combine(
				Paths::GetEngineRootDirectory(),
					L"SavedData/"
				)
			);
		return engineConfigDirectory;
	}

	// ------------------------------------------------------------------------------------------
	// Application specific paths.
	// ------------------------------------------------------------------------------------------

	/*!
	 * Returns path to the directory, where application is located.
	 * This maybe useful for reusing single application for several applications.
	 */
	GDAPI WideString const& PathsGeneric::GetApplicationRootDirectory()
	{
		auto static const applicationRootDirectory = Paths::Normalize(
			Paths::GetFullPath(
				L"../"
				)
			);
		return applicationRootDirectory;
	}

	/*!
	 * Returns path to the directory, where application binaries are located.
	 */
	GDAPI WideString const& PathsGeneric::GetApplicationBinariesDirectory()
	{
		auto static const applicationBinariesDirectory = Paths::Normalize(
			Paths::Combine(
				Paths::GetApplicationRootDirectory(),
					L"Bin/"
				)
			);
		return applicationBinariesDirectory;
	}

	/*!
	 * Returns path to the directory, where application content files are located.
	 */
	GDAPI WideString const& PathsGeneric::GetApplicationContentDirectory()
	{
		auto static const applicationContentDirectory = Paths::Normalize(
			Paths::Combine(
				Paths::GetApplicationRootDirectory(),
					L"Content/"
				)
			);
		return applicationContentDirectory;
	}

	/*!
	 * Returns path to the directory, where application configuration files are located.
	 */
	GDAPI WideString const& PathsGeneric::GetApplicationConfigDirectory()
	{
		auto static const applicationConfigDirectory = Paths::Normalize(
			Paths::Combine(
				Paths::GetApplicationRootDirectory(),
					L"Config/"
				)
			);
		return applicationConfigDirectory;
	}

	/*!
	 * Returns path to the directory, where application can store application-related files.
	 */
	GDAPI WideString const& PathsGeneric::GetApplicationSavedDataDirectory()
	{
		auto static const applicationSavedDataDirectory = Paths::Normalize(
			Paths::Combine(
				Paths::GetApplicationRootDirectory(),
					L"SavedData/"
				)
			);
		return applicationSavedDataDirectory;
	}

	/*!
	 * Returns path to the directory, where application can store user-related files.
	 */
	GDAPI WideString const& PathsGeneric::GetApplicationSavedDataUserDirectory()
	{
		auto static applicationSavedDataUserDirectory = Paths::Normalize(
			Paths::Combine(
				Paths::GetEngineSavedDataUserDirectory(),
				Paths::GetFileNameWithoutExtension(		// To remove configuration postfix. "file.release.exe" -> "file"
					Paths::GetFileNameWithoutExtension(	// To remove extension. E.g. "file.release.exe" -> "file.release"
						Paths::GetApplicationExecutableFile()
						)
					) + L"/"
				)
			);
		return applicationSavedDataUserDirectory;
	}

	// ReSharper restore CppRedundantQualifier

GD_NAMESPACE_END
