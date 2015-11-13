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
	//! @tparam TChar character type of this string.
	template<typename TChar>
	class BaseStringBuilder final
	{
	public:
		using CharType       = TChar;
		using ContainerType  = Vector<TChar>;
		using BaseStringType = BaseString<TChar>;

	private:
		ContainerType m_Container;

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructors and destructor.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Initializes empty string builder.
		GDINL BaseStringBuilder()
		{
			m_Container.InsertLast(GD_LITERAL(TChar, '\0'));
		}

		// ------------------------------------------------------------------------------------------
		//! Moves other string builder into this string builder.
		//! @param other The other string builder to move into this.
		GDINL BaseStringBuilder(BaseStringBuilder&& other)
			: m_Container(Move(other.m_Container))
		{
		}

		// ------------------------------------------------------------------------------------------
		//! Deinitializes a string builder.
		GDINL ~BaseStringBuilder()
		{
		}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Dynamic size management.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Returns m_Length of this string builder.
		//! @returns Size of this string builder.
		GDINL SizeTp GetLength() const
		{
			return m_Container.GetLength() - 1;
		}

		// ------------------------------------------------------------------------------------------
		//! Returns number of characters that can be placed into string builder without reallocation.
		//! @returns Number of characters that can be placed into string builder without reallocation.
		GDINL SizeTp GetCapacity() const
		{
			return m_Container.GetCapacity();
		}

		// ------------------------------------------------------------------------------------------
		//! Returns true if this string builder is empty.
		//! @returns True if this string builder is empty, false otherwise.
		GDINL bool IsEmpty() const
		{
			return GetLength() == 0;
		}

		// ------------------------------------------------------------------------------------------
		//! Resizes string builder to make it able to contain specified number of characters.
		//! @param newLength New required m_Length of the string.
		GDINL void Resize(SizeTp const newLength)
		{
			m_Container.Resize(newLength + 1);
		}

		// ------------------------------------------------------------------------------------------
		//! Reserves memory for string builder to make it contain specified number of characters without reallocation when calling Append method.
		//! @param newCapacity New required m_Capacity of the string.
		GDINL void Reserve(SizeTp const newCapacity)
		{
			m_Container.Reserve(newCapacity + 1);
		}

		// ------------------------------------------------------------------------------------------
		//! Returns pointer on this string builder.
		//! @returns C-String version of this object.
		//! @{
		GDINL TChar const* CStr() const
		{
			return m_Container.GetData();
		}
		GDINL TChar* CStr()
		{
			return const_cast<TChar*>(const_cast<BaseStringBuilder const*>(this)->CStr());
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Appends a character to this string builder.
		//! @param character The character to Append.
		//! @returns this.
		GDINL BaseStringBuilder& Append(TChar const character)
		{
			m_Container.GetLast() = character;
			m_Container.InsertLast(GD_LITERAL(TChar, '\0'));
			return *this;
		}

		// ------------------------------------------------------------------------------------------
		//! Appends format-able string to this string builder.
		//! @param format Standard printf-like format.
		//! @returns this.
		GDINL BaseStringBuilder& AppendFormat(TChar const* const format, ...)
		{
			//! @todo Optimize this code. possibly try to format everything inline in the builder.
			va_list args;
			va_start(args, format);
			BaseStringType const Formatted = BaseStringType::FormatVa(format, args);
			Append(Formatted);
			va_end(args);

			return *this;
		}

		// ------------------------------------------------------------------------------------------
		//! Appends a C string with specified m_Length to this string builder.
		//! @param cstring String C string to Append.
		//! @returns this.
		GDINL BaseStringBuilder& Append(TChar const* const cstring, SizeTp const length)
		{
			SizeTp const StartPos = m_Container.GetLength() - 1;
			m_Container.Resize(m_Container.GetLength() + length);
			CMemory::Memcpy(m_Container.GetData() + StartPos, (length + 1) * sizeof(TChar), cstring);
			return *this;
		}

		// ------------------------------------------------------------------------------------------
		//! Appends a C string to this string builder.
		//! @param cstring String C string to Append.
		//! @returns this.
		//! @{
		template <SizeTp const TLength>	// Length of string constant is determined at compile time..
		GDINL BaseStringBuilder& Append(TChar const(&cstring)[TLength])
		{
			return Append(cstring, TLength - 1);
		}
		GDINL BaseStringBuilder& Append(TChar const* const cstring)
		{
			return Append(cstring, BaseCString<TChar>::Strlen(cstring));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Appends a Goddamn string to this string builder.
		//! @param TheString Goddamn string to Append.
		//! @returns this.
		GDINL BaseStringBuilder& Append(BaseStringType const& TheString)
		{
			return Append(TheString.CStr(), TheString.GetLength());
		}

		// ------------------------------------------------------------------------------------------
		//! Converts this builder to normal string builder.
		//! @returns C-string equivalent of this string builder.
		GDINL BaseStringType ToString() const
		{
			return BaseStringType(CStr(), GetLength());
		}

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Overloaded operators.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		GDINL CharType const& operator[] (SizeTp const index) const
		{
			return m_Container[index];
		}
		GDINL CharType& operator[] (SizeTp const index)
		{
			return const_cast<Char&>(const_cast<BaseStringBuilder const&>(*this)[index]);
		}

		GDINL BaseStringBuilder& operator= (BaseStringBuilder&& other)
		{
			if (this != &other)
				m_Container = Move(other.m_Container);
			return *this;
		}
	};	// class BaseStringBuilder

	typedef BaseStringBuilder<Char> StringBuilder;
	typedef BaseStringBuilder<WideChar> WideStringBuilder;

GD_NAMESPACE_END
