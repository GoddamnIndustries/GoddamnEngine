// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Misc/CStdio.h
//! Traits, helper functions and definitions for standard IO functions.
#pragma once

#include <GoddamnEngine/Include.h>

#if GD_PLATFORM_API_MICROSOFT
struct DIR;
#else	// if GD_PLATFORM_API_MICROSOFT
#	include <dirent.h>
#endif	// if GD_PLATFORM_API_MICROSOFT

GD_NAMESPACE_BEGIN

	// ------------------------------------------------------------------------------------------
	//! Provides functions for C IO. Contains wrapped "*print*", "*scan*" and from "stdio.h".
	class CStdio final : public IUncreatable
	{
	public:
		// ... stdio.h's functions ...

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::remove" function.
		GDINL static int Remove(CStr const filename)
		{
			return ::remove(filename);
		}

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::rename" function.
		GDINL static int Rename(CStr const oldFilename, CStr const newFilename)
		{
			return ::rename(oldFilename, newFilename);
		}

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::tmpfile" function.
		GDINL static FILE* Tmpfile()
		{
#if GD_PLATFORM_API_MICROSOFT
			FILE* temporaryFile = nullptr;
			auto const result = ::tmpfile_s(&temporaryFile);
			GD_DEBUG_ASSERT(result == 0, "tmpfile_s failed.");
			return temporaryFile;
#else	// if GD_PLATFORM_API_MICROSOFT
			return ::tmpfile();
#endif	// if GD_PLATFORM_API_MICROSOFT
		}

		// ------------------------------------------------------------------------------------------
		//! @see @c "L_tmpnam" constant.
#if GD_PLATFORM_API_MICROSOFT
		SizeTp static const TmpnamBufferLength = static_cast<SizeTp>(L_tmpnam_s);
#else	// if GD_PLATFORM_API_MICROSOFT
		SizeTp static const TmpnamBufferLength = static_cast<SizeTp>(L_tmpnam);
#endif	// if GD_PLATFORM_API_MICROSOFT

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::tmpnam" function.
		GDINL static void Tmpnam(Char* const temporaryPath, SizeTp const temporaryPathLength)
		{
#if GD_PLATFORM_API_MICROSOFT
			auto const result = ::tmpnam_s(temporaryPath, temporaryPathLength);
			GD_DEBUG_ASSERT(result == 0, "tmpnam_s failed.");
#else	// if GD_PLATFORM_API_MICROSOFT
			GD_NOT_USED(temporaryPathLength);
			::tmpnam(temporaryPath);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::fopen" function.
		GDINL static FILE* Fopen(CStr const filename, CStr const mode)
		{
#if GD_PLATFORM_API_MICROSOFT
			FILE* fileHandle = nullptr;
			auto const result = ::fopen_s(&fileHandle, filename, mode);
			GD_DEBUG_ASSERT(result == 0, "fopen_s failed.");
			return fileHandle;
#else	// if GD_PLATFORM_API_MICROSOFT
			return ::fopen(filename, mode);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::fclose" function.
		GDINL static int Fclose(FILE* const file)
		{
			return ::fclose(file);
		}

		// ------------------------------------------------------------------------------------------
		//! Flushes file with the specified handle.
		//! @param file The file to flush.
		//! @returns Operation result.
		//! @see @c "std::fflush" function.
		GDINL static int Fflush(FILE* const file)
		{
			return ::fflush(file);
		}

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::ftell" function.
		GDINL static Int64 Ftell(FILE* const file)
		{
#if GD_PLATFORM_API_MICROSOFT
			return ::_ftelli64_nolock(file);
#else	// if GD_PLATFORM_API_MICROSOFT
			return static_cast<Int64>(::ftell(file));
#endif	// if GD_PLATFORM_API_MICROSOFT
		}

		// ------------------------------------------------------------------------------------------
		//! Enumeration for the last parameter for the @ref Fseek function.
		enum class FseekOrigin : decltype(SEEK_SET)
		{
			SeekSet = SEEK_SET,	//!< @see @c "SEEK_SET" constant.
			SeekCur = SEEK_CUR,	//!< @see @c "SEEK_CUR" constant.
			SeekEnd = SEEK_END,	//!< @see @c "SEEK_END" constant.
			Beginning = SeekSet,
			Current   = SeekCur,
			End       = SeekEnd,
		};	// enum FseekOrigin

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::fseek" function.
		GDINL static int Fseek(FILE* const file, Int64 const offset, FseekOrigin const origin)
		{
#if GD_PLATFORM_API_MICROSOFT
			return ::_fseeki64_nolock(file, offset, static_cast<int>(origin));
#else	// if GD_PLATFORM_API_MICROSOFT
			return ::fseek(file, static_cast<long>(offset), static_cast<int>(origin));
#endif	// if GD_PLATFORM_API_MICROSOFT
		}

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::getc" function.
		GDINL static int Getc(FILE* const file)
		{
			return ::getc(file);
		}

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::putc" function.
		GDINL static int Putc(Char const character, FILE* const file)
		{
			return ::putc(character, file);
		}

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::fread" function.
		//! @{
		GDINL static SizeTp Fread_s(Handle const bufferPtr, SizeTp const bufferSize, SizeTp const size
			, SizeTp const count, FILE* const file)
		{
#if GD_PLATFORM_API_MICROSOFT
			return ::fread_s(bufferPtr, bufferSize, size, count, file);
#else	// if GD_PLATFORM_API_MICROSOFT
			GD_NOT_USED(bufferSize);
			return ::fread(bufferPtr, size, count, file);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		GD_DEPRECATED("This function is insecure on several platforms.")
		GDINL static SizeTp Fread(Handle const bufferPtr, SizeTp const size, SizeTp const count, FILE* const file)
		{
			return Fread_s(bufferPtr, size * count, size, count, file);
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! @see @c "std::fwrite" function.
		GDINL static SizeTp Fwrite(Handle const bufferPtr, SizeTp const size, SizeTp const count, FILE* const file)
		{
			return ::fwrite(bufferPtr, size, count, file);
		}

		// ... dirent.h's functions ...

		// ------------------------------------------------------------------------------------------
		//! @see @c "::opendir" function.
#if GD_PLATFORM_API_MICROSOFT
		GDAPI static DIR* OpenDir(CStr const directoryPath);
#else	// if GD_PLATFORM_API_MICROSOFT
		GDINL static DIR* OpenDir(CStr const directoryPath)
		{
			return ::opendir(directoryPath);
		}
#endif	// if GD_PLATFORM_API_MICROSOFT

		// ------------------------------------------------------------------------------------------
		//! @see @c "::closedir" function.
#if GD_PLATFORM_API_MICROSOFT
		GDAPI static void CloseDir(DIR* const directory);
#else	// if GD_PLATFORM_API_MICROSOFT
		GDINL static void CloseDir(DIR* const directory)
		{
			::closedir(directory);
		}
#endif	// if GD_PLATFORM_API_MICROSOFT

		// ------------------------------------------------------------------------------------------
		//! @see @c "::closedir" function.
#if GD_PLATFORM_API_MICROSOFT
		GDAPI static CStr ReadDir(DIR* const directory);
#else	// if GD_PLATFORM_API_MICROSOFT
		GDINL static CStr ReadDir(DIR* const directory)
		{
			return ::readdir(directory)->d_name;
		}
#endif	// if GD_PLATFORM_API_MICROSOFT

	};	// class CStdio final

	using SeekOrigin = CStdio::FseekOrigin;

GD_NAMESPACE_END
