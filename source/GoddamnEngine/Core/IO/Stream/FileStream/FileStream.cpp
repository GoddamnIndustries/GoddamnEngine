/// ==========================================================================================
/// FileStream.cpp - File IO stream implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 18.06.2014 - Created by James Jhuighuy
/// ==========================================================================================

#include <GoddamnEngine/Core/IO/Stream/FileStream/FileStream.h>

#if (defined(GD_PLATFORM_API_WINAPI))
// Implemented using following:
// http://msdn.microsoft.com/en-us/library/1w06ktdy.aspx
#	include <io.h>
#	define access	(::_access)
#	define F_OK		(00)
#	define W_OK		(02)
#	define R_OK		(04)
#else	// if (defined(GD_PLATFORM_API_WINAPI))
#	if (defined(X_OK))
#		undef   X_OK	// X_OK is not supported on some platforms.
#	endif	// if (defined(X_OK))
#endif	// if (defined(GD_PLATFORM_API_WINAPI))

GD_NAMESPACE_BEGIN

	/// ==========================================================================================
	/// FileInputStream class.
	/// ==========================================================================================

	/// ------------------------------------------------------------------------------------------
	/// Class implementation API:
	/// ------------------------------------------------------------------------------------------

	FileInputStream::FileInputStream(String const& FilePath)
		: FilePath(FilePath)
	{
		if (access(this->FilePath.CStr(), F_OK) == -1) {
			throw FileNotFoundException(String::Format("File with path '%s' was not found on disk ('access(F_OK)' failed).", FilePath.CStr()));
		} else if (access(this->FilePath.CStr(), R_OK) == -1) {
			throw IOException(String::Format("File with path '%s' cannon be opened for reading ('access(R_OK)' failed).", FilePath.CStr()));
		}

		this->FileHandle = std::fopen(this->FilePath.CStr(), "rb");
		GD_DEBUG_ASSERT(this->FileHandle != nullptr, "File was validated with 'access' but 'fopen' has failed.");
	}

	FileInputStream::~FileInputStream()
	{
		if (this->FileHandle != nullptr) {
			this->Close();
		}
	}

	size_t FileInputStream::GetPosition() const
	{
		GD_DEBUG_ASSERT(this->FileHandle != nullptr, "Nullptr file handle.");
		long const Position = std::ftell(this->FileHandle);
		if (Position < 0l) {
			throw IOException("Failed to get position of stream ('ftell' returned negative value).");
		}

		return static_cast<size_t>(Position);
	}

	size_t FileInputStream::GetSize() const
	{
		if (this->FileLength == SIZE_MAX) {	// Finiding out position.
			FileInputStream* const MutableSelf = const_cast<FileInputStream*>(this);
			size_t const CurrentPosition = MutableSelf->GetPosition();
			MutableSelf->Seek(0, SeekOrigin::End);
			MutableSelf->FileLength = MutableSelf->GetPosition();
			MutableSelf->Seek(CurrentPosition, SeekOrigin::Begin);
		}

		return this->FileLength;
	}

	void FileInputStream::Close()
	{
		GD_DEBUG_ASSERT(this->FileHandle != nullptr, "Nullptr file handle.");
		if (std::fclose(this->FileHandle) != 0) {
			throw IOException("Failed to close file ('fclose' returned non-zero value).");
		}

		this->FilePath.Clear();
		this->FileHandle = nullptr;
		this->FileLength = SIZE_MAX;
	}

	void FileInputStream::Seek(ptrdiff_t const Offset, SeekOrigin const Origin /*= SeekOrigin::Current*/)
	{
		int static const SeekOriginTable[] = {
			/* SeekOrigin::Begin   = */ SEEK_SET,
			/* SeekOrigin::Current = */ SEEK_CUR,
			/* SeekOrigin::End     = */ SEEK_END
		};

		GD_DEBUG_ASSERT(this->FileHandle != nullptr, "Nullptr file handle.");
		int const StandartOrigin = SeekOriginTable[static_cast<size_t>(Origin)];
		if (std::fseek(this->FileHandle, static_cast<long>(Offset), StandartOrigin) != 0) {
			throw IOException("Failed to seek inside file ('fseek' returned non-zero value).");
		}
	}

	UInt8 FileInputStream::Read()
	{
		UInt8 Result = 0;
		this->Read(&Result, 1, sizeof(Result));
		return Result;
	}

	void FileInputStream::Read(handle const Array, size_t const Count, size_t const Length)
	{
		GD_DEBUG_ASSERT(Count != 0, "Zero elements count specified");
		GD_DEBUG_ASSERT(Length != 0, "Zero element length specified");
		GD_DEBUG_ASSERT(this->FileHandle != nullptr, "Nullptr file handle.");
		if (std::fread(Array, Length, Count, this->FileHandle) != Count) {
			throw IOException("Failed to read from file ('fread' returned invalid value).");
		}
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
		if (access(this->FilePath.CStr(), F_OK) != -1) {	// File exists.
			if (access(this->FilePath.CStr(), W_OK) == -1) {
				throw IOException(String::Format("File with path '%s' cannon be opened for writing ('access(W_OK)' failed).", FilePath.CStr()));
			}

			this->FileHandle = std::fopen(this->FilePath.CStr(), "wb");
			if (this->FileHandle == nullptr) {
				throw IOException("Failed to open existing file for writing (fopen(\"wb\") returned nullptr).");
			}
		} else {	// File does not exists.
			this->FileHandle = std::fopen(this->FilePath.CStr(), "wb");
			if (this->FileHandle == nullptr) {
				throw IOException("Failed to create new file for writing (fopen(\"wb\") returned nullptr).");
			}
		}
	}

	FileOutputStream::~FileOutputStream()
	{
		if (this->FileHandle != nullptr) {
			this->Close();
		}
	}

	size_t FileOutputStream::GetPosition() const
	{
		GD_DEBUG_ASSERT(this->FileHandle != nullptr, "Nullptr file handle.");
		long const Position = std::ftell(this->FileHandle);
		if (Position < 0l) {
			throw IOException("Failed to get position of stream ('ftell' returned negative value).");
		}
		return static_cast<size_t>(Position);
	}

	size_t FileOutputStream::GetSize() const
	{	// Since we just rewrite files, each output stream size is it's position + 1.
		return (this->GetPosition() + 1);
	}

	void FileOutputStream::Close()
	{
		GD_DEBUG_ASSERT(this->FileHandle != nullptr, "Nullptr file handle.");
		if (std::fclose(this->FileHandle) != 0) {
			throw IOException("Failed to close file ('fclose' returned non-zero value).");
		}

		this->FilePath.Clear();
		this->FileHandle = nullptr;
		this->FileLength = SIZE_MAX;
	}

	void FileOutputStream::Flush()
	{
		GD_DEBUG_ASSERT(this->FileHandle != nullptr, "Nullptr file handle.");
		if (std::fflush(this->FileHandle) != 0) {
			throw IOException("Failed to flush file ('fflush' returned non-zero value).");
		}
	}

	void FileOutputStream::Write(UInt8 const Byte)
	{
		this->Write(&Byte, 1, sizeof(Byte));
	}

	void FileOutputStream::Write(chandle const Array, size_t const Count, size_t const Length)
	{
		GD_DEBUG_ASSERT(Count != 0, "Zero elements count specified");
		GD_DEBUG_ASSERT(Length != 0, "Zero element length specified");
		GD_DEBUG_ASSERT(this->FileHandle != nullptr, "Nullptr file handle.");
		if (std::fwrite(Array, Length, Count, this->FileHandle) != Count) {
			throw IOException("Failed to write wo file ('fwrite' returned invalid value).");
		}
	}

GD_NAMESPACE_END
