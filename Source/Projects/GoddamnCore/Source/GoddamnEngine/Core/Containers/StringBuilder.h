// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
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
			m_Container.InsertLast(GD_TEXT(TChar, '\0'));
		}

		/*!
		 * Moves other string builder into this string builder.
		 * @param other The other string builder to move into this.
		 */
		GDINL BaseStringBuilder(BaseStringBuilder&& other) noexcept
			: m_Container(Utils::Move(other.m_Container))
		{
		}

		// ------------------------------------------------------------------------------------------
		// Dynamic size management.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Returns length of this string builder.
		 */
		GDINL SizeTp GetLength() const
		{
			return m_Container.GetLength() - 1;
		}

		/*!
		 * Returns number of characters that can be placed into string builder without reallocation.
		 */
		GDINL SizeTp GetCapacity() const
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
		GDINL void Resize(SizeTp const newLength)
		{
			m_Container.Resize(newLength + 1);
		}

		/*!
		 * Reserves memory for string builder to make it contain specified number of characters without reallocation when 
		 * calling Append method.
		 * @param newCapacity New required capacity of the string.
		 */
		GDINL void Reserve(SizeTp const newCapacity)
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
		 * Converts this builder to normal string builder.
		 * @returns C-string equivalent of this string builder.
		 */
		GDINL BaseStringType ToString() const
		{
			return BaseStringType(CStr(), GetLength());
		}

		// ------------------------------------------------------------------------------------------
		// Concatenations.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Appends specified string to this string builder.
		 * 
		 * @param text Text to append.
		 * @param textLength Length of text to append.
		 */
		GDINL BaseStringBuilder& Append(TChar const* const text, SizeTp const textLength)
		{
			auto const startPos = m_Container.GetLength() - 1;
			m_Container.Resize(m_Container.GetLength() + textLength);
			CMemory::Memcpy(m_Container.GetData() + startPos, text, (textLength + 1) * sizeof(TChar));
			return *this;
		}

		/*!
		 * Appends specified text to this string builder.
		 * @param text Text to append.
		 */
		//! @{
		GDINL BaseStringBuilder& Append(TChar const text)
		{
			m_Container.GetLast() = text;
			m_Container.InsertLast(GD_TEXT(TChar, '\0'));
			return *this;
		}
		template <SizeTp const TLength>	
		GDINL BaseStringBuilder& Append(TChar const(&text)[TLength])
		{
			return Append(text, TLength - 1);
		}
		GDINL BaseStringBuilder& Append(TChar const* const text)
		{
			return Append(text, CString::Strlen(text));
		}
		GDINL BaseStringBuilder& Append(BaseStringType const& text)
		{
			return Append(text.CStr(), text.GetLength());
		}
		GDINL BaseStringBuilder& Append(BaseStringBuilder const& text)
		{
			return Append(text.CStr(), text.GetLength());
		}
		//! @}

		/*!
		 * Appends formatable string to this string builder.
		 *
		 * @param format Standard printf-like format.
		 * @param ... Format Arguments.
		 * @returns this.
		 */
		GDINL BaseStringBuilder& AppendFormat(TChar const* const format, ...)
		{
			//! @todo Optimize this code. possibly try to format everything inline in the builder.
			va_list args;
			va_start(args, format);
			BaseStringType const formatted = BaseStringType::FormatVa(format, args);
			Append(formatted);
			va_end(args);

			return *this;
		}

	public:

		// ------------------------------------------------------------------------------------------
		// Overloaded operators.
		// ------------------------------------------------------------------------------------------

		// string[]
		GDINL CharType const& operator[] (SizeTp const index) const
		{
			return m_Container[index];
		}
		GDINL CharType& operator[] (SizeTp const index)
		{
			return m_Container[index];
		}

		// string builder = string builder
		GDINL BaseStringBuilder& operator= (BaseStringBuilder&& other) noexcept
		{
			if (this != &other)
			{
				m_Container = Utils::Move(other.m_Container);
			}
			return *this;
		}

	};	// class BaseStringBuilder

	using StringBuilder = BaseStringBuilder<Char>;
	using WideStringBuilder = BaseStringBuilder<WideChar>;

GD_NAMESPACE_END
