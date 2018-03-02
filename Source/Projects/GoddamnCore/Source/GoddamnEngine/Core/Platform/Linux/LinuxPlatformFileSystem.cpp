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

#include <GoddamnEngine/Core/Templates/UniquePtr.h>
#include "GoddamnEngine/Core/IO/Paths.h"

GD_NAMESPACE_BEGIN

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
		GDINL bool IsValid() const
		{
			return m_BufferCurrent < m_BufferEnd;
		}
		GDINL INotifyEventIterator FindNextRelated(INotifyEventIterator const& to) const
		{
			auto iter = *this;
			for (++iter; iter.IsValid() || iter->cookie != to->cookie; ++iter);
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
			GD_DEBUG_VERIFY(IsValid());
			return *reinterpret_cast<inotify_event const*>(m_BufferCurrent);
		}
		// iterator->
		GDINL inotify_event const* operator-> () const
		{
			GD_DEBUG_VERIFY(IsValid());
			return reinterpret_cast<inotify_event const*>(m_BufferCurrent);
		}
		// iterator++
		GDINL INotifyEventIterator& operator++ ()
		{
			m_BufferCurrent = Min(m_BufferCurrent + sizeof(**this) + (**this).len, m_BufferEnd);
			return *this;
		}
		GDINL INotifyEventIterator operator++ (int const unused)
		{
			GD_NOT_USED_L(unused);
			INotifyEventIterator copy(*this);
			++*this;
			return copy;
		}
	};  // class INotifyEventIterator

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! INotify event container.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL INotifyEventContainer final : public TNonCopyable
	{
	private:
		UniquePtr<Byte[]> m_Buffer;
		SizeTp m_BufferLength;
	public:
		GDINL explicit INotifyEventContainer(SizeTp const length)
			: m_Buffer(gd_new Byte[length * (sizeof(inotify_event) + FILENAME_MAX)]), m_BufferLength(length)
		{
		}
	public:
		GDINL Byte* GetData()
		{
			return m_Buffer.Get();
		}
		GDINL SizeTp GetLength() const
		{
			return m_BufferLength;
		}
	public:
		GDINL INotifyEventIterator Begin(SizeTp const lengthOfEvents) const
		{
			return INotifyEventIterator(m_Buffer.Get(), m_Buffer.Get() + lengthOfEvents);
		}
	};  // class INotifyEventContainer

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Disk file system watcher on Linux platforms.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL LinuxPlatformDiskFileSystemWatcher : public IPlatformDiskFileSystemWatcher
	{
	private:
		GDINT virtual bool WatchCreate(WideString const& directoryName, Handle& watchHandle) override final;
		GDINT virtual bool WatchDestroy(Handle const watchHandle) override final;
		GDINT virtual bool WatchReadEvents(Handle const watchHandle, IFileSystemWatcherDelegate& directoryWatcherDelegate) override final;
	};	// class LinuxPlatformDiskFileSystemWatcher

	GD_IMPLEMENT_SINGLETON(IPlatformDiskFileSystemWatcher, LinuxPlatformDiskFileSystemWatcher);

	// ------------------------------------------------------------------------------------------
	// Watch utilities.
	// ------------------------------------------------------------------------------------------

	/*!
	 * Creates a new watch for the specified directory.
	 *
	 * @param directoryName Path to the directory.
	 * @param watchHandle Watch handle.
	 *
	 * @returns True if directory exists or new watch was successfully created.
	 */
	GDINT bool LinuxPlatformDiskFileSystemWatcher::WatchCreate(WideString const& directoryName, Handle& watchHandle)
	{
		auto const watchHandleSystem = inotify_init();
		if (watchHandleSystem != -1)
		{
			auto const directoryNameSystem = StringConv::EncodeUTF8(Paths::Platformize(directoryName));
			if (inotify_add_watch(watchHandleSystem, directoryNameSystem.CStr(), IN_ALL_EVENTS) != -1)
			{
				watchHandle = reinterpret_cast<Handle>(watchHandleSystem);
				return true;
			}
		}
		return false;
	}

	/*!
	 * Closes a watch handle.
	 *
	 * @param watchHandle Watch handle.
	 * @returns True if operation succeeded.
	 */
	GDINT bool LinuxPlatformDiskFileSystemWatcher::WatchDestroy(Handle const watchHandle)
	{
		GD_DEBUG_VERIFY(watchHandle != nullptr);
		auto const watchHandleSystem = static_cast<Int32>(reinterpret_cast<IntPtr>(watchHandle));
		return close(watchHandleSystem) == 0;
	}

	/*!
	 * Reads all pending file system events of the specified watch.
	 *
	 * @param directoryName Path to the directory.
	 * @param directoryWatcherDelegate Watch event delegate.
	 *
	 * @returns True if directory exists or new watch was successfully added.
	 */
	GDINT bool LinuxPlatformDiskFileSystemWatcher::WatchReadEvents(Handle const watchHandle, IFileSystemWatcherDelegate& directoryWatcherDelegate)
	{
		GD_DEBUG_VERIFY(watchHandle != nullptr);
		INotifyEventContainer watchBuffer(1024);
		auto const watchHandleSystem = static_cast<Int32>(reinterpret_cast<IntPtr>(watchHandle));
		auto const numBytesRead = read(watchHandleSystem, watchBuffer.GetData(), watchBuffer.GetLength());
		if (numBytesRead != -1)
		{
			auto const lengthOfEvents = static_cast<SizeTp>(numBytesRead);
			for (auto watchEventIterator = watchBuffer.Begin(lengthOfEvents); watchEventIterator.IsValid(); ++watchEventIterator)
			{
				switch (watchEventIterator->mask & IN_ALL_EVENTS)
				{
					// File or directory was created.
					case IN_CREATE:
						{
							auto const watchEventPath = StringConv::DecodeUTF8(watchEventIterator->name);
							directoryWatcherDelegate.OnFileOrDirectoryCreated(watchEventPath);
							break;
						}
					// File or directory was removed from watch directory.
					case IN_DELETE:
						{
							auto const watchEventPath = StringConv::DecodeUTF8(watchEventIterator->name);
							directoryWatcherDelegate.OnFileOrDirectoryRemoved(watchEventPath);
							break;
						}
					// File or directory was modified.
					case IN_MODIFY:
						{
							auto const watchEventPath = StringConv::DecodeUTF8(watchEventIterator->name);
							directoryWatcherDelegate.OnFileOrDirectoryModified(watchEventPath);
							break;
						}
					// File or directory was renamed or moved from watch directory.
					case IN_MOVED_FROM:
						{
							auto const movedToEventIterator = watchEventIterator.FindNextRelated();
							if (movedToEventIterator.IsValid())
							{
								// File or directory was created or moved inside watch directory.
								auto const watchMovedFromEventPath = StringConv::DecodeUTF8(watchEventIterator->name);
								auto const watchMovedToEventPath = StringConv::DecodeUTF8(movedToEventIterator->name);
								directoryWatcherDelegate.OnFileOrDirectoryMoved(watchMovedFromEventPath, watchMovedToEventPath);
							}
							else
							{
								// File or directory was moved out from watch directory.
								auto const watchEventPath = StringConv::DecodeUTF8(watchEventIterator->name);
								directoryWatcherDelegate.OnFileOrDirectoryRemoved(watchEventPath);
							}
							break;
						}
					// File or directory was renamed or moved into watch directory.
					case IN_MOVED_TO:
						{
							auto const movedFromEventIterator = watchBuffer.Begin(lengthOfEvents).FindNextRelated(watchEventIterator);
							if (!movedFromEventIterator.IsValid())
							{
								// File or directory was moved into watch directory.
								auto const watchEventPath = StringConv::DecodeUTF8(watchEventIterator->name);
								directoryWatcherDelegate.OnFileOrDirectoryRemoved(watchEventPath);
							}
							break;
						}
					default:
						break;
				}
			}
			return true;
		}
		return false;
	}

GD_NAMESPACE_END

#endif  // if GD_PLATFORM_GNU_LINUX
