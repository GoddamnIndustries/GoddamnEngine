/// ==========================================================================================
/// Matrix4x4.h - 4x4 matrix class.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 09.06.2014  - Rewritten from scratch by James Jhuighuy,
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_MATH_MATRIX4X4
#define GD_CORE_MATH_MATRIX4X4

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Math/Misc.h>
#include <GoddamnEngine/Core/Math/Vector3.h>
#include <GoddamnEngine/Core/Math/Vector4.h>
#include <GoddamnEngine/Core/Math/Quaternion.h>
#include <GoddamnEngine/Core/Math/Float32x4Intrinsics/Float32x4Intrinsics.h>

#include <string.h>	// memcpy

GD_NAMESPACE_BEGIN

	/// Generic implementation of a 4 x 4 Row-major matrix representation.
	/// If is Float32x4 intrinsics are supported, uses them for better performance.
	template<typename ElementType>
	struct Matrix4x4t final
	{
		static_assert(TypeTraits::IsFloatingPoint<ElementType>::Value, "'Matrix4x4t<T>' error: T should be floating-point value.");
		typedef typename Conditional<TypeTraits::IsPodType<ElementType>::Value, ElementType, ElementType const&>::Type ElementTypeConstRef;
		enum : size_t { ThisRowsCount = 4, ThisColumnsCount = 4 };
		
		union {
			struct { ElementType Elements[ThisRowsCount * ThisColumnsCount]; };
			struct { Vector4t<ElementType> _0, _1, _2, _3; };
			struct {
				ElementType _00, _01, _02, _03;
				ElementType _10, _11, _12, _13;
				ElementType _20, _21, _22, _23;
				ElementType _30, _31, _32, _33;
			};	// anonymous struct
		};	// anonymous union

	public /* Constructors */:
		/// Initializes identity matrix with speficied diagonal value.
		/// @param ElementDiagVal Diagonal matrix value. 1 by default.
		GDINL Matrix4x4t(ElementTypeConstRef const ElementDiagVal = ElementType(1))
			: _00(ElementDiagVal), _01(ElementType(0)), _02(ElementType(0)), _03(ElementType(0))
			, _10(ElementType(0)), _11(ElementDiagVal), _12(ElementType(0)), _13(ElementType(0))
			, _20(ElementType(0)), _21(ElementType(0)), _22(ElementDiagVal), _23(ElementType(0))
			, _30(ElementType(0)), _31(ElementType(0)), _32(ElementType(0)), _33(ElementDiagVal)
		{
		}

		/// Initializes Matrix using 4 vectors.
		GDINL Matrix4x4t(Vector4t<ElementType> const& _0, Vector4t<ElementType> const& _1, Vector4t<ElementType> const& _2, Vector4t<ElementType> const& _3)
			: _0(_0)
			, _1(_1)
			, _2(_2)
			, _3(_3)
		{
		}

		/// Copy constructor.
		GDINL Matrix4x4t(Matrix4x4t const& Other)
		{
			memcpy(self, &Other, sizeof(*self));
		}

		GDINL ~Matrix4x4t()
		{
		}

	public /* Class API */:
		/// ==========================================================================================
		/// @name Common initializations.
		/// @{

		/// ------------------------------------------------------------------------------------------
		/// *** Idenitity *** 
		/// ------------------------------------------------------------------------------------------

		/// Initializes specified matrix to identity.
		/// @param Matrix         Matrix to perform initialization on.
		/// @param ElementDiagVal Matrix diagonal initial value.
		static inline void MakeIdentity(Matrix4x4t& Matrix, ElementTypeConstRef const ElementDiagVal = ElementType(1))
		{
			memset(&Matrix, 0, sizeof(Matrix));
			Matrix._00 = Matrix._11 = Matrix._22 = Matrix._33 = ElementDiagVal;
		}

		/// Initializes this matrix to identity.
		/// @returns Self.
		/// @see Matrix4x4t::MakeIdentity
		GDINL Matrix4x4t& Identity(ElementTypeConstRef const ElementDiagVal = ElementType(1))
		{
			Matrix4x4t::MakeIdentity((*self), ElementDiagVal);
			return (*self);
		}

		/// ------------------------------------------------------------------------------------------
		/// *** Inverse ***
		/// ------------------------------------------------------------------------------------------

		/// Inverses specified matrix.
		/// @param Input  Initial matrix that requires to be inverted.
		/// @param Matrix Matrix to perform inversion on.
		static inline void MakeInverse(Matrix4x4t& Matrix, Matrix4x4t const& Input)
		{
			if (&Matrix != &_Input) {
				ElementType const Coef00 = Input._22 * Input._33 - Input._32 * Input._23;
				ElementType const Coef02 = Input._12 * Input._33 - Input._32 * Input._13;
				ElementType const Coef03 = Input._12 * Input._23 - Input._22 * Input._13;

				ElementType const Coef04 = Input._21 * Input._33 - Input._31 * Input._23;
				ElementType const Coef06 = Input._11 * Input._33 - Input._31 * Input._13;
				ElementType const Coef07 = Input._11 * Input._23 - Input._21 * Input._13;

				ElementType const Coef08 = Input._21 * Input._32 - Input._31 * Input._22;
				ElementType const Coef10 = Input._11 * Input._32 - Input._31 * Input._12;
				ElementType const Coef11 = Input._11 * Input._22 - Input._21 * Input._12;

				ElementType const Coef12 = Input._20 * Input._33 - Input._30 * Input._23;
				ElementType const Coef14 = Input._10 * Input._33 - Input._30 * Input._13;
				ElementType const Coef15 = Input._10 * Input._23 - Input._20 * Input._13;

				ElementType const Coef16 = Input._20 * Input._32 - Input._30 * Input._22;
				ElementType const Coef18 = Input._10 * Input._32 - Input._30 * Input._12;
				ElementType const Coef19 = Input._10 * Input._22 - Input._20 * Input._12;

				ElementType const Coef20 = Input._20 * Input._31 - Input._30 * Input._21;
				ElementType const Coef22 = Input._10 * Input._31 - Input._30 * Input._11;
				ElementType const Coef23 = Input._10 * Input._21 - Input._20 * Input._11;

				Vector4t<ElementType> const Fac0(Coef00, Coef00, Coef02, Coef03);
				Vector4t<ElementType> const Fac1(Coef04, Coef04, Coef06, Coef07);
				Vector4t<ElementType> const Fac2(Coef08, Coef08, Coef10, Coef11);
				Vector4t<ElementType> const Fac3(Coef12, Coef12, Coef14, Coef15);
				Vector4t<ElementType> const Fac4(Coef16, Coef16, Coef18, Coef19);
				Vector4t<ElementType> const Fac5(Coef20, Coef20, Coef22, Coef23);

				Vector4t<ElementType> const Vec0(Input._10, Input._00, Input._00, Input._00);
				Vector4t<ElementType> const Vec1(Input._11, Input._01, Input._01, Input._01);
				Vector4t<ElementType> const Vec2(Input._12, Input._02, Input._02, Input._02);
				Vector4t<ElementType> const Vec3(Input._13, Input._03, Input._03, Input._03);

				Vector4t<ElementType> const static SignA(+1, -1, +1, -1);
				Vector4t<ElementType> const static SignB(-1, +1, -1, +1);
				Matrix._0 = SignA * (Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
				Matrix._1 = SignB * (Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
				Matrix._2 = SignA * (Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
				Matrix._3 = SignB * (Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);

				Vector4t<ElementType> const Row0(Matrix._00, Matrix._10, Matrix._20, Matrix._30);
				ElementType Determinant = Input._0.Dot(Row0);
				Matrix /= Determinant;
			} else {
				Matrix4x4t const InputCopy(Input);
				Matrix4x4t::MakeInverse(Matrix, InputCopy);
			}
		}

		/// Inverses this matrix.
		/// @returns Self.
		/// @see Matrix4x4t::MakeInverse
		GDINL Matrix4x4t& Inverse()
		{
			Matrix4x4t::MakeInverse((*self), (*self));
			return (*self);
		}

		/// ------------------------------------------------------------------------------------------
		/// *** Transpose ***
		/// ------------------------------------------------------------------------------------------

		/// Transposes specified matrix.
		/// @param Input  Initial matrix that requires to be transposed.
		/// @param Matrix Matrix to perform transposion on.
		static inline void MakeTranspose(Matrix4x4t& Matrix, Matrix4x4t const& Input)
		{
			if (&Matrix != &_Input) {
				Matrix._00 = Input._00; Matrix._01 = Input._10; Matrix._02 = Input._20; Matrix._03 = Input._30;
				Matrix._10 = Input._01; Matrix._11 = Input._11; Matrix._12 = Input._21; Matrix._13 = Input._31;
				Matrix._20 = Input._02; Matrix._21 = Input._12; Matrix._22 = Input._22; Matrix._23 = Input._32;
				Matrix._30 = Input._03; Matrix._31 = Input._13; Matrix._32 = Input._23; Matrix._33 = Input._33;
			} else {
				Matrix4x4t const InputCopy(Input);
				Matrix4x4t::MakeTranspose(Matrix, InputCopy);
			}
		}

		/// Transposes this matrix.
		/// @returns Self.
		/// @see Matrix4x4t::MakeTranspose
		Matrix4x4t& Transpose()
		{
			Matrix4x4t::MakeTranspose((*self), (*self));
			return (*self);
		}

		/// @}
		/// ==========================================================================================

		/// ==========================================================================================
		/// @name Matrix transformations.
		/// @{

		/// ------------------------------------------------------------------------------------------
		/// *** Rotation ***
		/// ------------------------------------------------------------------------------------------

		/// Rotates specified matrix on specified quaternion.
		/// @param Matrix   Matrix to perform transformations on.
		/// @param Rotation Value on which matrix would be rotated. 
		static inline void MakeRotation(Matrix4x4t& Matrix, Quaternion_t<ElementType> const& Rotation)
		{
			Matrix4x4t::MakeIdentity(Matrix);
			Matrix._00 = ElementType(1) - ElementType(2) * Rotation.y * Rotation.y - ElementType(2) * Rotation.z * Rotation.z;
			Matrix._01 =                  ElementType(2) * Rotation.x * Rotation.y + ElementType(2) * Rotation.w * Rotation.z;
			Matrix._02 =                  ElementType(2) * Rotation.x * Rotation.z - ElementType(2) * Rotation.w * Rotation.y;
			Matrix._10 =                  ElementType(2) * Rotation.x * Rotation.y - ElementType(2) * Rotation.w * Rotation.z;
			Matrix._11 = ElementType(1) - ElementType(2) * Rotation.x * Rotation.x - ElementType(2) * Rotation.z * Rotation.z;
			Matrix._12 =                  ElementType(2) * Rotation.y * Rotation.z + ElementType(2) * Rotation.w * Rotation.x;
			Matrix._20 =                  ElementType(2) * Rotation.x * Rotation.z + ElementType(2) * Rotation.w * Rotation.y;
			Matrix._21 =                  ElementType(2) * Rotation.y * Rotation.z - ElementType(2) * Rotation.w * Rotation.x;
			Matrix._22 = ElementType(1) - ElementType(2) * Rotation.x * Rotation.x - ElementType(2) * Rotation.y * Rotation.y;
		}

		/// Rotates this matrix on specified quaternion.
		/// @returns Self.
		/// @see Matrix4x4t::MakeRotation
		GDINL Matrix4x4t& Rotate(Quaternion_t<ElementType> const& Rotation)
		{
			Matrix4x4t::MakeRotation((*self), Rotation);
			return (*self);
		}

		/// ------------------------------------------------------------------------------------------
		/// *** Scale ***
		/// ------------------------------------------------------------------------------------------

		/// Scales specified matrix on specified vector.
		/// @param Matrix Matrix to perform transformations on.
		/// @param Scale  Value on which matrix would be scaled. 
		static inline void MakeScale(Matrix4x4t& Matrix, Vector3t<ElementType> const& Scale)
		{
			Matrix4x4t::MakeIdentity(Matrix);
			Matrix._00 = Scale.x;
			Matrix._11 = Scale.y;
			Matrix._22 = Scale.z;
			Matrix._33 = ElementType(1);
		}

		/// Scales this matrix on specified vector.
		/// @returns Self.
		/// @see Matrix4x4t::MakeScale
		GDINL Matrix4x4t& Scale(Vector3t<ElementType> const& Scale)
		{
			Matrix4x4t::MakeIdentity((*self), Scale);
			return (*self);
		}

		/// ------------------------------------------------------------------------------------------
		/// *** Translation ***
		/// ------------------------------------------------------------------------------------------

		/// Translates specified matrix on specified vector.
		/// @param Matrix       Matrix to perform transformations on.
		/// @param Translation  Value on which matrix would be translated. 
		static inline void MakeTranslation(Matrix4x4t& Matrix, Vector3t<ElementType> const& Translation)
		{
			Matrix4x4t::MakeIdentity(Matrix);
			Matrix._3 = Vector4t<ElementType>(Translation, ElementType(1));
		}

		/// Translates this matrix on specified vector.
		/// @returns Self.
		/// @see Matrix4x4t::MakeTranslation
		GDINL Matrix4x4t& Translate(Vector3t<ElementType> const& Translation)
		{
			Matrix4x4t::MakeTranslation((*self), Translation);
			return (*self);
		}

		/// @}
		/// ==========================================================================================

		/// ==========================================================================================
		/// @name View matrix computing
		/// @{

		/// ------------------------------------------------------------------------------------------
		/// *** LookTo ***
		/// ------------------------------------------------------------------------------------------

		/// Builds a look-to matrix (Left-Handed coordinate system)
		/// @param Matrix Matrix to perform transformations on.
		/// @param Eye    Position of camera.
		/// @param Dir    Direction to which we are looking.
		/// @param Up     Camera up vector.
		static inline void MakeLookToLH(Matrix4x4t& Matrix, Vector3Fast const& Eye, Vector3Fast const& Dir, Vector3Fast const& Up)
		{
			throw 0;
		}

		/// Builds a look-to matrix (Left-Handed coordinate system).
		/// @see Matrix4x4t::MakeLookToLH
		GDINL Matrix4x4t& LookToLH(Matrix4x4t& Matrix, Vector3Fast const& Eye, Vector3Fast const& Dir, Vector3Fast const& Up)
		{
			Matrix4x4t::MakeLookToLH((*self), Eye, Dir, Up);
			return (*self);
		}

		/// Builds a look-to matrix (Right-Handed coordinate system).
		/// @param Matrix Matrix to perform transformations on.
		/// @param Eye    Position of camera.
		/// @param Dir    Direction to which we are looking.
		/// @param Up     Camera up vector.
		static inline void MakeLookToRH(Matrix4x4t& Matrix, Vector3Fast const& Eye, Vector3Fast const& Dir, Vector3Fast const& Up)
		{
			Float32x4Intrinsics::VectorRegisterType const NegDir = Float32x4Intrinsics::VectorNegate(Dir.ElementsVector);
			Matrix4x4t::MakeLookToLH(Matrix, Eye, Vector3Fast(NegDir), Up);
		}

		/// Builds a look-to matrix (Right-Handed coordinate system).
		/// @see Matrix4x4t::MakeLookToRH
		GDINL Matrix4x4t& LookToRH(Matrix4x4t& Matrix, Vector3Fast const& Eye, Vector3Fast const& Dir, Vector3Fast const& Up)
		{
			Matrix4x4t::MakeLookToRH((*self), Eye, Dir, Up);
			return (*self);
		}

		/// ------------------------------------------------------------------------------------------
		/// *** LookAt ***
		/// ------------------------------------------------------------------------------------------

		/// Builds a look-at matrix (Left-Handed coordinate system)
		/// @param Matrix Matrix to perform transformations on.
		/// @param Eye    Position of camera
		/// @param At     Point we are looking at
		/// @param Up     Camera up vector
		static inline void MakeLookAtLH(Matrix4x4t& Matrix, Vector3t<ElementType> const& Eye, Vector3t<ElementType> const& At, Vector3t<ElementType> const& Up)
		{
			Vector3t<ElementType> const ZAxis = (At - Eye).Normalize();
			Vector3t<ElementType> const XAxis = Up.Cross(ZAxis).Normalize();
			Vector3t<ElementType> const YAxis = ZAxis.Cross(XAxis).Normalize();

			Matrix._00 =  XAxis.x;        Matrix._01 =  YAxis.x;        Matrix._02 =  ZAxis.x;        Matrix._03 = ElementType(0);
			Matrix._10 =  XAxis.y;        Matrix._11 =  YAxis.y;        Matrix._12 =  ZAxis.y;        Matrix._13 = ElementType(0);
			Matrix._20 =  XAxis.z;        Matrix._21 =  YAxis.z;        Matrix._22 =  ZAxis.z;        Matrix._23 = ElementType(0);
			Matrix._30 = -XAxis.Dot(Eye); Matrix._31 = -YAxis.Dot(Eye); Matrix._32 = -ZAxis.Dot(Eye); Matrix._33 = ElementType(1);
		}

		/// Makes view matrix from this one.
		/// @returns Self.
		/// @see Matrix4x4t::MakeLookAtLH
		GDINL Matrix4x4t& LookAtLH(Vector3t<ElementType> const& Eye, Vector3t<ElementType> const& At, Vector3t<ElementType> const& Up)
		{
			Matrix4x4t::MakeLookAtLH((*self), Eye, At, Up);
			return (*self);
		}

		/// Builds a look-at matrix (Right-Handed coordinate system)
		/// @param Matrix Matrix to perform transformations on.
		/// @param Eye    Position of camera
		/// @param At     Point we are looking at
		/// @param Up     Camera up vector
		static inline void MakeLookAtRH(Matrix4x4t& Matrix, Vector3t<ElementType> const& Eye, Vector3t<ElementType> const& At, Vector3t<ElementType> const& Up)
		{
			throw 0;
		}

		/// Makes view matrix from this one.
		/// @returns Self.
		/// @see Matrix4x4t::MakeLookAtRH
		GDINL Matrix4x4t& LookAtRH(Vector3t<ElementType> const& Eye, Vector3t<ElementType> const& At, Vector3t<ElementType> const& Up)
		{
			Matrix4x4t::MakeLookAtRH((*self), Eye, At, Up);
			return (*self);
		}

		/// @}
		/// ==========================================================================================

		/// ==========================================================================================
		/// @name Projection matrix computing
		/// @{

		/// ------------------------------------------------------------------------------------------
		/// *** Perspective ***
		/// ------------------------------------------------------------------------------------------

		/// Builds perspective-projection matrix (Left-Handed coordinate system)
		/// @param Matrix     Matrix to perform transformations on.
		/// @param FOVDegrees Field of view.
		/// @param Aspect     Screen aspect ration.
		/// @param ZNear      Near clipping plane.
		/// @param ZFar       Far clipping plane.
		static inline void MakePerspectiveLH(Matrix4x4t& Matrix, ElementTypeConstRef const FOVDegrees, ElementTypeConstRef const Aspect, ElementTypeConstRef const ZNear, ElementTypeConstRef const ZFar)
		{
			ElementType const FOV    = ElementType(M_PI / 180) * FOVDegrees;
			ElementType const YScale = ElementType(1) / tan(FOV * ElementType(0.5));
			ElementType const XScale = YScale / Aspect;

			Matrix._00 = XScale;         Matrix._01 = ElementType(0); Matrix._02 = ElementType(0);                Matrix._03 = ElementType(0);
			Matrix._10 = ElementType(0); Matrix._11 = YScale;         Matrix._12 = ElementType(0);                Matrix._13 = ElementType(0);
			Matrix._20 = ElementType(0); Matrix._21 = ElementType(0); Matrix._22 =         ZFar / (ZFar - ZNear); Matrix._23 = ElementType(1);
			Matrix._30 = ElementType(0); Matrix._31 = ElementType(0); Matrix._32 = ZNear * ZFar / (ZNear - ZFar); Matrix._33 = ElementType(0);
		}

		/// Makes perspective matrix from this one.
		/// @returns Self.
		/// @see Matrix4x4t::MakePerspectiveLH
		GDINL Matrix4x4t& PerspectiveLH(ElementTypeConstRef const FOVDegrees, ElementTypeConstRef const Aspect, ElementTypeConstRef const ZNear, ElementTypeConstRef const ZFar)
		{
			Matrix4x4t::MakePerspectiveLH((*self), FOVDegrees, Aspect, ZNear, ZFar);
			return (*self);
		}

		/// Builds perspective-projection matrix (Right-Handed coordinate system)
		/// @param Matrix     Matrix to perform transformations on.
		/// @param FOVDegrees Field of view.
		/// @param Aspect     Screen aspect ration.
		/// @param ZNear      Near clipping plane.
		/// @param ZFar       Far clipping plane.
		static inline void MakePerspectiveRH(Matrix4x4t& Matrix, ElementTypeConstRef const FOVDegrees, ElementTypeConstRef const Aspect, ElementTypeConstRef const ZNear, ElementTypeConstRef const ZFar)
		{
			throw 0;
		}

		/// Makes perspective matrix from this one.
		/// @returns Self.
		/// @see Matrix4x4t::MakePerspectiveRH
		GDINL Matrix4x4t& PerspectiveRH(ElementTypeConstRef const FOVDegrees, ElementTypeConstRef const Aspect, ElementTypeConstRef const ZNear, ElementTypeConstRef const ZFar)
		{
			Matrix4x4t::MakePerspectiveRH((*self), FOVDegrees, Aspect, ZNear, ZFar);
			return (*self);
		}

		/// ------------------------------------------------------------------------------------------
		/// *** Ortho ***
		/// ------------------------------------------------------------------------------------------

		/// Builds orthographic-projection matrix (Left-Handed coordinate system)
		/// @param Matrix Matrix to perform transformations on.
		/// @param Left   Minimum x-value of view volume.
		/// @param Right  Maximum x-value of view volume.
		/// @param Bottom Minimum y-value of view volume.
		/// @param Top    Maximum y-value of view volume.
		/// @param ZNear  Near clipping plane.
		/// @param ZFar   Far clipping plane.
		static inline void MakeOrthoLH(Matrix4x4t& Matrix, ElementTypeConstRef const Left, ElementTypeConstRef const Right, ElementTypeConstRef const Bottom, ElementTypeConstRef const Top, ElementTypeConstRef const ZNear, ElementTypeConstRef const ZFar)
		{
			Matrix._00 = ElementType(2) / (Right - Left); Matrix._01 = ElementType(0);                  Matrix._02 = ElementType(0);                  Matrix._03 = ElementType(0);
			Matrix._10 = ElementType(0);                  Matrix._11 = ElementType(2) / (Top - Bottom); Matrix._12 = ElementType(0);                  Matrix._13 = ElementType(0);
			Matrix._20 = ElementType(0);                  Matrix._21 = ElementType(0);                  Matrix._22 = ElementType(1) / (ZFar - ZNear); Matrix._23 = ElementType(0);
			Matrix._30 = (Left + Right) / (Left - Right); Matrix._31 = (Top + Bottom) / (Bottom - Top); Matrix._32 =          ZNear / (ZNear - ZFar); Matrix._33 = ElementType(1);
		}

		/// Makes ortho matrix from this one.
		/// @returns Self.
		/// @see Matrix4x4t::MakeOrthoLH
		GDINL Matrix4x4t<ElementType>& OrthoLH(ElementTypeConstRef const Left, ElementTypeConstRef const Right, ElementTypeConstRef const Bottom, ElementTypeConstRef const Top, ElementTypeConstRef const ZNear, ElementTypeConstRef const ZFar)
		{
			Matrix4x4t::MakeOrthoLH((*self), Left, Right, Bottom, Top, ZNear, ZFar);
			return (*self);
		}

		/// Builds orthographic-projection matrix (Right-Handed coordinate system)
		/// @param Matrix Matrix to perform transformations on.
		/// @param Left   Minimum x-value of view volume.
		/// @param Right  Maximum x-value of view volume.
		/// @param Bottom Minimum y-value of view volume.
		/// @param Top    Maximum y-value of view volume.
		/// @param ZNear  Near clipping plane.
		/// @param ZFar   Far clipping plane.
		static inline void MakeOrthoRH(Matrix4x4t& Matrix, ElementTypeConstRef const Left, ElementTypeConstRef const Right, ElementTypeConstRef const Bottom, ElementTypeConstRef const Top, ElementTypeConstRef const ZNear, ElementTypeConstRef const ZFar)
		{
			throw 0;
		}

		/// Makes ortho matrix from this one.
		/// @returns Self.
		/// @see Matrix4x4t::MakeOrthoRH
		GDINL Matrix4x4t<ElementType>& OrthoRH(ElementTypeConstRef const Left, ElementTypeConstRef const Right, ElementTypeConstRef const Bottom, ElementTypeConstRef const Top, ElementTypeConstRef const ZNear, ElementTypeConstRef const ZFar)
		{
			Matrix4x4t::MakeOrthoRH((*self), Left, Right, Bottom, Top, ZNear, ZFar);
			return (*self);
		}

		/// @}
		/// ==========================================================================================

	public /* Operators */:
		inline Matrix4x4t<ElementType> operator* (Matrix4x4t const& Other) const
		{
			return Matrix4x4t(
				self->_0 * Other._00 + self->_1 * Other._01 + self->_2 * Other._02 + self->_3 * Other._03,
				self->_0 * Other._10 + self->_1 * Other._11 + self->_2 * Other._12 + self->_3 * Other._13,
				self->_0 * Other._20 + self->_1 * Other._21 + self->_2 * Other._22 + self->_3 * Other._23,
				self->_0 * Other._30 + self->_1 * Other._31 + self->_2 * Other._32 + self->_3 * Other._33
			);
		}

		GDINL Vector4t<ElementType> operator* (Vector4t<ElementType> const& Vector) const
		{
			return Vector4t<ElementType>(
				self->_00 * Vector.x + self->_10 * Vector.y + self->_20 * Vector.z + self->_30 * Vector.w,
				self->_01 * Vector.x + self->_11 * Vector.y + self->_21 * Vector.z + self->_31 * Vector.w,
				self->_02 * Vector.x + self->_12 * Vector.y + self->_22 * Vector.z + self->_32 * Vector.w,
				self->_03 * Vector.x + self->_13 * Vector.y + self->_23 * Vector.z + self->_33 * Vector.w
			);
		}

		GDINL Matrix4x4t<ElementType>& operator*= (ElementTypeConstRef const& Scalar)
		{
			self->_0 *= Scalar;
			self->_1 *= Scalar;
			self->_2 *= Scalar;
			self->_3 *= Scalar;

			return (*self);
		}

		GDINL Matrix4x4t<ElementType>& operator/= (ElementTypeConstRef const& Scalar)
		{
			self->_0 /= Scalar;
			self->_1 /= Scalar;
			self->_2 /= Scalar;
			self->_3 /= Scalar;

			return (*self);
		}

		inline Matrix4x4t<ElementType>& operator*= (Matrix4x4t const& Other)
		{
			(*self) = ((*self) * Other);
			return (*self);
		}

		GDINL Matrix4x4t<ElementType>& operator= (Matrix4x4t const& Other)
		{
			memcpy(self, &Other, sizeof(*self));
			return (*self);
		}

		GDINL Vector4t<ElementType>& operator[] (size_t const Index)
		{ 
			GD_ASSERT(Index < ThisRowsCount, "invalid Matrix4x4 subindex.");
			return *(&self->_0 + Index);
		}

		GDINL Vector4t<ElementType> const& operator[] (size_t const Index) const 
		{
			return const_cast<Matrix4x4t*>(self)->operator[](Index);
		}
	};	// struct Matrix4x4t

#if (defined(GD_HAS_FLOAT32X4INTRINSICS) && (!defined(GD_DOCUMENTATION)))
	template<>
	struct GD_ALIGN_MSVC(16) Matrix4x4t<Float32>
	{
		enum : size_t { ThisRowsCount = 4, ThisColumnsCount = 4 };
		union {
			struct { Float32x4Intrinsics::VectorRegisterType _R0, _R1, _R2, _R3; };
			struct { Float32 Elements[ThisRowsCount * ThisColumnsCount]; };
			struct { Vector4t<Float32> _0, _1, _2, _3; };
			struct {
				Float32 _00, _01, _02, _03;
				Float32 _10, _11, _12, _13;
				Float32 _20, _21, _22, _23;
				Float32 _30, _31, _32, _33;
			};	// anonymous struct
		};	// anonymous union

	public /* Constructors */:
		GDINL Matrix4x4t(Float32 const Diag = 1.0f)
			: _0(Float32x4Intrinsics::VectorMake(Diag, 0.0f, 0.0f, 0.0f))
			, _1(Float32x4Intrinsics::VectorMake(0.0f, Diag, 0.0f, 0.0f))
			, _2(Float32x4Intrinsics::VectorMake(0.0f, 0.0f, Diag, 0.0f))
			, _3(Float32x4Intrinsics::VectorMake(0.0f, 0.0f, 0.0f, Diag))
		{
		}

		GDINL Matrix4x4t(Float32x4Intrinsics::VectorRegisterType const _R0, Float32x4Intrinsics::VectorRegisterType const _R1, 
			             Float32x4Intrinsics::VectorRegisterType const _R2, Float32x4Intrinsics::VectorRegisterType const _R3)
			: _0(_R0)
			, _1(_R1)
			, _2(_R2)
			, _3(_R3)
		{
		}

		GDINL Matrix4x4t(Vector4t<Float32> const& _0, Vector4t<Float32> const& _1, Vector4t<Float32> const& _2, Vector4t<Float32> const& _3)
			: _0(_0.ElementsVector)
			, _1(_1.ElementsVector)
			, _2(_2.ElementsVector)
			, _3(_3.ElementsVector)
		{
		}

		GDINL Matrix4x4t(Matrix4x4t const& Other)
			: _0(Other._R0)
			, _1(Other._R1)
			, _2(Other._R2)
			, _3(Other._R3)
		{
		}

		GDINL ~Matrix4x4t()
		{
		}

	public /* Class API */:
		
		/// ==========================================================================================
		/// @name Common modifications.
		/// @{

		/// ------------------------------------------------------------------------------------------
		/// *** Identity ***
		/// ------------------------------------------------------------------------------------------

		/// Initializes specified matrix to identity.
		/// @param Matrix         Matrix to perform initialization on.
		/// @param ElementDiagVal Matrix diagonal initial value.
		static inline void MakeIdentity(Matrix4x4t& Matrix, Float32 const Float32Def = 1.0f)
		{
			Float32 const Init = Float32Def;
			Matrix._R0 = Float32x4Intrinsics::VectorMake(Init, 0.0f, 0.0f, 0.0f);
			Matrix._R1 = Float32x4Intrinsics::VectorMake(0.0f, Init, 0.0f, 0.0f);
			Matrix._R2 = Float32x4Intrinsics::VectorMake(0.0f, 0.0f, Init, 0.0f);
			Matrix._R3 = Float32x4Intrinsics::VectorMake(0.0f, 0.0f, 0.0f, Init);
		}

		/// Initializes this matrix to identity.
		/// @returns Self.
		/// @see Matrix4x4t::MakeIdentity
		GDINL Matrix4x4t& Identity(Float32 const Float32Def = 1.0f)
		{
			Matrix4x4t::MakeIdentity((*self), Float32Def);
			return (*self);
		}

		/// ------------------------------------------------------------------------------------------
		/// *** Inverse ***
		/// ------------------------------------------------------------------------------------------

		/// Inverses specified matrix.
		/// @param Input  Initial matrix that requires to be inverted.
		/// @param Matrix Matrix to perform inversion on.
		GDINL static void MakeInverse(Matrix4x4t& Matrix, Matrix4x4t const& Input)
		{	// I've prefered inlining inverting code into intrinsics.
			Float32x4Intrinsics::MatrixInverse(&Matrix._R0, &Input._R0);
		}

		/// Inverses this matrix.
		/// @returns Self.
		/// @see Matrix4x4t::MakeInverse
		GDINL Matrix4x4t& Inverse()
		{
			Matrix4x4t::MakeInverse((*self), (*self));
			return (*self);
		}

		/// ------------------------------------------------------------------------------------------
		/// *** Transpose ***
		/// ------------------------------------------------------------------------------------------

		/// Transposes specified matrix.
		/// @param Input  Initial matrix that requires to be transposed.
		/// @param Matrix Matrix to perform transposion on.
		static inline void MakeTranspose(Matrix4x4t& Matrix, Matrix4x4t const& Input)
		{
			Float32x4Intrinsics::VectorRegisterType const Temp0 = Float32x4Intrinsics::VectorShuffle(Input._R0, Input._R1, 0, 1, 0, 1);
			Float32x4Intrinsics::VectorRegisterType const Temp2 = Float32x4Intrinsics::VectorShuffle(Input._R0, Input._R1, 2, 3, 2, 3);
			Float32x4Intrinsics::VectorRegisterType const Temp1 = Float32x4Intrinsics::VectorShuffle(Input._R2, Input._R3, 0, 1, 0, 1);
			Float32x4Intrinsics::VectorRegisterType const Temp3 = Float32x4Intrinsics::VectorShuffle(Input._R2, Input._R3, 2, 3, 2, 3);
			Matrix._R0 = Float32x4Intrinsics::VectorShuffle(Temp0, Temp1, 0, 2, 0, 2);
			Matrix._R1 = Float32x4Intrinsics::VectorShuffle(Temp0, Temp1, 1, 3, 1, 3);
			Matrix._R2 = Float32x4Intrinsics::VectorShuffle(Temp2, Temp3, 0, 2, 0, 2);
			Matrix._R3 = Float32x4Intrinsics::VectorShuffle(Temp2, Temp3, 1, 3, 1, 3);
		}

		/// Transposes this matrix.
		/// @returns Self.
		/// @see Matrix4x4t::MakeTranspose
		Matrix4x4t& Transpose()
		{
			Matrix4x4t::MakeTranspose((*self), (*self));
			return (*self);
		}

		/// @}
		/// ==========================================================================================

		/// ==========================================================================================
		/// @name Matrix transformations.
		/// @{

		/// ------------------------------------------------------------------------------------------
		/// *** Rotate ***
		/// ------------------------------------------------------------------------------------------

		/// Rotates specified matrix on specified quaternion.
		/// @param Matrix   Matrix to perform transformations on.
		/// @param Rotation Value on which matrix would be rotated. 
		static inline void MakeRotation(Matrix4x4t& Matrix, Quaternion_t<Float32> const& Rotation)
		{
			Float32x4Intrinsics::VectorRegisterType static const ConstantFloat1110 = { 1.0f, 1.0f, 1.0f, 0.0f };

			Float32x4Intrinsics::VectorRegisterType Q0 = Float32x4Intrinsics::VectorAdd(Rotation.ElementsVector, Rotation.ElementsVector);
			Float32x4Intrinsics::VectorRegisterType Q1 = Float32x4Intrinsics::VectorMul(Rotation.ElementsVector, Q0);

			Float32x4Intrinsics::VectorRegisterType V0 = Float32x4Intrinsics::VectorShuffle(Q1, Q1, 1, 0, 0, 3);
			V0 = Float32x4Intrinsics::VectorAnd(V0, Float32x4Intrinsics::VectorMake_1110());
			Float32x4Intrinsics::VectorRegisterType V1 = Float32x4Intrinsics::VectorSwizzle(Q1, 2, 2, 1, 3);
			V1 = Float32x4Intrinsics::VectorAnd(V1, Float32x4Intrinsics::VectorMake_1110());
			Float32x4Intrinsics::VectorRegisterType R0 = Float32x4Intrinsics::VectorSub(ConstantFloat1110, V0);
			R0 = Float32x4Intrinsics::VectorSub(R0, V1);

			V0 = Float32x4Intrinsics::VectorSwizzle(Rotation.ElementsVector, 0, 0, 1, 3);
			V1 = Float32x4Intrinsics::VectorSwizzle(Q0, 2, 1, 2, 3);
			V0 = Float32x4Intrinsics::VectorMul(V0, V1);

			V1 = Float32x4Intrinsics::VectorSwizzle(Rotation.ElementsVector, 3, 3, 3, 3);
			Float32x4Intrinsics::VectorRegisterType const V2 = Float32x4Intrinsics::VectorSwizzle(Q0, 1, 2, 0, 3);
			V1 = Float32x4Intrinsics::VectorMul(V1, V2);

			Float32x4Intrinsics::VectorRegisterType R1 = Float32x4Intrinsics::VectorAdd(V0, V1);
			Float32x4Intrinsics::VectorRegisterType R2 = Float32x4Intrinsics::VectorSub(V0, V1);

			V0 = Float32x4Intrinsics::VectorShuffle(R1, R2, 1, 2, 0, 1);
			V0 = Float32x4Intrinsics::VectorSwizzle(V0,     0, 2, 3, 1);
			V1 = Float32x4Intrinsics::VectorShuffle(R1, R2, 0, 0, 2, 2);
			V1 = Float32x4Intrinsics::VectorSwizzle(V1,     0, 2, 0, 2);
			Q1 = Float32x4Intrinsics::VectorShuffle(R0, V0, 0, 3, 0, 1);
			Q1 = Float32x4Intrinsics::VectorSwizzle(Q1,     0, 2, 3, 1);
			Matrix._R0 = Q1;
			Q1 = Float32x4Intrinsics::VectorShuffle(R0, V0, 1, 3, 2, 3);
			Q1 = Float32x4Intrinsics::VectorSwizzle(Q1,     2, 0, 3, 1);
			Matrix._R1 = Q1;
			Q1 = Float32x4Intrinsics::VectorShuffle(V1, R0, 0, 1, 2, 3);
			Matrix._R2 = Q1;
			Matrix._R3 = Float32x4Intrinsics::VectorMake(0.0f, 0.0f, 0.0f, 1.0f);
		}

		/// Rotates this matrix on specified quaternion.
		/// @returns Self.
		/// @see Matrix4x4t::MakeRotation
		GDINL Matrix4x4t& Rotate(Quaternion_t<Float32> const& Rotation)
		{
			Matrix4x4t::MakeRotation((*self), Rotation);
			return (*self);
		}

		/// ------------------------------------------------------------------------------------------
		/// *** Scale ***
		/// ------------------------------------------------------------------------------------------

		/// Scales specified matrix on specified vector.
		/// @param Matrix Matrix to perform transformations on.
		/// @param Scale  Value on which matrix would be scaled. 
		static inline void MakeScale(Matrix4x4t& Matrix, Vector3Fast const& Scale)
		{
			Matrix4x4t::MakeIdentity(Matrix);
			Matrix._00 = Scale.x;
			Matrix._11 = Scale.y;
			Matrix._22 = Scale.z;
			Matrix._33 = 1.0f;
		}

		/// Scales this matrix on specified vector.
		/// @returns Self.
		/// @see Matrix4x4t::MakeScale
		GDINL Matrix4x4t& Scale(Vector3Fast const& Scale)
		{
			Matrix4x4t::MakeScale((*self), Scale);
			return (*self);
		}

		/// ------------------------------------------------------------------------------------------
		/// *** Translate ***
		/// ------------------------------------------------------------------------------------------

		/// Translates specified matrix on specified vector.
		/// @param Matrix       Matrix to perform transformations on.
		/// @param Translation  Value on whitch matrix would be translated. 
		static inline void MakeTranslation(Matrix4x4t& Matrix, Vector3Fast const& Translation)
		{
			Matrix4x4t::MakeIdentity(Matrix);
			Matrix._3 = Vector4t<Float32>(Translation, 1.0f);
		}

		/// Translates this matrix on specified vector.
		/// @returns Self.
		/// @see Matrix4x4t::MakeTranslation
		GDINL Matrix4x4t& Translate(Vector3Fast const& Translation)
		{
			Matrix4x4t::MakeTranslation((*self), Translation);
			return (*self);
		}

		/// @}
		/// ==========================================================================================

		/// ==========================================================================================
		/// @name View matrix computing.
		/// @{

		/// ------------------------------------------------------------------------------------------
		/// *** LookTo ***
		/// ------------------------------------------------------------------------------------------

		/// Builds a look-to matrix (Left-Handed coordinate system)
		/// @param Matrix Matrix to perform transformations on.
		/// @param Eye    Position of camera.
		/// @param Dir    Direction to which we are looking.
		/// @param Up     Camera up vector.
		static inline void MakeLookToLH(Matrix4x4t& Matrix, Vector3Fast const& Eye, Vector3Fast const& Dir, Vector3Fast const& Up)
		{
			Float32x4Intrinsics::VectorRegisterType const R2 = Float32x4Intrinsics::Vector3Normalize(Dir.ElementsVector);
			Float32x4Intrinsics::VectorRegisterType const R0 = Float32x4Intrinsics::Vector3Normalize(Float32x4Intrinsics::Vector3Cross(Up.ElementsVector, R2));
			Float32x4Intrinsics::VectorRegisterType const R1 = Float32x4Intrinsics::Vector3Cross(R2, R0);
			
			Float32x4Intrinsics::VectorRegisterType const NegEye = Float32x4Intrinsics::VectorNegate(Eye.ElementsVector);
			
			Float32x4Intrinsics::VectorRegisterType const D0 = Float32x4Intrinsics::VectorDot3Vector(R0, NegEye);
			Float32x4Intrinsics::VectorRegisterType const D1 = Float32x4Intrinsics::VectorDot3Vector(R1, NegEye);
			Float32x4Intrinsics::VectorRegisterType const D2 = Float32x4Intrinsics::VectorDot3Vector(R2, NegEye);

			Matrix._R0 = Float32x4Intrinsics::VectorSelect(D0, R0, Float32x4Intrinsics::VectorMake_1110());
			Matrix._R1 = Float32x4Intrinsics::VectorSelect(D1, R1, Float32x4Intrinsics::VectorMake_1110());
			Matrix._R2 = Float32x4Intrinsics::VectorSelect(D2, R2, Float32x4Intrinsics::VectorMake_1110());
			Matrix._R3 = Float32x4Intrinsics::VectorMake(0.0f, 0.0f, 0.0f, 1.0f);
			Matrix4x4t::MakeTranspose(Matrix, Matrix);
		}

		/// Builds a look-to matrix (Left-Handed coordinate system).
		/// @see Matrix4x4t::MakeLookToLH
		GDINL Matrix4x4t& LookToLH(Matrix4x4t& Matrix, Vector3Fast const& Eye, Vector3Fast const& Dir, Vector3Fast const& Up)
		{
			Matrix4x4t::MakeLookToLH((*self), Eye, Dir, Up);
			return (*self);
		}

		/// Builds a look-to matrix (Right-Handed coordinate system).
		/// @param Matrix Matrix to perform transformations on.
		/// @param Eye    Position of camera.
		/// @param Dir    Direction to which we are looking.
		/// @param Up     Camera up vector.
		static inline void MakeLookToRH(Matrix4x4t& Matrix, Vector3Fast const& Eye, Vector3Fast const& Dir, Vector3Fast const& Up)
		{
			Float32x4Intrinsics::VectorRegisterType const NegDir = Float32x4Intrinsics::VectorNegate(Dir.ElementsVector);
			Matrix4x4t::MakeLookToLH(Matrix, Eye, Vector3Fast(NegDir), Up);
		}

		/// Builds a look-to matrix (Right-Handed coordinate system).
		/// @see Matrix4x4t::MakeLookToRH
		GDINL Matrix4x4t& LookToRH(Matrix4x4t& Matrix, Vector3Fast const& Eye, Vector3Fast const& Dir, Vector3Fast const& Up)
		{
			Matrix4x4t::MakeLookToRH((*self), Eye, Dir, Up);
			return (*self);
		}

		/// ------------------------------------------------------------------------------------------
		/// *** LookAt **
		/// ------------------------------------------------------------------------------------------

		/// Builds a look-at matrix (Left-Handed coordinate system)
		/// @param Matrix Matrix to perform transformations on.
		/// @param Eye    Position of camera.
		/// @param At     Point we are looking at.
		/// @param Up     Camera up vector.
		static inline void MakeLookAtLH(Matrix4x4t& Matrix, Vector3Fast const& Eye, Vector3Fast const& At, Vector3Fast const& Up)
		{
			Vector3Fast const Dir = At - Eye;
			Matrix4x4t::MakeLookToLH(Matrix, Eye, Dir, Up);
		}

		/// Makes view matrix from this one.
		/// @returns Self.
		/// @see Matrix4x4t::MakeLookAtLH
		GDINL Matrix4x4t& LookAtLH(Vector3Fast const& Eye, Vector3Fast const& At, Vector3Fast const& Up)
		{
			Matrix4x4t::MakeLookAtLH((*self), Eye, At, Up);
			return (*self);
		}

		/// Builds a look-at matrix (Right-Handed coordinate system)
		/// @param Matrix Matrix to perform transformations on.
		/// @param Eye    Position of camera.
		/// @param At     Point we are looking at.
		/// @param Up     Camera up vector.
		static inline void MakeLookAtRH(Matrix4x4t& Matrix, Vector3Fast const& Eye, Vector3Fast const& At, Vector3Fast const& Up)
		{
			Vector3Fast const NegDir = Eye - At;
			Matrix4x4t::MakeLookToLH(Matrix, Eye, NegDir, Up);
		}

		/// Makes view matrix from this one.
		/// @returns Self.
		/// @see Matrix4x4t::MakeLookAtRH
		GDINL Matrix4x4t& LookAtRH(Vector3Fast const& Eye, Vector3Fast const& At, Vector3Fast const& Up)
		{
			Matrix4x4t::MakeLookAtRH((*self), Eye, At, Up);
			return (*self);
		}

		/// @}
		/// ==========================================================================================

		/// ==========================================================================================
		/// @name Projection matrix computing
		/// @{

		/// ------------------------------------------------------------------------------------------
		/// *** Perspective ***
		/// ------------------------------------------------------------------------------------------

		/// Builds perspective-projection matrix (Left-Handed coordinate system).
		/// @param Matrix     Matrix to perform transformations on.
		/// @param FOVDegrees Field of view in degrees.
		/// @param Aspect     Screen aspect ration.
		/// @param ZNear      Near clipping plane.
		/// @param ZFar       Far clipping plane.
		static inline void MakePerspectiveLH(Matrix4x4t& Matrix, Float32 const FOVDegrees, Float32 const Aspect, Float32 const ZNear, Float32 const ZFar)
		{
			Float32 SinFOV = 0.0f;
			Float32 CosFOV = 0.0f;
			MathMisc::ScalarSinCos(SinFOV, CosFOV, 0.5f * (FOVDegrees / 180.0f * Float32(M_PI)));

			Float32 const Range = ZFar / (ZFar - ZNear);
			Float32 const Height = CosFOV / SinFOV;
			Float32x4Intrinsics::VectorRegisterType Values = Float32x4Intrinsics::VectorMake(Height / Aspect, Height, Range, -Range * ZNear);
			Float32x4Intrinsics::VectorRegisterType Temp   = Float32x4Intrinsics::VectorMake(0.0f);
			
			Matrix._R0 = Float32x4Intrinsics::VectorMove(Temp, Values);
			Matrix._R1 = Float32x4Intrinsics::VectorAnd(Values, Float32x4Intrinsics::VectorMake_0100());
			
			Values     = Float32x4Intrinsics::VectorShuffle(Values,                                 Float32x4Intrinsics::VectorMake(0.0f, 0.0f, 0.0f, 1.0f), 2, 3, 2, 3);
			Matrix._R2 = Float32x4Intrinsics::VectorShuffle(Float32x4Intrinsics::VectorMake_0000(), Values,                                                  0, 0, 0, 3);
			Matrix._R3 = Float32x4Intrinsics::VectorShuffle(Matrix._R2,                             Values,                                                  0, 0, 1, 2);
		}

		/// Makes perspecitive matrix from this one.
		/// @returns Self.
		/// @see Matrix4x4t::MakePerspectiveLH
		GDINL Matrix4x4t& PerspectiveLH(Float32 const FOVDegrees, Float32 const Aspect, Float32 const ZNear, Float32 const ZFar)
		{
			Matrix4x4t::MakePerspectiveLH((*self), FOVDegrees, Aspect, ZNear, ZFar);
			return (*self);
		}

		/// Builds perspective-projection matrix (Right-Handed coordinate system).
		/// @param Matrix     Matrix to perform transformations on.
		/// @param FOVDegrees Field of view in degrees.
		/// @param Aspect     Screen aspect ration.
		/// @param ZNear      Near clipping plane.
		/// @param ZFar       Far clipping plane.
		static inline void MakePerspectiveRH(Matrix4x4t& Matrix, Float32 const FOVDegrees, Float32 const Aspect, Float32 const ZNear, Float32 const ZFar)
		{
			Float32 SinFOV = 0.0f;
			Float32 CosFOV = 0.0f;
			MathMisc::ScalarSinCos(SinFOV, CosFOV, 0.5f * (FOVDegrees / 180.0f * Float32(M_PI)));

			Float32 const Range = ZFar / (ZNear - ZFar);
			Float32 const Height = CosFOV / SinFOV;
			Float32x4Intrinsics::VectorRegisterType Values = Float32x4Intrinsics::VectorMake(Height / Aspect, Height, Range, Range * ZNear);
			Float32x4Intrinsics::VectorRegisterType Temp   = Float32x4Intrinsics::VectorMake(0.0f);
			
			Matrix._R0 = Float32x4Intrinsics::VectorMove(Temp, Values);
			Matrix._R1 = Float32x4Intrinsics::VectorAnd(Values, Float32x4Intrinsics::VectorMake_0100());
		
			Values     = Float32x4Intrinsics::VectorShuffle(Values,                                 Float32x4Intrinsics::VectorMake(0.0f, 0.0f, 0.0f, -1.0f), 2, 3, 2, 3);
			Matrix._R2 = Float32x4Intrinsics::VectorShuffle(Float32x4Intrinsics::VectorMake_0000(), Values,                                                   0, 0, 0, 3);
			Matrix._R3 = Float32x4Intrinsics::VectorShuffle(Matrix._R2,                             Values,                                                   0, 0, 1, 2);
		}

		/// Makes perspecitive matrix from this one.
		/// @returns Self.
		/// @see Matrix4x4t::MakePerspectiveLH
		GDINL Matrix4x4t& PerspectiveRH(Float32 const FOVDegrees, Float32 const Aspect, Float32 const ZNear, Float32 const ZFar)
		{
			Matrix4x4t::MakePerspectiveRH((*self), FOVDegrees, Aspect, ZNear, ZFar);
			return (*self);
		}

		/// ------------------------------------------------------------------------------------------
		/// *** Ortho ***
		/// ------------------------------------------------------------------------------------------

		/// Builds orthographic-projection matrix (Left-Handed coordinate system)
		/// @param Matrix Matrix to perform transformations on.
		/// @param Left   Minimum x-value of view volume.
		/// @param Right  Maximum x-value of view volume.
		/// @param Bottom Minimum y-value of view volume.
		/// @param Top    Maximum y-value of view volume.
		/// @param ZNear  Near clipping plane.
		/// @param ZFar   Far clipping plane.
		static inline void MakeOrthoLH(Matrix4x4t& Matrix, Float32 const Left, Float32 const Right, Float32 const Bottom, Float32 const Top, Float32 const ZNear, Float32 const ZFar)
		{
			Matrix._00 = 2.0f / (Right - Left);           Matrix._01 = 0.0f;                            Matrix._02 = 0.0f;                   Matrix._03 = 0.0f;
			Matrix._10 = 0.0f;                            Matrix._11 = 2.0f / (Top - Bottom);           Matrix._12 = 0.0f;                   Matrix._13 = 0.0f;
			Matrix._20 = 0.0f;                            Matrix._21 = 0.0f;                            Matrix._22 =  1.0f / (ZFar - ZNear); Matrix._23 = 0.0f;
			Matrix._30 = (Left + Right) / (Left - Right); Matrix._31 = (Top + Bottom) / (Bottom - Top); Matrix._32 = ZNear / (ZNear - ZFar); Matrix._33 = 1.0f;
		}

		/// Makes ortho matrix from this one.
		/// @returns Self.
		/// @see Matrix4x4t::MakeOrthoLH
		GDINL Matrix4x4t<Float32>& OrthoLH(Float32 const Left, Float32 const Right, Float32 const Bottom, Float32 const Top, Float32 const ZNear, Float32 const ZFar)
		{
			Matrix4x4t::MakeOrthoLH((*self), Left, Right, Bottom, Top, ZNear, ZFar);
			return (*self);
		}

		/// Builds orthographic-projection matrix (Left-Handed coordinate system)
		/// @param Matrix Matrix to perform transformations on.
		/// @param Left   Minimum x-value of view volume.
		/// @param Right  Maximum x-value of view volume.
		/// @param Bottom Minimum y-value of view volume.
		/// @param Top    Maximum y-value of view volume.
		/// @param ZNear  Near clipping plane.
		/// @param ZFar   Far clipping plane.
		static inline void MakeOrthoRH(Matrix4x4t& Matrix, Float32 const Left, Float32 const Right, Float32 const Bottom, Float32 const Top, Float32 const ZNear, Float32 const ZFar)
		{
			throw 0;
		}

		/// Makes ortho matrix from this one.
		/// @returns Self.
		/// @see Matrix4x4t::MakeOrthoLH
		GDINL Matrix4x4t<Float32>& OrthoRH(Float32 const Left, Float32 const Right, Float32 const Bottom, Float32 const Top, Float32 const ZNear, Float32 const ZFar)
		{
			Matrix4x4t::MakeOrthoRH((*self), Left, Right, Bottom, Top, ZNear, ZFar);
			return (*self);
		}

		/// @}
		/// ==========================================================================================

	public /* Operators */:
		inline Matrix4x4t operator* (Matrix4x4t<Float32> const& Other) const
		{
			Matrix4x4t Result;
			Float32x4Intrinsics::VectorRegisterType Temp;

			Temp       = Float32x4Intrinsics::VectorMul   (Float32x4Intrinsics::VectorReplicate(self->_R0, 0), Other._R0      );
			Temp       = Float32x4Intrinsics::VectorMulAdd(Float32x4Intrinsics::VectorReplicate(self->_R0, 1), Other._R1, Temp);
			Temp       = Float32x4Intrinsics::VectorMulAdd(Float32x4Intrinsics::VectorReplicate(self->_R0, 2), Other._R2, Temp);
			Result._R0 = Float32x4Intrinsics::VectorMulAdd(Float32x4Intrinsics::VectorReplicate(self->_R0, 3), Other._R3, Temp);

			Temp       = Float32x4Intrinsics::VectorMul   (Float32x4Intrinsics::VectorReplicate(self->_R1, 0), Other._R0      );
			Temp       = Float32x4Intrinsics::VectorMulAdd(Float32x4Intrinsics::VectorReplicate(self->_R1, 1), Other._R1, Temp);
			Temp       = Float32x4Intrinsics::VectorMulAdd(Float32x4Intrinsics::VectorReplicate(self->_R1, 2), Other._R2, Temp);
			Result._R1 = Float32x4Intrinsics::VectorMulAdd(Float32x4Intrinsics::VectorReplicate(self->_R1, 3), Other._R3, Temp);

			Temp       = Float32x4Intrinsics::VectorMul   (Float32x4Intrinsics::VectorReplicate(self->_R2, 0), Other._R0      );
			Temp       = Float32x4Intrinsics::VectorMulAdd(Float32x4Intrinsics::VectorReplicate(self->_R2, 1), Other._R1, Temp);
			Temp       = Float32x4Intrinsics::VectorMulAdd(Float32x4Intrinsics::VectorReplicate(self->_R2, 2), Other._R2, Temp);
			Result._R2 = Float32x4Intrinsics::VectorMulAdd(Float32x4Intrinsics::VectorReplicate(self->_R2, 3), Other._R3, Temp);

			Temp       = Float32x4Intrinsics::VectorMul   (Float32x4Intrinsics::VectorReplicate(self->_R3, 0), Other._R0      );
			Temp       = Float32x4Intrinsics::VectorMulAdd(Float32x4Intrinsics::VectorReplicate(self->_R3, 1), Other._R1, Temp);
			Temp       = Float32x4Intrinsics::VectorMulAdd(Float32x4Intrinsics::VectorReplicate(self->_R3, 2), Other._R2, Temp);
			Result._R3 = Float32x4Intrinsics::VectorMulAdd(Float32x4Intrinsics::VectorReplicate(self->_R3, 3), Other._R3, Temp);

			return Result;
		}

		inline Vector4t<Float32> operator* (Vector4t<Float32> const& Vector) const
		{
			return Vector4t<Float32>(
				self->_00 * Vector.x + self->_10 * Vector.y + self->_20 * Vector.z + self->_30 * Vector.w,
				self->_01 * Vector.x + self->_11 * Vector.y + self->_21 * Vector.z + self->_31 * Vector.w,
				self->_02 * Vector.x + self->_12 * Vector.y + self->_22 * Vector.z + self->_32 * Vector.w,
				self->_03 * Vector.x + self->_13 * Vector.y + self->_23 * Vector.z + self->_33 * Vector.w
			);
		}

		Matrix4x4t<Float32>& operator*= (Float32 const& Scalar)
		{
			self->_0 *= Scalar;
			self->_1 *= Scalar;
			self->_2 *= Scalar;
			self->_3 *= Scalar;

			return (*self);
		}

		Matrix4x4t<Float32>& operator/= (Float32 const& Scalar)
		{
			self->_0 /= Scalar;
			self->_1 /= Scalar;
			self->_2 /= Scalar;
			self->_3 /= Scalar;

			return (*self);
		}

		inline Matrix4x4t<Float32>& operator*= (Matrix4x4t const& Other)
		{
			(*self) = ((*self) * Other);
			return (*self);
		}

		GDINL Matrix4x4t<Float32>& operator= (Matrix4x4t const& Other)
		{
			memcpy(self, &Other, sizeof(*self));
			return (*self);
		}

		GDINL Vector4t<Float32>& operator[] (size_t const Index)
		{
			GD_ASSERT(Index < ThisRowsCount, "invalid Matrix4x4 subindex.");
			return *(&self->_0 + Index);
		}

		GDINL Vector4t<Float32> const& operator[] (size_t const Index) const
		{
			return const_cast<Matrix4x4t*>(self)->operator[](Index);
		}
	} GD_ALIGN_GCC(16);	// struct Matrix4x4t
#endif	// if (defined(GD_HAS_FLOAT32X4INTRINSICS) && (!defined(GD_DOCUMENTATION)))

	typedef Matrix4x4t<Float32> Float32x4x4;
	typedef Matrix4x4t<Float64> Float64x4x4;
	typedef Float32x4x4 Matrix4x4;

GD_NAMESPACE_END

#endif
