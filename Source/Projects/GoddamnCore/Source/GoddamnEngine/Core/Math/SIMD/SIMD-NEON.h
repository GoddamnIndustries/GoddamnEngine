/// ==========================================================================================
/// SIMDRegister-DirectXMath.h: SIMD register intrinsics NEON implementation header.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_MATH_VECTOR_REGISTER_NEON
#define GD_CORE_MATH_VECTOR_REGISTER_NEON

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Math/SIMD/SIMD.h>
#include <GoddamnEngine/Core/Diagnostics/Assertion/Assertion.h>

#include <arm_neon.h>

GD_NAMESPACE_BEGIN

	/// @brief 4D Vector type stored in single register.
	typedef GD_ALIGN_MSVC(16) float32x4_t GD_ALIGN_GCC(16) SIMDRegisterNEON;

	/// @brief SIMD register math ARM NEON implementation.
	class SIMDNEON : public SIMDGeneric<SIMDRegisterNEON>
	{
	public:
		/// @brief Creates a new SIMD register based on four floats.
		/// @param X First component of the SIMD register.
		/// @param Y Second component of the SIMD register.
		/// @param Z Third component of the SIMD register.
		/// @param W Fourth component of the SIMD register.
		/// @returns Created vector.
		GDINL static SIMDRegister GD_VECTORCALL Create(Float32 const X, Float32 const Y, Float32 const Z, Float32 const W)
		{
			union {
				SIMDRegister Register;
				Float32 Values[4];
			} Wrapper;
			Wrapper.Values[0] = X;
			Wrapper.Values[1] = Y;
			Wrapper.Values[2] = Z;
			Wrapper.Values[3] = W;
			return Wrapper.Register;
		}

		/// @brief Creates a new SIMD register based on bitwise equivalent of four integers.
		/// @param X First component of the SIMD register bitwise value.
		/// @param Y Second component of the SIMD register bitwise value.
		/// @param Z Third component of the SIMD register bitwise value.
		/// @param W Fourth component of the SIMD register bitwise value.
		/// @returns Created vector.
		GDINL static SIMDRegister GD_VECTORCALL CreateInt(UInt32 const X, UInt32 const Y, UInt32 const Z, UInt32 const W)
		{
			union {
				SIMDRegister Register;
				UInt32 Values[4];
			} Wrapper;
			Wrapper.Values[0] = X;
			Wrapper.Values[1] = Y;
			Wrapper.Values[2] = Z;
			Wrapper.Values[3] = W;
			return Wrapper.Register;
		}

		/// @brief Gets a single component of a vector.
		/// @param Register Specified register.
		/// @tparam ElementIndex Specified index of the element.
		/// @returns Specified single component of a vector.
		template<UInt32 const ElementIndex>
		GDINL static Float32 GetComponent(SIMDRegister const Register)
		{
			static_assert(ElementIndex <= 3, "Invalid element index.");
			return vgetq_lane_f32(Register, ElementIndex);
		}

		/// @brief Sets a single component of a vector.
		/// @param Register Specified register.
		/// @param Value Value of the component to be set.
		/// @tparam ElementIndex Specified index of the element.
		/// @returns Modified SIMD register.
		template<UInt32 const ElementIndex>
		GDINL static SIMDRegister SetComponent(SIMDRegister const Register, Float32 const Value)
		{
			static_assert(ElementIndex <= 3, "Invalid element index.");
			return vsetq_lane_f32(Value, Register, ElementIndex);
		}

		/// @brief Creates a vector through selecting two components from each vector via a shuffle mask. 
		/// @param First First register to shuffle.
		/// @param Second Second register to shuffle.
		/// @tparam X Index for which component to use for X (literal 0-3).
		/// @tparam Y Index for which component to use for Y (literal 0-3).
		/// @tparam Z Index for which component to use for Z (literal 0-3).
		/// @tparam W Index for which component to use for W (literal 0-3).
		/// @returns The shuffled vector.
		/// @{
#if GD_DEBUG
		template<UInt32 const X, UInt32 const Y, UInt32 const Z, UInt32 const W>
		GDINL static SIMDRegister GD_VECTORCALL Shuffle(SIMDRegister const& First, SIMDRegister const& Second)
		{
			GD_DEBUG_ASSERT(&First != &Second, 
				"Two same SIMD register have been passed to 'Shuffle'. "
				"In NEON implementation, prefer using 'Swizzle' for better results.");

#else	// if GD_DEBUG
		template<UInt32 const X, UInt32 const Y, UInt32 const Z, UInt32 const W>
		GDINL static SIMDRegister GD_VECTORCALL Shuffle(SIMDRegister const First, SIMDRegister const Second)
		{
#endif	// if GD_DEBUG
			static_assert(X <= 3, "Invalid X index.");
			static_assert(Y <= 3, "Invalid Y index.");
			static_assert(Z <= 3, "Invalid Z index.");
			static_assert(W <= 3, "Invalid W index.");
#if __has_builtin(__builtin_shufflevector)
			return __builtin_shufflevector(First, Second, X, Y, Z + 4, W + 4);
#else	// if __has_builtin(__builtin_shufflevector)
			union {
				SIMDRegister const Register;
				Float32 Values[4];
			} FirstWrapper, SecondWrapper;
			FirstWrapper.Register = First;
			SecondWrapper.Register = SecondWrapper;
			return Create(FirstWrapper.Values[static_cast<size_t>(X)], FirstWrapper.Values[static_cast<size_t>(Y)], SecondWrapper.Values[static_cast<size_t>(Z)], SecondWrapper.Values[static_cast<size_t>(W)]);
#endif	// if __has_builtin(__builtin_shufflevector)
		}
		/// @todo Create explicit specializations for specific values.
		/// @}

		/// @brief Returns swizzling result of all components of a specified vector.
		/// @param Register Specified register.
		/// @tparam X Index for which component to use for X (literal 0-3).
		/// @tparam Y Index for which component to use for Y (literal 0-3).
		/// @tparam Z Index for which component to use for Z (literal 0-3).
		/// @tparam W Index for which component to use for W (literal 0-3).
		/// @returns The swizzled vector.
		/// @{
		template<UInt32 const X, UInt32 const Y, UInt32 const Z, UInt32 const W>
		GDINL static SIMDRegister GD_VECTORCALL Swizzle(SIMDRegister const Register)
		{
			static_assert(X <= 7, "Invalid X index.");
			static_assert(Y <= 7, "Invalid Y index.");
			static_assert(Z <= 7, "Invalid Z index.");
			static_assert(W <= 7, "Invalid W index.");
#if __has_builtin(__builtin_shufflevector)
			return __builtin_shufflevector(Register, Register, X, Y, Z, W);
#else	// if __has_builtin(__builtin_shufflevector)
			union {
				SIMDRegister const Register;
				Float32 Values[4];
			} Wrapper;
			Wrapper.Register = Register;
			return Create(Wrapper.Values[static_cast<size_t>(X)], Wrapper.Values[static_cast<size_t>(Y)], Wrapper.Values[static_cast<size_t>(Z)], Wrapper.Values[static_cast<size_t>(W)]);
#endif	// if __has_builtin(__builtin_shufflevector)
		}
		template<>
		GDINL static SIMDRegister GD_VECTORCALL Swizzle<0, 1, 2, 3>(SIMDRegister const Register)
		{
			return Register;
		}
		template<> 
		GDINL static SIMDRegister GD_VECTORCALL Swizzle<0, 0, 0, 0>(SIMDRegister const Register) 
		{ 
			return vdupq_lane_f32(vget_low_f32(Register), 0); 
		}
		template<> 
		GDINL static SIMDRegister GD_VECTORCALL Swizzle<1, 1, 1, 1>(SIMDRegister const Register) 
		{ 
			return vdupq_lane_f32(vget_low_f32(Register), 1); 
		}
		template<> 
		GDINL static SIMDRegister GD_VECTORCALL Swizzle<2, 2, 2, 2>(SIMDRegister const Register) 
		{ 
			return vdupq_lane_f32(vget_high_f32(Register), 0); 
		}
		template<> 
		GDINL static SIMDRegister GD_VECTORCALL Swizzle<3, 3, 3, 3>(SIMDRegister const Register) 
		{ 
			return vdupq_lane_f32(vget_high_f32(Register), 1); 
		}
		template<> 
		GDINL static SIMDRegister GD_VECTORCALL Swizzle<1, 0, 3, 2>(SIMDRegister const Register) 
		{ 
			return vrev64q_f32(Register); 
		}
		template<> 
		GDINL static SIMDRegister GD_VECTORCALL Swizzle<0, 1, 0, 1>(SIMDRegister const Register) 
		{ 
			float32x2_t const Temp = vget_low_f32(Register); 
			return vcombine_f32(Temp, Temp); 
		}
		template<> 
		GDINL static SIMDRegister GD_VECTORCALL Swizzle<2, 3, 2, 3>(SIMDRegister const Register) 
		{ 
			float32x2_t const Temp = vget_high_f32(Register);
			return vcombine_f32(Temp, Temp); 
		}
		template<> 
		GDINL static SIMDRegister GD_VECTORCALL Swizzle<1, 0, 1, 0>(SIMDRegister const Register) 
		{ 
			float32x2_t const Temp = vrev64_f32(vget_low_f32(Register));
			return vcombine_f32(Temp, Temp); 
		}
		template<> 
		GDINL static SIMDRegister GD_VECTORCALL Swizzle<3, 2, 3, 2>(SIMDRegister const Register) 
		{ 
			float32x2_t const Temp = vrev64_f32(vget_high_f32(Register));
			return vcombine_f32(Temp, Temp); 
		}
		template<> 
		GDINL static SIMDRegister GD_VECTORCALL Swizzle<0, 1, 3, 2>(SIMDRegister const Register) 
		{ 
			return vcombine_f32(vget_low_f32(Register), vrev64_f32(vget_high_f32(Register))); 
		}
		template<> 
		GDINL static SIMDRegister GD_VECTORCALL Swizzle<1, 0, 2, 3>(SIMDRegister const Register) 
		{ 
			return vcombine_f32(vrev64_f32(vget_low_f32(Register)), vget_high_f32(Register)); 
		}
		template<> 
		GDINL static SIMDRegister GD_VECTORCALL Swizzle<2, 3, 1, 0>(SIMDRegister const Register) 
		{ 
			return vcombine_f32(vget_high_f32(Register), vrev64_f32(vget_low_f32(Register))); 
		}
		template<> 
		GDINL static SIMDRegister GD_VECTORCALL Swizzle<3, 2, 0, 1>(SIMDRegister const Register) 
		{ 
			return vcombine_f32(vrev64_f32(vget_high_f32(Register)), vget_low_f32(Register)); 
		}
		template<> 
		GDINL static SIMDRegister GD_VECTORCALL Swizzle<3, 2, 1, 0>(SIMDRegister const Register) 
		{ 
			return vcombine_f32(vrev64_f32(vget_high_f32(Register)), vrev64_f32(vget_low_f32(Register))); 
		}
		template<> 
		GDINL static SIMDRegister GD_VECTORCALL Swizzle<0, 0, 2, 2>(SIMDRegister const Register) 
		{ 
			union {
				float32x4x2_t Registers;
				float32x4_t Values[2];
			} Wrapper;
			Wrapper.Registers = vtrnq_f32(Register, Register);
			return Wrapper.Values[0];
		}
		template<> 
		GDINL static SIMDRegister GD_VECTORCALL Swizzle<1, 1, 3, 3>(SIMDRegister const Register) 
		{ 
			union {
				float32x4x2_t Registers;
				float32x4_t Values[2];
			} Wrapper;
			Wrapper.Registers = vtrnq_f32(Register, Register);
			return Wrapper.Values[1];
		}
		template<> 
		GDINL static SIMDRegister GD_VECTORCALL Swizzle<0, 0, 1, 1>(SIMDRegister const Register) 
		{ 
			union {
				float32x4x2_t Registers;
				float32x4_t Values[2];
			} Wrapper;
			Wrapper.Registers = vzipq_f32(Register, Register);
			return Wrapper.Values[0];
		}
		template<> 
		GDINL static SIMDRegister GD_VECTORCALL Swizzle<2, 2, 3, 3>(SIMDRegister const Register) 
		{ 
			union {
				float32x4x2_t Registers;
				float32x4_t Values[2];
			} Wrapper;
			Wrapper.Registers = vzipq_f32(Register, Register);
			return Wrapper.Values[1];
		}
		template<> 
		GDINL static SIMDRegister GD_VECTORCALL Swizzle<0, 2, 0, 2>(SIMDRegister const Register) 
		{ 
			union {
				float32x4x2_t Registers;
				float32x4_t Values[2];
			} Wrapper;
			Wrapper.Registers = vuzpq_f32(Register, Register);
			return Wrapper.Values[0];
		}
		template<> 
		GDINL static SIMDRegister GD_VECTORCALL Swizzle<1, 3, 1, 3>(SIMDRegister const Register) 
		{ 
			union {
				float32x4x2_t Registers;
				float32x4_t Values[2];
			} Wrapper;
			Wrapper.Registers = vuzpq_f32(Register, Register);
			return Wrapper.Values[1];
		}
		template<> 
		GDINL static SIMDRegister GD_VECTORCALL Swizzle<1, 2, 3, 0>(SIMDRegister const Register) 
		{ 
			return vextq_f32(Register, Register, 1); 
		}
		template<> 
		GDINL static SIMDRegister GD_VECTORCALL Swizzle<2, 3, 0, 1>(SIMDRegister const Register) 
		{ 
			return vextq_f32(Register, Register, 2); 
		}
		template<> 
		GDINL static SIMDRegister GD_VECTORCALL Swizzle<3, 0, 1, 2>(SIMDRegister const Register) 
		{ 
			return vextq_f32(Register, Register, 3); 
		}
		/// @}

		/// @brief Returns new register with all components assigned with specified component of a specified register.
		/// @param Register Specified register.
		/// @tparam ElementIndex Specified index of the element.
		template<UInt32 const ElementIndex>
		GDINL static SIMDRegister GD_VECTORCALL Replicate(SIMDRegister const Register)
		{
			static_assert(ElementIndex <= 3, "Invalid element index.");
			return vdupq_n_f32(vgetq_lane_f32(Register, ElementIndex));
		}

		/// @brief Returns component-wise absolute value of the register.
		/// @param Register Specified register.
		/// @returns Component-wise negated value of the register.
		GDINL static SIMDRegister GD_VECTORCALL Abs(SIMDRegister const Register)
		{
			return vabsq_f32(Register);
		}

		/// @brief Returns component-wise negated value of the register.
		/// @param Register Specified register.
		/// @returns Component-wise negated value of the register.
		GDINL static SIMDRegister GD_VECTORCALL Negate(SIMDRegister const Register)
		{
			return vnegq_f32(Register);
		}

		/// @brief Performers bitwise OR for two registers.
		/// @param First First specified register.
		/// @param Second Second specified register.
		/// @returns Bitwise OR for two registers.
		GDINL static SIMDRegister GD_VECTORCALL BitwiseOr(SIMDRegister const First, SIMDRegister const Second)
		{
			return static_cast<SIMDRegister>(vorrq_u32(static_cast<uint32x4_t>(First), static_cast<uint32x4_t>(Second)));
		}

		/// @brief Performers bitwise XOR for two registers.
		/// @param First First specified register.
		/// @param Second Second specified register.
		/// @returns Bitwise XOR for two registers.
		GDINL static SIMDRegister GD_VECTORCALL BitwiseXor(SIMDRegister const First, SIMDRegister const Second)
		{
			return static_cast<SIMDRegister>(veorq_u32(static_cast<uint32x4_t>(First), static_cast<uint32x4_t>(Second)));
		}

		/// @brief Performers bitwise AND for two registers.
		/// @param First First specified register.
		/// @param Second Second specified register.
		/// @returns Bitwise AND for two registers.
		GDINL static SIMDRegister GD_VECTORCALL BitwiseAnd(SIMDRegister const First, SIMDRegister const Second)
		{
			return static_cast<SIMDRegister>(vandq_u32(static_cast<uint32x4_t>(First), static_cast<uint32x4_t>(Second)));
		}

		/// @brief Adds two registers and returns a result.
		/// @param First First specified register.
		/// @param Second Second specified register.
		/// @returns Per-component sum of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Add(SIMDRegister const First, SIMDRegister const Second)
		{
			return vaddq_f32(First, Second);
		}

		/// @brief Subtracts two registers and returns a result.
		/// @param First First specified register.
		/// @param Second Second specified register.
		/// @returns Per-component subtraction product of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Sub(SIMDRegister const First, SIMDRegister const Second)
		{
			return vsubq_f32(First, Second);
		}

		/// @brief Multiplies two registers and returns a result.
		/// @param First First specified register.
		/// @param Second Second specified register.
		/// @returns Per-component multiplication product of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Mul(SIMDRegister const First, SIMDRegister const Second)
		{
			return vmulq_f32(First, Second);
		}

		/// @brief Divides two registers and returns a result.
		/// @param First First specified register.
		/// @param Second Second specified register.
		/// @returns Per-component division product of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Div(SIMDRegister const First, SIMDRegister const Second)
		{
			SIMDRegister Reciprocal = vrecpeq_f32(Second);
			Reciprocal = vmulq_f32(vrecpsq_f32(Second, Reciprocal), Reciprocal);
			Reciprocal = vmulq_f32(vrecpsq_f32(Second, Reciprocal), Reciprocal);
			return vmulq_f32(First, Reciprocal);
		}

		/// @brief Computes dot product of two SIMD registers, treated as 3D vectors..
		/// @param First First specified register.
		/// @param Second Second specified register.
		/// @returns Dot product of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Dot3(SIMDRegister const First, SIMDRegister const Second)
		{
			SIMDRegister MulResult = vmulq_f32(First, Second);
			float32x2_t Temp1 = vget_low_f32(MulResult);
			float32x2_t Temp2 = vget_high_f32(MulResult);
			Temp1 = vpadd_f32(Temp1, Temp1);
			Temp2 = vdup_lane_f32(Temp2, 0);
			Temp1 = vadd_f32(Temp1, Temp2);
			return vcombine_f32(Temp1, Temp1);
		}

		/// @brief Computes dot product of two SIMD registers, treated as 4D vectors..
		/// @param First First specified register.
		/// @param Second Second specified register.
		/// @returns Dot product of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Dot4(SIMDRegister const First, SIMDRegister const Second)
		{
			float32x4_t MulResult = vmulq_f32(First, Second);
			float32x2_t Temp1 = vget_low_f32(MulResult);
			float32x2_t Temp2 = vget_high_f32(MulResult);
			Temp1 = vpadd_f32(Temp1, Temp1);
			Temp2 = vpadd_f32(Temp2, Temp2);
			Temp1 = vadd_f32(Temp1, Temp2);
			return vcombine_f32(Temp1, Temp1);
		}

		/// @brief Computes cross product of two SIMD registers, treated as 3D vectors..
		/// @param First First specified register.
		/// @param Second Second specified register.
		/// @returns Cross product of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Cross3(SIMDRegister const First, SIMDRegister const Second)
		{
			SIMDRegister Result = Sub(
				Mul(Swizzle<1, 2, 0, 1>(First), Swizzle<2, 0, 1, 3>(Second)), 
				Mul(Swizzle<2, 0, 1, 3>(First), Swizzle<1, 2, 0, 1>(Second)));
			Result = SetComponent<3>(Result, 0.0f);
			return Result;
		}

		/// @brief Computes cross product of three SIMD registers, treated as 3D vectors..
		/// @param First First specified register.
		/// @param Second Second specified register.
		/// @param Third Third specified register.
		/// @returns Cross product of three registers.
		GDINL static SIMDRegister GD_VECTORCALL Cross4(SIMDRegister const First, SIMDRegister const Second, SIMDRegister const Third)
		{
			SIMDRegister const static MaskX = CreateInt(0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000);
			float32x2_t const select = vget_low_f32(MaskX);
			float32x2_t const v2xy = vget_low_f32(Second);
			float32x2_t const v2zw = vget_high_f32(Second);
			float32x2_t const v2yx = vrev64_f32(v2xy);
			float32x2_t const v2wz = vrev64_f32(v2zw);
			float32x2_t const v2yz = vbsl_f32(select, v2yx, v2wz);
			float32x2_t const v3zw = vget_high_f32(Third);
			float32x2_t const v3wz = vrev64_f32(v3zw);
			float32x2_t const v3xy = vget_low_f32(Third);
			float32x2_t const v3wy = vbsl_f32(select, v3wz, v3xy);
			float32x4_t Temp1 = vcombine_f32(v2zw, v2yz);
			float32x4_t Temp2 = vcombine_f32(v3wz, v3wy);
			SIMDRegister Result = vmulq_f32(Temp1, Temp2);
			float32x2_t const v2wy = vbsl_f32(select, v2wz, v2xy);
			float32x2_t const v3yx = vrev64_f32(v3xy);
			float32x2_t const v3yz = vbsl_f32(select, v3yx, v3wz);
			Temp1 = vcombine_f32(v2wz, v2wy);
			Temp2 = vcombine_f32(v3zw, v3yz);
			Result = vmlsq_f32(Result, Temp1, Temp2);
			float32x2_t const v1xy = vget_low_f32(First);
			float32x2_t const v1yx = vrev64_f32(v1xy);
			Temp1 = vcombine_f32(v1yx, vdup_lane_f32(v1yx, 1));
			Result = vmulq_f32(Result, Temp1);
			float32x2_t const v2yw = vrev64_f32(v2wy);
			float32x2_t const v2xz = vbsl_f32(select, v2xy, v2wz);
			float32x2_t const v3wx = vbsl_f32(select, v3wz, v3yx);
			Temp1 = vcombine_f32(v2yw, v2xz);
			Temp2 = vcombine_f32(v3wx, v3wx);
			float32x4_t vTerm = vmulq_f32(Temp1, Temp2);
			float32x2_t const v2wx = vbsl_f32(select, v2wz, v2yx);
			float32x2_t const v3yw = vrev64_f32(v3wy);
			float32x2_t const v3xz = vbsl_f32(select, v3xy, v3wz);
			Temp1 = vcombine_f32(v2wx, v2wx);
			Temp2 = vcombine_f32(v3yw, v3xz);
			vTerm = vmlsq_f32(vTerm, Temp1, Temp2);
			float32x2_t const v1zw = vget_high_f32(First);
			Temp1 = vcombine_f32(vdup_lane_f32(v1zw, 0), vdup_lane_f32(v1yx, 0));
			Result = vmlsq_f32(Result, vTerm, Temp1);
			float32x2_t const v3zx = vrev64_f32(v3xz);
			Temp1 = vcombine_f32(v2yz, v2xy);
			Temp2 = vcombine_f32(v3zx, v3yx);
			vTerm = vmulq_f32(Temp1, Temp2);
			float32x2_t const v2zx = vrev64_f32(v2xz);
			Temp1 = vcombine_f32(v2zx, v2yx);
			Temp2 = vcombine_f32(v3yz, v3xy);
			vTerm = vmlsq_f32(vTerm, Temp1, Temp2);
			float32x2_t const v1wz = vrev64_f32(v1zw);
			Temp1 = vcombine_f32(vdup_lane_f32(v1wz, 0), v1wz);
			return vmlaq_f32(Result, vTerm, Temp1);
		}
	};	// class SIMDNEON
	typedef SIMDNEON SIMDImplementation;

GD_NAMESPACE_END

#endif	// ifndef GD_CORE_MATH_VECTOR_REGISTER_NEON
