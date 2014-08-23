/// ==========================================================================================
/// String.h - Dynamically sized HeapMemory interface
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.06.2013 - Created by James Jhuighuy.
///		* 13.05.2014 - Rewritten from scratch by James Jhuighuy.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_CONTAINERS_STRING
#define GD_CORE_CONTAINERS_STRING

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/Containers.h>
#include <GoddamnEngine/Core/Containers/Vector/Vector.h>
#include <GoddamnEngine/Core/Diagnostics/Assertion/Assertion.h>

#include <cstring>
#include <cwctype>
#include <cstdarg>
#include <cwchar>
#include <cctype>

#pragma push_macro("GDINL")
#undef  GDINL
#define GDINL inline

GD_NAMESPACE_BEGIN

	typedef UInt32 HashValueType;

	/// Represents hash summ that can not be implcilty casted to integer type. 
	struct HashCode final
	{
	private:
		HashValueType HashValue = 0;

	public:
		/// Initializes hash summ with precomputed integer value.
		GDINL explicit HashCode(HashValueType const HashValue = 0) : HashValue(HashValue) { }

		/// Returns integer representation of this hash summ.
		GDINL HashValueType GetValue() const { return self->HashValue; }

		/// Compares to hash summes.
		GDINL bool operator== (HashCode const& HashCode) const { return (self->HashValue == HashCode.HashValue); }
		GDINL bool operator!= (HashCode const& HashCode) const { return (self->HashValue != HashCode.HashValue); }
	};	// struct HashCode

	template<typename CharType>
	struct CharTraits;

	/// ANSI Character.
	typedef char CharAnsi;
	template<>
	struct CharTraits<CharAnsi> final
	{
		GDINL static CharAnsi        ToLower  (CharAnsi const Character)                                                                { return static_cast<CharAnsi>(::tolower(Character)); }
		GDINL static CharAnsi        ToUpper  (CharAnsi const Character)                                                                { return static_cast<CharAnsi>(::toupper(Character)); }
		GDINL static size_t          StrLen   (CharAnsi const* const CString)                                                           { return (::strlen)(CString); }
		GDINL static CharAnsi const* StrStr   (CharAnsi const* const First, CharAnsi const* const Second)                               { return (::strstr)(First, Second); }
		GDINL static CharAnsi const* StrChr   (CharAnsi const* const CString, CharAnsi const Character)                                 { return (::strchr)(CString, Character); }
		GDINL static CharAnsi const* StrRChr  (CharAnsi const* const CString, CharAnsi const Character)                                 { return (::strrchr)(CString, Character); }
		GDINL static int             StrNCmp  (CharAnsi const* const First, CharAnsi const* const Second, size_t const MaxLength)       { return (::strncmp)(First, Second, MaxLength); }
		GDINL static int             VSNPrintF(CharAnsi* CString, size_t const Count, CharAnsi const* const Format, va_list const Args) { return (::vsnprintf)(CString, Count, Format, Args); }
	};	// struct CharTraits<CharAnsi>

	/// Unicode Character.
#if defined(GD_COMPILER_MSVC)
	typedef wchar_t CharUtf16;
#else	// if defined(GD_COMPILER_MSVC)
	typedef char16_t CharUtf16;
#endif	// if defined(GD_COMPILER_MSVC)
	template<>
	struct CharTraits<CharUtf16> final
	{
		GDINL static CharUtf16        ToLower  (CharUtf16 const Character)                                                                 { return static_cast<CharUtf16>(::towlower(Character)); }
		GDINL static CharUtf16        ToUpper  (CharUtf16 const Character)                                                                 { return static_cast<CharUtf16>(::towupper(Character)); }
		GDINL static size_t           StrLen   (CharUtf16 const* const CString)                                                            { return (::wcslen)(CString); }
		GDINL static CharUtf16 const* StrStr   (CharUtf16 const* const First, CharUtf16 const* const Second)                               { return (::wcsstr)(First, Second); }
		GDINL static CharUtf16 const* StrChr   (CharUtf16 const* const CString, CharUtf16 const Character)                                 { return (::wcschr)(CString, Character); }
		GDINL static CharUtf16 const* StrRChr  (CharUtf16 const* const CString, CharUtf16 const Character)                                 { return (::wcsrchr)(CString, Character); }
		GDINL static int              StrNCmp  (CharUtf16 const* const First, CharUtf16 const* const Second, size_t const MaxLength)       { return (::wcsncmp)(First, Second, MaxLength); }
		GDINL static int              VSNPrintF(CharUtf16* CString, size_t const Count, CharUtf16 const* const Format, va_list const Args) { return (::vswprintf)(CString, Count, Format, Args); }
	};	// struct CharTraits<CharUtf16>

#if (defined(_UNICODE))
#	define GD_TEXT(Text) L##Text
	typedef CharUtf16 Char;
#else	// if (defined(_UNICODE))
#	define GD_TEXT(Text) Text
	typedef CharAnsi Char;
#endif	// if (defined(_UNICODE))

	template<typename CharType> 
	class BaseString final
	{
		/// ------------------------------------------------------------------------------------------
		/// Iterator type.
		/// ------------------------------------------------------------------------------------------
	private /*Class internal types.*/:
		template<typename Tag>
		struct Iterator final
		{
			typedef Char ThisElementType;
			typedef typename Conditional<Tag::IsConst, Char const*, Char*>::Type ThisPtrType;
			typedef typename Conditional<Tag::IsConst, Char const&, Char&>::Type ThisRefType;

		private:
			ThisPtrType Pointer = nullptr;

		public:
			GDINL  Iterator(ThisPtrType const  Pointer) : Pointer(Pointer) { }
			GDINL  Iterator(Iterator    const& Iterator) : Pointer(Iterator.Pointer) { }
			GDINL ~Iterator() { }

			/// Increases/decreases iterator.
			GDINL Iterator& operator++ (int const) { ++self->Pointer; return (*self); }
			GDINL Iterator& operator++ (         ) { ++self->Pointer; return (*self); }
			GDINL Iterator& operator-- (int const) { --self->Pointer; return (*self); }
			GDINL Iterator& operator-- (         ) { --self->Pointer; return (*self); }

			/// Increases/decreases iterator on specified value.
			inline Iterator& operator+= (ptrdiff_t const Offset)	   { self->Pointer += Offset; return (*self); }
			inline Iterator& operator-= (ptrdiff_t const Offset)       { self->Pointer -= Offset; return (*self); }
			inline Iterator  operator+  (ptrdiff_t const Offset) const { return Iterator(self->Pointer + Offset); }
			inline Iterator  operator-  (ptrdiff_t const Offset) const { return Iterator(self->Pointer - Offset); }

			/// Computes difference between iterators.
			inline ptrdiff_t operator- (Iterator const&       Iterator) const { return (self->Pointer - Iterator.Pointer); }
			inline ptrdiff_t operator- (Char     const* const  Pointer) const { return (self->Pointer - Pointer); }

			/// Compares iterators.
			GDINL bool operator== (Iterator const&       Other  ) const { return (self->Pointer == Other.Pointer); }
			GDINL bool operator!= (Iterator const&       Other  ) const { return (self->Pointer != Other.Pointer); }
			GDINL bool operator== (Char     const* const Pointer) const { return (self->Pointer == Pointer); }
			GDINL bool operator!= (Char     const* const Pointer) const { return (self->Pointer != Pointer); }

			/// Assigns this iterator other value.
			GDINL Iterator& operator= (ThisPtrType const  Pointer) { self->Pointer = Pointer; return (*self); }
			GDINL Iterator& operator= (Iterator    const& Iterator) { self->Pointer = Iterator->Pointer; return (*self); }

			/// (De)referensing iterator.
			GDINL ThisRefType operator*  () const { return (*self->Pointer); }
			GDINL ThisPtrType operator-> () const { return (self->Pointer); }
		};	// struct Iterator
	
	public /*Class public types.*/:
		/// Iterator type this container uses.
		typedef Iterator<ContainerDetails::IteratorTagMutable> MutableIterator;
		typedef Iterator<ContainerDetails::IteratorTagConst  > ConstIterator;

		/// Reverse iterator type this container uses.
		typedef ContainerDetails::ReverseIterator<MutableIterator> ReverseMutableIterator;
		typedef ContainerDetails::ReverseIterator<ConstIterator  > ReverseConstIterator;

	private /*Class members & API*/:
		enum : size_t { StringMaxHeapSize = 16 };
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
			self->StringSize = Size;
			size_t const DataSizeInBytes = (Size + 1) * sizeof(CharType);
			if (self->StringSize >= BaseString::StringMaxHeapSize) {
				self->StringHeapPointer = reinterpret_cast<CharType*>(Allocator::AllocateMemory(DataSizeInBytes));
				::memcpy(self->StringHeapPointer, Chars, DataSizeInBytes);
			} else {
				::memcpy(&self->StringStackArray[0], Chars, DataSizeInBytes);
			}
		}

		/// Initializes a string with some C string with unknown length.
		/// @param Chars  String initial data.
		GDINL void ConstructWithDataPointer(CharType const* const Chars)
		{
			self->ConstructWithDataPointerAndSize(Chars, CharTraits<CharType>::StrLen(Chars));
		}

	public /*Constructors/Destructor*/:
		/// ------------------------------------------------------------------------------------------
		/// *** Standart constructors. ***
		/// ------------------------------------------------------------------------------------------

		/// Initializes an empty string.
		GDINL BaseString()
		{
			self->StringSize = 0;
			self->StringStackArray[0] = CharType('\0');
		}

		/// Initializes this string with copy of other string.
		/// @param OtherBaseString Other string to copy.
		GDINL BaseString(BaseString const& OtherBaseString)
		{
			self->ConstructWithDataPointerAndSize(OtherBaseString.CStr(), OtherBaseString.GetSize());
		}

		/// Moves other string into this string.
		/// @param OtherBaseString Other string to move into this.
		GDINL BaseString(BaseString&& OtherBaseString)
		{
			(*self) = Forward<BaseString>(OtherBaseString);
		}

		/// ------------------------------------------------------------------------------------------
		/// *** Custom constructors. ***
		/// ------------------------------------------------------------------------------------------

		/// Initializes a string with a single character.
		/// @param Character Initial string character.
		GDINL BaseString(Char const Character)
		{
			self->StringSize = 1;
			self->StringStackArray[0] = Character;
			self->StringStackArray[1] = CharType('\0');
		}

		/// Initializes a string with some C string with known length.
		/// @param Chars  String initial data.
		/// @param Size Size of string initial data.
		GDINL BaseString(CharType const* const Chars, size_t const Size)
		{
			self->ConstructWithDataPointerAndSize(Chars, Size);
		}

		/// Initializes a string with some C string with unknown length.
		/// @param Chars  String initial data.
		GDINL BaseString(CharType const* const Chars)
		{
			self->ConstructWithDataPointer(Chars);
		}

		/// Fills a string with specified number of characters.
		/// @param Size   A length of string.
		/// @param FillWith A character that string would be filled with.
		inline BaseString(size_t const Size, CharType const FillWith = CharType('\0'))
		{
			if ((self->StringSize = Size) >= BaseString::StringMaxHeapSize) {
				size_t const DataSizeInBytes = (Size + 1) * sizeof(CharType);
				self->StringHeapPointer = reinterpret_cast<CharType*>(Allocator::AllocateMemory(DataSizeInBytes));
			}

			if (sizeof(CharType) != 1) {
				for (auto& Character : (*self)) {
					Character = FillWith;
				}
			} else {
				memset(self->CStr(), FillWith, Size);
			}

			(*(self->CStr() + self->GetSize())) = CharType('\0');
		}

		/// ------------------------------------------------------------------------------------------
		/// *** Destructor. ***
		/// ------------------------------------------------------------------------------------------

		/// Deinitialzes string.
		GDINL ~BaseString()
		{
			self->Clear();
		}

	public /*Class API*/:
		GDINL MutableIterator Begin()       { return MutableIterator(self->CStr()); }
		GDINL ConstIterator   Begin() const { return ConstIterator(self->CStr()); }
		GDINL MutableIterator End  ()       { return MutableIterator(self->CStr() + self->GetSize()); }
		GDINL ConstIterator   End  () const { return ConstIterator(self->CStr() + self->GetSize()); }

		/// Returns iterator that points to last container element.
		GDINL ReverseMutableIterator ReverseBegin()       { return ReverseMutableIterator(MutableIterator(self->CStr() + (self->GetSize() - 1))); }
		GDINL   ReverseConstIterator ReverseBegin() const { return   ReverseConstIterator(ConstIterator(self->CStr() + (self->GetSize() - 1))); }

		/// Returns iterator that points to preceding the first container element
		GDINL ReverseMutableIterator ReverseEnd()       { return ReverseMutableIterator(MutableIterator(self->CStr() - 1)); }
		GDINL   ReverseConstIterator ReverseEnd() const { return   ReverseConstIterator(ConstIterator(self->CStr() - 1)); }

		/// Returns length of this string.
		/// @returnes Size of this string.
		GDINL size_t GetSize() const
		{
			return self->StringSize;
		}

		GDINL bool IsEmpty() const { return (self->GetSize() != 0); }

		/// Returnes pointer on this string.
		/// @returnes C-String version of this object.
		GDINL CharType const* CStr() const
		{
			if (self->GetSize() >= BaseString::StringMaxHeapSize) {
				return self->StringHeapPointer;
			} else {
				return &self->StringStackArray[0];
			}
		}
		GDINL CharType* CStr()
		{
			return const_cast<CharType*>(const_cast<BaseString const*>(self)->CStr());
		}
		GDINL void Clear()
		{
			if (self->StringSize >= BaseString::StringMaxHeapSize) {
				Allocator::DeallocateMemory(self->StringHeapPointer);
				self->StringHeapPointer = nullptr;
			}
			self->Emptify();
		}
		GDINL void Emptify()
		{
			self->StringSize = 0;
		}
		GDINL CharType  GetLastElement() const { return (*(self->CStr() + self->GetSize() - 1)); }
		inline HashCode GetHashCode() const
		{
			HashValueType Computed = 0;
			for (auto const MyCharacter : (*self)) {
				Computed = 65599 * Computed + MyCharacter;
			}

			return HashCode((Computed >> 16) ^ Computed);
		}

		inline BaseString GetSubstring(size_t const From, size_t const To) const
		{
			GD_DEBUG_ASSERT(To >= From, "Invalid substring indices.");
			BaseString Result(To - From);
			::memcpy(Result.CStr(), &(*self)[From], (To - From) * sizeof(CharType));
			return Result;
		}
		GDINL BaseString GetSubstring(size_t const From) const
		{
			return self->GetSubstring(From, self->GetSize());
		}

		inline size_t Find(CharType const Character) const
		{
			CharType const* const CString = self->CStr();
			CharType const* const Location = CharTraits<CharType>::StrChr(CString, Character);
			if (Location != nullptr) {
				return static_cast<size_t>(Location - CString);
			} else {
				return SIZE_MAX;
			}
		}

		inline size_t Find(CharType const* const OtherCString) const
		{
			CharType const* const CString = self->CStr();
			CharType const* const Location = CharTraits<CharType>::StrStr(CString, OtherCString);
			if (Location != nullptr) {
				return static_cast<size_t>(Location - CString);
			} else {
				return SIZE_MAX;
			}
		}

		inline size_t ReverseFind(CharType const Character) const
		{
			CharType const* const CString = self->CStr();
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
			if (OtherCStringLength > self->StringSize) {
				CharType const* const CString = self->CStr();
				CharType const* const CStringStart = CString() + self->GetSize() - OtherCStringLength;
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
			BaseString Result(*self);
			for (auto& Character : Result) {
				Character = static_cast<CharType>(CharTraits<CharType>::ToUpper(Character));
			}

			return Result;
		}

		inline BaseString ToLower() const
		{
			BaseString Result(*self);
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
			self->~BaseString();
			self->ConstructWithDataPointerAndSize(OtherBaseString.CStr(), OtherBaseString.GetSize());
			return (*self);
		}

		inline BaseString& operator= (BaseString&& OtherBaseString)
		{
			self->~BaseString();
			self->StringSize = OtherBaseString.StringSize;
			OtherBaseString.StringSize = 0;
			if (self->StringSize >= BaseString::StringMaxHeapSize) {
				self->StringHeapPointer = OtherBaseString.StringHeapPointer;
				OtherBaseString.StringHeapPointer = nullptr;
			} else {
				size_t const DataSizeInBytes = (self->StringSize + 1) * sizeof(CharType);
				::memcpy(&self->StringStackArray[0], &OtherBaseString.StringStackArray[0], DataSizeInBytes);
			}

			return (*self);
		}

		/// ------------------------------------------------------------------------------------------
		/// *** Access Operators. ***
		/// ------------------------------------------------------------------------------------------

		GDINL CharType const& operator[] (size_t const Index) const
		{
			GD_DEBUG_ASSERT(Index <= self->GetSize(), "Invalid string subindex.");
			return (*(self->CStr() + Index));
		}

		GDINL CharType& operator[] (size_t const Index)
		{
			return const_cast<Char&>((*const_cast<BaseString const*>(self))[Index]);
		}

		/// ------------------------------------------------------------------------------------------
		/// *** Concatenation Operators ***
		/// ------------------------------------------------------------------------------------------

		inline BaseString operator+ (BaseString const& OtherBaseString) const
		{
			BaseString Result(self->StringSize + OtherBaseString.GetSize());
			::memcpy(Result.CStr(),                    self->          CStr(), self->StringSize           * sizeof(CharType));
			::memcpy(Result.CStr() + self->StringSize, OtherBaseString.CStr(), OtherBaseString.StringSize * sizeof(CharType));
			return Result;
		}

		inline BaseString& operator+= (BaseString const& OtherBaseString)
		{
			BaseString Copy(Move(*self));
			return ((*self) = Move(Copy + OtherBaseString));
		}

		/// ------------------------------------------------------------------------------------------
		/// Comparation operators.
		/// ------------------------------------------------------------------------------------------

		inline bool operator== (BaseString const & OtherBaseString) const
		{
			if (self->StringSize == OtherBaseString.StringSize) {
				return (CharTraits<CharType>::StrNCmp(self->CStr(), OtherBaseString.CStr(), self->StringSize) == 0);
			} else {
				return false;
			}
		}

		inline bool operator!= (BaseString const & OtherBaseString) const
		{
			return !((*self) == OtherBaseString);
		}

		inline bool operator== (CharType const Character) const
		{
			if (self->StringSize == 1) {
				return (*self->CStr() == Character);
			} else {
				return false;
			}
		}

		inline bool operator!= (CharType const Character) const
		{
			return !((*self) == Character);
		}

	private /**/:
		/// ------------------------------------------------------------------------------------------
		/// *** Range Operators. ***
		/// ------------------------------------------------------------------------------------------

		GDINL friend MutableIterator begin(BaseString      & some_string) { return some_string.Begin(); }
		GDINL friend   ConstIterator begin(BaseString const& some_string) { return some_string.Begin(); }
		GDINL friend MutableIterator end  (BaseString      & some_string) { return some_string.End(); }
		GDINL friend   ConstIterator end  (BaseString const& some_string) { return some_string.End(); }
	};	// class BaseString

	typedef BaseString<CharAnsi > ANSIString;
	typedef BaseString<CharUtf16> WideString;
	typedef BaseString<Char     >     String;
	 
#if 0
	/// @brief	Basic String class in GoddamnEngine
	///			This class uses default C's character type. UTF-8 and UTF-16 characters are not supported.
	///			Usage:
	///			@code
	///				String a("Hello,");
	///				String b(L" World");
	///				Debug::Log(a + b + "!");
	///			@endcode
	class String final
	{
	private:
		bool IsOnHeap = true;
		union {
			struct {
				Char*  HeapMemory;
				size_t HeapSize;
				size_t HeapCapacity;
			};	// anonymous struct
			struct {
				enum : size_t { StackCapacity = (3 * sizeof(size_t) - 1) };
				Char   StackMemory[StackCapacity];
				UInt8  StackSize;
			};	// anonymous struct
		};	// anonymous union

	public:
		template<typename Tag>
		struct Iterator final
		{
			typedef Char ThisElementType;
			typedef typename Conditional<Tag::IsConst, Char const*, Char*>::Type ThisPtrType;
			typedef typename Conditional<Tag::IsConst, Char const&, Char&>::Type ThisRefType;

		private:
			ThisPtrType Pointer = nullptr;

		public:
			GDINL  Iterator(ThisPtrType const  Pointer) : Pointer(Pointer) { }
			GDINL  Iterator(Iterator    const& Iterator) : Pointer(Iterator.Pointer) { }
			GDINL ~Iterator() { }

			/// Increases/decreases iterator.
			GDINL Iterator& operator++ (int const) { ++self->Pointer; return (*self); }
			GDINL Iterator& operator++ () { ++self->Pointer; return (*self); }
			GDINL Iterator& operator-- (int const) { --self->Pointer; return (*self); }
			GDINL Iterator& operator-- () { --self->Pointer; return (*self); }

			/// Increases/decreases iterator on specified value.
			inline Iterator& operator+= (ptrdiff_t const Offset)	      { self->Pointer += Offset; return (*self); }
			inline Iterator& operator-= (ptrdiff_t const Offset)       { self->Pointer -= Offset; return (*self); }
			inline Iterator  operator+  (ptrdiff_t const Offset) const { return Iterator(self->Pointer + Offset); }
			inline Iterator  operator-  (ptrdiff_t const Offset) const { return Iterator(self->Pointer - Offset); }

			/// Computes difference between iterators.
			inline ptrdiff_t operator- (Iterator const& Iterator) const { return (self->Pointer - Iterator.Pointer); }
			inline ptrdiff_t operator- (Char     const* const  Pointer) const { return (self->Pointer - Pointer); }

			/// Compares iterators.
			GDINL bool operator== (Iterator const&       Other) const { return (self->Pointer == Other.Pointer); }
			GDINL bool operator!= (Iterator const&       Other) const { return (self->Pointer != Other.Pointer); }
			GDINL bool operator== (Char     const* const Pointer) const { return (self->Pointer == Pointer); }
			GDINL bool operator!= (Char     const* const Pointer) const { return (self->Pointer != Pointer); }

			/// Assigns this iterator other value.
			GDINL Iterator& operator= (ThisPtrType const  Pointer) { self->Pointer = Pointer; return (*self); }
			GDINL Iterator& operator= (Iterator    const& Iterator) { self->Pointer = Iterator->Pointer; return (*self); }

			/// (De)referensing iterator.
			GDINL ThisRefType operator*  () const { return (*self->Pointer); }
			GDINL ThisPtrType operator-> () const { return (self->Pointer); }
		};	// struct Iterator

		/// Iterator type this container uses.
		typedef Iterator<ContainerDetails::IteratorTagConst  >   ConstIterator;
		typedef Iterator<ContainerDetails::IteratorTagMutable> MutableIterator;

		/// Reverse iterator type this container uses.
		typedef ContainerDetails::ReverseIterator<MutableIterator> ReverseMutableIterator;
		typedef ContainerDetails::ReverseIterator<ConstIterator  > ReverseConstIterator;

		/// @name String Format
		/// @{

		/// @brief			Builds 'String' from 'format'
		/// @param format	Format to build 'String'
		/// @param ...		Parameters to be passed to formatter:
		///		@li @p '^'	-	IncrdibleEngine`s 'String'
		///		@li @p 's' -	@p 'CharAnsi' pointer
		///		@li @p 'w' -	@p 'CharUtf16' pointer
		///		@li @p 'f' -	@p 'float' value
		///		@li @p 'd' -	@p 'int' value
		///		@li @p 'x' -	@p 'handle' value
		///		@li @p 'c' -	@p 'CharAnsi' value
		///		@li @p 'u' -	@p 'CharUtf16' value
		///		@li @p 'U' -	@p 'CharUtf32' value
		///		@li @p '\%' -	@p '\%' character
		/// @n
		///		Use this method instead of 'String' concatenation. It's faster,
		///		and looks more effective and good.
		/// @n
		///		Usage:
		///		@code
		///			String a = String::Format("Hello, %s!", "World");
		///		@endcode
		GDAPI static String Format(const CharAnsi* format, ...);

		/*/// @see Format(const CharAnsi* format, ...)
		GDAPI static String&& Format(const CharUtf16* format, ...);

		/// @see Format(const CharAnsi* format, ...)
		GDAPI static String&& Format(const CharUtf32* format, ...);*/

		/// @see Format(const CharAnsi* format, ...)
		GDAPI static String FormatVa(Str const Format, va_list const List);

		/// @}

		/// @name BASE-64 encoding and decoding
		/// @{

		/// @brief				Encodes HeapMemory in BASE-64
		/// @param data			Data to encode
		/// @param output		Reference to resulting HeapMemory
		GDAPI static void EncodeBase64(const Vector<UInt8>& data, String& output);

		/// @brief				Decodes HeapMemory in BASE-64
		/// @param base64		Encoded HeapMemory
		/// @param output		Reference to resulting data vector
		GDAPI static void DecodeBase64(Vector<UInt8>& output, const String& base64);

		/// @}

		/// @name Constructors / Destructor
		/// @{

		/// @brief				Allocates sized HeapMemory.
		/// @param _nullptr		Should be 'nullptr'
		/// @param HeapSize			Size for new String
		GDAPI String(const nullptr_t _nullptr = nullptr, const size_t HeapSize = 0, Char const fillWith = '\0');

		/// @brief				Copy constructor
		/// @param HeapMemory		Copies from HeapMemory.
		GDAPI String(const String& HeapMemory);
		GDINL String(String&& HeapMemory)
		{
			self->HeapSize = HeapMemory.HeapSize;
			self->HeapMemory = HeapMemory.HeapMemory;
			HeapMemory.HeapMemory = nullptr;
			HeapMemory.HeapSize = 0;
		}

		/// @brief				Copies data from 'Vector'
		/// @param queue		Copies from Vector.
		GDINL String(const Vector<CharAnsi>& vector);

		/// @brief				Copies data from 'Vector'
		/// @param queue		Copies from Vector.
		GDINL String(const Vector<CharUtf16>& vector);

		/// @brief				Copies data from 'Vector'
		/// @param queue		Copies from Vector.
		GDINL String(const Vector<CharUtf32>& vector);

		/// @brief				Copies data from UTF-8 String
		/// @param utf8			Pointer to character array
		GDAPI String(const CharAnsi* utf8);

		/// @brief				Copies data from UTF-16 String
		/// @param utf16		Pointer to character array
		GDAPI String(const CharUtf16* utf16);

		/// @brief				Copies data from UTF-32 String
		/// @param utf16		Pointer to character array
		GDAPI String(const CharUtf32* utf32); 

		/// @brief				Formats data from float
		/// @param value		Float to copy from
		GDAPI String(const int value);

		/// @brief				Formats data from float
		/// @param value		Float to copy from
		GDAPI String(const float value);

		/// @brief				Formats data from float
		/// @param value		Pointer to copy from
		GDAPI String(const handle value);

		/// @brief				Formats data from UTF-8 character
		/// @param value		Float to copy from
		GDAPI String(const CharAnsi value);

		/// @brief				Formats data from UTF-16 character
		/// @param value		Float to copy from
		GDAPI String(const CharUtf16 value);

		/// @brief				Formats data from UTF-32 character
		/// @param value		Float to copy from
		GDAPI String(const CharUtf32 value);
	
		/// @brief				Destructs HeapMemory.
		GDAPI ~String();

		/// @}

		GDINL Char const* CStr() const { return self->CStr(); }
		GDINL Char      * CStr()       { return self->CStr(); }

		GDINL MutableIterator Begin()       { return MutableIterator(self->CStr()); }
		GDINL ConstIterator   Begin() const { return ConstIterator(self->CStr()); }
		GDINL MutableIterator End()       { return MutableIterator(self->CStr() + self->GetSize()); }
		GDINL ConstIterator   End() const { return ConstIterator(self->CStr() + self->GetSize()); }

		/// Returns iterator that points to last container element.
		GDINL ReverseMutableIterator ReverseBegin()       { return ReverseMutableIterator(MutableIterator(self->CStr() + (self->GetSize() - 1))); }
		GDINL   ReverseConstIterator ReverseBegin() const { return   ReverseConstIterator(ConstIterator(self->CStr() + (self->GetSize() - 1))); }

		/// Returns iterator that points to preceding the first container element
		GDINL ReverseMutableIterator ReverseEnd()       { return ReverseMutableIterator(MutableIterator(self->CStr() - 1)); }
		GDINL   ReverseConstIterator ReverseEnd() const { return   ReverseConstIterator(ConstIterator(self->CStr() - 1)); }

		/// @name 'String'`s HeapSize
		/// @{

		/// @brief				Returns 'String'`s length
		/// @returns			'String'`s length
		GDINL size_t GetSize() const;

		GDINL bool IsEmpty() const { return self->GetSize() == 0; }

		/// @brief				Resizes a 'String'
		/// @param HeapSize			New HeapSize
		GDAPI void Resize(const size_t HeapSize);
		GDINL void Emptify() { self->Resize(0); }

		GDINL void SetLastElement(Char const C) { *(self->End() - 1) = C; }
		GDINL Char const& GetLastElement() const { return (*(self->End() - 1)); }

		/// @}

		/// @name Trimming
		/// @{

		/// @brief				Removes all white spaces from 'String'`s beginning
		/// @code
		///					const String a(" \t\n\RHello, world   ");
		///					const String b(a.TrimmLeft());	// 'b' now equals "Hello, world   "
		/// @endcode
		/// @returns			Trimmed from left-side HeapMemory
		GDAPI String TrimmLeft() const;

		/// @brief				Removes all white spaces from 'String'`s ending
		/// @code
		///					const String a(" \t\n\RHello, world   ");
		///					const String b(a.TrimmRight());	// 'b' now equals " \t\n\RHello, world"
		/// @endcode
		/// @returns			Trimmed from right-side HeapMemory
		GDAPI String TrimmRight() const;

		/// @brief				Removes all white spaces from 'String'`s beginning and ending
		/// @code
		///					const String a(" \t\n\RHello, world   ");
		///					const String b(a.Trimm());	// 'b' now equals "Hello, world"
		/// @endcode
		/// @returns			Trimmed HeapMemory
		GDINL String Trimm() const;

		/// @}

		/// @name Substring
		/// @{

		/// @brief				Returns a substring from this 'String'
		/// @param	from		Index of first character needed, including it
		/// @param	to			Index of last character needed, not including it
		/// @returns			A substring from this 'String'
		GDAPI String GetSubstring(const size_t from, const size_t to) const;

		/// @brief				Returns a substring from this 'String'
		/// @param	from		Index of first character needed, including it
		/// @returns			A substring from this 'String' from 'from' to last character
		GDINL String GetSubstring(const size_t from) const;

		/// @}

		/// @name String Find
		/// @{

		GDAPI bool StartsWith(Str const Prefix) const
		{
			GD_UNUSED(Prefix);
			return true;
		}

		GDAPI bool EndsWith(Str const Postfix) const
		{
			GD_UNUSED(Postfix);
			return true;
		}

		/// @brief				Searches for any instances of HeapMemory and returns first index found
		/// @param HeapMemory		String to search for
		/// @returns			First index found or '-1' if found nothing
		GDAPI size_t Find(const String& HeapMemory) const;

		/// @brief				Searches for any instances of character and returns first index found
		/// @param HeapMemory		String to search for
		/// @returns			First index found or '-1' if found nothing
		GDINL size_t Find(const Char character) const;

		/// @brief				Searches for any instances of HeapMemory and returns last index found
		/// @param HeapMemory		String to search for
		/// @returns			Last index found or '-1' if found nothing
		GDAPI size_t ReverseFind(const String& HeapMemory) const;

		/// @brief				Searches for any instances of character and returns last index found
		/// @param HeapMemory		String to search for
		/// @returns			Last index found or '-1' if found nothing
		GDAPI size_t ReverseFind(const Char character) const;

		/// @}

		/// @name String Split
		/// @{

		/// @brief				Splits 'String' into a 'Vector' by 'delimiter
		/// @param delimiter	Delimiter to split with
		/// @param include		Add delimiter to output vector
		/// @returns			Splinted 'String' into a 'Vector' by 'delimiter
		/// @code
		///				String a("iPhone is better than every Android device");
		///				Vector<String> splitted0 = a.Split(' ');		// 'splitted0' is now { "iPhone", "is", "better", ... }
		///				Vector<String> splitted1 = a.Split(' ', true);	// 'splitted1' is now { "iPhone", " ", "is", " ", "better", ... }
		/// @endcode
		GDAPI Vector<String> Split(const Char delimiter, const bool include = false) const;

		/// @brief			Spites 'String' by index
		///					Spites 'String' by index
		///					and assigns to this right part of this String, returns left part
		/// @param index	Index to be slitted with
		/// @returns		Substring of this 'String' from index
		/// @code
		///					String a("012345");
		///					String b = a.Split(3);	// 'a' now is "012", 'b' is "345"
		/// @endcode
		GDAPI String Split(const size_t index);

		/// @}

		/// @name Converters
		/// @{

		/// @brief				Converts 'String' to float.
		/// @returns			Float representation of String
		GDAPI float ToFloat() const;

		/// @brief				Converts 'String' to float.
		/// @returns			Integer representation of String
		GDAPI int ToInt() const;

		/// @brief				Converts HeapMemory to upper-case
		/// @returns			Upper-cased HeapMemory
		GDAPI String ToUpper() const;

		/// @brief				Converts HeapMemory to lower-case
		/// @returns			Upper-cased HeapMemory
		GDAPI String ToLower() const;

		/// @}

		/// @brief				Computes 'String'`s 'HashCode'
		/// @returns			'String'`s 'HashCode'
		GDAPI HashCode GetHashCode() const;

		/// @brief				Adds element to 'String'
		/// @param character	Char to add
		GDINL void PushLast(const Char character);

		/// @brief				Removes last character from 'String'
		GDINL void PopLast();

		/// @name Operators
		/// @{

		/// @brief			Accesses the character in 'String'
		/// @param index	Index in 'String'
		/// @returns		Character in 'String'
		GDINL const Char& operator[] (const size_t index) const;

		/// @brief			Accesses the character in 'String'
		/// @param index	Index in 'String'
		/// @returns		Reference on character in 'String'
		GDINL Char& operator[] (const size_t index);
		
		/// @brief			Assignment operator
		GDAPI String& operator=	(const String& HeapMemory);

		/// @brief			Concatenation operator
		GDAPI String operator+ (const String& HeapMemory) const;

		/// @brief			Concatenation operator
		GDINL String operator+ (const Char* HeapMemory) const;

		/// @brief			Concatenation operator
		GDINL String& operator+= (const String& HeapMemory);

		/// @brief			Concatenation operator
		GDINL String& operator+= (const Char* HeapMemory);
		
		/// @brief			Comparing operator
		GDAPI bool operator== (const String& s) const;

		/// @brief			Comparing operator
		GDINL bool operator== (const HashCode& hash) const;

		/// @brief			Comparing operator
		GDINL bool operator!= (const String& HeapMemory) const;

		/// @}

	private:
		GDINL friend MutableIterator begin(String      & some_string) { return some_string.Begin(); }
		GDINL friend   ConstIterator begin(String const& some_string) { return some_string.Begin(); }
		GDINL friend MutableIterator end  (String      & some_string) { return some_string.End(); }
		GDINL friend   ConstIterator end  (String const& some_string) { return some_string.End(); }
	};
#endif	// if 0

	/// Provides helper functions for processing ANSI characters.
	namespace CharAnsiHelpers
	{
		/// Returns true if this character is valid digit in specified notation.
		/// Currently supports notations range between 2 and 36.
		/// Full list of supported characters: @c 0123456789
		inline static bool IsDigit(CharAnsi const Character, size_t const Notation = 10)
		{
			GD_ASSERT((Notation >= 2) && (Notation <= static_cast<size_t>(('9' - '0') + ('Z' - 'A') + 2)), "This notation is invalid or not supported");
			if (Notation > 10)
			{
				if ((Character >= CharAnsi('0')) && (Character <= CharAnsi('9')))
					return true;
				if (Character >= CharAnsi('A') && (Character <= (CharAnsi('A') + Notation - 10)))
					return true;
				if (Character >= CharAnsi('a') && (Character <= (CharAnsi('z') + Notation - 10)))
					return true;

				return false;
			}
			else
				return (Character >= CharAnsi('0')) && (Character <= (CharAnsi('0') + Notation));
		}

		/// Returns true if this character can be used in identifier name.
		/// Full list of supported characters: @c _$QqWwEeRrTtYyUuIiOoPpAaSsDdFfGgHhJjKkLlZzXxCcVvBbNnMm
		inline static bool IsAlphabetic(CharAnsi const Character)
		{
			if ((Character == CharAnsi('_')) || (Character == CharAnsi('$')))
				return true;
			if (Character >= CharAnsi('A') && (Character <= CharAnsi('Z')))
				return true;
			if (Character >= CharAnsi('a') && (Character <= CharAnsi('z')))
				return true;
			return false;
		}

		/// Returns true if this character is valid special character
		/// Full list of supported characters: @c ~`!@#$%^&*()-_+={}[]:;"'|\<>,.?/
		GDINL static bool IsSpecialCharacter(CharAnsi const Character)
		{
			static String const ValidSpecialCharacters(R"(~`!@#%^&*()-+={}[]:;"'|\<>,.?/)");
			return (ValidSpecialCharacters.Find(Character) != -1);
		}

		/// Returns true if this character is valid special character
		/// Full list of supported characters: space (@c ' '), tabulation (@c '\t') and end-of-line characters (@c '\n', '\r')
		GDINL static bool IsSpace(CharAnsi const Character)
		{
			static String const SpaceCharacters(" \t\n\r");
			return (SpaceCharacters.Find(Character) != -1);
		}

		/// Converts this character in specified notation to decimal one
		/// Currently supports notations range between 2 and 36.
		GDINL static UInt8 ToDigit(Char const Character)
		{
			return ((Character >= CharAnsi('0')) && (Character <= CharAnsi('9')))
				  ? (Character  - CharAnsi('0')) : ((Character - CharAnsi('A')) + 10);
		}

		/// Converts this character into upper case
		GDINL static UInt8 ToUpperCase(Char const Character)
		{
			GD_DEBUG_ASSERT(CharAnsiHelpers::IsAlphabetic(Character), "Invalid character specified");
			return ((Character >= CharAnsi('a')) && (Character <= CharAnsi('z'))) ? (Character + (CharAnsi('A') - CharAnsi('a'))) : (Character);
		}
	}	// namespace CharAnsiHelpers

GD_NAMESPACE_END

#if 0
#	include <GoddamnEngine/Core/Text/String/String.inl>
#endif	// if 0

#undef GDINL
#pragma pop_macro("GDINL")
#endif
