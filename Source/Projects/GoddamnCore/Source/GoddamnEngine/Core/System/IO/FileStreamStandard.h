// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/System/IO/FileStream.h
 * File contains C-standard IO file stream interfaces.
 */
#pragma once

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! C-standard implementation for file input stream.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class FileInputStreamStandard : public FileInputStreamGeneric
	{
	private:
		FILE* m_FileHandle;

	public:

		/*!
		 * Initializes the file input stream.
		 * @param filename Name of the file.
		 */
		GDINL explicit FileInputStreamStandard(WideCStr const filename)
			: FileInputStreamGeneric(filename), m_FileHandle(CStdio::Fopen(filename, L"rb"))
		{
		}

		GDINT virtual ~FileInputStreamStandard()
		{
			Close();
		}

	public:

		// ------------------------------------------------------------------------------------------
		// BaseStream interface.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Returns true if this stream is valid and ready for I/O operations.
		 */
		GDINT virtual bool IsValid() const override final
		{
			return m_FileHandle != nullptr;
		}

		// ------------------------------------------------------------------------------------------
		// InputStream interface.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Returns current position in stream.
		 */
		GDINT virtual SizeTp GetPosition() const override final
		{
			GD_ASSERT(IsValid(), "Getting position of the invalid stream.");
			return static_cast<SizeTp>(CStdio::Ftell(m_FileHandle));
		}

		/*!
		 * Returns size of data that stream handles.
		 */
		GDINT virtual SizeTp GetLength() const override final
		{
			GD_ASSERT(IsValid(), "Getting length of the invalid stream.");
			auto const fileSavedPos = GetPosition();
			if (fileSavedPos != SizeTpMax)
			{
				const_cast<FileInputStreamStandard*>(this)->Seek(0, SeekOrigin::End);
				auto const fileEndPos = GetPosition();
				const_cast<FileInputStreamStandard*>(this)->Seek(fileSavedPos, SeekOrigin::Beginning);
				return fileEndPos;
			}
			return SizeTpMax;
		}

		/*!
		 * Closes this stream and releases all resources associated with this stream.
		 */
		GDINT virtual void Close() override final
		{
			if (m_FileHandle != nullptr)
			{
				GD_ASSERT(CStdio::Fclose(m_FileHandle) == 0, "'CStdio::Fclose' has failed.");
				m_FileHandle = nullptr;
			}
		}

		/*!
		 * Reposition this stream to new specified position.
		 *
		 * @param offset The offset in bytes from specified origin.
		 * @param origin Defines origin from which point make offset.
		 *
		 * @returns New position in file, -1 on error.
		 */
		GDINT virtual SizeTp Seek(PtrDiffTp const offset, SeekOrigin const origin = SeekOrigin::Current) override final
		{
			GD_ASSERT(IsValid(), "Seeking in invalid stream.");
			return CStdio::Fseek(m_FileHandle, offset, origin) == 0 ? GetPosition() : -1;
		}

		/*!
		 * Reads next byte from input stream.
		 * @returns Read byte from stream, or -1 on the error.
		 */
		GDINT virtual Int16 Read() override final
		{
			GD_ASSERT(IsValid(), "Reading from invalid stream.");
			return static_cast<Int16>(CStdio::Getc(m_FileHandle));
		}

		/*!
		 * Reads several elements from input stream.
		 *
		 * @param array Output memory to which data would be written.
		 * @param size Length of the element in bytes,
		 * @param count Amount of the elements.
		 *
		 * @returns Total amount of elements read.
		 */
		GDINT virtual SizeTp Read(Handle const array, SizeTp const size, SizeTp const count) override final
		{
			GD_ASSERT(IsValid(), "Reading from invalid stream.");
			return CStdio::Fread(array, size, count, m_FileHandle);
		}

	};	// class FileInputStreamStandard

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! C-standard implementation for file output stream.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class FileOutputStreamStandard : public FileOutputStreamGeneric
	{
	private:
		FILE* m_FileHandle;

	public:

		/*!
		 * Initializes the file input stream.
		 *
		 * @param filename Name of the file.
		 * @param updateFile Do clear file or write to it's end.
		 */
		GDINL explicit FileOutputStreamStandard(WideCStr const filename, bool const updateFile = false)
			: FileOutputStreamGeneric(filename, updateFile), m_FileHandle(CStdio::Fopen(filename, updateFile ? L"ab" : L"wb"))
		{
		}

		GDINT virtual ~FileOutputStreamStandard()
		{
			Close();
		}

		// ------------------------------------------------------------------------------------------
		// BaseStream interface.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Returns true if this stream is valid and ready for I/O operations.
		 */
		GDINT virtual bool IsValid() const override final
		{
			return m_FileHandle != nullptr;
		}

		// ------------------------------------------------------------------------------------------
		// InputStream interface.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Closes this stream and releases all resources associated with this stream.
		 */
		GDINT virtual void Close() override final
		{
			if (m_FileHandle != nullptr)
			{
				GD_ASSERT(CStdio::Fclose(m_FileHandle) == 0, "'CStdio::Fclose' has failed.");
				m_FileHandle = nullptr;
			}
		}

		/*!
		 * Writes all unsaved to the resource.
		 */
		GDINT virtual void Flush() override final
		{
			GD_ASSERT(IsValid(), "Flushing invalid stream.");
			CStdio::Fflush(m_FileHandle);
		}

		/*!
		 * Writes a byte into output.
		 *
		 * @param byte Value that would be written to output.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool Write(Byte const byte) override final
		{
			GD_ASSERT(IsValid(), "Writing to invalid stream.");
			return CStdio::Putc(static_cast<Char>(byte), m_FileHandle) == byte;
		}

		/*!
		 * Writes several elements to output.
		 *
		 * @param array Input elements that would be written.
		 * @param size Length of the element in bytes,
		 * @param count Amount of the elements.
		 *
		 * @returns Total amount of elements written.
		 */
		GDINT virtual SizeTp Write(CHandle const array, SizeTp const size, SizeTp const count) override final
		{
			GD_ASSERT(IsValid(), "Writing to invalid stream.");
			return CStdio::Fwrite(array, size, count, m_FileHandle);
		}

	};	// class FileOutputStreamStandard

GD_NAMESPACE_END
