// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/CStdlib/CStdlib.h
 * @note This file should be never directly included, please consider using <GoddamnEngine/Include.h> instead.
 * Wrappers, helper functions and definitions for standard library functions.
 */
#pragma once
#if !defined(GD_INSIDE_INCLUDE_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Include.h> instead.
#endif	// if !defined(GD_INSIDE_INCLUDE_H)

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Provides wrappers, helper functions and definitions for standard library functions.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class CStdlib final : public TNonCreatable
	{
	public:
		// ... Pseudo-random sequence generation ...

		/*!
		 * @see @c "std::rand" function.
		 */
		GDINL static Int32 Rand()
		{
			return static_cast<Int32>(::rand());
		}

		/*!
		 * @see @c "std::rand" function.
		 */
		GDINL static void SRand(UInt32 const seed)
		{
			srand(static_cast<unsigned int>(seed));
		}

		// ... Dynamic memory management ...


	};	// class CStdlib

GD_NAMESPACE_END
