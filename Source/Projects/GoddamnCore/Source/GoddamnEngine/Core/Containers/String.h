// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
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
#include <GoddamnEngine/Core/Iterators.h>

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// BaseString<T> class.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! A base string class used by the engine.
	//! @tparam TChar character type of this string.
	template<typename TChar>
	class BaseString final
	{
	public:
		using CharType                = TChar;
		using ElementType             = CharType;
		using Iterator                = IndexedContainerIterator<BaseString>;
		using ConstIterator           = IndexedContainerIterator<BaseString const>;
		using ReverseIterator         = ReverseContainerIterator<Iterator>;
		using ReverseConstIterator    = ReverseContainerIterator<ConstIterator>;
	
		GD_CONTAINER_DEFINE_ITERATION_SUPPORT(BaseString);

	private:
		using CChar   = BaseCChar<TChar>;
		using CString = BaseCString<TChar>;

	private:
#if !GD_RELEASE && GD_PLATFORM_WINDOWS && _MSC_VER >= 1900 && _MSC_VER <= 2000
		
		// On Windows, while debugging with VisualStudio, NATVIS is unable to catch the static constant variable 's_MaxInlineLength'.
		// So we have to cache it in the local variable.
		SizeTp static const _s_MaxInlineLength = sizeof(CharType*) / sizeof(CharType);
		SizeTp m_Length = 0;
		union {
			CharType  m_InlineMemory[_s_MaxInlineLength];
			CharType* m_HeapMemory;
		};	// anonymous union

		UInt8 const s_MaxInlineLength = _s_MaxInlineLength;

#else	// !GD_RELEASE && GD_PLATFORM_WINDOWS && _MSC_VER >= 1900 && _MSC_VER <= 2000

		SizeTp static const s_MaxInlineLength = sizeof(CharType*) / sizeof(CharType);
		SizeTp m_Length = 0;
		union {
			CharType  m_InlineMemory[s_MaxInlineLength];
			CharType* m_HeapMemory;
		};	// anonymous union

#endif	// !GD_RELEASE && GD_PLATFORM_WINDOWS && _MSC_VER >= 1900

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructors, initializers & destructor.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Initializes an empty string.
		GDINL BaseString()
		{
			m_Length = 0;
			m_InlineMemory[0] = GD_LITERAL(CharType, '\0');
		}

		// ------------------------------------------------------------------------------------------
		//! Initializes a string with a single character.
		//! @param character Initial String character.
		GDINL explicit BaseString(Char const character)
		{
			m_Length = 1;
			m_InlineMemory[0] = character;
			m_InlineMemory[1] = GD_LITERAL(CharType, '\0');
		}

		// ------------------------------------------------------------------------------------------
		//! Fills a String with specified number of characters.
		//! @param theLength A m_Length of the string.
		//! @param fillWith A character that String would be filled with.
		GDINL explicit BaseString(SizeTp const theLength, CharType const fillWith = GD_LITERAL(CharType, '\0'))
		{
			m_Length = theLength;
			if (m_Length >= s_MaxInlineLength)
			{
				auto const dataSizeInBytes = (m_Length + 1) * sizeof(CharType);
				m_HeapMemory = reinterpret_cast<CharType*>(GD_MALLOC(dataSizeInBytes));
			}
			if (sizeof(CharType) != 1)
			{
				for (auto& character : (*this))
					character = fillWith;
			}
			else
			{
				CMemory::Memset(CStr(), fillWith, m_Length);
			}
			*End() = GD_LITERAL(CharType, '\0');
		}

		// ------------------------------------------------------------------------------------------
		//! Initializes a String with some C String with known m_Length.
		//! @param chars String's initial data.
		//! @param theLength Size of String initial data.
		GDINL BaseString(CharType const* const chars, SizeTp const theLength)
		{
			GD_DEBUG_ASSERT(chars != nullptr, "Null pointer data specified");
			m_Length = theLength;
			auto const dataSizeInBytes = (m_Length + 1) * sizeof(CharType);
			if (m_Length >= s_MaxInlineLength)
			{
				m_HeapMemory = reinterpret_cast<CharType*>(GD_MALLOC(dataSizeInBytes));
				CMemory::Memcpy(m_HeapMemory, dataSizeInBytes, chars);
			}
			else
			{
				CMemory::Memcpy_s(&m_InlineMemory[0], sizeof(m_InlineMemory), chars, dataSizeInBytes);
			}
		}

		// ------------------------------------------------------------------------------------------
		//! Initializes a String with some C String with unknown m_Length.
		//! @param chars String initial data.
		//! @{   
		template <SizeTp const TLength>	// m_Length of string constant is determined at compile time..
		GDINL implicit BaseString(CharType const(&chars)[TLength])
			: BaseString(&chars[0], TLength - 1)
		{
		}
		GDINL implicit BaseString(CharType const* const chars)
			: BaseString(chars, CString::Strlen(chars))
		{
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Initializes this string with copy of other String.
		//! @param other The other String to copy.
		GDINL BaseString(BaseString const& other)
			: BaseString(other.CStr(), other.GetLength())
		{
		}

		// ------------------------------------------------------------------------------------------
		//! Moves other string into this String.
		//! @param other The other String to move into this.
		GDINL BaseString(BaseString&& other)
		{
			*this = Forward<BaseString>(other);
		}

		// ------------------------------------------------------------------------------------------
		//! Deinitializes string.
		GDINL ~BaseString()
		{
			if (m_Length >= s_MaxInlineLength)
			{
				GD_DEALLOC(m_HeapMemory);
				m_HeapMemory = nullptr;
			}
			else
			{
				m_InlineMemory[0] = GD_LITERAL(CharType, '\0');
			}
			m_Length = 0;
		}

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Iteration API.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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
	
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Dynamic size management.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Returns m_Length of this string.
		//! @returns Size of this String.
		GDINL SizeTp GetLength() const
		{
			return m_Length;
		}

		// ------------------------------------------------------------------------------------------
		//! Returns available size of the string.
		//! @returns Available size of the string.
		GDINL SizeTp GetAllocatedSize() const
		{
			if (m_Length >= s_MaxInlineLength)
				return (m_Length + 1) * sizeof(CharType);
			return sizeof(m_InlineMemory);
		}

		// ------------------------------------------------------------------------------------------
		//! Returns true if this string is empty.
		//! @returns True if this String is empty, false otherwise.
		GDINL bool IsEmpty() const
		{
			return m_Length == 0;
		}

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Dynamic elements access & API.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Returns pointer on this string.
		//! @returns C-String version of this object.
		//! @{
		GDINL CharType* CStr() { return const_cast<CharType*>(const_cast<BaseString const*>(this)->CStr()); }
		GDINL CharType const* CStr() const
		{
			if (GetLength() >= s_MaxInlineLength)
				return m_HeapMemory;
			return m_InlineMemory;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns last character of the string.
		//! @returns Last character of the string.
		GDINL CharType GetLast() const
		{
			return *(End() - 1);
		}

		// ------------------------------------------------------------------------------------------
		//! Returns unique signature of the string.
		//! @returns Constant-time comparable unique signature of the string. 
		GDINL HashCode GetHashCode() const
		{
			HashValue computed = 0;
			for (auto const character : (*this))
				computed = 65599 * computed + character;
			return HashCode(computed >> 16 ^ computed);
		}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Concatenations.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Concatenates two strings.
		//! @param other The other string to concatenate with.
		//! @returns result of the string concatenation.
		//! @{
		GDINL BaseString Concatenate(BaseString const& other) const
		{
			BaseString result(m_Length + other.GetLength());
			CMemory::Memcpy_s(result.CStr(), result.GetAllocatedSize(), CStr(), m_Length * sizeof(CharType));
			CMemory::Memcpy_s(result.CStr() + m_Length, result.GetAllocatedSize(), other.CStr(), other.m_Length * sizeof(CharType));
			return result;
		}
		GDINL BaseString Concatenate(CharType const* const other) const
		{
			SizeTp const otherLength = CString::Strlen(other);
			BaseString result(m_Length + otherLength);
			CMemory::Memcpy_s(result.CStr(), result.GetAllocatedSize(), CStr(), m_Length * sizeof(CharType));
			CMemory::Memcpy_s(result.CStr() + m_Length, result.GetAllocatedSize(), other, otherLength * sizeof(CharType));
			return result;
		}
		GDINL BaseString Concatenate(CharType const other) const
		{
			BaseString result(m_Length + 1);
			CMemory::Memcpy_s(result.CStr(), result.GetAllocatedSize(), CStr(), m_Length * sizeof(CharType));
			*(result.End() - 1) = other;
			return result;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Appends specified string to this string.
		//! @param character The character to Append.
		//! @todo Implement this code properly.
		//! @{
		GDINL BaseString& Append(BaseString const& other)
		{
			BaseString copy(Move(*this));
			return (*this = Move(copy.Concatenate(other)));
		}
		GDINL BaseString& Append(CharType const* const other)
		{
			BaseString copy(Move(*this));
			return (*this = Move(copy.Concatenate(other)));
		}
		GDINL BaseString& Append(CharType const other)
		{
			BaseString copy(Move(*this));
			return (*this = Move(copy.Concatenate(other)));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Prepends specified string to this string.
		//! @param character The character to Append.
		//! @todo Implement this code properly.
		//! @{
		GDINL BaseString& Prepend(BaseString const& other)
		{
			BaseString copy(other);
			return (*this = Move(copy.Concatenate(Move(*this))));
		}
		GDINL BaseString& Prepend(CharType const* const other)
		{
			BaseString copy(other);
			return (*this = Move(copy.Concatenate(Move(*this))));
		}
		GDINL BaseString& Prepend(CharType const other)
		{
			BaseString copy(other);
			return (*this = Move(copy.Concatenate(Move(*this))));
		}
		//! @}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Sub-stringing.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Extracts a substring from this string from specified Index to the specified Index.
		//! @param from Index of the first character that would be extracted to the substring.
		//! @param to Index of the last character that would be extracted to the substring.
		//! @returns A part of this string.
		//! @{
		GDINL BaseString Substring(SizeTp const from, SizeTp const to) const
		{
			GD_DEBUG_ASSERT(to >= from, "Invalid substring indices.");
			BaseString result(to - from);
			CMemory::Memcpy_s(result.CStr(), result.GetAllocatedSize(), &(*this)[from], (to - from) * sizeof(CharType));
			return result;
		}
		GDINL BaseString& SubstringSelf(SizeTp const from, SizeTp const to)
		{
			return *this = Forward<BaseString&&>(Substring(from, to));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Extracts a substring from this string from specified Index with specified length.
		//! @param from Index of the first character that would be extracted to the substring.
		//! @param length The length of the desired substring.
		//! @returns A part of this string.
		//! @{
		GDINL BaseString SubstringL(SizeTp const from, SizeTp const length) const
		{
			return Substring(from, from + length);
		}
		GDINL BaseString& SubstringLSelf(SizeTp const from, SizeTp const length)
		{
			return *this = Forward<BaseString&&>(Substring(from, length));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Extracts a substring from this string from specified Index to the end.
		//! @param from Index of the first character that would be extracted to the substring.
		//! @returns A part of this string.
		//! @{
		GDINL BaseString Substring(SizeTp const from) const
		{
			return Substring(from, GetLength());
		}
		GDINL BaseString& SubstringSelf(SizeTp const from) 
		{
			return *this = Forward<BaseString&&>(Substring(from));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Extracts a substring fro this string from start to ChopSelfped end.
		//! @param m_Length Amount of bytes to skip from end.
		//! @returns A part of this string.
		//! @{
		GDINL BaseString Remove(SizeTp const from, SizeTp const length) const
		{
			return Substring(0, from).Concatenate(Substring(from + length + 1));
		}
		GDINL BaseString& RemoveFromSelf(SizeTp const from, SizeTp const length)
		{
			return *this = Forward<BaseString&&>(Remove(from, length));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Extracts a substring fro this string from start to ChopSelfped end.
		//! @param m_Length Amount of bytes to skip from end.
		//! @returns A part of this string.
		//! @{
		GDINL BaseString Chop(SizeTp const length) const
		{
			return Substring(0, GetLength() - length);
		}
		GDINL BaseString& ChopSelf(SizeTp const length)
		{
			return *this = Forward<BaseString&&>(Chop(length));
		}
		//! @}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Miscellaneous.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Matches a string against a wildcard string.
		//! @param Wildcard The wildcard string.
		//! @returns @c true in matches, @c false otherwise.
		//! @{
		GDINL bool MatchesWildcard(CharType const* const Wildcard) const 
		{
			// Originally written by Jack Handy - <A href="mailto:jakkhandy@hotmail.com">jakkhandy@hotmail.com</A>.
			CharType const* cstring = CStr();
			CharType const* wildcardPtr = Wildcard;
			CharType const* charPtr = nullptr;
			CharType const* matchPointer = nullptr;
			while ((*cstring != GD_LITERAL(CharType, '\0')) && (*wildcardPtr != GD_LITERAL(CharType, '*')))
			{
				if ((*wildcardPtr != *cstring) && (*wildcardPtr != GD_LITERAL(CharType, '?')))
					return false;
				++wildcardPtr;
				++cstring;
			}
			while (*cstring != GD_LITERAL(CharType, '\0'))
			{
				if (*wildcardPtr == GD_LITERAL(CharType, '*'))
				{
					if (!*++wildcardPtr)
						return true;
					matchPointer = wildcardPtr;
					charPtr = cstring + 1;
				}
				else if ((*wildcardPtr == *cstring) || (*wildcardPtr == GD_LITERAL(CharType, '?')))
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
			while (*wildcardPtr == GD_LITERAL(CharType, '*'))
				++wildcardPtr;
			return (*wildcardPtr == GD_LITERAL(CharType, '\0'));
		}
		GDINL bool MatchesWildcard(BaseString const& Wildcard) const
		{
			return MatchesWildcard(Wildcard.CStr());
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		GDINL BaseString UnwrapEscapeSequences() const
		{
			return *this;
		}


		// ------------------------------------------------------------------------------------------
		GDINL BaseString WrapEscapeSequences() const
		{
			return *this;
		}

		// ------------------------------------------------------------------------------------------
		//! Searches for first occurrence of the character in the string.
		//! @param character The character we are looking for.
		//! @returns Index of the first occurrence of the character in the string or @c SizeTpMax if nothing was found.
		GDINL SizeTp Find(CharType const character) const
		{
			CharType const* const cstring = CStr();
			CharType const* const location = CString::Strchr(cstring, character);
			if (location != nullptr)
				return static_cast<SizeTp>(location - cstring);
			return SizeTpMax;
		}

		// ------------------------------------------------------------------------------------------
		//! Searches for first occurrence of the C string in the string.
		//! @param otherCString C string we are looking for.
		//! @returns Index of the first occurrence of the C string in the string or @c SizeTpMax if nothing was found.
		GDINL SizeTp Find(CharType const* const otherCString) const
		{
			CharType const* const cstring = CStr();
			CharType const* const location = CString::Strstr(cstring, otherCString);
			if (location != nullptr)
				return static_cast<SizeTp>(location - cstring);
			return SizeTpMax;
		}

		// ------------------------------------------------------------------------------------------
		//!	Searches for last occurrence of the character in the string.
		//! @param character The character we are looking for.
		//! @returns Index of the last occurrence of the character in the string or @c SizeTpMax if nothing was found.
		GDINL SizeTp ReverseFind(CharType const character) const
		{
			CharType const* const cstring = CStr();
			CharType const* const location = CString::Strrchr(cstring, character);
			if (location != nullptr)
				return static_cast<SizeTp>(location - cstring);
			return SizeTpMax;
		}

		// ------------------------------------------------------------------------------------------
		//!	Searches for last occurrence of the C string in the string.
		//! @param otherCString C string we are looking for.
		//! @returns Index of the last occurrence of the C string in the string or @c SizeTpMax if nothing was found.
		GDINL SizeTp ReverseFind(CharType const* const otherCString) const
		{
			CharType const* const cstring = CStr();
			CharType const* const location = CString::Strrstr(cstring, otherCString);
			if (location != nullptr)
				return static_cast<SizeTp>(location - cstring);
			return SizeTpMax;
		}

		// ------------------------------------------------------------------------------------------
		//! Returns true if this string starts with specified expression.
		//! @param otherCString C string we are testing against.
		//! @return True if this string starts with specified expression.
		//! @{
		GDINL bool StartsWith(CharType const* const otherCString) const
		{
			//! @todo Implement this code properly.
			return Find(otherCString) == 0;
		}
		GDINL bool StartsWith(CharType const character) const
		{
			return *CStr() == character;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns true if this string ends with specified expression.
		//! @param otherCString C string we are testing against.
		//! @return True if this string ends with specified expression.
		//! @{
		GDINL bool EndsWith(CharType const* const otherCString) const
		{
			//! @todo Implement this code properly.
			return Find(otherCString) == (m_Length - CString::Strlen(otherCString));
		}
		GDINL bool EndsWith(CharType const character) const
		{
			return *(End() - 1) == character;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns integer representation of this string.
		//! @param succeeded Would be written true if this is an integer.
		//! @param notation The notation in which value is represented.
		//! @returns Integer representation of this string.
		GDINL Int64 ToInt64(bool* const succeeded = nullptr, SizeTp const notation = 0) const
		{
			CharType const* endPtr = nullptr;
			Int64 const value = CString::Strtoi64(CStr(), &endPtr, notation);
			if (succeeded != nullptr)
				*succeeded = endPtr == &*End();
			return value;
		}

		// ------------------------------------------------------------------------------------------
		//! Returns lowercased version of this string.
		//! @returns Lowercased version of this string.
		GDINL BaseString ToUpper() const
		{
			BaseString result(*this);
			for (auto& character : result)
				character = static_cast<CharType>(CChar::ToUpper(character));
			return result;
		}

		// ------------------------------------------------------------------------------------------
		//! Returns uppercased version of this string.
		//! @returns Uppercased version of this string.
		GDINL BaseString ToLower() const
		{
			BaseString result(*this);
			for (auto& character : result)
				character = static_cast<CharType>(CChar::ToLower(character));
			return result;
		}

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Formatting.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Formats a specified string using C-like formatting.
		//! @param format Initial format of the string.
		//! @param list Varying arguments list for formatting.
		GDINL static BaseString FormatVa(CharType const* const format, va_list list)
		{
			CharType buffer[32000];
			CMemory::Memset(&buffer[0], 0, sizeof(buffer));
			CString::Vsnprintf(&buffer[0], GD_ARRAY_LENGTH(buffer), format, list);
			return BaseString(buffer);
		}

		// ------------------------------------------------------------------------------------------
		//! Formats a specified string using C-like formatting.
		//! @param format Initial format of the string.
		GDINL static BaseString Format(CharType const* const format, ...)
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

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Overloaded operators.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		GDINL BaseString& operator= (BaseString const& other)
		{
			this->~BaseString();
			m_Length = other.m_Length;
			auto const dataSizeInBytes = (m_Length + 1) * sizeof(CharType);
			if (m_Length >= s_MaxInlineLength)
			{
				m_HeapMemory = reinterpret_cast<CharType*>(GD_MALLOC(dataSizeInBytes));
				CMemory::Memcpy_s(m_HeapMemory, dataSizeInBytes, other.CStr(), dataSizeInBytes);
			}
			else
			{
				CMemory::Memcpy_s(&m_InlineMemory[0], GD_ARRAY_LENGTH(m_InlineMemory), other.CStr(), dataSizeInBytes);
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
				auto const dataSizeInBytes = (m_Length + 1) * sizeof(CharType);
				CMemory::Memcpy_s(m_InlineMemory, sizeof(m_InlineMemory), &other.m_InlineMemory[0], dataSizeInBytes);
			}
			return *this;
		}

		GDINL CharType const& operator[] (SizeTp const index) const
		{
			GD_DEBUG_ASSERT(index <= GetLength(), "Invalid String subindex.");
			return *(CStr() + index);
		}
		GDINL CharType& operator[] (SizeTp const index)
		{
			return const_cast<Char&>(const_cast<BaseString const&>(*this)[index]);
		}
	};	// class BaseString

	template<typename CharType>
	GDINL static bool operator== (BaseString<CharType> const& lhs, BaseString<CharType> const& rhs)
	{
		if (lhs.GetLength() == rhs.GetLength())
			return (CString::Strncmp(lhs.CStr(), rhs.CStr(), lhs.GetLength()) == 0);
		return false;
	}
	template<typename CharType>
	GDINL static bool operator== (BaseString<CharType> const& lhs, CharType const* const rhs)
	{
		return (CString::Strncmp(lhs.CStr(), rhs, lhs.GetLength()) == 0);
	}
	template<typename CharType>
	GDINL static bool operator== (CharType const* const lhs, BaseString<CharType> const& rhs)
	{
		return (CString::Strncmp(lhs, rhs.CStr(), rhs.GetLength()) == 0);
	}

	template<typename CharType>
	GDINL static bool operator!= (BaseString<CharType> const& lhs, BaseString<CharType> const & rhs)
	{
		return !(lhs == rhs);
	}
	template<typename CharType>
	GDINL static bool operator!= (BaseString<CharType> const& lhs, CharType const* const rhs)
	{
		return !(lhs == rhs);
	}

	template<typename CharType>
	GDINL static bool operator== (BaseString<CharType> const& lhs, CharType const character)
	{
		if (lhs.GetLength() == 1)
			return (*lhs.CStr() == character);
		return false;
	}
	template<typename CharType>
	GDINL static bool operator!= (BaseString<CharType> const& lhs, CharType const character)
	{
		return !(lhs == character);
	}

	template<typename CharType>
	GDINL static bool operator> (BaseString<CharType> const& lhs, BaseString<CharType> const& rhs)
	{
		return (CString::Strncmp(lhs.CStr(), rhs.CStr(), lhs.GetLength()) > 0);
	}
	template<typename CharType>
	GDINL static bool operator< (BaseString<CharType> const& lhs, BaseString<CharType> const& rhs)
	{
		return (CString::Strncmp(lhs.CStr(), rhs.CStr(), lhs.GetLength()) < 0);
	}

	
	template<typename CharType>
	GDINL static BaseString<CharType> operator+ (BaseString<CharType> const& lhs, BaseString<CharType> const& rhs)
	{
		return lhs.Concatenate(rhs);
	}
	
	template<typename CharType>
	GDINL static BaseString<CharType> operator+ (BaseString<CharType> const& lhs, CharType const* const rhs)
	{
		return lhs.Concatenate(rhs);
	}
	template<typename CharType>
	GDINL static BaseString<CharType> operator+ (CharType const* const lhs, BaseString<CharType> const& rhs)
	{
		return BaseString<CharType>(lhs).Concatenate(rhs);
	}
	
	template<typename CharType>
	GDINL static BaseString<CharType> operator+ (BaseString<CharType> const& lhs, CharType const rhs)
	{
		return lhs.Concatenate(rhs);
	}
	template<typename CharType>
	GDINL static BaseString<CharType> operator+ (CharType const lhs, BaseString<CharType> const& rhs)
	{
		return BaseString<CharType>(lhs).Concatenate(rhs);
	}

	template<typename CharType>
	GDINL static BaseString<CharType>& operator+= (BaseString<CharType>& lhs, BaseString<CharType> const& rhs)
	{
		return lhs.Append(rhs);
	}
	template<typename CharType>
	GDINL static BaseString<CharType>& operator+= (BaseString<CharType>& lhs, CharType const* const rhs)
	{
		return lhs.Append(rhs);
	}
	template<typename CharType>
	GDINL static BaseString<CharType>& operator+= (BaseString<CharType>& lhs, CharType const rhs)
	{
		return lhs.Append(rhs);
	}

	using String = BaseString<Char>;			//!< A string class used by the engine.
	using WideString = BaseString<WideChar>;	//!< A wide-character string class used by the engine.

GD_NAMESPACE_END
