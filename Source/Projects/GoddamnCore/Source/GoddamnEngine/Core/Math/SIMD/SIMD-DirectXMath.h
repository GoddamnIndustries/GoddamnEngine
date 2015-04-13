//! ==========================================================================================
//! SIMDRegister-DirectXMath.h: SIMD register intrinsics DirectXMath implementation header.
//! Copyright (C) Goddamn Industries 2015. All Rights Reserved.
//! ==========================================================================================

#pragma once
#ifndef GD_CORE_MATH_VECTOR_REGISTER_DIRECTXMATH
#define GD_CORE_MATH_VECTOR_REGISTER_DIRECTXMATH

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Math/SIMD/SIMD.h>

#include <DirectXMath.h>
#include <DirectXPackedVector.h>

GD_NAMESPACE_BEGIN

	//! @brief 4D Vector type stored in single register.
	typedef DirectX::XMVECTOR SIMDRegisterDirectXMath;

	//! @brief SIMD register math DirectXMath implementation.
	class SIMDDirectXMath : public SIMDGeneric<SIMDRegisterDirectXMath>
	{
	public:
		//! @brief Creates a new SIMD register based on four floats.
		//! @param X First component of the SIMD register.
		//! @param Y Second component of the SIMD register.
		//! @param Z Third component of the SIMD register.
		//! @param W Fourth component of the SIMD register.
		//! @returns Created vector.
		GDINL static SIMDRegister GD_VECTORCALL Create(Float32 const X, Float32 const Y, Float32 const Z, Float32 const W)
		{
			return DirectX::XMVectorSet(X, Y, Z, W);
		}

		//! @brief Creates a new SIMD register based on bitwise equivalent of four integers.
		//! @param X First component of the SIMD register bitwise value.
		//! @param Y Second component of the SIMD register bitwise value.
		//! @param Z Third component of the SIMD register bitwise value.
		//! @param W Fourth component of the SIMD register bitwise value.
		//! @returns Created vector.
		GDINL static SIMDRegister GD_VECTORCALL CreateInt(UInt32 const X, UInt32 const Y, UInt32 const Z, UInt32 const W)
		{
			return DirectX::XMVectorSetInt(X, Y, Z, W);
		}

		//! @brief Gets a single component of a vector.
		//! @param Register Specified register.
		//! @tparam ElementIndex Specified index of the element.
		//! @returns Specified single component of a vector.
		//! @{
		template<UInt32 const ElementIndex>
		GDINL static Float32 GetComponent(SIMDRegister const Register);
		template<>
		GDINL static Float32 GetComponent<0>(SIMDRegister const Register)
		{
			return DirectX::XMVectorGetX(Register);
		}
		template<>
		GDINL static Float32 GetComponent<1>(SIMDRegister const Register)
		{
			return DirectX::XMVectorGetY(Register);
		}
		template<>
		GDINL static Float32 GetComponent<2>(SIMDRegister const Register)
		{
			return DirectX::XMVectorGetZ(Register);
		}
		template<>
		GDINL static Float32 GetComponent<3>(SIMDRegister const Register)
		{
			return DirectX::XMVectorGetW(Register);
		}
		//! @}

		//! @brief Sets a single component of a vector.
		//! @param Register Specified register.
		//! @param Value Value of the component to be set.
		//! @tparam ElementIndex Specified index of the element.
		//! @returns Modified SIMD register.
		//! @{
		template<UInt32 const ElementIndex>
		GDINL static SIMDRegister SetComponent(SIMDRegister const Register, Float32 const Value);
		template<>
		GDINL static SIMDRegister SetComponent<0>(SIMDRegister const Register, Float32 const Value)
		{
			return DirectX::XMVectorSetX(Register, Value);
		}
		template<>
		GDINL static SIMDRegister SetComponent<1>(SIMDRegister const Register, Float32 const Value)
		{
			return DirectX::XMVectorSetY(Register, Value);
		}
		template<>
		GDINL static SIMDRegister SetComponent<2>(SIMDRegister const Register, Float32 const Value)
		{
			return DirectX::XMVectorSetZ(Register, Value);
		}
		template<>
		GDINL static SIMDRegister SetComponent<3>(SIMDRegister const Register, Float32 const Value)
		{
			return DirectX::XMVectorSetW(Register, Value);
		}
		//! @}

		//! @brief Creates a vector through selecting two components from each vector via a shuffle mask. 
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
			static_assert(X <= 7, "Invalid X index.");
			static_assert(Y <= 7, "Invalid Y index.");
			static_assert(Z <= 7, "Invalid Z index.");
			static_assert(W <= 7, "Invalid W index.");
			return DirectX::XMVectorPermute<X, Y, Z, W>(First, Second);
		}

		//! @brief Returns swizzling result of all components of a specified vector.
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
			return DirectX::XMVectorSwizzle<X, Y, Z + 4, W + 4>(Register);
		}

		//! @brief Returns new register with all components assigned with specified component of a specified register.
		//! @param Register Specified register.
		//! @tparam ElementIndex Specified index of the element.
		template<UInt32 const ElementIndex>
		GDINL static SIMDRegister GD_VECTORCALL Replicate(SIMDRegister const Register)
		{
			static_assert(ElementIndex <= 3, "Invalid element index.");
			return DirectX::XMVectorSwizzle<ElementIndex, ElementIndex, ElementIndex, ElementIndex>(Register);
		}
	
		//! @brief Returns component-wise absolute value of the register.
		//! @param Register Specified register.
		//! @returns Component-wise negated value of the register.
		GDINL static SIMDRegister GD_VECTORCALL Abs(SIMDRegister const Register)
		{
			return DirectX::XMVectorAbs(Register);
		}

		//! @brief Performers bitwise OR for two registers.
		//! @param First First specified register.
		//! @param Second Second specified register.
		//! @returns Bitwise OR for two registers.
		GDINL static SIMDRegister GD_VECTORCALL BitwiseOr(SIMDRegister const First, SIMDRegister const Second)
		{
			return DirectX::XMVectorOrInt(First, Second);
		}

		//! @brief Performers bitwise XOR for two registers.
		//! @param First First specified register.
		//! @param Second Second specified register.
		//! @returns Bitwise XOR for two registers.
		GDINL static SIMDRegister GD_VECTORCALL BitwiseXor(SIMDRegister const First, SIMDRegister const Second)
		{
			return DirectX::XMVectorXorInt(First, Second);
		}

		//! @brief Performers bitwise AND for two registers.
		//! @param First First specified register.
		//! @param Second Second specified register.
		//! @returns Bitwise AND for two registers.
		GDINL static SIMDRegister GD_VECTORCALL BitwiseAnd(SIMDRegister const First, SIMDRegister const Second)
		{
			return DirectX::XMVectorAndInt(First, Second);
		}

		//! @brief Returns component-wise negated value of the register.
		//! @param Register Specified register.
		//! @returns Component-wise negated value of the register.
		GDINL static SIMDRegister GD_VECTORCALL Negate(SIMDRegister const Register)
		{
			return DirectX::XMVectorNegate(Register);
		}

		//! @brief Adds two registers and returns a result.
		//! @param First First specified register.
		//! @param Second Second specified register.
		//! @returns Per-component sum of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Add(SIMDRegister const First, SIMDRegister const Second)
		{
			return DirectX::XMVectorAdd(First, Second);
		}

		//! @brief Subtracts two registers and returns a result.
		//! @param First First specified register.
		//! @param Second Second specified register.
		//! @returns Per-component subtraction product of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Sub(SIMDRegister const First, SIMDRegister const Second)
		{
			return DirectX::XMVectorSubtract(First, Second);
		}

		//! @brief Multiplies two registers and returns a result.
		//! @param First First specified register.
		//! @param Second Second specified register.
		//! @returns Per-component multiplication product of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Mul(SIMDRegister const First, SIMDRegister const Second)
		{
			return DirectX::XMVectorMultiply(First, Second);
		}

		//! @brief Divides two registers and returns a result.
		//! @param First First specified register.
		//! @param Second Second specified register.
		//! @returns Per-component division product of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Div(SIMDRegister const First, SIMDRegister const Second)
		{
			return DirectX::XMVectorDivide(First, Second);
		}

		//! @brief Computes dot product of two SIMD registers, treated as 3D vectors..
		//! @param First First specified register.
		//! @param Second Second specified register.
		//! @returns Dot product of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Dot3(SIMDRegister const First, SIMDRegister const Second)
		{
			return DirectX::XMVector3Dot(First, Second);
		}

		//! @brief Computes dot product of two SIMD registers, treated as 4D vectors..
		//! @param First First specified register.
		//! @param Second Second specified register.
		//! @returns Dot product of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Dot4(SIMDRegister const First, SIMDRegister const Second)
		{
			return DirectX::XMVector4Dot(First, Second);
		}

		//! @brief Computes cross product of two SIMD registers, treated as 3D vectors..
		//! @param First First specified register.
		//! @param Second Second specified register.
		//! @returns Cross product of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Cross3(SIMDRegister const First, SIMDRegister const Second)
		{
			return DirectX::XMVector3Cross(First, Second);
		}

		//! @brief Computes cross product of three SIMD registers, treated as 4D vectors..
		//! @param First First specified register.
		//! @param Second Second specified register.
		//! @param Third Third specified register.
		//! @returns Cross product of three registers.
		GDINL static SIMDRegister GD_VECTORCALL Cross4(SIMDRegister const First, SIMDRegister const Second, SIMDRegister const Third)
		{
			return DirectX::XMVector4Cross(First, Second, Third);
		}
	};	// class SIMDDirectXMath
	typedef SIMDDirectXMath SIMDImplementation;

GD_NAMESPACE_END

#endif	// ifndef GD_CORE_MATH_VECTOR_REGISTER_DIRECTXMATH
