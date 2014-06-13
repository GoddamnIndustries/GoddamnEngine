//////////////////////////////////////////////////////////////////////////
/// Vector4.hh: Four-dimensional vector class
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 07.06.2014  - Rewritten from scratch by James Jhuighuy,
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_CORE_MATH_VECTOR4
#define GD_CORE_MATH_VECTOR4

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/TypeTraits.hh>
#include <GoddamnEngine/Core/Math/Vector2.hh>
#include <GoddamnEngine/Core/Math/Vector3.hh>
#include <GoddamnEngine/Core/Math/Float32x4Intrinsics/Float32x4Intrinsics.hh>
#include <GoddamnEngine/Core/Diagnostics/Assertion/Assertion.hh>

GD_NAMESPACE_BEGIN
#include "MathCompilerSetupStart.hh"

	/// Four-dimensional vector class.
	template<typename ElementType>
	struct Vector4t final
	{
	public:
		typedef typename Conditional<TypeTraits::IsPodType<ElementType>::Value, ElementType, ElementType const&>::Type ElementTypeConstRef;
		enum : size_t { ThisComponentsCount = 4 };

		union {
			ElementType Elements[ThisComponentsCount];
			struct { ElementType Left, Top, Width, Height; }; ///< Compatibility with Rectangle
			struct { ElementType x,    y,   z,     w;      }; ///< Representation in XYZW coordinate system
			struct { ElementType r,    g,   b,     a;      }; ///< Representation in RGBA color system
			struct { ElementType _0, _1, _2, _3; };
		};	// anonymous union

	public /* Constructors */:
		GDINL Vector4t(ElementTypeConstRef const Vector4Value0, ElementTypeConstRef const Vector4Value1, ElementTypeConstRef const Vector4Value2, ElementTypeConstRef const Vector4Value3)
			: x(Vector4Value0)
			, y(Vector4Value1)
			, z(Vector4Value2)
			, w(Vector4Value3)
		{
		}

		GDINL Vector4t(Vector2t<ElementType> const& Vector4Value01, Vector2t<ElementType> const& Vector4Value23)
			: x(Vector4Value01.x)
			, y(Vector4Value01.y)
			, z(Vector4Value23.x)
			, w(Vector4Value23.y)
		{
		}

		GDINL Vector4t(Vector2t<ElementType> const& Vector4Value01, ElementTypeConstRef const Vector4Value2, ElementTypeConstRef const Vector4Value3)
			: x(Vector4Value01.x)
			, y(Vector4Value01.y)
			, z(Vector4Value2)
			, w(Vector4Value3)
		{
		}

		GDINL Vector4t(ElementTypeConstRef const Vector4Value0, Vector2t<ElementType> const& Vector4Value12, ElementTypeConstRef const Vector4Value3)
			: x(Vector4Value0)
			, y(Vector4Value12.x)
			, z(Vector4Value12.y)
			, w(Vector4Value3)
		{
		}

		GDINL Vector4t(ElementTypeConstRef const Vector4Value0, ElementTypeConstRef const Vector4Value1, Vector2t<ElementType> const& Vector4Value23)
			: x(Vector4Value0)
			, y(Vector4Value1)
			, z(Vector4Value23.x)
			, w(Vector4Value23.y)
		{
		}

		GDINL Vector4t(Vector3t<ElementType> const& Vector4Value012, ElementTypeConstRef const Vector4Value3)
			: x(Vector4Value012.x)
			, y(Vector4Value012.y)
			, z(Vector4Value012.z)
			, w(Vector4Value3)
		{
		}

		GDINL Vector4t(ElementTypeConstRef const Vector4Value0, Vector3t<ElementType> const& Vector4Value123)
			: x(Vector4Value0)
			, y(Vector4Value123.x)
			, z(Vector4Value123.y)
			, w(Vector4Value123.z)
		{
		}

		GDINL Vector4t(ElementTypeConstRef const Vector4Value)
			: x(Vector4Value)
			, y(Vector4Value)
			, z(Vector4Value)
			, w(Vector4Value)
		{
		}

		GDINL Vector4t(Vector4t const& Other)
			: x(Other.x)
			, y(Other.y)
			, z(Other.z)
			, w(Other.w)
		{
		}

		GDINL Vector4t()
			: x(ElementType(0))
			, y(ElementType(0))
			, z(ElementType(0))
			, w(ElementType(1))
		{
		}

		GDINL ~Vector4t()
		{
		}

	public /* Class API */:
		GDINL ElementType Dot(const Vector4t& v) const
		{
			return self->x * v.x + self->y * v.y + self->z * v.z + self->w * v.w;
		}

		GDINL Vector4t Cross(const Vector4t& v) const
		{
			return Vector4t(self->y * v.z - self->z * v.y, self->z * v.x - self->x * v.z, self->x * v.y - self->y * v.x, ElementType(1));
		}

		GDINL ElementType Length() const
		{
			return SquareRoot(self->Dot(*self));
		}

		GDINL Vector4t Normalize() const
		{
			return ((*self) / self->Length());
		}

	public /* Operators */:
		GDINL ElementTypeConstRef operator[] (size_t const Index) const
		{
			GD_ASSERT(Index < ThisComponentsCount, "invalid Vector4t subindex.");
			return self->Elements[Index];
		}

		GDINL bool operator== (Vector4t const& Other) const
		{
			if (self->x == Other.x)
				if (self->y == Other.y)
					if (self->z == Other.z)
						if (self->w == Other.w)
							return true;
			return false;
		}

		GDINL bool operator!= (Vector4t const& Other) const
		{
			return !((*self) == Other);
		}

#if (defined(GD_COMPILER_MSC))
#	pragma region [--- _= Operators ---]
#endif	// if (defined(GD_COMPILER_MSC))

		GDINL Vector4t& operator+= (ElementTypeConstRef const Vector4Value)
		{
			self->x += Vector4Value;
			self->y += Vector4Value;
			self->z += Vector4Value;
			self->w += Vector4Value;
			return (*self);
		}

		GDINL Vector4t& operator-= (ElementTypeConstRef const Vector4Value)
		{
			self->x -= Vector4Value;
			self->y -= Vector4Value;
			self->z -= Vector4Value;
			self->w -= Vector4Value;
			return (*self);
		}

		GDINL Vector4t& operator*= (ElementTypeConstRef const Vector4Value)
		{
			self->x *= Vector4Value;
			self->y *= Vector4Value;
			self->z *= Vector4Value;
			self->w *= Vector4Value;
			return (*self);
		}

		GDINL Vector4t& operator/= (ElementTypeConstRef const Vector4Value)
		{
			self->x /= Vector4Value;
			self->y /= Vector4Value;
			self->z /= Vector4Value;
			self->w /= Vector4Value;
			return (*self);
		}

		template<typename = typename EnableIf<TypeTraits::IsIntegral<ElementType>::Value>::Type>
		GDINL Vector4t& operator%= (ElementTypeConstRef const Vector4Value)
		{
			self->x %= Vector4Value;
			self->y %= Vector4Value;
			self->z %= Vector4Value;
			self->w %= Vector4Value;
			return (*self);
		}

		GDINL Vector4t& operator = (Vector4t const& Other)
		{
			self->x = Other.x;
			self->y = Other.y;
			self->z = Other.z;
			self->w = Other.w;
			return (*self);
		}

		GDINL Vector4t& operator+= (Vector4t const& Other)
		{
			self->x += Other.x;
			self->y += Other.y;
			self->z += Other.z;
			self->w += Other.w;
			return (*self);
		}

		GDINL Vector4t& operator-= (Vector4t const& Other)
		{
			self->x -= Other.x;
			self->y -= Other.y;
			self->z -= Other.z;
			self->w -= Other.w;
			return (*self);
		}

		GDINL Vector4t& operator*= (Vector4t const& Other)
		{
			self->x *= Other.x;
			self->y *= Other.y;
			self->z *= Other.z;
			self->w *= Other.w;
			return (*self);
		}

		GDINL Vector4t& operator/= (Vector4t const& Other)
		{
			self->x /= Other.x;
			self->y /= Other.y;
			self->z /= Other.z;
			self->w /= Other.w;
			return (*self);
		}

		template<typename = typename EnableIf<TypeTraits::IsIntegral<ElementType>::Value>::Type>
		GDINL Vector4t& operator%= (Vector4t const& Other)
		{
			self->x %= Other.x;
			self->y %= Other.y;
			self->z %= Other.z;
			self->w %= Other.w;
			return (*self);
		}

		GDINL Vector4t& operator++ ()
		{
			++self->x;
			++self->y;
			++self->z;
			++self->w;
			return (*self);
		}

		GDINL Vector4t& operator-- ()
		{
			--self->x;
			--self->y;
			--self->z;
			--self->w;
			return (*self);
		}

		GDINL Vector4t& operator++ (int)
		{
			return (++(*self));
		}

		GDINL Vector4t& operator-- (int)
		{
			return (--(*self));
		}

#if (defined(GD_COMPILER_MSC))
#	pragma endregion
#endif	// if (defined(GD_COMPILER_MSC))

#if (defined(GD_COMPILER_MSC))
#	pragma region [--- _ Operators ---]
#endif	// if (defined(GD_COMPILER_MSC))

		GDINL Vector4t operator+ (ElementTypeConstRef const Vector4Value) const
		{
			return Vector4t(self->x + Vector4Value, self->y + Vector4Value, self->z + Vector4Value, self->w + Vector4Value);
		}

		GDINL Vector4t operator- (ElementTypeConstRef const Vector4Value) const
		{
			return Vector4t(self->x - Vector4Value, self->y - Vector4Value, self->z - Vector4Value, self->w - Vector4Value);
		}

		GDINL Vector4t operator* (ElementTypeConstRef const Vector4Value) const
		{
			return Vector4t(self->x * Vector4Value, self->y * Vector4Value, self->z * Vector4Value, self->w * Vector4Value);
		}

		GDINL Vector4t operator/ (ElementTypeConstRef const Vector4Value) const
		{
			return Vector4t(self->x / Vector4Value, self->y / Vector4Value, self->z / Vector4Value, self->w / Vector4Value);
		}

		template<typename = typename EnableIf<TypeTraits::IsIntegral<ElementType>::Value>::Type>
		GDINL Vector4t operator% (ElementTypeConstRef const Vector4Value) const
		{
			return Vector4t(self->x % Vector4Value, self->y % Vector4Value, self->z % Vector4Value, self->w % Vector4Value);
		}

		GDINL Vector4t operator+ (Vector4t const& Other) const
		{
			return Vector4t(self->x + Other.x, self->y + Other.y, self->z + Other.z, self->w + Other.w);
		}

		GDINL Vector4t operator- (Vector4t const& Other) const
		{
			return Vector4t(self->x - Other.x, self->y - Other.y, self->z - Other.z, self->w - Other.w);
		}

		GDINL Vector4t operator* (Vector4t const& Other) const
		{
			return Vector4t(self->x * Other.x, self->y * Other.y, self->z * Other.z, self->w * Other.w);
		}

		GDINL Vector4t operator/ (Vector4t const& Other) const
		{
			return Vector4t(self->x / Other.x, self->y / Other.y, self->z / Other.z, self->w / Other.w);
		}

		template<typename = typename EnableIf<TypeTraits::IsIntegral<ElementType>::Value>::Type>
		GDINL Vector4t operator% (Vector4t const& Other) const
		{
			return Vector4t(self->x % Other.x, self->y % Other.y, self->z % Other.z, self->w % Other.w);
		}

#if (defined(GD_COMPILER_MSC))
#	pragma endregion
#endif	// if (defined(GD_COMPILER_MSC))
	};	// struct Vector4t

#if (defined(GD_HAS_FLOAT32X4INTRINSICS) && (!defined(GD_DOCUMENTATION)))
	template<>
	struct GD_ALIGN(16) Vector4t<Float32> final
	{
		enum : size_t { ThisComponentsCount = 4 };
		union {
			Float32x4Intrinsics::VectorRegisterType ElementsVector;
			Float32 Elements[ThisComponentsCount];
			struct { Float32 Left, Top, Width, Height; };
			struct { Float32 x,    y,   z,     w;      };
			struct { Float32 r,    g,   b,     a;      };
			struct { Float32 _0, _1, _2, _3; };
		};	// anonymous union

	public /* Constructors */:
		GDINL explicit Vector4t(Float32x4Intrinsics::VectorRegisterType const ElementsSSE)
			: ElementsVector(ElementsSSE)
		{
		}

		GDINL Vector4t()
			: ElementsVector(Float32x4Intrinsics::VectorMake(0.0f, 0.0f, 0.0f, 1.0f))
		{
		}

		GDINL Vector4t(Float32 const Vector4Value)
			: ElementsVector(Float32x4Intrinsics::VectorMake(Vector4Value))
		{
		}

		GDINL Vector4t(Float32 const Vector4Value0, Float32 const Vector4Value1, Float32 const Vector4Value2, Float32 const Vector4Value3 = Float32(1.0f))
			: ElementsVector(Float32x4Intrinsics::VectorMake(Vector4Value0, Vector4Value1, Vector4Value2, Vector4Value3))
		{
		}

		GDINL Vector4t(Vector2t<Float32> const& Vector4Value01, Vector2t<Float32> const& Vector4Value23)
			: ElementsVector(Float32x4Intrinsics::VectorMake(Vector4Value01.x, Vector4Value01.y, Vector4Value23.x, Vector4Value23.y))
		{
		}

		GDINL Vector4t(Vector2t<Float32> const& Vector4Value01, Float32 const Vector4Value2, Float32 const Vector4Value3)
			: ElementsVector(Float32x4Intrinsics::VectorMake(Vector4Value01.x, Vector4Value01.y, Vector4Value2, Vector4Value3))
		{
		}

		GDINL Vector4t(Float32 const Vector4Value0, Vector2t<Float32> const& Vector4Value12, Float32 const Vector4Value3)
			: ElementsVector(Float32x4Intrinsics::VectorMake(Vector4Value0, Vector4Value12.x, Vector4Value12.y, Vector4Value3))
		{
		}

		GDINL Vector4t(Float32 const Vector4Value0, Float32 const Vector4Value1, Vector2t<Float32> const& Vector4Value23)
			: ElementsVector(Float32x4Intrinsics::VectorMake(Vector4Value0, Vector4Value1, Vector4Value23.x, Vector4Value23.y))
		{
		}

		GDINL explicit Vector4t(Vector3t<Float32> const& Vector4Value012, Float32 const Vector4Value3 = Float32(1.0f))
			: ElementsVector(Float32x4Intrinsics::VectorMake(Vector4Value012.x, Vector4Value012.y, Vector4Value012.z, Vector4Value3))
		{
		}

		GDINL Vector4t(Vector4t<Float32> const& Vector4Value012, Float32 const Vector4Value3)
			: ElementsVector(Float32x4Intrinsics::VectorMake(Vector4Value012.x, Vector4Value012.y, Vector4Value012.z, Vector4Value3))
		{
		}

		GDINL Vector4t(Float32 const Vector4Value0, Vector3t<Float32> const& Vector4Value123)
			: ElementsVector(Float32x4Intrinsics::VectorMake(Vector4Value0, Vector4Value123.x, Vector4Value123.y, Vector4Value123.z))
		{
		}

		GDINL Vector4t(Vector4t const& Other)
			: ElementsVector(Other.ElementsVector)
		{
		}

		GDINL ~Vector4t()
		{
		}

	public /* Class API */:
		GDINL Float32 Length() const
		{
			return Float32x4Intrinsics::VectorLength(self->ElementsVector);
		}

		GDINL Vector4t Normalize() const
		{
			return Vector4t(Float32x4Intrinsics::VectorNormalize(self->ElementsVector));
		}

		GDINL Float32 Dot(Vector4t const& Other) const
		{
			return Float32x4Intrinsics::VectorDot4(self->ElementsVector, Other.ElementsVector);
		}

		GDINL Vector4t Cross(Vector4t const& Other) const
		{
			return Vector4t(Float32x4Intrinsics::VectorCross(self->ElementsVector, Other.ElementsVector));
		}

	public /* Operators */:
		GDINL Float32& operator[] (size_t const Index)
		{
			GD_ASSERT(Index < ThisComponentsCount, "invalid Vector4 subindex.");
			return self->Elements[Index];
		}

		GDINL Float32 const& operator[] (size_t const Index) const
		{
			return const_cast<Vector4t&>(*self)[Index];
		}

#if (defined(GD_COMPILER_MSC))
#	pragma region [--- _= Operators ---]
#endif	// if (defined(GD_COMPILER_MSC))

		GDINL Vector4t& operator+= (Float32 const Vector4Value)
		{
			self->ElementsVector = Float32x4Intrinsics::VectorAdd(self->ElementsVector, Float32x4Intrinsics::VectorMake(Vector4Value));
			return (*self);
		}

		GDINL Vector4t& operator-= (Float32 const Vector4Value)
		{
			self->ElementsVector = Float32x4Intrinsics::VectorSub(self->ElementsVector, Float32x4Intrinsics::VectorMake(Vector4Value));
			return (*self);
		}

		GDINL Vector4t& operator*= (Float32 const Vector4Value)
		{
			self->ElementsVector = Float32x4Intrinsics::VectorMul(self->ElementsVector, Float32x4Intrinsics::VectorMake(Vector4Value));
			return (*self);
		}

		GDINL Vector4t& operator/= (Float32 const Vector4Value)
		{
			self->ElementsVector = Float32x4Intrinsics::VectorDiv(self->ElementsVector, Float32x4Intrinsics::VectorMake(Vector4Value));
			return (*self);
		}

		GDINL Vector4t& operator = (Vector4t const& Other)
		{
			self->ElementsVector = Other.ElementsVector;
			return (*self);
		}

		GDINL Vector4t& operator+= (Vector4t const& Other)
		{
			self->ElementsVector = Float32x4Intrinsics::VectorAdd(self->ElementsVector, Other.ElementsVector);
			return (*self);
		}

		GDINL Vector4t& operator-= (Vector4t const& Other)
		{
			self->ElementsVector = Float32x4Intrinsics::VectorSub(self->ElementsVector, Other.ElementsVector);
			return (*self);
		}

		GDINL Vector4t& operator*= (Vector4t const& Other)
		{
			self->ElementsVector = Float32x4Intrinsics::VectorMul(self->ElementsVector, Other.ElementsVector);
			return (*self);
		}

		GDINL Vector4t& operator/= (Vector4t const& Other)
		{
			self->ElementsVector = Float32x4Intrinsics::VectorDiv(self->ElementsVector, Other.ElementsVector);
			return (*self);
		}

		GDINL Vector4t& operator++ ()
		{
			self->ElementsVector = Float32x4Intrinsics::VectorAdd(self->ElementsVector, Float32x4Intrinsics::VectorMake(1.0f));
			return (*self);
		}

		GDINL Vector4t& operator-- ()
		{
			self->ElementsVector = Float32x4Intrinsics::VectorAdd(self->ElementsVector, Float32x4Intrinsics::VectorMake(1.0f));
			return (*self);
		}

		GDINL Vector4t& operator++ (int)
		{
			return (++(*self));
		}

		GDINL Vector4t& operator-- (int)
		{
			return (--(*self));
		}

#if (defined(GD_COMPILER_MSC))
#	pragma endregion
#endif	// if (defined(GD_COMPILER_MSC))

#if (defined(GD_COMPILER_MSC))
#	pragma region [--- _ Operators ---]
#endif	// if (defined(GD_COMPILER_MSC))

		GDINL Vector4t operator+ (Float32 const Vector4Value) const
		{
			return Vector4t(Float32x4Intrinsics::VectorAdd(self->ElementsVector, Float32x4Intrinsics::VectorMake(Vector4Value)));
		}

		GDINL Vector4t operator- (Float32 const Vector4Value) const
		{
			return Vector4t(Float32x4Intrinsics::VectorSub(self->ElementsVector, Float32x4Intrinsics::VectorMake(Vector4Value)));
		}

		GDINL Vector4t operator* (Float32 const Vector4Value) const
		{
			return Vector4t(Float32x4Intrinsics::VectorMul(self->ElementsVector, Float32x4Intrinsics::VectorMake(Vector4Value)));
		}

		GDINL Vector4t operator/ (Float32 const Vector4Value) const
		{
			return Vector4t(Float32x4Intrinsics::VectorDiv(self->ElementsVector, Float32x4Intrinsics::VectorMake(Vector4Value)));
		}

		GDINL Vector4t operator+ (Vector4t const& Other) const
		{
			return Vector4t(Float32x4Intrinsics::VectorAdd(self->ElementsVector, Other.ElementsVector));
		}

		GDINL Vector4t operator- (Vector4t const& Other) const
		{
			return Vector4t(Float32x4Intrinsics::VectorSub(self->ElementsVector, Other.ElementsVector));
		}

		GDINL Vector4t operator* (Vector4t const& Other) const
		{
			return Vector4t(Float32x4Intrinsics::VectorMul(self->ElementsVector, Other.ElementsVector));
		}

		GDINL Vector4t operator/ (Vector4t const& Other) const
		{
			return Vector4t(Float32x4Intrinsics::VectorDiv(self->ElementsVector, Other.ElementsVector));
		}

#if (defined(GD_COMPILER_MSC))
#	pragma endregion
#endif	// if (defined(GD_COMPILER_MSC))
	};	// struct Vector4t<Float32, Comparator>
#endif	// if ((defined(GD_SUPPORT_SSE)) && (!defined(GD_DOCUMENTATION)))

	typedef Vector4t< Int8  >  Int8x4;
	typedef Vector4t<UInt8  > UInt8x4;
	typedef Vector4t< Int16 >  Int16x4;
	typedef Vector4t<UInt16 > UInt16x4;
	typedef Vector4t< Int32 >  Int32x4;
	typedef Vector4t<UInt32 > UInt32x4;
	typedef Vector4t< Int64 >  Int64x4;
	typedef Vector4t<UInt64 > UInt64x4;
	typedef Vector4t<Float32> Float32x4;
	typedef Vector4t<Float64> Float64x4;
	typedef Vector4t<size_t > size_tx4;
	typedef UInt32x4  Color32;
	typedef Float32x4 Vector4;
	typedef Float32x4 Color;
	typedef Float32x4 Rectangle;

#include "MathCompilerSetupEnd.hh"
GD_NAMESPACE_END

#endif