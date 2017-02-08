// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
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
#include <GoddamnEngine/Core/Templates/Algorithm.h>
#include <GoddamnEngine/Core/Templates/Iterators.h>

GD_NAMESPACE_BEGIN

	template<typename TChar>
	struct BaseStringDefaultLength
	{
		SizeTp static const Value = sizeof(TChar*) / sizeof(TChar);
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

		SizeTp static constexpr Npos = SizeTpMax;

		GD_CONTAINER_DEFINE_ITERATION_SUPPORT(BaseString);

	private:
		using MyCChar   = BaseCChar<TChar>;
		using MyCString = BaseCString<TChar>;

	private:
		SizeTp static constexpr s_MaxInlineLength = TInlineLength;
		SizeTp m_Length = 0;
		union {
			TChar  m_InlineMemory[s_MaxInlineLength];
			TChar* m_HeapMemory;
		};	// anonymous union

	public:

		// ------------------------------------------------------------------------------------------
		// Constructors, initializers & destructor.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Initializes an empty string.
		 */
		GDINL BaseString()
			: m_Length(0)
			, m_InlineMemory{ GD_LITERAL(TChar, '\0') }
		{
		}

		/*!
		 * Initializes a string with a single character.
		 * @param character Initial String character.
		 */
		GDINL explicit BaseString(Char const character)
			: m_Length(1)
			, m_InlineMemory{ character, GD_LITERAL(TChar, '\0') }
		{
		}

		/*!
		 * Fills a string with specified number of characters.
		 *
		 * @param theLength Length of the string.
		 * @param fillWith A character that String would be filled with.
		 */
		GDINL explicit BaseString(SizeTp const theLength, TChar const fillWith = GD_LITERAL(TChar, '\0'))
			: m_Length(theLength)
		{
			if (m_Length >= s_MaxInlineLength)
			{
				m_HeapMemory = CMemory::TMemset(GD_MALLOC_ARRAY_T(TChar, m_Length + 1), fillWith, m_Length);
				m_HeapMemory[m_Length] = GD_LITERAL(TChar, '\0');
			}
			else
			{
				CMemory::TMemset(m_InlineMemory, fillWith, m_Length);
				m_InlineMemory[m_Length] = GD_LITERAL(TChar, '\0');
			}
		}

		/*!
		 * Initializes a string with some C String with known length.
		 *
		 * @param chars String's initial data.
		 * @param theLength Size of String initial data.
		 */
		GDINL BaseString(TChar const* const chars, SizeTp const theLength)
			: m_Length(theLength)
		{
			GD_DEBUG_VERIFY(chars != nullptr, "Null pointer data specified");
			if (m_Length >= s_MaxInlineLength)
			{
				m_HeapMemory = CMemory::TMemcpy(GD_MALLOC_ARRAY_T(TChar, m_Length + 1), chars, m_Length);
				m_HeapMemory[m_Length] = GD_LITERAL(TChar, '\0');
			}
			else
			{
				CMemory::TMemcpy(m_InlineMemory, chars, m_Length);
				m_InlineMemory[m_Length] = GD_LITERAL(TChar, '\0');
			}
		}

		/*!
		 * Initializes a string with some C String.
		 * @param chars String initial data.
		 */
		//! @{   
		template<SizeTp const TLength>	
		GDINL implicit BaseString(TChar const(&chars)[TLength])
			: BaseString(chars, TLength - 1)
		{
		}
		GDINL implicit BaseString(TChar const* const chars)
			: BaseString(chars, static_cast<SizeTp>(MyCString::Strlen(chars)))
		{
		}
		//! @}

		/*!
		 * Initializes this string with copy of other string.
		 * @param other The other string to copy.
		 */
		//! @{
		GDINL BaseString(BaseString const& other)
			: BaseString(other.CStr(), other.GetLength())
		{
		}
		template<SizeTp TOtherInlineLength>
		GDINL implicit BaseString(BaseString<TChar, TOtherInlineLength> const& other)
			: BaseString(other.CStr(), other.GetLength())
		{
		}
		//! @}

		/*!
		 * Moves other string into this string.
		 * @param other The other string to move into this.
		 */
		//! @{
		GDINL BaseString(BaseString&& other) noexcept
			: m_Length(other.m_Length)
		{
			CMemory::TMemcpy(m_InlineMemory, other.m_InlineMemory, sizeof other.m_InlineMemory);
			CMemory::TMemset(other.m_InlineMemory, static_cast<TChar>(0), sizeof other.m_InlineMemory);
			other.m_Length = 0;
		}
		template<SizeTp TOtherInlineLength>
		GDINL implicit BaseString(typename EnableIf<TOtherInlineLength <= TInlineLength, BaseString<TChar, TOtherInlineLength>>::Value&& other)
			: m_Length(other.m_Length)
		{
			CMemory::TMemcpy(m_InlineMemory, other.m_InlineMemory, sizeof other.m_InlineMemory);
			CMemory::TMemset(other.m_InlineMemory, static_cast<TChar>(0), sizeof other.m_InlineMemory);
			other.m_Length = 0;
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
				m_InlineMemory[0] = GD_LITERAL(TChar, '\0');
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
		GDINL SizeTp GetLength() const
		{
			return m_Length;
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
		 * @returns C-String version of this object.
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
		 * @param other The other string to append.
		 */
		//! @{
		GDINL BaseString Append(TChar const* const other, SizeTp const theLength) const
		{
			BaseString result(m_Length + theLength);
			CMemory::TMemcpy(result.CStr(), CStr(), m_Length);
			CMemory::TMemcpy(result.CStr() + m_Length, other, theLength);
			return result;
		}
		template<SizeTp TOtherInlineLength>
		GDINL BaseString Append(BaseString<TChar, TOtherInlineLength> const& other) const
		{
			return this->Append(other.CStr(), other.m_Length);
		}
		GDINL BaseString Append(TChar const* const other) const
		{
			return this->Append(other, static_cast<SizeTp>(MyCString::Strlen(other)));
		}
		GDINL BaseString Append(TChar const other) const
		{
			return this->Append(&other, 1);
		}
		//! @}

		/*!
		 * Prepends specified string to this string.
		 * @param other The other string to prepend.
		 */
		//! @{
		GDINL BaseString Prepend(TChar const* const other, SizeTp const theLength) const
		{
			BaseString result(m_Length + theLength);
			CMemory::TMemcpy(result.CStr(), other, theLength);
			CMemory::TMemcpy(result.CStr() + theLength, CStr(), m_Length);
			return result;
		}
		template<SizeTp TOtherInlineLength>
		GDINL BaseString Prepend(BaseString<TChar, TOtherInlineLength> const& other) const
		{
			return this->Prepend(other.CStr(), other.m_Length);
		}
		GDINL BaseString Prepend(TChar const* const other) const
		{
			return this->Prepend(other, static_cast<SizeTp>(MyCString::Strlen(other)));
		}
		GDINL BaseString Prepend(TChar const other) const
		{
			return this->Prepend(&other, 1);
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
			GD_DEBUG_VERIFY(to >= from, "Invalid substring indices.");
			GD_DEBUG_VERIFY(to < m_Length, "Invalid substring indices.");
			GD_DEBUG_VERIFY(from < m_Length, "Invalid substring indices.");

			BaseString result(to - from + 1);
			CMemory::TMemcpy(result.CStr(), CStr() + from, result.m_Length * sizeof(TChar));
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
		// Finds.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Searches for first occurrence of the character in the string.
		 *
		 * @param character The character we are looking for.
		 * @returns Index of the first occurrence of the character in the string or @c Npos if nothing was found.
		 */
		GDINL SizeTp Find(TChar const character) const
		{
			auto const cstring = CStr();
			auto const location = MyCString::Strchr(cstring, character);
			return location != nullptr ? static_cast<SizeTp>(location - cstring) : Npos;
		}

		/*!
		 * Searches for first occurrence of the specified string in the string.
		 *
		 * @param otherCString C string we are looking for.
		 * @returns Index of the first occurrence of the C string in the string or @c Npos if nothing was found.
		 */
		//! @{
		GDINL SizeTp Find(TChar const* const otherCString) const
		{
			auto const cstring = CStr();
			auto const location = MyCString::Strstr(cstring, otherCString);
			return location != nullptr ? static_cast<SizeTp>(location - cstring) : Npos;
		}
		template<SizeTp TOtherInlineLength>
		GDINL SizeTp Find(BaseString<TChar, TOtherInlineLength> const& other) const
		{
			return this->Find(other.CStr());
		}
		//! @}

		/*!
		 * Searches for last occurrence of the character in the string.
		 *
		 * @param character The character we are looking for.
		 * @returns Index of the last occurrence of the character in the string or @c Npos if nothing was found.
		 */
		GDINL SizeTp ReverseFind(TChar const character) const
		{
			auto const cstring = CStr();
			auto const location = MyCString::Strrchr(cstring, character);
			return location != nullptr ? static_cast<SizeTp>(location - cstring) : Npos;
		}

		/*!
		 * Searches for last occurrence of the C string in the string.
		 *
		 * @param otherCString C string we are looking for.
		 * @returns Index of the last occurrence of the C string in the string or @c Npos if nothing was found.
		 */
		//! @{
		GDINL SizeTp ReverseFind(TChar const* const otherCString) const
		{
			auto const cstring = CStr();
			auto const location = MyCString::Strrstr(cstring, otherCString);
			return location != nullptr ? static_cast<SizeTp>(location - cstring) : Npos;
		}
		template<SizeTp TOtherInlineLength>
		GDINL SizeTp ReverseFind(BaseString<TChar, TOtherInlineLength> const& other) const
		{
			return this->ReverseFind(other.CStr());
		}
		//! @}

		/*!
		 * Returns true if this string starts with specified expression.
		 *
		 * @param other Text we are testing against.
		 * @return True if this string starts with specified expression.
		 */
		//! @{
		GDINL bool StartsWith(TChar const* const other, SizeTp const theLength) const
		{
			return theLength <= m_Length && MyCString::Strncmp(CStr(), other, theLength) == 0;
		}
		template<SizeTp TOtherInlineLength>
		GDINL bool StartsWith(BaseString<TChar, TOtherInlineLength> const& other) const
		{
			return this->StartsWith(other.CStr(), other.m_Length);
		}
		GDINL bool StartsWith(TChar const* const other) const
		{
			return this->StartsWith(other, static_cast<SizeTp>(MyCString::Strlen(other)));
		}
		GDINL bool StartsWith(TChar const other) const
		{
			return *CStr() == other;
		}
		//! @}

		/*!
		 * Returns true if this string ends with specified expression.
		 *
		 * @param other C string we are testing against.
		 * @return True if this string ends with specified expression.
		 */
		//! @{
		GDINL bool EndsWith(TChar const* const other, SizeTp const theLength) const
		{
			return theLength <= m_Length && MyCString::Strncmp(CStr() + m_Length - theLength, other, theLength) == 0;
		}
		template<SizeTp TOtherInlineLength>
		GDINL bool EndsWith(BaseString<TChar, TOtherInlineLength> const& other) const
		{
			return this->EndsWith(other.CStr(), other.m_Length);
		}
		GDINL bool EndsWith(TChar const* const other) const
		{
			return this->EndsWith(other, static_cast<SizeTp>(MyCString::Strlen(other)));
		}
		GDINL bool EndsWith(TChar const other) const
		{
			return *(End() - 1) == other;
		}
		//! @}

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
				ch = MyCChar::ToUpper(ch);
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
				ch = MyCChar::ToLower(ch);
			}
			return result;
		}

		/*!
		 * Returns boolean representation of this string.
		 *
		 * @param succeeded Would be written true if conversion succeeded.
		 * @returns Boolean representation of this string.
		 */
		GDINL bool ToBool(bool* const succeeded = nullptr) const
		{
			if (*this == GD_LITERAL(TChar, "0") || *this == GD_LITERAL(TChar, "false"))
			{
				if (succeeded != nullptr)
				{
					*succeeded = true;
				}
				return false;
			}
			if (*this == GD_LITERAL(TChar, "1") || *this == GD_LITERAL(TChar, "true"))
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
		 * Returns integer representation of this string.
		 *
		 * @param succeeded Would be written true if conversion succeeded.
		 * @param radix The radix in which value is represented.
		 *
		 * @returns Integer representation of this string.
		 */
		GDINL Int64 ToInt64(bool* const succeeded = nullptr, SizeTp const radix = 0) const
		{
			TChar const* endPtr = nullptr;
			auto const value = MyCString::Strtoi64(CStr(), &endPtr, radix);
			if (succeeded != nullptr)
			{
				*succeeded = endPtr == &*End();
			}
			return value;
		}

		/*!
		 * Returns double representation of this string.
		 *
		 * @param succeeded Would be written true if conversion succeeded.
		 * @returns Double representation of this string.
		 */
		GDINL Float64 ToFloat64(bool* const succeeded = nullptr) const
		{
			TChar const* endPtr = nullptr;
			auto const value = MyCString::Strtof64(CStr(), &endPtr);
			if (succeeded != nullptr)
			{
				*succeeded = endPtr == &*End();
			}
			return value;
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
			GD_DEBUG_VERIFY(wildcard != nullptr, "Null pointer wildcard was specified.");

			// Originally written by Jack Handy - <A href="mailto:jakkhandy@hotmail.com">jakkhandy@hotmail.com</A>.
			TChar const* cstring = CStr();
			TChar const* wildcardPtr = wildcard;
			TChar const* charPtr = nullptr;
			TChar const* matchPointer = nullptr;
			while ((*cstring != GD_LITERAL(TChar, '\0')) && (*wildcardPtr != GD_LITERAL(TChar, '*')))
			{
				if ((*wildcardPtr != *cstring) && (*wildcardPtr != GD_LITERAL(TChar, '?')))
				{
					return false;
				}
				++wildcardPtr;
				++cstring;
			}
			while (*cstring != GD_LITERAL(TChar, '\0'))
			{
				if (*wildcardPtr == GD_LITERAL(TChar, '*'))
				{
					if (!*++wildcardPtr)
					{
						return true;
					}
					matchPointer = wildcardPtr;
					charPtr = cstring + 1;
				}
				else if ((*wildcardPtr == *cstring) || (*wildcardPtr == GD_LITERAL(TChar, '?')))
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
			while (*wildcardPtr == GD_LITERAL(TChar, '*'))
			{
				++wildcardPtr;
			}
			return (*wildcardPtr == GD_LITERAL(TChar, '\0'));
		}
		template<SizeTp TOtherInlineLength>
		GDINL bool MatchesWildcard(BaseString<TChar, TOtherInlineLength> const& Wildcard) const
		{
			return MatchesWildcard(Wildcard.CStr());
		}
		//! @}

	public:

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
			MyCString::Vsnprintf_s(buffer, GD::GetLength(buffer), format, list);
			return BaseString(buffer);
		}

		/*!
		 * Formats a specified string using C-like formatting.
		 * @param format Initial format of the string.
		 */
		GDINL static BaseString Format(TChar const* const format, ...)
		{
			va_list list; 
			va_start(list, format);
			auto const string = BaseString::FormatVa(format, list);
			va_end(list);
			return string;
		}

		GDINL static BaseString FromFloat64(Float64 const value)
		{
			return Format("%f", value);
		}

	public:

		// ------------------------------------------------------------------------------------------
		// Overloaded operators.
		// ------------------------------------------------------------------------------------------

		// string = string
		GDINL BaseString& operator= (BaseString const& other)
		{
			if (this != &other)
			{
				this->~BaseString();
				new (this) BaseString(other);
			}
			return *this;
		}
		template<SizeTp TOtherInlineLength>
		GDINL BaseString& operator= (BaseString<TChar, TOtherInlineLength> const& other)
		{
			if (this != &other)
			{
				this->~BaseString();
				new (this) BaseString(other);
			}
			return *this;
		}
		GDINL BaseString& operator= (BaseString&& other) noexcept
		{
			if (this != &other)
			{
				this->~BaseString();
				new (this) BaseString(Utils::Forward<BaseString>(other));
			}
			return *this;
		}
		template<SizeTp TOtherInlineLength>
		GDINL BaseString& operator= (typename EnableIf<TOtherInlineLength <= TInlineLength, BaseString<TChar, TOtherInlineLength>>::Value&& other)
		{
			if (this != &other)
			{
				this->~BaseString();
				new (this) BaseString(Utils::Forward<BaseString<TChar, TOtherInlineLength>>(other));
			}
			return *this;
		}

		// string[]
		GDINL TChar const& operator[] (SizeTp const index) const
		{
			GD_DEBUG_VERIFY(index <= GetLength(), "Invalid string subindex.");
			return *(CStr() + index);
		}
		GDINL TChar& operator[] (SizeTp const index)
		{
			return const_cast<TChar&>(const_cast<BaseString const&>(*this)[index]);
		}

		// string == string
		template<SizeTp TInlineLengthLHS, SizeTp TInlineLengthRHS>
		GDINL friend bool operator== (BaseString<TChar, TInlineLengthLHS> const& lhs, BaseString<TChar, TInlineLengthRHS> const& rhs)
		{
			return lhs.GetLength() == rhs.GetLength() && BaseCString<TChar>::Strncmp(lhs.CStr(), rhs.CStr(), lhs.GetLength()) == 0;
		}
		template<SizeTp TInlineLengthLHS, SizeTp TInlineLengthRHS>
		GDINL friend bool operator!= (BaseString<TChar, TInlineLengthLHS> const& lhs, BaseString<TChar, TInlineLengthRHS> const& rhs)
		{
			return lhs.GetLength() != rhs.GetLength() || BaseCString<TChar>::Strncmp(lhs.CStr(), rhs.CStr(), lhs.GetLength()) != 0;
		}

		// string == char*
		template<SizeTp TInlineLengthLHS>
		GDINL friend bool operator== (BaseString<TChar, TInlineLengthLHS> const& lhs, TChar const* const rhs)
		{
			return BaseCString<TChar>::Strncmp(lhs.CStr(), rhs, lhs.GetLength()) == 0;
		}
		template<SizeTp TInlineLengthRHS>
		GDINL friend bool operator== (TChar const* const lhs, BaseString<TChar, TInlineLengthRHS> const& rhs)
		{
			return rhs == lhs;
		}

		template<SizeTp TInlineLengthLHS>
		GDINL friend bool operator!= (BaseString<TChar, TInlineLengthLHS> const& lhs, TChar const* const rhs)
		{
			return !(lhs == rhs);
		}
		template<SizeTp TInlineLengthRHS>
		GDINL friend bool operator!= (TChar const* const lhs, BaseString<TChar, TInlineLengthRHS> const& rhs)
		{
			return !(lhs == rhs);
		}

		// string == char
		template<SizeTp TInlineLengthLHS>
		GDINL friend bool operator== (BaseString<TChar, TInlineLengthLHS> const& lhs, TChar const rhs)
		{
			return lhs.GetLength() == 1 ? *lhs.CStr() == rhs : false;
		}
		template<SizeTp TInlineLengthRHS>
		GDINL friend bool operator== (TChar const lhs, BaseString<TChar, TInlineLengthRHS> const& rhs)
		{
			return rhs.GetLength() == 1 ? *rhs.CStr() == lhs : false;
		}

		template<SizeTp TInlineLengthLHS>
		GDINL friend bool operator!= (BaseString<TChar, TInlineLengthLHS> const& lhs, TChar const rhs)
		{
			return !(lhs == rhs);
		}
		template<SizeTp TInlineLengthRHS>
		GDINL friend bool operator!= (TChar const lhs, BaseString<TChar, TInlineLengthRHS> const& rhs)
		{
			return rhs != lhs;
		}

		// string > string
		template<SizeTp TInlineLengthLHS, SizeTp TInlineLengthRHS>
		GDINL friend bool operator> (BaseString<TChar, TInlineLengthLHS> const& lhs, BaseString<TChar, TInlineLengthRHS> const& rhs)
		{
			return MyCString::Strncmp(lhs.CStr(), rhs.CStr(), lhs.GetLength()) > 0;
		}
		template<SizeTp TInlineLengthLHS, SizeTp TInlineLengthRHS>
		GDINL friend bool operator>= (BaseString<TChar, TInlineLengthLHS> const& lhs, BaseString<TChar, TInlineLengthRHS> const& rhs)
		{
			return MyCString::Strncmp(lhs.CStr(), rhs.CStr(), lhs.GetLength()) >= 0;
		}

		// string < string
		template<SizeTp TInlineLengthLHS, SizeTp TInlineLengthRHS>
		GDINL friend bool operator< (BaseString<TChar, TInlineLengthLHS> const& lhs, BaseString<TChar, TInlineLengthRHS> const& rhs)
		{
			return MyCString::Strncmp(lhs.CStr(), rhs.CStr(), lhs.GetLength()) < 0;
		}
		template<SizeTp TInlineLengthLHS, SizeTp TInlineLengthRHS>
		GDINL friend bool operator<= (BaseString<TChar, TInlineLengthLHS> const& lhs, BaseString<TChar, TInlineLengthRHS> const& rhs)
		{
			return MyCString::Strncmp(lhs.CStr(), rhs.CStr(), lhs.GetLength()) <= 0;
		}

		// string > char*
		template<SizeTp TInlineLengthLHS>
		GDINL friend bool operator> (BaseString<TChar, TInlineLengthLHS> const& lhs, TChar const* const rhs)
		{
			return MyCString::Strncmp(lhs.CStr(), rhs, lhs.GetLength()) > 0;
		}
		template<SizeTp TInlineLengthRHS>
		GDINL friend bool operator> (TChar const* const lhs, BaseString<TChar, TInlineLengthRHS> const& rhs)
		{
			return MyCString::Strncmp(lhs, rhs.CStr(), rhs.GetLength()) > 0;
		}
		template<SizeTp TInlineLengthLHS>
		GDINL friend bool operator>= (BaseString<TChar, TInlineLengthLHS> const& lhs, TChar const* const rhs)
		{
			return MyCString::Strncmp(lhs.CStr(), rhs, lhs.GetLength()) >= 0;
		}
		template<SizeTp TInlineLengthRHS>
		GDINL friend bool operator>= (TChar const* const lhs, BaseString<TChar, TInlineLengthRHS> const& rhs)
		{
			return MyCString::Strncmp(lhs, rhs.CStr(), rhs.GetLength()) >= 0;
		}

		// string < char*
		template<SizeTp TInlineLengthLHS>
		GDINL friend bool operator< (BaseString<TChar, TInlineLengthLHS> const& lhs, TChar const* const rhs)
		{
			return MyCString::Strncmp(lhs.CStr(), rhs, lhs.GetLength()) < 0;
		}
		template<SizeTp TInlineLengthRHS>
		GDINL friend bool operator< (TChar const* const lhs, BaseString<TChar, TInlineLengthRHS> const& rhs)
		{
			return MyCString::Strncmp(lhs, rhs.CStr(), rhs.GetLength()) < 0;
		}
		template<SizeTp TInlineLengthLHS>
		GDINL friend bool operator<= (BaseString<TChar, TInlineLengthLHS> const& lhs, TChar const* const rhs)
		{
			return MyCString::Strncmp(lhs.CStr(), rhs, lhs.GetLength()) <= 0;
		}
		template<SizeTp TInlineLengthRHS>
		GDINL friend bool operator<= (TChar const* const lhs, BaseString<TChar, TInlineLengthRHS> const& rhs)
		{
			return MyCString::Strncmp(lhs, rhs.CStr(), rhs.GetLength()) <= 0;
		}

		// string + string
		template<SizeTp TInlineLengthLHS, SizeTp TInlineLengthRHS>
		GDINL friend BaseString<TChar, TInlineLengthLHS> operator+ (BaseString<TChar, TInlineLengthLHS> const& lhs, BaseString<TChar, TInlineLengthRHS> const& rhs)
		{
			return lhs.Append(rhs);
		}
		template<SizeTp TInlineLengthLHS>
		GDINL friend BaseString<TChar, TInlineLengthLHS>& operator+= (BaseString<TChar, TInlineLengthLHS>& lhs, BaseString<TChar> const& rhs)
		{
			return lhs = lhs.Append(rhs);
		}

		// string + char*
		template<SizeTp TInlineLengthLHS>
		GDINL friend BaseString<TChar, TInlineLengthLHS> operator+ (BaseString<TChar, TInlineLengthLHS> const& lhs, TChar const* const rhs)
		{
			return lhs.Append(rhs);
		}
		template<SizeTp TInlineLengthRHS>
		GDINL friend BaseString<TChar, TInlineLengthRHS> operator+ (TChar const* const lhs, BaseString<TChar, TInlineLengthRHS> const& rhs)
		{
			return BaseString<TChar>(lhs).Append(rhs);
		}
		template<SizeTp TInlineLengthLHS>
		GDINL friend BaseString<TChar, TInlineLengthLHS>& operator+= (BaseString<TChar, TInlineLengthLHS>& lhs, TChar const* const rhs)
		{
			return lhs = lhs.Append(rhs);
		}

		// string + char
		template<SizeTp TInlineLengthLHS>
		GDINL friend BaseString<TChar, TInlineLengthLHS> operator+ (BaseString<TChar, TInlineLengthLHS> const& lhs, TChar const rhs)
		{
			return lhs.Append(rhs);
		}
		template<SizeTp TInlineLengthLHS>
		GDINL friend BaseString<TChar, TInlineLengthLHS> operator+ (TChar const lhs, BaseString<TChar, TInlineLengthLHS> const& rhs)
		{
			return BaseString<TChar>(lhs).Append(rhs);
		}
		template<SizeTp TInlineLengthLHS>
		GDINL friend BaseString<TChar, TInlineLengthLHS>& operator+= (BaseString<TChar, TInlineLengthLHS>& lhs, TChar const rhs)
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
