// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Include.h
//! File contains GoddamnEngine environmental definitions. It should be directly included
//! at the top of each header and source file.
#pragma once
#define GD_INSIDE_INCLUDE_H

//! Defines a type-independent boolean constants. 
//! @note Please, consider using native 'true' and 'false' for native bool type.
//! @{
#define GD_TRUE								1	
#define GD_FALSE							0
//! @}

// Some code below should be stripped be the resource compiler.
#if defined(RC_INVOKED)
#	define GD_RESOURCE_COMPILER				GD_TRUE
#else
#	define GD_RESOURCE_COMPILER				GD_FALSE
#endif	// if (!defined(RC_INVOKED))

#if (!defined(GD_DOCUMENTATION))
#	define GD_DOCUMENTATION					GD_FALSE
#endif	// if (!defined(GD_DOCUMENTATION))

// ------------------------------------------------------------------------------------------
// Version and Namespace definitions.
// ------------------------------------------------------------------------------------------
#include <GoddamnEngine/Core/Definitions/Version.h>

// ------------------------------------------------------------------------------------------
// Configuration definitions.
// ------------------------------------------------------------------------------------------
#if !GD_RESOURCE_COMPILER
#	include <GoddamnEngine/Core/Definitions/Architecture.h>
#	include <GoddamnEngine/Core/Definitions/Types.h>
#	include <GoddamnEngine/Core/Definitions/Allocator.h>
#	include <GoddamnEngine/Core/Definitions/Assert.h>
#endif	// if !GD_RESOURCE_COMPILER

// ------------------------------------------------------------------------------------------
// C++ syntax "improvements" & useful macros.
// ------------------------------------------------------------------------------------------
#if !GD_RESOURCE_COMPILER
#	define fallthrough						/*fallthrough*/
#	define abstract							= 0
#	define object_cast						static_cast

#	define GD_NOT_USED(Argument)			(static_cast<void>(Argument))

#	define GD_PASS(...)						__VA_ARGS__

//! @todo Get rid of this macro
#	define GD_TRUNC_TO_INT32(Float32V)		static_cast<Int32>(Float32V)

//! Some 'stringification' magic.
//! @{
#	define GD_STRIGIFY_(A)					#A
#	define GD_STRIGIFY(A)					GD_STRIGIFY_(A)
//! @}

//! Some glue magic.
//! @{
#	define GD_GLUE_(A, B)					A ## B
#	define GD_GLUE(A, B)					GD_GLUE_(A, B)
//! @}

//! Some widening magic.
//! @{
#	define GD_WIDEN_(String)				L ## String
#	define GD_WIDEN(String)					GD_WIDEN_(String)
//! @}

#	define GD_BIT(Bit)						(1 << (Bit))

#endif	// if !GD_RESOURCE_COMPILER
#undef GD_INSIDE_INCLUDE_H
