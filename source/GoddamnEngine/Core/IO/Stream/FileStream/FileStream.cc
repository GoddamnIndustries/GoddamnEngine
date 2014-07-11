/// ==========================================================================================
/// FileStream.cc - File IO stream implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 18.06.2014 - Created by James Jhuighuy
/// ==========================================================================================

#include <GoddamnEngine/Core/IO/Stream/FileStream/FileStream.hh>

#if (defined(GD_PLATFORM_WINAPI))
// Implemented using following:
// http://msdn.microsoft.com/en-us/library/1w06ktdy.aspx
#	include <io.h>
#	define access	(::_access)
#	define F_OK		(00)
#	define W_OK		(02)
#	define R_OK		(04)
#else	// if (defined(GD_PLATFORM_WINAPI))
#	if (defined(X_OK))
#		undef   X_OK	// X_OK is not supported on some platforms.
#	endif	// if (defined(X_OK))
#endif	// if (defined(GD_PLATFORM_WINAPI))

GD_NAMESPACE_BEGIN

	/// ==========================================================================================
	/// FileInputStream class.
	/// Specifies read-only stream that provides reading from file.
	/// ==========================================================================================

	/// ------------------------------------------------------------------------------------------
	/// Class implementation API:
	/// ------------------------------------------------------------------------------------------

	/// Opens input stream on file with specifed file path.
	/// @param FilePath Path to file that is required to be read.
	FileInputStream::FileInputStream(String const& FilePath)
		: FilePath(FilePath)
	{
		if (access(self->FilePath.CStr(), F_OK) == -1)
			throw FileNotFoundException(String::Format("File with path '%s' was not found on disk ('access(F_OK)' failed).", FilePath.CStr()));
		if (access(self->FilePath.CStr(), R_OK) == -1)
			throw IOException(String::Format("File with path '%s' cannon be opened for reading ('access(R_OK)' failed).", FilePath.CStr()));
		
		self->FileHandle = ::fopen(self->FilePath.CStr(), "rb");
		GD_DEBUG_ASSERT(self->FileHandle != nullptr, "File was validated with 'access' but 'fopen' has failed.");
	}

	FileInputStream::~FileInputStream()
	{
		self->Close();
	}

	/// @see BaseStream::GetPosition()
	size_t FileInputStream::GetPosition() const
	{
		GD_DEBUG_ASSERT(self->FileHandle != nullptr, "Nullptr file handle.");
		long const Position = ::ftell(self->FileHandle);
		if (Position < 0l)
			throw IOException("Failed to get position of stream ('ftell' returned negative value).");

		return static_cast<size_t>(Position);
	}

	/// @see BaseStream::GetSize()
	size_t FileInputStream::GetSize() const
	{
		if (self->FileLength == SIZE_MAX)
		{	// Finiding out position.
			FileInputStream* const MutableSelf = const_cast<FileInputStream*>(self);
			size_t const CurrentPosition = MutableSelf->GetPosition();
			MutableSelf->Seek(0, SeekOrigin::End);
			MutableSelf->FileLength = MutableSelf->GetPosition();
			MutableSelf->Seek(CurrentPosition, SeekOrigin::Begin);
		}

		return self->FileLength;
	}

	/// @see InputStream::Close()
	void FileInputStream::Close()
	{
		GD_DEBUG_ASSERT(self->FileHandle != nullptr, "Nullptr file handle.");
		if (::fclose(self->FileHandle) != 0)
			throw IOException("Failed to close file ('fclose' returned non-zero value).");

		self->FilePath.Emptify();
		self->FileHandle = nullptr;
		self->FileLength = SIZE_MAX;
	}

	/// @see InputStream::Seek(ptrdiff_t const Offset, SeekOrigin const Origin)
	void FileInputStream::Seek(ptrdiff_t const Offset, SeekOrigin const Origin /*= SeekOrigin::Current*/)
	{
		int static const SeekOriginTable[] = {
			/* SeekOrigin::Begin   = */ SEEK_SET,
			/* SeekOrigin::Current = */ SEEK_CUR,
			/* SeekOrigin::End     = */ SEEK_END
		};

		GD_DEBUG_ASSERT(self->FileHandle != nullptr, "Nullptr file handle.");
		int const StandartOrigin = SeekOriginTable[static_cast<size_t>(Origin)];
		if (::fseek(self->FileHandle, static_cast<long>(Offset), StandartOrigin) != 0)
			throw IOException("Failed to seek inside file ('fseek' returned non-zero value).");
	}

	/// @see InputStream::Read()
	UInt8 FileInputStream::Read()
	{
		UInt8 Result = 0;
		self->Read(&Result, 1, sizeof(Result));
		return Result;
	}

	/// @see InputStream::Read(UInt8* const Array, size_t const Count, size_t const Length)
	void FileInputStream::Read(UInt8* const Array, size_t const Count, size_t const Length)
	{
		GD_DEBUG_ASSERT(self->FileHandle != nullptr, "Nullptr file handle.");
		if (::fread(Array, Length, Count, self->FileHandle) != Count)
			throw IOException("Failed to read from file ('fread' returned invalid value).");
	}

	/// ==========================================================================================
	/// FileOutputStream class.
	/// Specifies write-only stream that provides writing to file.
	/// ==========================================================================================

	/// ------------------------------------------------------------------------------------------
	/// Class implementation API:
	/// ------------------------------------------------------------------------------------------

	FileOutputStream::FileOutputStream(String const& FilePath)
		: FilePath(FilePath)
	{
		if (access(self->FilePath.CStr(), F_OK) != -1)
		{	// File exists.
			if (access(self->FilePath.CStr(), W_OK) == -1)
				throw IOException(String::Format("File with path '%s' cannon be opened for writing ('access(W_OK)' failed).", FilePath.CStr()));
			self->FileHandle = ::fopen(self->FilePath.CStr(), "wb");
			if (self->FileHandle == nullptr)
				throw IOException("Failed to open existing file for writing (fopen(\"wb\") returned nullptr).");
		}
		else
		{	// File does not exists.
			self->FileHandle = ::fopen(self->FilePath.CStr(), "wb");
			if (self->FileHandle == nullptr)
				throw IOException("Failed to create new file for writing (fopen(\"wb\") returned nullptr).");
		}
	}

	FileOutputStream::~FileOutputStream()
	{
		self->Close();
	}

	/// @see BaseStream::GetPosition()
	size_t FileOutputStream::GetPosition() const
	{
		GD_DEBUG_ASSERT(self->FileHandle != nullptr, "Nullptr file handle.");
		long const Position = ::ftell(self->FileHandle);
		if (Position < 0l)
			throw IOException("Failed to get position of stream ('ftell' returned negative value).");

		return static_cast<size_t>(Position);
	}

	/// @see BaseStream::GetSize()
	size_t FileOutputStream::GetSize() const
	{	// Since we just rewrite files, each output stream size is it's position + 1.
		return (self->GetPosition() + 1);
	}

	/// @see OutputStream::Close()
	void FileOutputStream::Close()
	{
		GD_DEBUG_ASSERT(self->FileHandle != nullptr, "Nullptr file handle.");
		if (::fclose(self->FileHandle) != 0)
			throw IOException("Failed to close file ('fclose' returned non-zero value).");

		self->FilePath.Emptify();
		self->FileHandle = nullptr;
		self->FileLength = SIZE_MAX;
	}

	/// @see OutputStream::Flush()
	void FileOutputStream::Flush()
	{
		GD_DEBUG_ASSERT(self->FileHandle != nullptr, "Nullptr file handle.");
		if (::fflush(self->FileHandle) != 0)
			throw IOException("Failed to flush file ('fflush' returned non-zero value).");
	}

	/// @see OutputStream::Write(UInt8 const Byte)
	void FileOutputStream::Write(UInt8 const Byte)
	{
		self->Write(&Byte, 1, sizeof(Byte));
	}

	/// @see OutputStream::Write(UInt8 const* const Array, size_t const Count, size_t const Length)
	void FileOutputStream::Write(UInt8 const* const Array, size_t const Count, size_t const Length)
	{
		GD_DEBUG_ASSERT(self->FileHandle != nullptr, "Nullptr file handle.");
		if (::fwrite(Array, Length, Count, self->FileHandle) != Count)
			throw IOException("Failed to write wo file ('fwrite' returned invalid value).");
	}

GD_NAMESPACE_END
