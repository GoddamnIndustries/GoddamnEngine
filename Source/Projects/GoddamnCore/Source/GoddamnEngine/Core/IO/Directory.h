// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/IO/Directory.h
 * File contains generic directory interface.
 */
#pragma once
#define GD_INSIDE_DIRECTORY_H

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/PlatformSpecificInclude.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Represents directory entry data.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	struct DirectoryEntry
	{
		WideCStr EntryName;
		bool EntryIsDirectory;
	};	// struct DirectoryEntry

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Generic directory visitor interface.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class DirectoryGeneric : public TNonCopyable
	{
	public:

		/*!
		 * Initializes a generic directory visitor.
		 * @param directoryPath Path to the directory.
		 */
		GDINL explicit DirectoryGeneric(WideCStr const directoryPath)
		{
			GD_DEBUG_VERIFY(directoryPath != nullptr, "Null pointer directory path specified.");
		}

		GDINL ~DirectoryGeneric()
		{
		}

	public:

		// ReSharper disable CppMemberFunctionMayBeConst
		// ReSharper disable CppMemberFunctionMayBeStatic

		/*!
		 * Returns true if this directory visitor is valid and ready for I/O operations.
		 */
		GDINT bool IsValid() const
		{
			GD_NOT_IMPLEMENTED();
		}

		/*!
		 * Closes this visitor and releases all resources associated with it.
		 */
		GDINT void Close()
		{
			GD_NOT_IMPLEMENTED();
		}

		/*!
		 * Reposition this visitor to the beginning of the directory.
		 */
		GDINL void Rewind()
		{
			GD_NOT_IMPLEMENTED();
		}

		/*!
		 * Reads next entry from the directory.
		 * @returns Next entry byte from stream, or value with null pointer path on error.
		 */
		GDINT DirectoryEntry ReadEntry()
		{
			GD_NOT_IMPLEMENTED();
		}

		// ReSharper restore CppMemberFunctionMayBeStatic
		// ReSharper restore CppMemberFunctionMayBeConst

	};	// class DirectoryGeneric

GD_NAMESPACE_END

#include GD_PLATFORM_INCLUDE(GoddamnEngine/Core/IO, Directory)
#undef GD_INSIDE_DIRECTORY_H
