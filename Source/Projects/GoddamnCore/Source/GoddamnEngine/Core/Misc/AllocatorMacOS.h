// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
//
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Misc/AllocatorMacOS.h
 * File contains Windows allocator interface.
 */
#pragma once
#if !defined(GD_INSIDE_ALLOCATOR_H)
#    error This file should be never directly included, please consider using <GoddamnEngine/Core/Misc/Allocator.h> instead.
#endif    // if !defined(GD_INSIDE_ALLOCATOR_H)

GD_NAMESPACE_BEGIN

    // **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
    //! Cross-platform allocator interface.
    // **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
    using PlatformAllocator = PlatformAllocatorGeneric;

GD_NAMESPACE_END
