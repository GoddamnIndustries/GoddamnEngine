// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/CStdlib/CMemoryMicrosoft.h
 * @note This file should be never directly included, please consider using <GoddamnEngine/Core/CStdlib/CMemory.h> instead.
 * Wrappers, helper functions and definitions for standard memory functions (Microsoft-specific).
 */
#pragma once
#if !defined(GD_INSIDE_CMEMORY_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Core/CStdlib/CMemory.h> instead.
#endif	// if !defined(GD_INSIDE_CMEMORY_H)

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Provides functions for C memory. Contains wrapped "mem*" methods and methods from "string.h",
	//! "stdlib.h" and etc.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class CMemoryMicrosoft final : public CMemoryGeneric
	{
	public:

		// ------------------------------------------------------------------------------------------
		// ... string.h's functions ...
		// ------------------------------------------------------------------------------------------

		/*!
		 * @see @c "std::memcpy" function.
		 */
		//! @{
		GDINL static Handle MemcpySafe(Handle const dest, SizeTp const destLength, CHandle const source, SizeTp const maxCount)
		{
			auto const result = ::memcpy_s(dest, destLength, source, maxCount);
			GD_DEBUG_VERIFY(result == 0, "memcpy_s failed.");
			return dest;
		}
		GDINL static Handle Memcpy(Handle const dest, CHandle const source, SizeTp const maxCount)
		{
			// as an exception, we force using unsafe 'memcpy'.
			return MemcpySafe(dest, maxCount, source, maxCount);
		}
		template<typename TPointee>
		GDINL static TPointee* TMemcpy(TPointee* const dest, TPointee const* const source, SizeTp const maxCount)
		{
			// as an exception, we force using unsafe 'memcpy'.
			return static_cast<TPointee*>(Memcpy(dest, source, maxCount * sizeof(TPointee)));
		}
		//! @}

		/*!
		 * @see @c "std::memmove" function.
		 */
		//! @{
		GDINL static Handle MemmoveSafe(Handle const dest, SizeTp const destLength, CHandle const source, SizeTp const maxCount)
		{
			auto const result = ::memmove_s(dest, destLength, source, maxCount);
			GD_DEBUG_VERIFY(result == 0, "memcpy_s failed.");
			return dest;
		}
		GDINL static Handle Memmove(Handle const dest, CHandle const source, SizeTp const maxCount)
		{
			// as an exception, we force using unsafe 'memmove'.
			return MemmoveSafe(dest, maxCount, source, maxCount);
		}
		template<typename TPointee>
		GDINL static TPointee* Memmove(TPointee* const dest, TPointee const* const source, SizeTp const maxCount)
		{
			// as an exception, we force using unsafe 'memmove'.
			return static_cast<TPointee*>(Memmove(dest, source, maxCount * sizeof(TPointee)));
		}
		//! @}

	};	// class CMemoryMicrosoft

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Provides cross-platform functions for C memory.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	using CMemory = CMemoryMicrosoft;

GD_NAMESPACE_END