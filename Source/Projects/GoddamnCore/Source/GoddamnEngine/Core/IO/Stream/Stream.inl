/// ==========================================================================================
/// Stream.inl - Base stream inline implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 30.04.2014 - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#if (!defined(GD_CORE_IO_STREAM))
#	error("Please, include Stream.h instead of Stream.inl")
#endif

#include <GoddamnEngine/Core/Utility.h>
#include <GoddamnEngine/Core/TypeTraits.h>

GD_NAMESPACE_BEGIN

	/// ==========================================================================================
	/// InputStream class.
	/// ==========================================================================================

	/// ------------------------------------------------------------------------------------------
	/// Public API.
	/// ------------------------------------------------------------------------------------------

	GDINL void InputStream::Read(handle const _Array, size_t const Count, size_t const Length)
	{
		UInt8* const Array = static_cast<UInt8*>(_Array);
		for (UInt8* Byte = Array; Byte < (Array + (Count * Length)); ++Byte) {
			*Byte = this->Read();
		}
	}

	/// ==========================================================================================
	/// OutputStream class.
	/// ==========================================================================================

	/// ------------------------------------------------------------------------------------------
	/// Public API.
	/// ------------------------------------------------------------------------------------------

	GDINL void OutputStream::Write(chandle const _Array, size_t const Count, size_t const Length)
	{
		UInt8 const* const Array = static_cast<UInt8 const*>(_Array);
		for (UInt8 const* Byte = Array; Byte < (Array + (Count * Length)); ++Byte) {
			this->Write(*Byte);
		}
	}

GD_NAMESPACE_END
