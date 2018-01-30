// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file 
 * Wrappers, helper functions and definitions for standard memory functions (Microsoft-specific).
 */
#pragma once
#if !defined(GD_INSIDE_CMEMORY_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Core/CStdlib/CMemory.h> instead.
#endif	// if !defined(GD_INSIDE_CMEMORY_H)

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! @copydoc CMemoryGeneric
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class CMemoryMicrosoft final : public CMemoryGeneric
	{
	public:

		/*!
		 * @see @c "std::memcpy" function.
		 */
		//! @{
		GDINL static Handle Memcpy(Handle const dest, CHandle const source, SizeTp const sourceLength)
		{
			auto const result = ::memcpy_s(dest, sourceLength, source, sourceLength);
			GD_DEBUG_VERIFY(result == 0, "memcpy_s failed.");
			return dest;
		}
		GDINL static Char* CMemcpy(Char* const dest, CStr const source, SizeTp const sourceLength)
		{
			return static_cast<Char*>(Memcpy(dest, source, sourceLength));
		}
		GDINL static WideChar* CMemcpy(WideChar* const dest, WideCStr const source, SizeTp const sourceLength)
		{
			auto const result = ::wmemcpy_s(dest, sourceLength, source, sourceLength);
			GD_DEBUG_VERIFY(result == 0, "wmemcpy_s failed.");
			return dest;
		}
		//! @}

		/*!
		 * @see @c "std::memmove" function.
		 */
		//! @{
		GDINL static Handle Memmove(Handle const dest, CHandle const source, SizeTp const sourceLength)
		{
			auto const result = ::memmove_s(dest, sourceLength, source, sourceLength);
			GD_DEBUG_VERIFY(result == 0, "memcpy_s failed.");
			return dest;
		}
		GDINL static Char* CMemmove(Char* const dest, CStr const source, SizeTp const sourceLength)
		{
			return static_cast<Char*>(Memmove(dest, source, sourceLength));
		}
		GDINL static WideChar* CMemmove(WideChar* const dest, WideCStr const source, SizeTp const sourceLength)
		{
			auto const result = ::wmemmove_s(dest, sourceLength, source, sourceLength);
			GD_DEBUG_VERIFY(result == 0, "wmemmov_s failed.");
			return dest;
		}
		//! @}

	};	// class CMemoryMicrosoft

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! @copydoc CMemoryMicrosoft
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	using CMemory = CMemoryMicrosoft;

GD_NAMESPACE_END