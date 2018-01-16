// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/IO/Archive.h
 * File contains archive interfaces.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/SharedPtr.h>
#include <GoddamnEngine/Core/IO/Stream.h>
#include <GoddamnEngine/Core/Containers/String.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Archive interface.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class Archive : public IVirtuallyDestructible
	{
	public:

		/*!
		 * Returns true of this archive is valid.
		 */
		GDAPI virtual bool IsValid() const GD_PURE_VIRTUAL;

	};	// class Archive

	class ArchiveReader : public Archive
	{
	public:

		/*!
		 * Closes this archive and releases all resources associated with this archive.
		 */
		GDAPI virtual void Close() GD_PURE_VIRTUAL;

		/*!
		 * @brief Opens the input stream that can read from this archive.
		 * Each file could be opened for reading multiple times.
		 *
		 * @param fileName Name of the file being opened.
		 * @returns Pointer to opened stream or nullptr on error.
		 */
		GDAPI virtual SharedPtr<InputStream> OpenInputStream(WideString const& fileName) GD_PURE_VIRTUAL;

	};	// class ArchiveReader

	class ArchiveWriter : public Archive
	{
		
		/*!
		 * @brief Opens the output stream that can write to this archive.
		 * Each file could be opened for writing only once, reading from this file would be not supported.
		 * If not file exists, it would be created. 
		 *
		 * @param fileName Name of the file being opened.
		 * @param error Pointer to error code if opening fails.
		 *
		 * @returns Pointer to opened stream or nullptr on error.
		 */
		GDAPI virtual SharedPtr<OutputStream> OpenOutputStream(WideCStr const fileName) GD_PURE_VIRTUAL;

	};	// class ArchiveWriter

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Global archive manager.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class ArchiveManager final : public TNonCreatable
	{
	public:

		/*! 
		 * @brief Opens archive with specified filename.
		 * If not archive such archive exists, it would be created.
		 *
		 * @param fileName Path to archive. Relative to the engine content folder.
		 * @returns Pointer to opened stream or nullptr on error.
		 */
		GDAPI static SharedPtr<Archive> OpenArchive(WideCStr const fileName, UInt32 archiveVersion = 1);

	};	// class ArchiveManager

GD_NAMESPACE_END
