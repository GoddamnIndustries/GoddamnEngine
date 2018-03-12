// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Containers/String.h
 * Constantly sized string class.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/CStdlib/CChar.h>
#include <GoddamnEngine/Core/CStdlib/CString.h>
#include <GoddamnEngine/Core/CStdlib/CMemory.h>
#include <GoddamnEngine/Core/Templates/Algorithm.h>
#include <GoddamnEngine/Core/Templates/Iterators.h>

GD_NAMESPACE_BEGIN

	template<typename TChar>
	struct BaseStringDefaultLength
	{
		SizeTp static const Value = sizeof(TChar*) / sizeof(TChar);  // NOLINT
	};	// struct BaseStringDefaultLength

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                               BaseString<T> class.                               ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	// **------------------------------------------------------------------------------------------**
	//! A base string class used by the engine.
	//! @tparam TChar character type of this string.
	//! @tparam TInlineLength Max length of data that could be stored inside string.
	// **------------------------------------------------------------------------------------------**
	template<typename TChar, SizeTp TInlineLength = BaseStringDefaultLength<TChar>::Value>
	class BaseString final
	{
		static_assert(TInlineLength > 1, "Zero inline length.");
		static_assert(TInlineLength < 256, "Inline length is greater than 255 characters.");

	public:
		using ElementType          = TChar;
		using ConstElementType     = TChar const;
		using ReferenceType        = TChar&;
		using ConstReferenceType   = TChar const&;
		using Iterator             = IndexedContainerIterator<BaseString>;
		using ConstIterator        = IndexedContainerIterator<BaseString const>;
		using ReverseIterator      = ReverseContainerIterator<Iterator>;
		using ReverseConstIterator = ReverseContainerIterator<ConstIterator>;

		enum class Base
		{
			Decimal    = 10,	//!< Octal base.
			Octal       = 8,	//!< Octal base.
			Hexadecimal = 16,	//!< Hexadecimal base.
			Unknown,
		};	// enum class Base

		SizeTp static constexpr Npos = SizeTpMax;

		GD_CONTAINER_DEFINE_ITERATION_SUPPORT(BaseString)

	private:
		// ReSharper disable CppInconsistentNaming
		SizeTp static constexpr s_MaxInlineLength = TInlineLength;
		SizeTp m_Length;
		union {
			TChar  m_InlineMemory[s_MaxInlineLength];
			TChar* m_HeapMemory;
		};	// anonymous union
		// ReSharper restore CppInconsistentNaming

	public:

		// ------------------------------------------------------------------------------------------
		// Constructors, initializers & destructor.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Initializes an empty string.
		 */
		GDINL BaseString()
			: m_Length(0)
			, m_InlineMemory{ GD_TEXT(TChar, '\0') }
		{
		}

		/*!
		 * Initializes a string with a single character.
		 * @param text Initial String character.
		 */
		GDINL explicit BaseString(Char const text)
			: m_Length(1)
			, m_InlineMemory{ text, GD_TEXT(TChar, '\0') }
		{
		}

		/*!
		 * Fills a string with specified number of characters.
		 *
		 * @param length Length of the string.
		 * @param fillWith A character that String would be filled with.
		 */
		GDINL explicit BaseString(SizeTp const length, TChar const fillWith = GD_TEXT(TChar, '\0'))
			: m_Length(length)
		{
			if (m_Length >= s_MaxInlineLength)
			{
				m_HeapMemory = CMemory::CMemset(GD_MALLOC_ARRAY_T(TChar, m_Length + 1), fillWith, m_Length);
				m_HeapMemory[m_Length] = GD_TEXT(TChar, '\0');
			}
			else
			{
				CMemory::CMemset(m_InlineMemory, fillWith, m_Length);
				m_InlineMemory[m_Length] = GD_TEXT(TChar, '\0');
			}
		}

		/*!
		 * Initializes a string with some C String with known length.
		 *
		 * @param text String's initial data.
		 * @param textLength Size of String initial data.
		 */
		GDINL BaseString(TChar const* const text, SizeTp const textLength)
			: m_Length(textLength)
		{
			GD_ASSERT(text != nullptr, "Null pointer data specified");
			if (m_Length >= s_MaxInlineLength)
			{
				m_HeapMemory = CMemory::CMemcpy(GD_MALLOC_ARRAY_T(TChar, m_Length + 1), text, m_Length);
				m_HeapMemory[m_Length] = GD_TEXT(TChar, '\0');
			}
			else
			{
				CMemory::CMemcpy(m_InlineMemory, text, m_Length);
				m_InlineMemory[m_Length] = GD_TEXT(TChar, '\0');
			}
		}

		/*!
		 * Initializes a string with some C String.
		 * @param text String initial data.
		 */
		//! @{   
		GDINL implicit BaseString(TChar const* const text)  // NOLINT
			: BaseString(text, CString::Strlen(text))
		{
		}
		//! @}

		/*!
		 * Initializes this string with copy of other string.
		 * @param text The other string to copy.
		 */
		//! @{
		GDINL BaseString(BaseString const& text)  // NOLINT
			: BaseString(text.CStr(), text.GetLength())
		{
		}
		template<SizeTp TOtherInlineLength>
		GDINL BaseString(BaseString<TChar, TOtherInlineLength> const& text)
			: BaseString(text.CStr(), text.GetLength())
		{
		}
		//! @}

		/*!
		 * Moves other string into this string.
		 * @param text The other string to move into this.
		 */
		//! @{
		GDINL BaseString(BaseString&& text) noexcept
			: m_Length(text.m_Length)
		{
			CMemory::CMemcpy(m_InlineMemory, text.m_InlineMemory, GD::GetLength(text.m_InlineMemory));
			CMemory::CMemset(text.m_InlineMemory, TChar(0), GD::GetLength(text.m_InlineMemory));
			text.m_Length = 0;
		}
		template<SizeTp TOtherInlineLength>
		GDINL implicit BaseString(typename EnableIf<TOtherInlineLength <= TInlineLength, BaseString<TChar, TOtherInlineLength>>::Value&& text)
			: m_Length(text.m_Length)
		{
			CMemory::CMemcpy(m_InlineMemory, text.m_InlineMemory, GD::GetLength(text.m_InlineMemory));
			CMemory::CMemset(text.m_InlineMemory, TChar(0), GD::GetLength(text.m_InlineMemory));
			text.m_Length = 0;
		}
		//! @}

		GDINL ~BaseString()
		{
			if (m_Length >= s_MaxInlineLength)
			{
				GD_FREE(m_HeapMemory);
				m_HeapMemory = nullptr;
			}
			else
			{
				m_InlineMemory[0] = GD_TEXT(TChar, '\0');
			}
			m_Length = 0;
		}

	public:

		// ------------------------------------------------------------------------------------------
		// Iteration API.
		// ------------------------------------------------------------------------------------------

		GDINL Iterator Begin()
		{
			return Iterator(*this);
		}
		GDINL ConstIterator Begin() const
		{
			return ConstIterator(*this);
		}

		GDINL Iterator End()
		{
			return Begin() + m_Length;
		}
		GDINL ConstIterator End() const
		{
			return Begin() + m_Length;
		}

		GDINL ReverseIterator ReverseBegin()
		{
			return ReverseIterator(End() - 1);
		}
		GDINL ReverseConstIterator ReverseBegin() const
		{
			return ReverseConstIterator(End() - 1);
		}

		GDINL ReverseIterator ReverseEnd()
		{
			return ReverseIterator(Begin() - 1);
		}
		GDINL ReverseConstIterator ReverseEnd() const
		{
			return ReverseConstIterator(Begin() - 1);
		}

	public:
	
		// ------------------------------------------------------------------------------------------
		// Dynamic size management.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Returns length of this string.
		 */
		GDINL SizeTp Length() const
		{
			return m_Length;
		}
		GD_DEPRECATED("Length") GDINL auto GetLength() const
		{
			return Length();
		}

		/*!
		 * Returns true if this string is empty.
		 */
		GDINL bool IsEmpty() const
		{
			return m_Length == 0;
		}

	public:

		// ------------------------------------------------------------------------------------------
		// Dynamic elements access & API.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Returns pointer on this string.
		 */
		//! @{
		GDINL TChar* CStr()
		{
			return const_cast<TChar*>(const_cast<BaseString const*>(this)->CStr());
		}
		GDINL TChar const* CStr() const
		{
			return GetLength() >= s_MaxInlineLength ? m_HeapMemory : m_InlineMemory;
		}
		//! @}

		/*!
		 * Returns last character of the string.
		 */
		//! @{
		GDINL TChar GetLast() const
		{
			return *(End() - 1);
		}
		GDINL TChar& GetLast()
		{
			return *(End() - 1);
		}
		//! @}

		/*!
		 * Returns unique signature of the string.
		 * @returns Constant-time comparable unique signature of the string. 
		 */
		GDINL HashCode GetHashCode() const
		{
			auto computed = 0;
			for (auto const character : (*this))
			{
				computed = 65599 * computed + character;
			}
			return HashCode(computed >> 16 ^ computed);
		}

		// ------------------------------------------------------------------------------------------
		// Concatenations.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Appends specified string to this string.
		 * 
		 * @param text Text to append.
		 * @param textLength Length of text to append.
		 */
		GDINL BaseString Append(TChar const* const text, SizeTp const textLength) const
		{
			BaseString result(m_Length + textLength);
			CMemory::CMemcpy(result.CStr(), CStr(), m_Length);
			CMemory::CMemcpy(result.CStr() + m_Length, text, textLength);
			return result;
		}

		/*!
		 * Appends specified text to this string.
		 * @param text Text to append.
		 */
		//! @{
		template<SizeTp TOtherInlineLength>
		GDINL BaseString Append(BaseString<TChar, TOtherInlineLength> const& text) const
		{
			return this->Append(text.CStr(), text.m_Length);
		}
		GDINL BaseString Append(TChar const* const text) const
		{
			return this->Append(text, CString::Strlen(text));
		}
		GDINL BaseString Append(TChar const text) const
		{
			return this->Append(&text, 1);
		}
		//! @}

		/*!
		 * Prepends specified string to this string.
		 * 
		 * @param text Text to append.
		 * @param textLength Length of text to append.
		 */
		GDINL BaseString Prepend(TChar const* const text, SizeTp const textLength) const
		{
			BaseString result(m_Length + textLength);
			CMemory::CMemcpy(result.CStr(), text, textLength);
			CMemory::CMemcpy(result.CStr() + textLength, CStr(), m_Length);
			return result;
		}

		/*!
		 * Prepends specified string to this string.
		 * @param text Text to append.
		 */
		//! @{
		template<SizeTp TOtherInlineLength>
		GDINL BaseString Prepend(BaseString<TChar, TOtherInlineLength> const& text) const
		{
			return this->Prepend(text.CStr(), text.m_Length);
		}
		GDINL BaseString Prepend(TChar const* const text) const
		{
			return this->Prepend(text, static_cast<SizeTp>(CString::Strlen(text)));
		}
		GDINL BaseString Prepend(TChar const text) const
		{
			return this->Prepend(&text, 1);
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// Sub-stringing.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Extracts a substring from this string from specified index to the specified index (including both).
		 *
		 * @param from Index of the first character that would be extracted to the substring.
		 * @param to Index of the last character that would be extracted to the substring.
		 *
		 * @returns A part of this string.
		 */
		GDINL BaseString Substring(SizeTp const from, SizeTp const to) const
		{
			GD_ASSERT(to >= from, "Invalid substring indices.");
			GD_ASSERT(to < m_Length, "Invalid substring indices.");
			GD_ASSERT(from < m_Length, "Invalid substring indices.");

			BaseString result(to - from + 1);
			CMemory::CMemcpy(result.CStr(), CStr() + from, result.m_Length);
			return result;
		}

		/*!
		 * Extracts a substring from this string from specified index to the end.
		 *
		 * @param from Index of the first character that would be extracted to the substring.
		 * @returns A part of this string.
		 */
		GDINL BaseString Substring(SizeTp const from) const
		{
			return Substring(from, m_Length - 1);
		}

		/*!
		 * Extracts a substring from this string from specified index with specified length.
		 *
		 * @param from Index of the first character that would be extracted to the substring.
		 * @param length The length of the desired substring.
		 *
		 * @returns A part of this string.
		 */
		GDINL BaseString SubstringLen(SizeTp const from, SizeTp const length) const
		{
			return Substring(from, from + length - 1);
		}
		
		/*!
		 * Extracts a substring for this string from chopped start to end.
		 *
		 * @param length Amount of bytes to skip from front.
		 * @returns A part of this string.
		 */
		GDINL BaseString ChopFront(SizeTp const length) const
		{
			return Substring(length);
		}

		/*!
		 * Extracts a substring for this string from start to chopped end.
		 *
		 * @param length Amount of bytes to skip from end.
		 * @returns A part of this string.
		 */
		GDINL BaseString ChopEnd(SizeTp const length) const
		{
			return SubstringLen(0, m_Length - length);
		}

		/*!
		 * Removes a substring from this string from specified index to the specified index (excluding both).
		 *
		 * @param from Index of the first character that would be removed to the substring.
		 * @param to Index of the last character that would be removed to the substring.
		 *
		 * @returns A part of this string.
		 */
		GDINL BaseString Remove(SizeTp const from, SizeTp const to) const
		{
			auto result = Substring(0, from - 1);
			if (to < m_Length - 1)
			{
				result += Substring(to + 1);
			}
			return result;
		}

		/*!
		 * Removes a substring from this string from specified index to the specified index (excluding both).
		 *
		 * @param from Index of the first character that would be removed to the substring.
		 * @returns A part of this string.
		 */
		GDINL BaseString Remove(SizeTp const from) const
		{
			return Remove(from, m_Length - 1);
		}

		/*!
		 * Removes a substring from this string from specified index with specified length.
		 *
		 * @param from Index of the first character that would be removed to the substring.
		 * @param length The length of the desired removed substring.
		 *
		 * @returns A part of this string.
		 */
		GDINL BaseString RemoveLen(SizeTp const from, SizeTp const length) const
		{
			return Substring(0, from - 1) + Substring(from + length);
		}

		// ------------------------------------------------------------------------------------------
		// 'Finds' and 'Starts/Ends With'.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Searches for first occurrence of the specified text in the string.
		 *
		 * @param text Text we are looking for.
		 * @returns Index of the first occurrence of the text in the string or @c Npos if nothing was found.
		 */
		//! @{
		GDINL SizeTp Find(TChar const text) const
		{
			auto const cstring = CStr();
			auto const location = CString::Strchr(cstring, text);
			return location != nullptr ? static_cast<SizeTp>(location - cstring) : Npos;
		}
		GDINL SizeTp Find(TChar const* const text) const
		{
			auto const cstring = CStr();
			auto const location = CString::Strstr(CStr(), text);
			return location != nullptr ? static_cast<SizeTp>(location - cstring) : Npos;
		}
		template<SizeTp TOtherInlineLength>
		GDINL SizeTp Find(BaseString<TChar, TOtherInlineLength> const& text) const
		{
			return this->Find(text.CStr());
		}
		//! @}

		/*!
		 * Searches for last occurrence of the C string in the string.
		 *
		 * @param text Text we are looking for.
		 * @returns Index of the last occurrence of the text in the string or @c Npos if nothing was found.
		 */
		//! @{
		GDINL SizeTp ReverseFind(TChar const text) const
		{
			auto const cstring = CStr();
			auto const location = CString::Strrchr(cstring, text);
			return location != nullptr ? static_cast<SizeTp>(location - cstring) : Npos;
		}
		GDINL SizeTp ReverseFind(TChar const* const text) const
		{
			auto const cstring = CStr();
			auto const location = CString::Strrstr(cstring, text);
			return location != nullptr ? static_cast<SizeTp>(location - cstring) : Npos;
		}
		template<SizeTp TOtherInlineLength>
		GDINL SizeTp ReverseFind(BaseString<TChar, TOtherInlineLength> const& text) const
		{
			return this->ReverseFind(text.CStr());
		}
		//! @}

		/*!
		 * Returns true if this string starts with specified text.
		 *
		 * @param text Text we are testing against.
		 * @param textLength Length of the string we are testing against.
		 */
		GDINL bool StartsWith(TChar const* const text, SizeTp const textLength) const
		{
			return textLength <= m_Length && CString::Strncmp(CStr(), text, textLength) == 0;
		}

		/*!
		 * Returns true if this string starts with specified expression.
		 * @param text Text we are testing against.
		 */
		//! @{
		template<SizeTp TOtherInlineLength>
		GDINL bool StartsWith(BaseString<TChar, TOtherInlineLength> const& text) const
		{
			return this->StartsWith(text.CStr(), text.m_Length);
		}
		GDINL bool StartsWith(TChar const* const text) const
		{
			return this->StartsWith(text, static_cast<SizeTp>(CString::Strlen(text)));
		}
		GDINL bool StartsWith(TChar const text) const
		{
			return *CStr() == text;
		}
		//! @}

		/*!
		 * Returns true if this string ends with specified text.
		 *
		 * @param text Text we are testing against.
		 * @param textLength Length of the text we are testing against.
		 */
		GDINL bool EndsWith(TChar const* const text, SizeTp const textLength) const
		{
			return textLength <= m_Length && CString::Strncmp(CStr() + m_Length - textLength, text, textLength) == 0;
		}

		/*!
		 * Returns true if this string ends with specified expression.
		 * @param text Text we are testing against.
		 */
		//! @{
		template<SizeTp TOtherInlineLength>
		GDINL bool EndsWith(BaseString<TChar, TOtherInlineLength> const& text) const
		{
			return this->EndsWith(text.CStr(), text.m_Length);
		}
		GDINL bool EndsWith(TChar const* const text) const
		{
			return this->EndsWith(text, static_cast<SizeTp>(CString::Strlen(text)));
		}
		GDINL bool EndsWith(TChar const text) const
		{
			return *(End() - 1) == text;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// Formatting.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Formats a specified string using C-like formatting.
		 *
		 * @param format Initial format of the string.
		 * @param list Varying arguments list for formatting.
		 */
		GDINL static BaseString FormatVa(TChar const* const format, va_list list)
		{
			TChar buffer[15000] = {};
			CString::Vsnprintf(buffer, GD::GetLength(buffer), format, list);
			return BaseString(buffer);
		}

		/*!
		 * Formats a specified string using C-like formatting.
		 * @param format Initial format of the string.
		 * @param ... Format arguments.
		 */
		GDINL static BaseString Format(TChar const* const format, ...)
		{
			va_list list; 
			va_start(list, format);
			auto const string = BaseString::FormatVa(format, list);
			va_end(list);
			return string;
		}

		// ------------------------------------------------------------------------------------------
		// Conversions.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Returns lowercased version of this string.
		 */
		GDINL BaseString ToUpper() const
		{
			auto result(*this);
			for (auto& ch : result)
			{
				ch = CChar::ToUpper(ch);
			}
			return result;
		}

		/*!
		 * Returns uppercased version of this string.
		 */
		GDINL BaseString ToLower() const
		{
			auto result(*this);
			for (auto& ch : result)
			{
				ch = CChar::ToLower(ch);
			}
			return result;
		}

		/*!
		 * Returns boolean representation of this string.
		 * @c 0 and @c false is treated as @c false.
		 * @c 1 and @c true is treated as @c true.
		 *
		 * @param succeeded Would be written true if conversion succeeded.
		 * @returns Boolean representation of this string.
		 */
		GDINL bool ToBool(bool* const succeeded = nullptr) const
		{
			if (*this == GD_TEXT(TChar, "0") || *this == GD_TEXT(TChar, "false"))
			{
				if (succeeded != nullptr)
				{
					*succeeded = true;
				}
				return false;
			}
			if (*this == GD_TEXT(TChar, "1") || *this == GD_TEXT(TChar, "true"))
			{
				if (succeeded != nullptr)
				{
					*succeeded = true;
				}
				return true;
			}
			if (succeeded != nullptr)
			{
				*succeeded = false;
			}
			return false;
		}

		/*!
		 * Returns string representation of the specified boolean.
		 * @param value Value to convert.
		 */
		GDINL static BaseString FromBool(bool const value)
		{
			return value ? BaseString("true") : BaseString("false");
		}

		/*!
		 * Returns signed integer representation of this string.
		 * @param succeeded Would be written true if conversion succeeded.
		 */
		GDINL Int64 ToInt64(bool* const succeeded = nullptr) const
		{
			TChar const* endPtr = nullptr;
			auto const value = CString::Strtoi64(CStr(), &endPtr, 0);
			if (succeeded != nullptr)
			{
				*succeeded = endPtr == &*End();
			}
			return value; 
		}

		/*!
		 * Returns unsigned integer representation of this string.
		 *
		 * @param succeeded Would be written true if conversion succeeded.
		 * @param valueBase The base in which value is represented.
		 */
		GDINL Int64 ToUInt64(bool* const succeeded = nullptr, Base const valueBase = Base::Decimal) const
		{
			GD_ASSERT(valueBase < Base::Unknown);

			Int64 value = 0; 
			TChar const* endPtr = nullptr;
			auto cstring = CStr();
			switch (valueBase) // NOLINT
			{
				case Base::Hexadecimal:
					if (cstring[0] == GD_TEXT(TChar, '0') 
						&& (cstring[1] == GD_TEXT(TChar, 'x') || cstring[1] == GD_TEXT(TChar, 'X')))
					{
						// Skipping the leading '0x' prefix.
						cstring += 2;
					}
				case Base::Decimal:
				case Base::Octal:
					value = CString::Strtoi64(cstring, &endPtr, static_cast<SizeTp>(valueBase));
					break;
			}
			if (succeeded != nullptr)
			{
				*succeeded = endPtr == &*End();
			}
			return value; 
		}

		/*!
		 * Returns string representation of the specified signed integral number.
		 * @param value Value to convert.
		 */
		GDINL static BaseString FromInt64(Int64 const value)
		{
			return Format(GD_TEXT(TChar, "%lld"), value);
		}

		/*!
		 * Returns string representation of the specified unsigned integral number.
		 * 
		 * @param value Value to convert.
		 * @param valueBase The base in which value should be represented.
		 * @param valueBasePrefix Should the base prefix be added, e.g. '0x' for hexadecimals.
		 */
		GDINL static BaseString FromUInt64(UInt64 const value, Base const valueBase = Base::Decimal, bool const valueBasePrefix = false)
		{
			GD_ASSERT(valueBase < Base::Unknown);
			switch (valueBase)
			{
				case Base::Decimal:
					return Format(GD_TEXT(TChar, "%llu"), value);
				case Base::Octal:
					return Format(valueBasePrefix ? GD_TEXT(TChar, "0%llo") : GD_TEXT(TChar, "%llo"), value);
				case Base::Hexadecimal:
					return Format(valueBasePrefix ? GD_TEXT(TChar, "0x%llx") : GD_TEXT(TChar, "%llx"), value);
				default:
					GD_NOT_SUPPORTED();
			}
		}

		/*!
		 * Returns double representation of this string.
		 * @param succeeded Would be written true if conversion succeeded.
		 */
		GDINL Float64 ToFloat64(bool* const succeeded = nullptr) const
		{
			TChar const* endPtr = nullptr;
			auto const value = CString::Strtof64(CStr(), &endPtr);
			if (succeeded != nullptr)
			{
				*succeeded = endPtr == &*End();
			}
			return value;
		}

		/*!
		 * Returns string representation of the specified floating point number.
		 * @param value Value to convert.
		 */
		GDINL static BaseString FromFloat64(Float64 const value)
		{
			return Format(GD_TEXT(TChar, "%f"), value);
		}

		// ------------------------------------------------------------------------------------------
		// Miscellaneous.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Matches a string against a wildcard string.
		 *
		 * @param wildcard The wildcard string.
		 * @returns @c true in matches, @c false otherwise.
		 */
		//! @{
		GDINL bool MatchesWildcard(TChar const* const wildcard) const 
		{
			GD_ASSERT(wildcard != nullptr, "Null pointer wildcard was specified.");

			// Originally written by Jack Handy - 
			// <A href="mailto:jakkhandy@hotmail.com">jakkhandy@hotmail.com</A>.
			TChar const* cstring = CStr();
			TChar const* wildcardPtr = wildcard;
			TChar const* charPtr = nullptr;
			TChar const* matchPointer = nullptr;
			while ((*cstring != GD_TEXT(TChar, '\0')) && (*wildcardPtr != GD_TEXT(TChar, '*')))
			{
				if ((*wildcardPtr != *cstring) && (*wildcardPtr != GD_TEXT(TChar, '?')))
				{
					return false;
				}
				++wildcardPtr;
				++cstring;
			}
			while (*cstring != GD_TEXT(TChar, '\0'))
			{
				if (*wildcardPtr == GD_TEXT(TChar, '*'))
				{
					if (!*++wildcardPtr)
					{
						return true;
					}
					matchPointer = wildcardPtr;
					charPtr = cstring + 1;
				}
				else if ((*wildcardPtr == *cstring) || (*wildcardPtr == GD_TEXT(TChar, '?')))
				{
					++wildcardPtr;
					++cstring;
				}
				else
				{
					wildcardPtr = matchPointer;
					cstring = charPtr++;
				}
			}
			while (*wildcardPtr == GD_TEXT(TChar, '*'))
			{
				++wildcardPtr;
			}
			return (*wildcardPtr == GD_TEXT(TChar, '\0'));
		}
		template<SizeTp TOtherInlineLength>
		GDINL bool MatchesWildcard(BaseString<TChar, TOtherInlineLength> const& wildcard) const
		{
			return MatchesWildcard(wildcard.CStr());
		}
		//! @}

	public:

		// ------------------------------------------------------------------------------------------
		// Overloaded operators.
		// ------------------------------------------------------------------------------------------

		// string = string
		GDINL BaseString& operator= (BaseString const& string)
		{
			if (this != &string)
			{
				this->~BaseString();
				new (this) BaseString(string);
			}
			return *this;
		}
		template<SizeTp TOtherInlineLength>
		GDINL BaseString& operator= (BaseString<TChar, TOtherInlineLength> const& string)
		{
			if (this != &string)
			{
				this->~BaseString();
				new (this) BaseString(string);
			}
			return *this;
		}
		GDINL BaseString& operator= (BaseString&& string) noexcept
		{
			if (this != &string)
			{
				this->~BaseString();
				new (this) BaseString(Utils::Forward<BaseString>(string));
			}
			return *this;
		}
		template<SizeTp TOtherInlineLength>
		GDINL BaseString& operator= (typename EnableIf<TOtherInlineLength <= TInlineLength, BaseString<TChar, TOtherInlineLength>>::Value&& string)
		{
			if (this != &string)
			{
				this->~BaseString();
				new (this) BaseString(Utils::Forward<BaseString<TChar, TOtherInlineLength>>(string));
			}
			return *this;
		}

		// string[]
		GDINL TChar const& operator[] (SizeTp const index) const
		{
			GD_ASSERT(index <= GetLength(), "Invalid string subindex.");
			return *(CStr() + index);
		}
		GDINL TChar& operator[] (SizeTp const index)
		{
			return const_cast<TChar&>(const_cast<BaseString const&>(*this)[index]);
		}

		// string == string
		template<SizeTp TInlineLengthLhs, SizeTp TInlineLengthRhs>
		GDINL friend bool operator== (BaseString<TChar, TInlineLengthLhs> const& lhs, BaseString<TChar, TInlineLengthRhs> const& rhs)
		{
			return lhs.GetLength() == rhs.GetLength() && CString::Strncmp(lhs.CStr(), rhs.CStr(), lhs.GetLength()) == 0;
		}
		template<SizeTp TInlineLengthLhs, SizeTp TInlineLengthRhs>
		GDINL friend bool operator!= (BaseString<TChar, TInlineLengthLhs> const& lhs, BaseString<TChar, TInlineLengthRhs> const& rhs)
		{
			return lhs.GetLength() != rhs.GetLength() || CString::Strncmp(lhs.CStr(), rhs.CStr(), lhs.GetLength()) != 0;
		}

		// string == char*
		template<SizeTp TInlineLengthLhs>
		GDINL friend bool operator== (BaseString<TChar, TInlineLengthLhs> const& lhs, TChar const* const rhs)
		{
			return CString::Strncmp(lhs.CStr(), rhs, lhs.GetLength()) == 0;
		}
		template<SizeTp TInlineLengthRhs>
		GDINL friend bool operator== (TChar const* const lhs, BaseString<TChar, TInlineLengthRhs> const& rhs)
		{
			return rhs == lhs;
		}

		template<SizeTp TInlineLengthLhs>
		GDINL friend bool operator!= (BaseString<TChar, TInlineLengthLhs> const& lhs, TChar const* const rhs)
		{
			return !(lhs == rhs);
		}
		template<SizeTp TInlineLengthRhs>
		GDINL friend bool operator!= (TChar const* const lhs, BaseString<TChar, TInlineLengthRhs> const& rhs)
		{
			return !(lhs == rhs);
		}

		// string == char
		template<SizeTp TInlineLengthLhs>
		GDINL friend bool operator== (BaseString<TChar, TInlineLengthLhs> const& lhs, TChar const rhs)
		{
			return lhs.GetLength() == 1 && *lhs.CStr() == rhs;
		}
		template<SizeTp TInlineLengthRhs>
		GDINL friend bool operator== (TChar const lhs, BaseString<TChar, TInlineLengthRhs> const& rhs)
		{
			return rhs == lhs;
		}

		template<SizeTp TInlineLengthLhs>
		GDINL friend bool operator!= (BaseString<TChar, TInlineLengthLhs> const& lhs, TChar const rhs)
		{
			return !(lhs == rhs);
		}
		template<SizeTp TInlineLengthRhs>
		GDINL friend bool operator!= (TChar const lhs, BaseString<TChar, TInlineLengthRhs> const& rhs)
		{
			return rhs != lhs;
		}

		// string > string
		template<SizeTp TInlineLengthLhs, SizeTp TInlineLengthRhs>
		GDINL friend bool operator> (BaseString<TChar, TInlineLengthLhs> const& lhs, BaseString<TChar, TInlineLengthRhs> const& rhs)
		{
			return CString::Strncmp(lhs.CStr(), rhs.CStr(), lhs.GetLength()) > 0;
		}
		template<SizeTp TInlineLengthLhs, SizeTp TInlineLengthRhs>
		GDINL friend bool operator>= (BaseString<TChar, TInlineLengthLhs> const& lhs, BaseString<TChar, TInlineLengthRhs> const& rhs)
		{
			return CString::Strncmp(lhs.CStr(), rhs.CStr(), lhs.GetLength()) >= 0;
		}

		// string < string
		template<SizeTp TInlineLengthLhs, SizeTp TInlineLengthRhs>
		GDINL friend bool operator< (BaseString<TChar, TInlineLengthLhs> const& lhs, BaseString<TChar, TInlineLengthRhs> const& rhs)
		{
			return CString::Strncmp(lhs.CStr(), rhs.CStr(), lhs.GetLength()) < 0;
		}
		template<SizeTp TInlineLengthLhs, SizeTp TInlineLengthRhs>
		GDINL friend bool operator<= (BaseString<TChar, TInlineLengthLhs> const& lhs, BaseString<TChar, TInlineLengthRhs> const& rhs)
		{
			return CString::Strncmp(lhs.CStr(), rhs.CStr(), lhs.GetLength()) <= 0;
		}

		// string > char*
		template<SizeTp TInlineLengthLhs>
		GDINL friend bool operator> (BaseString<TChar, TInlineLengthLhs> const& lhs, TChar const* const rhs)
		{
			return CString::Strncmp(lhs.CStr(), rhs, lhs.GetLength()) > 0;
		}
		template<SizeTp TInlineLengthRhs>
		GDINL friend bool operator> (TChar const* const lhs, BaseString<TChar, TInlineLengthRhs> const& rhs)
		{
			return CString::Strncmp(lhs, rhs.CStr(), rhs.GetLength()) > 0;
		}
		template<SizeTp TInlineLengthLhs>
		GDINL friend bool operator>= (BaseString<TChar, TInlineLengthLhs> const& lhs, TChar const* const rhs)
		{
			return CString::Strncmp(lhs.CStr(), rhs, lhs.GetLength()) >= 0;
		}
		template<SizeTp TInlineLengthRhs>
		GDINL friend bool operator>= (TChar const* const lhs, BaseString<TChar, TInlineLengthRhs> const& rhs)
		{
			return CString::Strncmp(lhs, rhs.CStr(), rhs.GetLength()) >= 0;
		}

		// string < char*
		template<SizeTp TInlineLengthLhs>
		GDINL friend bool operator< (BaseString<TChar, TInlineLengthLhs> const& lhs, TChar const* const rhs)
		{
			return CString::Strncmp(lhs.CStr(), rhs, lhs.GetLength()) < 0;
		}
		template<SizeTp TInlineLengthRhs>
		GDINL friend bool operator< (TChar const* const lhs, BaseString<TChar, TInlineLengthRhs> const& rhs)
		{
			return CString::Strncmp(lhs, rhs.CStr(), rhs.GetLength()) < 0;
		}
		template<SizeTp TInlineLengthLhs>
		GDINL friend bool operator<= (BaseString<TChar, TInlineLengthLhs> const& lhs, TChar const* const rhs)
		{
			return CString::Strncmp(lhs.CStr(), rhs, lhs.GetLength()) <= 0;
		}
		template<SizeTp TInlineLengthRhs>
		GDINL friend bool operator<= (TChar const* const lhs, BaseString<TChar, TInlineLengthRhs> const& rhs)
		{
			return CString::Strncmp(lhs, rhs.CStr(), rhs.GetLength()) <= 0;
		}

		// string + string
		template<SizeTp TInlineLengthLhs, SizeTp TInlineLengthRhs>
		GDINL friend BaseString<TChar, TInlineLengthLhs> operator+ (BaseString<TChar, TInlineLengthLhs> const& lhs, BaseString<TChar, TInlineLengthRhs> const& rhs)
		{
			return lhs.Append(rhs);
		}
		template<SizeTp TInlineLengthLhs>
		GDINL friend BaseString<TChar, TInlineLengthLhs>& operator+= (BaseString<TChar, TInlineLengthLhs>& lhs, BaseString<TChar> const& rhs)
		{
			return lhs = lhs.Append(rhs);
		}

		// string + char*
		template<SizeTp TInlineLengthLhs>
		GDINL friend BaseString<TChar, TInlineLengthLhs> operator+ (BaseString<TChar, TInlineLengthLhs> const& lhs, TChar const* const rhs)
		{
			return lhs.Append(rhs);
		}
		template<SizeTp TInlineLengthRhs>
		GDINL friend BaseString<TChar, TInlineLengthRhs> operator+ (TChar const* const lhs, BaseString<TChar, TInlineLengthRhs> const& rhs)
		{
			return BaseString<TChar>(lhs).Append(rhs);
		}
		template<SizeTp TInlineLengthLhs>
		GDINL friend BaseString<TChar, TInlineLengthLhs>& operator+= (BaseString<TChar, TInlineLengthLhs>& lhs, TChar const* const rhs)
		{
			return lhs = lhs.Append(rhs);
		}

		// string + char
		template<SizeTp TInlineLengthLhs>
		GDINL friend BaseString<TChar, TInlineLengthLhs> operator+ (BaseString<TChar, TInlineLengthLhs> const& lhs, TChar const rhs)
		{
			return lhs.Append(rhs);
		}
		template<SizeTp TInlineLengthLhs>
		GDINL friend BaseString<TChar, TInlineLengthLhs> operator+ (TChar const lhs, BaseString<TChar, TInlineLengthLhs> const& rhs)
		{
			return BaseString<TChar>(lhs).Append(rhs);
		}
		template<SizeTp TInlineLengthLhs>
		GDINL friend BaseString<TChar, TInlineLengthLhs>& operator+= (BaseString<TChar, TInlineLengthLhs>& lhs, TChar const rhs)
		{
			return lhs = lhs.Append(rhs);
		}

	};	// class BaseString

	/*!
	 * A string class used by the engine.
	 */
	using String = BaseString<Char>;
	
	/*!
	 * A wide-character string class used by the engine.
	 */
	using WideString = BaseString<WideChar>;

	/*!
	 * A stack string class used by the engine.
	 */
	template<SizeTp TInlineLength = InlineAllocatorCapacityTiny>
	using InlineString = BaseString<Char, TInlineLength>;

	/*!
	 * A stack wide-character string class used by the engine.
	 */
	template<SizeTp TInlineLength = InlineAllocatorCapacityTiny>
	using InlineWideString = BaseString<WideChar, TInlineLength>;

GD_NAMESPACE_END
