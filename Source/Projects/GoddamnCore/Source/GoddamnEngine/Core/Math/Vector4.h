/// ==========================================================================================
/// Vector4.h: Four-dimensional vector class
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_MATH_VECTOR4
#define GD_CORE_MATH_VECTOR4

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/TypeTraits.h>
#include <GoddamnEngine/Core/Diagnostics/Assertion/Assertion.h>

#include <GoddamnEngine/Core/Math/Math.h>
#include <GoddamnEngine/Core/Math/Vector2.h>
#include <GoddamnEngine/Core/Math/Vector3.h>
#include <GoddamnEngine/Core/Math/Float32x4Intrinsics/Float32x4Intrinsics.h>

GD_NAMESPACE_BEGIN

	/// @brief Four-dimensional vector class.
	/// @code
	///  Y ^  > Z
	///    | /    ...W...
	///    |/   
	/// ---0------> X
	///   /|
	/// @endcode
	/// @tparam ElementType Type of the element stored in Vector. 
	template<typename ElementType>
	struct Vector4t final
	{
	public:
		typedef typename Conditional<TypeTraits::IsPOD<ElementType>::Value, ElementType, ElementType const&>::Type ElementTypeConstRef;
		size_t static constexpr ComponentsCount = 4;

		union {
			ElementType Elements[ComponentsCount];
			struct { ElementType _0,   _1,  _2,    _3;     }; ///< Compatibility with D3D Math.
			struct { ElementType Left, Top, Width, Height; }; ///< Compatibility with Rectangle
			struct { ElementType x,    y,   z,     w;      }; ///< Representation in XYZW coordinate system
			struct { ElementType r,    g,   b,     a;      }; ///< Representation in RGBA color system
		};	// anonymous union

	public /* Constructors */:

		/// @brief Initializes a 4D vector with value {X:0, Y:0, Z:0, W:1}.
		GDINL explicit Vector4t()
			: _0(static_cast<ElementType>(0))
			, _1(static_cast<ElementType>(0))
			, _2(static_cast<ElementType>(0))
			, _3(static_cast<ElementType>(1))
		{
		}

		/// @brief Initializes a 4D vector.
		/// @param Vector4Value Value of the all coordinates of the vector.
		GDINL explicit Vector4t(ElementTypeConstRef const Vector4Value)
			: _0(Vector4Value)
			, _1(Vector4Value)
			, _2(Vector4Value)
			, _3(Vector4Value)
		{
		}

		/// @brief Initializes a 4D vector.
		/// @param Vector4Value0 Value of the first coordinate of the vector.
		/// @param Vector4Value1 Value of the second coordinate of the vector.
		/// @param Vector4Value2 Value of the third coordinate of the vector.
		/// @param Vector4Value3 Value of the fourth coordinate of the vector.
		GDINL Vector4t(ElementTypeConstRef const Vector4Value0, ElementTypeConstRef const Vector4Value1, ElementTypeConstRef const Vector4Value2, ElementTypeConstRef const Vector4Value3)
			: _0(Vector4Value0)
			, _1(Vector4Value1)
			, _2(Vector4Value2)
			, _3(Vector4Value3)
		{
		}

		/// @brief Initializes a 4D vector.
		/// @param Vector4Value0 Value of the first coordinate of the vector.
		/// @param Vector4Value1 Value of the second coordinate of the vector.
		/// @param Vector4Value23 Value of the third and fourth coordinates of the vector.
		GDINL Vector4t(ElementTypeConstRef const Vector4Value0, ElementTypeConstRef const Vector4Value1, Vector2t<ElementType> const& Vector4Value23)
			: _0(Vector4Value0)
			, _1(Vector4Value1)
			, _2(Vector4Value23._0)
			, _3(Vector4Value23._1)
		{
		}

		/// @brief Initializes a 4D vector.
		/// @param Vector4Value0 Value of the first coordinate of the vector.
		/// @param Vector4Value12 Value of the second and third coordinates of the vector.
		/// @param Vector4Value3 Value of the fourth coordinate of the vector.
		GDINL Vector4t(ElementTypeConstRef const Vector4Value0, Vector2t<ElementType> const& Vector4Value12, ElementTypeConstRef const Vector4Value3)
			: _0(Vector4Value0)
			, _1(Vector4Value12._0)
			, _2(Vector4Value12._1)
			, _3(Vector4Value3)
		{
		}

		/// @brief Initializes a 4D vector.
		/// @param Vector4Value0 Value of the first coordinate of the vector.
		/// @param Vector4Value123 Value of the second, third and fourth coordinates of the vector.
		GDINL Vector4t(ElementTypeConstRef const Vector4Value0, Vector3t<ElementType> const& Vector4Value123)
			: _0(Vector4Value0)
			, _1(Vector4Value123._0)
			, _2(Vector4Value123._1)
			, _3(Vector4Value123._2)
		{
		}

		/// @brief Initializes a 3D vector. Third coordinate is set to 0, fourth - to 1.
		/// @param Vector4Value01 Value of the first two coordinates of the vector.
		GDINL explicit Vector4t(Vector2t<ElementType> const& Vector4Value01)
			: _0(Vector4Value01._0)
			, _1(Vector4Value01._1)
			, _2(static_cast<ElementType>(0))
			, _3(static_cast<ElementType>(1))
		{
		}

		/// @brief Initializes a 4D vector.
		/// @param Vector4Value01 Value of the first two coordinates of the vector.
		/// @param Vector4Value2 Value of the third coordinate of the vector.
		/// @param Vector4Value3 Value of the fourth coordinate of the vector.
		GDINL Vector4t(Vector2t<ElementType> const& Vector4Value01, ElementTypeConstRef const Vector4Value2, ElementTypeConstRef const Vector4Value3)
			: _0(Vector4Value01._0)
			, _1(Vector4Value01._1)
			, _2(Vector4Value2)
			, _3(Vector4Value3)
		{
		}

		/// @brief Initializes a 4D vector.
		/// @param Vector4Value01 Value of the first two coordinates of the vector.
		/// @param Vector4Value23 Value of the third and fourth coordinates of the vector.
		GDINL Vector4t(Vector2t<ElementType> const& Vector4Value01, Vector2t<ElementType> const& Vector4Value23)
			: _0(Vector4Value01._0)
			, _1(Vector4Value01._1)
			, _2(Vector4Value23._0)
			, _3(Vector4Value23._1)
		{
		}

		/// @brief Initializes a 3D vector. Fourth is set to 1.
		/// @param Vector4Value012 Value of the first, second and third coordinates of the vector.
		GDINL explicit Vector4t(Vector2t<ElementType> const& Vector4Value012)
			: _0(Vector4Value012._0)
			, _1(Vector4Value012._1)
			, _2(Vector4Value012._2)
			, _3(static_cast<ElementType>(1))
		{
		}

		/// @brief Initializes a 4D vector.
		/// @param Vector4Value012 Value of the first, second and third coordinates of the vector.
		/// @param Vector4Value3 Value of the fourth coordinate of the vector.
		GDINL Vector4t(Vector3t<ElementType> const& Vector4Value012, ElementTypeConstRef const Vector4Value3)
			: _0(Vector4Value012._0)
			, _1(Vector4Value012._1)
			, _2(Vector4Value012._2)
			, _3(Vector4Value3)
		{
		}

	public /* Class API */:

		/// @brief Computes dot product of two vectors.
		/// @returns Dot product of two vectors.
		GDINL ElementType Dot(Vector3t const& Other) const
		{
			return this->x * v.x + this->y * v.y + this->z * v.z + this->w * v.w;
		}

		GDINL Vector4t Cross(const Vector4t& v) const
		{
			return Vector4t(this->y * v.z - this->z * v.y, this->z * v.x - this->x * v.z, this->x * v.y - this->y * v.x, ElementType(1));
		}

		GDINL ElementType Length() const
		{
			return SquareRoot(this->Dot(*this));
		}

		GDINL Vector4t Normalize() const
		{
			return ((*this) / this->Length());
		}

	public /* Operators */:
		GDINL ElementTypeConstRef operator[] (size_t const Index) const
		{
			GD_ASSERT(Index < ComponentsCount, "invalid Vector4t subindex.");
			return this->Elements[Index];
		}

		GDINL bool operator== (Vector4t const& Other) const
		{
			if (this->x == Other.x) {
				if (this->y == Other.y) {
					if (this->z == Other.z) {
						if (this->w == Other.w) {
							return true;
						}
					}
				}
			}
			return false;
		}

		GDINL bool operator!= (Vector4t const& Other) const
		{
			return !((*this) == Other);
		}

		GDINL Vector4t& operator+= (ElementTypeConstRef const Vector4Value)
		{
			this->x += Vector4Value;
			this->y += Vector4Value;
			this->z += Vector4Value;
			this->w += Vector4Value;
			return (*this);
		}

		GDINL Vector4t& operator-= (ElementTypeConstRef const Vector4Value)
		{
			this->x -= Vector4Value;
			this->y -= Vector4Value;
			this->z -= Vector4Value;
			this->w -= Vector4Value;
			return (*this);
		}

		GDINL Vector4t& operator*= (ElementTypeConstRef const Vector4Value)
		{
			this->x *= Vector4Value;
			this->y *= Vector4Value;
			this->z *= Vector4Value;
			this->w *= Vector4Value;
			return (*this);
		}

		GDINL Vector4t& operator/= (ElementTypeConstRef const Vector4Value)
		{
			this->x /= Vector4Value;
			this->y /= Vector4Value;
			this->z /= Vector4Value;
			this->w /= Vector4Value;
			return (*this);
		}

		template<typename = typename EnableIf<TypeTraits::IsIntegral<ElementType>::Value>::Type>
		GDINL Vector4t& operator%= (ElementTypeConstRef const Vector4Value)
		{
			this->x %= Vector4Value;
			this->y %= Vector4Value;
			this->z %= Vector4Value;
			this->w %= Vector4Value;
			return (*this);
		}

		GDINL Vector4t& operator = (Vector4t const& Other)
		{
			this->x = Other.x;
			this->y = Other.y;
			this->z = Other.z;
			this->w = Other.w;
			return (*this);
		}

		GDINL Vector4t& operator+= (Vector4t const& Other)
		{
			this->x += Other.x;
			this->y += Other.y;
			this->z += Other.z;
			this->w += Other.w;
			return (*this);
		}

		GDINL Vector4t& operator-= (Vector4t const& Other)
		{
			this->x -= Other.x;
			this->y -= Other.y;
			this->z -= Other.z;
			this->w -= Other.w;
			return (*this);
		}

		GDINL Vector4t& operator*= (Vector4t const& Other)
		{
			this->x *= Other.x;
			this->y *= Other.y;
			this->z *= Other.z;
			this->w *= Other.w;
			return (*this);
		}

		GDINL Vector4t& operator/= (Vector4t const& Other)
		{
			this->x /= Other.x;
			this->y /= Other.y;
			this->z /= Other.z;
			this->w /= Other.w;
			return (*this);
		}

		template<typename = typename EnableIf<TypeTraits::IsIntegral<ElementType>::Value>::Type>
		GDINL Vector4t& operator%= (Vector4t const& Other)
		{
			this->x %= Other.x;
			this->y %= Other.y;
			this->z %= Other.z;
			this->w %= Other.w;
			return (*this);
		}

		GDINL Vector4t& operator++ ()
		{
			++this->x;
			++this->y;
			++this->z;
			++this->w;
			return (*this);
		}

		GDINL Vector4t& operator-- ()
		{
			--this->x;
			--this->y;
			--this->z;
			--this->w;
			return (*this);
		}

		GDINL Vector4t& operator++ (int)
		{
			return (++(*this));
		}

		GDINL Vector4t& operator-- (int)
		{
			return (--(*this));
		}

		GDINL Vector4t operator+ (ElementTypeConstRef const Vector4Value) const
		{
			return Vector4t(this->x + Vector4Value, this->y + Vector4Value, this->z + Vector4Value, this->w + Vector4Value);
		}

		GDINL Vector4t operator- (ElementTypeConstRef const Vector4Value) const
		{
			return Vector4t(this->x - Vector4Value, this->y - Vector4Value, this->z - Vector4Value, this->w - Vector4Value);
		}

		GDINL Vector4t operator* (ElementTypeConstRef const Vector4Value) const
		{
			return Vector4t(this->x * Vector4Value, this->y * Vector4Value, this->z * Vector4Value, this->w * Vector4Value);
		}

		GDINL Vector4t operator/ (ElementTypeConstRef const Vector4Value) const
		{
			return Vector4t(this->x / Vector4Value, this->y / Vector4Value, this->z / Vector4Value, this->w / Vector4Value);
		}

		template<typename = typename EnableIf<TypeTraits::IsIntegral<ElementType>::Value>::Type>
		GDINL Vector4t operator% (ElementTypeConstRef const Vector4Value) const
		{
			return Vector4t(this->x % Vector4Value, this->y % Vector4Value, this->z % Vector4Value, this->w % Vector4Value);
		}

		GDINL Vector4t operator+ (Vector4t const& Other) const
		{
			return Vector4t(this->x + Other.x, this->y + Other.y, this->z + Other.z, this->w + Other.w);
		}

		GDINL Vector4t operator- (Vector4t const& Other) const
		{
			return Vector4t(this->x - Other.x, this->y - Other.y, this->z - Other.z, this->w - Other.w);
		}

		GDINL Vector4t operator* (Vector4t const& Other) const
		{
			return Vector4t(this->x * Other.x, this->y * Other.y, this->z * Other.z, this->w * Other.w);
		}

		GDINL Vector4t operator/ (Vector4t const& Other) const
		{
			return Vector4t(this->x / Other.x, this->y / Other.y, this->z / Other.z, this->w / Other.w);
		}

		template<typename = typename EnableIf<TypeTraits::IsIntegral<ElementType>::Value>::Type>
		GDINL Vector4t operator% (Vector4t const& Other) const
		{
			return Vector4t(this->x % Other.x, this->y % Other.y, this->z % Other.z, this->w % Other.w);
		}
	};	// struct Vector4t

#if (defined(GD_HAS_FLOAT32X4INTRINSICS) && (!defined(GD_DOCUMENTATION)))
	template<>
	struct GD_ALIGN_MSVC(16) Vector4t<Float32> final
	{
		enum : size_t { ComponentsCount = 4 };
		union {
			Float32x4Intrinsics::VectorRegisterType ElementsVector;
			Float32 Elements[ComponentsCount];
			struct { Float32 Left, Top, _3idth, Height; };
			struct { Float32 x, _1,   z,  w;   };
			struct { Float32 r, g,   b,  a;   };
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
			return Float32x4Intrinsics::VectorLength(this->ElementsVector);
		}

		GDINL Vector4t Normalize() const
		{
			return Vector4t(Float32x4Intrinsics::VectorNormalize(this->ElementsVector));
		}

		GDINL Float32 Dot(Vector4t const& Other) const
		{
			return Float32x4Intrinsics::VectorDot4(this->ElementsVector, Other.ElementsVector);
		}

		GDINL Vector4t Cross(Vector4t const& Other) const
		{
			return Vector4t(Float32x4Intrinsics::Vector3Cross(this->ElementsVector, Other.ElementsVector));
		}

	public /* Operators */:
		GDINL Float32& operator[] (size_t const Index)
		{
			GD_ASSERT(Index < ComponentsCount, "invalid Vector4 subindex.");
			return this->Elements[Index];
		}

		GDINL Float32 const& operator[] (size_t const Index) const
		{
			return const_cast<Vector4t&>(*this)[Index];
		}

#if (defined(GD_COMPILER_MSVC))
#	pragma region [--- _= Operators ---]
#endif	// if (defined(GD_COMPILER_MSVC))

		GDINL Vector4t& operator+= (Float32 const Vector4Value)
		{
			this->ElementsVector = Float32x4Intrinsics::VectorAdd(this->ElementsVector, Float32x4Intrinsics::VectorMake(Vector4Value));
			return (*this);
		}

		GDINL Vector4t& operator-= (Float32 const Vector4Value)
		{
			this->ElementsVector = Float32x4Intrinsics::VectorSub(this->ElementsVector, Float32x4Intrinsics::VectorMake(Vector4Value));
			return (*this);
		}

		GDINL Vector4t& operator*= (Float32 const Vector4Value)
		{
			this->ElementsVector = Float32x4Intrinsics::VectorMul(this->ElementsVector, Float32x4Intrinsics::VectorMake(Vector4Value));
			return (*this);
		}

		GDINL Vector4t& operator/= (Float32 const Vector4Value)
		{
			this->ElementsVector = Float32x4Intrinsics::VectorDiv(this->ElementsVector, Float32x4Intrinsics::VectorMake(Vector4Value));
			return (*this);
		}

		GDINL Vector4t& operator = (Vector4t const& Other)
		{
			this->ElementsVector = Other.ElementsVector;
			return (*this);
		}

		GDINL Vector4t& operator+= (Vector4t const& Other)
		{
			this->ElementsVector = Float32x4Intrinsics::VectorAdd(this->ElementsVector, Other.ElementsVector);
			return (*this);
		}

		GDINL Vector4t& operator-= (Vector4t const& Other)
		{
			this->ElementsVector = Float32x4Intrinsics::VectorSub(this->ElementsVector, Other.ElementsVector);
			return (*this);
		}

		GDINL Vector4t& operator*= (Vector4t const& Other)
		{
			this->ElementsVector = Float32x4Intrinsics::VectorMul(this->ElementsVector, Other.ElementsVector);
			return (*this);
		}

		GDINL Vector4t& operator/= (Vector4t const& Other)
		{
			this->ElementsVector = Float32x4Intrinsics::VectorDiv(this->ElementsVector, Other.ElementsVector);
			return (*this);
		}

		GDINL Vector4t& operator++ ()
		{
			this->ElementsVector = Float32x4Intrinsics::VectorAdd(this->ElementsVector, Float32x4Intrinsics::VectorMake(1.0f));
			return (*this);
		}

		GDINL Vector4t& operator-- ()
		{
			this->ElementsVector = Float32x4Intrinsics::VectorAdd(this->ElementsVector, Float32x4Intrinsics::VectorMake(1.0f));
			return (*this);
		}

		GDINL Vector4t& operator++ (int)
		{
			return (++(*this));
		}

		GDINL Vector4t& operator-- (int)
		{
			return (--(*this));
		}

#if (defined(GD_COMPILER_MSVC))
#	pragma endregion
#endif	// if (defined(GD_COMPILER_MSVC))

#if (defined(GD_COMPILER_MSVC))
#	pragma region [--- _ Operators ---]
#endif	// if (defined(GD_COMPILER_MSVC))

		GDINL Vector4t operator+ (Float32 const Vector4Value) const
		{
			return Vector4t(Float32x4Intrinsics::VectorAdd(this->ElementsVector, Float32x4Intrinsics::VectorMake(Vector4Value)));
		}

		GDINL Vector4t operator- (Float32 const Vector4Value) const
		{
			return Vector4t(Float32x4Intrinsics::VectorSub(this->ElementsVector, Float32x4Intrinsics::VectorMake(Vector4Value)));
		}

		GDINL Vector4t operator* (Float32 const Vector4Value) const
		{
			return Vector4t(Float32x4Intrinsics::VectorMul(this->ElementsVector, Float32x4Intrinsics::VectorMake(Vector4Value)));
		}

		GDINL Vector4t operator/ (Float32 const Vector4Value) const
		{
			return Vector4t(Float32x4Intrinsics::VectorDiv(this->ElementsVector, Float32x4Intrinsics::VectorMake(Vector4Value)));
		}

		GDINL Vector4t operator+ (Vector4t const& Other) const
		{
			return Vector4t(Float32x4Intrinsics::VectorAdd(this->ElementsVector, Other.ElementsVector));
		}

		GDINL Vector4t operator- (Vector4t const& Other) const
		{
			return Vector4t(Float32x4Intrinsics::VectorSub(this->ElementsVector, Other.ElementsVector));
		}

		GDINL Vector4t operator* (Vector4t const& Other) const
		{
			return Vector4t(Float32x4Intrinsics::VectorMul(this->ElementsVector, Other.ElementsVector));
		}

		GDINL Vector4t operator/ (Vector4t const& Other) const
		{
			return Vector4t(Float32x4Intrinsics::VectorDiv(this->ElementsVector, Other.ElementsVector));
		}

#if (defined(GD_COMPILER_MSVC))
#	pragma endregion
#endif	// if (defined(GD_COMPILER_MSVC))
	} GD_ALIGN_GCC(16);	// struct Vector4t<Float32, Comparator>
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

GD_NAMESPACE_END

#endif
