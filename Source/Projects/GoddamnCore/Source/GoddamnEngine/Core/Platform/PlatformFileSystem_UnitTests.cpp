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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCSimplifyInspection"

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

		// Checking if function fails on non-existing file.
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileExists(L"Tests/FileDoesNotExist.txt") == false);

		// Checking if function fails on an existing directory.
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileExists(L"Tests/DirectoryExists") == false);
	};

	gd_testing_unit_test(PlatformFileSystemFileRemove)
	{
		// Checking if function removed existing file.
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileRemove(L"Tests/FileCreateEmpty.txt"));
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileExists(L"Tests/FileCreateEmpty.txt") == false);

		// Checking if function fails on non-existing file.
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileRemove(L"Tests/FileDoesNotExist.txt") == false);

		// Checking if function fails on an existing directory.
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileRemove(L"Tests/DirectoryExists") == false);
	};

	gd_testing_unit_test(PlatformFileSystemFileCreateEmpty)
	{
		// Checking if empty file is created.
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileCreateEmpty(L"Tests/FileCreateEmpty.txt"));
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileExists(L"Tests/FileCreateEmpty.txt"));

		// Checking if function returns true for an existing file.
		// If this function modifies contents of the directory then PlatformFileSystemFileSize test will fail.
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileCreateEmpty(L"Tests/FileExists.txt"));

		// Checking if function fails on an existing directory.
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileCreateEmpty(L"Tests/DirectoryExists") == false);
	};

	gd_testing_unit_test(PlatformFileSystemFileCopy)
	{
		// Checking if function copies existing file.
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileCopy(L"Tests/FileExists.txt", L"Tests/FileExists2.txt"));
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileExists(L"Tests/FileExists2.txt"));

		// Checking if function copies existing file with overwrite.
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileCopy(L"Tests/FileExists.txt", L"Tests/FileExists2.txt") == false);
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileCopy(L"Tests/FileExists.txt", L"Tests/FileExists2.txt", true));

		// Checking if function fails on non-existing destination file directory.
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileCopy(L"Tests/FileExists.txt", L"Tests/DirectoryDoesNotExist/FileExists2.txt") == false);
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileCopy(L"Tests/FileExists.txt", L"Tests/DirectoryDoesNotExist/FileExists2.txt", true) == false);

		// Checking if function fails on non-existing file.
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileCopy(L"Tests/FileDoesNotExist.txt", L"Tests/FileDoesNotExist2.txt") == false);
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileCopy(L"Tests/FileDoesNotExist.txt", L"Tests/FileDoesNotExist2.txt", true) == false);

		// Checking if function fails on an existing directory source.
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileCopy(L"Tests/DirectoryExists", L"Tests/FileExists2.txt") == false);
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileCopy(L"Tests/DirectoryExists", L"Tests/FileExists2.txt", true) == false);

		// Checking if function fails on an existing directory destination.
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileCopy(L"Tests/FileExists.txt", L"Tests/DirectoryExists") == false);
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileCopy(L"Tests/FileExists.txt", L"Tests/DirectoryExists", true) == false);
	};

	gd_testing_unit_test(PlatformFileSystemFileMove)
	{
		// Checking if function copies existing file with overwrite.
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileMove(L"Tests/FileExists2.txt", L"Tests/FileExists.txt") == false);
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileMove(L"Tests/FileExists2.txt", L"Tests/FileExists.txt", true));

		// Checking if function fails on non-existing destination file directory.
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileMove(L"Tests/FileExists.txt", L"Tests/DirectoryDoesNotExist/FileExists2.txt") == false);
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileMove(L"Tests/FileExists.txt", L"Tests/DirectoryDoesNotExist/FileExists2.txt", true) == false);

		// Checking if function fails on non-existing file.
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileMove(L"Tests/FileDoesNotExist.txt", L"Tests/FileDoesNotExist2.txt") == false);
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileMove(L"Tests/FileDoesNotExist.txt", L"Tests/FileDoesNotExist2.txt", true) == false);

		// Checking if function fails on an existing directory source.
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileMove(L"Tests/DirectoryExists", L"Tests/FileExists2.txt") == false);
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileMove(L"Tests/DirectoryExists", L"Tests/FileExists2.txt", true) == false);

		// Checking if function fails on an existing directory destination.
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileMove(L"Tests/FileExists.txt", L"Tests/DirectoryExists") == false);
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileMove(L"Tests/FileExists.txt", L"Tests/DirectoryExists", true) == false);
	};

	gd_testing_unit_test(PlatformFileSystemFileSize)
	{
		// Checking if function determines size of the existing file correctly. 
		UInt64 fileSize = 0;
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileSize(L"Tests/FileExists.txt", fileSize));
		gd_testing_assert(fileSize == 1);

		// Checking if function fails on non-existing file.
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileSize(L"Tests/FileDoesNotExist.txt", fileSize) == false);

		// Checking if function fails on an existing directory.
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileSize(L"Tests/DirectoryExists", fileSize) == false);
	};

	gd_testing_unit_test(PlatformFileSystemFileOpenRead)
	{
		// Checking if function opens an input stream for the existing file correctly.
		auto const fileInputStream = IPlatformDiskFileSystem::Get().FileStreamOpenRead(L"Tests/FileExists.txt");
		gd_testing_assert(fileInputStream != nullptr);

		// Checking contents of the file.
		Char contents[256] = {};
		bool result = false;
		auto numBytesRead = fileInputStream->Read(contents, sizeof(contents), &result);
		gd_testing_assert(result && numBytesRead == 1 && contents[0] == '1');

		// Checking if end of file is correctly handled.
		numBytesRead = fileInputStream->Read(contents, sizeof(contents), &result);
		gd_testing_assert(result && numBytesRead == 0);

		// Checking if function fails on non-existing file.
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileStreamOpenRead(L"Tests/FileDoesNotExist.txt") == nullptr);

		// Checking if function fails on an existing directory.
		gd_testing_assert(IPlatformDiskFileSystem::Get().FileStreamOpenRead(L"Tests/DirectoryExists") == nullptr);
	};

	gd_testing_unit_test(PlatformFileSystemFileOpenWrite)
	{
		// Checking if function opens an input stream for the existing file correctly.
		auto fileOutputStream = IPlatformDiskFileSystem::Get().FileStreamOpenWrite(L"Tests/FileExists.txt", true);
		gd_testing_assert(fileOutputStream != nullptr);

		Char contents[256] = { '2' };
		bool result = false;
		auto numBytesWritten = fileOutputStream->Write(contents, sizeof(contents[0]), &result);
		gd_testing_assert(result && numBytesWritten == sizeof(contents[0]));
		fileOutputStream->Close(&result);
		gd_testing_assert(result);

		auto fileInputStream = IPlatformDiskFileSystem::Get().FileStreamOpenRead(L"Tests/FileExists.txt");
		gd_testing_assert(fileInputStream != nullptr);
		auto numBytesRead = fileInputStream->Read(contents, sizeof(contents), &result);
		gd_testing_assert(result && numBytesRead == 2 && contents[0] == '1' && contents[1] == '2');
		fileInputStream->Close(&result);
		gd_testing_assert(result);

		// Checking if function opens an input stream for the existing file correctly.
		fileOutputStream = IPlatformDiskFileSystem::Get().FileStreamOpenWrite(L"Tests/FileExists.txt", false);
		gd_testing_assert(fileOutputStream != nullptr);
		contents[0] = '1';
		numBytesWritten = fileOutputStream->Write(contents, sizeof(contents[0]), &result);
		gd_testing_assert(result && numBytesWritten == sizeof(contents[0]));
		fileOutputStream->Close(&result);
		gd_testing_assert(result);

		fileInputStream = IPlatformDiskFileSystem::Get().FileStreamOpenRead(L"Tests/FileExists.txt");
		gd_testing_assert(fileInputStream != nullptr);
		numBytesRead = fileInputStream->Read(contents, sizeof(contents), &result);
		gd_testing_assert(result && numBytesRead == 1 && contents[0] == '1');
		fileInputStream->Close(&result);
		gd_testing_assert(result);
	};

	// ------------------------------------------------------------------------------------------
	// Directory utilities.
	// ------------------------------------------------------------------------------------------

	gd_testing_unit_test(PlatformFileSystemDirectoryExists)
	{
		// Checking if function returns true for an existing directory.
		gd_testing_assert(IPlatformDiskFileSystem::Get().DirectoryExists(L"Tests/DirectoryExists"));

		// Checking if function fails on non-existing directory.
		gd_testing_assert(IPlatformDiskFileSystem::Get().DirectoryExists(L"Tests/DirectoryDoesNotExist") == false);

		// Checking if function fails on an existing file.
		gd_testing_assert(IPlatformDiskFileSystem::Get().DirectoryExists(L"Tests/FileExists.txt") == false);
	};

	gd_testing_unit_test(PlatformFileSystemDirectoryRemove)
	{
		// Checking if empty directory is removed.
		gd_testing_assert(IPlatformDiskFileSystem::Get().DirectoryRemove(L"Tests/DirectoryCreateEmpty"));
		gd_testing_assert(IPlatformDiskFileSystem::Get().DirectoryExists(L"Tests/DirectoryCreateEmpty") == false);

		// Checking if function fails on non-empty directory.
		// If this function modifies contents of the directory then PlatformFileSystemDirectoryIterate test will fail.
		gd_testing_assert(IPlatformDiskFileSystem::Get().DirectoryRemove(L"Tests/DirectoryExists") == false);

		// Checking if function fails on an existing file.
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

		// Checking if function fails on an existing file.
		gd_testing_assert(IPlatformDiskFileSystem::Get().DirectoryCreateEmpty(L"Tests/FileExists.txt") == false);
	};

	gd_testing_unit_test(PlatformFileSystemDirectoryIterate)
	{
		// Checking if function returns true existing directory and iteration result is equal to expected.
		DirectoryTestIterateDelegate directoryTestIterateDelegate;
		gd_testing_assert(IPlatformDiskFileSystem::Get().DirectoryIterate(L"Tests/DirectoryExists", directoryTestIterateDelegate));
		gd_testing_assert(directoryTestIterateDelegate.Contents.ToString() == L"1.txt 0;2 1;");

		// Checking if function fails on non-existing directory.
		gd_testing_assert(IPlatformDiskFileSystem::Get().DirectoryIterate(L"Tests/DirectoryDoesNotExist", directoryTestIterateDelegate) == false);

		// Checking if function fails on an existing file.
		gd_testing_assert(IPlatformDiskFileSystem::Get().DirectoryIterate(L"Tests/FileExists.txt", directoryTestIterateDelegate) == false);
	};

	gd_testing_unit_test(PlatformFileSystemDirectoryIterateRecursive)
	{
		// Checking if function returns true existing directory and iteration result is equal to expected.
		DirectoryTestIterateDelegate directoryTestIterateDelegate;
		gd_testing_assert(IPlatformDiskFileSystem::Get().DirectoryIterateRecursive(L"Tests/DirectoryExists", directoryTestIterateDelegate));
	//	gd_testing_assert(directoryTestIterateDelegate.Contents.ToString() == L"1.txt 0;3.txt 0;2 1;");

		// Checking if function fails on non-existing directory.
		gd_testing_assert(IPlatformDiskFileSystem::Get().DirectoryIterateRecursive(L"Tests/DirectoryDoesNotExist", directoryTestIterateDelegate) == false);

		// Checking if function fails on an existing file.
		gd_testing_assert(IPlatformDiskFileSystem::Get().DirectoryIterateRecursive(L"Tests/FileExists.txt", directoryTestIterateDelegate) == false);
	};

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

	//	// Checking if function fails on non-existing file.
	//	gd_testing_assert(IPlatformDiskFileSystem::Get().FileMove(L"Tests/FileDoesNotExist.txt", L"Tests/FileDoesNotExist2.txt") == false);

	//	// Checking if function fails on non-existing destination path.
	//	IPlatformDiskFileSystem::Get().FileCreateEmpty(L"Tests/FileCreateEmpty.txt");
	//	gd_testing_assert(IPlatformDiskFileSystem::Get().FileMove(L"Tests/FileCreateEmpty.txt", L"Tests/DirectoryDoesNotExist/FileCreateEmpty.txt") == false);
	//	IPlatformDiskFileSystem::Get().FileRemove(L"Tests/FileCreateEmpty.txt");

	//	// Checking if function fails on existing destination file without overwrite option.
	//	IPlatformDiskFileSystem::Get().FileCreateEmpty(L"Tests/FileCreateEmpty.txt");
	//	IPlatformDiskFileSystem::Get().FileCreateEmpty(L"Tests/FileCreateEmpty2.txt");
	//	gd_testing_assert(IPlatformDiskFileSystem::Get().FileMove(L"Tests/FileCreateEmpty.txt", L"Tests/FileCreateEmpty2.txt") == false);
	//	IPlatformDiskFileSystem::Get().FileRemove(L"Tests/FileCreateEmpty.txt");
	//	IPlatformDiskFileSystem::Get().FileRemove(L"Tests/FileCreateEmpty2.txt");
	//};

GD_NAMESPACE_END

#pragma clang diagnostic pop

#endif	// if GD_TESTING_ENABLED
