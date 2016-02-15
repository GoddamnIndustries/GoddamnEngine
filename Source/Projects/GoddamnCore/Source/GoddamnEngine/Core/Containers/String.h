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
#include <GoddamnEngine/Core/Algorithm.h>
#include <GoddamnEngine/Core/Templates/Iterators.h>

GD_NAMESPACE_BEGIN

	template<typename TChar>
	struct BaseStringDefaultLength
	{
		UInt32 static const Value = sizeof(TChar*) / sizeof(TChar);
	};	// struct BaseStringDefaultLength

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                               BaseString<T> class.                               ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	// **------------------------------------------------------------------------------------------**
	//! A base string class used by the engine.
	//! @tparam TChar character type of this string.
	// **------------------------------------------------------------------------------------------**
	template<typename TChar, UInt32 TInlineLength = BaseStringDefaultLength<TChar>::Value>
	class BaseString final : public IIteratable<BaseString<TChar, TInlineLength>>
	{
	public:
		using ElementType          = TChar;
		using Iterator             = IndexedContainerIterator<BaseString>;
		using ConstIterator        = IndexedContainerIterator<BaseString const>;
		using ReverseIterator      = ReverseContainerIterator<Iterator>;
		using ReverseConstIterator = ReverseContainerIterator<ConstIterator>;

		GD_CONTAINER_DEFINE_ITERATION_SUPPORT(BaseString);

	private:
		using MyCChar   = BaseCChar<TChar>;
		using MyCString = BaseCString<TChar>;

	private:
		UInt32 static const s_MaxInlineLength = TInlineLength;
		UInt32 m_Length = 0;
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
		{
			m_Length = 0;
			m_InlineMemory[0] = GD_LITERAL(TChar, '\0');
		}

		/*!
		 * Initializes a string with a single character.
		 * @param character Initial String character.
		 */
		GDINL explicit BaseString(Char const character)
		{
			m_Length = 1;
			m_InlineMemory[0] = character;
			m_InlineMemory[1] = GD_LITERAL(TChar, '\0');
		}

		/*!
		 * Fills a String with specified number of characters.
		 *
		 * @param theLength Length of the string.
		 * @param fillWith A character that String would be filled with.
		 */
		GDINL explicit BaseString(UInt32 const theLength, TChar const fillWith = GD_LITERAL(TChar, '\0'))
		{
			m_Length = theLength;
			if (m_Length >= s_MaxInlineLength)
			{
				auto const dataSizeInBytes = (m_Length + 1) * sizeof(TChar);
				m_HeapMemory = reinterpret_cast<TChar*>(GD_MALLOC(dataSizeInBytes));
			}
			if (sizeof(TChar) != 1)
			{
				for (auto& character : (*this))
					character = fillWith;
			}
			else
			{
				CMemory::Memset(CStr(), fillWith, m_Length);
			}
			*End() = GD_LITERAL(TChar, '\0');
		}

		/*!
		 * Initializes a String with some C String with known m_Length.
		 *
		 * @param chars String's initial data.
		 * @param theLength Size of String initial data.
		 */
		GDINL BaseString(TChar const* const chars, UInt32 const theLength)
		{
			GD_DEBUG_ASSERT(chars != nullptr, "Null pointer data specified");
			m_Length = theLength;
			auto const dataSizeInBytes = (m_Length + 1) * sizeof(TChar);
			if (m_Length >= s_MaxInlineLength)
			{
				m_HeapMemory = reinterpret_cast<TChar*>(GD_MALLOC(dataSizeInBytes));
				CMemory::Memcpy(m_HeapMemory, dataSizeInBytes, chars);
			}
			else
			{
				CMemory::Memcpy_s(m_InlineMemory, sizeof(m_InlineMemory), chars, dataSizeInBytes);
			}
		}

		/*!
		 * Initializes a String with some C String with unknown m_Length.
		 * @param chars String initial data.
		 */
		//! @{   
		template<UInt32 const TLength>	
		GDINL implicit BaseString(TChar const(&chars)[TLength])
			: BaseString(chars, TLength - 1)
		{
		}
		GDINL implicit BaseString(TChar const* const chars)
			: BaseString(chars, static_cast<UInt32>(MyCString::Strlen(chars)))
		{
		}
		//! @}

		/*!
		 * Initializes this string with copy of other String.
		 * @param other The other String to copy.
		 */
		//! @{
		GDINL BaseString(BaseString const& other)
			: BaseString(other.CStr(), other.GetLength())
		{
		}
		template<UInt32 TOtherInlineLength>
		GDINL implicit BaseString(BaseString<TChar, TOtherInlineLength> const& other)
			: BaseString(other.CStr(), other.GetLength())
		{
		}
		//! @}

		/*!
		 * Moves other string into this String.
		 * @param other The other String to move into this.
		 */
		GDINL BaseString(BaseString&& other)
		{
			*this = Forward<BaseString>(other);
		}

		GDINL ~BaseString()
		{
			if (m_Length >= s_MaxInlineLength)
			{
				GD_DEALLOC(m_HeapMemory);
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
		GDINL UInt32 GetLength() const
		{
			return m_Length;
		}

		/*!
		 * Returns available size of the string.
		 */
		GDINL SizeTp GetAllocatedSize() const
		{
			if (m_Length >= s_MaxInlineLength)
			{
				return (m_Length + 1) * sizeof(TChar);
			}
			return sizeof(m_InlineMemory);
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
		GDINL TChar GetLast() const
		{
			return *(End() - 1);
		}

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
		 * Concatenates two strings.
		 *
		 * @param other The other string to concatenate with.
		 * @returns result of the string concatenation.
		 */
		//! @{
		GDINL BaseString Concatenate(BaseString const& other) const
		{
			BaseString result(m_Length + other.GetLength());
			CMemory::Memcpy_s(result.CStr(), result.GetAllocatedSize(), CStr(), m_Length * sizeof(TChar));
			CMemory::Memcpy_s(result.CStr() + m_Length, result.GetAllocatedSize(), other.CStr(), other.m_Length * sizeof(TChar));
			return result;
		}
		GDINL BaseString Concatenate(TChar const* const other) const
		{
			SizeTp const otherLength = MyCString::Strlen(other);
			BaseString result(m_Length + static_cast<UInt32>(otherLength));
			CMemory::Memcpy_s(result.CStr(), result.GetAllocatedSize(), CStr(), m_Length * sizeof(TChar));
			CMemory::Memcpy_s(result.CStr() + m_Length, result.GetAllocatedSize(), other, otherLength * sizeof(TChar));
			return result;
		}
		GDINL BaseString Concatenate(TChar const other) const
		{
			BaseString result(m_Length + 1);
			CMemory::Memcpy_s(result.CStr(), result.GetAllocatedSize(), CStr(), m_Length * sizeof(TChar));
			*(result.End() - 1) = other;
			return result;
		}
		//! @}

		/*!
		 * Appends specified string to this string.
		 * @param character The character to Append.
		 */
		//! @{
		//GD_DEPRECATED("Strings are constant objects. For mutable string use 'StringBuilder' class.")
		GDINL BaseString& Append(BaseString const& other)
		{
			BaseString copy(Move(*this));
			return (*this = Move(copy.Concatenate(other)));
		}
		//GD_DEPRECATED("Strings are constant objects. For mutable string use 'StringBuilder' class.")
		GDINL BaseString& Append(TChar const* const other)
		{
			BaseString copy(Move(*this));
			return (*this = Move(copy.Concatenate(other)));
		}
		//GD_DEPRECATED("Strings are constant objects. For mutable string use 'StringBuilder' class.")
		GDINL BaseString& Append(TChar const other)
		{
			BaseString copy(Move(*this));
			return (*this = Move(copy.Concatenate(other)));
		}
		//! @}

		/*!
		 * Prepends specified string to this string.
		 * @param character The character to Append.
		 */
		//! @{
		GD_DEPRECATED("Strings are constant objects. For mutable string use 'StringBuilder' class.")
		GDINL BaseString& Prepend(BaseString const& other)
		{
			BaseString copy(other);
			return (*this = Move(copy.Concatenate(Move(*this))));
		}
		GD_DEPRECATED("Strings are constant objects. For mutable string use 'StringBuilder' class.")
		GDINL BaseString& Prepend(TChar const* const other)
		{
			BaseString copy(other);
			return (*this = Move(copy.Concatenate(Move(*this))));
		}
		GD_DEPRECATED("Strings are constant objects. For mutable string use 'StringBuilder' class.")
		GDINL BaseString& Prepend(TChar const other)
		{
			BaseString copy(other);
			return (*this = Move(copy.Concatenate(Move(*this))));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// Sub-stringing.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Extracts a substring from this string from specified Index to the specified Index.
		 *
		 * @param from Index of the first character that would be extracted to the substring.
		 * @param to Index of the last character that would be extracted to the substring.
		 * @returns A part of this string.
		 */
		//! @{
		GDINL BaseString Substring(UInt32 const from, UInt32 const to) const
		{
			GD_DEBUG_ASSERT(to >= from, "Invalid substring indices.");
			BaseString result(to - from);
			CMemory::Memcpy_s(result.CStr(), result.GetAllocatedSize(), &(*this)[from], (to - from) * sizeof(TChar));
			return result;
		}
		GD_DEPRECATED("Strings are constant objects. For mutable string use 'StringBuilder' class.")
		GDINL BaseString& SubstringSelf(UInt32 const from, UInt32 const to)
		{
			return *this = Forward<BaseString&&>(Substring(from, to));
		}
		//! @}

		/*!
		 * Extracts a substring from this string from specified Index with specified length.
		 *
		 * @param from Index of the first character that would be extracted to the substring.
		 * @param length The length of the desired substring.
		 * @returns A part of this string.
		 */
		//! @{
		GDINL BaseString SubstringLen(UInt32 const from, UInt32 const length) const
		{
			return Substring(from, from + length);
		}
		GD_DEPRECATED("Strings are constant objects. For mutable string use 'StringBuilder' class.")
		GDINL BaseString& SubstringLenSelf(UInt32 const from, UInt32 const length)
		{
			return *this = Forward<BaseString&&>(Substring(from, length));
		}
		//! @}

		/*!
		 * Extracts a substring from this string from specified Index to the end.
		 *
		 * @param from Index of the first character that would be extracted to the substring.
		 * @returns A part of this string.
		 */
		//! @{
		GDINL BaseString Substring(UInt32 const from) const
		{
			return Substring(from, GetLength());
		}
		GDINL BaseString& SubstringSelf(UInt32 const from)
		{
			return *this = Forward<BaseString&&>(Substring(from));
		}
		//! @}

		/*!
		 * Extracts a substring for this string from start to ChopSelfped end.
		 *
		 * @param length Amount of bytes to skip from end.
		 * @returns A part of this string.
		 */
		//! @{
		GDINL BaseString Remove(UInt32 const from, UInt32 const length) const
		{
			return Substring(0, from).Concatenate(Substring(from + length + 1));
		}
		GD_DEPRECATED("Strings are constant objects. For mutable string use 'StringBuilder' class.")
		GDINL BaseString& RemoveFromSelf(UInt32 const from, UInt32 const length)
		{
			return *this = Forward<BaseString&&>(Remove(from, length));
		}
		//! @}

		/*!
		 * Extracts a substring for this string from start to ChopSelfped end.
		 *
		 * @param length Amount of bytes to skip from end.
		 * @returns A part of this string.
		 */
		//! @{
		GDINL BaseString Chop(UInt32 const length) const
		{
			return Substring(0, GetLength() - length);
		}
		GD_DEPRECATED("Strings are constant objects. For mutable string use 'StringBuilder' class.")
		GDINL BaseString& ChopSelf(UInt32 const length)
		{
			return *this = Forward<BaseString&&>(Chop(length));
		}
		//! @}

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
			GD_DEBUG_ASSERT(wildcard != nullptr, "Null pointer wildcard was specified.");

			// Originally written by Jack Handy - <A href="mailto:jakkhandy@hotmail.com">jakkhandy@hotmail.com</A>.
			TChar const* cstring = CStr();
			TChar const* wildcardPtr = wildcard;
			TChar const* charPtr = nullptr;
			TChar const* matchPointer = nullptr;
			while ((*cstring != GD_LITERAL(TChar, '\0')) && (*wildcardPtr != GD_LITERAL(TChar, '*')))
			{
				if ((*wildcardPtr != *cstring) && (*wildcardPtr != GD_LITERAL(TChar, '?')))
					return false;
				++wildcardPtr;
				++cstring;
			}
			while (*cstring != GD_LITERAL(TChar, '\0'))
			{
				if (*wildcardPtr == GD_LITERAL(TChar, '*'))
				{
					if (!*++wildcardPtr)
						return true;
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
				++wildcardPtr;
			return (*wildcardPtr == GD_LITERAL(TChar, '\0'));
		}
		GDINL bool MatchesWildcard(BaseString const& Wildcard) const
		{
			return MatchesWildcard(Wildcard.CStr());
		}
		//! @}

		/*!
		 */
		GDINL BaseString UnwrapEscapeSequences() const
		{
			return *this;
		}


		/*!
		 */
		GDINL BaseString WrapEscapeSequences() const
		{
			return *this;
		}

		/*!
		 * Searches for first occurrence of the character in the string.
		 *
		 * @param character The character we are looking for.
		 * @returns Index of the first occurrence of the character in the string or @c SizeTpMax if nothing was found.
		 */
		GDINL SizeTp Find(TChar const character) const
		{
			TChar const* const cstring = CStr();
			TChar const* const location = MyCString::Strchr(cstring, character);
			if (location != nullptr)
				return static_cast<SizeTp>(location - cstring);
			return SizeTpMax;
		}

		/*!
		 * Searches for first occurrence of the C string in the string.
		 *
		 * @param otherCString C string we are looking for.
		 * @returns Index of the first occurrence of the C string in the string or @c SizeTpMax if nothing was found.
		 */
		GDINL SizeTp Find(TChar const* const otherCString) const
		{
			TChar const* const cstring = CStr();
			TChar const* const location = MyCString::Strstr(cstring, otherCString);
			return location == nullptr ? SizeTpMax : static_cast<SizeTp>(location - cstring);
		}

		/*!
		 * Searches for last occurrence of the character in the string.
		 *
		 * @param character The character we are looking for.
		 * @returns Index of the last occurrence of the character in the string or @c SizeTpMax if nothing was found.
		 */
		GDINL SizeTp ReverseFind(TChar const character) const
		{
			TChar const* const cstring = CStr();
			TChar const* const location = MyCString::Strrchr(cstring, character);
			return location == nullptr ? SizeTpMax : static_cast<SizeTp>(location - cstring);
		}

		/*!
		 * Searches for last occurrence of the C string in the string.
		 *
		 * @param otherCString C string we are looking for.
		 * @returns Index of the last occurrence of the C string in the string or @c SizeTpMax if nothing was found.
		 */
		GDINL SizeTp ReverseFind(TChar const* const otherCString) const
		{
			TChar const* const cstring = CStr();
			TChar const* const location = MyCString::Strrstr(cstring, otherCString);
			return location == nullptr ? SizeTpMax : static_cast<SizeTp>(location - cstring);
		}

		/*!
		 * Returns true if this string starts with specified expression.
		 *
		 * @param other Text we are testing against.
		 * @return True if this string starts with specified expression.
		 */
		//! @{
		GDINL bool StartsWith(BaseString const& other) const
		{
			return MyCString::Strncmp(CStr(), other.GetLength()) == 0;
		}
		GDINL bool StartsWith(TChar const* const other) const
		{
			return MyCString::Strncmp(CStr(), MyCString::Strlen(other)) == 0;
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
		GDINL bool EndsWith(TChar const* const other) const
		{
			//! @todo Implement this code properly.
			return Find(other) == (m_Length - MyCString::Strlen(other));
		}
		GDINL bool EndsWith(TChar const other) const
		{
			return *(End() - 1) == other;
		}
		//! @}

		/*!
		 * Returns boolean representation of this string.
		 *
		 * @param succeeded Would be written true if conversion succeeded.
		 * @param notation The notation in which value is represented.
		 * @returns Boolean representation of this string.
		 */
		GDINL bool ToBool(bool* const succeeded = nullptr) const
		{
			if (*this == GD_LITERAL(TChar, "0") || *this == GD_LITERAL(TChar, "false"))
			{
				if (succeeded != nullptr)
					*succeeded = true;
				return false;
			}
			if (*this == GD_LITERAL(TChar, "1") || *this == GD_LITERAL(TChar, "true"))
			{
				if (succeeded != nullptr)
					*succeeded = true;
				return true;
			}
			if (succeeded != nullptr)
				*succeeded = false;
			return false;
		}

		/*!
		 * Returns integer representation of this string.
		 *
		 * @param succeeded Would be written true if conversion succeeded.
		 * @param notation The notation in which value is represented.
		 * @returns Integer representation of this string.
		 */
		GDINL Int64 ToInt64(bool* const succeeded = nullptr, SizeTp const notation = 0) const
		{
			TChar const* endPtr = nullptr;
			Int64 const value = MyCString::Strtoi64(CStr(), &endPtr, notation);
			if (succeeded != nullptr)
				*succeeded = endPtr == &*End();
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
			Float64 const value = MyCString::Strtof64(CStr(), &endPtr);
			if (succeeded != nullptr)
				*succeeded = endPtr == &*End();
			return value;
		}

		/*!
		 * Returns lowercased version of this string.
		 */
		GDINL BaseString ToUpper() const
		{
			BaseString result(*this);
			for (auto& character : result)
			{
				character = static_cast<TChar>(MyCChar::ToUpper(character));
			}
			return result;
		}

		/*!
		 * Returns uppercased version of this string.
		 */
		GDINL BaseString ToLower() const
		{
			BaseString result(*this);
			for (auto& character : result)
			{
				character = static_cast<TChar>(MyCChar::ToLower(character));
			}
			return result;
		}

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
			MyCString::Vsnprintf(&buffer[0], GD_ARRAY_LENGTH(buffer), format, list);
			return BaseString(buffer);
		}

		/*!
		 * Formats a specified string using C-like formatting.
		 * @param format Initial format of the string.
		 */
		GDINL static BaseString Format(TChar const* const format, ...)
		{
			va_list List; 
			va_start(List, format);
			return BaseString::FormatVa(format, List);
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
			this->~BaseString();
			m_Length = other.m_Length;
			auto const dataSizeInBytes = (m_Length + 1) * sizeof(TChar);
			if (m_Length >= s_MaxInlineLength)
			{
				m_HeapMemory = reinterpret_cast<TChar*>(GD_MALLOC(dataSizeInBytes));
				CMemory::Memcpy_s(m_HeapMemory, dataSizeInBytes, other.CStr(), dataSizeInBytes);
			}
			else
			{
				CMemory::Memcpy_s(&m_InlineMemory[0], sizeof(m_InlineMemory), other.CStr(), dataSizeInBytes);
			}
			return *this;
		}
		GDINL BaseString& operator= (BaseString&& other)
		{
			this->~BaseString();
			m_Length = other.m_Length;
			other.m_Length = 0;
			if (m_Length >= s_MaxInlineLength)
			{
				m_HeapMemory = other.m_HeapMemory;
				other.m_HeapMemory = nullptr;
			}
			else
			{
				auto const dataSizeInBytes = (m_Length + 1) * sizeof(TChar);
				CMemory::Memcpy_s(m_InlineMemory, sizeof(m_InlineMemory), &other.m_InlineMemory[0], dataSizeInBytes);
			}
			return *this;
		}

		// string[]
		GDINL TChar const& operator[] (SizeTp const index) const
		{
			GD_DEBUG_ASSERT(index <= GetLength(), "Invalid String subindex.");
			return *(CStr() + index);
		}
		GDINL TChar& operator[] (SizeTp const index)
		{
			return const_cast<TChar&>(const_cast<BaseString const&>(*this)[index]);
		}

		// string == string
		template<UInt32 TInlineLengthLHS, UInt32 TInlineLengthRHS>
		GDINL friend static bool operator== (BaseString<TChar, TInlineLengthLHS> const& lhs, BaseString<TChar, TInlineLengthRHS> const& rhs)
		{
			return lhs.GetLength() == rhs.GetLength() ? BaseCString<TChar>::Strncmp(lhs.CStr(), rhs.CStr(), lhs.GetLength()) == 0 : false;
		}

		template<UInt32 TInlineLengthLHS, UInt32 TInlineLengthRHS>
		GDINL friend static bool operator!= (BaseString<TChar, TInlineLengthLHS> const& lhs, BaseString<TChar, TInlineLengthRHS> const& rhs)
		{
			return !(lhs == rhs);
		}

		// string == char*
		template<UInt32 TInlineLengthLHS>
		GDINL friend static bool operator== (BaseString<TChar, TInlineLengthLHS> const& lhs, TChar const* const rhs)
		{
			return BaseCString<TChar>::Strncmp(lhs.CStr(), rhs, lhs.GetLength()) == 0;
		}
		template<UInt32 TInlineLengthRHS>
		GDINL friend static bool operator== (TChar const* const lhs, BaseString<TChar, TInlineLengthRHS> const& rhs)
		{
			return rhs == lhs;
		}

		template<UInt32 TInlineLengthLHS>
		GDINL friend static bool operator!= (BaseString<TChar, TInlineLengthLHS> const& lhs, TChar const* const rhs)
		{
			return !(lhs == rhs);
		}
		template<UInt32 TInlineLengthRHS>
		GDINL friend static bool operator!= (TChar const* const lhs, BaseString<TChar, TInlineLengthRHS> const& rhs)
		{
			return !(lhs == rhs);
		}

		// string == char
		template<UInt32 TInlineLengthLHS>
		GDINL friend static bool operator== (BaseString<TChar, TInlineLengthLHS> const& lhs, TChar const rhs)
		{
			return lhs.GetLength() == 1 ? *lhs.CStr() == rhs : false;
		}
		template<UInt32 TInlineLengthRHS>
		GDINL friend static bool operator== (TChar const lhs, BaseString<TChar, TInlineLengthRHS> const& rhs)
		{
			return rhs.GetLength() == 1 ? *rhs.CStr() == lhs : false;
		}

		template<UInt32 TInlineLengthLHS>
		GDINL friend static bool operator!= (BaseString<TChar, TInlineLengthLHS> const& lhs, TChar const rhs)
		{
			return !(lhs == rhs);
		}
		template<UInt32 TInlineLengthRHS>
		GDINL friend static bool operator!= (TChar const lhs, BaseString<TChar, TInlineLengthRHS> const& rhs)
		{
			return rhs != lhs;
		}

		// string > string
		template<UInt32 TInlineLengthLHS, UInt32 TInlineLengthRHS>
		GDINL friend static bool operator> (BaseString<TChar, TInlineLengthLHS> const& lhs, BaseString<TChar, TInlineLengthRHS> const& rhs)
		{
			return MyCString::Strncmp(lhs.CStr(), rhs.CStr(), lhs.GetLength()) > 0;
		}
		template<UInt32 TInlineLengthLHS, UInt32 TInlineLengthRHS>
		GDINL friend static bool operator>= (BaseString<TChar, TInlineLengthLHS> const& lhs, BaseString<TChar, TInlineLengthRHS> const& rhs)
		{
			return MyCString::Strncmp(lhs.CStr(), rhs.CStr(), lhs.GetLength()) >= 0;
		}

		// string < string
		template<UInt32 TInlineLengthLHS, UInt32 TInlineLengthRHS>
		GDINL friend static bool operator< (BaseString<TChar, TInlineLengthLHS> const& lhs, BaseString<TChar, TInlineLengthRHS> const& rhs)
		{
			return MyCString::Strncmp(lhs.CStr(), rhs.CStr(), lhs.GetLength()) < 0;
		}
		template<UInt32 TInlineLengthLHS, UInt32 TInlineLengthRHS>
		GDINL friend static bool operator<= (BaseString<TChar, TInlineLengthLHS> const& lhs, BaseString<TChar, TInlineLengthRHS> const& rhs)
		{
			return MyCString::Strncmp(lhs.CStr(), rhs.CStr(), lhs.GetLength()) <= 0;
		}

		// string > char*
		template<UInt32 TInlineLengthLHS>
		GDINL friend static bool operator> (BaseString<TChar, TInlineLengthLHS> const& lhs, TChar const* const rhs)
		{
			return MyCString::Strncmp(lhs.CStr(), rhs, lhs.GetLength()) > 0;
		}
		template<UInt32 TInlineLengthRHS>
		GDINL friend static bool operator> (TChar const* const lhs, BaseString<TChar, TInlineLengthRHS> const& rhs)
		{
			return MyCString::Strncmp(lhs, rhs.CStr(), rhs.GetLength()) > 0;
		}
		template<UInt32 TInlineLengthLHS>
		GDINL friend static bool operator>= (BaseString<TChar, TInlineLengthLHS> const& lhs, TChar const* const rhs)
		{
			return MyCString::Strncmp(lhs.CStr(), rhs, lhs.GetLength()) >= 0;
		}
		template<UInt32 TInlineLengthRHS>
		GDINL friend static bool operator>= (TChar const* const lhs, BaseString<TChar, TInlineLengthRHS> const& rhs)
		{
			return MyCString::Strncmp(lhs, rhs.CStr(), rhs.GetLength()) >= 0;
		}

		// string < char*
		template<UInt32 TInlineLengthLHS>
		GDINL friend static bool operator< (BaseString<TChar, TInlineLengthLHS> const& lhs, TChar const* const rhs)
		{
			return MyCString::Strncmp(lhs.CStr(), rhs, lhs.GetLength()) < 0;
		}
		template<UInt32 TInlineLengthRHS>
		GDINL friend static bool operator< (TChar const* const lhs, BaseString<TChar, TInlineLengthRHS> const& rhs)
		{
			return MyCString::Strncmp(lhs, rhs.CStr(), rhs.GetLength()) < 0;
		}
		template<UInt32 TInlineLengthLHS>
		GDINL friend static bool operator<= (BaseString<TChar, TInlineLengthLHS> const& lhs, TChar const* const rhs)
		{
			return MyCString::Strncmp(lhs.CStr(), rhs, lhs.GetLength()) <= 0;
		}
		template<UInt32 TInlineLengthRHS>
		GDINL friend static bool operator<= (TChar const* const lhs, BaseString<TChar, TInlineLengthRHS> const& rhs)
		{
			return MyCString::Strncmp(lhs, rhs.CStr(), rhs.GetLength()) <= 0;
		}

		// string + string
		template<UInt32 TInlineLengthLHS, UInt32 TInlineLengthRHS>
		GDINL friend static BaseString<TChar> operator+ (BaseString<TChar, TInlineLengthLHS> const& lhs, BaseString<TChar, TInlineLengthRHS> const& rhs)
		{
			return lhs.Concatenate(rhs);
		}

	};	// class BaseString

	template<typename TChar>
	GDINL static BaseString<TChar> operator+ (BaseString<TChar> const& lhs, TChar const* const rhs)
	{
		return lhs.Concatenate(rhs);
	}
	template<typename TChar>
	GDINL static BaseString<TChar> operator+ (TChar const* const lhs, BaseString<TChar> const& rhs)
	{
		return BaseString<TChar>(lhs).Concatenate(rhs);
	}

	template<typename TChar>
	GDINL static BaseString<TChar> operator+ (BaseString<TChar> const& lhs, TChar const rhs)
	{
		return lhs.Concatenate(rhs);
	}
	template<typename TChar>
	GDINL static BaseString<TChar> operator+ (TChar const lhs, BaseString<TChar> const& rhs)
	{
		return BaseString<TChar>(lhs).Concatenate(rhs);
	}

	template<typename TChar>
	GDINL static BaseString<TChar>& operator+= (BaseString<TChar>& lhs, BaseString<TChar> const& rhs)
	{
		return lhs.Append(rhs);
	}
	template<typename TChar>
	GDINL static BaseString<TChar>& operator+= (BaseString<TChar>& lhs, TChar const* const rhs)
	{
		return lhs.Append(rhs);
	}
	template<typename TChar>
	GDINL static BaseString<TChar>& operator+= (BaseString<TChar>& lhs, TChar const rhs)
	{
		return lhs.Append(rhs);
	}

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
	template<UInt32 TInlineLength>
	using InlineString = BaseString<Char, TInlineLength>;

	/*!
	 * A stack wide-character string class used by the engine.
	 */
	template<UInt32 TInlineLength>
	using InlineWideString = BaseString<WideChar, TInlineLength>;

GD_NAMESPACE_END
