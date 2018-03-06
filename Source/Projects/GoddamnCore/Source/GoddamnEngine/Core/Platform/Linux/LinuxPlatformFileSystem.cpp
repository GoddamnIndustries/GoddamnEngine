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
#if GD_PLATFORM_GNU_LINUX

#include <sys/inotify.h>
#include <unistd.h>

#include <GoddamnEngine/Core/Platform/PlatformApplication.h>

#include <GoddamnEngine/Core/Templates/UniquePtr.h>
#include <GoddamnEngine/Core/Containers/Map.h>
#include "GoddamnEngine/Core/IO/Paths.h"

GD_NAMESPACE_BEGIN

	/*!
	 * Returns max number of user watches.
	 */
	GDINT static SizeTp INotifyGetMaxNumOfUserWatches()
	{
		static SizeTp maxNumOfUserWatches = 0;
		if (maxNumOfUserWatches)
		{
			auto const inputStream = IPlatformDiskFileSystem::Get().FileStreamOpenRead(L"/proc/sys/fs/inotify/max_user_watches");
			if (inputStream != nullptr)
			{
				Char maxNumOfUserWatchesString[16 + 1] = {};
				inputStream->Read(maxNumOfUserWatchesString, sizeof(maxNumOfUserWatchesString) - sizeof(maxNumOfUserWatchesString[0]));
				maxNumOfUserWatches = CString::Strtoui64(maxNumOfUserWatchesString);
			}
		}
		return maxNumOfUserWatches;
	}

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! INotify event iterator.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL INotifyEventIterator final
	{
	private:
		Byte const* m_BufferCurrent;
		Byte const* m_BufferEnd;
	public:
		GDINL explicit INotifyEventIterator(Byte const* const bufferCurrent, Byte const* const bufferEnd)
			: m_BufferCurrent(bufferCurrent), m_BufferEnd(bufferEnd)
		{
		}
	public:
		GDINL bool IsEnd() const
		{
			return m_BufferCurrent < m_BufferEnd;
		}
		GDINL INotifyEventIterator FindNextRelated(INotifyEventIterator const& to) const
		{
			auto iter = *this;
			for (++iter; iter.IsEnd() && iter->cookie != to->cookie; ++iter);
			return iter;
		}
		GDINL INotifyEventIterator FindNextRelated() const
		{
			return FindNextRelated(*this);
		}
	public:
		// *iterator
		GDINL inotify_event const& operator* () const
		{
			GD_DEBUG_VERIFY(IsEnd());
			return *reinterpret_cast<inotify_event const*>(m_BufferCurrent);
		}
		// iterator->
		GDINL inotify_event const* operator-> () const
		{
			GD_DEBUG_VERIFY(IsEnd());
			return reinterpret_cast<inotify_event const*>(m_BufferCurrent);
		}
		// ++iterator
		GDINL INotifyEventIterator& operator++ ()
		{
			m_BufferCurrent = Min(m_BufferCurrent + sizeof(**this) + (**this).len, m_BufferEnd);
			return *this;
		}
	};  // class INotifyEventIterator

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! INotify event queue.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL INotifyEventQueue final : public TNonCopyable
	{
	private:
		UniquePtr<Byte[]> m_QueueBuffer;
		SizeTp m_QueueLength;
	public:
		GDINL explicit INotifyEventQueue()
			: m_QueueBuffer(gd_new Byte[GetCapacity()]), m_QueueLength(0)
		{
		}
	public:
		GDINL static SizeTp GetCapacity()
		{
			return 1024 * (sizeof(inotify_event) + FILENAME_MAX);
		}
		GDINL Byte* GetData() const
		{
			return m_QueueBuffer.Get();
		}
		GDINL SizeTp GetLength() const
		{
			return m_QueueLength;
		}
		GDINL void Resize(SizeTp const newLength)
		{
			m_QueueLength = newLength;
		}
	public:
		GDINL INotifyEventIterator Begin() const
		{
			return INotifyEventIterator(m_QueueBuffer.Get(), m_QueueBuffer.Get() + GetLength());
		}
	};  // class INotifyEventQueue

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! INotify wrapper.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL INotifyWatch final : public TNonCopyable
	{
	public:
		Int32 INotify;
		Map<Int32, WideString> WatchDescriptors;
		Map<WideString, Int32> WatchDescriptorsPaths;
		INotifyEventQueue EventQueue;
	public:
		GDINL explicit INotifyWatch(Int32 const iNotify)
			: INotify(iNotify)
		{
		}
		GDINL ~INotifyWatch()
		{
			close(INotify);
		}
	public:
		GDINL WideString EventPath(INotifyEventIterator const& eventIterator)
		{
			return Paths::Combine(WatchDescriptors[eventIterator->wd], StringConv::DecodeUTF8(eventIterator->name));
		}
	public:
		GDINL bool AddDirectory(WideString const& directoryName)
		{
			auto const directoryNameSystem = StringConv::EncodeUTF8(Paths::Platformize(directoryName));
			auto const watchDescriptor = inotify_add_watch(INotify, directoryNameSystem.CStr(), IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO);
			if (watchDescriptor != -1)
			{
				WatchDescriptors.Insert(watchDescriptor, directoryName);
				WatchDescriptorsPaths.Insert(directoryName, watchDescriptor);
				return true;
			}
			return false;
		}
		GDINL bool AddDirectoryRecursive(WideString const& directoryName)
		{
			class AddDirectoryRecursiveDelegate final : public IFileSystemDirectoryIterateDelegate
			{
			private:
				INotifyWatch& m_Watch;
			public:
				GDINL explicit AddDirectoryRecursiveDelegate(INotifyWatch& watch)
					: m_Watch(watch)
				{
				}
			public:
				GDINT virtual bool OnVisitDirectoryEntry(WideString const& path, bool const isDirectory) override final
				{
					if (isDirectory)
					{
						return m_Watch.AddDirectory(path);
					}
					return true;
				}
			};  // class AddDirectoryRecursiveDelegate

			if (AddDirectory(directoryName))
			{
				AddDirectoryRecursiveDelegate addDirectoryRecursiveDelegate(*this);
				return IPlatformDiskFileSystem::Get().DirectoryIterateRecursive(directoryName, addDirectoryRecursiveDelegate);
			}
			return false;
		}
		GDINL bool RemoveDirectory(WideString const& directoryName)
		{
			auto const watchDescriptorPtr = WatchDescriptorsPaths.Find(directoryName);
			if (watchDescriptorPtr != nullptr)
			{
				if (inotify_rm_watch(INotify, *watchDescriptorPtr) != -1)
				{
					WatchDescriptors.Erase(*watchDescriptorPtr);
					WatchDescriptorsPaths.Erase(directoryName);
				}
				return true;
			}
			return false;
		}
		GDINL bool Read()
		{
			auto const numBytesRead = read(INotify, EventQueue.GetData(), EventQueue.GetCapacity());
			if (numBytesRead != -1)
			{
				EventQueue.Resize(static_cast<SizeTp>(numBytesRead));
				return true;
			}
			return false;
		}
	};  // class INotifyWatch

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Disk file system watcher on Linux platforms.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL LinuxPlatformDiskFileSystemWatcher : public IPlatformDiskFileSystemWatcher
	{
	private:
		GDINT virtual bool WatchDirectoryRecursive(WideString const& directoryName, IFileSystemWatcherDelegate& directoryWatcherDelegate) override final;
	};	// class LinuxPlatformDiskFileSystemWatcher

	GD_IMPLEMENT_SINGLETON(IPlatformDiskFileSystemWatcher, LinuxPlatformDiskFileSystemWatcher);

	// ------------------------------------------------------------------------------------------
	// Watch utilities.
	// ------------------------------------------------------------------------------------------

	/*!
	 * Watches all filesystem events on the specified directory.
	 * This function returns on application exit.
	 *
	 * @param directoryName Path to the directory.
	 * @param directoryWatcherDelegate Watch event delegate.
	 *
	 * @returns True if operation succeeded.
	 */
	GDINT bool LinuxPlatformDiskFileSystemWatcher::WatchDirectoryRecursive(WideString const& directoryName, IFileSystemWatcherDelegate& directoryWatcherDelegate)
	{
		auto const watchINotifyDescriptor = inotify_init();
		if (watchINotifyDescriptor == -1)
		{
			return false;
		}

		INotifyWatch iNotifyWatch(watchINotifyDescriptor);
		if (!iNotifyWatch.AddDirectoryRecursive(directoryName))
		{
			return false;
		}

		//! @todo
		while (/*IPlatformApplication::Get().IsRunning()*/ true)
		{
			if (!iNotifyWatch.Read())
			{
				return false;
			}
			for (auto iNotifyEventIterator = iNotifyWatch.EventQueue.Begin(); iNotifyEventIterator.IsEnd(); ++iNotifyEventIterator)
			{
				switch (iNotifyEventIterator->mask & IN_ALL_EVENTS)
				{
					// File or directory was created.
					case IN_CREATE:
						{
							auto const iNotifyEventIsDir = (iNotifyEventIterator->mask & IN_ISDIR) != 0;
							auto const iNotifyEventPath = iNotifyWatch.EventPath(iNotifyEventIterator);
							if (iNotifyEventIsDir && !iNotifyWatch.AddDirectory(iNotifyEventPath))
							{
								return false;
							}
							if (!directoryWatcherDelegate.OnFileOrDirectoryCreated(iNotifyEventPath, iNotifyEventIsDir))
							{
								return false;
							}
							break;
						}
					// File or directory was removed from watch directory.
					case IN_DELETE:
						{
							auto const iNotifyEventIsDir = (iNotifyEventIterator->mask & IN_ISDIR) != 0;
							auto const iNotifyEventPath = iNotifyWatch.EventPath(iNotifyEventIterator);
							if (iNotifyEventIsDir && !iNotifyWatch.RemoveDirectory(iNotifyEventPath))
							{
								return false;
							}
							if (!directoryWatcherDelegate.OnFileOrDirectoryRemoved(iNotifyEventPath, iNotifyEventIsDir))
							{
								return false;
							}
							break;
						}
					// File or directory was modified.
					case IN_MODIFY:
						{
							auto const iNotifyEventIsDir = (iNotifyEventIterator->mask & IN_ISDIR) != 0;
							auto const iNotifyEventPath = iNotifyWatch.EventPath(iNotifyEventIterator);
							if (!directoryWatcherDelegate.OnFileOrDirectoryModified(iNotifyEventPath, iNotifyEventIsDir))
							{
								return false;
							}
							break;
						}
					// File or directory was renamed or moved from watch directory.
					case IN_MOVED_FROM:
						{
							auto const iNotifyEventIsDir = (iNotifyEventIterator->mask & IN_ISDIR) != 0;
							auto const movedToEventIterator = iNotifyEventIterator.FindNextRelated();
							if (movedToEventIterator.IsEnd())
							{
								// File or directory was created or moved inside watch directory.
								auto const watchMovedFromEventPath = iNotifyWatch.EventPath(iNotifyEventIterator);
								auto const watchMovedToEventPath = iNotifyWatch.EventPath(movedToEventIterator);
								if (!directoryWatcherDelegate.OnFileOrDirectoryMoved(watchMovedFromEventPath, watchMovedToEventPath, iNotifyEventIsDir))
								{
									return false;
								}
							}
							else
							{
								// File or directory was moved out from watch directory.
								auto const iNotifyEventPath = iNotifyWatch.EventPath(iNotifyEventIterator);
								if (!directoryWatcherDelegate.OnFileOrDirectoryRemoved(iNotifyEventPath, iNotifyEventIsDir))
								{
									return false;
								}
							}
							break;
						}
					// File or directory was renamed or moved into watch directory.
					case IN_MOVED_TO:
						{
							auto const movedFromEventIterator = iNotifyWatch.EventQueue.Begin().FindNextRelated(iNotifyEventIterator);
							if (!movedFromEventIterator.IsEnd())
							{
								// File or directory was moved into watch directory.
								auto const iNotifyEventIsDir = (iNotifyEventIterator->mask & IN_ISDIR) != 0;
								auto const iNotifyEventPath = iNotifyWatch.EventPath(iNotifyEventIterator);
								if (!directoryWatcherDelegate.OnFileOrDirectoryCreated(iNotifyEventPath, iNotifyEventIsDir))
								{
									return false;
								}
							}
							break;
						}
					default:
						break;
				}
			}
		}
		return true;
	}

GD_NAMESPACE_END

#endif  // if GD_PLATFORM_GNU_LINUX
