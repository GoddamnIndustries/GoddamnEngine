/// ==========================================================================================
/// SIMDRegister.h: SIMD register intrinsics common header.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_MATH_VECTOR_REGISTER
#define GD_CORE_MATH_VECTOR_REGISTER

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/TypeTraits.h>

GD_NAMESPACE_BEGIN

	/// @brief Basic SIMD register math generic implementation. Do not use directly.
	/// @tparam SIMDRegisterType Type of a SIMD register.
	template<typename SIMDRegisterType>
	class SIMDGeneric
	{
	public:
		/// @brief Type this CPU uses as a SIMD register.
		typedef SIMDRegisterType SIMDRegister;
		
		/// @brief Creates a new SIMD register based on four floats.
		/// @param X First component of the SIMD register.
		/// @param Y Second component of the SIMD register.
		/// @param Z Third component of the SIMD register.
		/// @param W Fourth component of the SIMD register.
		/// @returns Created vector.
		GDINL static SIMDRegister GD_VECTORCALL Create(Float32 const X, Float32 const Y, Float32 const Z, Float32 const W)
		{
			GD_NOT_USED(X);
			GD_NOT_USED(Y);
			GD_NOT_USED(Z);
			GD_NOT_USED(W);
			GD_NOT_IMPLEMENTED();
		}

		/// @brief Creates a new SIMD register based on bitwise equivalent of four integers.
		/// @param X First component of the SIMD register bitwise value.
		/// @param Y Second component of the SIMD register bitwise value.
		/// @param Z Third component of the SIMD register bitwise value.
		/// @param W Fourth component of the SIMD register bitwise value.
		/// @returns Created vector.
		GDINL static SIMDRegister GD_VECTORCALL CreateInt(UInt32 const X, UInt32 const Y, UInt32 const Z, UInt32 const W)
		{
			GD_NOT_USED(X);
			GD_NOT_USED(Y);
			GD_NOT_USED(Z);
			GD_NOT_USED(W);
			GD_NOT_IMPLEMENTED();
		}

		/// @brief Gets a single component of a vector.
		/// @param Register Specified register.
		/// @tparam ElementIndex Specified index of the element.
		/// @returns Specified single component of a vector.
		template<UInt32 const ElementIndex>
		GDINL static Float32 GetComponent(SIMDRegister const Register)
		{
			static_assert(ElementIndex <= 3, "Invalid element index.");
			GD_NOT_USED(Register);
			GD_NOT_IMPLEMENTED();
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
			GD_NOT_USED(Register);
			GD_NOT_IMPLEMENTED();
		}

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
			GD_NOT_USED(First);
			GD_NOT_USED(Second);
			GD_NOT_IMPLEMENTED();
		}

		/// @brief Returns swizzling result of all components of a specified vector.
		/// @param Register Specified register.
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
			GD_NOT_USED(Register);
			GD_NOT_IMPLEMENTED();
		}

		/// @brief Returns new register with all components assigned with specified component of a specified register.
		/// @param Register Specified register.
		/// @tparam ElementIndex Specified index of the element.
		template<UInt32 const ElementIndex>
		GDINL static SIMDRegister GD_VECTORCALL Replicate(SIMDRegister const Register)
		{
			static_assert(ElementIndex <= 3, "Invalid element index.");
			GD_NOT_USED(Register);
			GD_NOT_IMPLEMENTED();
		}

		/// @brief Returns component-wise absolute value of the register.
		/// @param Register Specified register.
		/// @returns Component-wise negated value of the register.
		GDINL static SIMDRegister GD_VECTORCALL Abs(SIMDRegister const Register)
		{
			GD_NOT_USED(Register);
			GD_NOT_IMPLEMENTED();
		}

		/// @brief Performers bitwise OR for two registers.
		/// @param First First specified register.
		/// @param Second Second specified register.
		/// @returns Bitwise OR for two registers.
		GDINL static SIMDRegister GD_VECTORCALL BitwiseOr(SIMDRegister const First, SIMDRegister const Second)
		{
			GD_NOT_USED(First);
			GD_NOT_USED(Second);
			GD_NOT_IMPLEMENTED();
		}

		/// @brief Performers bitwise XOR for two registers.
		/// @param First First specified register.
		/// @param Second Second specified register.
		/// @returns Bitwise XOR for two registers.
		GDINL static SIMDRegister GD_VECTORCALL BitwiseXor(SIMDRegister const First, SIMDRegister const Second)
		{
			GD_NOT_USED(First);
			GD_NOT_USED(Second);
			GD_NOT_IMPLEMENTED();
		}

		/// @brief Performers bitwise AND for two registers.
		/// @param First First specified register.
		/// @param Second Second specified register.
		/// @returns Bitwise AND for two registers.
		GDINL static SIMDRegister GD_VECTORCALL BitwiseAnd(SIMDRegister const First, SIMDRegister const Second)
		{
			GD_NOT_USED(First);
			GD_NOT_USED(Second);
			GD_NOT_IMPLEMENTED();
		}

		/// @brief Returns component-wise negated value of the register.
		/// @param Register Specified register.
		/// @returns Component-wise negated value of the register.
		GDINL static SIMDRegister GD_VECTORCALL Negate(SIMDRegister const Register)
		{
			GD_NOT_USED(Register);
			GD_NOT_IMPLEMENTED();
		}

		/// @brief Adds two registers and returns a result.
		/// @param First First specified register.
		/// @param Second Second specified register.
		/// @returns Per-component sum of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Add(SIMDRegister const First, SIMDRegister const Second)
		{
			GD_NOT_USED(First);
			GD_NOT_USED(Second);
			GD_NOT_IMPLEMENTED();
		}

		/// @brief Subtracts two registers and returns a result.
		/// @param First First specified register.
		/// @param Second Second specified register.
		/// @returns Per-component subtraction product of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Sub(SIMDRegister const First, SIMDRegister const Second)
		{
			GD_NOT_USED(First);
			GD_NOT_USED(Second);
			GD_NOT_IMPLEMENTED();
		}

		/// @brief Multiplies two registers and returns a result.
		/// @param First First specified register.
		/// @param Second Second specified register.
		/// @returns Per-component multiplication product of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Mul(SIMDRegister const First, SIMDRegister const Second)
		{
			GD_NOT_USED(First);
			GD_NOT_USED(Second);
			GD_NOT_IMPLEMENTED();
		}

		/// @brief Divides two registers and returns a result.
		/// @param First First specified register.
		/// @param Second Second specified register.
		/// @returns Per-component division product of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Div(SIMDRegister const First, SIMDRegister const Second)
		{
			GD_NOT_USED(First);
			GD_NOT_USED(Second);
			GD_NOT_IMPLEMENTED();
		}

		/// @brief Computes dot product of two SIMD registers, treated as 3D vectors..
		/// @param First First specified register.
		/// @param Second Second specified register.
		/// @returns Dot product of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Dot3(SIMDRegister const First, SIMDRegister const Second)
		{
			GD_NOT_USED(First);
			GD_NOT_USED(Second);
			GD_NOT_IMPLEMENTED();
		}

		/// @brief Computes dot product of two SIMD registers, treated as 4D vectors..
		/// @param First First specified register.
		/// @param Second Second specified register.
		/// @returns Dot product of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Dot4(SIMDRegister const First, SIMDRegister const Second)
		{
			GD_NOT_USED(First);
			GD_NOT_USED(Second);
			GD_NOT_IMPLEMENTED();
		}

		/// @brief Computes cross product of two SIMD registers, treated as 3D vectors..
		/// @param First First specified register.
		/// @param Second Second specified register.
		/// @returns Cross product of two registers.
		GDINL static SIMDRegister GD_VECTORCALL Cross3(SIMDRegister const First, SIMDRegister const Second)
		{
			GD_NOT_USED(First);
			GD_NOT_USED(Second);
			GD_NOT_IMPLEMENTED();
		}

		/// @brief Computes cross product of three SIMD registers, treated as 4D vectors..
		/// @param First First specified register.
		/// @param Second Second specified register.
		/// @param Third Third specified register.
		/// @returns Cross product of three registers.
		GDINL static SIMDRegister GD_VECTORCALL Cross4(SIMDRegister const First, SIMDRegister const Second, SIMDRegister const Third)
		{
			GD_NOT_USED(First);
			GD_NOT_USED(Second);
			GD_NOT_IMPLEMENTED();
		}
	};	// class SIMDGeneric<T>

GD_NAMESPACE_END

/// @todo Determine best conditions for DirectX-driven math.
#if (!defined(GD_DOCUMENTATION))
#	if GD_PLATFORM_API_WINAPI && (!defined(__cplusplus_cli)) && 0 
#		include <GoddamnEngine/Core/Math/SIMD/SIMD-DirectXMath.h>
#	else	// if GD_PLATFORM_API_WINAPI
#		if GD_ARCHITECTURE_ARM32 || GD_ARCHITECTURE_ARM64
#			include <GoddamnEngine/Core/Math/SIMD/SIMD-NEON.h>
#		elif GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_X86	// if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_X86
#			include <GoddamnEngine/Core/Math/SIMD/SIMD-SSE.h>
#		else // if GD_ARCHITECTURE_ARM32 || GD_ARCHITECTURE_ARM64
#			include <GoddamnEngine/Core/Math/SIMD/SIMD-FPU.h>
#		endif	// GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_X86 or GD_ARCHITECTURE_ARM32 || GD_ARCHITECTURE_ARM64
#	endif	// if GD_PLATFORM_API_WINAPI
#else	// if (!defined(GD_DOCUMENTATION))
#	define SIMDImplementation SIMDGeneric
#endif	// if (!defined(GD_DOCUMENTATION))

GD_NAMESPACE_BEGIN

	/// @brief Type this CPU uses as a SIMD register.
	typedef typename SIMDImplementation::SIMDRegister SIMDRegister;

	/// @brief SIMD register math implementation.
	class SIMD final : public SIMDImplementation
	{
		static_assert(TypeTraits::IsBase<SIMDGeneric<SIMDRegister>, SIMDImplementation>::Value, "Invalid inheritance for SIMD registers implementation class.");
	
	public:

	};	// class SIMD

GD_NAMESPACE_END

#endif	// ifndef GD_CORE_MATH_VECTOR_REGISTER
