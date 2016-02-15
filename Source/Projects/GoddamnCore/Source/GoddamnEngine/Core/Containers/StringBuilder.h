// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Containers/StringBuilder.h
 * Mutable string interface.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Containers/Vector.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                           BaseStringBuilder<T> class.                            ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	// **------------------------------------------------------------------------------------------**
	//! Represents mutable sequence of characters.
	//! @tparam TChar character type of this string.
	// **------------------------------------------------------------------------------------------**
	template<typename TChar, typename TAllocator = DefaultContainerAllocator>
	class BaseStringBuilder final
	{
	public:
		using CharType       = TChar;
		using ContainerType  = Vector<TChar, TAllocator>;
		using BaseStringType = BaseString<TChar>;

	private:
		ContainerType m_Container;

	public:

		// ------------------------------------------------------------------------------------------
		// Constructors and destructor.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Initializes empty string builder.
		 */
		GDINL BaseStringBuilder()
		{
			m_Container.InsertLast(GD_LITERAL(TChar, '\0'));
		}

		/*!
		 * Moves other string builder into this string builder.
		 * @param other The other string builder to move into this.
		 */
		GDINL BaseStringBuilder(BaseStringBuilder&& other)
			: m_Container(Move(other.m_Container))
		{
		}

		// ------------------------------------------------------------------------------------------
		// Dynamic size management.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Returns length of this string builder.
		 */
		GDINL UInt32 GetLength() const
		{
			return m_Container.GetLength() - 1;
		}

		/*!
		 * Returns number of characters that can be placed into string builder without reallocation.
		 */
		GDINL UInt32 GetCapacity() const
		{
			return m_Container.GetCapacity();
		}

		/*!
		 * Returns true if this string builder is empty.
		 */
		GDINL bool IsEmpty() const
		{
			return GetLength() == 0;
		}

		/*!
		 * Resizes string builder to make it able to contain specified number of characters.
		 * @param newLength New required length of the string.
		 */
		GDINL void Resize(UInt32 const newLength)
		{
			m_Container.Resize(newLength + 1);
		}

		/*!
		 * Reserves memory for string builder to make it contain specified number of characters without reallocation when calling Append method.
		 * @param newCapacity New required capacity of the string.
		 */
		GDINL void Reserve(UInt32 const newCapacity)
		{
			m_Container.Reserve(newCapacity + 1);
		}

		/*!
		 * Returns C-String version of this object.
		 */
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

		/*!
		 * Appends a character to this string builder.
		 *
		 * @param character The character to Append.
		 * @returns this.
		 */
		GDINL BaseStringBuilder& Append(TChar const character)
		{
			m_Container.GetLast() = character;
			m_Container.InsertLast(GD_LITERAL(TChar, '\0'));
			return *this;
		}

		/*!
		 * Appends format-able string to this string builder.
		 *
		 * @param format Standard printf-like format.
		 * @returns this.
		 */
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

		/*!
		 * Appends a C string with specified m_Length to this string builder.
		 *
		 * @param cstring String C string to Append.
		 * @returns this.
		 */
		GDINL BaseStringBuilder& Append(TChar const* const cstring, UInt32 const length)
		{
			UInt32 const startPos = m_Container.GetLength() - 1;
			m_Container.Resize(m_Container.GetLength() + length);
			CMemory::Memcpy(m_Container.GetData() + startPos, (length + 1) * sizeof(TChar), cstring);
			return *this;
		}

		/*!
		 * Appends a C string to this string builder.
		 *
		 * @param cstring String C string to Append.
		 * @returns this.
		 */
		//! @{
		template <UInt32 const TLength>	// Length of string constant is determined at compile time..
		GDINL BaseStringBuilder& Append(TChar const(&cstring)[TLength])
		{
			return Append(cstring, TLength - 1);
		}
		GDINL BaseStringBuilder& Append(TChar const* const cstring)
		{
			return Append(cstring, static_cast<UInt32>(BaseCString<TChar>::Strlen(cstring)));
		}
		//! @}

		/*!
		 * Appends a Goddamn string to this string builder.
		 *
		 * @param string Goddamn string to Append.
		 * @returns this.
		 */
		GDINL BaseStringBuilder& Append(BaseStringType const& string)
		{
			return Append(string.CStr(), string.GetLength());
		}

		/*!
		 * Converts this builder to normal string builder.
		 * @returns C-string equivalent of this string builder.
		 */
		GDINL BaseStringType ToString() const
		{
			return BaseStringType(CStr(), GetLength());
		}

	public:

		// ------------------------------------------------------------------------------------------
		// Overloaded operators.
		// ------------------------------------------------------------------------------------------

		GDINL CharType const& operator[] (SizeTp const index) const
		{
			return m_Container[index];
		}
		GDINL CharType& operator[] (SizeTp const index)
		{
			return m_Container[index];
		}

		GDINL BaseStringBuilder& operator= (BaseStringBuilder&& other)
		{
			if (this != &other)
			{
				m_Container = Move(other.m_Container);
			}
			return *this;
		}
	};	// class BaseStringBuilder

	using StringBuilder = BaseStringBuilder<Char>;
	using WideStringBuilder = BaseStringBuilder<WideChar>;

GD_NAMESPACE_END
