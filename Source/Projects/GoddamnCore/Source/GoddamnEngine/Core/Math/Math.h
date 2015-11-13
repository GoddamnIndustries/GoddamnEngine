// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Math/Math.h
 * Mathematical functions.
 */
#pragma once

#include <GoddamnEngine/Include.h>

GD_NAMESPACE_BEGIN

	struct Math
	{

		// ------------------------------------------------------------------------------------------
		//! Returns absolute value of the parameter.
		//! @{
		GDINL static Float32 Abs(Float32 const value)
		{
			return static_cast<Float32>(::fabs(value));
		}
		GDINL static Int32 Abs(Int32 const value)
		{
			return abs(value);
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Returns sign value of the parameter - positive one, if the parameter is positive,
		//! negative one - if the parameter is negative, zero if is zero.
		//! @{
		GDINL static Float32 Sign(Float32 const value)
		{
			return value != 0.0f ? value / Abs(value) : 0.0f;
		}
		GDINL static Int32 Sign(Int32 const value)
		{
			return value != 0 ? value / Abs(value) : 0;
		}
		//! @}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Trigonometric functions.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Computes a sine of the specified radians angle.
		GDINL static Float32 Sin(Float32 const angleRadians)
		{
			return sinf(angleRadians);
		}

		// ------------------------------------------------------------------------------------------
		//! Computes the arc-sine value of the specified value in radians.
		GDINL static Float32 Asin(Float32 const sinValue)
		{
			return asinf(sinValue);
		}

		// ------------------------------------------------------------------------------------------
		//! Computes a cosine of the specified radians angle.
		GDINL static Float32 Cos(Float32 const angleRadians)
		{
			return cosf(angleRadians);
		}

		// ------------------------------------------------------------------------------------------
		//! Computes the arc-cosine value of the specified value in radians.
		GDINL static Float32 Acos(Float32 const cosValue)
		{
			return acosf(cosValue);
		}

		// ------------------------------------------------------------------------------------------
		//! Computes a tangent of the specified radians angle.
		GDINL static Float32 Tan(Float32 const angleRadians)
		{
			return tanf(angleRadians);
		}

		// ------------------------------------------------------------------------------------------
		//! Computes the arc-tangent value of the specified value in radians.
		GDINL static Float32 Atan(Float32 const tanValue)
		{
			return atanf(tanValue);
		}

		// ------------------------------------------------------------------------------------------
		//! Computes the arc-tangent value of the value, specified Y/X.
		GDINL static Float32 Atan2(Float32 const y, Float32 const x)
		{
			return atan2f(y, x);
		}

		// ------------------------------------------------------------------------------------------
		//! Computes a cotangent of the specified radians angle.
		GDINL static Float32 Cot(Float32 const angleRadians)
		{
			return 1.0f / Tan(angleRadians);
		}

		// ------------------------------------------------------------------------------------------
		//! Computes the arc-cotangent value of the specified value in radians.
		GDINL static Float32 Acot(Float32 const cotValue)
		{
			return Atan(1.0f / cotValue);
		}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Exponential functions.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		GDINL static Float32 Sqrt(Float32 const value)
		{
			return sqrtf(value);
		}

		GDINL static Float32 Pow(Float32 const value, Float32 const exp)
		{
			return powf(value, exp);
		}

	};	// struct Math

GD_NAMESPACE_END
