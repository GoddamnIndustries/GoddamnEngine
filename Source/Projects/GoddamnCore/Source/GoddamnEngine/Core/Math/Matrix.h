// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Math/Matrix.h
 * 4x4 matrix class.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Math/Math.h>
#include <GoddamnEngine/Core/Math/Vector.h>
#include <GoddamnEngine/Core/Math/Quaternion.h>

GD_NAMESPACE_BEGIN

	
	// ==========================================================================================
	// Matrix4x4 struct.
	// ==========================================================================================
	
	// ------------------------------------------------------------------------------------------
	//! Fast four-x-four floating-point matrix class.
	//! All mathematical operations are implemented with the support of the SIMD instruction (if available).
	GD_ALIGN_MSVC(64)
	struct Matrix4x4 final
	{
	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Public fields and constants.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		Matrix4x4 static const Zero, Identity;

		union {
			Vector4 Rows[4];
			Float32 M[4][4];
			struct {
				Float32 M00, M10, M20, M30;
				Float32 M01, M11, M21, M31;
				Float32 M02, M12, M22, M32;
				Float32 M03, M13, M23, M33;
			};	// anonymous struct
#if GD_SIMD_DIRECTX_MATH
			DirectX::XMMATRIX Registers;
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_SSE	// GD_SIMD_DIRECTX_MATH
			__m128 Registers[4];
#endif	// if GD_SIMD_SSE
		};	// anonymous union

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructors.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Constructs a 3D vector with all components replicated to zero.
		GDINL Matrix4x4()
#if GD_SIMD_DIRECTX_MATH
			: Registers(DirectX::XMVectorZero(), DirectX::XMVectorZero(), DirectX::XMVectorZero(), DirectX::XMVectorZero())
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_FPU
			: Rows{ Vector4(), Vector4(), Vector4(), Vector4() }
#endif	// if GD_SIMD_FPU
#if GD_SIMD_SSE	// GD_SIMD_DIRECTX_MATH
			: Registers{ _mm_setzero_ps(), _mm_setzero_ps(), _mm_setzero_ps(), _mm_setzero_ps() }
#endif	// if GD_SIMD_SSE
		{}

		// ------------------------------------------------------------------------------------------
		//! Constructs a 4x4 matrix with diagonal set to the specified value.
		//! @param TheValue Value to replicate through the diagonal.
		GDINL explicit Matrix4x4(Float32 const TheValue)
#if GD_SIMD_DIRECTX_MATH
			: Registers(DirectX::XMMatrixIdentity() * TheValue)
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_FPU
			: Rows{
				Vector4(TheValue, 0.0f, 0.0f, 0.0f), 
				Vector4(0.0f, TheValue, 0.0f, 0.0f),
				Vector4(0.0f, 0.0f, TheValue, 0.0f), 
				Vector4(0.0f, 0.0f, 0.0f, TheValue) }
#endif	// if GD_SIMD_FPU
#if GD_SIMD_SSE
			: Registers{ 
				_mm_set_ps(TheValue, 0.0f, 0.0f, 0.0f), 
				_mm_set_ps(0.0f, TheValue, 0.0f, 0.0f), 
				_mm_set_ps(0.0f, 0.0f, TheValue, 0.0f), 
				_mm_set_ps(0.0f, 0.0f, 0.0f, TheValue) }
#endif	// if GD_SIMD_SSE
		{}

		// ------------------------------------------------------------------------------------------
		//! Constructs a 4x4 matrix with a pointer to matrix data
		//! @param TheValue Value to replicate through the diagonal.
		GDINL explicit Matrix4x4(Float32 const* const TheData)
#if GD_SIMD_DIRECTX_MATH
			: Registers(TheData) {}
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_FPU
		{ CMemory::Memcpy(this, sizeof(*this), TheData); }
#endif	// if GD_SIMD_FPU
#if GD_SIMD_SSE
			: Registers{
				_mm_load_ps(TheData + 0 * sizeof(Vector4)),
				_mm_load_ps(TheData + 1 * sizeof(Vector4)),
				_mm_load_ps(TheData + 2 * sizeof(Vector4)),
				_mm_load_ps(TheData + 3 * sizeof(Vector4)) } {}
#endif	// if GD_SIMD_SSE

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Unary 4x4 matrix operations.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		// Components access operator.
		//! @{
		GDINL Float32 const& GD_VECTORCALL operator()(SizeTp const IndexRow, SizeTp const IndexColumn) const
		{
			GD_DEBUG_ASSERT(IndexRow < 4, "Row index out of range.");
			GD_DEBUG_ASSERT(IndexColumn < 4, "Column index out of range.");
			return M[IndexRow][IndexColumn];
		}
		GDINL Float32& GD_VECTORCALL operator()(SizeTp const IndexRow, SizeTp const IndexColumn)
		{
			return const_cast<Float32&>(const_cast<Matrix4x4 const&>(*this)(IndexRow, IndexColumn));
		}
		//! @}

#if GD_SIMD_DIRECTX_MATH
		GDINL Matrix4x4& GD_VECTORCALL operator= (Matrix4x4 const& other)
		{
			Registers = other.Registers;
			return *this;
		}
#endif	// if GD_SIMD_DIRECTX_MATH

		// ------------------------------------------------------------------------------------------
		//! Transposes this matrix.
		//! @returns Transposed version of this matrix.
		GDINL Matrix4x4 GD_VECTORCALL Transpose() const
		{
			Matrix4x4 Result = {};
			
#if GD_SIMD_DIRECTX_MATH
			Result.Registers = DirectX::XMMatrixTranspose(Registers);
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_FPU
			UnrolledForIndex<4>([&](SizeTp const I) {
				UnrolledForIndex<4>([&](SizeTp const J) {
					Result(I, J) = (*this)(J, I);
				});
			});
#endif	// if GD_SIMD_FPU
#if GD_SIMD_SSE	// GD_SIMD_DIRECTX_MATH
			__m128 const Temp1 = _mm_shuffle_ps(Registers[0], Registers[1], _MM_SHUFFLE(1, 0, 1, 0));
			__m128 const Temp3 = _mm_shuffle_ps(Registers[0], Registers[1], _MM_SHUFFLE(3, 2, 3, 2));
			__m128 const Temp2 = _mm_shuffle_ps(Registers[2], Registers[3], _MM_SHUFFLE(1, 0, 1, 0));
			__m128 const Temp4 = _mm_shuffle_ps(Registers[2], Registers[3], _MM_SHUFFLE(3, 2, 3, 2));
			Result.Registers[0] = _mm_shuffle_ps(Temp1, Temp2, _MM_SHUFFLE(2, 0, 2, 0));
			Result.Registers[1] = _mm_shuffle_ps(Temp1, Temp2, _MM_SHUFFLE(3, 1, 3, 1));
			Result.Registers[2] = _mm_shuffle_ps(Temp3, Temp4, _MM_SHUFFLE(2, 0, 2, 0));
			Result.Registers[3] = _mm_shuffle_ps(Temp3, Temp4, _MM_SHUFFLE(3, 1, 3, 1));
#endif	// if GD_SIMD_SSE
			return Result;
		}

		// ------------------------------------------------------------------------------------------
		//! Inverses this matrix.
		//! @param DeterminantPtr Optional pointer to the determinant.
		//! @returns Inversed version of this matrix.
		GDINL Matrix4x4 GD_VECTORCALL Inverse(Float32* const DeterminantPtr = nullptr) const
		{
			Matrix4x4 Result = {};
#if GD_SIMD_DIRECTX_MATH
			DirectX::XMVECTOR Determinant;
			Result.Registers = DirectX::XMMatrixInverse(DeterminantPtr != nullptr ? &Determinant : nullptr, Registers);
			if (DeterminantPtr != nullptr)
				*DeterminantPtr = DirectX::XMVectorGetX(Determinant);
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_FPU
			Float32 const Coef00 = M22 * M33 - M32 * M23, Coef02 = M12 * M33 - M32 * M13, Coef03 = M12 * M23 - M22 * M13, Coef04 = M21 * M33 - M31 * M23;
			Float32 const Coef06 = M11 * M33 - M31 * M13, Coef07 = M11 * M23 - M21 * M13, Coef08 = M21 * M32 - M31 * M22, Coef10 = M11 * M32 - M31 * M12;
			Float32 const Coef11 = M11 * M22 - M21 * M12, Coef12 = M20 * M33 - M30 * M23, Coef14 = M10 * M33 - M30 * M13, Coef15 = M10 * M23 - M20 * M13;
			Float32 const Coef16 = M20 * M32 - M30 * M22, Coef18 = M10 * M32 - M30 * M12, Coef19 = M10 * M22 - M20 * M12, Coef20 = M20 * M31 - M30 * M21;
			Float32 const Coef22 = M10 * M31 - M30 * M11, Coef23 = M10 * M21 - M20 * M11;
			Vector4 const Fac0(Coef00, Coef00, Coef02, Coef03), Fac1(Coef04, Coef04, Coef06, Coef07), Fac2(Coef08, Coef08, Coef10, Coef11);
			Vector4 const Fac3(Coef12, Coef12, Coef14, Coef15), Fac4(Coef16, Coef16, Coef18, Coef19), Fac5(Coef20, Coef20, Coef22, Coef23);
			Vector4 const Vec0(M10, M00, M00, M00), Vec1(M11, M01, M01, M01);
			Vector4 const Vec2(M12, M02, M02, M02), Vec3(M13, M03, M03, M03);
			Vector4 const static SignA(+1.0f, -1.0f, +1.0f, -1.0f);
			Vector4 const static SignB(-1.0f, +1.0f, -1.0f, +1.0f);
			Result.Rows[0] = SignA * (Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
			Result.Rows[1] = SignB * (Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
			Result.Rows[2] = SignA * (Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
			Result.Rows[3] = SignB * (Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);
			Vector4 const Row0(Result.M00, Result.M10, Result.M20, Result.M30);
			auto Determinant = Vector4::DotProduct(Result.Rows[0], Row0);
			Result *= 1.0f / Determinant;
			if (DeterminantPtr != nullptr)
				*DeterminantPtr = Determinant;
#endif	// if GD_SIMD_FPU
#if GD_SIMD_SSE	// GD_SIMD_DIRECTX_MATH 
#	error 'Not implemented.'
#endif	// if GD_SIMD_SSE
			return Result;
		}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Binary 4x4 matrix operations.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		// Compares two matrices.
		//! @{
		GDINL bool GD_VECTORCALL operator== (Matrix4x4 const& other) const
		{
			return Rows[0] == other.Rows[0] && Rows[1] == other.Rows[1] && Rows[2] == other.Rows[2] && Rows[3] == other.Rows[3];
		}
		GDINL bool GD_VECTORCALL operator!= (Matrix4x4 const& other)
		{
			return Rows[0] != other.Rows[0] || Rows[1] != other.Rows[1] || Rows[2] != other.Rows[2] || Rows[3] != other.Rows[3];
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// Adds two matrices.
		//! @{
		GDINL Matrix4x4 GD_VECTORCALL operator+ (Matrix4x4 const& other) const
		{
			Matrix4x4 Result = {};
			Result.Rows[0] = Rows[0] + other.Rows[0];
			Result.Rows[1] = Rows[1] + other.Rows[1];
			Result.Rows[2] = Rows[2] + other.Rows[2];
			Result.Rows[3] = Rows[3] + other.Rows[3];
			return Result;
		}
		GDINL Matrix4x4& GD_VECTORCALL operator+= (Matrix4x4 const& other)
		{
			return *this = *this + other;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// Subtracts two matrices.
		//! @{
		GDINL Matrix4x4 GD_VECTORCALL operator- (Matrix4x4 const& other) const
		{
			Matrix4x4 Result = {};
			Result.Rows[0] = Rows[0] - other.Rows[0];
			Result.Rows[1] = Rows[1] - other.Rows[1];
			Result.Rows[2] = Rows[2] - other.Rows[2];
			Result.Rows[3] = Rows[3] - other.Rows[3];
			return Result;
		}
		GDINL Matrix4x4& GD_VECTORCALL operator-= (Matrix4x4 const& other)
		{
			return *this = *this - other;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// Multiplies a matrix and a number.
		//! @{
		GDINL Matrix4x4 GD_VECTORCALL operator* (Float32 const Value) const
		{
			auto Result(*this);
			Result.Rows[0] *= Vector4(Value);
			Result.Rows[1] *= Vector4(Value);
			Result.Rows[2] *= Vector4(Value);
			Result.Rows[3] *= Vector4(Value);
			return Result;
		}
		GDINL Matrix4x4& GD_VECTORCALL operator*= (Float32 const Value)
		{
			return *this = *this * Value;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// Multiplies a matrix and a vector.
		//! @{
		GDINL Vector4 GD_VECTORCALL operator* (Vector4 const Vector) const
		{
			Vector4 Result;
#if GD_SIMD_DIRECTX_MATH
			Result.Register = DirectX::XMVector4Transform(Vector.Register, Registers);
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_FPU
			UnrolledForIndex<4>([&](SizeTp const I) {
				UnrolledForIndex<4>([&](SizeTp const J) {
					Result[I] = (*this)(I, J) * Vector(J);
				});
			});
#endif	// if GD_SIMD_FPU
#if GD_SIMD_SSE
#	error 'Not implemented.'
#endif	// if GD_SIMD_SSE
			return Result;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// Multiplies two matrices.
		//! @{
		GDINL Matrix4x4 GD_VECTORCALL operator* (Matrix4x4 const& other) const
		{
			Matrix4x4 Result = {};
#if GD_SIMD_DIRECTX_MATH
			Result.Registers = DirectX::XMMatrixMultiply(Registers, other.Registers);
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_FPU
			UnrolledForIndex<4>([&](SizeTp const I) {
				UnrolledForIndex<4>([&](SizeTp const J) {
					UnrolledForIndex<4>([&](SizeTp const K) {
						Result(I, J) += (*this)(I, K) * other(K, J);
					});
				});
			});
#endif	// if GD_SIMD_FPU
#if GD_SIMD_SSE
#	error 'Not implemented.'
#endif	// if GD_SIMD_SSE
			return Result;
		}
		GDINL Matrix4x4& GD_VECTORCALL operator*= (Matrix4x4 const& other)
		{
			return *this = *this - other;
		}
		//! @}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Matrix construction operations.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Generates the orthographic projection matrix.
		//! @param m_Left Minimum x - value of view volume.
		//! @param m_Right Maximum x - value of view volume.
		//! @param Bottom Minimum y - value of view volume.
		//! @param Top Maximum y - value of view volume.
		//! @param ZNear Near clipping plane.
		//! @param ZFar Far clipping plane.
		GDINL static Matrix4x4 GD_VECTORCALL Ortho(Float32 const Left, Float32 const Right, Float32 const Bottom
			, Float32 const Top, Float32 const ZNear, Float32 const ZFar)
		{
			Matrix4x4 Result = {};
#if GD_SIMD_DIRECTX_MATH
			Result.Registers = DirectX::XMMatrixOrthographicLH(Right - Left, Top - Bottom, ZNear, ZFar);
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_FPU
			auto const Range = 1.0f / (ZFar - ZNear);
			Result.M00 = 2.0f / (m_Right - m_Left);
			Result.M11 = 2.0f / (Top - Bottom);
			Result.M22 = Range;
			Result.M32 = -Range * ZNear;
			Result.M33 = 1.0f;
#endif	// if GD_SIMD_FPU
#if GD_SIMD_SSE
#	error 'Not implemented.'
#endif	// if GD_SIMD_SSE
			return Result;
		}

		// ------------------------------------------------------------------------------------------
		//! Generates the perspective projection matrix.
		//! @param FieldOfViewRadians Field of view in radians.
		//! @param Aspect Screen aspect ration.
		//! @param ZNear Near clipping plane.
		//! @param ZFar Far clipping plane.
		GDINL static Matrix4x4 GD_VECTORCALL Perspective(Float32 const FieldOfViewRadians, Float32 const Aspect
			, Float32 const ZNear, Float32 const ZFar)
		{
			Matrix4x4 Result = {};
#if GD_SIMD_DIRECTX_MATH
			Result.Registers = DirectX::XMMatrixPerspectiveFovLH(FieldOfViewRadians, Aspect, ZNear, ZFar);
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_FPU
			Result.M11 = Math::Cos(FieldOfViewRadians * 0.5f) / Math::Sin(FieldOfViewRadians * 0.5f);
			Result.M00 = Result.M11 / Aspect;
			Result.M22 = ZFar / (ZFar - ZNear);
			Result.M23 = 1.0f;
			Result.M32 = -Result.M22 * ZNear;
#endif	// if GD_SIMD_FPU
#if GD_SIMD_SSE
#	error 'Not implemented.'
#endif	// if GD_SIMD_SSE
			return Result;
		}

		// ------------------------------------------------------------------------------------------
		//! Generates the combined translation-rotation-scaling matrix for specified parameters.
		//! @param Translation The translation vector.
		//! @param Rotation The rotation quaternion.
		//! @param Scale The scale vector.
		GDINL static Matrix4x4 GD_VECTORCALL TranslateRotateScale(Vector3Fast const Translation
			, Quaternion const Rotation, Vector3Fast const Scale)
		{
			Matrix4x4 Result = {};
#if GD_SIMD_DIRECTX_MATH
			Result.Registers = DirectX::XMMatrixScalingFromVector(Scale.Register);
			Result.Registers *= DirectX::XMMatrixRotationQuaternion(Rotation.Register);
			Result.Registers *= DirectX::XMMatrixTranslationFromVector(Translation.Register);
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_FPU
#	error 'Not implemented.'
			Result.Rows[0] *= Vector4(Scale.X);
			Result.Rows[1] *= Vector4(Scale.Y);
			Result.Rows[2] *= Vector4(Scale.Z);
			Result.Rows[3] = Vector4(Translation.X, Translation.Y, Translation.Z, 1.0f);
#endif	// if GD_SIMD_FPU
#if GD_SIMD_SSE
#	error 'Not implemented.'
#endif	// if GD_SIMD_SSE
			return Result;
		}
	} GD_ALIGN_GCC(64);

GD_NAMESPACE_END
