// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Base/TypeTraits.h
 * @note This file should be never directly included, please consider using <GoddamnEngine/Include.h> instead.
 * Compile-time types information and modifiers management.
 */
#pragma once

/*!
 * Defines bitwise operation for the enums.
 * @param TEnumType Enum type to define for.
 */
#define GD_ENUM_DEFINE_FLAG_OPERATORS(TEnumType) \
	extern "C++" \
	{ \
		GDINL static constexpr TEnumType operator~ (TEnumType const e) \
		{ \
			typedef GD::TypeTraits::Underlying<TEnumType>::Type TEnumInteger; \
			return static_cast<TEnumType>(~static_cast<TEnumInteger>(e)); \
		} \
		\
		GDINL static constexpr TEnumType operator| (TEnumType const lhs, TEnumType const rhs) \
		{ \
			typedef GD::TypeTraits::Underlying<TEnumType>::Type TEnumInteger; \
			return static_cast<TEnumType>(static_cast<TEnumInteger>(lhs) | static_cast<TEnumInteger>(rhs)); \
		} \
		GDINL static TEnumType& operator|= (TEnumType& lhs, TEnumType const rhs) \
		{\
			return lhs = (lhs | rhs); \
		} \
		\
		GDINL static constexpr TEnumType operator& (TEnumType const lhs, TEnumType const rhs) \
		{ \
			typedef GD::TypeTraits::Underlying<TEnumType>::Type TEnumInteger; \
			return static_cast<TEnumType>(static_cast<TEnumInteger>(lhs) & static_cast<TEnumInteger>(rhs)); \
		} \
		GDINL static TEnumType& operator&= (TEnumType& lhs, TEnumType const rhs) \
		{ \
			return lhs = (lhs & rhs); \
		} \
		\
		GDINL static constexpr TEnumType operator^ (TEnumType const lhs, TEnumType const rhs) \
		{ \
			typedef GD::TypeTraits::Underlying<TEnumType>::Type TEnumInteger; \
			return static_cast<TEnumType>(static_cast<TEnumInteger>(lhs) ^ static_cast<TEnumInteger>(rhs)); \
		} \
		GDINL static TEnumType& operator^= (TEnumType& lhs, TEnumType const rhs) \
		{ \
			return lhs = (lhs ^ rhs); \
		} \
	}

/*!
 * Defines bitwise operation for the enum classes.
 * @param TEnumType Enum class type to define for.
 */
#define GD_ENUM_CLASS_DEFINE_FLAG_OPERATORS(TEnumType) \
	GD_ENUM_DEFINE_FLAG_OPERATORS(TEnumType) \
	extern "C++" \
	{ \
		GDINL static constexpr bool operator== (TEnumType const e, typename GD::TypeTraits::Underlying<TEnumType>::Type const i) \
		{ \
			typedef GD::TypeTraits::Underlying<TEnumType>::Type TEnumInteger; \
			return static_cast<TEnumInteger>(e) == i; \
		} \
		GDINL static constexpr bool operator!= (TEnumType const e, typename GD::TypeTraits::Underlying<TEnumType>::Type const i) \
		{ \
			typedef GD::TypeTraits::Underlying<TEnumType>::Type TEnumInteger; \
			return static_cast<TEnumInteger>(e) != i; \
		} \
	} 

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                                 Type traits.                                     ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	/*!
	 * Template type selector. Similar to ternary operator.
	 *
	 * @tparam TCondition Compile-time condition for type selector.
	 * @tparam TTrueType Type that would be selected if condition is true.
	 * @tparam TFalseType Type that would be selected if condition is false.
	 */
	//! @{
	template<bool TCondition, typename TTrueType, typename TFalseType> 
	struct Conditional final : public TNonCreatable
	{ 
		typedef TTrueType Type; 
	};	// struct Conditional 
	template<typename TTrueType, typename TFalseType> 
	struct Conditional<false, TTrueType, TFalseType> final : public TNonCreatable
	{
		typedef TFalseType Type;
	};	// struct Conditional 
	//! @}

	/*!
	 * Typedefs specified type if condition is true.
	 *
	 * @tparam TCondition Compile-time condition for the check.
	 * @tparam TCondType Type that would available if condition is true.
	 */
	//! @{
	template<bool TCondition, typename TCondType = void> 
	struct EnableIf final : public TNonCreatable
	{};
	template<typename TCondType> 
	struct EnableIf<true, TCondType> final : public TNonCreatable
	{
		typedef TCondType Type;
	};	// struct EnableIf
	//! @}

	/*!
	 * Typedefs specified type if condition is false.
	 *
	 * @tparam TCondition Compile-time condition for the check.
	 * @tparam TCondType Type that would available if condition is false.
	 */
	//! @{
	template<bool TCondition, typename TCondType = void> 
	struct DisableIf final : public TNonCreatable
	{};	
	template<typename TCondType> 
	struct DisableIf<false, TCondType> final : public TNonCreatable
	{
		typedef TCondType Type;
	};	// struct DisableIf
	//! @}

	namespace TypeTraits
	{
		/*!
		 * Substitutes the underlying type of the specified.
		 * @tparam TType Type to perform operation on.
		 */
		template<typename TType> 
		struct Underlying final : public TNonCreatable
		{
		public:
			typedef __underlying_type(TType) Type;
		};	// struct Underlying

		// ------------------------------------------------------------------------------------------
		// Remove something.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Removes one level constant modifier.
		 * @tparam TType Type to perform operation on.
		 */
		//! @{
		template<typename TType> 
		struct RemoveConst final : public TNonCreatable
		{
			typedef TType Type;
		};	// struct RemoveConst
		template<typename TType> 
		struct RemoveConst<TType const> final : public TNonCreatable
		{
			typedef TType Type;
		};	// struct RemoveConst<TType const>
		//! @}

		/*!
		 * Removes one level volatile modifier.
		 * @tparam TType Type to perform operation on.
		 */
		//! @{
		template<typename TType> 
		struct RemoveVolatile final : public TNonCreatable
		{
			typedef TType Type;
		};	// struct RemoveVolatile
		template<typename TType> 
		struct RemoveVolatile<TType volatile> final : public TNonCreatable
		{
			typedef TType Type;
		};	// struct RemoveVolatile<TType const>
		//! @}

		/*!
		 * Removes one level reference modifier.
		 * @tparam TType Type to perform operation on.
		 */
		//! @{
		template<typename TType> 
		struct RemoveReference final : public TNonCreatable
		{
			typedef TType Type;
		};	// struct RemoveReference
		template<typename TType> 
		struct RemoveReference<TType&> final : public TNonCreatable
		{
			typedef TType Type;
		};	// struct RemoveReference
		template<typename TType> 
		struct RemoveReference<TType&&> final : public TNonCreatable
		{
			typedef TType Type;
		};	// struct RemoveReference
		//! @}

		/*!
		 * Removes one level pointer modifier.
		 * @tparam TType Type to perform operation on.
		 */
		//! @{
		template<typename TType> 
		struct RemovePointer final : public TNonCreatable
		{
			typedef TType Type;
		};	// struct RemovePointer
		template<typename TType> 
		struct RemovePointer<TType*> final : public TNonCreatable
		{
			typedef TType Type;
		};	// struct RemovePointer
		template<typename TType> 
		struct RemovePointer<TType const*> final : public TNonCreatable
		{
			typedef TType Type;
		};	// struct RemovePointer
		template<typename TType> 
		struct RemovePointer<TType volatile*> final : public TNonCreatable
		{
			typedef TType Type;
		};	// struct RemovePointer
		template<typename TType> 
		struct RemovePointer<TType volatile const*> final : public TNonCreatable
		{
			typedef TType Type;
		};	// struct RemovePointer
		//! @}

		/*!
		 * Removes all one level modifiers.
		 * @tparam TType Type to perform operation on.
		 */
		template<typename TType> 
		struct RemoveCVR final : public TNonCreatable
		{
			typedef typename RemoveReference<
						typename RemoveVolatile<
							typename RemoveConst<TType>::Type
						>::Type
					>::Type Type;
		};	// struct RemoveCVR

		// ------------------------------------------------------------------------------------------
		// Is something.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Removes all one level modifiers.
		 * @tparam TValue Trait check result value.
		 */
		template<bool TValue> 
		struct TypeTraitsBase : public TNonCreatable
		{
			enum { Value = static_cast<int>(TValue) };
		};	// struct TypeTraitsBase

		/*!
		 * Checks if specified type is integral.
		 * @tparam TType Type to perform checks on.
		 */
		//! @{
		template<typename TType> 
		struct IsIntegral final : public TypeTraitsBase<false> 
		{};
		template<> 
		struct IsIntegral<Int8> final : public TypeTraitsBase<true>
		{};
		template<>
		struct IsIntegral<UInt8> final : public TypeTraitsBase<true>
		{};
		template<> 
		struct IsIntegral<Int16> final : public TypeTraitsBase<true> 
		{};
		template<>
		struct IsIntegral<UInt16> final : public TypeTraitsBase<true>
		{};
		template<> 
		struct IsIntegral<Int32> final : public TypeTraitsBase<true> 
		{};
		template<>
		struct IsIntegral<UInt32> final : public TypeTraitsBase<true>
		{};
		template<> 
		struct IsIntegral<Int64> final : public TypeTraitsBase<true> 
		{};
		template<> 
		struct IsIntegral<UInt64> final : public TypeTraitsBase<true> 
		{};
		//! @}

		/*!
		 * Checks if specified type is floating point.
		 * @tparam TType Type to perform checks on.
		 */
		//! @{
		template<typename TType> 
		struct IsFloatingPoint final : public TypeTraitsBase<false> 
		{};
		template<> 
		struct IsFloatingPoint<Float32> final : public TypeTraitsBase<true>
		{};
		template<> 
		struct IsFloatingPoint<Float64> final : public TypeTraitsBase<true> 
		{};
		//! @}

		/*!
		 * Checks if specified type is enum.
		 * @tparam TType Type to perform checks on.
		 */
		template<typename TType>
		struct IsEnum final : public TypeTraitsBase<__is_enum(TType)>
		{};

		/*!
		 * Checks if specified type is enum.
		 * @tparam TType Type to perform checks on.
		 */
		template<typename TType>
		struct IsClass final : public TypeTraitsBase<__is_class(TType)>
		{};

		/*!
		 * Checks if specified type is character type.
		 * @tparam TType Type to perform checks on.
		 */
		//! @{
		template<typename TType>
		struct IsCharacter final : public TypeTraitsBase<false> 
		{};
		template<> 
		struct IsCharacter<Char> final : public TypeTraitsBase<true>
		{};
		template<> 
		struct IsCharacter<WideChar> final : public TypeTraitsBase<true> 
		{};
		//! @}

		/*!
		 * Checks if specified type is reference.
		 * @tparam TType Type to perform checks on.
		 */
		//! @{
		template<typename TType>
		struct IsReference final : public TypeTraitsBase<false>
		{};
		template<typename TType>
		struct IsReference<TType&> final : public TypeTraitsBase<true>
		{};
		template<typename TType>
		struct IsReference<TType&&> final : public TypeTraitsBase<true>
		{};
		//! @}

		/*!
		 * Checks if specified type is L-Value reference.
		 * @tparam TType Type to perform checks on.
		 */
		//! @{
		template<typename TType> 
		struct IsLValueReference final : public TypeTraitsBase<false> 
		{};
		template<typename TType> 
		struct IsLValueReference<TType&> final : public TypeTraitsBase<true>
		{};
		//! @}

		/*!
		 * Checks if specified type is R-Value reference.
		 * @tparam TType Type to perform checks on.
		 */
		//! @{
		template<typename TType> 
		struct IsRValueReference final : public TypeTraitsBase<false>
		{};
		template<typename TType> 
		struct IsRValueReference<TType&&> final : public TypeTraitsBase<true>
		{};
		//! @}

		/*!
		 * Checks if type is pointer.
		 * @tparam TType Type to perform checks on.
		 */
		//! @{
		template<typename TType> 
		struct IsPointer final : public TypeTraitsBase<false>
		{};
		template<typename TType> 
		struct IsPointer<TType*> final : public TypeTraitsBase<true>
		{};
		template<typename TType>
		struct IsPointer<TType const*> final : public TypeTraitsBase<true>
		{};
		template<typename TType> 
		struct IsPointer<TType volatile*> final : public TypeTraitsBase<true> 
		{};
		template<typename TType> 
		struct IsPointer<TType volatile const*> final : public TypeTraitsBase<true> 
		{};
		//! @}

		/*!
		 * Checks if type is const.
		 * @tparam TType Type to perform checks on.
		 */
		//! @{
		template<typename TType>
		struct IsConst final : public TypeTraitsBase<false> 
		{};
		template<typename TType>
		struct IsConst<TType const> final : public TypeTraitsBase<true>
		{};
		//! @}

		/*!
		 * Checks if type is volatile.
		 * @tparam TType Type to perform checks on.
		 */
		//! @{
		template<typename TType>
		struct IsVolatile final : public TypeTraitsBase<false> 
		{};
		template<typename TType>
		struct IsVolatile<TType volatile> final : public TypeTraitsBase<true>
		{};
		//! @}

		template<typename TType>
		struct HasUnsignedValues : public TypeTraitsBase<(static_cast<TType>(-1) > static_cast<TType>(0))>
		{};

		/*!
		 * Checks if type is signed.
		 * @tparam TType Type to perform checks on.
		 */
		template<typename TType>
		struct IsSigned final : public Conditional<IsIntegral<TType>::Value, TypeTraitsBase<!HasUnsignedValues<TType>::Value>, TypeTraitsBase<false>>::Type
		{};

		/*!
		 * Checks if type is unsigned.
		 * @tparam TType Type to perform checks on.
		 */
		template<typename TType>
		struct IsUnsigned final : public Conditional<IsIntegral<TType>::Value, HasUnsignedValues<TType>, TypeTraitsBase<false>>::Type
		{};

		/*!
		 * Checks if type is plain old data.
		 * @tparam TType Type to perform checks on.
		 */
		template<typename TType>
		struct IsPOD final : public TypeTraitsBase<__is_pod(TType)>
		{};

		/*!
		 * Checks if type is contains pure virtual methods.
		 * @tparam TType Type to perform checks on.
		 */
		template<typename TType>
		struct IsAbstract final : public TypeTraitsBase<__is_abstract(TType)>
		{};

		/*!
		 * Checks if two types are same.
		 *
		 * @tparam TLHS First comparand type.
		 * @tparam TRHS Second comparand type.
		 */
		//! @{
		template<typename TLHS, typename TRHS> 
		struct IsSame final : TypeTraitsBase<false> 
		{};
		template<typename TLHS> 
		struct IsSame<TLHS, TLHS> final : TypeTraitsBase<true> 
		{};
		//! @}

		/*!
		 * Checks if second type is derived from first type.
		 *
		 * @tparam TBaseType Type that is supposed to be base.
		 * @tparam TDerivedType Type that is supposed to be derived.
		 */
		template<typename TBaseType, typename TDerivedType>
		struct IsBase final
		{
		private:
			typedef Byte No[1];
			typedef Byte Yes[2];
			typedef typename RemovePointer<TBaseType>::Type BaseTypeUnPtr;
			typedef typename RemovePointer<TDerivedType>::Type DerivedTypeUnPtr;
			GDINT static Yes& Test(BaseTypeUnPtr const* const) = delete;
			GDINT static No	& Test(...) = delete;
			
		public:
			enum { Value = sizeof(Test(static_cast<DerivedTypeUnPtr const*>(nullptr))) == sizeof(Yes) };
		};	// struct IsBase

	}	// namespace TypeTraits

GD_NAMESPACE_END
