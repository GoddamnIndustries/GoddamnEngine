// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/System/IO/FileUtilitiesWindows.h
 * File contains file and directory utilities Windows interface.
 */
#pragma once
#if !defined(GD_INSIDE_FILEUTILITIES_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Core/System/IO/FileUtilities.h> instead.
#endif	// if !defined(GD_INSIDE_FILEUTILITIES_H)

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Windows file and directory utilities interface.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class FileUtilitiesWindows : public FileUtilitiesGeneric
	{
	public:

		// ------------------------------------------------------------------------------------------
		// File utilities.
		// ------------------------------------------------------------------------------------------

		GDAPI static bool FileExists(WideString const& filename);

		GDAPI static UInt64 FileSize(WideString const& filename);

		GDAPI static bool FileCreateEmpty(WideString const& filename);

		GDAPI static bool FileRemove(WideString const& filename);

		GDAPI static bool FileMove(WideString const& sourceFilename, WideString const& destFilename, bool const doOverwrite = false);

		GDAPI static bool FileCopy(WideString const& sourceFilename, WideString const& destFilename, bool const doOverwrite = false);

		// ------------------------------------------------------------------------------------------
		// Directory utilities.
		// ------------------------------------------------------------------------------------------

		GDAPI static bool DirectoryExists(WideString const& directoryName);

		GDAPI static bool DirectoryCreateEmpty(WideString const& directoryName);

		GDAPI static bool DirectoryRemove(WideString const& directoryName);

	};	// class FileUtilitiesWindows

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! File and directory utilities interface.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	using FileUtilities = FileUtilitiesWindows;

GD_NAMESPACE_END
