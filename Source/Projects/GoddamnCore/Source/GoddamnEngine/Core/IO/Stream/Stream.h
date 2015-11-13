// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/IO/Stream.h
//! IO stream interfaces.
#pragma once

#include <GoddamnEngine/Include.h>

GD_NAMESPACE_BEGIN

	class InputStream;
	class OutputStream;

	// ------------------------------------------------------------------------------------------
	//! Represents Input/Output stream basic functionality.
	//! Streams are divided into Read-Only and Write-Only streams.
	class BaseStream : public IUncopiable
	{
	public:
		GDINL BaseStream() { }
		GDINL virtual ~BaseStream() { }

		// ------------------------------------------------------------------------------------------
		//! Returns true if this stream is valid and ready for IO.
		//! @returns True if this stream is valid and ready for IO.
		GDAPI virtual bool IsValid() const GD_PURE_VIRTUAL;

		// ------------------------------------------------------------------------------------------
		//! Returns current position in stream.
		//! @returns Current position in stream.
		GDAPI virtual SizeTp GetPosition() const GD_PURE_VIRTUAL;

		// ------------------------------------------------------------------------------------------
		//! Returns size of data that stream handles.
		//! @returns Size of data that stream handles.
		GDAPI virtual SizeTp GetLength() const GD_PURE_VIRTUAL;
	};	// class BaseStream

	// ------------------------------------------------------------------------------------------
	//! Base class for all classes representing an input stream of bytes.
	class InputStream : public BaseStream
	{
	public:

		// ------------------------------------------------------------------------------------------
		//! Closes this stream and releases all resources associated with this stream.
		GDAPI virtual void Close() GD_PURE_VIRTUAL;

		// ------------------------------------------------------------------------------------------
		//! Reposition this stream to new specified position.
		//! @param offset The offset in bytes from specified origin.
		//! @param origin Defines origin from which point make offset.
		GDAPI virtual void Seek(ptrdiff_t const offset, SeekOrigin const origin = SeekOrigin::Current) GD_PURE_VIRTUAL;

		// ------------------------------------------------------------------------------------------
		//! Reads next element from input stream.
		//! @returns Read element from stream, or -1 on the error.
		GDAPI virtual Int16 Read() GD_PURE_VIRTUAL;

		// ------------------------------------------------------------------------------------------
		//! Reads several elements from input stream.
		//! @param array Output memory to which data would be written.
		//! @param count m_Length of single element.
		//! @param m_Length The m_Length of memory in elements.
		//! @returns True if operation succeeded.
		GDAPI virtual bool Read(Handle const array, SizeTp const count, SizeTp const length)
		{
			auto const theArray = static_cast<UInt8*>(array);
			for (auto bytePtr = theArray; bytePtr < theArray + count * length; ++bytePtr)
			{
				auto const byte = Read();
				if (byte == -1)
					return false;
				*bytePtr = static_cast<UInt8>(byte);
			}
			return true;
		}
	};	// class InputStream

	// ------------------------------------------------------------------------------------------
	//! Base class for all classes representing an output stream of bytes.
	class OutputStream : public BaseStream
	{
	public:

		// ------------------------------------------------------------------------------------------
		//! Closes this stream and releases all resources associated with this stream.
		GDAPI virtual void Close() GD_PURE_VIRTUAL;

		// ------------------------------------------------------------------------------------------
		//! Writes all unsaved to the resource.
		GDAPI virtual void Flush() GD_PURE_VIRTUAL;

		// ------------------------------------------------------------------------------------------
		//! Writes a byte into output.
		//! @param Byte Value that would be written to output.
		//! @returns True if operation succeeded.
		GDAPI virtual bool Write(UInt8 const Byte) GD_PURE_VIRTUAL;

		// ------------------------------------------------------------------------------------------
		//!  Writes several elements to output.
		//! @param array Input elements that would be written.
		//! @param count m_Length of single element.
		//! @param m_Length The m_Length of memory in elements.
		//! @returns True if operation succeeded.
		GDAPI virtual bool Write(CHandle const array, SizeTp const count, SizeTp const length)
		{
			auto const theArray = static_cast<UInt8 const*>(array);
			for (auto bytePtr = theArray; bytePtr < theArray + count * length; ++bytePtr) 
			{
				if (!Write(*bytePtr)) 
					return false;
			}
			return true;
		}

	};	// class OutputStream

GD_NAMESPACE_END
