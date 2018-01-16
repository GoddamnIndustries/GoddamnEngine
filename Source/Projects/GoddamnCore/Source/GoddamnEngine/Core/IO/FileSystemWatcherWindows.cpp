// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/IO/FileSystemWatcherWindows.cpp
 * File contains Windows file system watcher interface.
 */
#include <GoddamnEngine/Include.h>
#if GD_PLATFORM_HAS_MULTITHREADING

#include <GoddamnEngine/Core/IO/FileSystemWatcher.h>
#if GD_PLATFORM_WINDOWS

#include <GoddamnEngine/Core/IO/Paths.h>

GD_NAMESPACE_BEGIN

	/*!
	 * Initializes a new file system watcher.
	 *
	 * @param directoryName Path to the directory to watch.
	 * @param filter Wildcard for controlled file types.
	 * @param recursive Do recursively watch subdirectories.
	 */
	GDAPI FileSystemWatcherWindows::FileSystemWatcherWindows(WideString const& directoryName, WideString const& filter /*= L""*/, bool const recursive /*= true*/)
		: FileSystemWatcherGeneric(directoryName, filter)
		, m_Filter(filter), m_DirectoryHandle(nullptr), m_Recursive(recursive)
	{
		m_DirectoryHandle = CreateFileW(Paths::Platformize(directoryName).CStr(), FILE_LIST_DIRECTORY
			, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr
			, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
		GD_VERIFY(m_DirectoryHandle != INVALID_HANDLE_VALUE, "'CreateFile' function has failed.");

		ReadDirectoryChangesW(m_DirectoryHandle, m_ChangesBuffer, sizeof m_ChangesBuffer, m_Recursive
			, FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE
			, nullptr, nullptr, nullptr);
	}

	GDAPI FileSystemWatcherWindows::~FileSystemWatcherWindows()
	{
		CloseHandle(m_DirectoryHandle);
	}

	/*!
	 * Updates the watcher state and notifies the handlers.
	 * Would called from separate thread once per second.
	 */
	GDAPI void FileSystemWatcherWindows::OnUpdate()
	{
		if (ReadDirectoryChangesW(m_DirectoryHandle, m_ChangesBuffer, sizeof m_ChangesBuffer, m_Recursive
			, FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE
			, nullptr, nullptr, nullptr) == TRUE)
		{
			for (SizeTp offset = 0;;)
			{
				// Reading next change from the buffer.
				auto notifyInformation = reinterpret_cast<PFILE_NOTIFY_INFORMATION>(m_ChangesBuffer + offset);

				// Converting event to the known format.
				FileSystemWatcherEvent event = {};
				event.Name = WideString(notifyInformation->FileName, notifyInformation->FileNameLength / sizeof notifyInformation->FileName[0]);
				if (m_Filter.IsEmpty() || event.Name.MatchesWildcard(m_Filter))
				{
					switch (notifyInformation->Action)
					{
						case FILE_ACTION_MODIFIED:
							event.Action = FileSystemWatcherEventAction::Changed;
							break;
						case FILE_ACTION_ADDED:
							event.Action = FileSystemWatcherEventAction::Created;
							break;
						case FILE_ACTION_REMOVED:
							event.Action = FileSystemWatcherEventAction::Deleted;
							break;
						case FILE_ACTION_RENAMED_OLD_NAME:
							{	// Renaming action sends two notifications: first with old name, and second with the new one.
								offset += notifyInformation->NextEntryOffset;
								notifyInformation = reinterpret_cast<PFILE_NOTIFY_INFORMATION>(m_ChangesBuffer + offset);
								GD_DEBUG_VERIFY(notifyInformation->Action == FILE_ACTION_RENAMED_NEW_NAME, "Unknown changes buffer layout for renaming.");
							}
							event.Action = FileSystemWatcherEventAction::Renamed;
							event.NewName = WideString(notifyInformation->FileName, notifyInformation->FileNameLength / sizeof notifyInformation->FileName[0]);
							break;
						default:
							GD_NOT_IMPLEMENTED();
					}

					{	// And notifying all handlers.
						ScopedCriticalSection handlersLock(m_HandlersLock);
						for (auto handler : m_Handlers)
						{
							if ((handler.Filters & event.Action) != FileSystemWatcherNotifyFilters::None)
							{
								handler.Function(event);
							}
						}
					}
				}

				if (notifyInformation->NextEntryOffset != 0)
				{
					offset += notifyInformation->NextEntryOffset;
				}
				else
				{
					break;
				}
			}
		}
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_WINDOWS

#endif	// if GD_PLATFORM_HAS_MULTITHREADING
