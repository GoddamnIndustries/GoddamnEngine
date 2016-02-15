// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/System/IO/FileSystemWatcher.h
 * File contains generic file system watcher interface.
 */
#pragma once
#define GD_INSIDE_FILESYSTEMWATCHER_H

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/System/Platform.h>
#include <GoddamnEngine/Core/System/Threading/CriticalSection.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Containers/Vector.h>

// This API is used to implement hot reloading, which is generally enabled
// for Editor. So, there is no need for this API for non-desktop platforms.
#if GD_PLATFORM_DESKTOP
#	if !GD_PLATFORM_HAS_MULTITHREADING
#		error FileSystemWatcher API requires mutithreading support.
#	endif	// if !GD_PLATFORM_HAS_MULTITHREADING

#include <functional>

GD_NAMESPACE_BEGIN

	template<typename Tp>
	using Function = ::std::function<Tp>;

	// **------------------------------------------------------------------------------------------**
	//! Defines what types of events watcher handlers would handle.
	// **------------------------------------------------------------------------------------------**
	//! @{
	enum class FileSystemWatcherNotifyFilters : UInt8
	{
		Changed = GD_BIT(0),	//!< Notifies if file or directory was changed.
		Created = GD_BIT(1),	//!< Notifies if file or directory was created.
		Renamed = GD_BIT(2),	//!< Notifies if file or directory was renamed.
		Deleted = GD_BIT(3),	//!< Notifies if file or directory was deleted.
		All		= UInt8Max,	
		None	= 0,
	};	// enum class FileSystemWatcherNotifyFilters
	GD_ENUM_DEFINE_FLAG_OPERATORS(FileSystemWatcherNotifyFilters);
	//! @}

	// **------------------------------------------------------------------------------------------**
	//! Defines what types of events that have happened.
	// **------------------------------------------------------------------------------------------**
	typedef FileSystemWatcherNotifyFilters FileSystemWatcherEventAction;

	// **------------------------------------------------------------------------------------------**
	//! File system event.
	// **------------------------------------------------------------------------------------------**
	struct FileSystemWatcherEvent final : public TNonCopyable
	{
		FileSystemWatcherEventAction Action;	
		WideString Name;
		WideString NewName;
	};	// struct FileSystemWatcherEvent

	// **------------------------------------------------------------------------------------------**
	//! File system event handler.
	// **------------------------------------------------------------------------------------------**
	typedef Function<void(FileSystemWatcherEvent const&)> FileSystemWatcherHandler;

	// **------------------------------------------------------------------------------------------**
	//! Generic file system watcher.
	// **------------------------------------------------------------------------------------------**
	class FileSystemWatcherGeneric : public TNonCopyable
	{
		friend class FileSystemWatcherThread;

	protected:
		struct HandlerInst
		{
			FileSystemWatcherHandler Function;
			FileSystemWatcherNotifyFilters Filters;
		};	// struct HandlerInst
		Vector<HandlerInst> m_Handlers;
		CriticalSection     m_HandlersLock;

	public:

		/*!
		 * Initializes a new file system watcher.
		 *
		 * @param directoryName Path to the directory to watch.
		 * @param filter Wildcard for controlled file types.
		 * @param recursive Do recursively watch subdirectories.
		 */
		GDAPI explicit FileSystemWatcherGeneric(WideString const& directoryName, WideString const& filter = L"", bool const recursive = true);

		GDAPI ~FileSystemWatcherGeneric();

	public:

		/*!
		 * Adds a new handler to the watcher.
		 * 
		 * @param hander Function that handles file system events.
		 * @param notifyFilters Defines what types of events this handler would handle.
		 */
		GDINL void AddHandler(FileSystemWatcherHandler const& handler, FileSystemWatcherNotifyFilters const notifyFilters = FileSystemWatcherNotifyFilters::All)
		{
			GD_DEBUG_ASSERT(handler != nullptr, "Null pointer handler was specified.");
			GD_DEBUG_ASSERT(notifyFilters != FileSystemWatcherNotifyFilters::None, "Empty notification filters were specified.");
			ScopedCriticalSection handlersLock(m_HandlersLock);
			m_Handlers.InsertLast({ handler, notifyFilters });
		}

	private:

		/*!
		 * Updates the watcher state and notifies the handlers.
		 * Would called from separate thread once per second.
		 */
		// ReSharper disable once CppMemberFunctionMayBeStatic
		// ReSharper disable once CppMemberFunctionMayBeConst
		GDINL void OnUpdate()
		{
			GD_NOT_IMPLEMENTED();
		}

	};	// class FileSystemWatcherGeneric

GD_NAMESPACE_END

#include GD_PLATFORM_INCLUDE(GoddamnEngine/Core/System/IO, FileSystemWatcher)
#undef GD_INSIDE_FILESYSTEMWATCHER_H

#endif	// if GD_PLATFORM_DESKTOP 
