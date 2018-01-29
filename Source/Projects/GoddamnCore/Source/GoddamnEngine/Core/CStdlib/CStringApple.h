// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
//
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/CStdlib/CStringApple.h
 * @note This file should be never directly included, please consider using <GoddamnEngine/Core/CStdlib/CString.h> instead.
 * Traits, helper functions and definitions for standard string/memory functions (Apple-specific).
 */
#pragma once
#if !defined(GD_INSIDE_CSTRING_H)
#    error This file should be never directly included, please consider using <GoddamnEngine/Core/CStdlib/CString.h> instead.
#endif    // if !defined(GD_INSIDE_CSTRING_H)

#include <iconv.h>

GD_NAMESPACE_BEGIN

    // **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
    //! Provides Microsoft-specific functions for C string.
    // **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
    class CStringApple final : public CStringGeneric
    {
    };  // class CStringApple

    // **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
    //! Provides cross-platform functions for C string.
    // **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
    using CString = CStringApple;

GD_NAMESPACE_END
