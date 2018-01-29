// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/CStdlib/CStdioMicrosoft.h
 * Wrappers, helper functions and definitions for standard IO functions (Microsoft-specific).
 */
#pragma once
#if !defined(GD_INSIDE_CSTDIO_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Core/CStdlib/CStdio.h> instead.
#endif	// if !defined(GD_INSIDE_CSTDIO_H)

#include <GoddamnEngine/Core/Containers/StringConv.h>

#include <cstdio>
#include <cwchar>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Provides Microsoft-specific functions for C IO.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class CStdioMicrosoft final : public CStdioGeneric
	{
	public:

		// ------------------------------------------------------------------------------------------
		// Filesystem functions.
		// ------------------------------------------------------------------------------------------

		/*! 
		 * @see @c "std::remove" function.
		 */
		GDINL static int Remove(WideCStr const filename)
		{
			return ::_wremove(filename);
		}

		/*! 
		 * @see @c "std::rename" function.
		 */
		GDINL static int Rename(WideCStr const oldFilename, WideCStr const newFilename)
		{
			return ::_wrename(oldFilename, newFilename);
		}

		// ------------------------------------------------------------------------------------------
		// File IO functions.
		// ------------------------------------------------------------------------------------------

		/*! 
		 * @see @c "std::fopen" function.
		 */
		//! @{
		GDINL static FILE* Fopen(CStr const filename, CStr const mode)
		{
			FILE* fileHandle = nullptr;
			auto const result = ::fopen_s(&fileHandle, filename, mode);
			GD_VERIFY(result == 0, "fopen_s failed.");
			return fileHandle;
		}
		GDINL static FILE* Fopen(WideCStr const filename, WideCStr const mode)
		{
			FILE* fileHandle = nullptr;
			auto const result = ::_wfopen_s(&fileHandle, filename, mode);
			GD_VERIFY(result == 0, "fopen_s failed.");
			return fileHandle;
		}
		//! @}

		/*! 
		 * @see @c "std::ftell" function.
		 */
		GDINL static Int64 Ftell(FILE* const file)
		{
			return ::_ftelli64_nolock(file);
		}

		/*! 
		 * @see @c "std::fseek" function.
		 */
		GDINL static int Fseek(FILE* const file, Int64 const offset, FseekOrigin const origin)
		{
			return ::_fseeki64_nolock(file, offset, static_cast<int>(origin));
		}

		/*! 
		 * @see @c "std::fread" function.
		 */
		//! @{
		GDINL static SizeTp Fread(Handle const bufferPtr, SizeTp const bufferSize, SizeTp const size  // NOLINT
			, SizeTp const count, FILE* const file)
		{
			return ::fread_s(bufferPtr, bufferSize, size, count, file);
		}
		GDINL static SizeTp Fread(Handle const bufferPtr, SizeTp const size, SizeTp const count, FILE* const file)  // NOLINT
		{
			return Fread(bufferPtr, size * count, size, count, file);
		}
		//! @}

		/*! 
		 * @see @c "std::getc" function.
		 */
		GDINL static int Getw(FILE* const file)
		{
			return ::_getw(file);
		}

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
			return ::_putw(character, file);
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// Temporary File IO functions.
		// ------------------------------------------------------------------------------------------

		/*! 
		 * @see @c "std::tmpfile" function.
		 */
		GDINL static FILE* Tmpfile()
		{
			FILE* temporaryFile = nullptr;
			auto const result = ::tmpfile_s(&temporaryFile);
			GD_DEBUG_VERIFY(result == 0, "tmpfile_s failed.");
			return temporaryFile;
		}

		/*! 
		 * @see @c "L_tmpnam" constant.
		 */
		SizeTp static const TmpnamBufferLength = static_cast<SizeTp>(L_tmpnam_s);

		/*! 
		 * @see @c "std::tmpnam" function.
		 */
		GDINL static void TmpnamSafe(Char* const temporaryPath, SizeTp const temporaryPathLength)
		{
			auto const result = ::tmpnam_s(temporaryPath, temporaryPathLength);
			GD_DEBUG_VERIFY(result == 0, "tmpnam_s failed.");
		}

	};	// class CStdioMicrosoft

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Provides cross-platform functions for C IO.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	using CStdio = CStdioMicrosoft;

GD_NAMESPACE_END
