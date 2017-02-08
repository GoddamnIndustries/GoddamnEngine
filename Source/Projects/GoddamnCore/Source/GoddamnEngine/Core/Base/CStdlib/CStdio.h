// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Base/CStdlib/CStdio.h
 * @note This file should be never directly included, please consider using <GoddamnEngine/Include.h> instead.
 * Wrappers, helper functions and definitions for standard IO functions.
 */
#pragma once
#if !defined(GD_INSIDE_INCLUDE_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Include.h> instead.
#endif	// if !defined(GD_INSIDE_INCLUDE_H)

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Provides functions for C IO. Contains wrapped "*print*", "*scan*" and from "stdio.h".
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class CStdio final : public TNonCreatable
	{
	public:
		// ... stdio.h's functions ...

		/*! 
		 * @see @c "std::remove" function.
		 */
		 //! @{
		GDINL static int Remove(CStr const filename)
		{
			return ::remove(filename);
		}
		GDINL static int Remove(WideCStr const filename)
		{
#if GD_PLATFORM_API_MICROSOFT
			return ::_wremove(filename);
#else	// if GD_PLATFORM_API_MICROSOFT
			return ::wremove(filename);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		//! @}

		/*! 
		 * @see @c "std::rename" function.
		 */
		//! @{
		GDINL static int Rename(CStr const oldFilename, CStr const newFilename)
		{
			return ::rename(oldFilename, newFilename);
		}
		GDINL static int Rename(WideCStr const oldFilename, WideCStr const newFilename)
		{
#if GD_PLATFORM_API_MICROSOFT
			return ::_wrename(oldFilename, newFilename);
#else	// if GD_PLATFORM_API_MICROSOFT
			return ::wrename(oldFilename, newFilename);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		//! @}

		/*! 
		 * @see @c "std::tmpfile" function.
		 */
		GDINL static FILE* Tmpfile()
		{
#if GD_PLATFORM_API_MICROSOFT
			FILE* temporaryFile = nullptr;
			auto const result = ::tmpfile_s(&temporaryFile);
			GD_DEBUG_VERIFY(result == 0, "tmpfile_s failed.");
			return temporaryFile;
#else	// if GD_PLATFORM_API_MICROSOFT
			return ::tmpfile();
#endif	// if GD_PLATFORM_API_MICROSOFT
		}

		/*! 
		 * @see @c "L_tmpnam" constant.
		 */
#if GD_PLATFORM_API_MICROSOFT
		SizeTp static const TmpnamBufferLength = static_cast<SizeTp>(L_tmpnam_s);
#else	// if GD_PLATFORM_API_MICROSOFT
		SizeTp static const TmpnamBufferLength = static_cast<SizeTp>(L_tmpnam);
#endif	// if GD_PLATFORM_API_MICROSOFT

		/*! 
		 * @see @c "std::tmpnam" function.
		 */
		GDINL static void Tmpnam_s(Char* const temporaryPath, SizeTp const temporaryPathLength)
		{
#if GD_PLATFORM_API_MICROSOFT
			auto const result = ::tmpnam_s(temporaryPath, temporaryPathLength);
			GD_DEBUG_VERIFY(result == 0, "tmpnam_s failed.");
#else	// if GD_PLATFORM_API_MICROSOFT
			GD_NOT_USED(temporaryPathLength);
			::tmpnam(temporaryPath);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}

		/*! 
		 * @see @c "std::fopen" function.
		 */
		//! @{
		GDINL static FILE* Fopen(CStr const filename, CStr const mode)
		{
#if GD_PLATFORM_API_MICROSOFT
			FILE* fileHandle = nullptr;
			auto const result = ::fopen_s(&fileHandle, filename, mode);
			GD_DEBUG_VERIFY(result == 0, "fopen_s failed.");
			return fileHandle;
#else	// if GD_PLATFORM_API_MICROSOFT
			return ::fopen(filename, mode);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		GDINL static FILE* Fopen(WideCStr const filename, WideCStr const mode)
		{
#if GD_PLATFORM_API_MICROSOFT
			FILE* fileHandle = nullptr;
			auto const result = ::_wfopen_s(&fileHandle, filename, mode);
			GD_DEBUG_VERIFY(result == 0, "fopen_s failed.");
			return fileHandle;
#else	// if GD_PLATFORM_API_MICROSOFT
			return ::wfopen(filename, mode);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		//! @}

		/*! 
		 * @see @c "std::fclose" function.
		 */
		GDINL static int Fclose(FILE* const file)
		{
			return ::fclose(file);
		}

		/*! 
		 * @see @c "std::fflush" function.
		 */
		GDINL static int Fflush(FILE* const file)
		{
			return ::fflush(file);
		}

		/*! 
		 * @see @c "std::ftell" function.
		 */
		GDINL static Int64 Ftell(FILE* const file)
		{
#if GD_PLATFORM_API_MICROSOFT
			return ::_ftelli64_nolock(file);
#else	// if GD_PLATFORM_API_MICROSOFT
			return static_cast<Int64>(::ftell(file));
#endif	// if GD_PLATFORM_API_MICROSOFT
		}

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

		/*! 
		 * @see @c "std::fseek" function.
		 */
		GDINL static int Fseek(FILE* const file, Int64 const offset, FseekOrigin const origin)
		{
#if GD_PLATFORM_API_MICROSOFT
			return ::_fseeki64_nolock(file, offset, static_cast<int>(origin));
#else	// if GD_PLATFORM_API_MICROSOFT
			return ::fseek(file, static_cast<long>(offset), static_cast<int>(origin));
#endif	// if GD_PLATFORM_API_MICROSOFT
		}

		/*! 
		 * @see @c "std::getc" function.
		 */
		 //! @{
		GDINL static int Getc(FILE* const file)
		{
			return ::getc(file);
		}
		GDINL static int Getw(FILE* const file)
		{
#if GD_PLATFORM_API_MICROSOFT
			return ::_getw(file);
#else	// if GD_PLATFORM_API_MICROSOFT
			return ::getw(file);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		//! @}

		/*! 
		 * @see @c "std::putc" function.
		 */
		//! @{
		GDINL static int Putc(Char const character, FILE* const file)
		{
			return ::putc(character, file);
		}
		GDINL static int Putc(WideChar const character, FILE* const file)
		{
#if GD_PLATFORM_API_MICROSOFT
			return ::_putw(character, file);
#else	// if GD_PLATFORM_API_MICROSOFT
			return ::putw(character, file);
#endif	// if GD_PLATFORM_API_MICROSOFT
		}
		//! @}

		/*! 
		 * @see @c "std::putc" function.
		 */
		//! @{
		GDINL static int Puts(CStr const string, FILE* const file)
		{
			return ::fputs(string, file);
		}
		GDINL static int Puts(WideCStr const string, FILE* const file)
		{
			return ::fputws(string, file);
		}
		//! @}

		/*! 
		 * @see @c "std::fread" function.
		 */
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
		GDINL static SizeTp Fread(Handle const bufferPtr, SizeTp const size, SizeTp const count, FILE* const file)
		{
			return Fread_s(bufferPtr, size * count, size, count, file);
		}
		//! @}

		/*! 
		 * @see @c "std::fwrite" function.
		 */
		GDINL static SizeTp Fwrite(CHandle const bufferPtr, SizeTp const size, SizeTp const count, FILE* const file)
		{
			return ::fwrite(bufferPtr, size, count, file);
		}

	};	// class CStdio final

	using SeekOrigin = CStdio::FseekOrigin;

	/*!
	 * Declarations used to ban standard functions. 
	 */
	enum LibIOUnallowedFunctions
	{
		remove, _wremove, wremove,
		rename, _wrename, wrename,
		tmpfile_s, tmpfile,
		tmpnam_s, tmpnam,
		fopen_s, fopen, _wfopen_s, wfopen,
		fclose,
		fflush,
		_ftelli64_nolock, ftell,
		_fseeki64_nolock, fseek,
		getc, _getw, getw,
		putc, _putw, putw,
		fputs, fputws,
		fread_s, fread,
		fwrite
	};	// enum LibIOUnallowedFunctions

GD_NAMESPACE_END
