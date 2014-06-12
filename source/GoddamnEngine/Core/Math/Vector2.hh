//////////////////////////////////////////////////////////////////////////
/// Vector2.h: Two-dimensional vector class
/// Copyright (C) $(GD_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 07.06.2014  - Rewritten from scratch by James Jhuighuy,
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_CORE_MATH_VECTOR2
#define GD_CORE_MATH_VECTOR2

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/TypeTraits.hh>
#include <GoddamnEngine/Core/Diagnostics/Assertion/Assertion.hh>

GD_NAMESPACE_BEGIN

	/// Two-dimensional vector class.
	///  Y ^ 
	///    |
	///    |
	/// ---0------> X
	///    |
	template<typename ElementType>
	struct Vector2t final
	{
		typedef typename Conditional<TypeTraits::IsPodType<ElementType>::Value, ElementType, ElementType const&>::Type ElementTypeConstRef;
		typedef typename Conditional<TypeTraits::IsPodType<ElementType>::Value, ElementType, ElementType      &>::Type ElementTypeMutRef;
		enum : size_t { ThisComponentsCount = 2 };

		union {
			ElementType Elements[ThisComponentsCount];
			struct { ElementType _0,   _1;    }; ///< Compatibility with D3D Math.
			struct { ElementType Width, Height; }; ///< Representation as dimensional system
			struct { ElementType Min,   Max;    }; ///< Representation as range
			struct { ElementType x,     y;      }; ///< Representation in XY coordinate system
			struct { ElementType r,     g;      }; ///< Representation in RG color system
			struct { ElementType u,     v;      }; ///< Representation in UV coordinate system
		};	// anonymous union

	public /* Constructors */:
		GDINL Vector2t(ElementTypeConstRef const Vector2Value0, ElementTypeConstRef const Vector2Value1)
			: x(Vector2Value0)
			, y(Vector2Value1)
		{
		}

		GDINL explicit Vector2t(ElementTypeConstRef const Vector2Value = ElementType(0))
			: x(Vector2Value)
			, y(Vector2Value)
		{
		}

		GDINL Vector2t(Vector2t const& Other)
			: x(Other.x)
			, y(Other.y)
		{
		}

		GDINL ~Vector2t()
		{
		}

	public /* Class API */:
		GDINL ElementType Length() const
		{
			return SquareRoot(self->Dot(*self));
		}

		GDINL Vector2t Normalize() const
		{
			return ((*self) / self->Length());
		}

		GDINL ElementType Dot(Vector2t const& Other) const
		{
			return (self->x * Other.x + self->y * Other.y);
		}

	public /* Operators */:
		GDINL ElementTypeConstRef operator[] (size_t const Index) const
		{
			GD_ASSERT(Index < ThisComponentsCount, "invalid Vector2t subindex.");
			return self->Elements[Index];
		}

		GDINL bool operator== (Vector2t const& Other) const
		{
			if (self->x == Other.x)
				if (self->y == Other.y)
					return true;
			return false;
		}

		GDINL bool operator!= (Vector2t const& Other) const
		{
			return !((*self) == Other);
		}

#if (defined(GD_COMPILER_MSC))
#	pragma region [--- _= Operators ---]
#endif	// if (defined(GD_COMPILER_MSC))

		GDINL Vector2t& operator+= (ElementTypeConstRef const Vector2Value)
		{
			self->x += Vector2Value;
			self->y += Vector2Value;
			return (*self);
		}

		GDINL Vector2t& operator-= (ElementTypeConstRef const Vector2Value)
		{
			self->x -= Vector2Value;
			self->y -= Vector2Value;
			return (*self);
		}

		GDINL Vector2t& operator*= (ElementTypeConstRef const Vector2Value)
		{
			self->x *= Vector2Value;
			self->y *= Vector2Value;
			return (*self);
		}

		GDINL Vector2t& operator/= (ElementTypeConstRef const Vector2Value)
		{
			self->x /= Vector2Value;
			self->y /= Vector2Value;
			return (*self);
		}

		template<typename = typename EnableIf<TypeTraits::IsIntegral<ElementType>::Value>::Type>
		GDINL Vector2t& operator%= (ElementTypeConstRef const Vector2Value)
		{
			self->x %= Vector2Value;
			self->y %= Vector2Value;
			return (*self);
		}

		GDINL Vector2t& operator = (Vector2t const& Other)
		{
			self->x = Other.x;
			self->y = Other.y;
			return (*self);
		}

		GDINL Vector2t& operator+= (Vector2t const& Other)
		{
			self->x += Other.x;
			self->y += Other.y;
			return (*self);
		}

		GDINL Vector2t& operator-= (Vector2t const& Other)
		{
			self->x -= Other.x;
			self->y -= Other.y;
			return (*self);
		}

		GDINL Vector2t& operator*= (Vector2t const& Other)
		{
			self->x *= Other.x;
			self->y *= Other.y;
			return (*self);
		}

		GDINL Vector2t& operator/= (Vector2t const& Other)
		{
			self->x /= Other.x;
			self->y /= Other.y;
			return (*self);
		}

		template<typename = typename EnableIf<TypeTraits::IsIntegral<ElementType>::Value>::Type>
		GDINL Vector2t& operator%= (Vector2t const& Other)
		{
			self->x %= Other.x;
			self->y %= Other.y;
			return (*self);
		}

		GDINL Vector2t& operator++ ()
		{
			++self->x;
			++self->y;
			return (*self);
		}

		GDINL Vector2t& operator-- ()
		{
			--self->x;
			--self->y;
			return (*self);
		}

		GDINL Vector2t& operator++ (int)
		{
			return (++(*self));
		}

		GDINL Vector2t& operator-- (int)
		{
			return (--(*self));
		}

#if (defined(GD_COMPILER_MSC))
#	pragma endregion
#endif	// if (defined(GD_COMPILER_MSC))

#if (defined(GD_COMPILER_MSC))
#	pragma region [--- _ Operators ---]
#endif	// if (defined(GD_COMPILER_MSC))

		GDINL Vector2t operator+ (ElementTypeConstRef const Vector2Value) const
		{
			return Vector2t(self->x + Vector2Value, self->y + Vector2Value);
		}

		GDINL Vector2t operator- (ElementTypeConstRef const Vector2Value) const
		{
			return Vector2t(self->x - Vector2Value, self->y - Vector2Value);
		}

		GDINL Vector2t operator* (ElementTypeConstRef const Vector2Value) const
		{
			return Vector2t(self->x * Vector2Value, self->y * Vector2Value);
		}

		GDINL Vector2t operator/ (ElementTypeConstRef const Vector2Value) const
		{
			return Vector2t(self->x / Vector2Value, self->y / Vector2Value);
		}

		template<typename = typename EnableIf<TypeTraits::IsIntegral<ElementType>::Value>::Type>
		GDINL Vector2t operator% (ElementTypeConstRef const Vector2Value) const
		{
			return Vector2t(self->x % Vector2Value, self->y % Vector2Value);
		}

		GDINL Vector2t operator+ (Vector2t const& Other) const
		{
			return Vector2t(self->x + Other.x, self->y + Other.y);
		}

		GDINL Vector2t operator- (Vector2t const& Other) const
		{
			return Vector2t(self->x - Other.x, self->y - Other.y);
		}

		GDINL Vector2t operator* (Vector2t const& Other) const
		{
			return Vector2t(self->x * Other.x, self->y * Other.y);
		}

		GDINL Vector2t operator/ (Vector2t const& Other) const
		{
			return Vector2t(self->x / Other.x, self->y / Other.y);
		}

		template<typename = typename EnableIf<TypeTraits::IsIntegral<ElementType>::Value>::Type>
		GDINL Vector2t operator% (Vector2t const& Other) const
		{
			return Vector2t(self->x % Other.x, self->y % Other.y);
		}

#if (defined(GD_COMPILER_MSC))
#	pragma endregion
#endif	// if (defined(GD_COMPILER_MSC))
	};	// struct Vector2t

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
	typedef Vector2t<size_t > size_tx2;
	typedef Float32x2 Vector2;

	typedef long Dimension;
	typedef Vector2t<Dimension> Resolution;
	typedef Vector2t<Dimension> Position;

GD_NAMESPACE_END

#endif