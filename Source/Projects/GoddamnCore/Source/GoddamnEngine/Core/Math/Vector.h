// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Math/Vector.h
 * 2D, 3D and 4D vectors classes.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Math/Math.h>

// Define this to disable SIMD instruction forcedly.
#define GD_SIMD_FORCE_DISABLE GD_FALSE
#if !GD_SIMD_FORCE_DISABLE
#	if GD_PLATFORM_API_MICROSOFT
#		include <DirectXMath.h>
#		define GD_SIMD_ENABLED GD_TRUE
#		define GD_SIMD_DIRECTX_MATH GD_TRUE
#	else	// if GD_PLATFORM_API_MICROSOFT
#		if GD_ARCHITECTURE_X64
#			include <immintrin.h>
#			define GD_SIMD_ENABLED GD_TRUE
#			define GD_SIMD_SSE GD_TRUE
#		endif	// if GD_ARCHITECTURE_X64
#	endif	// if GD_PLATFORM_API_MICROSOFT
#endif	// if !GD_SIMD_FORCE_DISABLE

#define GD_SIMD_SSE 0
#define GD_SIMD_FPU !GD_SIMD_ENABLED
//#include <arm_neon.h>

GD_NAMESPACE_BEGIN

	// ==========================================================================================
	// Vector2 struct.
	// ==========================================================================================

	// ------------------------------------------------------------------------------------------
	//! Two-dimensional floating-point vector class.
	struct Vector2 final
	{
	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Public fields and constants.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		Vector2 static const Zero, One;
		Vector2 static const Right, Left, Up, Down;

		union {
			struct { Float32 X, Y; };
			struct { Float32 U, V; };
		};	// anonymous union

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructors.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Constructs a 2D vector with all components replicated to zero.
		GDINL Vector2()
			: X(0.0f), Y(0.0f)
		{}

		// ------------------------------------------------------------------------------------------
		//! Constructs a 2D vector with all components replicated to the specified value.
		//! @param TheValue Value to replicate through the components.
		GDINL explicit Vector2(Float32 const TheValue)
			: X(TheValue), Y(TheValue) 
		{}

		// ------------------------------------------------------------------------------------------
		//! Constructs a 2D vector with a specified X and Y components.
		//! @param TheX lhs vector component value.
		//! @param TheY rhs vector component value.
		GDINL Vector2(Float32 const TheX, Float32 const TheY)
			: X(TheX), Y(TheY)
		{}

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Unary 2D vector operations.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		// Components access operator.
		//! @{
		GDINL Float32 const& operator()(SizeTp const Index) const
		{
			GD_DEBUG_ASSERT(Index < 2, "index out of range.");
			return *(&this->X + Index);
		}
		GDINL Float32& operator()(SizeTp const Index)
		{
			return const_cast<Float32&>(const_cast<Vector2 const&>(*this)(Index));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// Negates a vector.
		GDINL Vector2 operator- () const
		{
			return Vector2(-X, -Y);
		}

		// ------------------------------------------------------------------------------------------
		//! Computes a squared version of the magnitude of this vector.
		//! @returns A squared version of the magnitude of this vector.
		GDINL Float32 GetSqrMagnitude() const
		{
			return DotProduct(*this, *this);
		}

		// ------------------------------------------------------------------------------------------
		//! Computes a magnitude of this vector.
		//! @returns A magnitude of this vector.
		GDINL Float32 GetMagnitude() const
		{
			return Math::Sqrt(GetSqrMagnitude());
		}

		// ------------------------------------------------------------------------------------------
		//! Computes a normalized version of this vector.
		//! @returns A normalized version of this vector.
		GDINL Vector2 Normalize() const
		{
			auto RevMagnitude = GetMagnitude();
			if (RevMagnitude != 0.0)
				RevMagnitude = 1.0f / RevMagnitude;
			return *this * Vector2(RevMagnitude);
		}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Binary 2D vector operations.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		// Compares two vectors.
		//! @{
		GDINL bool operator== (Vector2 const other) const
		{
			return X == other.X && Y == other.Y;
		}
		GDINL bool operator!= (Vector2 const& other)
		{
			return !(*this == other);
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// Adds two vectors.
		//! @{
		GDINL Vector2 operator+ (Vector2 const other) const
		{
			return Vector2(X + other.X, Y + other.Y);
		}
		template<typename Tp>
		GDINL Vector2& operator+= (Vector2& other)
		{
			return *this = *this + other;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// Subtracts two vectors.
		//! @{
		GDINL Vector2 operator- (Vector2 const other) const
		{
			return Vector2(X - other.X, Y - other.Y);
		}
		GDINL Vector2& operator-= (Vector2& other)
		{
			return *this = *this - other;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// Multiplies two vectors.
		//! @{
		GDINL Vector2 operator* (Vector2 const other) const
		{
			return Vector2(X * other.X, Y * other.Y);
		}
		GDINL Vector2& operator*= (Vector2& other)
		{
			return *this = *this * other;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Computes a dot product of two 2D vectors.
		//! @param V1 lhs vector.
		//! @param V2 rhs vector.
		GDINL static Float32 DotProduct(Vector2 const V1, Vector2 const V2)
		{
			return V1.X * V2.X + V1.Y * V2.Y;
		}

		// ------------------------------------------------------------------------------------------
		// Divides (per component) two vectors.
		//! @{
		GDINL Vector2 operator/ (Vector2 const other) const
		{
			return Vector2(X / other.X, Y / other.Y);
		}
		GDINL Vector2& operator/= (Vector2& other)
		{
			return *this = *this / other;
		}
		//! @}

	};	// struct Vector2

	// ==========================================================================================
	// Vector3 struct.
	// ==========================================================================================

	// ------------------------------------------------------------------------------------------
	//! Three-dimensional floating-point vector class.
	struct Vector3 final
	{
	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Public fields and constants.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		Vector3 static const Zero, One;
		Vector3 static const Right, Left, Up, Down, Forward, Backward;

		union {
			struct { Float32 X, Y, Z; };
			struct { Float32 U, V, W; };
		};	// anonymous union

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructors.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Constructs a 3D vector with all components replicated to zero.
		GDINL Vector3()
			: X(0.0f), Y(0.0f), Z(0.0f)
		{}

		// ------------------------------------------------------------------------------------------
		//! Constructs a 3D vector with all components replicated to the specified value.
		//! @param TheValue Value to replicate through the components.
		GDINL explicit Vector3(Float32 const TheValue)
			: X(TheValue), Y(TheValue), Z(TheValue)
		{}

		// ------------------------------------------------------------------------------------------
		//! Constructs a 3D vector with a specified X, Y and Z components.
		//! @param TheX lhs vector component value.
		//! @param TheY rhs vector component value.
		//! @param TheZ Third vector component value.
		GDINL Vector3(Float32 const TheX, Float32 const TheY, Float32 const TheZ)
			: X(TheX), Y(TheY), Z(TheZ)
		{}

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Unary 3D vector operations.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		// Components access operator.
		//! @{
		GDINL Float32 const& operator()(SizeTp const Index) const
		{
			GD_DEBUG_ASSERT(Index < 3, "index out of range.");
			return *(&this->X + Index);
		}
		GDINL Float32& operator()(SizeTp const Index)
		{
			return const_cast<Float32&>(const_cast<Vector3 const&>(*this)(Index));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// Negates a vector.
		GDINL Vector3 operator- () const
		{
			return Vector3(-X, -Y, -Z);
		}

		// ------------------------------------------------------------------------------------------
		//! Computes a squared version of the magnitude of this vector.
		//! @returns A squared version of the magnitude of this vector.
		GDINL Float32 GetSqrMagnitude() const
		{
			return DotProduct(*this, *this);
		}

		// ------------------------------------------------------------------------------------------
		//! Computes a magnitude of this vector.
		//! @returns A magnitude of this vector.
		GDINL Float32 GetMagnitude() const
		{
			return Math::Sqrt(GetSqrMagnitude());
		}

		// ------------------------------------------------------------------------------------------
		//! Computes a normalized version of this vector.
		//! @returns A normalized version of this vector.
		GDINL Vector3 Normalize() const
		{
			auto RevMagnitude = GetMagnitude();
			if (RevMagnitude != 0.0)
				RevMagnitude = 1.0f / RevMagnitude;
			return *this * Vector3(RevMagnitude);
		}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Binary 3D vector operations.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		// Compares two vectors.
		//! @{
		GDINL bool operator== (Vector3 const other) const
		{
			return X == other.X && Y == other.Y && Z == other.Z;
		}
		GDINL bool operator!= (Vector3 const& other)
		{
			return !(*this == other);
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// Adds two vectors.
		//! @{
		GDINL Vector3 operator+ (Vector3 const other) const
		{
			return Vector3(X + other.X, Y + other.Y, Z + other.Z);
		}
		template<typename Tp>
		GDINL Vector3& operator+= (Vector3& other)
		{
			return *this = *this + other;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// Subtracts two vectors.
		//! @{
		GDINL Vector3 operator- (Vector3 const other) const
		{
			return Vector3(X - other.X, Y - other.Y, Z - other.Z);
		}
		GDINL Vector3& operator-= (Vector3& other)
		{
			return *this = *this - other;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// Multiplies two vectors.
		//! @{
		GDINL Vector3 operator* (Vector3 const other) const
		{
			return Vector3(X * other.X, Y * other.Y, Z * other.Z);
		}
		GDINL Vector3& operator*= (Vector3& other)
		{
			return *this = *this * other;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Computes a dot product of two 3D vectors.
		//! @param V1 lhs vector.
		//! @param V2 rhs vector.
		GDINL static Float32 DotProduct(Vector3 const V1, Vector3 const V2)
		{
			return V1.X * V2.X + V1.Y * V2.Y + V1.Z * V2.Z;
		}

		// ------------------------------------------------------------------------------------------
		//! Computes a cross product of two 4D vectors.
		//! @param V1 lhs vector.
		//! @param V2 rhs vector.
		GDINL static Vector3 CrossProduct(Vector3 const V1, Vector3 const V2)
		{
			return Vector3(V1.Y*V2.Z - V1.Z*V2.Y, V1.Z*V2.X - V1.X*V2.Z, V1.X*V2.Y - V1.Y*V2.X);
		}

		// ------------------------------------------------------------------------------------------
		// Divides (per component) two vectors.
		//! @{
		GDINL Vector3 operator/ (Vector3 const other) const
		{
			return Vector3(X / other.X, Y / other.Y, Z / other.Z);
		}
		GDINL Vector3& operator/= (Vector3& other)
		{
			return *this = *this / other;
		}
		//! @}

	};	// struct Vector3

	// ==========================================================================================
	// Vector3Fast struct.
	// ==========================================================================================

#if GD_SIMD_ENABLED || GD_DOCUMENTATION // We do not need to redefine a class for the missing SIMD instructions.

	// ------------------------------------------------------------------------------------------
	//! Fast three-dimensional floating-point vector class.
	//! All mathematical operations are implemented with the support of the SIMD instruction (if available).
	//! @note Size of this type may depend on target platform.
	GD_ALIGN_MSVC(16)
	struct Vector3Fast final
	{
	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Public fields and constants.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		Vector3Fast static const Zero, Identity, One;
		Vector3Fast static const Right, Left, Up, Down, Forward, Backward;

		union {
			Vector3 Vector;
			struct { Float32 X, Y, Z; };
			struct { Float32 R, G, B; };
#if GD_SIMD_DIRECTX_MATH
			DirectX::XMVECTOR Register;
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_SSE	// GD_SIMD_DIRECTX_MATH
			__m128 Register;
#endif	// if GD_SIMD_SSE
		};	// anonymous union

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructors.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Constructs a 3D vector with all components replicated to zero.
		GDINL Vector3Fast()
#if GD_SIMD_DIRECTX_MATH
			: Register(DirectX::XMVectorZero())
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_SSE	
			: Register(_mm_setzero_ps())
#endif	// if GD_SIMD_SSE
		{}

		// ------------------------------------------------------------------------------------------
		//! Constructs a 3D vector with all components replicated to the specified value.
		//! @param TheValue Value to replicate through the components.
		GDINL explicit Vector3Fast(Float32 const TheValue)
#if GD_SIMD_DIRECTX_MATH
			: Register(DirectX::XMVectorReplicate(TheValue))
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_SSE
			: Register(_mm_set_ps(TheValue, TheValue, TheValue, 1.0f))
#endif	// if GD_SIMD_SSE
		{}

		// ------------------------------------------------------------------------------------------
		//! Constructs a 3D vector with a specified X, Y and Z components.
		//! @param TheX lhs vector component value.
		//! @param TheY rhs vector component value.
		//! @param TheZ Third vector component value.
		GDINL Vector3Fast(Float32 const TheX, Float32 const TheY, Float32 const TheZ)
#if GD_SIMD_DIRECTX_MATH
			: Register(DirectX::XMVectorSet(TheX, TheY, TheZ, 1.0f))
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_SSE	
			: Register(_mm_set_ps(TheX, TheY, TheZ, 1.0f))
#endif	// if GD_SIMD_SSE
		{}

		// ------------------------------------------------------------------------------------------
		//! Constructs a 3D vector with all components replicated to zero.
		GDINL /*implicit*/ Vector3Fast(Vector3 const& Vector)
			: Vector3Fast(Vector.X, Vector.Y, Vector.Z)
		{}

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Unary 3D vector operations.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		// Components access operator.
		//! @{
		GDINL Float32 const& GD_VECTORCALL operator()(SizeTp const Index) const
		{
			GD_DEBUG_ASSERT(Index < 3, "index out of range.");
			return *(&this->X + Index);
		}
		GDINL Float32& GD_VECTORCALL operator()(SizeTp const Index)
		{
			return const_cast<Float32&>(const_cast<Vector3Fast const&>(*this)(Index));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// Cast to Vector3 operator.
		GDINL operator Vector3() const
		{
			return Vector;
		}

		// ------------------------------------------------------------------------------------------
		// Negates a vector.
		GDINL Vector3Fast GD_VECTORCALL operator- () const
		{
			Vector3Fast Result = {};
#if GD_SIMD_DIRECTX_MATH
			Result.Register = DirectX::XMVectorNegate(Register);
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_SSE	// GD_SIMD_DIRECTX_MATH
			Result.Register = _mm_sub_ps(_mm_setzero_ps(), Register);
#endif	// if GD_SIMD_SSE
			return Result;
		}

		// ------------------------------------------------------------------------------------------
		//! Computes a squared version of the magnitude of this vector.
		//! @returns A squared version of the magnitude of this vector.
		GDINL Float32 GD_VECTORCALL GetSqrMagnitude() const
		{
			return DotProduct(*this, *this);
		}

		// ------------------------------------------------------------------------------------------
		//! Computes a magnitude of this vector.
		//! @returns A magnitude of this vector.
		GDINL Float32 GD_VECTORCALL GetMagnitude() const
		{
			return Math::Sqrt(GetSqrMagnitude());
		}

		// ------------------------------------------------------------------------------------------
		//! Computes a normalized version of this vector.
		//! @returns A normalized version of this vector.
		GDINL Vector3Fast GD_VECTORCALL Normalize() const
		{
			Vector3Fast Result = {};
#if GD_SIMD_DIRECTX_MATH
			Result.Register = DirectX::XMVector3Normalize(Register);
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_SSE	// GD_SIMD_DIRECTX_MATH
			auto const Magnitude = _mm_sqrt_ps(_mm_dp_ps(Register, Register, 0x7F));
			Result.Register = _mm_div_ps(Register, Magnitude);
#endif	// if GD_SIMD_SSE
			return Result;
		}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Binary 3D vector operations.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		// Compares two vectors.
		//! @{
		GDINL bool GD_VECTORCALL operator== (Vector3Fast const other) const
		{
			return X == other.X && Y == other.Y && Z == other.Z;

			//! @todo Implement this using SSE..
#if GD_SIMD_DIRECTX_MATH && 0
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_SSE	&& 0 
			auto const CompareResult = _mm_cmpeq_ps(Register, other.Register);
			return _mm_movemask_ps(CompareResult) == -1;
#endif	// if GD_SIMD_SSE
		}
		GDINL bool GD_VECTORCALL operator!= (Vector3Fast const& other)
		{
			return !(*this == other);
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// Adds two vectors.
		//! @{
		GDINL Vector3Fast GD_VECTORCALL operator+ (Vector3Fast const other) const
		{
			Vector3Fast Result = {};
#if GD_SIMD_DIRECTX_MATH
			Result.Register = DirectX::XMVectorAdd(Register, other.Register);
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_SSE	// GD_SIMD_DIRECTX_MATH
			Result.Register = _mm_add_ps(Register, other.Register);
#endif	// if GD_SIMD_SSE
			return Result;
		}
		GDINL Vector3Fast& GD_VECTORCALL operator+= (Vector3Fast& other)
		{
			return *this = *this + other;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// Subtracts two vectors.
		//! @{
		GDINL Vector3Fast GD_VECTORCALL operator- (Vector3Fast const other) const
		{
			Vector3Fast Result = {};
#if GD_SIMD_DIRECTX_MATH
			Result.Register = DirectX::XMVectorSubtract(Register, other.Register);
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_SSE	// GD_SIMD_DIRECTX_MATH
			Result.Register = _mm_sub_ps(Register, other.Register);
#endif	// if GD_SIMD_SSE
			return Result;
		}
		GDINL Vector3Fast& GD_VECTORCALL operator-= (Vector3Fast& other)
		{
			return *this = *this - other;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// Multiplies two vectors.
		//! @{
		GDINL Vector3Fast GD_VECTORCALL operator* (Vector3Fast const other) const
		{
			Vector3Fast Result = {};
#if GD_SIMD_DIRECTX_MATH
			Result.Register = DirectX::XMVectorMultiply(Register, other.Register);
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_SSE	// GD_SIMD_DIRECTX_MATH
			Result.Register = _mm_mul_ps(Register, other.Register);
#endif	// if GD_SIMD_SSE
			return Result;
		}
		GDINL Vector3Fast& GD_VECTORCALL operator*= (Vector3Fast& other)
		{
			return *this = *this * other;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Computes a dot product of two 3D vectors.
		//! @param V1 lhs vector.
		//! @param V2 rhs vector.
		GDINL static Float32 GD_VECTORCALL DotProduct(Vector3Fast const V1, Vector3Fast const V2)
		{
#if GD_SIMD_DIRECTX_MATH
			return DirectX::XMVectorGetX(DirectX::XMVector3Dot(V1.Register, V2.Register));
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_SSE	// GD_SIMD_DIRECTX_MATH
			return _mm_cvtss_f32(_mm_dp_ps(V1.Register, V2.Register, 0x7F));
#endif	// if GD_SIMD_SSE
		}

		// ------------------------------------------------------------------------------------------
		//! Computes a cross product of two 3D vectors.
		//! @param V1 lhs vector.
		//! @param V2 rhs vector.
		GDINL static Vector3Fast GD_VECTORCALL CrossProduct(Vector3Fast const V1, Vector3Fast const V2)
		{
			Vector3Fast Result = {};
#if GD_SIMD_DIRECTX_MATH
			Result.Register = DirectX::XMVector3Cross(V1.Register, V2.Register);
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_SSE	// GD_SIMD_DIRECTX_MATH
			auto vTemp1 = XM_PERMUTE_PS(V1.Register, _MM_SHUFFLE(3, 0, 2, 1));	// y1,z1,x1,w1
			auto vTemp2 = XM_PERMUTE_PS(V2.Register, _MM_SHUFFLE(3, 1, 0, 2));	// z2,x2,y2,w2
			Result.Register = _mm_mul_ps(vTemp1, vTemp2);	
			vTemp1 = XM_PERMUTE_PS(vTemp1, _MM_SHUFFLE(3, 0, 2, 1));	// z1,x1,y1,w1
			vTemp2 = XM_PERMUTE_PS(vTemp2, _MM_SHUFFLE(3, 1, 0, 2));	// y2,z2,x2,w2
			vTemp1 = _mm_mul_ps(vTemp1, vTemp2);						
			Result.Register = _mm_sub_ps(Result.Register, vTemp1);		
#endif	// if GD_SIMD_SSE
			return Result;
		}

		// ------------------------------------------------------------------------------------------
		// Divides (per component) two vectors.
		//! @{
		GDINL Vector3Fast GD_VECTORCALL operator/ (Vector3Fast const other) const
		{
			Vector3Fast Result = {};
#if GD_SIMD_DIRECTX_MATH
			Result.Register = DirectX::XMVectorDivide(Register, other.Register);
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_SSE
			Result.Register = _mm_div_ps(Register, other.Register);
#endif	// if GD_SIMD_SSE
			return Result;
		}
		GDINL Vector3Fast& operator/= (Vector3Fast& other)
		{
			return *this = *this / other;
		}
		//! @}
	} GD_ALIGN_GCC(16);	// struct Vector3Fast 

#else	// if GD_SIMD_ENABLED || GD_DOCUMENTATION

	typedef GD_ALIGN_MSVC(16) Vector3 Vector3Fast GD_ALIGN_GCC(16);

#endif	// if GD_SIMD_ENABLED || GD_DOCUMENTATION

	// ==========================================================================================
	// Vector4 struct.
	// ==========================================================================================

	// ------------------------------------------------------------------------------------------
	//! Fast four-dimensional floating-point vector class.
	//! All mathematical operations are implemented with the support of the SIMD instruction (if available).
	GD_ALIGN_MSVC(16)
	struct Vector4 final
	{
	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Public fields and constants.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		Vector4 static const Zero, Identity, One;
		Vector4 static const Right, Left, Up, Down, Forward, Backward;

		union {
			struct { Float32 X, Y, Z, W; };
			struct { Float32 R, G, B, A; };
#if GD_SIMD_DIRECTX_MATH
			DirectX::XMVECTOR Register;
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_SSE	
			__m128 Register;
#endif	// if GD_SIMD_SSE
		};	// anonymous union

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructors.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Constructs a 4D vector with all components replicated to zero.
		GDINL Vector4()
#if GD_SIMD_DIRECTX_MATH
			: Register(DirectX::XMVectorZero())
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_FPU
			: X(0.0f), Y(0.0f), Z(0.0f), W(0.0f) 
#endif	// if GD_SIMD_FPU
#if GD_SIMD_SSE
			: Register(_mm_setzero_ps())
#endif	// if GD_SIMD_SSE
		{}

		// ------------------------------------------------------------------------------------------
		//! Constructs a 4D vector with all components replicated to the specified value.
		//! @param TheValue Value to replicate through the components.
		GDINL explicit Vector4(Float32 const TheValue)
#if GD_SIMD_DIRECTX_MATH
			: Register(DirectX::XMVectorReplicate(TheValue))
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_FPU
			: X(TheValue), Y(TheValue), Z(TheValue), W(TheValue)
#endif	// if GD_SIMD_FPU
#if GD_SIMD_SSE
			: Register(_mm_set_ps1(TheValue))
#endif	// if GD_SIMD_SSE
		{}

		// ------------------------------------------------------------------------------------------
		//! Constructs a 4D vector with a specified X, Y, Z and W components.
		//! @param TheX lhs vector component value.
		//! @param TheY rhs vector component value.
		//! @param TheZ Third vector component value.
		//! @param TheW Fourth vector component value.
		GDINL Vector4(Float32 const TheX, Float32 const TheY, Float32 const TheZ, Float32 const TheW)
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

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Unary 4D vector operations.
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
			return const_cast<Float32&>(const_cast<Vector4 const&>(*this)(Index));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// Negates a vector.
		GDINL Vector4 GD_VECTORCALL operator- () const
		{
			Vector4 Result;
#if GD_SIMD_DIRECTX_MATH
			Result.Register = DirectX::XMVectorNegate(Register);
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_FPU
			Result = Vector4(-X, -Y, -Z, -W);
#endif	// if GD_SIMD_FPU
#if GD_SIMD_SSE
			Result.Register = _mm_sub_ps(_mm_setzero_ps(), Register);
#endif	// if GD_SIMD_SSE
			return Result;
		}

		// ------------------------------------------------------------------------------------------
		//! Computes a squared version of the magnitude of this vector.
		//! @returns A squared version of the magnitude of this vector.
		GDINL Float32 GD_VECTORCALL GetSqrMagnitude() const
		{
			return DotProduct(*this, *this);
		}

		// ------------------------------------------------------------------------------------------
		//! Computes a magnitude of this vector.
		//! @returns A magnitude of this vector.
		GDINL Float32 GD_VECTORCALL GetMagnitude() const
		{
			return Math::Sqrt(GetSqrMagnitude());
		}

		// ------------------------------------------------------------------------------------------
		//! Computes a normalized version of this vector.
		//! @returns A normalized version of this vector.
		GDINL Vector4 GD_VECTORCALL Normalize() const
		{
			Vector4 Result;
#if GD_SIMD_DIRECTX_MATH
			Result.Register = DirectX::XMVector4Normalize(Register);
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_FPU
			Float32 RevMagnitude = GetMagnitude();
			if (RevMagnitude != 0.0)
				RevMagnitude = 1.0f / RevMagnitude;
			Result = *this * Vector4(RevMagnitude);
#endif	// if GD_SIMD_FPU
#if GD_SIMD_SSE
			auto const RevMagnitude = _mm_rsqrt_ps(_mm_dp_ps(Register, Register, 0xFF));
			Result.Register = _mm_mul_ps(Register, Magnitude);
#endif	// if GD_SIMD_SSE
			return Result;
		}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Binary 4D vector operations.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		// Compares two vectors.
		//! @{
		GDINL bool GD_VECTORCALL operator== (Vector4 const other) const
		{
#if GD_SIMD_DIRECTX_MATH && 0
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_FPU || 1
			return X == other.X && Y == other.Y && Z == other.Z && W == other.W;
#endif	// if GD_SIMD_FPU
#if GD_SIMD_SSE && 0
			auto const CompareResult = _mm_cmpeq_ps(Register, other.Register);
			return _mm_movemask_ps(CompareResult) == -1;
#endif	// if GD_SIMD_SSE
		}
		GDINL bool operator!= (Vector4 const& other)
		{
			return !(*this == other);
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// Adds two vectors.
		//! @{
		GDINL Vector4 GD_VECTORCALL operator+ (Vector4 const other) const
		{
			Vector4 Result;
#if GD_SIMD_FPU
			Result = Vector4(X + other.X, Y + other.Y, Z + other.Z, W + other.W);
#endif	// if GD_SIMD_FPU
#if GD_SIMD_SSE
			Result.Register = _mm_add_ps(Register, other.Register);
#endif	// if GD_SIMD_SSE
			return Result;
		}
		GDINL Vector4& GD_VECTORCALL operator+= (Vector4 const& other)
		{
			return *this = *this + other;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// Subtracts two vectors.
		//! @{
		GDINL Vector4 GD_VECTORCALL operator- (Vector4 const other) const
		{
			Vector4 Result;
#if GD_SIMD_DIRECTX_MATH
			Result.Register = DirectX::XMVectorSubtract(Register, other.Register);
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_FPU
			Result = Vector4(X - other.X, Y - other.Y, Z - other.Z, W - other.W);
#endif	// if GD_SIMD_FPU
#if GD_SIMD_SSE
			Result.Register = _mm_sub_ps(Register, other.Register);
#endif	// if GD_SIMD_SSE
			return Result;
		}
		GDINL Vector4& operator-= (Vector4 const& other)
		{
			return *this = *this - other;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// Multiplies two vectors.
		//! @{
		GDINL Vector4 GD_VECTORCALL operator* (Vector4 const other) const
		{
			Vector4 Result;
#if GD_SIMD_DIRECTX_MATH
			Result.Register = DirectX::XMVectorMultiply(Register, other.Register);
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_FPU
			Result = Vector4(X * other.X, Y * other.Y, Z * other.Z, W * other.W);
#endif	// if GD_SIMD_FPU
#if GD_SIMD_SSE
			Result.Register = _mm_mul_ps(Register, other.Register);
#endif	// if GD_SIMD_SSE
			return Result;
		}
		GDINL Vector4& GD_VECTORCALL operator*= (Vector4 const& other)
		{
			return *this = *this * other;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Computes a dot product of two 4D vectors.
		//! @param V1 lhs vector.
		//! @param V2 rhs vector.
		GDINL static Float32 GD_VECTORCALL DotProduct(Vector4 const V1, Vector4 const V2)
		{
#if GD_SIMD_DIRECTX_MATH
			return DirectX::XMVectorGetX(DirectX::XMVector4Dot(V1.Register, V2.Register));
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_FPU
			return V1.X * V2.X + V1.Y * V2.Y + V1.Z * V2.Z + V1.W * V2.W;
#endif	// if GD_SIMD_FPU
#if GD_SIMD_SSE
			return _mm_cvtss_f32(_mm_dp_ps(V1.Register, V2.Register, 0xFF));
#endif	// if GD_SIMD_SSE
		}

		// ------------------------------------------------------------------------------------------
		// Divides (per component) two vectors.
		//! @{
		GDINL Vector4 GD_VECTORCALL operator/ (Vector4 const other) const
		{
			Vector4 Result;
#if GD_SIMD_DIRECTX_MATH
			Result.Register = DirectX::XMVectorDivide(Register, other.Register);
#endif	// if GD_SIMD_DIRECTX_MATH
#if GD_SIMD_FPU
			Result = Vector4(X / other.X, Y / other.Y, Z / other.Z, W / other.W);
#endif	// if GD_SIMD_SSE
#if GD_SIMD_SSE
			Result.Register = _mm_div_ps(Register, other.Register);
#endif	// if GD_SIMD_SSE
			return Result;
		}
		GDINL Vector4& GD_VECTORCALL operator/= (Vector4 const& other)
		{
			return *this = *this / other;
		}
		//! @}
	} GD_ALIGN_GCC(16);	// struct Vector4 

GD_NAMESPACE_END
