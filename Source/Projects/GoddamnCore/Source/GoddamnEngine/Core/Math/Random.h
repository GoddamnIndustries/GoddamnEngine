// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Math/Random.h
 * File contains pseudo-random numbers generating algorithms.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Misc/Endian.h>

GD_NAMESPACE_BEGIN

	// **------------------------------------------------------------------------------------------**
	//! Template wrapper for random generating algorithms.
	// **------------------------------------------------------------------------------------------**
	template<typename TImplementation>
	class TRandomBase : public TNonCopyable
	{
	public:

		/*!
		 * Generates the pseudo-random unsigned 16-bit integer using specified random algorithm.
		 * @returns Unsigned 16-bit integer in range [0; UInt16Max].
		 */
		GDINL UInt16 GenerateUnsigned16()
		{
			return static_cast<UInt16>(GenerateUnsigned32());
		}

		/*!
		 * Generates the pseudo-random signed 16-bit integer using specified random algorithm.
		 * @returns Signed 16-bit integer in range [Int16Min; Int16Max].
		 */
		GDINL Int16 GenerateSigned16()
		{
			return union_cast<Int16>(GenerateUnsigned16());
		}

		/*!
		 * Generates the pseudo-random unsigned 32-bit integer using specified random algorithm.
		 * @returns Unsigned 32-bit integer in range [0; UInt32Max].
		 */
		GDINL UInt32 GenerateUnsigned32()
		{
			return static_cast<TImplementation*>(this)->GenerateUnsigned32Impl();
		}

		/*!
		 * Generates the pseudo-random signed 32-bit integer using specified random algorithm.
		 * @returns Signed 32-bit integer in range [Int32Min; Int32Max].
		 */
		GDINL Int32 GenerateSigned32()
		{
			return union_cast<Int32>(GenerateUnsigned32());
		}

		/*!
		 * Generates the pseudo-random unsigned 64-bit integer using specified random algorithm.
		 * @returns Unsigned 64-bit integer in range [0; UInt64Max].
		 */
		GDINL UInt64 GenerateUnsigned64()
		{
			return static_cast<UInt64>(GenerateUnsigned32()) | static_cast<UInt64>(GenerateUnsigned32()) << 32;
		}

		/*!
		 * Generates the pseudo-random signed 64-bit integer using specified random algorithm.
		 * @returns Signed 64-bit integer in range [Int64Min; Int64Max].
		 */
		GDINL Int64 GenerateSigned64()
		{
			return union_cast<Int64>(GenerateUnsigned64());
		}

		/*!
		 * Generates the pseudo-random 32-bit floating-point value using specified random algorithm.
		 * @returns Floating-point value in range [0; 1].
		 */
		GDINL Float32 Generate32()
		{
			return static_cast<Float32>(GenerateUnsigned32()) / static_cast<Float32>(UInt32Max);
		}

		/*!
		 * Generates the pseudo-random 64-bit floating-point value using specified random algorithm.
		 * @returns Floating-point value in range [0; 1].
		 */
		GDINL Float32 Generate64()
		{
			return static_cast<Float32>(GenerateUnsigned64()) / static_cast<Float32>(UInt64Max);
		}

	};	// class TRandomBase

	// **------------------------------------------------------------------------------------------**
	//! Pseudo-random number generator using LGC random algorithm with varying coefficients.
	// **------------------------------------------------------------------------------------------**
	class LGCRandom final : public TRandomBase<LGCRandom>
	{
	private:
		UInt32 m_Seed;

	public:

		/*!
		 * Initializes a new LGC random generator with varying coefficients.
		 * @param seed Initial seed value.
		 */
		GDAPI explicit LGCRandom(UInt32 const seed = 0);

	private:
		friend class TRandomBase<LGCRandom>;
		GDAPI UInt32 GenerateUnsigned32Impl();
	};	// class LGCRandom

	/*!
	 * Global pseudo-random generator.
	 */
	GDAPI extern LGCRandom GlobalPseudoRandom;
		
	typedef LGCRandom TrueRandom;

GD_NAMESPACE_END
