//! ==========================================================================================
//! Vector2.h: Two-dimensional vector class
//! Copyright (C) Goddamn Industries 2015. All Rights Reserved.
//! ==========================================================================================

#pragma once
#ifndef GD_CORE_MATH_VECTOR2
#define GD_CORE_MATH_VECTOR2

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Math/Math.h>
#include <GoddamnEngine/Core/Templates/TypeTraits.h>

GD_NAMESPACE_BEGIN

	//! @brief Two-dimensional vector class.
	//! @code
	//!  Y ^ 
	//!    |
	//!    |
	//! ---0------> X
	//!    |
	//! @endcode
	//! @tparam ElementType Type of the element stored in Vector. 
	template<typename ElementType>
	class Vector2t final
	{
	public:
		typedef typename Conditional<TypeTraits::IsPOD<ElementType>::Value, ElementType, ElementType const&>::Type ElementTypeConstRef;
		typedef typename Conditional<TypeTraits::IsPOD<ElementType>::Value, ElementType, ElementType&>::Type ElementTypeMutRef;
		SizeTp static constexpr ComponentsCount = 2;

		union {
			ElementType Elements[ComponentsCount];
			struct { ElementType Left,  Top;    }; //!< Representation as dimensional system.
			struct { ElementType Width, Height; }; //!< Representation as dimensional system (2).
			struct { ElementType Min,   Max;    }; //!< Representation as range.
			struct { ElementType _0,   _1;      }; //!< Compatibility with D3D Math.
			struct { ElementType x,    y;       }; //!< Representation in XY coordinate system.
			struct { ElementType r,    g;       }; //!< Representation in RG color system.
			struct { ElementType u,    v;       }; //!< Representation in UV coordinate system.
		};	// anonymous union

	public:

		//! @brief Initializes a 2D vector with value {X:0, Y:0}.
		GDINL explicit Vector2t()
			: _0(static_cast<ElementType>(0))
			, _1(static_cast<ElementType>(0))
		{
		}

		//! @brief Initializes a 2D vector.
		//! @param Vector2Value Value of the both coordinates of the vector.
		GDINL explicit Vector2t(ElementTypeConstRef const Vector2Value)
			: _0(Vector2Value)
			, _1(Vector2Value)
		{
		}

		//! @brief Initializes a 2D vector.
		//! @param Vector2Value0 Value of the first coordinate of the vector.
		//! @param Vector2Value1 Value of the second coordinate of the vector.
		GDINL Vector2t(ElementTypeConstRef const Vector2Value0, ElementTypeConstRef const Vector2Value1)
			: _0(Vector2Value0)
			, _1(Vector2Value1)
		{
		}

	public:

		//! @brief Computes dot product of two vectors.
		//! @returns Dot product of two vectors.
		GDINL ElementType Dot(Vector2t const& Other) const
		{
			return (this->x * Other.x + this->y * Other.y);
		}

		//! @brief Computes length of the vector.
		//! @returns Length of the vector.
		GDINL ElementType Length() const
		{
			return SquareRoot(this->Dot(*this));
		}

		//! @brief Returns normalized version of this vector.
		//! @returns Normalized version of this vector.
		GDINL Vector2t Normalize() const
		{
			return (*this) / this->Length();
		}

	public /* Operators */:
		GDINL ElementTypeConstRef operator[] (SizeTp const Index) const
		{
			GD_ASSERT(Index < ComponentsCount, "invalid Vector2t subindex.");
			return this->Elements[Index];
		}

		GDINL bool operator== (Vector2t const& Other) const
		{
			if (this->x == Other.x) {
				if (this->y == Other.y) {
					return true;
				}
			}

			return false;
		}

		GDINL bool operator!= (Vector2t const& Other) const
		{
			return !((*this) == Other);
		}

		GDINL Vector2t& operator+= (ElementTypeConstRef const Vector2Value)
		{
			this->x += Vector2Value;
			this->y += Vector2Value;
			return (*this);
		}

		GDINL Vector2t& operator-= (ElementTypeConstRef const Vector2Value)
		{
			this->x -= Vector2Value;
			this->y -= Vector2Value;
			return (*this);
		}

		GDINL Vector2t& operator*= (ElementTypeConstRef const Vector2Value)
		{
			this->x *= Vector2Value;
			this->y *= Vector2Value;
			return (*this);
		}

		GDINL Vector2t& operator/= (ElementTypeConstRef const Vector2Value)
		{
			this->x /= Vector2Value;
			this->y /= Vector2Value;
			return (*this);
		}

		template<typename = typename EnableIf<TypeTraits::IsIntegral<ElementType>::Value>::Type>
		GDINL Vector2t& operator%= (ElementTypeConstRef const Vector2Value)
		{
			this->x %= Vector2Value;
			this->y %= Vector2Value;
			return (*this);
		}

		GDINL Vector2t& operator = (Vector2t const& Other)
		{
			this->x = Other.x;
			this->y = Other.y;
			return (*this);
		}

		GDINL Vector2t& operator+= (Vector2t const& Other)
		{
			this->x += Other.x;
			this->y += Other.y;
			return (*this);
		}

		GDINL Vector2t& operator-= (Vector2t const& Other)
		{
			this->x -= Other.x;
			this->y -= Other.y;
			return (*this);
		}

		GDINL Vector2t& operator*= (Vector2t const& Other)
		{
			this->x *= Other.x;
			this->y *= Other.y;
			return (*this);
		}

		GDINL Vector2t& operator/= (Vector2t const& Other)
		{
			this->x /= Other.x;
			this->y /= Other.y;
			return (*this);
		}

		template<typename = typename EnableIf<TypeTraits::IsIntegral<ElementType>::Value>::Type>
		GDINL Vector2t& operator%= (Vector2t const& Other)
		{
			this->x %= Other.x;
			this->y %= Other.y;
			return (*this);
		}

		GDINL Vector2t& operator++ ()
		{
			++this->x;
			++this->y;
			return (*this);
		}

		GDINL Vector2t& operator-- ()
		{
			--this->x;
			--this->y;
			return (*this);
		}

		GDINL Vector2t& operator++ (int)
		{
			return (++(*this));
		}

		GDINL Vector2t& operator-- (int)
		{
			return (--(*this));
		}

		GDINL Vector2t operator+ (ElementTypeConstRef const Vector2Value) const
		{
			return Vector2t(this->x + Vector2Value, this->y + Vector2Value);
		}

		GDINL Vector2t operator- (ElementTypeConstRef const Vector2Value) const
		{
			return Vector2t(this->x - Vector2Value, this->y - Vector2Value);
		}

		GDINL Vector2t operator* (ElementTypeConstRef const Vector2Value) const
		{
			return Vector2t(this->x * Vector2Value, this->y * Vector2Value);
		}

		GDINL Vector2t operator/ (ElementTypeConstRef const Vector2Value) const
		{
			return Vector2t(this->x / Vector2Value, this->y / Vector2Value);
		}

		template<typename = typename EnableIf<TypeTraits::IsIntegral<ElementType>::Value>::Type>
		GDINL Vector2t operator% (ElementTypeConstRef const Vector2Value) const
		{
			return Vector2t(this->x % Vector2Value, this->y % Vector2Value);
		}

		GDINL Vector2t operator+ (Vector2t const& Other) const
		{
			return Vector2t(this->x + Other.x, this->y + Other.y);
		}

		GDINL Vector2t operator- (Vector2t const& Other) const
		{
			return Vector2t(this->x - Other.x, this->y - Other.y);
		}

		GDINL Vector2t operator* (Vector2t const& Other) const
		{
			return Vector2t(this->x * Other.x, this->y * Other.y);
		}

		GDINL Vector2t operator/ (Vector2t const& Other) const
		{
			return Vector2t(this->x / Other.x, this->y / Other.y);
		}

		template<typename = typename EnableIf<TypeTraits::IsIntegral<ElementType>::Value>::Type>
		GDINL Vector2t operator% (Vector2t const& Other) const
		{
			return Vector2t(this->x % Other.x, this->y % Other.y);
		}
	};	// class Vector2t

	typedef Vector2t< Int8  >  Int8x2;
	typedef Vector2t<UInt8  > UInt8x2;
	typedef Vector2t< Int16 >  Int16x2;
	typedef Vector2t<UInt16 > UInt16x2;
	typedef Vector2t< Int32 >  Int32x2;
	typedef Vector2t<UInt32 > UInt32x2;
	typedef Vector2t< Int64 >  Int64x2;
	typedef Vector2t<UInt64 > UInt64x2;
	typedef Vector2t<Float32> Float32x2;
	typedef Vector2t<Float64> Float64x2;
	typedef Vector2t<SizeTp > size_tx2;
	typedef Float32x2 Vector2;

	typedef long Dimension;
	typedef Vector2t<Dimension> Resolution;
	typedef Vector2t<Dimension> Position;

GD_NAMESPACE_END

#endif