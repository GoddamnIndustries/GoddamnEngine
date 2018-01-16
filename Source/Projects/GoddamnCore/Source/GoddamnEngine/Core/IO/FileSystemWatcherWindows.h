// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/IO/FileSystemWatcherWindows.h
 * File contains Windows file system watcher interface.
 */
#pragma once
#if !defined(GD_INSIDE_FILESYSTEMWATCHER_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Core/IO/FileSystemWatcher.h> instead.
#endif	// if !defined(GD_INSIDE_FILESYSTEMWATCHER_H)

#include <Windows.h>

GD_NAMESPACE_BEGIN

	// **------------------------------------------------------------------------------------------**
	//! Windows file system watcher.
	// **------------------------------------------------------------------------------------------**
	class FileSystemWatcherWindows final : public FileSystemWatcherGeneric
	{
		friend class FileSystemWatcherThread;

	private:
		WideString m_Filter;
		HANDLE     m_DirectoryHandle;
		BYTE       m_ChangesBuffer[sizeof(DWORD) * 8 * 1024];
		BOOL	   m_Recursive;

	public:
		GDAPI explicit FileSystemWatcherWindows(WideString const& directoryName, WideString const& filter = L"", bool const recursive = true);
		GDAPI ~FileSystemWatcherWindows();

	private:
		GDAPI void OnUpdate();
	};	// class FileSystemWatcherWindows

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Cross-platform file system watcher class.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	using FileSystemWatcher = FileSystemWatcherWindows;

GD_NAMESPACE_END
