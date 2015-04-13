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
#include <GoddamnEngine/Core/Misc/Hash.h>
#include <GoddamnEngine/Core/Misc/CString.h>
#include <GoddamnEngine/Core/Misc/CMemory.h>
#include <GoddamnEngine/Core/Containers/Vector.h>

GD_NAMESPACE_BEGIN

	//! @brief A base string class used by the engine.
	//! @tparam CharType Character type of this string.
	template<typename CharType>
	class BaseString final : public IContainer
		, public ContainerIteratableTag
		, public ContainerReverseIteratableTag
		, public ContainerPtrIteratableTag
		, public ContainerPtrReverseIteratableTag
	{
	private:
		typedef CCharTraits<CharType> CChar;
		typedef CStringTraits<CharType> CString;

	public:
		typedef IndexedContainerIterator<BaseString, CharType> Iterator;
		typedef IndexedContainerIterator<BaseString const, CharType const> ConstIterator;
		GD_CONTAINER_CHECK_ITERATORS(BaseString);

		typedef ReverseContainerIterator<Iterator> ReverseIterator;
		typedef ReverseContainerIterator<ConstIterator> ReverseConstIterator;
		GD_CONTAINER_CHECK_REVERSE_ITERATORS(BaseString);

		typedef CharType* PtrIterator;
		typedef CharType const* PtrConstIterator;
		GD_CONTAINER_CHECK_PTR_ITERATORS(BaseString);

		typedef ReverseContainerIterator<PtrIterator> ReversePtrIterator;
		typedef ReverseContainerIterator<PtrConstIterator> ReversePtrConstIterator;
		GD_CONTAINER_CHECK_REVERSE_PTR_ITERATORS(BaseString);

		GD_CONTAINER_DEFINE_PTR_ITERATION_SUPPORT(BaseString);

	private:
		SizeTp Length = 0;
		SizeTp static const MaxStackLength = 2 * (sizeof(CharType*) / sizeof(CharType));
		static_assert((MaxStackLength >= 2) && (MaxStackLength >= sizeof(CharType*)), "'MaxStackLength' has invalid size");
		union 
		{
			CharType StackMemory[MaxStackLength];
			CharType* HeapMemory;
		};	// anonymous union

	public:

		//! @brief Initializes an empty string.
		GDINL BaseString()
		{
			this->Length = 0;
			this->StackMemory[0] = GD_LITERAL(CharType, '\0');
		}

		//! @brief Initializes a string with a single character.
		//! @param Character Initial String character.
		GDINL BaseString(Char const Character)
		{
			this->Length = 1;
			this->StackMemory[0] = Character;
			this->StackMemory[1] = GD_LITERAL(CharType, '\0');
		}

		//! @brief Fills a String with specified number of characters.
		//! @param Size A length of the string.
		//! @param FillWith A character that String would be filled with.
		GDINL BaseString(SizeTp const Length, CharType const FillWith = GD_LITERAL(CharType, '\0'))
		{
			this->Length = Length;
			if (this->Length >= BaseString::MaxStackLength)
			{
				SizeTp const DataSizeInBytes = (Length + 1) * sizeof(CharType);
				this->HeapMemory = reinterpret_cast<CharType*>(GD_MALLOC(DataSizeInBytes));
			}

			if (sizeof(CharType) != 1)
			{
				for (auto& Character : (*this))
				{
					Character = FillWith;
				}
			}
			else
			{
				CMemory::Memset(this->CStr(), FillWith, Length);
			}

			*this->PtrEnd() = GD_LITERAL(CharType, '\0');
		}

		//! @brief Initializes a String with some C String with known length.
		//! @param Chars String's initial data.
		//! @param Size Size of String initial data.
		GDINL BaseString(CharType const* const Chars, SizeTp const Length)
		{
			GD_DEBUG_ASSERT(Chars != nullptr, "Null pointer data specified");
			this->Length = Length;
			SizeTp const DataSizeInBytes = (Length + 1) * sizeof(CharType);
			if (this->Length >= BaseString::MaxStackLength)
			{
				this->HeapMemory = reinterpret_cast<CharType*>(GD_MALLOC(DataSizeInBytes));
				CMemory::Memcpy(this->HeapMemory, DataSizeInBytes, Chars, DataSizeInBytes);
			}
			else
			{
				CMemory::Memcpy(&this->StackMemory[0], GD_ARRAY_SIZE(this->StackMemory), Chars, DataSizeInBytes);
			}
		}

		//! @brief Initializes a String with some C String with unknown length.
		//! @param Chars String initial data.
		//! @{   
		template <SizeTp const Length>	// Length of string constant is determined at compile time..
		GDINL BaseString(CharType const(&Chars)[Length])
			: BaseString(&Chars[0], Length - 1)
		{
		}
		GDINL BaseString(CharType const* const Chars)
			: BaseString(Chars, CString::Strlen(Chars))
		{
		}
		//! @}

		//! @brief Initializes this string with copy of other String.
		//! @param Other Other String to copy.
		GDINL BaseString(BaseString const& Other)
			: BaseString(Other.CStr(), Other.GetLength())
		{
		}

		//! @brief Moves other string into this String.
		//! @param Other Other String to move into this.
		GDINL BaseString(BaseString&& Other)
		{
			*this = Forward<BaseString>(Other);
		}

		//! @brief Deinitializes string.
		GDINL ~BaseString()
		{
			if (this->Length >= BaseString::MaxStackLength)
			{
				GD_DEALLOC(this->HeapMemory);
				this->HeapMemory = nullptr;
			}
			else
			{
				this->StackMemory[0] = GD_LITERAL(CharType, '\0');
			}

			this->Length = 0;
		}

	private:

		//! @brief Returns pointer iterator that points to first container element.
		//! @returns Simple pointer iterator that points to first container element.
		//! @{
		GDINL PtrIterator PtrBegin()
		{
			return reinterpret_cast<PtrIterator>(this->CStr());
		}
		GDINL PtrConstIterator PtrBegin() const
		{
			return reinterpret_cast<PtrConstIterator>(this->CStr());
		}
		//! @}

		//! @brief Returns pointer iterator that points to past the end container element.
		//! @returns Simple pointer iterator that points to past the end container element.
		//! @{
		GDINL PtrIterator PtrEnd()
		{
			return this->PtrBegin() + this->Length;
		}
		GDINL PtrConstIterator PtrEnd() const
		{
			return this->PtrBegin() + this->Length;
		}
		//! @}

		//! @brief Returns pointer iterator that points to last container element.
		//! @returns Simple pointer iterator that points to last container element.
		//! @{
		GDINL ReversePtrIterator ReversePtrBegin()
		{
			return ReversePtrIterator(this->PtrEnd() - 1);
		}
		GDINL ReversePtrConstIterator ReversePtrBegin() const
		{
			return ReversePtrConstIterator(this->PtrEnd() - 1);
		}
		//! @}

		//! @brief Returns pointer iterator that points to preceding the first container element.
		//! @returns Simple pointer iterator that points to preceding the first container element.
		//! @{
		GDINL ReversePtrIterator ReversePtrEnd()
		{
			return ReversePtrIterator(this->PtrBegin() - 1);
		}
		GDINL ReversePtrConstIterator ReversePtrEnd() const
		{
			return ReversePtrConstIterator(this->PtrBegin() - 1);
		}
		//! @}

	public:

		//! @brief Returns iterator that points to first container element.
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

		//! @brief Returns iterator that points to past the end container element.
		//! @returns Iterator that points to past the end container element.
		//! @{
		GDINL Iterator End()
		{
			return this->Begin() + this->Length;
		}
		GDINL ConstIterator End() const
		{
			return this->Begin() + this->Length;
		}
		//! @}

		//! @brief Returns iterator that points to last container element.
		//! @returns Iterator that points to last container element.
		//! @{
		GDINL ReverseIterator ReverseBegin()
		{
			return ReverseIterator(this->End() - 1);
		}
		GDINL ReverseConstIterator ReverseBegin() const
		{
			return ReverseConstIterator(this->End() - 1);
		}
		//! @}

		//! @brief Returns iterator that points to preceding the first container element.
		//! @returns Iterator that points to preceding the first container element.
		//! @{
		GDINL ReverseIterator ReverseEnd()
		{
			return ReverseIterator(this->Begin() - 1);
		}
		GDINL ReverseConstIterator ReverseEnd() const
		{
			return ReverseConstIterator(this->Begin() - 1);
		}
		//! @}

		//! @brief Returns length of this string.
		//! @returns Size of this String.
		GDINL SizeTp GetLength() const
		{
			return this->Length;
		}

		//! @brief Returns available size of the container.
		//! @returns Available size of the container.
		GDINL SizeTp GetAllocatedSize() const
		{
			if (this->GetLength() >= BaseString::MaxStackLength)
			{
				return (this->Length + 1) * sizeof(CharType);
			}
			else
			{
				return GD_ARRAY_SIZE(this->StackMemory);
			}
		}

		//! @brief Returns true if this string is empty.
		//! @returns True if this String is empty, false otherwise.
		GDINL bool IsEmpty() const
		{
			return this->Length == 0;
		}

		//! @brief Returns pointer on this string.
		//! @returns C-String version of this object.
		//! @{
		GDINL CharType const* CStr() const
		{
			if (this->GetLength() >= BaseString::MaxStackLength)
			{
				return this->HeapMemory;
			}
			else
			{
				return &this->StackMemory[0];
			}
		}
		GDINL CharType* CStr()
		{
			return const_cast<CharType*>(const_cast<BaseString const*>(this)->CStr());
		}
		//! @}

		//! @brief Returns last character of the string.
		//! @returns Last character of the string.
		GDINL CharType GetLastElement() const
		{
			return *(this->PtrEnd() - 1);
		}

		//! @brief Returns unique signature of the string.
		//! @returns Constant-time comparable unique signature of the string. 
		GDINL HashCode GetHashCode() const
		{
			HashValueType Computed = 0;
			for (auto const MyCharacter : (*this))
			{
				Computed = 65599 * Computed + MyCharacter;
			}

			return HashCode((Computed >> 16) ^ Computed);
		}

		//! @brief Extracts a substring fro this string from specified index to the specified index.
		//! @param From Index of the first character that would be extracted to the substring.
		//! @param To Index of the last character that would be extracted to the substring.
		//! @returns A part of this string.
		GDINL BaseString Substring(SizeTp const From, SizeTp const To) const
		{
			GD_DEBUG_ASSERT(To >= From, "Invalid subString indices.");
			BaseString Result(To - From);
			CMemory::Memcpy(Result.CStr(), Result.GetAllocatedSize(), &(*this)[From], (To - From) * sizeof(CharType));
			return Result;
		}

		//! @brief Extracts a substring fro this string from specified index to the end.
		//! @param From Index of the first character that would be extracted to the substring.
		//! @returns A part of this string.
		GDINL BaseString Substring(SizeTp const From) const
		{
			return this->Substring(From, this->GetLength());
		}

		//! @brief Searches for first occurrence of the character in the string.
		//! @param Character Character we are looking for.
		//! @returns Index of the first occurrence of the character in the string or @c SizeTpMax if nothing was found.
		GDINL SizeTp Find(CharType const Character) const
		{
			CharType const* const CString = this->CStr();
			CharType const* const Location = CString::Strchr(CString, Character);
			if (Location != nullptr)
			{
				return static_cast<SizeTp>(Location - CString);
			}
			else
			{
				return SizeTpMax;
			}
		}

		//! @brief Searches for first occurrence of the C string in the string.
		//! @param OtherCString C string we are looking for.
		//! @returns Index of the first occurrence of the C string in the string or @c SizeTpMax if nothing was found.
		GDINL SizeTp Find(CharType const* const OtherCString) const
		{
			CharType const* const CString = this->CStr();
			CharType const* const Location = CString::Strstr(CString, OtherCString);
			if (Location != nullptr)
			{
				return static_cast<SizeTp>(Location - CString);
			}
			else
			{
				return SizeTpMax;
			}
		}

		///	@brief Searches for last occurrence of the character in the string.
		//! @param Character Character we are looking for.
		//! @returns Index of the last occurrence of the character in the string or @c SizeTpMax if nothing was found.
		GDINL SizeTp ReverseFind(CharType const Character) const
		{
			CharType const* const CString = this->CStr();
			CharType const* const Location = CString::Strrchr(CString, Character);
			if (Location != nullptr)
			{
				return static_cast<SizeTp>(Location - CString);
			}
			else
			{
				return SizeTpMax;
			}
		}

		///	@brief Searches for last occurrence of the C string in the string.
		//! @param OtherCString C string we are looking for.
		//! @returns Index of the last occurrence of the C string in the string or @c SizeTpMax if nothing was found.
		GDINL SizeTp ReverseFind(CharType const* const OtherCString) const
		{
			CharType const* const CString = this->CStr();
			CharType const* const Location = CString::Strrstr(CString, OtherCString);
			if (Location != nullptr)
			{
				return static_cast<SizeTp>(Location - CString);
			}
			else
			{
				return SizeTpMax;
			}
		}

		//! @brief Returns lowercased version of this string.
		//! @returns Lowercased version of this string.
		GDINL BaseString ToUpper() const
		{
			BaseString Result(*this);
			for (auto& Character : Result)
			{
				Character = static_cast<CharType>(CChar::ToUpper(Character));
			}

			return Result;
		}

		//! @brief Returns uppercased version of this string.
		//! @returns Uppercased version of this string.
		GDINL BaseString ToLower() const
		{
			BaseString Result(*this);
			for (auto& Character : Result)
			{
				Character = static_cast<CharType>(CChar::ToLower(Character));
			}

			return Result;
		}

		//! @brief Formats a specified string using C-like formatting.
		//! @param Format Initial format of the string.
		//! @param List Varying arguments list for formatting.
		GDINL static BaseString FormatVa(CharType const* const Format, va_list List)
		{
			CharType Buffer1024[1024];
			CMemory::Memset(&Buffer1024[0], 0, sizeof(Buffer1024));
			int const Result1204 = CString::Vsnprintf(&Buffer1024[0], GD_ARRAY_SIZE(Buffer1024), Format, List);
			if ((Result1204 > 0) && (Result1204 < GD_ARRAY_SIZE(Buffer1024)))
			{
				return BaseString(&Buffer1024[0]);
			}
			else
			{
				CharType Buffer2048[2048];
				CMemory::Memset(&Buffer2048[0], 0, sizeof(Buffer2048));
				int const Result2048 = CString::Vsnprintf(&Buffer2048[0], GD_ARRAY_SIZE(Buffer2048), Format, List);
				if ((Result2048 > 0) && (Result2048 < GD_ARRAY_SIZE(Buffer2048)))
				{
					return BaseString(&Buffer2048[0]);
				}
				else
				{
					GD_DEBUG_ASSERT_FALSE("String formatting failed due buffers error.");
				}
			}
		}

		//! @brief Formats a specified string using C-like formatting.
		//! @param Format Initial format of the string.
		GDINL static BaseString Format(CharType const* const Format, ...)
		{
			va_list List; va_start(List, Format);
			return BaseString::FormatVa(Format, List);
		}

		//! @brief Assigns string with copy of other string.
		//! @param OtherVector String would be assigned.
		//! @returns this.
		GDINL BaseString& operator= (BaseString const& Other)
		{
			this->~BaseString();
			this->Length = Other.Length;
			SizeTp const DataSizeInBytes = (this->Length + 1) * sizeof(CharType);
			if (this->Length >= BaseString::MaxStackLength)
			{
				this->HeapMemory = reinterpret_cast<CharType*>(GD_MALLOC(DataSizeInBytes));
				CMemory::Memcpy(this->HeapMemory, DataSizeInBytes, Other.CStr(), DataSizeInBytes);
			}
			else
			{
				CMemory::Memcpy(&this->StackMemory[0], GD_ARRAY_SIZE(this->StackMemory), Other.CStr(), DataSizeInBytes);
			}

			return *this;
		}

		//! @brief Moves other string here.
		//! @param Other String would be moved into current object.
		//! @returns this.
		GDINL BaseString& operator= (BaseString&& Other)
		{
			this->~BaseString();
			this->Length = Other.Length;
			Other.Length = 0;
			if (this->Length >= BaseString::MaxStackLength)
			{
				this->HeapMemory = Other.HeapMemory;
				Other.HeapMemory = nullptr;
			}
			else
			{
				SizeTp const DataSizeInBytes = (this->Length + 1) * sizeof(CharType);
				CMemory::Memcpy(this->StackMemory, GD_ARRAY_SIZE(this->StackMemory), &Other.StackMemory[0], DataSizeInBytes);
			}

			return *this;
		}

		//! @brief Returns reference on the character at specified index.
		//! @param Index Index of the desired character in vector.
		//! @returns Reference on some character in the container.
		//! @{
		GDINL CharType const& operator[] (SizeTp const Index) const
		{
			GD_DEBUG_ASSERT(Index <= this->GetLength(), "Invalid String subindex.");
			return *(this->CStr() + Index);
		}
		GDINL CharType& operator[] (SizeTp const Index)
		{
			return const_cast<Char&>(const_cast<BaseString const&>(*this)[Index]);
		}
		//! @}

		//! @brief Checks if this string is same to specified one.
		//! @param Other Other string to compare to.
		//! @returns True if both string have same length and characters set.
		GDINL bool operator== (BaseString const& Other) const
		{
			if (this->Length == Other.Length)
			{
				return (CString::Strncmp(this->CStr(), Other.CStr(), this->Length) == 0);
			}
			else
			{
				return false;
			}
		}

		//! @brief Checks if this string is different to specified one.
		//! @param Other Other string to compare to.
		//! @returns True if both string have different length or characters set.
		GDINL bool operator!= (BaseString const & Other) const
		{
			return !(*this == Other);
		}

		//! @brief Checks if this string contains single character that is same two specified one.
		//! @param Character Character to compare to.
		//! @returns True if this string contains single character that is same two specified one, false otherwise.
		GDINL bool operator== (CharType const Character) const
		{
			if (this->Length == 1)
			{
				return (*this->CStr() == Character);
			}
			else
			{
				return false;
			}
		}

		//! @brief Checks if this string contains single character that is same two specified one.
		//! @param Character Character to compare to.
		//! @returns False if this string contains single character that is same two specified one, true otherwise.
		GDINL bool operator!= (CharType const Character) const
		{
			return !(*this == Character);
		}

		//! @brief Checks if this string is greater to specified one.
		//! @param Other Other string to compare to.
		//! @returns True if this string is greater to specified one, false otherwise.
		GDINL bool operator> (BaseString const& Other) const
		{
			return (CString::Strcmp(this->CStr(), Other.CStr(), this->Length) > 0);
		}

		//! @brief Checks if this string is less to specified one.
		//! @param Other Other string to compare to.
		//! @returns True if this string is less to specified one, false otherwise.
		GDINL bool operator< (BaseString const& Other) const
		{
			return (CString::Strcmp(this->CStr(), Other.CStr(), this->Length) < 0);
		}

		//! @brief Concatenates two strings.
		//! @param Other Other string to concatenate with.
		//! @returns Result of the string concatenation.
		//! @{
		GDINL BaseString operator+ (BaseString const& Other) const
		{
			BaseString Result(this->Length + Other.GetLength());
			CMemory::Memcpy(Result.CStr(), Result.GetAllocatedSize(), this->CStr(), this->Length * sizeof(CharType));
			CMemory::Memcpy(Result.CStr() + this->Length, Result.GetAllocatedSize(), Other.CStr(), Other.Length * sizeof(CharType));
			return Result;
		}
		GDINL BaseString& operator+= (BaseString const& Other)
		{
			BaseString Copy(Move(*this));
			return (*this = Move(Copy + Other));
		}
		//! @}

	private:
		template<typename SomeCharType>
		GDINL friend void Swap(BaseString<SomeCharType>& First, BaseString<SomeCharType>& Second);
	};	// class BaseString

	//! @brief Swaps two strings.
	//! @param First The first strings.
	//! @param Second The second strings.
	template<typename SomeCharType>
	GDINL void Swap(BaseString<SomeCharType>& First, BaseString<SomeCharType>& Second)
	{
		if (&First != &Second)
		{
			Swap(First.Length, Second.Length);
			Swap(First.StackMemory, Second.StackMemory); // Swapping StackMemory because it supposed to be greater than HeapMemory pointer.
		}
	}

	typedef BaseString<Char> String;
	typedef BaseString<WideChar> WideString;

GD_NAMESPACE_END
