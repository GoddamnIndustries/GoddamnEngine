/// ==========================================================================================
/// Float32x4Intrinsics.h: Common Float32x4 vector intrinsics.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// 
/// History:
///		* 07.06.2014  - Created by James Jhuighuy,
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_MATH_FLOAT32X4INTRINSICS_FLOAT32X4INTRINSICS
#define GD_CORE_MATH_FLOAT32X4INTRINSICS_FLOAT32X4INTRINSICS

#include <GoddamnEngine/Include.h>

#if (defined(GD_PLATFORM_DESKTOP) || defined(GD_PLATFORM_CONSOLE))	// We have SSE support
#	include <GoddamnEngine/Core/Math/Float32x4Intrinsics/Float32x4IntrinsicsSSE.inl>
#else
#	define GD_NO_FLOAT32X4INTRINSICS
#endif

#if (!defined(GD_NO_FLOAT32X4INTRINSICS))
GD_NAMESPACE_BEGIN

	/// Contains abstraction layer on Float32x4 vector intrinsics.
	namespace Float32x4Intrinsics
	{
#if (defined(GD_DOCUMENTATION))
		/// Platform vector Vector type.
		typedef some_intrinsic_type VectorRegisterType;

		/// Initializes new Vector with specified parameters.
		GDINT VectorRegisterType VectorMake(Float32 const CommonValue);
		GDINT VectorRegisterType VectorMake(Float32 const x, Float32 const y, Float32 const z, Float32 const w);

		/// Returns new register with all components assigned with specified component of a specified register.
		GDINT VectorRegisterType VectorReplicate(VectorRegisterType const Register, unsigned const ElementIndex);

		/// Creates a vector through selecting two components from each vector via a shuffle mask. 
		/// @param Register0 First source vector.
		/// @param Register1 Second source vector.
		/// @param x   Index for which component of first vector to use for X (literal 0-3)
		/// @param y   Index for which component to first vector to use for Y (literal 0-3)
		/// @param z   Index for which component to second vector to use for Z (literal 0-3)
		/// @param w   Index for which component to second vector to use for W (literal 0-3)
		GDINT VectorRegisterType VectorShuffle(VectorRegisterType const Register0, VectorRegisterType const Register1, unsigned const x, unsigned const y, unsigned const z, unsigned const w);

		/// Returns swizzling result of all components of a specified vector.
		GDINT VectorRegisterType VectorSwizzle(VectorRegisterType const Register, unsigned const x, unsigned const y, unsigned const z, unsigned const w);

		/// Selects elements from vector using control vector.
		GDINL VectorRegisterType VectorSelect(VectorRegisterType const First, VectorRegisterType const Second, VectorRegisterType const Control);

		/// Selects X from second vector and YZW from second vector.
		GDINL VectorRegisterType VectorMove(VectorRegisterType const First, VectorRegisterType const Second);

		/// Adds ('+') two vectors.
		GDINT VectorRegisterType VectorAdd(VectorRegisterType const First, VectorRegisterType const Second);
		/// Subtracts ('-') two vectors.
		GDINT VectorRegisterType VectorSub(VectorRegisterType const First, VectorRegisterType const Second);
		/// Multiplies ('*') two vectors.
		GDINT VectorRegisterType VectorMul(VectorRegisterType const First, VectorRegisterType const Second);
		/// Divides ('/') two vectors.
		GDINT VectorRegisterType VectorDiv(VectorRegisterType const First, VectorRegisterType const Second);
		/// Bitwise ands ('&') two vectors.
		GDINT VectorRegisterType VectorAnd(VectorRegisterType const First, VectorRegisterType const Second);
		/// Bitwise ors ('|') two vectors.
		GDINT VectorRegisterType VectorOr(VectorRegisterType const First, VectorRegisterType const Second);

		/// Multiplies first on seconds and add third vector to result.
		GDINT VectorRegisterType VectorMulAdd(VectorRegisterType const First, VectorRegisterType const Second, VectorRegisterType const Third)

		/// Returns two vectors DOT product.
		GDINT Float32 VectorDot4(VectorRegisterType const First, VectorRegisterType const Second);
		GDINT Float32 VectorDot3(VectorRegisterType const First, VectorRegisterType const Second);
		GDINT VectorRegisterType VectorDot4Vector(VectorRegisterType const First, VectorRegisterType const Second);
		GDINT VectorRegisterType VectorDot3Vector(VectorRegisterType const First, VectorRegisterType const Second);

		/// Negates vector.
		GDINT VectorRegisterType VectorNegate(VectorRegisterType const Vector);

		/// Returns two vectors CROSS product.
		GDINT VectorRegisterType VectorCross(VectorRegisterType const First, VectorRegisterType const Second);
		GDINT VectorRegisterType Vector3Cross(VectorRegisterType const First, VectorRegisterType const Second);

		GDINT Float32 VectorLength(VectorRegisterType const Vector);
		GDINT VectorRegisterType VectorNormalize(VectorRegisterType const Vector);
		GDINT VectorRegisterType Vector3Normalize(VectorRegisterType const Vector);

		/// Inverses specified matrix.
		/// @param Input  Initial matrix that requires to be inversed.
		/// @param Matrix Matrix to perform inversion on.
		GDINT void MatrixInverse(VectorRegisterType* const Matrix, VectorRegisterType const* const Input);
#endif	// if (defined(GD_DOCUMENTATION))

		GDINL Float32 static UInt32ToFloat32(UInt32 const UInt) { return (*reinterpret_cast<Float32 const*>(&UInt)); }
		GDINL Float32 static GetFloat32_0xFFFFFFFF() { return UInt32ToFloat32(0xFFFFFFFF); }
	
		GDINL VectorRegisterType const static VectorMake_0000() { return VectorMake(0.0f); }
		GDINL VectorRegisterType const static VectorMake_1000() { return VectorMake(GetFloat32_0xFFFFFFFF(), 0.0f, 0.0f, 0.0f); }
		GDINL VectorRegisterType const static VectorMake_0100() { return VectorMake(0.0f, GetFloat32_0xFFFFFFFF(), 0.0f, 0.0f); }
		GDINL VectorRegisterType const static VectorMake_1110() { return VectorMake(GetFloat32_0xFFFFFFFF(), GetFloat32_0xFFFFFFFF(), GetFloat32_0xFFFFFFFF(), 0.0f); }
	}	// namespace Float32x4Intrinsics

	GD_NAMESPACE_END
#endif	// if (!defined(GD_NO_FLOAT32X4INTRINSICS))

#if (!defined(GD_NO_FLOAT32X4INTRINSICS))
#	define GD_HAS_FLOAT32X4INTRINSICS 1
#endif	// if (!defined(GD_NO_FLOAT32X4INTRINSICS))

#endif
