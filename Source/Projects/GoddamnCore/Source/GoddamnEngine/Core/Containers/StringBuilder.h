/// ==========================================================================================
/// StringBuilder.h: Mutable string interface.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_TEXT_STRINGBUILDER
#define GD_CORE_TEXT_STRINGBUILDER

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Containers/Vector.h>

#include <cstdarg>

GD_NAMESPACE_BEGIN

	/// @brief Represents mutable sequence of characters.
	/// @tparam CharType Character type of this string.
 	template<typename CharType>
	class BaseStringBuilder final
	{
	private:
		typedef Vector<CharType> ContainerType;
		typedef BaseString<CharType> BaseStringType;
		ContainerType Container;

	public:

		/// @brief Initializes empty string builder.
		GDINL BaseStringBuilder()
		{
			this->Container.PushLast(CharType('\0'));
		}

		/// @brief Moves other string builder into this string builder.
		/// @param Other Other string builder to move into this.
		GDINL BaseStringBuilder(BaseStringBuilder&& Other)
			: Container(Move(Other.MutableString)) 
		{
		}

		/// @brief Deinitializes a string builder.
		GDINL ~BaseStringBuilder()
		{
		}

		/// @brief Returns pointer on this string builder.
		/// @returns C-String version of this object.
		/// @{
		GDINL CharType const* CStr() const
		{
			return &this->Container[0];
		}
		GDINL CharType* CStr()
		{
			return const_cast<CharType*>(const_cast<BaseStringBuilder const*>(this)->CStr());
		}
		/// @}
		
		/// @brief Returns length of this string builder.
		/// @returns Size of this string builder.
		GDINL size_t GetLength() const
		{
			return this->Container.GetLength() - 1;
		}

		/// @brief Returns number of characters that can be placed into string builder without reallocation.
		/// @returns Number of characters that can be placed into string builder without reallocation.
		GDINL size_t GetCapacity() const
		{
			return this->Container.GetCapacity();
		}

		/// @brief Returns true if this string builder is empty.
		/// @returns True if this string builder is empty, false otherwise.
		GDINL bool IsEmpty() const
		{
			return this->GetLength() == 0;
		}

		/// @brief Resizes string builder to make it able to contain specified number of characters.
		/// @param NewLength New required length of the string.
		GDINL void Resize(size_t const NewLength)
		{ 
			this->Container.Resize(NewLength + 1);
		}

		/// @brief Reserves memory for string builder to make it contain specified number of characters without reallocation when calling Append method.
		/// @param NewCapacity New required capacity of the string.
		GDINL void Reserve(size_t const NewCapacity) 
		{ 
			this->Container.Reserve(NewCapacity + 1); 
		}

		/// @brief Appends a character to this string builder.
		/// @param Character Character to append.
 		/// @returns this.
		inline BaseStringBuilder& Append(CharType const Character)
		{
			*(this->Container.End() - 1) = Character;
			this->Container.PushLast(CharType('\0'));
			return *this;
		}

		/// @brief Appends formatable string to this string builder.
		/// @param Format Standart printf-like format.
		/// @returns this.
		inline BaseStringBuilder& AppendFormat(CharType const* const Format, ...)
		{
			/// @todo Optimize this code. possibly try to format everything inline in the builder.
			va_list ArgumentsList;
			va_start(ArgumentsList, Format);
			BaseStringType const Formatted = BaseStringType::FormatVa(Format, ArgumentsList);
			this->Append(Formatted);
			va_end(ArgumentsList);

			return *this;
		}


		/// @brief Appends a C string with specified length to this string builder.
		/// @param String C string to append.
		/// @returns this.
		inline BaseStringBuilder& Append(CharType const* const CString, size_t const Length)
		{
			size_t const StartPos = this->Container.GetLength() - 1;
			this->Container.Resize(this->Container.GetLength() + Length);
			::memcpy(&this->Container[StartPos], CString, (Length + 1) * sizeof(CharType));

			return *this;
		}

		/// @brief Appends a C string to this string builder.
		/// @param String C string to append.
		/// @returns this.
		/// @{
		template <size_t const Length>	// Length of string constant is determined at compile time..
		inline BaseStringBuilder& Append(CharType const(&CString)[Length])
		{
			return this->Append(CString, Length - 1);
		}
		inline BaseStringBuilder& Append(CharType const* const CString)
		{
			return this->Append(CString, CharTraits<CharType>::StrLen(CString));
		}
		/// @}

		/// @brief Appends a Goddamn string to this string builder.
		/// @param TheString Goddamn string to append.
		/// @returns this.
		inline BaseStringBuilder& Append(BaseStringType const& TheString)
		{
			return this->Append(TheString.CStr(), TheString.GetLength());
		}

		/// @brief Converts this builder to normal string builder.
		/// @returns C-string equivalent of this string builder.
		GDINL BaseStringType ToString() const
		{
			return BaseStringType(this->CStr(), this->GetLength());
		}

		/// @brief Moves other string builder here.
		/// @param Other String builder that would be moved into current object.
		/// @returns this.
		GDINL BaseStringBuilder& operator= (BaseStringBuilder&& Other) 
		{ 
			if (this != &Other) {
				this->Container = Move(Other.Container);
			}

			return *this;
		}
	};	// class BaseStringBuilder

	typedef BaseStringBuilder<Char> StringBuilder;

GD_NAMESPACE_END

#endif
