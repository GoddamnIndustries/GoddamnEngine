// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/IO/PathsWindows.h
 * File contains Windows paths helpers interface.
 */
#pragma once
#if !defined(GD_INSIDE_PATHS_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Core/IO/Paths.h> instead.
#endif	// if !defined(GD_INSIDE_PATHS_H)

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// Handles Windows global paths and contains helpers to work with paths.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class PathsWindows : public PathsGeneric
	{
	public:

		// ------------------------------------------------------------------------------------------
		// Helper functions.
		// ------------------------------------------------------------------------------------------

		GDAPI static WideString Normalize(WideString const& filename);

		GDAPI static WideString Platformize(WideString const& filename);

		GDAPI static WideString GetFullPath(WideString const& filename);

		GDAPI static WideString GetRelativePath(WideString const& filenameFrom, WideString const& filenameTo);

		// ------------------------------------------------------------------------------------------
		// Engine specific paths.
		// ------------------------------------------------------------------------------------------

		GDAPI static WideString const& GetEngineRootDirectory();

		GDAPI static WideString const& GetEngineSavedDataUserDirectory();

		// ------------------------------------------------------------------------------------------
		// Application specific paths.
		// ------------------------------------------------------------------------------------------

		GDAPI static WideString const& GetApplicationExecutableFile();

	};	// class PathsWindows

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// Handles global paths and contains helpers to work with paths.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	using Paths = PathsWindows;

GD_NAMESPACE_END
