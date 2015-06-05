//! ==========================================================================================
//! Vector3.h: Three-dimensional vector class
//! Copyright (C) Goddamn Industries 2015. All Rights Reserved.
//! ==========================================================================================

#pragma once
#ifndef GD_CORE_MATH_VECTOR3
#define GD_CORE_MATH_VECTOR3

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/TypeTraits.h>

#include <GoddamnEngine/Core/Math/Math.h>
#include <GoddamnEngine/Core/Math/Vector2.h>

GD_NAMESPACE_BEGIN

	//! Three-dimensional vector class.
	//! @code
	//!  Y ^  > Z
	//!    | / 
	//!    |/
	//! ---0------> X
	//!   /|
	//! @endcode
	//! @tparam ElementType Type of the element stored in Vector. 
	template<typename ElementType>
	struct Vector3t final
	{
	public:
		typedef typename Conditional<TypeTraits::IsPOD<ElementType>::Value, ElementType, ElementType const&>::Type ElementTypeConstRef;
		SizeTp static constexpr ComponentsCount = 3;

		union {
			ElementType Elements[ComponentsCount];
			struct { ElementType Width, Height, Depth; }; //!< Representation as dimensional system
			struct { ElementType _0,    _1,     _2;    }; //!< Compatibility with D3D Math.
			struct { ElementType x,     y,      z;     }; //!< Representation in XYZ coordinate system
			struct { ElementType r,     g,      b;     }; //!< Representation in RGB color system
			struct { ElementType u,     v,      w;     }; //!< Representation in UVW coordinate system
		};	// anonymous union

	public:

		//! Initializes a 2D vector with value {X:0, Y:0, Z:0}.
		GDINL explicit Vector3t()
			: _0(static_cast<ElementType>(0))
			, _1(static_cast<ElementType>(0))
			, _2(static_cast<ElementType>(0))
		{
		}

		//! Initializes a 3D vector.
		//! @param Vector3Value Value of the all coordinates of the vector.
		GDINL explicit Vector3t(ElementTypeConstRef const Vector3Value)
			: _0(Vector3Value)
			, _1(Vector3Value)
			, _2(Vector3Value)
		{
		}

		//! Initializes a 3D vector.
		//! @param Vector3Value0 Value of the first coordinate of the vector.
		//! @param Vector3Value1 Value of the second coordinate of the vector.
		//! @param Vector3Value2 Value of the third coordinate of the vector.
		GDINL Vector3t(ElementTypeConstRef const Vector3Value0, ElementTypeConstRef const Vector3Value1, ElementTypeConstRef const Vector3Value2)
			: _0(Vector3Value0)
			, _1(Vector3Value1)
			, _2(Vector3Value2)
		{
		}

		//! Initializes a 3D vector.
		//! @param Vector3Value0 Value of the first coordinate of the vector.
		//! @param Vector3Value12 Value of the second and third coordinates of the vector.
		GDINL Vector3t(ElementTypeConstRef const Vector3Value0, Vector2t<ElementType> const& Vector3Value12)
			: _0(Vector3Value0)
			, _1(Vector3Value12._0)
			, _2(Vector3Value12._1)
		{
		}

		//! Initializes a 3D vector. Third coordinate is set to 0.
		//! @param Vector3Value01 Value of the first two coordinates of the vector.
		GDINL Vector3t(Vector2t<ElementType> const& Vector3Value01)
			: _0(Vector3Value01._0)
			, _1(Vector3Value01._1)
			, _2(static_cast<ElementType>(0))
		{
		}

		//! Initializes a 3D vector.
		//! @param Vector3Value01 Value of the first two coordinates of the vector.
		//! @param Vector3Value2 Value of the third coordinate of the vector.
		GDINL Vector3t(Vector2t<ElementType> const& Vector3Value01, ElementTypeConstRef const Vector3Value2)
			: _0(Vector3Value01._0)
			, _1(Vector3Value01._1)
			, _2(Vector3Value2)
		{
		}

	public /* Class API */:

		//! Computes dot product of two vectors.
		//! @returns Dot product of two vectors.
		GDINL ElementType Dot(Vector3t const& Other) const
		{
			return this->x * Other.x + this->y * Other.y + this->z * Other.z;
		}

		GDINL Vector3t Cross(Vector3t const& Other) const
		{
			return Vector3t(this->y * Other.z - this->z * Other.y, this->z * Other.x - this->x * Other.z, this->x * Other.y - this->y * Other.x);
		}

		GDINL ElementType Length() const
		{
			return SquareRoot(this->Dot(*this));
		}

		GDINL Vector3t Normalize() const
		{
			return ((*this) / this->Length());
		}

	public /* Operators */:

		GDINL ElementTypeConstRef operator[] (SizeTp const Index) const
		{
			GD_ASSERT(Index < ComponentsCount, "invalid Vector3t subindex.");
			return this->Elements[Index];
		}

		GDINL bool operator== (Vector3t const& Other) const
		{
			if (this->x == Other.x)
				if (this->y == Other.y)
					if (this->z == Other.z)
						return true;
			return false;
		}

		GDINL bool operator!= (Vector3t const& Other) const
		{
			return !((*this) == Other);
		}

#if (defined(GD_COMPILER_MSVC))
#	pragma region [--- _= Operators ---]
#endif	// if (defined(GD_COMPILER_MSVC))

		GDINL Vector3t& operator+= (ElementTypeConstRef const Vector3Value)
		{
			this->x += Vector3Value;
			this->y += Vector3Value;
			this->z += Vector3Value;
			return (*this);
		}

		GDINL Vector3t& operator-= (ElementTypeConstRef const Vector3Value)
		{
			this->x -= Vector3Value;
			this->y -= Vector3Value;
			this->z -= Vector3Value;
			return (*this);
		}

		GDINL Vector3t& operator*= (ElementTypeConstRef const Vector3Value)
		{
			this->x *= Vector3Value;
			this->y *= Vector3Value;
			this->z *= Vector3Value;
			return (*this);
		}

		GDINL Vector3t& operator/= (ElementTypeConstRef const Vector3Value)
		{
			this->x /= Vector3Value;
			this->y /= Vector3Value;
			this->z /= Vector3Value;
			return (*this);
		}

		template<typename = typename EnableIf<TypeTraits::IsIntegral<ElementType>::Value>::Type>
		GDINL Vector3t& operator%= (ElementTypeConstRef const Vector3Value)
		{
			this->x %= Vector3Value;
			this->y %= Vector3Value;
			this->z %= Vector3Value;
			return (*this);
		}

		GDINL Vector3t& operator = (Vector3t const& Other)
		{
			this->x = Other.x;
			this->y = Other.y;
			this->z = Other.z;
			return (*this);
		}

		GDINL Vector3t& operator+= (Vector3t const& Other)
		{
			this->x += Other.x;
			this->y += Other.y;
			this->z += Other.z;
			return (*this);
		}

		GDINL Vector3t& operator-= (Vector3t const& Other)
		{
			this->x -= Other.x;
			this->y -= Other.y;
			this->z -= Other.z;
			return (*this);
		}

		GDINL Vector3t& operator*= (Vector3t const& Other)
		{
			this->x *= Other.x;
			this->y *= Other.y;
			this->z *= Other.z;
			return (*this);
		}

		GDINL Vector3t& operator/= (Vector3t const& Other)
		{
			this->x /= Other.x;
			this->y /= Other.y;
			this->z /= Other.z;
			return (*this);
		}

		template<typename = typename EnableIf<TypeTraits::IsIntegral<ElementType>::Value>::Type>
		GDINL Vector3t& operator%= (Vector3t const& Other)
		{
			this->x %= Other.x;
			this->y %= Other.y;
			this->z %= Other.z;
			return (*this);
		}

		GDINL Vector3t& operator++ ()
		{
			++this->x;
			++this->y;
			++this->z;
			return (*this);
		}

		GDINL Vector3t& operator-- ()
		{
			--this->x;
			--this->y;
			--this->z;
			return (*this);
		}

		GDINL Vector3t& operator++ (int)
		{
			return (++(*this));
		}

		GDINL Vector3t& operator-- (int)
		{
			return (--(*this));
		}

#if (defined(GD_COMPILER_MSVC))
#	pragma endregion
#endif	// if (defined(GD_COMPILER_MSVC))

#if (defined(GD_COMPILER_MSVC))
#	pragma region [--- _ Operators ---]
#endif	// if (defined(GD_COMPILER_MSVC))

		GDINL Vector3t operator+ (ElementTypeConstRef const Vector3Value) const
		{
			return Vector3t(this->x + Vector3Value, this->y + Vector3Value, this->z + Vector3Value);
		}

		GDINL Vector3t operator- (ElementTypeConstRef const Vector3Value) const
		{
			return Vector3t(this->x - Vector3Value, this->y - Vector3Value, this->z - Vector3Value);
		}

		GDINL Vector3t operator* (ElementTypeConstRef const Vector3Value) const
		{
			return Vector3t(this->x * Vector3Value, this->y * Vector3Value, this->z * Vector3Value);
		}

		GDINL Vector3t operator/ (ElementTypeConstRef const Vector3Value) const
		{
			return Vector3t(this->x / Vector3Value, this->y / Vector3Value, this->z / Vector3Value);
		}

		template<typename = typename EnableIf<TypeTraits::IsIntegral<ElementType>::Value>::Type>
		GDINL Vector3t operator% (ElementTypeConstRef const Vector3Value) const
		{
			return Vector3t(this->x % Vector3Value, this->y % Vector3Value, this->z % Vector3Value);
		}

		GDINL Vector3t operator+ (Vector3t const& Other) const
		{
			return Vector3t(this->x + Other.x, this->y + Other.y, this->z + Other.z);
		}

		GDINL Vector3t operator- (Vector3t const& Other) const
		{
			return Vector3t(this->x - Other.x, this->y - Other.y, this->z - Other.z);
		}

		GDINL Vector3t operator* (Vector3t const& Other) const
		{
			return Vector3t(this->x * Other.x, this->y * Other.y, this->z * Other.z);
		}

		GDINL Vector3t operator/ (Vector3t const& Other) const
		{
			return Vector3t(this->x / Other.x, this->y / Other.y, this->z / Other.z);
		}

		template<typename = typename EnableIf<TypeTraits::IsIntegral<ElementType>::Value>::Type>
		GDINL Vector3t operator% (Vector3t const& Other) const
		{
			return Vector3t(this->x % Other.x, this->y % Other.y, this->z % Other.z);
		}

#if (defined(GD_COMPILER_MSVC))
#	pragma endregion
#endif	// if (defined(GD_COMPILER_MSVC))
	};	// struct Vector3t

	typedef Vector3t< Int8  >  Int8x3;
	typedef Vector3t<UInt8  > UInt8x3;
	typedef Vector3t< Int16 >  Int16x3;
	typedef Vector3t<UInt16 > UInt16x3;
	typedef Vector3t< Int32 >  Int32x3;
	typedef Vector3t<UInt32 > UInt32x3;
	typedef Vector3t< Int64 >  Int64x3;
	typedef Vector3t<UInt64 > UInt64x3;
	typedef Vector3t<Float32> Float32x3;
	typedef Vector3t<Float64> Float64x3;
	typedef Vector3t<SizeTp > size_tx3;
	typedef Float32x3 Vector3;

GD_NAMESPACE_END

#endif
