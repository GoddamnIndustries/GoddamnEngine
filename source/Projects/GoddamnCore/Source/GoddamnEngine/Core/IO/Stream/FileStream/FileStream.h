/// ==========================================================================================
/// FileStream.h - File IO stream interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 18.06.2014 - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_IO_STREAM_FILESTREAM
#define GD_CORE_IO_STREAM_FILESTREAM

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/IO/Stream/Stream.h>
#include <GoddamnEngine/Core/Text/String/String.h>

GD_NAMESPACE_BEGIN

	/// Represents runtime error that indicates that file with specified path does not exists.
	GD_DEFINE_EXCPETION_DERIVED(FileNotFoundException, IOException);

	/// Specifies read-only stream that provides reading from file.
	class FileInputStream final : public InputStream
	{
	private /* Class members. */:
		String FilePath;
		FILE*  FileHandle = nullptr;
		size_t FileLength = SIZE_MAX;

	public /*Class implementation API*/:
		/// Opens input stream on file with specifed file path.
		/// @param FilePath Path to file that is required to be read.
		GDAPI explicit FileInputStream(String const& FilePath);
		GDAPI virtual ~FileInputStream();

		/// @see BaseStream::GetPosition()
		GDAPI virtual size_t GetPosition() const override final;

		/// @see BaseStream::GetLength()
		GDAPI virtual size_t GetLength() const override final;

		/// @see InputStream::Close()
		GDAPI virtual void Close() override final;

		/// @see InputStream::Seek(ptrdiff_t const Offset, SeekOrigin const Origin = SeekOrigin::Current)
		GDAPI virtual void Seek(ptrdiff_t const Offset, SeekOrigin const Origin = SeekOrigin::Current) override final;

		/// @see InputStream::Read()
		GDAPI virtual UInt8 Read() override final;

		/// @see InputStream::Read(handle const Array, size_t const Count, size_t const Length)
		GDAPI virtual void Read(handle const Array, size_t const Count, size_t const Length) override final;
	};	// class FileInputStream

	/// Specifies write-only stream that provides writing to file.
	class FileOutputStream final : public OutputStream
	{
	private /* Class members. */:
		String FilePath;
		FILE*  FileHandle = nullptr;
		size_t FileLength = 0;

	public /*Class implementation API*/:
		/// Opens output stream on file with specifed file path.
		/// @param FilePath Path to file that is required to be written.
		GDAPI explicit FileOutputStream(String const& FilePath);
		GDAPI virtual ~FileOutputStream();

		/// @see BaseStream::GetPosition()
		GDAPI virtual size_t GetPosition() const override final;

		/// @see BaseStream::GetLength()
		GDAPI virtual size_t GetLength() const override final;

		/// @see OutputStream::Close()
		GDAPI virtual void Close() override final;

		/// @see OutputStream::Flush()
		GDAPI virtual void Flush() override final;

		/// @see OutputStream::Write(UInt8 const Byte)
		GDAPI virtual void Write(UInt8 const Byte) override final;

		/// @see OutputStream::Write(chandle const Array, size_t const Count, size_t const Length)
		GDAPI virtual void Write(chandle const Array, size_t const Count, size_t const Length) override final;
	};	// class FileOutputStream

GD_NAMESPACE_END

#endif
