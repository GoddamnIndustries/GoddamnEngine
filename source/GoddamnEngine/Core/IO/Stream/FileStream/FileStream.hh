//////////////////////////////////////////////////////////////////////////
/// FileStream.hh - File IO stream interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 18.06.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_CORE_IO_STREAM_FILESTREAM_FILESTREAM
#define GD_CORE_IO_STREAM_FILESTREAM_FILESTREAM

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/IO/Stream/Stream.hh>

#include <cstdio>

GD_NAMESPACE_BEGIN

	/// Specifies read-only stream that provides reading from file.
	class FileInputStream final : public InputStream
	{
	private /* Class members. */:
		FILE*  FileHandle = nullptr;
		size_t FileLength = 0;

	public /* Constructor / Destructor */:
		GDINL virtual ~FileInputStream(                      )                                            { self->Dispose(); }
		GDINL explicit FileInputStream(String const& FilePath) : FileHandle(fopen(FilePath.CStr(), "rb")) 
		{
			if (self->FileHandle != nullptr)
			{
				fseek(self->FileHandle, 0, GD_SEEK_ORIGIN_END);
				self->FileLength = size_t(ftell(self->FileHandle));
				fseek(self->FileHandle, 0, SEEK_SET);
			}
		}

	public /* Class implemented API */:
		GDINL virtual size_t GetLength  () const final { return   self->FileLength; }
		GDINL virtual size_t GetPosition() const final { return ((self->FileHandle != nullptr) ? size_t(ftell(self->FileHandle)) : SIZE_MAX); }

		GDINL virtual void Dispose() final
		{
			if (self->FileHandle != nullptr)
				fclose(self->FileHandle);
			self->FileHandle = nullptr;
			self->FileLength = 0;
		}

		GDINL virtual bool CanSeek() const
		{
			if (!self->InputStream::CanSeek())
				return false;

			return (self->FileHandle != nullptr);
		}

		inline virtual size_t Seek(ptrdiff_t const Offset, SeekOrigin const Origin) final
		{
			int static const GD2STDOrigin[] = {
				/*GD_SEEK_ORIGIN_BEGIN   */ SEEK_SET,
				/*GD_SEEK_ORIGIN_CURRENT */ SEEK_CUR,
				/*GD_SEEK_ORIGIN_END     */ SEEK_END,
			};

			return ((self->FileHandle != nullptr) ? fseek(self->FileHandle, static_cast<long>(Offset), GD2STDOrigin[Origin]) : SIZE_MAX);
		}

		GDINL virtual bool CanRead() const 
		{ 
			if (!self->InputStream::CanRead())
				return false;

			return (self->FileHandle != nullptr); 
		}

		inline virtual size_t Read(handle const OutputBuffer, ptrdiff_t const Offset, size_t const Count) final
		{
			return ((self->FileHandle != nullptr) ? fread(OutputBuffer, 1, Count, self->FileHandle) : 0);
		}
	};	// class FileInputStream

	/// Specifies write-only stream that provides writing to file.
	class FileOutputStream final : public OutputStream
	{
	private /* Class members. */:
		String const FilePath;
		FILE*  FileHandle = nullptr;
		size_t FileLength = 0;

	public /* Constructor / Destructor */:
		GDINL explicit FileOutputStream(String const& FilePath) : FilePath(FilePath) { self->Flush(); }
		GDINL virtual ~FileOutputStream(                      )                      { self->Dispose(); }

	public /* Class implemented API */:
		GDINL virtual size_t GetLength  () const final { return   self->FileLength; }
		GDINL virtual size_t GetPosition() const final { return ((self->FileHandle != nullptr) ? size_t(ftell(self->FileHandle)) : SIZE_MAX); }

		GDINL virtual void Dispose() final
		{
			if (self->FileHandle != nullptr)
				fclose(self->FileHandle);
		}

		inline virtual void Flush() final
		{
			self->Dispose();
			self->FileHandle = fopen(self->FilePath.CStr(), "wb");
		}

		GDINL virtual bool CanWrite() const
		{
			if (!self->OutputStream::CanWrite())
				return false;

			return (self->FileHandle != nullptr);
		}

		inline virtual size_t Write(chandle const InputBuffer, ptrdiff_t const Offset, size_t const Count) final
		{
			if (self->FileHandle != nullptr)
			{
				UInt8  const* String = reinterpret_cast<UInt8 const*>(InputBuffer) + Offset;
				size_t const NumElementsWritten = fwrite(String, 1, Count, self->FileHandle);
				self->FileLength += NumElementsWritten;
				return NumElementsWritten;
			}

			return 0;
		}

	};	// class FileOutputStream

GD_NAMESPACE_END

#endif
