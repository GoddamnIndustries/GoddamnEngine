/// ==========================================================================================
/// StringBuilder.h: Mutable string implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 16.06.2014 - Created by James Jhuighuy.
/// ==========================================================================================

#if (!defined(GD_CORE_TEXT_STRINGBUILDER_STRINGBUILDER))
#	error "Attempting to include 'StringBuilder.inl' file. Please, use 'StringBuilder.h' instead.""
#endif	// if (!defined(GD_CORE_CONTAINERS_VECTOR))

#include <cstdarg>

GD_NAMESPACE_BEGIN

	template<typename CharType>
	inline BaseStringBuilder<CharType>& BaseStringBuilder<CharType>::Append(CharType const* const String, size_t const Length)
	{
		size_t const StartPos = this->MutableString.GetLength() - 1;
		this->MutableString.Resize(this->MutableString.GetLength() + Length);
		memcpy(&this->MutableString[StartPos], String, (Length + 1) * sizeof(String[0]));

		return (*this);
	}

	template<typename CharType>
	inline BaseStringBuilder<CharType>& BaseStringBuilder<CharType>::Append(CharType const* const String)
	{
		return this->Append(String, strlen(String));
	}

	template<typename CharType>
	inline BaseStringBuilder<CharType>& BaseStringBuilder<CharType>::Append(String const& TheString)
	{
		return this->Append(TheString.CStr(), TheString.GetLength());
	}

	template<typename CharType> template<size_t const Count>
	inline BaseStringBuilder<CharType>& BaseStringBuilder<CharType>::Append(CharType const (&Array)[Count])
	{
		return this->Append(&Array[0], Count);
	}

	template<typename CharType>
	inline BaseStringBuilder<CharType>& BaseStringBuilder<CharType>::AppendFormat(CharType const* const Format, ...)
	{
		va_list ArgumentsList = nullptr;
		va_start(ArgumentsList, Format);

		String const Formatted = BaseString<CharType>::FormatVa(Format, ArgumentsList);
		this->Append(Formatted);

		va_end(ArgumentsList);

		return (*this);
	}

	template<typename CharType>
	inline BaseStringBuilder<CharType>& BaseStringBuilder<CharType>::Append(CharType const Character)
	{
		*(this->MutableString.End() - 1) = Character;
		  this->MutableString.PushLast(CharType('\0'));
		return (*this);
	}

	template<typename CharType>
	GDINL String BaseStringBuilder<CharType>::ToString() const
	{
		return String(&this->MutableString[0]);
	}

GD_NAMESPACE_END
