/// ==========================================================================================
/// Misc.h: miscellaneous math functions.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_MATH_MISC
#define GD_CORE_MATH_MISC

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/TypeTraits.h>
#include <GoddamnEngine/Core/Diagnostics/Assertion/Assertion.h>

GD_NAMESPACE_BEGIN

	/// Contains miscellaneous math functions.
	namespace MathMisc
	{
		template<typename ElementType>
		inline void ScalarSinCos(ElementType& Sin, ElementType& Cos, ElementType const Value)
		{	
			static_assert(TypeTraits::IsFloatingPoint<ElementType>::Value, "'MathMisc::ScalarSinCos<T>' error: T should be floating-point scalar.");
			
			// Map Value to Y in [-pi,pi], x = 2*pi*quotient + remainder.
			ElementType Quotient = ElementType(0.5f / M_PI) * Value;
			if (Value >= 0.0f) {
				Quotient = ElementType(static_cast<int>(Quotient + ElementType(0.5f)));
			} else {
				Quotient = ElementType(static_cast<int>(Quotient - ElementType(0.5f)));
			}

			ElementType Y = Value - ElementType(2.0f * M_PI) * Quotient;

			// Map Y to [-pi/2,pi/2] with sin(Y) = sin(Value).
			ElementType Sign;
			if (Y > ElementType(M_PI * 0.5f)) {
				Y = ElementType(M_PI) - Y;
				Sign = ElementType(- 1.0f);
			} else if (Y < -ElementType(M_PI * 0.5f)) {
				Y = -ElementType(M_PI) - Y;
				Sign = ElementType(-1.0f);
			} else {
				Sign = ElementType(+1.0f);
			}


			// 11-degree minimax approximation
			ElementType const Y2 = Y * Y;
			Sin = (((((ElementType(-2.3889859e-08f) * Y2 + ElementType(2.7525562e-06f)) * Y2 - ElementType(0.00019840874f)) * Y2 + ElementType(0.0083333310f)) * Y2 - ElementType(0.16666667f)) * Y2 + ElementType(1.0f)) * Y;

			// 10-degree minimax approximation
			ElementType const P = ((((-2.6051615e-07f * Y2 + 2.4760495e-05f) * Y2 - 0.0013888378f) * Y2 + 0.041666638f) * Y2 - 0.5f) * Y2 + 1.0f;
			Cos = Sign * P;
		}
	}	// namespace MathMisc

GD_NAMESPACE_END

#endif
