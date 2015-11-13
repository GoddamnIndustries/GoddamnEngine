// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Math/Quaternion.h
 * Quaternion class.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Math/Math.h>
#include <GoddamnEngine/Core/Math/Vector.h>

GD_NAMESPACE_BEGIN

	// ------------------------------------------------------------------------------------------
	//! Fast four-dimensional floating-point quaternion class.
	//! All mathematical operations are implemented with the support of the SIMD instruction (if available).
	GD_ALIGN_MSVC(16)
	struct Quaternion final
	{
	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Public fields and constants.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		union {
			struct { Float32 X, Y, Z, W; };
#if GD_SIMD_DIRECTX_MATH
			DirectX::XMVECTOR Register;
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_SSE
			__m128 Register;
#endif	// if GD_SIMD_SSE
		};	// anonymous union

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructors.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Constructs an identity 4D quaternion
		GDINL Quaternion()
#if GD_SIMD_DIRECTX_MATH
			: Register(DirectX::XMVectorZero())
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_FPU
			: X(0.0f), Y(0.0f), Z(0.0f), W(1.0f)
#endif	// if GD_SIMD_FPU
#if GD_SIMD_SSE
			: Register(_mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f))
#endif	// if GD_SIMD_SSE
		{}

		// ------------------------------------------------------------------------------------------
		//! Constructs a 4D quaternion with a specified X, Y, Z and W components.
		//! @param TheX lhs quaternion component value.
		//! @param TheY rhs quaternion component value.
		//! @param TheZ Third quaternion component value.
		//! @param TheW Fourth quaternion component value.
		GDINL Quaternion(Float32 const TheX, Float32 const TheY, Float32 const TheZ, Float32 const TheW)
#if GD_SIMD_DIRECTX_MATH
			: Register(DirectX::XMVectorSet(TheX, TheY, TheZ, TheW))
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_FPU
			: X(TheX), Y(TheY), Z(TheZ), W(TheW)
#endif	// if GD_SIMD_FPU
#if GD_SIMD_SSE
			: Register(_mm_set_ps(TheX, TheY, TheZ, TheW))
#endif	// if GD_SIMD_SSE
		{}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Unary 4D quaternion operations.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		// Components access operator.
		//! @{
		GDINL Float32 const& GD_VECTORCALL operator()(SizeTp const Index) const
		{
			GD_DEBUG_ASSERT(Index < 4, "index out of range.");
			return *(&this->X + Index);
		}
		GDINL Float32& GD_VECTORCALL operator()(SizeTp const Index)
		{
			return const_cast<Float32&>(const_cast<Quaternion const&>(*this)(Index));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Constructs a 4D quaternion with the Euler angles.
		//! @param EulerRadians Euler angles to convert to quaternions.
		GDINL static Quaternion GD_VECTORCALL FromEulerRadians(Vector3Fast const EulerRadians)
		{
			Quaternion Result = {};
#if GD_SIMD_DIRECTX_MATH
			Result.Register = DirectX::XMQuaternionRotationRollPitchYawFromVector(EulerRadians.Register);
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_FPU
			auto const CosZ = Math::Cos(EulerRadians.Z * 0.5f), CosY = Math::Cos(EulerRadians.Y * 0.5f), CosX = Math::Cos(EulerRadians.X * 0.5f);
			auto const SinZ = Math::Sin(EulerRadians.Z * 0.5f), SinY = Math::Sin(EulerRadians.Y * 0.5f), SinX = Math::Sin(EulerRadians.X * 0.5f);
			Result.X = SinX * CosY * CosZ - CosX * SinY * SinZ;
			Result.Y = CosX * SinY * CosZ + SinX * CosY * SinZ;
			Result.Z = CosX * CosY * SinZ - SinX * SinY * CosZ;
			Result.W = CosX * CosY * CosZ + SinX * SinY * SinZ;
#endif	// if GD_SIMD_FPU
#if GD_SIMD_SSE
#	error 'Not implemented.'
#endif	// if GD_SIMD_SSE
			return Result;
		}

	} GD_ALIGN_GCC(16);	// struct Quaternion

GD_NAMESPACE_END
