//////////////////////////////////////////////////////////////////////////
/// Float32x4IntrinsicsSSE.h: x86 SSE4 Float32x4 vector intrinsics.
/// Copyright (C) $(GD_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 07.06.2014  - Created by James Jhuighuy,
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_CORE_MATH_FLOAT32X4INTRINSICS_FLOAT32X4INTRINSICS_SSE
#define GD_CORE_MATH_FLOAT32X4INTRINSICS_FLOAT32X4INTRINSICS_SSE

#include <smmintrin.h>

#define GD_FLOAT32X4INTRINSICS_SSE_SHUFFLEMASK(A0, A1, B2, B3) _MM_SHUFFLE(B3, B2, A1, A0)

GD_NAMESPACE_BEGIN

	namespace Float32x4Intrinsics
	{
		typedef __m128 VectorRegisterType;

		/// ~~~ VECTOR CODE ~~~

		GDINL VectorRegisterType VectorMake(Float32 const CommonValue)
		{
			return _mm_set1_ps(CommonValue);
		}

		// Dirty hack to provide constexpr 'ElementIndex' param.
#define VectorReplicate(Register, ElementIndex) _VectorReplicate<ElementIndex>(Register)
		template<unsigned const ElementIndex>
		GDINL VectorRegisterType _VectorReplicate(VectorRegisterType const Register)
		{
			enum : unsigned { ShuffleMask = GD_FLOAT32X4INTRINSICS_SSE_SHUFFLEMASK(ElementIndex, ElementIndex, ElementIndex, ElementIndex) };
			return _mm_shuffle_ps(Register, Register, ShuffleMask);
		}

#define VectorShuffle(Register0, Register1, x, y, z, w)	_VectorShuffle<x, y, z, w>(Register0, Register1)
		template<unsigned const x, unsigned const y, unsigned const z, unsigned const w>
		GDINL VectorRegisterType _VectorShuffle(VectorRegisterType const Register0, VectorRegisterType const Register1)
		{
			enum : unsigned { ShuffleMask = GD_FLOAT32X4INTRINSICS_SSE_SHUFFLEMASK(x, y, z, w) };
			return _mm_shuffle_ps(Register0, Register1, ShuffleMask);
		}

		// Dirty hack to provide constexpr 'x..w' params.
#define VectorSwizzle(Register, x, y, z, w) _VectorSwizzle<x, y, z, w>(Register)
		template<unsigned const x, unsigned const y, unsigned const z, unsigned const w>
		GDINL VectorRegisterType _VectorSwizzle(VectorRegisterType const Register)
		{
			enum : unsigned { ShuffleMask = GD_FLOAT32X4INTRINSICS_SSE_SHUFFLEMASK(x, y, z, w) };
			return _mm_shuffle_ps(Register, Register, ShuffleMask);
		}

		GDINL VectorRegisterType VectorMake(Float32 const x, Float32 const y, Float32 const z, Float32 const w)
		{
			return _mm_setr_ps(x, y, z, w);
		}

		GDINL VectorRegisterType VectorAdd(VectorRegisterType const First, VectorRegisterType const Second)
		{
			return _mm_add_ps(First, Second);
		}

		GDINL VectorRegisterType VectorSub(VectorRegisterType const First, VectorRegisterType const Second)
		{
			return _mm_sub_ps(First, Second);
		}

		GDINL VectorRegisterType VectorMul(VectorRegisterType const First, VectorRegisterType const Second)
		{
			return _mm_mul_ps(First, Second);
		}

		GDINL VectorRegisterType VectorDiv(VectorRegisterType const First, VectorRegisterType const Second)
		{
			return _mm_div_ps(First, Second);
		}

		GDINL VectorRegisterType VectorMulAdd(VectorRegisterType const First, VectorRegisterType const Second, VectorRegisterType const Third)
		{
			return _mm_add_ps(_mm_mul_ps(First, Second), Third);
		}

		inline Float32 VectorLength(VectorRegisterType const Vector)
		{
			__m128 const Temp1 = _mm_dp_ps(Vector, Vector, 0xFF);
			__m128 const Temp2 = _mm_sqrt_ss(Temp1);
			return _mm_cvtss_f32(Temp2);
		}

		inline VectorRegisterType VectorNormalize(VectorRegisterType const Vector)
		{
			__m128 const Temp1 = _mm_dp_ps(Vector, Vector, 0xFF);
			__m128 const Temp2 = _mm_rsqrt_ps(Temp1);
			return _mm_mul_ps(Vector, Temp2);
		}

		inline Float32 VectorDot4(VectorRegisterType const First, VectorRegisterType const Second)
		{
			__m128 const Temp1 = _mm_dp_ps(First, Second, 0xFF);
			return _mm_cvtss_f32(Temp1);
		}

		inline Float32 VectorDot3(VectorRegisterType const First, VectorRegisterType const Second)
		{
			enum : unsigned { ShuffleMask = GD_FLOAT32X4INTRINSICS_SSE_SHUFFLEMASK(1, 2, 3, 0) };
			__m128 const Temp1 = _mm_dp_ps(First, Second, ShuffleMask);
			return _mm_cvtss_f32(Temp1);
		}

		inline VectorRegisterType VectorCross(VectorRegisterType const First, VectorRegisterType const Second)
		{
			__m128 const  FirstYZXW = _mm_shuffle_ps(First,  First,  GD_FLOAT32X4INTRINSICS_SSE_SHUFFLEMASK(1, 2, 0, 3));
			__m128 const  FirstZXYW = _mm_shuffle_ps(First,  First,  GD_FLOAT32X4INTRINSICS_SSE_SHUFFLEMASK(2, 0, 1, 3));
			__m128 const SecondZXYW = _mm_shuffle_ps(Second, Second, GD_FLOAT32X4INTRINSICS_SSE_SHUFFLEMASK(2, 0, 1, 3));
			__m128 const SecondYZXW = _mm_shuffle_ps(Second, Second, GD_FLOAT32X4INTRINSICS_SSE_SHUFFLEMASK(1, 2, 0, 3));
			return _mm_sub_ps(_mm_mul_ps(FirstYZXW, SecondZXYW), _mm_mul_ps(FirstZXYW, SecondYZXW));
		}

		inline void MatrixInverse(VectorRegisterType* const _Matrix, VectorRegisterType const* const Input)
		{
			// Code was taken from http://download.intel.com/design/PentiumIII/sml/24504301.pdf
			// Modifications:
			// * C-style cast changed to C++-style cont-correct casts,
			// * Variables renamed to match coding standarts, 
			// * Unwrapped shuffle masks.
			
			Float32* const Matrix = &_Matrix->m128_f32[0];
			__m128 Minor0 = _mm_setzero_ps(), Minor1 = _mm_setzero_ps(), Minor2 = _mm_setzero_ps(), Minor3 = _mm_setzero_ps();
			__m128   Row0 = _mm_setzero_ps(),   Row1 = _mm_setzero_ps(),   Row2 = _mm_setzero_ps(),   Row3 = _mm_setzero_ps();
			__m128 Determinant = _mm_setzero_ps(), Temp = _mm_setzero_ps();

			Temp = _mm_loadh_pi(_mm_loadl_pi(Temp, reinterpret_cast<__m64 const*>(Input    )), reinterpret_cast<__m64 const*>(Input + 4 ));
			Row1 = _mm_loadh_pi(_mm_loadl_pi(Row1, reinterpret_cast<__m64 const*>(Input + 8)), reinterpret_cast<__m64 const*>(Input + 12));
			Row0 = _mm_shuffle_ps(Temp, Row1, GD_FLOAT32X4INTRINSICS_SSE_SHUFFLEMASK(0, 2, 0, 2));
			Row1 = _mm_shuffle_ps(Row1, Temp, GD_FLOAT32X4INTRINSICS_SSE_SHUFFLEMASK(1, 3, 1, 3));
			Temp = _mm_loadh_pi(_mm_loadl_pi(Temp, reinterpret_cast<__m64 const*>(Input + 2 )), reinterpret_cast<__m64 const*>(Input + 6 ));
			Row3 = _mm_loadh_pi(_mm_loadl_pi(Row3, reinterpret_cast<__m64 const*>(Input + 10)), reinterpret_cast<__m64 const*>(Input + 14));
			Row2 = _mm_shuffle_ps(Temp, Row3, GD_FLOAT32X4INTRINSICS_SSE_SHUFFLEMASK(0, 2, 0, 2));
			Row3 = _mm_shuffle_ps(Row3, Temp, GD_FLOAT32X4INTRINSICS_SSE_SHUFFLEMASK(1, 3, 1, 3));
			// -----------------------------------------------
			Temp = _mm_mul_ps(Row2, Row3);
			Temp = _mm_shuffle_ps(Temp, Temp, GD_FLOAT32X4INTRINSICS_SSE_SHUFFLEMASK(1, 0, 3, 2));
			Minor0 = _mm_mul_ps(Row1, Temp);
			Minor1 = _mm_mul_ps(Row0, Temp);
			Temp = _mm_shuffle_ps(Temp, Temp, GD_FLOAT32X4INTRINSICS_SSE_SHUFFLEMASK(2, 3, 0, 1));
			Minor0 = _mm_sub_ps(_mm_mul_ps(Row1, Temp), Minor0);
			Minor1 = _mm_sub_ps(_mm_mul_ps(Row0, Temp), Minor1);
			Minor1 = _mm_shuffle_ps(Minor1, Minor1, GD_FLOAT32X4INTRINSICS_SSE_SHUFFLEMASK(2, 3, 0, 1));
			// -----------------------------------------------
			Temp = _mm_mul_ps(Row1, Row2);
			Temp = _mm_shuffle_ps(Temp, Temp, GD_FLOAT32X4INTRINSICS_SSE_SHUFFLEMASK(1, 0, 3, 2));
			Minor0 = _mm_add_ps(_mm_mul_ps(Row3, Temp), Minor0);
			Minor3 = _mm_mul_ps(Row0, Temp);
			Temp = _mm_shuffle_ps(Temp, Temp, GD_FLOAT32X4INTRINSICS_SSE_SHUFFLEMASK(2, 3, 0, 1));

			Minor0 = _mm_sub_ps(Minor0, _mm_mul_ps(Row3, Temp));
			Minor3 = _mm_sub_ps(_mm_mul_ps(Row0, Temp), Minor3);
			Minor3 = _mm_shuffle_ps(Minor3, Minor3, GD_FLOAT32X4INTRINSICS_SSE_SHUFFLEMASK(2, 3, 0, 1));
			// -----------------------------------------------
			Temp = _mm_mul_ps(_mm_shuffle_ps(Row1, Row1, GD_FLOAT32X4INTRINSICS_SSE_SHUFFLEMASK(2, 3, 0, 1)), Row3);
			Temp = _mm_shuffle_ps(Temp, Temp, GD_FLOAT32X4INTRINSICS_SSE_SHUFFLEMASK(1, 0, 3, 2));
			Row2 = _mm_shuffle_ps(Row2, Row2, GD_FLOAT32X4INTRINSICS_SSE_SHUFFLEMASK(2, 3, 0, 1));
			Minor0 = _mm_add_ps(_mm_mul_ps(Row2, Temp), Minor0);
			Minor2 = _mm_mul_ps(Row0, Temp);
			Temp = _mm_shuffle_ps(Temp, Temp, GD_FLOAT32X4INTRINSICS_SSE_SHUFFLEMASK(2, 3, 0, 1));
			Minor0 = _mm_sub_ps(Minor0, _mm_mul_ps(Row2, Temp));
			Minor2 = _mm_sub_ps(_mm_mul_ps(Row0, Temp), Minor2);
			Minor2 = _mm_shuffle_ps(Minor2, Minor2, GD_FLOAT32X4INTRINSICS_SSE_SHUFFLEMASK(2, 3, 0, 1));
			// -----------------------------------------------
			Temp = _mm_mul_ps(Row0, Row1);

			Temp = _mm_shuffle_ps(Temp, Temp, GD_FLOAT32X4INTRINSICS_SSE_SHUFFLEMASK(1, 0, 3, 2));
			Minor2 = _mm_add_ps(_mm_mul_ps(Row3, Temp), Minor2);
			Minor3 = _mm_sub_ps(_mm_mul_ps(Row2, Temp), Minor3);
			Temp = _mm_shuffle_ps(Temp, Temp, GD_FLOAT32X4INTRINSICS_SSE_SHUFFLEMASK(2, 3, 0, 1));
			Minor2 = _mm_sub_ps(_mm_mul_ps(Row3, Temp), Minor2);
			Minor3 = _mm_sub_ps(Minor3, _mm_mul_ps(Row2, Temp));
			// -----------------------------------------------
			Temp = _mm_mul_ps(Row0, Row3);
			Temp = _mm_shuffle_ps(Temp, Temp, GD_FLOAT32X4INTRINSICS_SSE_SHUFFLEMASK(1, 0, 3, 2));
			Minor1 = _mm_sub_ps(Minor1, _mm_mul_ps(Row2, Temp));
			Minor2 = _mm_add_ps(_mm_mul_ps(Row1, Temp), Minor2);
			Temp = _mm_shuffle_ps(Temp, Temp, GD_FLOAT32X4INTRINSICS_SSE_SHUFFLEMASK(2, 3, 0, 1));
			Minor1 = _mm_add_ps(_mm_mul_ps(Row2, Temp), Minor1);
			Minor2 = _mm_sub_ps(Minor2, _mm_mul_ps(Row1, Temp));
			// -----------------------------------------------
			Temp = _mm_mul_ps(Row0, Row2);
			Temp = _mm_shuffle_ps(Temp, Temp, GD_FLOAT32X4INTRINSICS_SSE_SHUFFLEMASK(1, 0, 3, 2));
			Minor1 = _mm_add_ps(_mm_mul_ps(Row3, Temp), Minor1);
			Minor3 = _mm_sub_ps(Minor3, _mm_mul_ps(Row1, Temp));
			Temp = _mm_shuffle_ps(Temp, Temp, GD_FLOAT32X4INTRINSICS_SSE_SHUFFLEMASK(2, 3, 0, 1));

			Minor1 = _mm_sub_ps(Minor1, _mm_mul_ps(Row3, Temp));
			Minor3 = _mm_add_ps(_mm_mul_ps(Row1, Temp), Minor3);
			// -----------------------------------------------
			Determinant = _mm_mul_ps(Row0, Minor0);
			Determinant = _mm_add_ps(_mm_shuffle_ps(Determinant, Determinant, GD_FLOAT32X4INTRINSICS_SSE_SHUFFLEMASK(2, 3, 0, 1)), Determinant);
			Determinant = _mm_add_ss(_mm_shuffle_ps(Determinant, Determinant, GD_FLOAT32X4INTRINSICS_SSE_SHUFFLEMASK(1, 0, 3, 2)), Determinant);
			Temp = _mm_rcp_ss(Determinant);
			Determinant = _mm_sub_ss(_mm_add_ss(Temp, Temp), _mm_mul_ss(Determinant, _mm_mul_ss(Temp, Temp)));
			Determinant = _mm_shuffle_ps(Determinant, Determinant, GD_FLOAT32X4INTRINSICS_SSE_SHUFFLEMASK(0, 0, 0, 0));
			Minor0 = _mm_mul_ps(Determinant, Minor0);
			_mm_storel_pi(reinterpret_cast<__m64*>(Matrix     ), Minor0);
			_mm_storeh_pi(reinterpret_cast<__m64*>(Matrix + 2 ), Minor0);
			Minor1 = _mm_mul_ps(Determinant, Minor1);
			_mm_storel_pi(reinterpret_cast<__m64*>(Matrix + 4 ), Minor1);
			_mm_storeh_pi(reinterpret_cast<__m64*>(Matrix + 6 ), Minor1);
			Minor2 = _mm_mul_ps(Determinant, Minor2);
			_mm_storel_pi(reinterpret_cast<__m64*>(Matrix + 8 ), Minor2);
			_mm_storeh_pi(reinterpret_cast<__m64*>(Matrix + 10), Minor2);
			Minor3 = _mm_mul_ps(Determinant, Minor3);
			_mm_storel_pi(reinterpret_cast<__m64*>(Matrix + 12), Minor3);
			_mm_storeh_pi(reinterpret_cast<__m64*>(Matrix + 14), Minor3);
		}
	}	// namespace Float32x4Intrinsics

GD_NAMESPACE_END

#endif
