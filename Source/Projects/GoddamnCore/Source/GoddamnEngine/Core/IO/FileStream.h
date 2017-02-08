// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/IO/FileStream.h
 * File contains IO file stream interfaces.
 */
#pragma once
#define GD_INSIDE_FILESTREAM_H

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/BasePlatform.h>
#include <GoddamnEngine/Core/IO/Stream.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Generic implementation for file input stream.
	//! Use this class this directly only inside the trusted code.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class FileInputStreamGeneric : public InputStream
	{
	public:

		/*!
		 * Initializes the file input stream.
		 * @param filename Name of the file.
		 */
		GDINL explicit FileInputStreamGeneric(WideCStr const filename)
		{
			GD_DEBUG_VERIFY(filename != nullptr, "Null pointer file name specified.");
		}

	};	// class FileInputStreamGeneric

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Generic implementation for file output stream.
	//! Use this class this directly only inside the trusted code.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class FileOutputStreamGeneric : public OutputStream
	{
	public:

		/*!
		 * Initializes the file input stream.
		 *
		 * @param filename Name of the file.
		 * @param updateFile Do clear file or write to it's end.
		 */
		GDINL explicit FileOutputStreamGeneric(WideCStr const filename, bool const updateFile = false)
		{
			GD_DEBUG_VERIFY(filename != nullptr, "Null pointer file name specified.");
			GD_NOT_USED(updateFile);
		}

	};	// class FileOutputStreamGeneric

GD_NAMESPACE_END

#include GD_PLATFORM_INCLUDE(GoddamnEngine/Core/IO, FileStream)
#undef GD_INSIDE_FILESTREAM_H
