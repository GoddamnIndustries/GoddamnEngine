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

	GDINL size_t Stream::ReadCheck(handle const OutputBuffer, ptrdiff_t const Offset, size_t const Count)
	{
		size_t const NumElementRead = self->Read(OutputBuffer, Offset, Count);
		GD_ASSERT(NumElementRead == Count, "Failed to read all data");
		return NumElementRead;
	}

	template<typename ReadingElementType>
	GDINL ReadingElementType&& Stream::Read()
	{	// We should check if we are reading into POD type to prevent memory leaks.
		static_assert(TypeTraits::IsPodType<ReadingElementType>::Value, "ReadingElementType should be POD.");
		ReadingElementType ReadingElement = ReadingElementType();
		self->ReadCheck(&ReadingElement, 0, sizeof(ReadingElement));
		return Move(ReadingElement);
	}

	GDINL size_t Stream::WriteCheck(chandle const InputBuffer, ptrdiff_t const Offset, size_t const Count)
	{
		size_t const NumElementsWritten = self->Write(InputBuffer, Offset, Count);
		GD_ASSERT(NumElementsWritten == Count, "Failed to write all data");
		return NumElementsWritten;
	}

	GDINL void Stream::WriteString(String const& TheString, bool const WriteNullTerminator /* = false */)
	{
		size_t const StringSize = (TheString.GetSize() + (WriteNullTerminator ? 1 : 0)) * sizeof(Char);
		self->WriteCheck(TheString.CStr(), 0, StringSize);
	}

	template<typename WritingElementType>
	GDINL void Stream::Write(WritingElementType const& Element)
	{	// We should check if we are writing a POD type to prevent memory leaks.
        static_assert(TypeTraits::IsPodType<WritingElementType>::Value, "WritingElementType should be POD.");
		self->WriteCheck(&Element, 0, sizeof(Element));
	}

GD_NAMESPACE_END
