//////////////////////////////////////////////////////////////////////////
/// Stream.inl - Base stream inline implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 30.04.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#pragma once
#if (!defined(GD_CORE_IO_STREAM))
#	error("Please, include Stream.h instead of Stream.inl")
#endif

#include <GoddamnEngine/Core/Utility.hh>
#include <GoddamnEngine/Core/TypeTraits.hh>
#include <GoddamnEngine/Core/Diagnostics/Assertion/Assertion.hh>

GD_NAMESPACE_BEGIN

	template<typename ReadingElementType>
	GDINL ReadingElementType&& Stream::Read()
	{	// We should check if we are reading into POD type to prevent memory leaks.
		static_assert(TypeTraits::IsPodType<ReadingElementType>::Value, "ReadingElementType should be POD.");
		ReadingElementType ReadingElement = ReadingElementType();
		size_t const NumElementsRead = self->Read(&ReadingElement, 0, sizeof(ReadingElement));
		GD_ASSERT((sizeof(ReadingElement) == NumElementsRead), "Reading failed.");
		return Move(ReadingElement);
	}

	template<typename WritingElementType>
	GDINL void Stream::Write(WritingElementType const& Element)
	{	// We should check if we are writing a POD type to prevent memory leaks.
        static_assert(TypeTraits::IsPodType<WritingElementType>::Value, "WritingElementType should be POD.");
		size_t const NumElementsWritten = self->Write(&Element, 0, sizeof(Element));
        GD_ASSERT((sizeof(WritingElementType) == NumElementsWritten), "Writing failed.");
	}

GD_NAMESPACE_END
