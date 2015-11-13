//! ==========================================================================================
//! FileStream.cpp - File IO stream implementation.
//! Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
//! 
//! History:
///		* 18.06.2014 - Created by James Jhuighuy
//! ==========================================================================================

#define _CRT_SECURE_NO_WARNINGS
#include <GoddamnEngine/Core/IO/Stream/FileStream/FileStream.h>

#if GD_PLATFORM_API_MICROSOFT 
// Implemented using following:
// http://msdn.microsoft.com/en-us/library/1w06ktdy.aspx
#	include <io.h>
#	define access	_access
#	define F_OK		(00)
#	define W_OK		(02)
#	define R_OK		(04)
#else // if GD_PLATFORM_API_MICROSOFT

#	if (defined(X_OK))
#		undef   X_OK	// X_OK is not supported on some platforms.
#	endif // if (defined(X_OK))

#endif // if GD_PLATFORM_API_MICROSOFT


GD_NAMESPACE_BEGIN
	//! ==========================================================================================
	//! FileInputStream class.
	//! ==========================================================================================

	FileInputStream::FileInputStream(String const& filePath)
		: filePath(filePath)
	{
		if (::access(filePath.CStr(), F_OK) == -1)
		{
			throw FileNotFoundException(String::Format("File with path '%s' was not found on disk ('access(F_OK)' failed).", filePath.CStr()));
		}
		else if (::access(filePath.CStr(), R_OK) == -1)
		{
			throw IOException(String::Format("File with path '%s' cannon be opened for reading ('access(R_OK)' failed).", filePath.CStr()));
		}

		fileHandle = ::fopen(filePath.CStr(), "rb");
		GD_DEBUG_ASSERT(fileHandle != nullptr, "File was validated with 'access' but 'fopen' has failed.");
	}

	FileInputStream::~FileInputStream()
	{
		if (fileHandle != nullptr)
		{
			Close();
		}
	}

	SizeTp FileInputStream::GetPosition() const
	{
		GD_DEBUG_ASSERT(fileHandle != nullptr, "Nullptr file Handle.");
		long const Position = ::ftell(fileHandle);
		if (Position < 0l)
		{
			throw IOException("Failed to get position of stream ('ftell' returned negative value).");
		}

		return static_cast<SizeTp>(Position);
	}

	SizeTp FileInputStream::GetLength() const
	{
		if (fileLength == SizeTpMax)
		{ // Finding out position.
			FileInputStream* const MutableThis = const_cast<FileInputStream*>(this);
			SizeTp const CurrentPosition = MutableThis->GetPosition();
			MutableThis->Seek(0, SeekOrigin::End);
			MutableThis->fileLength = MutableThis->GetPosition();
			MutableThis->Seek(CurrentPosition, SeekOrigin::Beginning);
		}

		return fileLength;
	}

	void FileInputStream::Close()
	{
		GD_DEBUG_ASSERT(fileHandle != nullptr, "Nullptr file Handle.");
		if (::fclose(fileHandle) != 0)
		{
			throw IOException("Failed to close file ('fclose' returned non-zero value).");
		}

		filePath = "";
		fileHandle = nullptr;
		fileLength = SizeTpMax;
	}

	void FileInputStream::Seek(ptrdiff_t const Offset, SeekOrigin const Origin /*= SeekOrigin::Current*/)
	{
		int static const SeekOriginTable[] = {
			/* SeekOrigin::Begin   = */ SEEK_SET,
			/* SeekOrigin::Current = */ SEEK_CUR,
			/* SeekOrigin::End  = */ SEEK_END
		};

		GD_DEBUG_ASSERT(fileHandle != nullptr, "Nullptr file Handle.");
		int const StandartOrigin = SeekOriginTable[static_cast<SizeTp>(Origin)];
		if (::fseek(fileHandle, static_cast<long>(Offset), StandartOrigin) != 0)
		{
			throw IOException("Failed to seek inside file ('fseek' returned non-zero value).");
		}
	}

	Int16 FileInputStream::Read()
	{
		UInt8 Result = 0;
		Read(&Result, 1, sizeof(Result));
		return Result;
	}

	bool FileInputStream::Read(Handle const Array, SizeTp const Count, SizeTp const length)
	{
		GD_DEBUG_ASSERT(Count != 0, "Zero elements count specified");
		GD_DEBUG_ASSERT(length != 0, "Zero element m_Length specified");
		GD_DEBUG_ASSERT(fileHandle != nullptr, "Nullptr file Handle.");
		if (::fread(Array, length, Count, fileHandle) != Count)
		{
			throw IOException("Failed to read from file ('fread' returned invalid value).");
		}
		return true;
	}

	//! ==========================================================================================
	//! FileOutputStream class.
	//! Specifies write-only stream that provides writing to file.
	//! ==========================================================================================

	FileOutputStream::FileOutputStream(String const& filePath)
		: filePath(filePath)
	{
		if (::access(filePath.CStr(), F_OK) != -1)
		{ // File exists.
			if (::access(filePath.CStr(), W_OK) == -1)
			{
				throw IOException(String::Format("File with path '%s' cannon be opened for writing ('access(W_OK)' failed).", filePath.CStr()));
			}

			fileHandle = ::fopen(filePath.CStr(), "wb");
			if (fileHandle == nullptr)
			{
				throw IOException("Failed to open existing file for writing (fopen(\"wb\") returned nullptr).");
			}
		}
		else
		{ // File does not exists.
			fileHandle = ::fopen(filePath.CStr(), "wb");
			if (fileHandle == nullptr)
			{
				throw IOException("Failed to create new file for writing (fopen(\"wb\") returned nullptr).");
			}
		}
	}

	FileOutputStream::~FileOutputStream()
	{
		if (fileHandle != nullptr)
		{
			Close();
		}
	}

	SizeTp FileOutputStream::GetPosition() const
	{
		GD_DEBUG_ASSERT(fileHandle != nullptr, "Nullptr file Handle.");
		long const Position = ::ftell(fileHandle);
		if (Position < 0l)
		{
			throw IOException("Failed to get position of stream ('ftell' returned negative value).");
		}
		return static_cast<SizeTp>(Position);
	}

	SizeTp FileOutputStream::GetLength() const
	{ // Since we just rewrite files, each output stream size is it's position + 1.
		return (GetPosition() + 1);
	}

	void FileOutputStream::Close()
	{
		GD_DEBUG_ASSERT(fileHandle != nullptr, "Nullptr file Handle.");
		if (::fclose(fileHandle) != 0)
		{
			throw IOException("Failed to close file ('fclose' returned non-zero value).");
		}

		filePath = "";
		fileHandle = nullptr;
		fileLength = SizeTpMax;
	}

	void FileOutputStream::Flush()
	{
		GD_DEBUG_ASSERT(fileHandle != nullptr, "Nullptr file Handle.");
		if (::fflush(fileHandle) != 0)
		{
			throw IOException("Failed to flush file ('fflush' returned non-zero value).");
		}
	}

	bool FileOutputStream::Write(UInt8 const Byte)
	{
		return Write(&Byte, 1, sizeof(Byte));
	}

	bool FileOutputStream::Write(CHandle const Array, SizeTp const Count, SizeTp const length)
	{
		GD_DEBUG_ASSERT(Count != 0, "Zero elements count specified");
		GD_DEBUG_ASSERT(length != 0, "Zero element m_Length specified");
		GD_DEBUG_ASSERT(fileHandle != nullptr, "Nullptr file Handle.");
		if (::fwrite(Array, length, Count, fileHandle) != Count)
		{
			throw IOException("Failed to write to file ('fwrite' returned invalid value).");
		}
		return true;
	}

GD_NAMESPACE_END
