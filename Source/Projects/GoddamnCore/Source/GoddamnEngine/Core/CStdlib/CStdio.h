// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file
 * Wrappers, helper functions and definitions for standard IO functions.
 */
#pragma once
#define GD_INSIDE_CSTDIO_H

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Misc/StringConv.h>

#include <cstdio>
#include <cwchar>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Provides functions for C IO.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class CStdioGeneric : public TNonCreatable
	{
	public:

		// ------------------------------------------------------------------------------------------
		// Filesystem functions.
		// ------------------------------------------------------------------------------------------

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
			return ::remove(StringConv::EncodeUTF8(filename).CStr());
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
			return ::rename(StringConv::EncodeUTF8(oldFilename).CStr(), StringConv::EncodeUTF8(newFilename).CStr());
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// File IO functions.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Enumeration for the last parameter for the @ref Fseek function.
		 */
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
		 * @see @c "std::fopen" function.
		 */
		//! @{
		GDINL static FILE* Fopen(CStr const filename, CStr const mode)
		{
			GD_MSVC_PRAGMA(warning(push))
			GD_MSVC_PRAGMA(warning(suppress : 4996))
			return ::fopen(filename, mode);
			GD_MSVC_PRAGMA(warning(pop))
		}
		GDINL static FILE* Fopen(WideCStr const filename, WideCStr const mode)
		{
			GD_MSVC_PRAGMA(warning(push))
			GD_MSVC_PRAGMA(warning(suppress : 4996))
			return ::fopen(StringConv::EncodeUTF8(filename).CStr(), StringConv::EncodeUTF8(mode).CStr());
			GD_MSVC_PRAGMA(warning(pop))
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
			return static_cast<Int64>(::ftell(file));
		}

		/*! 
		 * @see @c "std::fseek" function.
		 */
		GDINL static int Fseek(FILE* const file, Int64 const offset, FseekOrigin const origin)
		{
			return ::fseek(file, static_cast<long>(offset), static_cast<int>(origin));
		}

		/*! 
		 * @see @c "std::fread" function.
		 */
		GDINL static SizeTp Fread(Handle const bufferPtr, SizeTp const bufferSize, SizeTp const size  // NOLINT
			, SizeTp const count, FILE* const file)
		{
			GD_NOT_USED(bufferSize);
			return ::fread(bufferPtr, size, count, file);
		}

		/*! 
		 * @see @c "std::fwrite" function.
		 */
		GDINL static SizeTp Fwrite(CHandle const bufferPtr, SizeTp const size, SizeTp const count, FILE* const file)
		{
			return ::fwrite(bufferPtr, size, count, file);
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
			GD_MSVC_PRAGMA(warning(push))
			GD_MSVC_PRAGMA(warning(suppress : 4996))
			return ::getw(file);
			GD_MSVC_PRAGMA(warning(pop))
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
			GD_MSVC_PRAGMA(warning(push))
			GD_MSVC_PRAGMA(warning(suppress : 4996))
			return ::putw(character, file);
			GD_MSVC_PRAGMA(warning(pop))
		}
		//! @}

		/*! 
		 * @see @c "std::puts" function.
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

		// ------------------------------------------------------------------------------------------
		// Temporary File IO functions.
		// ------------------------------------------------------------------------------------------

		/*! 
		 * @see @c "L_tmpnam" constant.
		 */
		SizeTp static const TmpnamBufferLength = static_cast<SizeTp>(L_tmpnam);

		/*! 
		 * @see @c "std::tmpfile" function.
		 */
		GDINL static FILE* Tmpfile()
		{
			GD_MSVC_PRAGMA(warning(push))
			GD_MSVC_PRAGMA(warning(suppress : 4996))
			return ::tmpfile();
			GD_MSVC_PRAGMA(warning(pop))
		}

		/*! 
		 * @see @c "std::tmpnam" function.
		 */
		GDINL static void TmpnamSafe(Char* const temporaryPath, SizeTp const temporaryPathLength)
		{
			GD_NOT_USED(temporaryPathLength);
			GD_MSVC_PRAGMA(warning(push))
			GD_MSVC_PRAGMA(warning(suppress : 4996))
			::tmpnam(temporaryPath);
			GD_MSVC_PRAGMA(warning(pop))
		}

	};	// class CStdioGeneric

	using SeekOrigin = CStdioGeneric::FseekOrigin;
	enum 
	{
		// ReSharper disable CppInconsistentNaming
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
		// ReSharper restore CppInconsistentNaming
	};	// enum

GD_NAMESPACE_END

#if GD_PLATFORM_API_MICROSOFT
#   include GD_PLATFORM_API_INCLUDE(GoddamnEngine/Core/CStdlib, CStdio)
#else   // if GD_PLATFORM_API_MICROSOFT

GD_NAMESPACE_BEGIN

    // **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
    //! @copydoc CStdioGeneric
    // **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
    using CStdio = CStdioGeneric;

GD_NAMESPACE_END

#endif  // if GD_PLATFORM_API_MICROSOFT
#undef GD_INSIDE_CSTDIO_H
