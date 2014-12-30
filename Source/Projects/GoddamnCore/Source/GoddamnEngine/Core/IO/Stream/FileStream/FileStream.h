/// ==========================================================================================
/// FileStream.h - File IO stream interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// @author James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_IO_STREAM_FILESTREAM
#define GD_CORE_IO_STREAM_FILESTREAM

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/IO/Stream/Stream.h>
#include <GoddamnEngine/Core/Containers/String.h>

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
		/// @brief				Opens input stream on file with specified file path.
		/// @param FilePath Path to file that is required to be read.
		GDAPI explicit			FileInputStream(String const& FilePath);

		/// @brief				Closes this stream.
		GDAPI virtual			~FileInputStream();

		/// @brief				Returns current position in stream.
		/// @returns			Current position in stream.
		GDINT virtual size_t	GetPosition() const override final;

		/// @brief				Returns size of data that stream handles.
		/// @returns			Size of data that stream handles.
		GDINT virtual size_t	GetLength() const override final;

		/// @brief				Closes this stream and releases all resources associated with this stream.
		GDINT virtual void		Close() override final;

		/// @brief				Reposition this stream to new specified position.
		/// @param Offset	Offset in bytes from specified origin.
		/// @param Origin	Defines origin from which point make offset.
		GDINT virtual void		Seek(ptrdiff_t const Offset, SeekOrigin const Origin = SeekOrigin::Current) override final;

		/// @brief				Reads next element from input stream. Full endian abstraction is provided.
		/// @returns			Read element from stream.
		GDINT virtual UInt8		Read() override final;

		/// @brief				Reads several elements from input stream.
		/// @param Array	Output memory to which data would be written.
		/// @param  Count	Length of one element
		/// @param  Length	Length of memory in elements.
		GDAPI virtual void		Read(handle const Array, size_t const Count, size_t const Length) override final;
	};	// class FileInputStream

	/// Specifies write-only stream that provides writing to file.
	class FileOutputStream final : public OutputStream
	{
	private /* Class members. */:
		String FilePath;
		FILE*  FileHandle = nullptr;
		size_t FileLength = 0;

	public:
		/// @brief				Opens output stream on file with specified file path.
		/// @param FilePath Path to file that is required to be written.
		GDAPI explicit FileOutputStream(String const& FilePath);

		/// @brief				Closes this stream.
		GDAPI virtual			~FileOutputStream();

		/// @brief				Returns current position in stream.
		/// @returns			Current position in stream.
		GDINT virtual size_t	GetPosition() const override final;

		/// @brief				Returns size of data that stream handles.
		/// @returns			Size of data that stream handles.
		GDINT virtual size_t	GetLength() const override final;

		/// @brief				Closes this stream and releases all resources associated with this stream.
		GDINT virtual void		Close() override final;

		/// @brief				Writes all unsaved to the resource.
		GDINT virtual void		Flush() override final;

		/// @brief				Writes a byte into output.
		/// @param			Byte Byte that would be written to output.
		GDINT virtual void		Write(UInt8 const Byte) override final;

		/// @brief				Writes several elements to output.
		/// @param Array	Input elements that would be written.
		/// @param Count	Length of one element.
		/// @param Length	Length of memory in elements.
		GDINL virtual void		Write(chandle const Array, size_t const Count, size_t const Length) override final;
	};	// class FileOutputStream

GD_NAMESPACE_END

#endif
