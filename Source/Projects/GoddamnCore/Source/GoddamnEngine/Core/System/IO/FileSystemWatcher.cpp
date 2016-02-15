// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/System/IO/FileSystemWatcher.cpp
 * File contains generic file system watcher interface.
 */
#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/System/App/App.h>
#if GD_PLATFORM_HAS_MULTITHREADING

#include <GoddamnEngine/Core/System/IO/FileSystemWatcher.h>
#if GD_PLATFORM_DESKTOP

#include <GoddamnEngine/Core/Templates/UniquePtr.h>
#include <GoddamnEngine/Core/System/IO/FileUtilities.h>
#include <GoddamnEngine/Core/System/Threading/Thread.h>
#include <GoddamnEngine/Core/System/Misc/Misc.h>

GD_NAMESPACE_BEGIN
	
	// ------------------------------------------------------------------------------------------
	// FileSystemWatcherThread class.
	// ------------------------------------------------------------------------------------------

	class FileSystemWatcherThread final : public Thread
	{
	public:
		GDAPI explicit FileSystemWatcherThread()
			: Thread("FileSystemWatchers' thread.", ThreadPriority::Lowest)
		{
		}

	private:
		GDINT virtual void OnRun() override final;
	};	// class FileSystemWatcherThread final

	// ------------------------------------------------------------------------------------------
	// FileSystemWatcherGeneric class.
	// ------------------------------------------------------------------------------------------

	static Vector<FileSystemWatcher*>         g_FileSystemWatchers;
	static UniquePtr<FileSystemWatcherThread> g_FileSystemWatchersThread;
	static CriticalSection                    g_FileSystemWatchersLock;

	/*!
	 * Periodically updates the watchers.
	 */
	GDINT void FileSystemWatcherThread::OnRun()
	{
		while (!App::IsTerminating())
		{
			{	// Detecting whether any active watchers are left. 
				ScopedCriticalSection fileSystemWatchersLock(g_FileSystemWatchersLock);
				if (g_FileSystemWatchers.IsEmpty())
				{
					g_FileSystemWatchersThread.Reset(nullptr);
					return;
				}

				// Updating states for all watchers.
				for (auto const fileSystemWatcher : g_FileSystemWatchers)
				{
					fileSystemWatcher->OnUpdate();
				}
			}

			PlatformMisc::Sleep(1000);
		}
	}

	/*!
	 * Initializes a new file system watcher.
	 *
	 * @param directoryName Path to the directory to watch.
	 * @param filter Wildcard for controlled file types.
	 */
	GDAPI FileSystemWatcherGeneric::FileSystemWatcherGeneric(WideString const& directoryName, WideString const& filter /*= L""*/, bool const recursive /*= true*/)
	{
		GD_NOT_USED(filter);
		GD_NOT_USED(recursive);
		GD_ASSERT(FileUtilities::DirectoryExists(directoryName), "Specified directory does not exist.");
		ScopedCriticalSection fileSystemWatchersLock(g_FileSystemWatchersLock);
		if (g_FileSystemWatchers.IsEmpty())
		{
			// No watcher was registered. This is first. We need to start watchers' thread. 
			if (g_FileSystemWatchersThread == nullptr)
				g_FileSystemWatchersThread = gd_new FileSystemWatcherThread();
		}
		g_FileSystemWatchers.InsertLast(static_cast<FileSystemWatcher*>(this));
	}

	GDAPI FileSystemWatcherGeneric::~FileSystemWatcherGeneric()
	{
		ScopedCriticalSection fileSystemWatchersLock(g_FileSystemWatchersLock);
		g_FileSystemWatchers.EraseAt(g_FileSystemWatchers.FindFirst(static_cast<FileSystemWatcher*>(this)));
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_DESKTOP

#endif	// if GD_PLATFORM_HAS_MULTITHREADING
