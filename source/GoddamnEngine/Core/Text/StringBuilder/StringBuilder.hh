//////////////////////////////////////////////////////////////////////////
/// StringBuilder.hh: Mutable string interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
///
/// History:
///		* 15.06.2014 - Created by James Jhuighuy.
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_CORE_TEXT_STRING_BUILDER_STRING_BUILDER
#define GD_CORE_TEXT_STRING_BUILDER_STRING_BUILDER

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Text/String/String.h>
#include <GoddamnEngine/Core/Containters/Vector/Vector.h>

GD_NAMESPACE_BEGIN

    /// Represents mutable charcters sequence.
    template<typename CharacterType>
    class BaseStringBuilder final
    {
    private:
        Vector<CharacterType> MutableString;
        
    public:
        GDINL size_t GetCapacity() const { return self->MutableString.GetCapacity(); }
        GDINL size_t GetSize() const { return self->MutableString.GetSize(); }
        
        GDINL BaseStringBuilder& Append(String const& SomeString);
        GDINL BaseStringBuilder& Append(CharacterType const* const Format, ...);
        
        GDINL BaseString<CharacterType>&& ToString() const;
    };  // class StringBuilder

    typedef BaseStringBuilder<Char> StringBuilder;

GD_NAMESPACE_END

#endif
