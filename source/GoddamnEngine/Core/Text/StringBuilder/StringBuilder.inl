//////////////////////////////////////////////////////////////////////////
/// StringBuilder.hh: Mutable string implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 16.06.2014 - Created by James Jhuighuy.
//////////////////////////////////////////////////////////////////////////

#if (!defined(GD_CORE_TEXT_STRINGBUILDER_STRINGBUILDER))
#	error "Attempting to include 'StringBuilder.inl' file. Please, use 'StringBuilder.hh' instead.""
#endif	// if (!defined(GD_CORE_CONTAINERS_VECTOR))

#include <cstdarg>

GD_NAMESPACE_BEGIN

	template<typename CharType>
	inline BaseStringBuilder<CharType>& BaseStringBuilder<CharType>::Append(CharType const* const String, size_t const Length)
	{
		size_t const StartPos = self->MutableString.GetSize() - 1;
		self->MutableString.Resize(self->MutableString.GetSize() + Length);
		memcpy(&self->MutableString[StartPos], String, (Length + 1) * sizeof(String[0]));

		return (*self);
	}

	template<typename CharType>
	inline BaseStringBuilder<CharType>& BaseStringBuilder<CharType>::Append(CharType const* const String)
	{
		return self->Append(String, strlen(String));
	}

	template<typename CharType>
	inline BaseStringBuilder<CharType>& BaseStringBuilder<CharType>::Append(String const& TheString)
	{
		return self->Append(TheString.CStr(), TheString.GetSize());
	}

	template<typename CharType> template<size_t const Count>
	inline BaseStringBuilder<CharType>& BaseStringBuilder<CharType>::Append(CharType const (&Array)[Count])
	{
		return self->Append(&Array[0], Count);
	}

	template<typename CharType>
	inline BaseStringBuilder<CharType>& BaseStringBuilder<CharType>::AppendFormat(CharType const* const Format, ...)
	{
		va_list ArgumentsList = va_list();
		va_start(ArgumentsList, Format);

		String const Formatted = BaseString<CharType>::FormatVa(Format, ArgumentsList);
		self->Append(Formatted);

		va_end(ArgumentsList);

		return (*self);
	}

	template<typename CharType>
	inline BaseStringBuilder<CharType>& BaseStringBuilder<CharType>::Append(CharType const Character)
	{
		*(self->MutableString.End() - 1) = Character;
		  self->MutableString.PushLast(CharType('\0'));
		return (*self);
	}

	template<typename CharType>
	GDINL String&& BaseStringBuilder<CharType>::ToString() const
	{
		return String(&self->MutableString[0]);
	}

GD_NAMESPACE_END
