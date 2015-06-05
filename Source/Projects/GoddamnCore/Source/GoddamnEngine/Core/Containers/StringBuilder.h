// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Containers/StringBuilder.h
//! Mutable string interface.

#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Containers/Vector.h>

GD_NAMESPACE_BEGIN

	//! Represents mutable sequence of characters.
	//! @tparam CharType Character type of this string.
	template<typename CharType>
	class BaseStringBuilder final : public IContainer
	{
	private:
		typedef Vector<CharType> ContainerType;
		typedef BaseString<CharType> BaseStringType;
		ContainerType Container;

	public:

		//! Initializes empty string builder.
		GDINL BaseStringBuilder()
		{
			this->Container.InsertLast(GD_LITERAL(CharType, '\0'));
		}

		//! Moves other string builder into this string builder.
		//! @param Other Other string builder to move into this.
		GDINL BaseStringBuilder(BaseStringBuilder&& Other)
			: Container(Move(Other.Container))
		{
		}

		//! Deinitializes a string builder.
		GDINL ~BaseStringBuilder()
		{
		}

		//! Returns pointer on this string builder.
		//! @returns C-String version of this object.
		//! @{
		GDINL CharType const* CStr() const
		{
			return &this->Container[0];
		}
		GDINL CharType* CStr()
		{
			return const_cast<CharType*>(const_cast<BaseStringBuilder const*>(this)->CStr());
		}
		//! @}

		//! Returns length of this string builder.
		//! @returns Size of this string builder.
		GDINL SizeTp GetLength() const
		{
			return this->Container.GetLength() - 1;
		}

		//! Returns number of characters that can be placed into string builder without reallocation.
		//! @returns Number of characters that can be placed into string builder without reallocation.
		GDINL SizeTp GetCapacity() const
		{
			return this->Container.GetCapacity();
		}

		//! Returns true if this string builder is empty.
		//! @returns True if this string builder is empty, false otherwise.
		GDINL bool IsEmpty() const
		{
			return this->GetLength() == 0;
		}

		//! Resizes string builder to make it able to contain specified number of characters.
		//! @param NewLength New required length of the string.
		GDINL void Resize(SizeTp const NewLength)
		{
			this->Container.Resize(NewLength + 1);
		}

		//! Reserves memory for string builder to make it contain specified number of characters without reallocation when calling Append method.
		//! @param NewCapacity New required capacity of the string.
		GDINL void Reserve(SizeTp const NewCapacity)
		{
			this->Container.Reserve(NewCapacity + 1);
		}

		//! Appends a character to this string builder.
		//! @param Character Character to append.
		//! @returns this.
		GDINL BaseStringBuilder& Append(CharType const Character)
		{
			*(this->Container.End() - 1) = Character;
			this->Container.InsertLast(GD_LITERAL(CharType, '\0'));
			return *this;
		}

		//! Appends formatable string to this string builder.
		//! @param Format Standart printf-like format.
		//! @returns this.
		GDINL BaseStringBuilder& AppendFormat(CharType const* const Format, ...)
		{
			//! @todo Optimize this code. possibly try to format everything inline in the builder.
			va_list ArgumentsList;
			va_start(ArgumentsList, Format);
			BaseStringType const Formatted = BaseStringType::FormatVa(Format, ArgumentsList);
			this->Append(Formatted);
			va_end(ArgumentsList);

			return *this;
		}

		//! Appends a C string with specified length to this string builder.
		//! @param String C string to append.
		//! @returns this.
		GDINL BaseStringBuilder& Append(CharType const* const CString, SizeTp const Length)
		{
			SizeTp const StartPos = this->Container.GetLength() - 1;
			this->Container.Resize(this->Container.GetLength() + Length);
			CMemory::Memcpy(&this->Container[StartPos], CString, (Length + 1) * sizeof(CharType));

			return *this;
		}

		//! Appends a C string to this string builder.
		//! @param String C string to append.
		//! @returns this.
		//! @{
		template <SizeTp const Length>	// Length of string constant is determined at compile time..
		GDINL BaseStringBuilder& Append(CharType const(&CString)[Length])
		{
			return this->Append(CString, Length - 1);
		}
		GDINL BaseStringBuilder& Append(CharType const* const CString)
		{
			return this->Append(CString, CCharTraits<CharType>::StrLen(CString));
		}
		//! @}

		//! Appends a Goddamn string to this string builder.
		//! @param TheString Goddamn string to append.
		//! @returns this.
		GDINL BaseStringBuilder& Append(BaseStringType const& TheString)
		{
			return this->Append(TheString.CStr(), TheString.GetLength());
		}

		//! Converts this builder to normal string builder.
		//! @returns C-string equivalent of this string builder.
		GDINL BaseStringType ToString() const
		{
			return BaseStringType(this->CStr(), this->GetLength());
		}

		//! Moves other string builder here.
		//! @param Other String builder that would be moved into current object.
		//! @returns this.
		GDINL BaseStringBuilder& operator= (BaseStringBuilder&& Other)
		{
			if (this != &Other)
				this->Container = Move(Other.Container);
			return *this;
		}
	};	// class BaseStringBuilder

	typedef BaseStringBuilder<Char> StringBuilder;
	typedef BaseStringBuilder<WideChar> WideStringBuilder;

GD_NAMESPACE_END
