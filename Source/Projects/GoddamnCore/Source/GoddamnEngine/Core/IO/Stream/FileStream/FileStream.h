// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/IO/FileStream.h
//! File IO stream interfaces.
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/IO/IO.h>
#include <GoddamnEngine/Core/IO/Stream/Stream.h>
#include <GoddamnEngine/Core/Containers/String.h>

GD_NAMESPACE_BEGIN

	// ------------------------------------------------------------------------------------------
	//! Read-only stream that provides reading from file.
	class FileInputStream final : public InputStream
	{
	private:
		String filePath;
		FILE*  fileHandle = nullptr;
		SizeTp fileLength = SizeTpMax;

	public:
		GDAPI explicit FileInputStream(String const& filePath);
		GDAPI virtual ~FileInputStream();

	public:

		// ------------------------------------------------------------------------------------------
		//! Returns true if this stream is valid and ready for IO.
		//! @returns True if this stream is valid and ready for IO.
		GDAPI virtual bool IsValid() const override final { return true; }

		// ------------------------------------------------------------------------------------------
		//! Returns current position in stream.
		//! @returns Current position in stream.
		GDAPI virtual SizeTp GetPosition() const override final;

		// ------------------------------------------------------------------------------------------
		//! Returns size of data that stream handles.
		//! @returns Size of data that stream handles.
		GDAPI virtual SizeTp GetLength() const override final;

		// ------------------------------------------------------------------------------------------
		//! Closes this stream and releases all resources associated with this stream.
		GDAPI virtual void Close() override final;

		// ------------------------------------------------------------------------------------------
		//! Reposition this stream to new specified position.
		//! @param offset The offset in bytes from specified origin.
		//! @param origin Defines origin from which point make offset.
		GDAPI virtual void Seek(ptrdiff_t const offset, SeekOrigin const origin = SeekOrigin::Current) override final;

		// ------------------------------------------------------------------------------------------
		//! Reads next element from input stream.
		//! @returns Read element from stream, or -1 on the error.
		GDAPI virtual Int16 Read() override final;

		// ------------------------------------------------------------------------------------------
		//! Reads several elements from input stream.
		//! @param array Output memory to which data would be written.
		//! @param count m_Length of single element.
		//! @param m_Length The m_Length of memory in elements.
		//! @returns True if operation succeeded.
		GDAPI virtual bool Read(Handle const array, SizeTp const count, SizeTp const length) override final;

	};	// class FileInputStream

	// ------------------------------------------------------------------------------------------
	//! Write-only stream that provides writing to file.
	class FileOutputStream final : public OutputStream
	{
	private:
		String filePath;
		FILE*  fileHandle = nullptr;
		SizeTp fileLength = 0;

	public:
		GDAPI explicit FileOutputStream(String const& filePath);
		GDAPI virtual ~FileOutputStream();

	public:

		// ------------------------------------------------------------------------------------------
		//! Returns true if this stream is valid and ready for IO.
		//! @returns True if this stream is valid and ready for IO.
		GDAPI virtual bool IsValid() const override final { return true; }

		// ------------------------------------------------------------------------------------------
		//! Returns current position in stream.
		//! @returns Current position in stream.
		GDAPI virtual SizeTp GetPosition() const override final;

		// ------------------------------------------------------------------------------------------
		//! Returns size of data that stream handles.
		//! @returns Size of data that stream handles.
		GDAPI virtual SizeTp GetLength() const override final;

		// ------------------------------------------------------------------------------------------
		//! Closes this stream and releases all resources associated with this stream.
		GDAPI virtual void Close() override final;

		// ------------------------------------------------------------------------------------------
		//! Writes all unsaved to the resource.
		GDAPI virtual void Flush() override final;

		// ------------------------------------------------------------------------------------------
		//! Writes a byte into output.
		//! @param Byte Value that would be written to output.
		//! @returns True if operation succeeded.
		GDAPI virtual bool Write(UInt8 const Byte) override final;

		// ------------------------------------------------------------------------------------------
		//!  Writes several elements to output.
		//! @param array Input elements that would be written.
		//! @param count m_Length of single element.
		//! @param m_Length The m_Length of memory in elements.
		//! @returns True if operation succeeded.
		GDAPI virtual bool Write(CHandle const array, SizeTp const count, SizeTp const length) override final;

	};	// class FileOutputStream

GD_NAMESPACE_END
