//! ==========================================================================================
//! Stream.inl - Base stream inline implementation.
//! Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
//! 
//! History:
///		* 30.04.2014 - Created by James Jhuighuy
//! ==========================================================================================

#pragma once
#if (!defined(GD_CORE_IO_STREAM))
#	error("Please, include Stream.h instead of Stream.inl")
#endif

#include <GoddamnEngine/Core/Utility.h>
#include <GoddamnEngine/Core/TypeTraits.h>

GD_NAMESPACE_BEGIN

	//! ==========================================================================================
	//! InputStream class.
	//! ==========================================================================================

	GDINL void InputStream::Read(Handle const _Array, SizeTp const count, SizeTp const length)
	{
		UInt8* const Array = static_cast<UInt8*>(_Array);
		for (UInt8* Byte = Array; Byte < (Array + (count * length)); ++Byte) {
			*Byte = Read();
		}
	}

	//! ==========================================================================================
	//! OutputStream class.
	//! ==========================================================================================

	GDINL void OutputStream::Write(CHandle const _Array, SizeTp const count, SizeTp const length)
	{
		UInt8 const* const Array = static_cast<UInt8 const*>(_Array);
		for (UInt8 const* Byte = Array; Byte < (Array + (count * length)); ++Byte) {
			Write(*Byte);
		}
	}

GD_NAMESPACE_END
