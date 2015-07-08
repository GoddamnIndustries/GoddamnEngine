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

// ------------------------------------------------------------------------------------------
//! Defines a type-independent boolean constants. 
//! @note Please, consider using native 'true' and 'false' for native bool type.
//! @{
#define GD_TRUE								1	
#define GD_FALSE							0
//! @}

// ------------------------------------------------------------------------------------------
// Some code below should be stripped be the resource compiler.
#if defined(RC_INVOKED)
#	define GD_RESOURCE_COMPILER				GD_TRUE
#else
#	define GD_RESOURCE_COMPILER				GD_FALSE
#endif	// if (!defined(RC_INVOKED))

#if (!defined(GD_DOCUMENTATION))
#	define GD_DOCUMENTATION					GD_FALSE
#endif	// if (!defined(GD_DOCUMENTATION))

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// C++ syntax "improvements" & useful macros.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#if !GD_RESOURCE_COMPILER

// ------------------------------------------------------------------------------------------
//! Use this keyword at the end of @c switch branches that do not require @c break statement.
//! @{
#if defined(fallthrough)
#	error The 'fallthrough' macro, required to build GoddamnEngine is already used.
#endif	// if defined(fallthrough)
#define fallthrough /*fallthrough*/		
//! @}

// ------------------------------------------------------------------------------------------
//! Use this keyword to declare pure virtual methods.
//! @{
#if defined(abstract)
#	error The 'abstract' macro, required to build GoddamnEngine is already used.
#endif	// if defined(abstract)
#define abstract = 0					
//! @}

// ------------------------------------------------------------------------------------------
//! Used to hack the Visual C++ compiler's preprocessor at some points of macro expansion.
#define GD_PASS(...)					__VA_ARGS__

// ------------------------------------------------------------------------------------------
//! Generated an compile-time expression that forces the compiler to suppress the 
//! 'unused variable' warning.
//! @param Parameter Name of the unused variable.
//! @{
#define GD_NOT_USED(Parameter)			static_cast<void>(Parameter)
#define GD_NOT_USED_L(...)				static_cast<void>(__VA_ARGS__)
//! @}

// ------------------------------------------------------------------------------------------
//! Temporary macro that converts specified expression to the Int32 type.
//! @todo Get rid of this macro.
#define GD_TRUNC_TO_INT32(Float32V)		static_cast<Int32>(Float32V)

// ------------------------------------------------------------------------------------------
//! Generates a compile-time constant with all bits set to zero instead of the specified one.
//! @param Bit The specified bit index.
#define GD_BIT(Bit)						(1 << (Bit))

// ------------------------------------------------------------------------------------------
//! Some 'stringification' magic.
//! @param Expression The expression that would be converted to string literal.
//! @{
#define GD_STRIGIFY(Expression)			GD_STRIGIFY_(Expression)
#if !GD_DOCUMENTATION
#	define GD_STRIGIFY_(Expression)		#Expression
#endif	// if !GD_DOCUMENTATION
//! @}

// ------------------------------------------------------------------------------------------
//! Some glue magic.
//! @param LHS Left handed expression to be glued.
//! @param RHS Right handed expression to be glued.
//! @{
#define GD_GLUE(LHS, RHS)				GD_GLUE_(LHS, RHS)
#if !GD_DOCUMENTATION
#	define GD_GLUE_(LHS, RHS)			LHS ## RHS
#endif	// if !GD_DOCUMENTATION
//! @}

// ------------------------------------------------------------------------------------------
//! Some widening magic.
//! @param Literal String or character literal to be converted to wide one.
//! @{
#define GD_WIDEN(Literal)				GD_WIDEN_(Literal)
#if !GD_DOCUMENTATION
#	define GD_WIDEN_(Literal)			L ## Literal
#endif	// if !GD_DOCUMENTATION
//! @}

#endif	// if !GD_RESOURCE_COMPILER

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Configuration definitions.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <GoddamnEngine/Core/Base/Version.h>
#if !GD_RESOURCE_COMPILER
#	include <GoddamnEngine/Core/Base/Architecture.h>
#	include <GoddamnEngine/Core/Base/Types.h>
#	include <GoddamnEngine/Core/Base/Allocator.h>
#	include <GoddamnEngine/Core/Base/Assert.h>
#	include <GoddamnEngine/Core/Base/CMemory.h>
#	include <GoddamnEngine/Core/Base/CChar.h>
#	include <GoddamnEngine/Core/Base/CString.h>
#endif	// if !GD_RESOURCE_COMPILER
#undef GD_INSIDE_INCLUDE_H
