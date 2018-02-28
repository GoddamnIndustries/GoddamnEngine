// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file
 * Streams implementation.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/CStdlib/CStdio.h>
#include <GoddamnEngine/Core/Templates/SharedPtr.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Input stream of bytes.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL IInputStream : public ReferenceTarget
	{
	public:

		/*!
		 * Closes the stream.
		 * @param resultPtr Pointer to the function call result. If no parameter presented and function fails it will throw.
		 */
		GDINT virtual void Close(bool* const resultPtr = nullptr) GD_PURE_VIRTUAL;

		/*!
		 * Reposition this stream to new specified position.
		 *
		 * @param offset The offset in bytes from specified origin.
		 * @param origin Defines origin from which point make offset.
		 * @param resultPtr Pointer to the function call result. If no parameter presented and function fails it will throw.
		 * 
		 * @returns New position in file.
		 */
		GDINT virtual UInt64 Seek(Int64 const offset, SeekOrigin const origin, bool* const resultPtr = nullptr) GD_PURE_VIRTUAL;

		/*!
		 * Reads data from stream.
		 * 
		 * @param readBuffer Output memory to which data would be written.
		 * @param readBufferSizeBytes Length of the element in bytes.
		 * @param resultPtr Pointer to the function call result. If no parameter presented and function fails it will throw.
		 * 
		 * @returns Amount of bytes that was read from stream.
		 */
		GDINT virtual UInt32 Read(Handle const readBuffer, UInt32 const readBufferSizeBytes, bool* const resultPtr = nullptr) GD_PURE_VIRTUAL;
	};	// class InputStream

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Represents an output stream of bytes.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL IOutputStream : public ReferenceTarget
	{
	public:

		/*!
		 * Closes the stream.
		 * @param resultPtr Pointer to the function call result. If no parameter presented and function fails it will throw.
		 */
		GDINT virtual void Close(bool* const resultPtr = nullptr) GD_PURE_VIRTUAL;

		/*!
		 * Reposition this stream to new specified position.
		 *
		 * @param offset The offset in bytes from specified origin.
		 * @param origin Defines origin from which point make offset.
		 * @param resultPtr Pointer to the function call result. If no parameter presented and function fails it will throw.
		 * 
		 * @returns New position in file.
		 */
		GDINT virtual UInt64 Seek(Int64 const offset, SeekOrigin const origin, bool* const resultPtr = nullptr) GD_PURE_VIRTUAL;

		/*!
		 * Writes data to stream.
		 * 
		 * @param writeBuffer Input memory that would be written.
		 * @param writeBufferSizeBytes Length of the element in bytes.
		 * @param resultPtr Pointer to the function call result. If no parameter presented and function fails it will throw.
		 * 
		 * @returns Amount of bytes that were written to stream.
		 */
		GDINT virtual UInt32 Write(CHandle const writeBuffer, UInt32 const writeBufferSizeBytes, bool* const resultPtr = nullptr) GD_PURE_VIRTUAL;
	};	// class IOutputStream

GD_NAMESPACE_END
