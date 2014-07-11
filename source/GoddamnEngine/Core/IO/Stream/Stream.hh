/// ==========================================================================================
/// Stream.hh - Base stream interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 29.04.2014 - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_IO_STREAM
#define GD_CORE_IO_STREAM

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/IO/IO.hh>
#include <GoddamnEngine/Core/Utility.hh>					
#include <GoddamnEngine/Core/Text/String/String.hh>	
#include <GoddamnEngine/Core/Containers/Vector/Vector.hh>

GD_NAMESPACE_BEGIN

	class InputStream;
	class OutputStream;

	/// Specifies the position in a stream to use for seeking.
	/// Streams are divided into Read-Only and Write-Only streams.
	enum class SeekOrigin : size_t
	{
		Begin,		///< Specifies beginning of stream.
		Current,	///< Specifies current position of stream.
		End			///< Specifies end of stream.
	};	// enum SeekOrigin

	/// Represents Input/Output stream basic functionality.
	class BaseStream
	{
		GD_CLASS_UNASSIGNABLE(BaseStream);
		GD_CLASS_UNCOPIABLE  (BaseStream);
	public/*Interface API*/:
		GDINL BaseStream() { }
		GDINL virtual ~BaseStream() { }

		/// Returns current position in stream.
		GDINT virtual size_t GetPosition() const abstract;

		/// Returns size of data that stream handles.
		GDINT virtual size_t GetSize() const abstract;
	};	// class BaseStream

	/// Base class for all classes representing an input stream of bytes.
	class InputStream : public BaseStream
	{
	public/*Interface API*/:
		GDINL InputStream() { }
		GDINL virtual ~InputStream() { }

		/// Closes this stream and releases all resources associated with this stream.
		GDINT virtual void Close() abstract;

		/// Reposition this stream to new specified position.
		/// @param Offset Offset in bytes from specified origin.
		/// @param Origin Defines origin from which point make offset.
		GDINT virtual void Seek(ptrdiff_t const Offset, SeekOrigin const Origin = SeekOrigin::Current) abstract;

		/// Reads next elemnt from input stream. Full endian abstraction is provided.
		/// @returns Read element from stream.
		GDINT virtual UInt8 Read() abstract;

		/// Reads several elements from input stream.
		/// @param Array  Output memory to which data would be written.
		/// @param Count  Length of one element
		/// @param Length Length of memory in elements.
		GDAPI virtual void Read(UInt8* const Array, size_t const Count, size_t const Length);
	};	// class InputStream

	/// Base class for all classes representing an output stream stream of bytes.
	class OutputStream : public BaseStream
	{
	public/*Interface API*/:
		GDINL OutputStream() { }
		GDINL virtual ~OutputStream() { }

		/// Closes this stream and releases all resources associated with this stream.
		GDINT virtual void Close() abstract;

		/// Writes all unsaved to the resource.
		GDINT virtual void Flush() abstract;

		/// Writes a byte into output.
		/// @param Byte Byte that would be written to output.
		GDINT virtual void Write(UInt8 const Byte) abstract;

		/// Writes several elements to output.
		/// @param Array  Input elements that would be written.
		/// @param Count  Length of one element
		/// @param Length Length of memory in elements.
		GDINL virtual void Write(UInt8 const* const Array, size_t const Count, size_t const Length);
	};	// class OutputStream

GD_NAMESPACE_END

#include <GoddamnEngine/Core/IO/Stream/Stream.inl>

#endif
