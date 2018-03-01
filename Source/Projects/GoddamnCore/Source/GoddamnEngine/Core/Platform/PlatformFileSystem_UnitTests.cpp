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
#include "GoddamnEngine/Core/IO/Paths.h"
#if GD_TESTING_ENABLED

#include <GoddamnEngine/Core/Containers/StringBuilder.h>

GD_NAMESPACE_BEGIN

	class DirectoryTestIterateDelegate : public IFileSystemDirectoryIterateDelegate
	{
	public:
		WideStringBuilder Contents;
	private:
		GDINT virtual bool OnVisitDirectoryEntry(WideString const& path, bool const isDirectory) override final
		{
			// Directories and files may come in different order on different platforms, so we need to sort entries.
			if (isDirectory)
			{
				Contents.AppendFormat(L"%ls 1;", Paths::GetFileName(path).CStr());
			}
			else
			{
				WideStringBuilder newContents;
				newContents.AppendFormat(L"%ls 0;", Paths::GetFileName(path).CStr());
				newContents.Append(Contents);
				Contents = Utils::Move(newContents);
			}
			return true;
		}
	};	// class DirectoryTestIterateDelegate

	// ------------------------------------------------------------------------------------------
	// File utilities.
	// ------------------------------------------------------------------------------------------

	gd_testing_unit_test(PlatformFileSystemFileExists)
	{
		// Checking if function returns true for an existing file.
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileExists(L"Tests/FileExists.txt"));

		// Checking if function returns false for non-existing file.
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileExists(L"Tests/FileDoesNotExist.txt") == false);

		// Checking if function returns false for an existing directory.
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileExists(L"Tests/DirectoryExists") == false);
	};

	gd_testing_unit_test(PlatformFileSystemFileRemove)
	{
		// Checking if function removed existing file.
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileRemove(L"Tests/FileCreateEmpty.txt"));
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileExists(L"Tests/FileCreateEmpty.txt") == false);

		// Checking if function failes on non-existing file.
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileRemove(L"Tests/FileDoesNotExist.txt") == false);

		// Checking if function returns false for an existing directory.
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileRemove(L"Tests/DirectoryExists") == false);
	};

	gd_testing_unit_test(PlatformFileSystemFileCreateEmpty)
	{
		// Checking if empty file is created.
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileCreateEmpty(L"Tests/FileCreateEmpty.txt"));
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileExists(L"Tests/FileCreateEmpty.txt"));

		// Checking if function returns true for an existing file.
		// If this function modifies contents of the directory then PlatformFileSystemFileOpenRead test will fail.
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileCreateEmpty(L"Tests/FileAlreadyExists.txt"));

		// Checking if function returns false for an existing directory.
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileCreateEmpty(L"Tests/DirectoryExists") == false);
	};

	// ------------------------------------------------------------------------------------------
	// Directory utilities.
	// ------------------------------------------------------------------------------------------

	gd_testing_unit_test(PlatformFileSystemDirectoryExists)
	{
		// Checking if function returns true for an existing directory.
		gd_testing_assert(IPlatformDiskFileSystem::Get().DirectoryExists(L"Tests/DirectoryExists"));

		// Checking if function returns false for non-existing directory.
		gd_testing_assert(IPlatformDiskFileSystem::Get().DirectoryExists(L"Tests/DirectoryDoesNotExist") == false);

		// Checking if function returns false for an existing file.
		gd_testing_assert(IPlatformDiskFileSystem::Get().DirectoryExists(L"Tests/FileExists.txt") == false);
	};

	gd_testing_unit_test(PlatformFileSystemDirectoryRemove)
	{
		// Checking if empty directory is removed.
		gd_testing_assert(IPlatformDiskFileSystem::Get().DirectoryRemove(L"Tests/DirectoryCreateEmpty"));
		gd_testing_assert(IPlatformDiskFileSystem::Get().DirectoryExists(L"Tests/DirectoryCreateEmpty") == false);

		// Checking if function returns false for non-empty directory.
		// If this function modifies contents of the directory then PlatformFileSystemDirectoryIterate test will fail.
		gd_testing_assert(IPlatformDiskFileSystem::Get().DirectoryRemove(L"Tests/DirectoryExists") == false);

		// Checking if function returns false for an existing file.
		gd_testing_assert(IPlatformDiskFileSystem::Get().DirectoryRemove(L"Tests/FileExists.txt") == false);
	};

	gd_testing_unit_test(PlatformFileSystemDirectoryRemoveRecursive)
	{
	};

	gd_testing_unit_test(PlatformFileSystemDirectoryCreateEmpty)
	{
		// Checking if empty directory is created.
		gd_testing_assert(IPlatformDiskFileSystem::Get().DirectoryCreateEmpty(L"Tests/DirectoryCreateEmpty"));
		gd_testing_assert(IPlatformDiskFileSystem::Get().DirectoryExists(L"Tests/DirectoryCreateEmpty"));

		// Checking if function returns true for an existing directory.
		// If this function modifies contents of the directory then PlatformFileSystemDirectoryIterate test will fail.
		gd_testing_assert(IPlatformDiskFileSystem::Get().DirectoryCreateEmpty(L"Tests/DirectoryExists"));

		// Checking if function returns false for an existing file.
		gd_testing_assert(IPlatformDiskFileSystem::Get().DirectoryCreateEmpty(L"Tests/FileExists.txt") == false);
	};

	gd_testing_unit_test(PlatformFileSystemDirectoryIterate)
	{
		// Checking if function returns true existing directory and iteration result is equal to expected.
		DirectoryTestIterateDelegate directoryTestIterateDelegate;
		gd_testing_assert(IPlatformDiskFileSystem::Get().DirectoryIterate(L"Tests/DirectoryExists", directoryTestIterateDelegate));
		gd_testing_assert(directoryTestIterateDelegate.Contents.ToString() == L"1.txt 0;2 1;");

		// Checking if function returns false for non-existing directory.
		gd_testing_assert(IPlatformDiskFileSystem::Get().DirectoryIterate(L"Tests/DirectoryDoesNotExist", directoryTestIterateDelegate) == false);

		// Checking if function returns false for an existing file.
		gd_testing_assert(IPlatformDiskFileSystem::Get().DirectoryIterate(L"Tests/FileExists.txt", directoryTestIterateDelegate) == false);
	};

	gd_testing_unit_test(PlatformFileSystemDirectoryIterateRecursive)
	{
		// Checking if function returns true existing directory and iteration result is equal to expected.
		DirectoryTestIterateDelegate directoryTestIterateDelegate;
		gd_testing_assert(IPlatformDiskFileSystem::Get().DirectoryIterateRecursive(L"Tests/DirectoryExists", directoryTestIterateDelegate));
		gd_testing_assert(directoryTestIterateDelegate.Contents.ToString() == L"1.txt 0;3.txt 0;2 1;");

		// Checking if function returns false for non-existing directory.
		gd_testing_assert(IPlatformDiskFileSystem::Get().DirectoryIterateRecursive(L"Tests/DirectoryDoesNotExist", directoryTestIterateDelegate) == false);

		// Checking if function returns false for an existing file.
		gd_testing_assert(IPlatformDiskFileSystem::Get().DirectoryIterateRecursive(L"Tests/FileExists.txt", directoryTestIterateDelegate) == false);
	};

	//gd_testing_unit_test(PlatformFileSystemFileSize)
	//{
	//	// Checking if function determines size of the existing file correctly. 
	//	UInt64 fileSize = 0;
	//	gd_testing_assert(IPlatformDiskFileSystem::Get().FileSize(L"Tests/FileExists.txt", fileSize));
	//	gd_testing_assert(fileSize == 1);

	//	// Checking if function failes on non-existing file.
	//	gd_testing_assert(IPlatformDiskFileSystem::Get().FileSize(L"Tests/FileDoesNotExist.txt", fileSize) == false);
	//};

	//gd_testing_unit_test(PlatformFileSystemFileMove)
	//{
	//	// Checking if function moves existing file to the exising location.
	//	IPlatformDiskFileSystem::Get().FileCreateEmpty(L"Tests/FileCreateEmpty.txt");
	//	IPlatformDiskFileSystem::Get().FileRemove(L"Tests/FileCreateEmpty2.txt");
	//	{
	//		// Checking if function moves existing file to the exising location.
	//		gd_testing_assert(IPlatformDiskFileSystem::Get().FileMove(L"Tests/FileCreateEmpty.txt", L"Tests/FileCreateEmpty2.txt"));
	//		gd_testing_assert(IPlatformDiskFileSystem::Get().FileExists(L"Tests/FileCreateEmpty2.txt"));
	//		gd_testing_assert(IPlatformDiskFileSystem::Get().FileExists(L"Tests/FileCreateEmpty.txt") == false);
	//	}
	//	IPlatformDiskFileSystem::Get().FileRemove(L"Tests/FileCreateEmpty2.txt");

	//	// Checking if function moves existing file to the exising location and overwrites exising file.
	//	IPlatformDiskFileSystem::Get().FileCreateEmpty(L"Tests/FileCreateEmpty.txt");
	//	IPlatformDiskFileSystem::Get().FileCreateEmpty(L"Tests/FileCreateEmpty2.txt");
	//	{
	//		// Checking if function moves existing file to the exising location and overwrites exising file.
	//		gd_testing_assert(IPlatformDiskFileSystem::Get().FileMove(L"Tests/FileCreateEmpty.txt", L"Tests/FileCreateEmpty2.txt", true));
	//		gd_testing_assert(IPlatformDiskFileSystem::Get().FileExists(L"Tests/FileCreateEmpty2.txt"));
	//		gd_testing_assert(IPlatformDiskFileSystem::Get().FileExists(L"Tests/FileCreateEmpty.txt") == false);
	//	}
	//	IPlatformDiskFileSystem::Get().FileRemove(L"Tests/FileCreateEmpty2.txt");

	//	// Checking if function failes on non-existing file.
	//	gd_testing_assert(IPlatformDiskFileSystem::Get().FileMove(L"Tests/FileDoesNotExist.txt", L"Tests/FileDoesNotExist2.txt") == false);

	//	// Checking if function failes on non-existing destination path.
	//	IPlatformDiskFileSystem::Get().FileCreateEmpty(L"Tests/FileCreateEmpty.txt");
	//	gd_testing_assert(IPlatformDiskFileSystem::Get().FileMove(L"Tests/FileCreateEmpty.txt", L"Tests/DirectoryDoesNotExist/FileCreateEmpty.txt") == false);
	//	IPlatformDiskFileSystem::Get().FileRemove(L"Tests/FileCreateEmpty.txt");

	//	// Checking if function failes on existing destination file without overwrite option.
	//	IPlatformDiskFileSystem::Get().FileCreateEmpty(L"Tests/FileCreateEmpty.txt");
	//	IPlatformDiskFileSystem::Get().FileCreateEmpty(L"Tests/FileCreateEmpty2.txt");
	//	gd_testing_assert(IPlatformDiskFileSystem::Get().FileMove(L"Tests/FileCreateEmpty.txt", L"Tests/FileCreateEmpty2.txt") == false);
	//	IPlatformDiskFileSystem::Get().FileRemove(L"Tests/FileCreateEmpty.txt");
	//	IPlatformDiskFileSystem::Get().FileRemove(L"Tests/FileCreateEmpty2.txt");
	//};

	//gd_testing_unit_test(PlatformFileSystemFileCopy)
	//{
	//	// Checking if function moves existing file to the exising location.
	//	IPlatformDiskFileSystem::Get().FileCreateEmpty(L"Tests/FileCreateEmpty.txt");
	//	IPlatformDiskFileSystem::Get().FileRemove(L"Tests/FileCreateEmpty2.txt");
	//	gd_testing_assert(IPlatformDiskFileSystem::Get().FileMove(L"Tests/FileCreateEmpty.txt", L"Tests/FileCreateEmpty2.txt"));
	//	IPlatformDiskFileSystem::Get().FileRemove(L"Tests/FileCreateEmpty2.txt");

	//	// Checking if function moves existing file to the exising location and overwrites exising file.
	//	IPlatformDiskFileSystem::Get().FileCreateEmpty(L"Tests/FileCreateEmpty.txt");
	//	IPlatformDiskFileSystem::Get().FileCreateEmpty(L"Tests/FileCreateEmpty2.txt");
	//	gd_testing_assert(IPlatformDiskFileSystem::Get().FileMove(L"Tests/FileCreateEmpty.txt", L"Tests/FileCreateEmpty2.txt", true));
	//	IPlatformDiskFileSystem::Get().FileRemove(L"Tests/FileCreateEmpty2.txt");

	//	// Checking if function failes on non-existing file.
	//	gd_testing_assert(IPlatformDiskFileSystem::Get().FileMove(L"Tests/FileDoesNotExist.txt", L"Tests/FileDoesNotExist2.txt") == false);

	//	// Checking if function failes on non-existing destination path.
	//	IPlatformDiskFileSystem::Get().FileCreateEmpty(L"Tests/FileCreateEmpty.txt");
	//	gd_testing_assert(IPlatformDiskFileSystem::Get().FileMove(L"Tests/FileCreateEmpty.txt", L"Tests/DirectoryDoesNotExist/FileCreateEmpty.txt") == false);
	//	IPlatformDiskFileSystem::Get().FileRemove(L"Tests/FileCreateEmpty.txt");

	//	// Checking if function failes on existing destination file without overwrite option.
	//	IPlatformDiskFileSystem::Get().FileCreateEmpty(L"Tests/FileCreateEmpty.txt");
	//	IPlatformDiskFileSystem::Get().FileCreateEmpty(L"Tests/FileCreateEmpty2.txt");
	//	gd_testing_assert(IPlatformDiskFileSystem::Get().FileMove(L"Tests/FileCreateEmpty.txt", L"Tests/FileCreateEmpty2.txt") == false);
	//	IPlatformDiskFileSystem::Get().FileRemove(L"Tests/FileCreateEmpty.txt");
	//	IPlatformDiskFileSystem::Get().FileRemove(L"Tests/FileCreateEmpty2.txt");
	//};

GD_NAMESPACE_END

#endif	// if GD_TESTING_ENABLED
