/// ==========================================================================================
/// Vector3.h: Three-dimensional vector class
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// 
/// History:
///		* 07.06.2014  - Rewritten from scratch by James Jhuighuy,
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_MATH_VECTOR3
#define GD_CORE_MATH_VECTOR3

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/TypeTraits.h>
#include <GoddamnEngine/Core/Math/Vector2.h>
#include <GoddamnEngine/Core/Diagnostics/Assertion/Assertion.h>

GD_NAMESPACE_BEGIN

	/// Three-dimensional vector class.
	///  Y ^
	/// | / Z
	/// |/
	/// ---0------> X
	///   /|
	template<typename ElementType>
	struct Vector3t final
	{
	public:
		typedef typename Conditional<TypeTraits::IsPOD<ElementType>::Value, ElementType, ElementType const&>::Type ElementTypeConstRef;
		enum : size_t { ThisComponentsCount = 3 };

		union {
			ElementType Elements[ThisComponentsCount];
			struct { ElementType Width, Height, Depth; }; ///< Representation as dimensional system
			struct { ElementType x,  y,   z;  }; ///< Representation in XYZ coordinate system
			struct { ElementType r,  g,   b;  }; ///< Representation in RGB color system
			struct { ElementType u,  v,   w;  }; ///< Representation in UVW coordinate system
			struct { ElementType _0, _1, _2; };
		};	// anonymous union

	public /* Constructors */:
		GDINL Vector3t(ElementTypeConstRef const Vector3Value0, ElementTypeConstRef const Vector3Value1, ElementTypeConstRef const Vector3Value2)
			: x(Vector3Value0)
			, y(Vector3Value1)
			, z(Vector3Value2)
		{
		}

		GDINL Vector3t(Vector2t<ElementType> const& Vector3Value01, ElementTypeConstRef const Vector3Value2)
			: x(Vector3Value01.x)
			, y(Vector3Value01.y)
			, z(Vector3Value2)
		{
		}

		GDINL Vector3t(ElementTypeConstRef const Vector3Value0, Vector2t<ElementType> const& Vector3Value12)
			: x(Vector3Value0)
			, y(Vector3Value12.x)
			, z(Vector3Value12.y)
		{
		}

		GDINL Vector3t(ElementTypeConstRef const Vector3Value = ElementType(0))
			: x(Vector3Value)
			, y(Vector3Value)
			, z(Vector3Value)
		{
		}

		GDINL Vector3t(Vector3t const& Other)
			: x(Other.x)
			, y(Other.y)
			, z(Other.z)
		{
		}

		GDINL ~Vector3t()
		{
		}

	public /* Class API */:
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
		GDINL ElementTypeConstRef operator[] (size_t const Index) const
		{
			GD_ASSERT(Index < ThisComponentsCount, "invalid Vector3t subindex.");
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
	typedef Vector3t<size_t > size_tx3;
	typedef Float32x3 Vector3;

GD_NAMESPACE_END

#endif
