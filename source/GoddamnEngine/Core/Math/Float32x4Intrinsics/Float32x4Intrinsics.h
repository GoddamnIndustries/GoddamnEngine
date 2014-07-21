/// ==========================================================================================
/// Float32x4Intrinsics.h: Common Float32x4 vector intrinsics.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 07.06.2014  - Created by James Jhuighuy,
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_MATH_FLOAT32X4INTRINSICS_FLOAT32X4INTRINSICS
#define GD_CORE_MATH_FLOAT32X4INTRINSICS_FLOAT32X4INTRINSICS

#include <GoddamnEngine/Include.h>

#  if (defined(GD_PLATFORM_DESKTOP) || defined(GD_PLATFORM_CONSOLE))	// We have SSE support
#	include <GoddamnEngine/Core/Math/Float32x4Intrinsics/Float32x4IntrinsicsSSE.inl>
#elif (defined(GD_PLATFORM_MOBILE))
#	include <GoddamnEngine/Core/Math/Float32x4Intrinsics/Float32x4IntrinsicsNeon.inl>
#else
#	define GD_NO_FLOAT32X4INTRINSICS 1
#	if (defined(GD_DOCUMENTATION))

GD_NAMESPACE_BEGIN

	/// Contains abstraction layer on Float32x4 vector intrinsics.
	namespace Float32x4Intrinsics
	{
		/// Platform vector Vector type.
		typedef some_intrinsic_type VectorRegisterType;

		/// Initializes new Vector with sepcified parameters.
		GDINT VectorRegisterType VectorMake(Float32 const CommonValue);
		GDINT VectorRegisterType VectorMake(Float32 const x, Float32 const y, Float32 const z, Float32 const w);

		/// Returns new register with all components assigned with specified component of a specified register.
		GDINT VectorRegisterType VectorReplicate(VectorRegisterType const Register, unsigned const ElementIndex);

		/// Creates a vector through selecting two components from each vector via a shuffle mask. 
		/// @param Register0 First source vector.
		/// @param Register1 Second source vector.
		/// @param x         Index for which component of first vector to use for X (literal 0-3)
		/// @param y         Index for which component to first vector to use for Y (literal 0-3)
		/// @param z         Index for which component to second vector to use for Z (literal 0-3)
		/// @param w         Index for which component to second vector to use for W (literal 0-3)
		GDINT VectorRegisterType VectorShuffle(VectorRegisterType const Register0, VectorRegisterType const Register1, unsigned const x, unsigned const y, unsigned const z, unsigned const w);

		/// Returns swizzling result of all components of a specified vector.
		GDINT VectorRegisterType VectorSwizzle(VectorRegisterType const Register, unsigned const x, unsigned const y, unsigned const z, unsigned const w);

		/// Adds ('+') two vectors.
		GDINT VectorRegisterType VectorAdd(VectorRegisterType const First, VectorRegisterType const Second);
		/// Subtracts ('-') two vectors.
		GDINT VectorRegisterType VectorSub(VectorRegisterType const First, VectorRegisterType const Second);
		/// Multiplies ('*') two vectors.
		GDINT VectorRegisterType VectorMul(VectorRegisterType const First, VectorRegisterType const Second);
		/// Divides ('/') two vectors.
		GDINT VectorRegisterType VectorDiv(VectorRegisterType const First, VectorRegisterType const Second);
		
		/// Multiplies first on seconds and add third vector to result.
		GDINT VectorRegisterType VectorMulAdd(VectorRegisterType const First, VectorRegisterType const Second, VectorRegisterType const Third)

		/// Returns two vectors DOT product.
		GDINT Float32 VectorDot4(VectorRegisterType const First, VectorRegisterType const Second);
		GDINT Float32 VectorDot3(VectorRegisterType const First, VectorRegisterType const Second);

		/// Returns two vectors CROSS product.
		GDINT VectorRegisterType VectorCross(VectorRegisterType const First, VectorRegisterType const Second);

		GDINT Float32 VectorLength(VectorRegisterType const Vector);
		GDINT VectorRegisterType VectorNormalize(VectorRegisterType const Vector);

		/// Inverses specified matrix.
		/// @param Input  Initial matrix that requires to be inversed.
		/// @param Matrix Matrix to perform inversion on.
		GDINT void MatrixInverse(VectorRegisterType* const Matrix, VectorRegisterType const* const Input)
	}	// namespace Float32x4Intrinsics

GD_NAMESPACE_END

#	endif	// if (defined(GD_DOCUMENTATION))
#endif

#if (!defined(GD_NO_FLOAT32X4INTRINSICS))
#	define GD_HAS_FLOAT32X4INTRINSICS 1
#endif	// if (!defined(GD_NO_FLOAT32X4INTRINSICS))

#endif
