// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Include.h
 * File contains GoddamnEngine environmental definitions. It should be directly included at the top of each header and source file.
 */
#pragma once
#define GD_INSIDE_INCLUDE_H

/*!
 * Defines a type-independent boolean constants. 
 * Please, consider using native 'true' and 'false' for native bool type.
 */
//! @{
#define GD_TRUE								1	
#define GD_FALSE							0
//! @}

/*
 * Some code below should be stripped be the resource compiler.
 */
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

/*!
 * Use this keyword at the end of @c switch branches that do not require @c break statement.
 */
//! @{
#if defined(fallthrough)
#	error The 'fallthrough' macro, required to build GoddamnEngine is already used.
#endif	// if defined(fallthrough)
#define fallthrough /*fallthrough*/		
//! @}

//! @{
#if defined(implicit)
#	error The 'implicit' macro, required to build GoddamnEngine is already used.
#endif	// if defined(implicit)
#define implicit
//! @}

/*!
 * Use this keyword to pass empty macro arguments.
 */
#define GD_EMPTY

/*!
 * Use this keyword to declare pure virtual methods.
 */
#define GD_PURE_VIRTUAL = 0					

/*!
 * Used to hack the Visual C++ compiler's preprocessor at some points of macro expansion.
 */
#define GD_PASS(...)					__VA_ARGS__

/*!
 * Generated an compile-time Expression that forces the compiler to suppress the 'unused variable' warning.
 * @param Parameter Name of the unused variable.
 */
//! @{
#define GD_NOT_USED(Parameter)            NotUsed(Parameter)                /*NOLINT*/
#define GD_NOT_USED_L(...)                NotUsed(__VA_ARGS__)            /*NOLINT*/
#define GD_NOT_USED_CONSTEXPR(...)      NotUsedConstexpr(__VA_ARGS__)   /*NOLINT*/
//! @}

template<typename... T>
static void NotUsed(T const&...) {}
template<typename... T>
constexpr static void NotUsedConstexpr(T...) {}

/*!
 * Generates a compile-time constant with all bits set to zero instead of the specified one.
 * @param Bit The specified bit index.
 */
#define GD_BIT(Bit)						(static_cast<decltype(Bit)>(1) << (Bit))

/*!
 * Some 'stringification' magic.
 * @param Expression The Expression that would be converted to string literal.
 */
//! @{
#define GD_STRIGIFY(Expression)			GD_STRIGIFY_(Expression)
#if !GD_DOCUMENTATION
#	define GD_STRIGIFY_(Expression)		#Expression
#endif	// if !GD_DOCUMENTATION
//! @}

/*!
 * Some glue magic.
 *
 * @param lhs Left handed Expression to be glued.
 * @param rhs Right handed Expression to be glued.
 */
//! @{
#define GD_GLUE(lhs, rhs)				GD_GLUE_(lhs, rhs)
#if !GD_DOCUMENTATION
#	define GD_GLUE_(lhs, rhs)			lhs ## rhs
#endif	// if !GD_DOCUMENTATION
//! @}

/*!
 * Some widening magic.
 * @param literal String or character literal to be converted to wide one.
 */
//! @{
#define GD_WIDEN(literal)				GD_WIDEN_(literal)
#if !GD_DOCUMENTATION
#	define GD_WIDEN_(literal)			L ## literal
#endif	// if !GD_DOCUMENTATION
//! @}

#endif	// if !GD_RESOURCE_COMPILER

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Configuration definitions.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define GD_PLATFORM_KERNEL
#define GD_PLATFORM_WRAPPER

#define GD_TESTING_ENABLED 1

#include <GoddamnEngine/Core/Base/Version.h>
#if !GD_RESOURCE_COMPILER
#	if GD_TESTING_ENABLED
#		include <GoddamnEngine/Core/Testing/Testing.h>
#		include <GoddamnEngine/Core/Testing/Testing2.h>
#	endif	// if GD_TESTING_ENABLED
#	include <GoddamnEngine/Core/Base/Architecture.h>
#	include <GoddamnEngine/Core/Base/Types.h>
#	include <GoddamnEngine/Core/Platform/PlatformAssert.h>
#	include <GoddamnEngine/Core/Platform/PlatformAllocator.h>
#endif	// if !GD_RESOURCE_COMPILER

#undef GD_INSIDE_INCLUDE_H
