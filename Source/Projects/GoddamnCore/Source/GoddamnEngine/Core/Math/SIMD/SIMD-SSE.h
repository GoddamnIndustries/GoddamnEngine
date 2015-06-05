//! ==========================================================================================
//! SIMDRegister-SSE.h: SIMD register intrinsics SSE implementation header.
//! Copyright (C) Goddamn Industries 2015. All Rights Reserved.
//! ==========================================================================================

#pragma once
#ifndef GD_CORE_MATH_VECTOR_REGISTER_SSE
#define GD_CORE_MATH_VECTOR_REGISTER_SSE

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Math/SIMD/SIMD.h>

#include <mmintrin.h>

// Assuming SSE4 is available..
#include <smmintrin.h>
#define GD_SSE_VERSION_MAJOR 4
#define GD_SSE_VERSION_MINOR 0
#if GD_SSE_VERSION_MAJOR < 2
#	error Unsupported SSE version. At least major version 2 is required.
#endif	// if GD_SSE_VERSION_MAJOR < 2

//! Represents true.
#define GD_SSE_YES 1

//! Represents false.
#define GD_SSE_NO 0

//! Represents axes.
//! @{
#define GD_SSE_X 0
#define GD_SSE_Y 1
#define GD_SSE_Z 2
#define GD_SSE_W 3
//! @}

//! Generates a shuffle mask for SSE.
//! @param A0 Selects which element (0 - 3) from 'A' into first slot in the result.
//! @param A1 Selects which element (0 - 3) from 'A' into second slot in the result.
//! @param B2 Selects which element (0 - 3) from 'B' into third slot in the result.
//! @param B3 Selects which element (0 - 3) from 'B' into fourth slot in the result.
#define GD_SSE_SHUFFLEMASK(A0, A1, B2, B3) _MM_SHUFFLE(B3, B2, A1, A0)

#if GD_SSE_VERSION_MAJOR >= 4

//! Generates a dot product computation mask for SSE.
//! @param DoUseSlot0 Determines whether slot 0 of input is used during dot product computation.
//! @param DoUseSlot1 Determines whether slot 1 of input is used during dot product computation.
//! @param DoUseSlot2 Determines whether slot 2 of input is used during dot product computation.
//! @param DoUseSlot3 Determines whether slot 3 of input is used during dot product computation.
//! @param DoCopyResultTo0 Determines whether result of the dot product computation would be copies to output slot 0.
//! @param DoCopyResultTo1 Determines whether result of the dot product computation would be copies to output slot 1.
//! @param DoCopyResultTo2 Determines whether result of the dot product computation would be copies to output slot 2.
//! @param DoCopyResultTo3 Determines whether result of the dot product computation would be copies to output slot 3.
//! @see https://msdn.microsoft.com/en-us/library/bb514054.aspx
//! @code
///		tmp0 := (DoUseSlot0 == 1) ? (a0 * b0) : +0.0
///		tmp1 := (DoUseSlot1 == 1) ? (a1 * b1) : +0.0
///		tmp2 := (DoUseSlot2 == 1) ? (a2 * b2) : +0.0
///		tmp3 := (DoUseSlot3 == 1) ? (a3 * b3) : +0.0
///
///		tmp4 := tmp0 + tmp1 + tmp2 + tmp3
///
///		r0 := (DoUseSlot0 == 1) ? tmp4 : +0.0
///		r1 := (DoUseSlot1 == 1) ? tmp4 : +0.0
///		r2 := (DoUseSlot2 == 1) ? tmp4 : +0.0
///		r3 := (DoUseSlot3 == 1) ? tmp4 : +0.0
//! @endcode
#define GD_SSE_DOTPRODUCT_MASK(DoUseSlot0, DoUseSlot1, DoUseSlot2, DoUseSlot3, DoCopyResultTo0, DoCopyResultTo1, DoCopyResultTo2, DoCopyResultTo3) \
	GD::UInt32(0  \
		| (((DoCopyResultTo1) & 1) << 0) | (((DoCopyResultTo1) & 1) << 1) | (((DoCopyResultTo2) & 1) << 2) | (((DoCopyResultTo3) & 1) << 3) /* Mask of the output of the dot product. */ \
		| (((DoUseSlot0)      & 1) << 4) | (((DoUseSlot1)      & 1) << 5) | (((DoUseSlot2)      & 1) << 6) | (((DoUseSlot3)      & 1) << 7) /* Mask of the usage of the input register slots. */ \
		) \

#endif	// if GD_SSE_VERSION_MAJOR >= 4

GD_NAMESPACE_BEGIN

	//! 4D Vector type stored in single register.
	typedef __m128 SIMDRegisterSSE;

	//! SIMD register math x86 SSE implementation.
	class SIMDSSE : public SIMDGeneric<SIMDRegisterSSE>
	{
	public:
		//! Creates a new SIMD register based on four floats.
		//! @param X First component of the SIMD register.
		//! @param Y Second component of the SIMD register.
		//! @param Z Third component of the SIMD register.
		//! @param W Fourth component of the SIMD register.
		//! @returns Created vector.
		GDINL static SIMDRegister GD_VECTORCALL Create(Float32 const X, Float32 const Y, Float32 const Z, Float32 const W)
		{
			return _mm_setr_ps(X, Y, Z, W);
		}

		//! Creates a new SIMD register based on bitwise equivalent of four integers.
		//! @param X First component of the SIMD register bitwise value.
		//! @param Y Second component of the SIMD register bitwise value.
		//! @param Z Third component of the SIMD register bitwise value.
		//! @param W Fourth component of the SIMD register bitwise value.
		//! @returns Created vector.
		GDINL static SIMDRegister GD_VECTORCALL CreateInt(UInt32 const X, UInt32 const Y, UInt32 const Z, UInt32 const W)
		{
			union 
			{
				__m128  F32Register;
				__m128i U32Register;
			} Wrapper;
			Wrapper.U32Register = _mm_setr_epi32(X, Y, Z, W);
			return Wrapper.F32Register;
		}

		//! Gets a single component of a vector.
		//! @param Register Specified register.
		//! @tparam ElementIndex Specified index of the element.
		//! @returns Specified single component of a vector.
		//! @{
		template<UInt32 const ElementIndex>
		GDINL static Float32 GetComponent(SIMDRegister const Register);
		template<>
		GDINL static Float32 GetComponent<GD_SSE_X>(SIMDRegister const Register)
		{
			return _mm_cvtss_f32(Register);
		}
		template<>
		GDINL static Float32 GetComponent<GD_SSE_Y>(SIMDRegister const Register)
		{
			return _mm_cvtss_f32(Replicate<GD_SSE_Y>(Register));
		}
		template<>
		GDINL static Float32 GetComponent<GD_SSE_Z>(SIMDRegister const Register)
		{
			return _mm_cvtss_f32(Replicate<GD_SSE_Z>(Register));
		}
		template<>
		GDINL static Float32 GetComponent<GD_SSE_W>(SIMDRegister const Register)
		{
			return _mm_cvtss_f32(Replicate<GD_SSE_W>(Register));
		}
		//! @}

		//! Sets a single component of a vector.
		//! @param Register Specified register.
		//! @param Value Value of the component to be set.
		//! @tparam ElementIndex Specified index of the element.
		//! @returns Modified SIMD register.
		//! @{
		template<UInt32 const ElementIndex>
		GDINL static SIMDRegister SetComponent(SIMDRegister const Register, Float32 const Value);
		template<>
		GDINL static SIMDRegister SetComponent<GD_SSE_X>(SIMDRegister const Register, Float32 const Value)
		{
			return _mm_move_ss(Register, _mm_set_ss(Value));
		}
		template<>
		GDINL static SIMDRegister SetComponent<GD_SSE_Y>(SIMDRegister const Register, Float32 const Value)
		{
			SIMDRegister const Result = _mm_move_ss(_mm_shuffle_ps(Register, Register, GD_SSE_SHUFFLEMASK(GD_SSE_Y, GD_SSE_X, GD_SSE_Z, GD_SSE_W)), _mm_set_ss(Value));
			return _mm_shuffle_ps(Result, Result, GD_SSE_SHUFFLEMASK(GD_SSE_Y, GD_SSE_X, GD_SSE_Z, GD_SSE_W));
		}
		template<>
		GDINL static SIMDRegister SetComponent<GD_SSE_Z>(SIMDRegister const Register, Float32 const Value)
		{
			SIMDRegister const Result = _mm_move_ss(_mm_shuffle_ps(Register, Register, GD_SSE_SHUFFLEMASK(GD_SSE_Z, GD_SSE_Y, GD_SSE_X, GD_SSE_W)), _mm_set_ss(Value));
			return _mm_shuffle_ps(Result, Result, GD_SSE_SHUFFLEMASK(GD_SSE_Z, GD_SSE_Y, GD_SSE_X, GD_SSE_W));
		}
		template<>
		GDINL static SIMDRegister SetComponent<GD_SSE_W>(SIMDRegister const Register, Float32 const Value)
		{
			SIMDRegister const Result = _mm_move_ss(_mm_shuffle_ps(Register, Register, GD_SSE_SHUFFLEMASK(GD_SSE_W, GD_SSE_Y, GD_SSE_Z, GD_SSE_X)), _mm_set_ss(Value));
			return _mm_shuffle_ps(Result, Result, GD_SSE_SHUFFLEMASK(GD_SSE_W, GD_SSE_Y, GD_SSE_Z, GD_SSE_X));
		}
		//! @}

		//! Creates a vector through selecting two components from each vector via a shuffle mask. 
		//! @param First First register to shuffle.
		//! @param Second Second register to shuffle.
		//! @tparam X Index for which component to use for X (literal 0-3).
		//! @tparam Y Index for which component to use for Y (literal 0-3).
		//! @tparam Z Index for which component to use for Z (literal 0-3).
		//! @tparam W Index for which component to use for W (literal 0-3).
		//! @returns The shuffled vector.
		template<UInt32 const X, UInt32 const Y, UInt32 const Z, UInt32 const W>
		GDINL static SIMDRegister GD_VECTORCALL Shuffle(SIMDRegister const First, SIMDRegister const Second)
		{
			static_assert(X <= 3, "Invalid X index.");
			static_assert(Y <= 3, "Invalid Y index.");
			static_assert(Z <= 3, "Invalid Z index.");
			static_assert(W <= 3, "Invalid W index.");
			return _mm_shuffle_ps(First, Second, GD_SSE_SHUFFLEMASK(X, Y, Z, W));
		}

		//! Returns swizzling result of all components of a specified vector.
		//! @param Register Specified register.
		//! @tparam X Index for which component to use for X (literal 0-3).
		//! @tparam Y Index for which component to use for Y (literal 0-3).
		//! @tparam Z Index for which component to use for Z (literal 0-3).
		//! @tparam W Index for which component to use for W (literal 0-3).
		//! @returns The swizzled vector.
		template<UInt32 const X, UInt32 const Y, UInt32 const Z, UInt32 const W>
		GDINL static SIMDRegister GD_VECTORCALL Swizzle(SIMDRegister const Register)
		{
			static_assert(X <= 3, "Invalid X index.");
			static_assert(Y <= 3, "Invalid Y index.");
			static_assert(Z <= 3, "Invalid Z index.");
			static_assert(W <= 3, "Invalid W index.");
			return _mm_shuffle_ps(Register, Register, GD_SSE_SHUFFLEMASK(X, Y, Z, W));
		}

		//! Returns new register with all components assigned with specified component of a specified register.
		//! @param Register Specified register.
		//! @tparam ElementIndex Specified index of the element.
		template<UInt32 const ElementIndex>
		GDINL static SIMDRegister GD_VECTORCALL Replicate(SIMDRegister const Register)
		{
			static_assert(ElementIndex <= 3, "Invalid element index.");
			return _mm_shuffle_ps(Register, Register, GD_SSE_SHUFFLEMASK(ElementIndex, ElementIndex, ElementIndex, ElementIndex));
		}

		//! Returns component-wise absolute value of the register.
		//! @param Register Specified register.
		//! @returns Component-wise negated value of the register.
		GDINL static SIMDRegister GD_VECTORCALL Abs(SIMDRegister const Register)
		{
			UInt32 const static SignBit = (~(1 << 31));
			SIMDRegister const static SignMask = CreateInt(SignBit, SignBit, SignBit, SignBit);
			return _mm_and_ps(Register, SignMask);
		}

		//! Performers bitwise OR for two registers.
		//! @param First First specified register.
		//! @param Second Second specified register.
		//! @returns Bitwise OR for two registers.
		GDINL static SIMDRegister GD_VECTORCALL BitwiseOr(SIMDRegister const First, SIMDRegister const Second)
		{
			return _mm_or_ps(First, Second);
		}

		//! Performers bitwise XOR for two registers.
		//! @param First First specified register.
		//! @param Second Second specified register.
		//! @returns Bitwise XOR for two registers.
		GDINL static SIMDRegister GD_VECTORCALL BitwiseXor(SIMDRegister const First, SIMDRegister const Second)
		{
			return _mm_xor_ps(First, Second);
		}

		//! Performers bitwise AND for two registers.
		//! @param First First specified register.
		//! @param Second Second specified register.
		//! @returns Bitwise AND for two registers.
		GDINL static SIMDRegister GD_VECTORCALL BitwiseAnd(SIMDRegister const First, SIMDRegister const Second)
		{
			return _mm_and_ps(First, Second);
		}

		//! Returns component-wise negated value of the register.
		//! @param Register Specified register.
		//! @returns Component-wise negated value of the register.
		GDINL static SIMDRegister GD_VECTORCALL Negate(SIMDRegister const Register)
		{
			return _mm_sub_ps(_mm_setzero_ps(), Register);
		}

		//! Adds two registers and returns a result.
		//! @param First First specified register.
		//! @param Second Second specified register.
		//! @returns Per-component sum of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Add(SIMDRegister const First, SIMDRegister const Second)
		{
			return _mm_add_ps(First, Second);
		}

		//! Subtracts two registers and returns a result.
		//! @param First First specified register.
		//! @param Second Second specified register.
		//! @returns Per-component subtraction product of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Sub(SIMDRegister const First, SIMDRegister const Second)
		{
			return _mm_sub_ps(First, Second);
		}

		//! Multiplies two registers and returns a result.
		//! @param First First specified register.
		//! @param Second Second specified register.
		//! @returns Per-component multiplication product of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Mul(SIMDRegister const First, SIMDRegister const Second)
		{
			return _mm_mul_ps(First, Second);
		}

		//! Divides two registers and returns a result.
		//! @param First First specified register.
		//! @param Second Second specified register.
		//! @returns Per-component division product of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Div(SIMDRegister const First, SIMDRegister const Second)
		{
			return _mm_div_ps(First, Second);
		}

		//! Computes dot product of two SIMD registers, treated as 3D vectors..
		//! @param First First specified register.
		//! @param Second Second specified register.
		//! @returns Dot product of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Dot3(SIMDRegister const First, SIMDRegister const Second)
		{
#if GD_SSE_VERSION_MAJOR >= 4
			return _mm_dp_ps(First, Second, GD_SSE_DOTPRODUCT_MASK(GD_SSE_YES, GD_SSE_YES, GD_SSE_YES, GD_SSE_NO, GD_SSE_YES, GD_SSE_YES, GD_SSE_YES, GD_SSE_YES));
#else	// if GD_SSE_VERSION_MAJOR >= 4
			SIMDRegister const MulResult = Mul(First, Second);
			return Add(Replicate<GD_SSE_X>(MulResult), Add(Replicate<GD_SSE_Y>(MulResult), Replicate<GD_SSE_Z>(MulResult)));
#endif	// if GD_SSE_VERSION_MAJOR >= 4
		}

		//! Computes dot product of two SIMD registers, treated as 4D vectors..
		//! @param First First specified register.
		//! @param Second Second specified register.
		//! @returns Dot product of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Dot4(SIMDRegister const First, SIMDRegister const Second)
		{
#if GD_SSE_VERSION_MAJOR >= 4
			return _mm_dp_ps(First, Second, GD_SSE_DOTPRODUCT_MASK(GD_SSE_YES, GD_SSE_YES, GD_SSE_YES, GD_SSE_YES, GD_SSE_YES, GD_SSE_YES, GD_SSE_YES, GD_SSE_YES));
#else	// if GD_SSE_VERSION_MAJOR >= 4
			SIMDRegister MulResult = Mul(First, Second);
			SIMDRegister Temp = _mm_shuffle_ps(MulResult, MulResult, GD_SSE_SHUFFLEMASK(GD_SSE_Z, GD_SSE_W, GD_SSE_X, GD_SSE_Y));
			MulResult = Add(MulResult, Temp);
			Temp = _mm_shuffle_ps(MulResult, MulResult, GD_SSE_SHUFFLEMASK(GD_SSE_Y, GD_SSE_Z, GD_SSE_W, GD_SSE_X));
			return Add(MulResult, Temp);
#endif	// if GD_SSE_VERSION_MAJOR >= 4
		}

		//! Computes cross product of two SIMD registers, treated as 3D vectors..
		//! @param First First specified register.
		//! @param Second Second specified register.
		//! @returns Cross product of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Cross3(SIMDRegister const First, SIMDRegister const Second)
		{
			SIMDRegister const  FirstYZXW = _mm_shuffle_ps(First,   First, GD_SSE_SHUFFLEMASK(GD_SSE_Y, GD_SSE_Z, GD_SSE_X, GD_SSE_W));
			SIMDRegister const  FirstZXYW = _mm_shuffle_ps(First,   First, GD_SSE_SHUFFLEMASK(GD_SSE_Z, GD_SSE_X, GD_SSE_Y, GD_SSE_W));
			SIMDRegister const SecondZXYW = _mm_shuffle_ps(Second, Second, GD_SSE_SHUFFLEMASK(GD_SSE_Z, GD_SSE_X, GD_SSE_Y, GD_SSE_W));
			SIMDRegister const SecondYZXW = _mm_shuffle_ps(Second, Second, GD_SSE_SHUFFLEMASK(GD_SSE_Y, GD_SSE_Z, GD_SSE_X, GD_SSE_W));
			return Sub(Mul(FirstYZXW, SecondZXYW), Mul(FirstZXYW, SecondYZXW));
		}

		//! Computes cross product of three SIMD registers, treated as 4D vectors..
		//! @param First First specified register.
		//! @param Second Second specified register.
		//! @param Third Third specified register.
		//! @returns Cross product of three registers.
		GDINL static SIMDRegister GD_VECTORCALL Cross4(SIMDRegister const First, SIMDRegister const Second, SIMDRegister const Third)
		{
			SIMDRegister Result = _mm_shuffle_ps(Second, Second, GD_SSE_SHUFFLEMASK(GD_SSE_Z, GD_SSE_Y, GD_SSE_W, GD_SSE_Z));
			SIMDRegister Temp3 = _mm_shuffle_ps(Third, Third, GD_SSE_SHUFFLEMASK(GD_SSE_Y, GD_SSE_W, GD_SSE_Z, GD_SSE_W));
			Result = _mm_mul_ps(Result, Temp3);
			SIMDRegister Temp2 = _mm_shuffle_ps(Second, Second, GD_SSE_SHUFFLEMASK(GD_SSE_Y, GD_SSE_W, GD_SSE_Z, GD_SSE_W));
			Temp3 = _mm_shuffle_ps(Temp3, Temp3, GD_SSE_SHUFFLEMASK(GD_SSE_Y, GD_SSE_W, GD_SSE_X, GD_SSE_Y));
			Temp2 = _mm_mul_ps(Temp2, Temp3);
			Result = _mm_sub_ps(Result, Temp2);
			SIMDRegister Temp1 = _mm_shuffle_ps(First, First, GD_SSE_SHUFFLEMASK(GD_SSE_X, GD_SSE_X, GD_SSE_X, GD_SSE_Y));
			Result = _mm_mul_ps(Result, Temp1);
			Temp2 = _mm_shuffle_ps(Second, Second, GD_SSE_SHUFFLEMASK(GD_SSE_Z, GD_SSE_X, GD_SSE_W, GD_SSE_Y));
			Temp3 = _mm_shuffle_ps(Third, Third, GD_SSE_SHUFFLEMASK(GD_SSE_X, GD_SSE_W, GD_SSE_X, GD_SSE_W));
			Temp3 = _mm_mul_ps(Temp3, Temp2);
			Temp2 = _mm_shuffle_ps(Temp2, Temp2, GD_SSE_SHUFFLEMASK(GD_SSE_Z, GD_SSE_Y, GD_SSE_Z, GD_SSE_Y));
			Temp1 = _mm_shuffle_ps(Third, Third, GD_SSE_SHUFFLEMASK(GD_SSE_Z, GD_SSE_X, GD_SSE_W, GD_SSE_Y));
			Temp2 = _mm_mul_ps(Temp2, Temp1);
			Temp3 = _mm_sub_ps(Temp3, Temp2);
			Temp1 = _mm_shuffle_ps(First, First, GD_SSE_SHUFFLEMASK(GD_SSE_Y, GD_SSE_Y, GD_SSE_Z, GD_SSE_Z));
			Temp1 = _mm_mul_ps(Temp1, Temp3);
			Result = _mm_sub_ps(Result, Temp1);
			Temp2 = _mm_shuffle_ps(Second, Second, GD_SSE_SHUFFLEMASK(GD_SSE_Y, GD_SSE_X, GD_SSE_Z, GD_SSE_Y));
			Temp3 = _mm_shuffle_ps(Third, Third, GD_SSE_SHUFFLEMASK(GD_SSE_X, GD_SSE_Y, GD_SSE_X, GD_SSE_Z));
			Temp3 = _mm_mul_ps(Temp3, Temp2);
			Temp2 = _mm_shuffle_ps(Temp2, Temp2, GD_SSE_SHUFFLEMASK(GD_SSE_Z, GD_SSE_X, GD_SSE_Z, GD_SSE_Y));
			Temp1 = _mm_shuffle_ps(Third, Third, GD_SSE_SHUFFLEMASK(GD_SSE_Y, GD_SSE_X, GD_SSE_Z, GD_SSE_Y));
			Temp1 = _mm_mul_ps(Temp1, Temp2);
			Temp3 = _mm_sub_ps(Temp3, Temp1);
			Temp1 = _mm_shuffle_ps(First, First, GD_SSE_SHUFFLEMASK(GD_SSE_Z, GD_SSE_W, GD_SSE_W, GD_SSE_W));
			Temp3 = _mm_mul_ps(Temp3, Temp1);
			Result = _mm_add_ps(Result, Temp3);
			return Result;
		}
	};	// class SIMDSSE
	typedef SIMDSSE SIMDImplementation;

GD_NAMESPACE_END

#undef GD_SSE_DOTPRODUCT_MASK
#undef GD_SSE_SHUFFLEMASK

#undef GD_SSE_X
#undef GD_SSE_Y
#undef GD_SSE_Z
#undef GD_SSE_W

#undef GD_SSE_YES
#undef GD_SSE_NO

#undef GD_SSE_VERSION_MAJOR
#undef GD_SSE_VERSION_MINOR

#endif	// ifndef GD_CORE_MATH_VECTOR_REGISTER_SSE
