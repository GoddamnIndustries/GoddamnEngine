/// ==========================================================================================
/// String.h - Dynamically sized string interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.06.2013 - Created by James Jhuighuy.
///		* 13.05.2014 - Rewritten from scratch by James Jhuighuy.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_TEXT_STRING
#define GD_CORE_TEXT_STRING

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Text/Hash/Hash.h>
#include <GoddamnEngine/Core/Text/CharTraits.h>
#include <GoddamnEngine/Core/Containers/Containers.h>
#include <GoddamnEngine/Core/Containers/Vector/Vector.h>
#include <GoddamnEngine/Core/Diagnostics/Assertion/Assertion.h>

GD_NAMESPACE_BEGIN

	template<typename CharType>
	class BaseString;

	template<typename CharType>
	void Swap(BaseString<CharType>& First, BaseString<CharType>& Second);

	template<typename CharType> 
	class BaseString final
	{
	public:
		typedef CharType ThisCharType;
		typedef BaseString<CharType> ThisBaseString;

		typedef ContainerDetails::IndexedContainerIterator<ThisBaseString const, CharType const> ConstIterator;
		typedef ContainerDetails::IndexedContainerIterator<ThisBaseString,       CharType      > MutableIterator;
		typedef ContainerDetails::ReverseIterator<ConstIterator  > ReverseConstIterator;
		typedef ContainerDetails::ReverseIterator<MutableIterator> ReverseMutableIterator;

		typedef ThisCharType const* PtrConstIterator;
		typedef ThisCharType      * PtrMutableIterator;
		typedef ContainerDetails::ReverseIterator<PtrConstIterator  > ReversePtrConstIterator;
		typedef ContainerDetails::ReverseIterator<PtrMutableIterator> ReversePtrMutableIterator;

	private /*Class members & API*/:
		size_t static const StringMaxHeapSize = 2 * (sizeof(handle) / sizeof(CharType));
		size_t StringSize = 0;
		union {
			CharType  StringStackArray[StringMaxHeapSize];
			CharType* StringHeapPointer;
		};	// anonymous union

		/// ------------------------------------------------------------------------------------------
		/// *** ConstructWith *** 
		/// ------------------------------------------------------------------------------------------

		/// Initializes a string with a array of characters and length.
		/// @param Chars  String initial data.
		/// @param Size Size of string initial data.
		inline void ConstructWithDataPointerAndSize(CharType const* const Chars, size_t const Size)
		{
			GD_DEBUG_ASSERT(Chars != nullptr, "Nullptr data specified");
			this->StringSize = Size;
			size_t const DataSizeInBytes = (Size + 1) * sizeof(CharType);
			if (this->StringSize >= BaseString::StringMaxHeapSize) {
				this->StringHeapPointer = reinterpret_cast<CharType*>(Allocator::AllocateMemory(DataSizeInBytes));
				::memcpy(this->StringHeapPointer, Chars, DataSizeInBytes);
			} else {
				::memcpy(&this->StringStackArray[0], Chars, DataSizeInBytes);
			}
		}

		/// Initializes a string with some C string with unknown length.
		/// @param Chars  String initial data.
		GDINL void ConstructWithDataPointer(CharType const* const Chars)
		{
			this->ConstructWithDataPointerAndSize(Chars, CharTraits<CharType>::StrLen(Chars));
		}

	public /*Constructors/Destructor*/:
		/// ------------------------------------------------------------------------------------------
		/// *** Standart constructors. ***
		/// ------------------------------------------------------------------------------------------

		/// Initializes an empty string.
		GDINL BaseString()
		{
			this->StringSize = 0;
			this->StringStackArray[0] = CharType('\0');
		}

		/// Initializes this string with copy of other string.
		/// @param OtherBaseString Other string to copy.
		GDINL BaseString(BaseString const& OtherBaseString)
		{
			this->ConstructWithDataPointerAndSize(OtherBaseString.CStr(), OtherBaseString.GetSize());
		}

		/// Moves other string into this string.
		/// @param OtherBaseString Other string to move into this.
		GDINL BaseString(BaseString&& OtherBaseString)
		{
			(*this) = Forward<BaseString>(OtherBaseString);
		}

		/// ------------------------------------------------------------------------------------------
		/// *** Custom constructors. ***
		/// ------------------------------------------------------------------------------------------

		/// Initializes a string with a single character.
		/// @param Character Initial string character.
		GDINL BaseString(Char const Character)
		{
			this->StringSize = 1;
			this->StringStackArray[0] = Character;
			this->StringStackArray[1] = CharType('\0');
		}

		/// Initializes a string with some C string with known length.
		/// @param Chars  String initial data.
		/// @param Size Size of string initial data.
		GDINL BaseString(CharType const* const Chars, size_t const Size)
		{
			this->ConstructWithDataPointerAndSize(Chars, Size);
		}

		/// Initializes a string with some C string with unknown length.
		/// @param Chars  String initial data.
		GDINL BaseString(CharType const* const Chars)
		{
			this->ConstructWithDataPointer(Chars);
		}

		/// Fills a string with specified number of characters.
		/// @param Size   A length of string.
		/// @param FillWith A character that string would be filled with.
		inline BaseString(size_t const Size, CharType const FillWith = CharType('\0'))
		{
			if ((this->StringSize = Size) >= BaseString::StringMaxHeapSize) {
				size_t const DataSizeInBytes = (Size + 1) * sizeof(CharType);
				this->StringHeapPointer = reinterpret_cast<CharType*>(Allocator::AllocateMemory(DataSizeInBytes));
			}

			if (sizeof(CharType) != 1) {
				for (auto& Character : (*this)) {
					Character = FillWith;
				}
			} else {
				memset(this->CStr(), FillWith, Size);
			}

			(*(this->CStr() + this->GetSize())) = CharType('\0');
		}

		/// ------------------------------------------------------------------------------------------
		/// *** Destructor. ***
		/// ------------------------------------------------------------------------------------------

		/// Deinitialzes string.
		GDINL ~BaseString()
		{
			this->Clear();
		}

	public /*Class API*/:
		GDINL MutableIterator Begin()       { return MutableIterator(this->CStr()); }
		GDINL ConstIterator   Begin() const { return ConstIterator(this->CStr()); }
		GDINL MutableIterator End  ()       { return MutableIterator(this->CStr() + this->GetSize()); }
		GDINL ConstIterator   End  () const { return ConstIterator(this->CStr() + this->GetSize()); }

		/// Returns iterator that points to last container element.
		GDINL ReverseMutableIterator ReverseBegin()       { return ReverseMutableIterator(MutableIterator(this->CStr() + (this->GetSize() - 1))); }
		GDINL   ReverseConstIterator ReverseBegin() const { return   ReverseConstIterator(ConstIterator(this->CStr() + (this->GetSize() - 1))); }

		/// Returns iterator that points to preceding the first container element
		GDINL ReverseMutableIterator ReverseEnd()       { return ReverseMutableIterator(MutableIterator(this->CStr() - 1)); }
		GDINL   ReverseConstIterator ReverseEnd() const { return   ReverseConstIterator(ConstIterator(this->CStr() - 1)); }

		/// Returns length of this string.
		/// @returnes Size of this string.
		GDINL size_t GetSize() const
		{
			return this->StringSize;
		}

		GDINL bool IsEmpty() const { return (this->GetSize() != 0); }

		/// Returnes pointer on this string.
		/// @returnes C-String version of this object.
		GDINL CharType const* CStr() const
		{
			if (this->GetSize() >= BaseString::StringMaxHeapSize) {
				return this->StringHeapPointer;
			} else {
				return &this->StringStackArray[0];
			}
		}
		GDINL CharType* CStr()
		{
			return const_cast<CharType*>(const_cast<BaseString const*>(this)->CStr());
		}
		GDINL void Clear()
		{
			if (this->StringSize >= BaseString::StringMaxHeapSize) {
				Allocator::DeallocateMemory(this->StringHeapPointer);
				this->StringHeapPointer = nullptr;
			}
			this->Emptify();
		}
		GDINL void Emptify()
		{
			this->StringSize = 0;
		}
		GDINL CharType  GetLastElement() const { return (*(this->CStr() + this->GetSize() - 1)); }
		inline HashCode GetHashCode() const
		{
			HashValueType Computed = 0;
			for (auto const MyCharacter : (*this)) {
				Computed = 65599 * Computed + MyCharacter;
			}

			return HashCode((Computed >> 16) ^ Computed);
		}

		inline BaseString GetSubstring(size_t const From, size_t const To) const
		{
			GD_DEBUG_ASSERT(To >= From, "Invalid substring indices.");
			BaseString Result(To - From);
			::memcpy(Result.CStr(), &(*this)[From], (To - From) * sizeof(CharType));
			return Result;
		}
		GDINL BaseString GetSubstring(size_t const From) const
		{
			return this->GetSubstring(From, this->GetSize());
		}

		inline size_t Find(CharType const Character) const
		{
			CharType const* const CString = this->CStr();
			CharType const* const Location = CharTraits<CharType>::StrChr(CString, Character);
			if (Location != nullptr) {
				return static_cast<size_t>(Location - CString);
			} else {
				return SIZE_MAX;
			}
		}

		inline size_t Find(CharType const* const OtherCString) const
		{
			CharType const* const CString = this->CStr();
			CharType const* const Location = CharTraits<CharType>::StrStr(CString, OtherCString);
			if (Location != nullptr) {
				return static_cast<size_t>(Location - CString);
			} else {
				return SIZE_MAX;
			}
		}

		inline size_t ReverseFind(CharType const Character) const
		{
			CharType const* const CString = this->CStr();
			CharType const* const Location = CharTraits<CharType>::StrRChr(CString, Character);
			if (Location != nullptr) {
				return static_cast<size_t>(Location - CString);
			} else {
				return SIZE_MAX;
			}
		}

		inline size_t ReverseFind(CharType const* const OtherCString) const
		{
			size_t const OtherCStringLength = CharTraits<CharType>::StrLen(OtherCString);
			if (OtherCStringLength > this->StringSize) {
				CharType const* const CString = this->CStr();
				CharType const* const CStringStart = CString() + this->GetSize() - OtherCStringLength;
				CharType const* const CStringEnd = CString - 1;
				for (CharType const*  CStringIter = CStringStart; CStringIter != CStringEnd; --CStringIter) {
					 CharType const* const Location = CharTraits<CharType>::StrStr(CStringIter, OtherCString);
					if (Location != nullptr) {
						return static_cast<size_t>(Location - CString);
					}
				}
			} 
				
			return SIZE_MAX;
		}

		inline BaseString ToUpper() const
		{
			BaseString Result(*this);
			for (auto& Character : Result) {
				Character = static_cast<CharType>(CharTraits<CharType>::ToUpper(Character));
			}

			return Result;
		}

		inline BaseString ToLower() const
		{
			BaseString Result(*this);
			for (auto& Character : Result) {
				Character = static_cast<CharType>(CharTraits<CharType>::ToLower(Character));
			}

			return Result;
		}

		inline static BaseString FormatVa(CharType const* const Format, va_list const List)
		{
			CharType Buffer1024[1024];
			memset(&Buffer1024[0], 0, sizeof(Buffer1024));
			int const Result0 = CharTraits<CharType>::VSNPrintF(&Buffer1024[0], GD_ARRAY_SIZE(Buffer1024), Format, List);
			if ((Result0 > 0) && (Result0 < GD_ARRAY_SIZE(Buffer1024))) {
				return String(&Buffer1024[0]);
			} else {
				CharType Buffer2048[2048];
				memset(&Buffer2048[0], 0, sizeof(Buffer2048));
				int const Result1 = CharTraits<CharType>::VSNPrintF(&Buffer2048[0], GD_ARRAY_SIZE(Buffer2048), Format, List);
				if ((Result1 > 0) && (Result1 < GD_ARRAY_SIZE(Buffer2048))) {
					return String(&Buffer2048[0]);
				} else {
					GD_DEBUG_ASSERT_FALSE("String formatting failed due buffers error.");
				}
			}
		}

		inline static BaseString Format(CharType const* const Format, ...)
		{
			va_list List; va_start(List, Format);
			return (String::FormatVa(Format, List));
		}

	public /*Operators*/:
		/// ------------------------------------------------------------------------------------------
		/// *** Assigment Operators. ***
		/// ------------------------------------------------------------------------------------------

		GDINL BaseString& operator=	(BaseString const& OtherBaseString)
		{
			this->~BaseString();
			this->ConstructWithDataPointerAndSize(OtherBaseString.CStr(), OtherBaseString.GetSize());
			return (*this);
		}

		inline BaseString& operator= (BaseString&& OtherBaseString)
		{
			this->~BaseString();
			this->StringSize = OtherBaseString.StringSize;
			OtherBaseString.StringSize = 0;
			if (this->StringSize >= BaseString::StringMaxHeapSize) {
				this->StringHeapPointer = OtherBaseString.StringHeapPointer;
				OtherBaseString.StringHeapPointer = nullptr;
			} else {
				size_t const DataSizeInBytes = (this->StringSize + 1) * sizeof(CharType);
				::memcpy(&this->StringStackArray[0], &OtherBaseString.StringStackArray[0], DataSizeInBytes);
			}

			return (*this);
		}

		/// ------------------------------------------------------------------------------------------
		/// *** Access Operators. ***
		/// ------------------------------------------------------------------------------------------

		GDINL CharType const& operator[] (size_t const Index) const
		{
			GD_DEBUG_ASSERT(Index <= this->GetSize(), "Invalid string subindex.");
			return (*(this->CStr() + Index));
		}

		GDINL CharType& operator[] (size_t const Index)
		{
			return const_cast<Char&>((*const_cast<BaseString const*>(this))[Index]);
		}

		/// ------------------------------------------------------------------------------------------
		/// *** Concatenation Operators ***
		/// ------------------------------------------------------------------------------------------

		inline BaseString operator+ (BaseString const& OtherBaseString) const
		{
			BaseString Result(this->StringSize + OtherBaseString.GetSize());
			::memcpy(Result.CStr(),                    this->          CStr(), this->StringSize           * sizeof(CharType));
			::memcpy(Result.CStr() + this->StringSize, OtherBaseString.CStr(), OtherBaseString.StringSize * sizeof(CharType));
			return Result;
		}

		inline BaseString& operator+= (BaseString const& OtherBaseString)
		{
			BaseString Copy(Move(*this));
			return ((*this) = Move(Copy + OtherBaseString));
		}

		/// ------------------------------------------------------------------------------------------
		/// Comparation operators.
		/// ------------------------------------------------------------------------------------------

		inline bool operator== (BaseString const & OtherBaseString) const
		{
			if (this->StringSize == OtherBaseString.StringSize) {
				return (CharTraits<CharType>::StrNCmp(this->CStr(), OtherBaseString.CStr(), this->StringSize) == 0);
			} else {
				return false;
			}
		}

		inline bool operator!= (BaseString const & OtherBaseString) const
		{
			return !((*this) == OtherBaseString);
		}

		inline bool operator== (CharType const Character) const
		{
			if (this->StringSize == 1) {
				return (*this->CStr() == Character);
			} else {
				return false;
			}
		}

		inline bool operator!= (CharType const Character) const
		{
			return !((*this) == Character);
		}

	private /* STL compatibility */:
		GDINL friend MutableIterator begin(BaseString      & some_string) { return some_string.PtrBegin(); }
		GDINL friend   ConstIterator begin(BaseString const& some_string) { return some_string.PtrBegin(); }
		GDINL friend MutableIterator end  (BaseString      & some_string) { return some_string.PtrEnd(); }
		GDINL friend   ConstIterator end  (BaseString const& some_string) { return some_string.PtrEnd(); }

		template<typename CharType>
		inline friend void GD Swap(BaseString<CharType>& First, BaseString<CharType>& Second);
	};	// class BaseString

	typedef BaseString<CharAnsi > StringAnsi;
	typedef BaseString<CharUtf16> StringUtf16;
	typedef BaseString<Char     > String;

GD_NAMESPACE_END

#include <GoddamnEngine/Core/Text/String/String.inl>

#endif
