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
#include <GoddamnEngine/Core/Platform/FileSystem.h>
#include "GoddamnEngine/Core/IO/Directory.h"
#include "GoddamnEngine/Core/IO/Paths.h"

GD_NAMESPACE_BEGIN

	/*!
	 * Removes the existing directory and everything inside it.
	 *
	 * @param directoryName Path to the directory.
	 * @returns True if directory was successfully deleted.
	 */
	GDINT bool IFileSystem::DirectoryRemoveRecursive(WideString const& directoryName)
	{
		Directory directory(directoryName.CStr());
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
		}
		return false;
	}

GD_NAMESPACE_END
