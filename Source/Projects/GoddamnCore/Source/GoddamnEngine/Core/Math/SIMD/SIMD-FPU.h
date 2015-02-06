/// ==========================================================================================
/// SIMDRegister-FPU.h: SIMD register intrinsics FPU implementation header.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_MATH_VECTOR_REGISTER_FPU
#define GD_CORE_MATH_VECTOR_REGISTER_FPU

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Math/SIMD/SIMD.h>

#include <cmath>

GD_NAMESPACE_BEGIN

	/// @brief 4D Vector type stored in single register.
	typedef GD_ALIGN_MSVC(4) struct { Float32 Values[4]; } GD_ALIGN_GCC(4) SIMDRegisterFPU;

	/// @brief SIMD register math FPU/generic implementation.
	class SIMDFPU : public SIMDGeneric<SIMDRegisterFPU>
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
			SIMDRegister const Register = { { X, Y, Z, W } };
			return Register;
		}

		/// @brief Creates a new SIMD register based on bitwise equivalent of four integers.
		/// @param X First component of the SIMD register bitwise value.
		/// @param Y Second component of the SIMD register bitwise value.
		/// @param Z Third component of the SIMD register bitwise value.
		/// @param W Fourth component of the SIMD register bitwise value.
		/// @returns Created vector.
		GDINL static SIMDRegister GD_VECTORCALL CreateInt(UInt32 const X, UInt32 const Y, UInt32 const Z, UInt32 const W)
		{
			SIMDRegister Register;
			reinterpret_cast<UInt32&>(Register.Values[0]) = X;
			reinterpret_cast<UInt32&>(Register.Values[1]) = Y;
			reinterpret_cast<UInt32&>(Register.Values[2]) = Z;
			reinterpret_cast<UInt32&>(Register.Values[3]) = W;
			return Register;
		}

		/// @brief Gets a single component of a vector.
		/// @param Register Specified register.
		/// @tparam ElementIndex Specified index of the element.
		/// @returns Specified single component of a vector.
		template<UInt32 const ElementIndex>
		GDINL static Float32 GetComponent(SIMDRegister const Register)
		{
			static_assert(ElementIndex <= 3, "Invalid element index.");
			return Register.Values[ElementIndex];
		}

		/// @brief Sets a single component of a vector.
		/// @param Register Specified register.
		/// @param Value Value of the component to be set.
		/// @tparam ElementIndex Specified index of the element.
		/// @returns Modified SIMD register.
		/// @{
		template<UInt32 const ElementIndex>
		GDINL static SIMDRegister SetComponent(SIMDRegister const Register, Float32 const Value);
		template<>
		GDINL static SIMDRegister SetComponent<0>(SIMDRegister const Register, Float32 const Value)
		{
			return Create(Value, Register.Values[1], Register.Values[2], Register.Values[3]);
		}
		template<>
		GDINL static SIMDRegister SetComponent<1>(SIMDRegister const Register, Float32 const Value)
		{
			return Create(Register.Values[0], Value, Register.Values[2], Register.Values[3]);
		}
		template<>
		GDINL static SIMDRegister SetComponent<2>(SIMDRegister const Register, Float32 const Value)
		{
			return Create(Register.Values[0], Register.Values[1], Value, Register.Values[3]);
		}
		template<>
		GDINL static SIMDRegister SetComponent<3>(SIMDRegister const Register, Float32 const Value)
		{
			return Create(Register.Values[0], Register.Values[1], Register.Values[2], Value);
		}
		/// @}

		/// @brief Creates a vector through selecting two components from each vector via a shuffle mask. 
		/// @param First First register to shuffle.
		/// @param Second Second register to shuffle.
		/// @tparam X Index for which component to use for X (literal 0-3).
		/// @tparam Y Index for which component to use for Y (literal 0-3).
		/// @tparam Z Index for which component to use for Z (literal 0-3).
		/// @tparam W Index for which component to use for W (literal 0-3).
		/// @returns The shuffled vector.
		template<UInt32 const X, UInt32 const Y, UInt32 const Z, UInt32 const W>
		GDINL static SIMDRegister GD_VECTORCALL Shuffle(SIMDRegister const First, SIMDRegister const Second)
		{
			static_assert(X <= 3, "Invalid X index.");
			static_assert(Y <= 3, "Invalid Y index.");
			static_assert(Z <= 3, "Invalid Z index.");
			static_assert(W <= 3, "Invalid W index.");
			return Create(First.Values[static_cast<size_t>(X)], First.Values[static_cast<size_t>(Y)], Second.Values[static_cast<size_t>(Z)], Second.Values[static_cast<size_t>(W)]);
		}

		/// @brief Returns swizzling result of all components of a specified vector.
		/// @tparam X Index for which component to use for X (literal 0-3).
		/// @tparam Y Index for which component to use for Y (literal 0-3).
		/// @tparam Z Index for which component to use for Z (literal 0-3).
		/// @tparam W Index for which component to use for W (literal 0-3).
		/// @returns The swizzled vector.
		template<UInt32 const X, UInt32 const Y, UInt32 const Z, UInt32 const W>
		GDINL static SIMDRegister GD_VECTORCALL Swizzle(SIMDRegister const Register)
		{
			static_assert(X <= 3, "Invalid X index.");
			static_assert(Y <= 3, "Invalid Y index.");
			static_assert(Z <= 3, "Invalid Z index.");
			static_assert(W <= 3, "Invalid W index.");
			return Create(Register.Values[static_cast<size_t>(X)], Register.Values[static_cast<size_t>(Y)], Register.Values[static_cast<size_t>(Z)], Register.Values[static_cast<size_t>(W)]);
		}

		/// @brief Returns new register with all components assigned with specified component of a specified register.
		/// @param Register Specified register.
		/// @tparam ElementIndex Specified index of the element.
		template<UInt32 const ElementIndex>
		GDINL static SIMDRegister GD_VECTORCALL Replicate(SIMDRegister const& Register)
		{
			Float32 const Value = Register.Values[static_cast<size_t>(ElementIndex)];
			return Create(Value, Value, Value, Value);
		}

		/// @brief Returns component-wise absolute value of the register.
		/// @param Register Specified register.
		/// @returns Component-wise negated value of the register.
		GDINL static SIMDRegister GD_VECTORCALL Abs(SIMDRegister const& Register)
		{
			return Create(std::fabsf(Register.Values[0]), std::fabsf(Register.Values[1]), std::fabsf(Register.Values[2]), std::fabsf(Register.Values[3]));
		}

		/// @brief Performers bitwise OR for two registers.
		/// @param First First specified register.
		/// @param Second Second specified register.
		/// @returns Bitwise OR for two registers.
		GDINL static SIMDRegister GD_VECTORCALL BitwiseOr(SIMDRegister const First, SIMDRegister const Second)
		{
			UInt32 const* const PtrFirst = reinterpret_cast<UInt32 const*>(&First.Values[0]);
			UInt32 const* const PtrSecond = reinterpret_cast<UInt32 const*>(&Second.Values[0]);
			return CreateInt(*(PtrFirst + 0) | *(PtrSecond + 0), *(PtrFirst + 1) | *(PtrSecond + 1), *(PtrFirst + 2) | *(PtrSecond + 2), *(PtrFirst + 3) | *(PtrSecond + 3));
		}

		/// @brief Performers bitwise XOR for two registers.
		/// @param First First specified register.
		/// @param Second Second specified register.
		/// @returns Bitwise XOR for two registers.
		GDINL static SIMDRegister GD_VECTORCALL BitwiseXor(SIMDRegister const First, SIMDRegister const Second)
		{
			UInt32 const* const PtrFirst = reinterpret_cast<UInt32 const*>(&First.Values[0]);
			UInt32 const* const PtrSecond = reinterpret_cast<UInt32 const*>(&Second.Values[0]);
			return CreateInt(*(PtrFirst + 0) ^ *(PtrSecond + 0), *(PtrFirst + 1) ^ *(PtrSecond + 1), *(PtrFirst + 2) ^ *(PtrSecond + 2), *(PtrFirst + 3) ^ *(PtrSecond + 3));
		}

		/// @brief Performers bitwise AND for two registers.
		/// @param First First specified register.
		/// @param Second Second specified register.
		/// @returns Bitwise AND for two registers.
		GDINL static SIMDRegister GD_VECTORCALL BitwiseAnd(SIMDRegister const First, SIMDRegister const Second)
		{
			UInt32 const* const PtrFirst = reinterpret_cast<UInt32 const*>(&First.Values[0]);
			UInt32 const* const PtrSecond = reinterpret_cast<UInt32 const*>(&Second.Values[0]);
			return CreateInt(*(PtrFirst + 0) & *(PtrSecond + 0), *(PtrFirst + 1) & *(PtrSecond + 1), *(PtrFirst + 2) & *(PtrSecond + 2), *(PtrFirst + 3) & *(PtrSecond + 3));
		}

		/// @brief Returns component-wise negated value of the register.
		/// @param Register Specified register.
		/// @returns Component-wise negated value of the register.
		GDINL static SIMDRegister GD_VECTORCALL Negate(SIMDRegister const& Register)
		{
			return Create(-1.0f * Register.Values[0], -1.0f * Register.Values[1], -1.0f * Register.Values[2], -1.0f * Register.Values[3]);
		}

		/// @brief Adds two registers and returns a result.
		/// @param First First specified register.
		/// @param Second Second specified register.
		/// @returns Per-component sum of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Add(SIMDRegister const First, SIMDRegister const Second)
		{
			return Create(First.Values[0] + Second.Values[0], First.Values[1] + Second.Values[1], First.Values[2] + Second.Values[2], First.Values[3] + Second.Values[3]);
		}

		/// @brief Subtracts two registers and returns a result.
		/// @param First First specified register.
		/// @param Second Second specified register.
		/// @returns Per-component subtraction product of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Sub(SIMDRegister const First, SIMDRegister const Second)
		{
			return Create(First.Values[0] - Second.Values[0], First.Values[1] - Second.Values[1], First.Values[2] - Second.Values[2], First.Values[3] - Second.Values[3]);
		}

		/// @brief Multiplies two registers and returns a result.
		/// @param First First specified register.
		/// @param Second Second specified register.
		/// @returns Per-component multiplication product of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Mul(SIMDRegister const First, SIMDRegister const Second)
		{
			return Create(First.Values[0] * Second.Values[0], First.Values[1] * Second.Values[1], First.Values[2] * Second.Values[2], First.Values[3] * Second.Values[3]);
		}

		/// @brief Divides two registers and returns a result.
		/// @param First First specified register.
		/// @param Second Second specified register.
		/// @returns Per-component division product of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Div(SIMDRegister const First, SIMDRegister const Second)
		{
			return Create(First.Values[0] / Second.Values[0], First.Values[1] / Second.Values[1], First.Values[2] / Second.Values[2], First.Values[3] / Second.Values[3]);
		}

		/// @brief Computes dot product of two SIMD registers, treated as 3D vectors..
		/// @param First First specified register.
		/// @param Second Second specified register.
		/// @returns Dot product of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Dot3(SIMDRegister const First, SIMDRegister const Second)
		{
			Float32 const DotProduct = (First.Values[0] * Second.Values[0]) + (First.Values[1] * Second.Values[1]) + (First.Values[2] * Second.Values[2]);
			return Create(DotProduct, DotProduct, DotProduct, DotProduct);
		}

		/// @brief Computes dot product of two SIMD registers, treated as 4D vectors..
		/// @param First First specified register.
		/// @param Second Second specified register.
		/// @returns Dot product of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Dot4(SIMDRegister const First, SIMDRegister const Second)
		{
			Float32 const DotProduct = (First.Values[0] * Second.Values[0]) + (First.Values[1] * Second.Values[1]) + (First.Values[2] * Second.Values[2]) + (First.Values[3] * Second.Values[3]);
			return Create(DotProduct, DotProduct, DotProduct, DotProduct);
		}

		/// @brief Computes cross product of two SIMD registers, treated as 3D vectors..
		/// @param First First specified register.
		/// @param Second Second specified register.
		/// @returns Cross product of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Cross3(SIMDRegister const First, SIMDRegister const Second)
		{
			return Create(
				(First.Values[1] * Second.Values[2]) - (First.Values[2] * Second.Values[1]),
				(First.Values[2] * Second.Values[0]) - (First.Values[0] * Second.Values[2]),
				(First.Values[0] * Second.Values[1]) - (First.Values[1] * Second.Values[0]),
				0.0f
				);
		}

		/// @brief Computes cross product of three SIMD registers, treated as 4D vectors..
		/// @param First First specified register.
		/// @param Second Second specified register.
		/// @param Third Third specified register.
		/// @returns Cross product of three registers.
		GDINL static SIMDRegister GD_VECTORCALL Cross4(SIMDRegister const First, SIMDRegister const Second, SIMDRegister const Third)
		{
			return Create(
				(((Second.Values[2] * Third.Values[3]) - (Second.Values[3] * Third.Values[2]))*First.Values[1]) - (((Second.Values[1] * Third.Values[3]) - (Second.Values[3] * Third.Values[1]))*First.Values[2]) + (((Second.Values[1] * Third.Values[2]) - (Second.Values[2] * Third.Values[1]))*First.Values[3]),
				(((Second.Values[3] * Third.Values[2]) - (Second.Values[2] * Third.Values[3]))*First.Values[0]) - (((Second.Values[3] * Third.Values[0]) - (Second.Values[0] * Third.Values[3]))*First.Values[2]) + (((Second.Values[2] * Third.Values[0]) - (Second.Values[0] * Third.Values[2]))*First.Values[3]),
				(((Second.Values[1] * Third.Values[3]) - (Second.Values[3] * Third.Values[1]))*First.Values[0]) - (((Second.Values[0] * Third.Values[3]) - (Second.Values[3] * Third.Values[0]))*First.Values[1]) + (((Second.Values[0] * Third.Values[1]) - (Second.Values[1] * Third.Values[0]))*First.Values[3]),
				(((Second.Values[2] * Third.Values[1]) - (Second.Values[1] * Third.Values[2]))*First.Values[0]) - (((Second.Values[2] * Third.Values[0]) - (Second.Values[0] * Third.Values[2]))*First.Values[1]) + (((Second.Values[1] * Third.Values[0]) - (Second.Values[0] * Third.Values[1]))*First.Values[2])
				);
		}
	};	// class SIMDFPU
	typedef SIMDFPU SIMDImplementation;

GD_NAMESPACE_END

#endif	// ifndef GD_CORE_MATH_VECTOR_REGISTER_FPU
