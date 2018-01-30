// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file
 * Traits, helper functions and definitions for standard string/memory functions (Microsoft-specific).
 */
#pragma once
#if !defined(GD_INSIDE_CSTRING_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Core/CStdlib/CString.h> instead.
#endif	// if !defined(GD_INSIDE_CSTRING_H)

#include <GoddamnEngine/Core/Templates/Algorithm.h>
#include <Windows.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! @copydoc CStringGeneric
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class CStringMicrosoft final : public CStringGeneric
	{
	public:

		// ------------------------------------------------------------------------------------------
		// String copy functions.
		// ------------------------------------------------------------------------------------------

		/*!
		 * @see @c "std::strcpy" function.
		 */
		//! @{
		GDINL static CStr Strcpy(Char* const dest, SizeTp const destLength, CStr const source)
		{
			auto const result = ::strcpy_s(dest, destLength, source);
			GD_DEBUG_VERIFY(result == 0, "strcpy_s failed.");
			return dest;
		}
		GDINL static WideCStr Strcpy(WideChar* const dest, SizeTp const destLength, WideCStr const source)
		{
			auto const result = ::wcscpy_s(dest, destLength, source);
			GD_DEBUG_VERIFY(result == 0, "wcscpy_s failed.");
			return dest;
		}
		//! @}

		/*!
		 * @see @c "std::strncpy" function.
		 */
		//! @{
		GDINL static CStr Strncpy(Char* const dest, SizeTp const destLength, CStr const source, SizeTp const maxCount)
		{
			auto const result = ::strncpy_s(dest, destLength, source, maxCount);
			GD_DEBUG_VERIFY(result == 0, "strncpy_s failed.");
			return dest;
		}
		GDINL static WideCStr Strncpy(WideChar* const dest, SizeTp const destLength, WideCStr const source, SizeTp const maxCount)
		{
			auto const result = ::wcsncpy_s(dest, destLength, source, maxCount);
			GD_DEBUG_VERIFY(result == 0, "wcsncpy_s failed.");
			return dest;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// String concatenation functions.
		// ------------------------------------------------------------------------------------------

		/*!
		 * @see @c "std::strcat" function.
		 */
		//! @{
		GDINL static CStr Strcat(Char* const dest, SizeTp const destLength, CStr const source)
		{
			auto const result = ::strcat_s(dest, destLength, source);
			GD_DEBUG_VERIFY(result == 0, "strcat_s failed.");
			return dest;
		}
		GDINL static WideCStr Strcat(WideChar* const dest, SizeTp const destLength, WideCStr const source)
		{
			auto const result = ::wcscat_s(dest, destLength, source);
			GD_DEBUG_VERIFY(result == 0, "wcscat_s failed.");
			return dest;
		}
		//! @}

		/*!
		 * @see @c "std::strncat" function.
		 */
		//! @{
		GDINL static CStr Strncat(Char* const dest, SizeTp const destLength, CStr const source, SizeTp const maxCount)
		{
			auto const result = ::strncat_s(dest, destLength, source, maxCount);
			GD_DEBUG_VERIFY(result == 0, "strncat_s failed.");
			return dest;
		}
		GDINL static WideCStr Strncat(WideChar* const dest, SizeTp const destLength, WideCStr const source, SizeTp const maxCount)
		{
			auto const result = ::wcsncat_s(dest, destLength, source, maxCount);
			GD_DEBUG_VERIFY(result == 0, "wcsncat_s failed.");
			return dest;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// String conversion functions.
		// ------------------------------------------------------------------------------------------

		/*!
		 * @see @c "std::wcstombs/::mbstowcs" function.
		 */
		GDINL static SizeTp Wcstombs(Char* const dest, SizeTp const destLength, WideChar const* const source, SizeTp const maxCount)
		{
			SizeTp output = 0;
			auto const result = ::wcstombs_s(&output, dest, destLength, source, maxCount);
			GD_DEBUG_VERIFY(result == 0, "wcstombs_s failed.");
			return output;
		}

		/*!
		 * @see @c "::mbstowcs" function.
		 */
		GDINL static SizeTp Strtowcs(WideChar* const dest, SizeTp const destLength, CStr const source, SizeTp const maxCount)
		{
			SizeTp output = 0;
			auto const result = ::mbstowcs_s(&output, dest, destLength, source, maxCount);
			GD_DEBUG_VERIFY(result == 0, "mbstowcs_s failed.");
			return output;
		}

		/*!
		 * @see @c "std::strtoull" function.
		 */
		//! @{
		GDINL static UInt64 Strtoui64(CStr const cstr, Char** const endPtr = nullptr, SizeTp const base = 0)
		{
			return ::_strtoui64(cstr, endPtr, static_cast<int>(base));
		}
		GDINL static UInt64 Strtoui64(WideChar const* const cstr, WideChar** const endPtr = nullptr, SizeTp const base = 0)
		{
			return ::_wcstoui64(cstr, endPtr, static_cast<int>(base));
		}
		//! @}

		/*!
		 * @see @c "std::strtoll" function.
		 */
		//! @{
		GDINL static Int64 Strtoi64(CStr const cstr, CStr* const endPtr = nullptr, SizeTp const base = 0)
		{
			return ::_strtoi64(cstr, const_cast<char**>(endPtr), static_cast<int>(base));
		}
		GDINL static Int64 Strtoi64(WideChar const* const cstr, WideChar const** const endPtr = nullptr, SizeTp const base = 0)
		{
			return ::_wcstoi64(cstr, const_cast<wchar_t**>(endPtr), static_cast<int>(base));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// String formatting functions.
		// ------------------------------------------------------------------------------------------

		/*!
		 * @see @c "std::vsnprintf" function.
		 */
		//! @{
		GDINL static Int32 Vsnprintf(Char* const dest, SizeTp const destLength, CStr const format, va_list arguments)
		{
			// size_t inside _TRUNCATE macro.
			GD_MSVC_PRAGMA(warning(push))
			GD_MSVC_PRAGMA(warning(suppress : 4996))
			return ::_vsnprintf_s(dest, destLength, _TRUNCATE, format, arguments);	// NOLINT
			GD_MSVC_PRAGMA(warning(pop))
		}
		GDINL static Int32 Vsnprintf(WideChar* const dest, SizeTp const destLength, WideCStr const format, va_list arguments)
		{
			// size_t inside _TRUNCATE macro.
			GD_MSVC_PRAGMA(warning(push))
			GD_MSVC_PRAGMA(warning(suppress : 4996))
			return ::_vsnwprintf_s(dest, destLength, _TRUNCATE, format, arguments);	// NOLINT
			GD_MSVC_PRAGMA(warning(pop))
		}
		//! @}

		/*!
		 * @see @c "std::vsscanf" function.
		 */
		//! @{
		GDINL static Int32 Vsscanf(CStr const source, CStr const format, va_list arguments)	// NOLINT
		{
			return ::vsscanf_s(source, format, arguments);
		}
		GDINL static Int32 Vsscanf(WideCStr const source, WideCStr const format, va_list arguments)	// NOLINT
		{
			return ::vswscanf_s(source, format, arguments);
		}
		//! @}

	};	// class CStringMicrosoft

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! @copydoc CStringMicrosoft
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	using CString = CStringMicrosoft;

GD_NAMESPACE_END
