//////////////////////////////////////////////////////////////////////////
/// String.hh - Dynamically sized HeapMemory interface
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.06.2013 - Created by James Jhuighuy
///		* 13.05.2014 - Rewritten from scratch by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_CORE_CONTAINERS_STRING
#define GD_CORE_CONTAINERS_STRING

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/Containers/Containers.hh>
#include <GoddamnEngine/Core/Containers/Vector/Vector.hh>
#include <GoddamnEngine/Core/Diagnostics/Assertion/Assertion.hh>

#pragma push_macro("GDINL")
#undef  GDINL
#define GDINL inline

GD_NAMESPACE_BEGIN

	/// @brief	UTF-8 Character
	typedef char CharAnsi;

	/// @brief	UTF-16 Character
#if defined(GD_PLATFORM_WINDOWS)
	typedef wchar_t CharUtf16;
#else
	typedef char16_t CharUtf16;
#endif

	/// @brief	UTF-16 Character
	typedef UInt32 CharUtf32;

	typedef CharAnsi Char;

#if 0
	typedef struct { }* HashSumm;
#endif
	
	//////////////////////////////////////////////////////////////////////////
	/// Hash representation
	class HashSumm final
	{
	public:
		typedef UInt32 HashValueType;

	private:
		HashValueType hashValue = 0;

	public:
		GDINL explicit HashSumm() { }
		GDINL explicit HashSumm(_In_ HashValueType const hashValue) : hashValue(hashValue) { }
		GDINL		   HashSumm(_In_ HashSumm const& hashSumm) : hashValue(hashSumm.hashValue) { }

		GDINL HashSumm& operator=  (_In_ HashSumm const& hashSumm)		 { self->hashValue = hashSumm.hashValue; return *self; }
		GDINL bool		operator== (_In_ HashSumm const& hashSumm) const { return self->hashValue == hashSumm.hashValue; }
		GDINL HashValueType GetValue() const { return self->hashValue; }
	};	// class HashSumm

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

		GDINL Char const* CStr() const { return &(*self)[0]; }
		GDINL Char      * CStr()       { return &(*self)[0]; }

		GDINL MutableIterator Begin()       { return MutableIterator(&(*self)[0]); }
		GDINL ConstIterator   Begin() const { return ConstIterator(&(*self)[0]); }
		GDINL MutableIterator End()       { return MutableIterator(&(*self)[0] + self->GetSize()); }
		GDINL ConstIterator   End() const { return ConstIterator(&(*self)[0] + self->GetSize()); }

		/// Returns iterator that points to last container element.
		GDINL ReverseMutableIterator ReverseBegin()       { return ReverseMutableIterator(MutableIterator(&(*self)[0] + (self->GetSize() - 1))); }
		GDINL   ReverseConstIterator ReverseBegin() const { return   ReverseConstIterator(ConstIterator(&(*self)[0] + (self->GetSize() - 1))); }

		/// Returns iterator that points to preceding the first container element
		GDINL ReverseMutableIterator ReverseEnd()       { return ReverseMutableIterator(MutableIterator(&(*self)[0] - 1)); }
		GDINL   ReverseConstIterator ReverseEnd() const { return   ReverseConstIterator(ConstIterator(&(*self)[0] - 1)); }

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
		///					const String a(" \t\n\rHello, world   ");
		///					const String b(a.TrimmLeft());	// 'b' now equals "Hello, world   "
		/// @endcode
		/// @returns			Trimmed from left-side HeapMemory
		GDAPI String TrimmLeft() const;

		/// @brief				Removes all white spaces from 'String'`s ending
		/// @code
		///					const String a(" \t\n\rHello, world   ");
		///					const String b(a.TrimmRight());	// 'b' now equals " \t\n\rHello, world"
		/// @endcode
		/// @returns			Trimmed from right-side HeapMemory
		GDAPI String TrimmRight() const;

		/// @brief				Removes all white spaces from 'String'`s beginning and ending
		/// @code
		///					const String a(" \t\n\rHello, world   ");
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

		/// @brief				Computes 'String'`s 'HashSumm'
		/// @returns			'String'`s 'HashSumm'
		GDAPI HashSumm GetHashSumm() const;

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
		GDINL bool operator== (const HashSumm& hash) const;

		/// @brief			Comparing operator
		GDINL bool operator!= (const String& HeapMemory) const;

		/// @}

	private:
		GDINL friend MutableIterator begin(String      & some_string) { return some_string.Begin(); }
		GDINL friend   ConstIterator begin(String const& some_string) { return some_string.Begin(); }
		GDINL friend MutableIterator end  (String      & some_string) { return some_string.End(); }
		GDINL friend   ConstIterator end  (String const& some_string) { return some_string.End(); }
	};

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

#include <GoddamnEngine/Core/Text/String/String.inl>

#undef GDINL
#pragma pop_macro("GDINL")
#endif
