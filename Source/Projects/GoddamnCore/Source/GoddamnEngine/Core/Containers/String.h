// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Containers/String.h
//! Constantly sized string class.
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
	//! @tparam CharTypeTp Character type of this string.
	template<typename CharTypeTp>
	class BaseString final
	{
	public:
		using CharType                = CharTypeTp;
		using ElementType             = CharTypeTp;
		using PtrIterator             = ElementType*;
		using PtrConstIterator        = ElementType const*;
		using Iterator                = IndexedContainerIterator<BaseString>;
		using ConstIterator           = IndexedContainerIterator<BaseString const>;
		using ReverseIterator         = ReverseContainerIterator<Iterator>;
		using ReverseConstIterator    = ReverseContainerIterator<ConstIterator>;
		using ReversePtrIterator      = ReverseContainerIterator<PtrIterator>;
		using ReversePtrConstIterator = ReverseContainerIterator<PtrConstIterator>;
	
		GD_CONTAINER_DEFINE_PTR_ITERATION_SUPPORT(BaseString);

	private:
		using CChar                   = BaseCChar<CharTypeTp>;
		using CString                 = BaseCString<CharTypeTp>;

	private:
		template<typename SomeCharType>
		GDINL friend void Swap(BaseString<SomeCharType>& First, BaseString<SomeCharType>& Second);

	private:
		SizeTp _Length = 0;
		SizeTp static const _MaxStackLength = 2 * (sizeof(CharType*) / sizeof(CharType));
		static_assert((_MaxStackLength >= 2) && (_MaxStackLength >= sizeof(CharType*)), "'_MaxStackLength' has invalid size");
		union {
			CharType _StackMemory[_MaxStackLength];
			CharType* _HeapMemory;
		};	// anonymous union

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructors, initializers & destructor.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Initializes an empty string.
		GDINL BaseString()
		{
			this->_Length = 0;
			this->_StackMemory[0] = GD_LITERAL(CharType, '\0');
		}

		// ------------------------------------------------------------------------------------------
		//! Initializes a string with a single character.
		//! @param Character Initial String character.
		GDINL explicit BaseString(Char const Character)
		{
			this->_Length = 1;
			this->_StackMemory[0] = Character;
			this->_StackMemory[1] = GD_LITERAL(CharType, '\0');
		}

		// ------------------------------------------------------------------------------------------
		//! Fills a String with specified number of characters.
		//! @param Size A length of the string.
		//! @param FillWith A character that String would be filled with.
		GDINL explicit BaseString(SizeTp const Length, CharType const FillWith = GD_LITERAL(CharType, '\0'))
		{
			this->_Length = Length;
			if (this->_Length >= BaseString::_MaxStackLength)
			{
				auto const DataSizeInBytes = (Length + 1) * sizeof(CharType);
				this->_HeapMemory = reinterpret_cast<CharType*>(GD_MALLOC(DataSizeInBytes));
			}
			if (sizeof(CharType) != 1)
			{
				for (auto& Character : (*this))
					Character = FillWith;
			}
			else
			{
				CMemory::Memset(this->CStr(), FillWith, Length);
			}
			*this->_PtrEnd() = GD_LITERAL(CharType, '\0');
		}

		// ------------------------------------------------------------------------------------------
		//! Initializes a String with some C String with known length.
		//! @param Chars String's initial data.
		//! @param Size Size of String initial data.
		GDINL BaseString(CharType const* const Chars, SizeTp const Length)
		{
			GD_DEBUG_ASSERT(Chars != nullptr, "Null pointer data specified");
			this->_Length = Length;
			auto const DataSizeInBytes = (Length + 1) * sizeof(CharType);
			if (this->_Length >= BaseString::_MaxStackLength)
			{
				this->_HeapMemory = reinterpret_cast<CharType*>(GD_MALLOC(DataSizeInBytes));
				CMemory::Memcpy(this->_HeapMemory, DataSizeInBytes, Chars, DataSizeInBytes);
			}
			else
			{
				CMemory::Memcpy(&this->_StackMemory[0], GD_ARRAY_LENGTH(this->_StackMemory), Chars, DataSizeInBytes);
			}
		}

		// ------------------------------------------------------------------------------------------
		//! Initializes a String with some C String with unknown length.
		//! @param Chars String initial data.
		//! @{   
		template <SizeTp const Length>	// _Length of string constant is determined at compile time..
		GDINL BaseString(CharType const(&Chars)[Length])
			: BaseString(&Chars[0], Length - 1)
		{
		}
		GDINL BaseString(CharType const* const Chars)
			: BaseString(Chars, CString::Strlen(Chars))
		{
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Initializes this string with copy of other String.
		//! @param Other Other String to copy.
		GDINL BaseString(BaseString const& Other)
			: BaseString(Other.CStr(), Other.GetLength())
		{
		}

		// ------------------------------------------------------------------------------------------
		//! Moves other string into this String.
		//! @param Other Other String to move into this.
		GDINL BaseString(BaseString&& Other)
		{
			*this = Forward<BaseString>(Other);
		}

		// ------------------------------------------------------------------------------------------
		//! Deinitializes string.
		GDINL ~BaseString()
		{
			if (this->_Length >= _MaxStackLength)
			{
				GD_DEALLOC(this->_HeapMemory);
				this->_HeapMemory = nullptr;
			}
			else
			{
				this->_StackMemory[0] = GD_LITERAL(CharType, '\0');
			}
			this->_Length = 0;
		}

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Iteration API.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Returns iterator that points to first container element.
		//! @returns Iterator that points to first container element.
		//! @{
		GDINL Iterator Begin()
		{
			return Iterator(*this);
		}
		GDINL ConstIterator Begin() const
		{
			return ConstIterator(*this);
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns iterator that points to past the end container element.
		//! @returns Iterator that points to past the end container element.
		//! @{
		GDINL Iterator End()
		{
			return this->Begin() + this->_Length;
		}
		GDINL ConstIterator End() const
		{
			return this->Begin() + this->_Length;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns iterator that points to last container element.
		//! @returns Iterator that points to last container element.
		//! @{
	public:
		GDINL ReverseIterator ReverseBegin()
		{
			return ReverseIterator(this->End() - 1);
		}
		GDINL ReverseConstIterator ReverseBegin() const
		{
			return ReverseConstIterator(this->End() - 1);
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns iterator that points to preceding the first container element.
		//! @returns Iterator that points to preceding the first container element.
		//! @{
	public:
		GDINL ReverseIterator ReverseEnd()
		{
			return ReverseIterator(this->Begin() - 1);
		}
		GDINL ReverseConstIterator ReverseEnd() const
		{
			return ReverseConstIterator(this->Begin() - 1);
		}
		//! @}

	private:

		GDINL PtrIterator _PtrBegin()
		{
			return reinterpret_cast<PtrIterator>(this->CStr());
		}
		GDINL PtrConstIterator _PtrBegin() const
		{
			return reinterpret_cast<PtrConstIterator>(this->CStr());
		}

		GDINL PtrIterator _PtrEnd()
		{
			return this->_PtrBegin() + this->_Length;
		}
		GDINL PtrConstIterator _PtrEnd() const
		{
			return this->_PtrBegin() + this->_Length;
		}

		GDINL ReversePtrIterator _ReversePtrBegin()
		{
			return ReversePtrIterator(this->_PtrEnd() - 1);
		}
		GDINL ReversePtrConstIterator _ReversePtrBegin() const
		{
			return ReversePtrConstIterator(this->_PtrEnd() - 1);
		}

		GDINL ReversePtrIterator _ReversePtrEnd()
		{
			return ReversePtrIterator(this->_PtrBegin() - 1);
		}
		GDINL ReversePtrConstIterator ReversePtrEnd() const
		{
			return ReversePtrConstIterator(this->_PtrBegin() - 1);
		}

	public:
	
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Dynamic size management.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Returns length of this string.
		//! @returns Size of this String.
		GDINL SizeTp GetLength() const
		{
			return this->_Length;
		}

		// ------------------------------------------------------------------------------------------
		//! Returns available size of the container.
		//! @returns Available size of the container.
		GDINL SizeTp GetAllocatedSize() const
		{
			if (this->GetLength() >= BaseString::_MaxStackLength)
				return (this->_Length + 1) * sizeof(CharType);
			return GD_ARRAY_LENGTH(this->_StackMemory);
		}

		// ------------------------------------------------------------------------------------------
		//! Returns true if this string is empty.
		//! @returns True if this String is empty, false otherwise.
		GDINL bool IsEmpty() const
		{
			return this->_Length == 0;
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
			if (this->GetLength() >= BaseString::_MaxStackLength)
				return this->_HeapMemory;
			return &this->_StackMemory[0];
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns last character of the string.
		//! @returns Last character of the string.
		GDINL CharType GetLastElement() const
		{
			return *(this->_PtrEnd() - 1);
		}

		// ------------------------------------------------------------------------------------------
		//! Returns unique signature of the string.
		//! @returns Constant-time comparable unique signature of the string. 
		GDINL HashCode GetHashCode() const
		{
			HashValueType Computed = 0;
			for (auto const MyCharacter : (*this))
				Computed = 65599 * Computed + MyCharacter;
			return HashCode((Computed >> 16) ^ Computed);
		}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Concatenations.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Concatenates two strings.
		//! @param Other Other string to concatenate with.
		//! @returns Result of the string concatenation.
		//! @{
		GDINL BaseString Concatenate(BaseString const& Other) const
		{
			BaseString Result(this->_Length + Other.GetLength());
			CMemory::Memcpy(Result.CStr(), Result.GetAllocatedSize(), this->CStr(), this->_Length * sizeof(CharType));
			CMemory::Memcpy(Result.CStr() + this->_Length, Result.GetAllocatedSize(), Other.CStr(), Other._Length * sizeof(CharType));
			return Result;
		}
		GDINL BaseString Concatenate(CharType const* const Other) const
		{
			SizeTp const OtherLength = CString::Strlen(Other);
			BaseString Result(this->_Length + OtherLength);
			CMemory::Memcpy(Result.CStr(), Result.GetAllocatedSize(), this->CStr(), this->_Length * sizeof(CharType));
			CMemory::Memcpy(Result.CStr() + this->_Length, Result.GetAllocatedSize(), Other, OtherLength * sizeof(CharType));
			return Result;
		}
		GDINL BaseString Concatenate(CharType const Other) const
		{
			BaseString Result(this->_Length + 1);
			CMemory::Memcpy(Result.CStr(), Result.GetAllocatedSize(), this->CStr(), this->_Length * sizeof(CharType));
			*(Result.End() - 1) = Other;
			return Result;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Appends specified string to this string.
		//! @param Character Character to Append.
		//! @todo Implement this code properly.
		//! @{
		GDINL BaseString& Append(BaseString const& Other)
		{
			BaseString Copy(Move(*this));
			return (*this = Move(Copy.Concatenate(Other)));
		}
		GDINL BaseString& Append(CharType const* const Other)
		{
			BaseString Copy(Move(*this));
			return (*this = Move(Copy.Concatenate(Other)));
		}
		GDINL BaseString& Append(CharType const Other)
		{
			BaseString Copy(Move(*this));
			return (*this = Move(Copy.Concatenate(Other)));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Prepends specified string to this string.
		//! @param Character Character to Append.
		//! @todo Implement this code properly.
		//! @{
		GDINL BaseString& Prepend(BaseString const& Other)
		{
			BaseString Copy(Other);
			return (*this = Move(Copy.Concatenate(Move(*this))));
		}
		GDINL BaseString& Prepend(CharType const* const Other)
		{
			BaseString Copy(Other);
			return (*this = Move(Copy.Concatenate(Move(*this))));
		}
		GDINL BaseString& Prepend(CharType const Other)
		{
			BaseString Copy(Other);
			return (*this = Move(Copy.Concatenate(Move(*this))));
		}
		//! @}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Substringing.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Extracts a substring from this string from specified index to the specified index.
		//! @param From Index of the first character that would be extracted to the substring.
		//! @param To Index of the last character that would be extracted to the substring.
		//! @returns A part of this string.
		//! @{
		GDINL BaseString Substring(SizeTp const From, SizeTp const To) const
		{
			GD_DEBUG_ASSERT(To >= From, "Invalid substring indices.");
			BaseString Result(To - From);
			CMemory::Memcpy(Result.CStr(), Result.GetAllocatedSize(), &(*this)[From], (To - From) * sizeof(CharType));
			return Result;
		}
		GDINL BaseString& SubstringSelf(SizeTp const From, SizeTp const To)
		{
			return *this = Forward<BaseString&&>(this->Substring(From, To));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Extracts a substring from this string from specified index with specified length.
		//! @param From Index of the first character that would be extracted to the substring.
		//! @param _Length The length of the desired substring.
		//! @returns A part of this string.
		//! @{
		GDINL BaseString SubstringL(SizeTp const From, SizeTp const Length) const
		{
			return this->Substring(From, From + Length);
		}
		GDINL BaseString& SubstringLSelf(SizeTp const From, SizeTp const Length)
		{
			return *this = Forward<BaseString&&>(this->Substring(From, Length));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Extracts a substring from this string from specified index to the end.
		//! @param From Index of the first character that would be extracted to the substring.
		//! @returns A part of this string.
		//! @{
		GDINL BaseString Substring(SizeTp const From) const
		{
			return this->Substring(From, this->GetLength());
		}
		GDINL BaseString& SubstringSelf(SizeTp const From) 
		{
			return *this = Forward<BaseString&&>(this->Substring(From));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Extracts a substring fro this string from start to ChopSelfped end.
		//! @param _Length Amount of bytes to skip from end.
		//! @returns A part of this string.
		//! @{
		GDINL BaseString Remove(SizeTp const From, SizeTp const Length) const
		{
			return this->Substring(0, From).Concatenate(this->Substring(From + Length + 1));
		}
		GDINL BaseString& RemoveFromSelf(SizeTp const From, SizeTp const Length)
		{
			return *this = Forward<BaseString&&>(this->Remove(From, Length));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Extracts a substring fro this string from start to ChopSelfped end.
		//! @param _Length Amount of bytes to skip from end.
		//! @returns A part of this string.
		//! @{
		GDINL BaseString Chop(SizeTp const Length) const
		{
			return this->Substring(0, this->GetLength() - Length);
		}
		GDINL BaseString& ChopSelf(SizeTp const Length)
		{
			return *this = Forward<BaseString&&>(this->Chop(Length));
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
			CharType const* CString = this->CStr();
			CharType const* WildcardPtr = Wildcard;
			CharType const* CharPtr = nullptr;
			CharType const* MatchPointer = nullptr;
			while ((*CString != GD_LITERAL(CharType, '\0')) && (*WildcardPtr != GD_LITERAL(CharType, '*')))
			{
				if ((*WildcardPtr != *CString) && (*WildcardPtr != GD_LITERAL(CharType, '?')))
					return false;
				++WildcardPtr;
				++CString;
			}
			while (*CString != GD_LITERAL(CharType, '\0'))
			{
				if (*WildcardPtr == GD_LITERAL(CharType, '*'))
				{
					if (!*++WildcardPtr)
						return true;
					MatchPointer = WildcardPtr;
					CharPtr = CString + 1;
				}
				else if ((*WildcardPtr == *CString) || (*WildcardPtr == GD_LITERAL(CharType, '?')))
				{
					++WildcardPtr;
					++CString;
				}
				else
				{
					WildcardPtr = MatchPointer;
					CString = CharPtr++;
				}
			}
			while (*WildcardPtr == GD_LITERAL(CharType, '*'))
				++WildcardPtr;
			return (*WildcardPtr == GD_LITERAL(CharType, '\0'));
		}
		GDINL bool MatchesWildcard(BaseString const& Wildcard) const
		{
			return this->MatchesWildcard(Wildcard.CStr());
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Searches for first occurrence of the character in the string.
		//! @param Character Character we are looking for.
		//! @returns Index of the first occurrence of the character in the string or @c SizeTpMax if nothing was found.
		GDINL SizeTp Find(CharType const Character) const
		{
			CharType const* const CString = this->CStr();
			CharType const* const Location = CString::Strchr(CString, Character);
			if (Location != nullptr)
				return static_cast<SizeTp>(Location - CString);
			return SizeTpMax;
		}

		// ------------------------------------------------------------------------------------------
		//! Searches for first occurrence of the C string in the string.
		//! @param OtherCString C string we are looking for.
		//! @returns Index of the first occurrence of the C string in the string or @c SizeTpMax if nothing was found.
		GDINL SizeTp Find(CharType const* const OtherCString) const
		{
			CharType const* const CString = this->CStr();
			CharType const* const Location = CString::Strstr(CString, OtherCString);
			if (Location != nullptr)
				return static_cast<SizeTp>(Location - CString);
			return SizeTpMax;
		}

		// ------------------------------------------------------------------------------------------
		//!	Searches for last occurrence of the character in the string.
		//! @param Character Character we are looking for.
		//! @returns Index of the last occurrence of the character in the string or @c SizeTpMax if nothing was found.
		GDINL SizeTp ReverseFind(CharType const Character) const
		{
			CharType const* const CString = this->CStr();
			CharType const* const Location = CString::Strrchr(CString, Character);
			if (Location != nullptr)
				return static_cast<SizeTp>(Location - CString);
			return SizeTpMax;
		}

		// ------------------------------------------------------------------------------------------
		//!	Searches for last occurrence of the C string in the string.
		//! @param OtherCString C string we are looking for.
		//! @returns Index of the last occurrence of the C string in the string or @c SizeTpMax if nothing was found.
		GDINL SizeTp ReverseFind(CharType const* const OtherCString) const
		{
			CharType const* const CString = this->CStr();
			CharType const* const Location = CString::Strrstr(CString, OtherCString);
			if (Location != nullptr)
				return static_cast<SizeTp>(Location - CString);
			return SizeTpMax;
		}

		// ------------------------------------------------------------------------------------------
		//! Returns true if this string starts with specified expression.
		//! @param OtherCString C string we are testing against.
		//! @return True if this string starts with specified expression.
		//! @{
		GDINL bool StartsWith(CharType const* const OtherCString) const
		{
			//! @todo Implement this code properly.
			return this->Find(OtherCString) == 0;
		}
		GDINL bool StartsWith(CharType const Character) const
		{
			return *this->CStr() == Character;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns true if this string ends with specified expression.
		//! @param OtherCString C string we are testing against.
		//! @return True if this string ends with specified expression.
		//! @{
		GDINL bool EndsWith(CharType const* const OtherCString) const
		{
			//! @todo Implement this code properly.
			return this->Find(OtherCString) == (this->_Length - CString::Strlen(OtherCString));
		}
		GDINL bool EndsWith(CharType const Character) const
		{
			return *(this->_PtrEnd() - 1) == Character;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns integer representation of this string.
		//! @param Succeeded Would be written true if this is an integer.
		//! @param Notation Notation in which value is represented.
		//! @returns Integer representation of this string.
		GDINL Int64 ToInt64(bool* const Succeeded = nullptr, SizeTp const Notation = 0) const
		{
			CharType const* EndPtr = nullptr;
			Int64 const Value = CString::Strtoi64(this->CStr(), &EndPtr, Notation);
			if (Succeeded != nullptr)
				*Succeeded = EndPtr == this->_PtrEnd();
			return Value;
		}

		// ------------------------------------------------------------------------------------------
		//! Returns lowercased version of this string.
		//! @returns Lowercased version of this string.
		GDINL BaseString ToUpper() const
		{
			BaseString Result(*this);
			for (auto& Character : Result)
				Character = static_cast<CharType>(CChar::ToUpper(Character));
			return Result;
		}

		// ------------------------------------------------------------------------------------------
		//! Returns uppercased version of this string.
		//! @returns Uppercased version of this string.
		GDINL BaseString ToLower() const
		{
			BaseString Result(*this);
			for (auto& Character : Result)
				Character = static_cast<CharType>(CChar::ToLower(Character));
			return Result;
		}

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Formatting.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Formats a specified string using C-like formatting.
		//! @param Format Initial format of the string.
		//! @param List Varying arguments list for formatting.
		GDINL static BaseString FormatVa(CharType const* const Format, va_list List)
		{
#if 0
			CharType Buffer1024[1024];
			CMemory::Memset(&Buffer1024[0], 0, sizeof(Buffer1024));
			int const Result1204 = CString::Vsnprintf(&Buffer1024[0], GD_ARRAY_LENGTH(Buffer1024), Format, List);
			if ((Result1204 > 0) && (Result1204 < GD_ARRAY_LENGTH(Buffer1024)))
			{
				return BaseString(&Buffer1024[0]);
			}
#endif	// if 0
			CharType Buffer2048[2048];
			CMemory::Memset(&Buffer2048[0], 0, sizeof(Buffer2048));
			int const Result2048 = CString::Vsnprintf(&Buffer2048[0], GD_ARRAY_LENGTH(Buffer2048), Format, List);
			if ((Result2048 > 0) && (Result2048 < GD_ARRAY_LENGTH(Buffer2048)))
				return BaseString(&Buffer2048[0]);
			GD_DEBUG_ASSERT_FALSE("String formatting failed due internal buffers error.");
		}

		// ------------------------------------------------------------------------------------------
		//! Formats a specified string using C-like formatting.
		//! @param Format Initial format of the string.
		GDINL static BaseString Format(CharType const* const Format, ...)
		{
			va_list List; 
			va_start(List, Format);
			return BaseString::FormatVa(Format, List);
		}

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Overloaded operators.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Assigns string with copy of other string.
		//! @param OtherVector String would be assigned.
		//! @returns this.
		GDINL BaseString& operator= (BaseString const& Other)
		{
			this->~BaseString();
			this->_Length = Other._Length;
			SizeTp const DataSizeInBytes = (this->_Length + 1) * sizeof(CharType);
			if (this->_Length >= BaseString::_MaxStackLength)
			{
				this->_HeapMemory = reinterpret_cast<CharType*>(GD_MALLOC(DataSizeInBytes));
				CMemory::Memcpy(this->_HeapMemory, DataSizeInBytes, Other.CStr(), DataSizeInBytes);
			}
			else
			{
				CMemory::Memcpy(&this->_StackMemory[0], GD_ARRAY_LENGTH(this->_StackMemory), Other.CStr(), DataSizeInBytes);
			}
			return *this;
		}

		// ------------------------------------------------------------------------------------------
		//! Moves other string here.
		//! @param Other String would be moved into current object.
		//! @returns this.
		GDINL BaseString& operator= (BaseString&& Other)
		{
			this->~BaseString();
			this->_Length = Other._Length;
			Other._Length = 0;
			if (this->_Length >= BaseString::_MaxStackLength)
			{
				this->_HeapMemory = Other._HeapMemory;
				Other._HeapMemory = nullptr;
			}
			else
			{
				SizeTp const DataSizeInBytes = (this->_Length + 1) * sizeof(CharType);
				CMemory::Memcpy(this->_StackMemory, GD_ARRAY_LENGTH(this->_StackMemory), &Other._StackMemory[0], DataSizeInBytes);
			}
			return *this;
		}

		// ------------------------------------------------------------------------------------------
		//! Returns reference on the character at specified index.
		//! @param Index Index of the desired character in vector.
		//! @returns Reference on some character in the container.
		//! @{
		GDINL CharType& operator[] (SizeTp const Index) { return const_cast<Char&>(const_cast<BaseString const&>(*this)[Index]); }
		GDINL CharType const& operator[] (SizeTp const Index) const
		{
			GD_DEBUG_ASSERT(Index <= this->GetLength(), "Invalid String subindex.");
			return *(this->CStr() + Index);
		}
		//! @}
	};	// class BaseString

	// ------------------------------------------------------------------------------------------
	//! Swaps two strings.
	//! @param First The first strings.
	//! @param Second The second strings.
	template<typename SomeCharType>
	GDINL void Swap(BaseString<SomeCharType>& First, BaseString<SomeCharType>& Second)
	{
		if (&First != &Second)
		{
			Swap(First._Length, Second._Length);
			Swap(First._StackMemory, Second._StackMemory); // Swapping _StackMemory because it supposed to be greater than _HeapMemory pointer.
		}
	}

	// ------------------------------------------------------------------------------------------
	//! Checks whether two strings are same.
	//! @param LHS First string to compare.
	//! @param RHS Second string to compare.
	//! @returns True if both strings have same length and elements.
	//! @{
	template<typename CharType>
	GDINL static bool operator== (BaseString<CharType> const& LHS, BaseString<CharType> const& RHS)
	{
		if (LHS.GetLength() == RHS.GetLength())
			return (CString::Strncmp(LHS.CStr(), RHS.CStr(), LHS.GetLength()) == 0);
		return false;
	}
	template<typename CharType>
	GDINL static bool operator== (BaseString<CharType> const& LHS, CharType const* const RHS)
	{
		return (CString::Strncmp(LHS.CStr(), RHS, LHS.GetLength()) == 0);
	}
	template<typename CharType>
	GDINL static bool operator== (CharType const* const LHS, BaseString<CharType> const& RHS)
	{
		return (CString::Strncmp(LHS, RHS.CStr(), RHS.GetLength()) == 0);
	}
	//! @}

	// ------------------------------------------------------------------------------------------
	//! Checks if this string is different to specified one.
	//! @param RHS RHS string to compare to.
	//! @returns True if both string have different length or characters set.
	//! @{
	template<typename CharType>
	GDINL static bool operator!= (BaseString<CharType> const& LHS, BaseString<CharType> const & RHS)
	{
		return !(LHS == RHS);
	}
	template<typename CharType>
	GDINL static bool operator!= (BaseString<CharType> const& LHS, CharType const* const RHS)
	{
		return !(LHS == RHS);
	}
	//! @}

	// ------------------------------------------------------------------------------------------
	//! Checks if string contains single character that is same two specified one.
	//! @param LHS String to compare.
	//! @param Character Character to compare to.
	//! @returns True if this string contains single character that is same two specified one, false otherwise.
	template<typename CharType>
	GDINL static bool operator== (BaseString<CharType> const& LHS, CharType const Character)
	{
		if (LHS.GetLength() == 1)
			return (LHS->CStr() == Character);
		return false;
	}

	// ------------------------------------------------------------------------------------------
	//! Checks if string is not equal to single character.
	//! @param LHS String to compare.
	//! @param Character Character to compare to.
	//! @returns False if this string contains single character that is same two specified one, true otherwise.
	template<typename CharType>
	GDINL static bool operator!= (BaseString<CharType> const& LHS, CharType const Character)
	{
		return !(LHS == Character);
	}

	// ------------------------------------------------------------------------------------------
	//! Checks if first string is greater to second one.
	//! @param LHS First string to compare.
	//! @param RHS Second string to compare.
	//! @returns True if first string is greater than second one.
	template<typename CharType>
	GDINL static bool operator> (BaseString<CharType> const& LHS, BaseString<CharType> const& RHS)
	{
		return (CString::Strncmp(LHS.CStr(), RHS.CStr(), LHS.GetLength()) > 0);
	}

	// ------------------------------------------------------------------------------------------
	//! Checks if first string is less to second one.
	//! @param LHS First string to compare.
	//! @param RHS Second string to compare.
	//! @returns True if first string is less than second one.
	template<typename CharType>
	GDINL static bool operator< (BaseString<CharType> const& LHS, BaseString<CharType> const& RHS)
	{
		return (CString::Strncmp(LHS.CStr(), RHS.CStr(), LHS.GetLength()) < 0);
	}

	// ------------------------------------------------------------------------------------------
	//! Concatenates two strings.
	//! @param LHS First string to concatenate to.
	//! @param RHS Second string to concatenate with.
	//! @returns Result of the string concatenation.
	//! @{
	template<typename CharType>
	GDINL static BaseString<CharType> operator+ (BaseString<CharType> const& LHS, BaseString<CharType> const& RHS)
	{
		return LHS.Concatenate(RHS);
	}
	template<typename CharType>
	GDINL static BaseString<CharType> operator+ (BaseString<CharType> const& LHS, CharType const* const RHS)
	{
		return LHS.Concatenate(RHS);
	}
	template<typename CharType>
	GDINL static BaseString<CharType> operator+ (CharType const* const LHS, BaseString<CharType> const& RHS)
	{
		return BaseString<CharType>(LHS).Concatenate(RHS);
	}
	template<typename CharType>
	GDINL static BaseString<CharType> operator+ (BaseString<CharType> const& LHS, CharType const RHS)
	{
		return LHS.Concatenate(RHS);
	}
	template<typename CharType>
	GDINL static BaseString<CharType> operator+ (CharType const LHS, BaseString<CharType> const& RHS)
	{
		return BaseString<CharType>(LHS).Concatenate(RHS);
	}
	template<typename CharType>
	GDINL static BaseString<CharType>& operator+= (BaseString<CharType>& LHS, BaseString<CharType> const& RHS)
	{
		return LHS.Append(RHS);
	}
	template<typename CharType>
	GDINL static BaseString<CharType>& operator+= (BaseString<CharType>& LHS, CharType const* const RHS)
	{
		return LHS.Append(RHS);
	}
	template<typename CharType>
	GDINL static BaseString<CharType>& operator+= (BaseString<CharType>& LHS, CharType const RHS)
	{
		return LHS.Append(RHS);
	}
	//! @}

	typedef BaseString<Char> String;
	typedef BaseString<WideChar> WideString;

GD_NAMESPACE_END
