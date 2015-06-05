/// ==========================================================================================
/// Stream.h - Base stream interface.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// 
/// @author James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_IO_STREAM
#define GD_CORE_IO_STREAM

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/IO/IO.h>
#include <GoddamnEngine/Core/Templates/Utility.h>					
#include <GoddamnEngine/Core/Containers/String.h>	
#include <GoddamnEngine/Core/Containers/Vector.h>

GD_NAMESPACE_BEGIN

	class InputStream;
	class OutputStream;

	class IOException { public: IOException(...) { } };
	typedef IOException FileNotFoundException;

	/// Specifies the position in a stream to use for seeking.
	/// Streams are divided into Read-Only and Write-Only streams.
	enum class SeekOrigin : SizeTp
	{
		Begin,		///< Specifies beginning of stream.
		Current,	///< Specifies current position of stream.
		End			///< Specifies end of stream.
	};	// enum SeekOrigin

	/// Represents Input/Output stream basic functionality.
	class BaseStream : public IUncopiable
	{
	public:
		GDINL					BaseStream() { }
		GDINL virtual			~BaseStream() { }

		/// @brief				Returns current position in stream.
		/// @returns			Current position in stream.
		GDINT virtual SizeTp	GetPosition() const abstract;

		/// @brief				Returns size of data that stream Handles.
		/// @returns			Size of data that stream Handles.
		GDINT virtual SizeTp	GetLength() const abstract;
	};	// class BaseStream

	/// Base class for all classes representing an input stream of bytes.
	class InputStream : public BaseStream
	{
	public/*Interface API*/:
		GDINL					InputStream() { }
		GDINL virtual			~InputStream() { }

		/// @brief				Closes this stream and releases all resources associated with this stream.
		GDINT virtual void		Close() abstract;

		/// @brief				Reposition this stream to new specified position.
		/// @param Offset	Offset in bytes from specified origin.
		/// @param Origin	Defines origin from which point make offset.
		GDINT virtual void		Seek(PtrDiffTp const Offset, SeekOrigin const Origin = SeekOrigin::Current) abstract;

		/// @brief				Reads next element from input stream. Full endian abstraction is provided.
		/// @returns			Read element from stream.
		GDINT virtual UInt8		Read() abstract;

		/// @brief				Reads several elements from input stream.
		/// @param Array	Output memory to which data would be written.
		/// @param  Count	Length of one element
		/// @param  Length	Length of memory in elements.
		GDAPI virtual void		Read(Handle const Array, SizeTp const Count, SizeTp const Length);
	};	// class InputStream

	/// Base class for all classes representing an output stream stream of bytes.
	class OutputStream : public BaseStream
	{
	public:
		GDINL					OutputStream() { }
		GDINL virtual			~OutputStream() { }

		/// @brief				Closes this stream and releases all resources associated with this stream.
		GDINT virtual void		Close() abstract;

		/// @brief				Writes all unsaved to the resource.
		GDINT virtual void		Flush() abstract;

		/// @brief				Writes a byte into output.
		/// @param			Byte Byte that would be written to output.
		GDINT virtual void		Write(UInt8 const Byte) abstract;

		/// @brief				Writes several elements to output.
		/// @param Array	Input elements that would be written.
		/// @param Count	Length of one element.
		/// @param Length	Length of memory in elements.
		GDINL virtual void		Write(CHandle const Array, SizeTp const Count, SizeTp const Length);
	};	// class OutputStream

GD_NAMESPACE_END

#include <GoddamnEngine/Core/IO/Stream/Stream.inl>

#endif
