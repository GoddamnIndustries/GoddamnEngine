//////////////////////////////////////////////////////////////////////////
/// Path.hh - Path management utility interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 27.04.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_CORE_IO_PATH
#define GD_CORE_IO_PATH

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/Text/String/String.hh>

GD_NAMESPACE_BEGIN
	
	/// Performs operations on strings that contain information about path to directory or file.
	/// All performed operations are cross platform.
	namespace Path
	{
		/// Changes extension of path string into new one or appends it if no extension specified.
		GDAPI extern String&& ChangeExtension(String const& SomePath, String const& NewExtension);

		/// Combines list of paths into one.
		GDAPI extern String&& Combine(Vector<String> const& Paths);

		/// Returns true if specified path contains extension.
		GDAPI extern bool HasExtension(String const& SomePath);

		/// Returns directory information for specified path string.
		GDAPI extern String&& GetDirectoryName(String const& SomePath);

		/// Returns extension information for specified path string.
		GDAPI extern String&& GetExtension(String const& SomePath);

		/// Returns file and extension information for specified path string.
		GDAPI extern String&& GetFileName(String const& SomePath);

		/// Returns file information for specified path string (without extension).
		GDAPI extern String&& GetFileNameWithoutExtension(String const& SomePath);

		/// Creates a uniquely named, zero-byte temporary file on disk and returns the full path of that file.
		GDAPI extern String&& GetTemporaryFileName();

		/// Returns path to system`s temporary folder.
		GDAPI extern String const& GetTemporaryPath();
	};	// namespace Path

GD_NAMESPACE_END

#endif
