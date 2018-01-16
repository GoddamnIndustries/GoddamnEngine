// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
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
			typedef GD::TypeTraits::Underlying<TEnumType> TEnumInteger; \
			return static_cast<TEnumType>(~static_cast<TEnumInteger>(e)); \
		} \
		\
		GDINL static constexpr TEnumType operator| (TEnumType const lhs, TEnumType const rhs) \
		{ \
			typedef GD::TypeTraits::Underlying<TEnumType> TEnumInteger; \
			return static_cast<TEnumType>(static_cast<TEnumInteger>(lhs) | static_cast<TEnumInteger>(rhs)); \
		} \
		GDINL static TEnumType& operator|= (TEnumType& lhs, TEnumType const rhs) \
		{\
			return lhs = (lhs | rhs); \
		} \
		\
		GDINL static constexpr TEnumType operator& (TEnumType const lhs, TEnumType const rhs) \
		{ \
			typedef GD::TypeTraits::Underlying<TEnumType> TEnumInteger; \
			return static_cast<TEnumType>(static_cast<TEnumInteger>(lhs) & static_cast<TEnumInteger>(rhs)); \
		} \
		GDINL static TEnumType& operator&= (TEnumType& lhs, TEnumType const rhs) \
		{ \
			return lhs = (lhs & rhs); \
		} \
		\
		GDINL static constexpr TEnumType operator^ (TEnumType const lhs, TEnumType const rhs) \
		{ \
			typedef GD::TypeTraits::Underlying<TEnumType> TEnumInteger; \
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
		GDINL static constexpr bool operator== (TEnumType const e, GD::TypeTraits::Underlying<TEnumType> const i) \
		{ \
			typedef GD::TypeTraits::Underlying<TEnumType> TEnumInteger; \
			return static_cast<TEnumInteger>(e) == i; \
		} \
		GDINL static constexpr bool operator!= (TEnumType const e, GD::TypeTraits::Underlying<TEnumType> const i) \
		{ \
			typedef GD::TypeTraits::Underlying<TEnumType> TEnumInteger; \
			return static_cast<TEnumInteger>(e) != i; \
		} \
	} 

/*!
 * Defines helper code for determination whether class contains named member function.
 * @param TFunctionName Name of the function.
 */
#define GD_HAS_MEMBER_FUNCTION(TFunctionName) \
	namespace TypeTraits \
	{ \
		template<typename TClass> \
		struct HasMemberFunction_ ## TFunctionName final : public TNonCreatable \
		{ \
		private: \
			template<typename TType, TType TInstance> \
			struct TTest; \
			\
			template<typename TType> \
			GDINT static Internal::TTrue& Test(TTest<decltype(&TType::TFunctionName), &TType::TFunctionName>* const) { GD_NOT_SUPPORTED(); } \
			template<typename TType> \
			GDINT static Internal::TFalse& Test(...) { GD_NOT_SUPPORTED(); } \
			\
		public: \
			enum { Value = sizeof(Test<TClass const>(nullptr)) == sizeof(Internal::TTrue) };\
		};	\
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
	namespace TypeTraits
	{
		namespace Internal
		{
			template<bool TCondition, typename TTrueType, typename TFalseType>
			struct TConditional final : public TNonCreatable
			{
				typedef TTrueType Type;
			};	// struct TConditional 
			template<typename TTrueType, typename TFalseType>
			struct TConditional<false, TTrueType, TFalseType> final : public TNonCreatable
			{
				typedef TFalseType Type;
			};	// struct TConditional 
		}	// namespace Internal
	}	// namespace TypeTraits
	template<bool TCondition, typename TTrueType, typename TFalseType>
	using Conditional = typename TypeTraits::Internal::TConditional<TCondition, TTrueType, TFalseType>::Type;
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
		namespace Internal
		{
			typedef Byte TFalse[1];
			typedef Byte TTrue[2];
		}	// namespace Internal

		/*!
		 * Substitutes the underlying type of the specified.
		 * @tparam TType Type to perform operation on.
		 */
		//! @{
		namespace Internal
		{
			template<typename TType>
			struct TUnderlying final : public TNonCreatable
			{
			public:
				typedef __underlying_type(TType) Type;
			};	// struct TUnderlying
		}	// namespace Internal
		template<typename TType>
		using Underlying = typename Internal::TUnderlying<TType>::Type;
		//! @}

		// ------------------------------------------------------------------------------------------
		// Remove something.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Removes one level constant modifier.
		 * @tparam TType Type to perform operation on.
		 */
		//! @{
		namespace Internal
		{
			template<typename TType> 
			struct TRemoveConst final : public TNonCreatable
			{
				typedef TType Type;
			};	// struct TRemoveConst
			template<typename TType> 
			struct TRemoveConst<TType const> final : public TNonCreatable
			{
				typedef TType Type;
			};	// struct TRemoveConst<TType const>
		}	// namespace Internal
		template<typename TType>
		using RemoveConst = typename Internal::TRemoveConst<TType>::Type;
		//! @}

		/*!
		 * Removes one level volatile modifier.
		 * @tparam TType Type to perform operation on.
		 */
		//! @{
		namespace Internal
		{
			template<typename TType> 
			struct TRemoveVolatile final : public TNonCreatable
			{
				typedef TType Type;
			};	// struct TRemoveVolatile
			template<typename TType> 
			struct TRemoveVolatile<TType volatile> final : public TNonCreatable
			{
				typedef TType Type;
			};	// struct TRemoveVolatile<TType volatile>
		}	// namespace Internal
		template<typename TType>
		using RemoveVolatile = typename Internal::TRemoveVolatile<TType>::Type;
		//! @}

		/*!
		 * Removes one level reference modifier.
		 * @tparam TType Type to perform operation on.
		 */
		//! @{
		namespace Internal
		{
			template<typename TType> 
			struct TRemoveReference final : public TNonCreatable
			{
				typedef TType Type;
			};	// struct RemoveReference
			template<typename TType> 
			struct TRemoveReference<TType&> final : public TNonCreatable
			{
				typedef TType Type;
			};	// struct RemoveReference
			template<typename TType> 
			struct TRemoveReference<TType&&> final : public TNonCreatable
			{
				typedef TType Type;
			};	// struct RemoveReference
		}	// namespace Internal
		template<typename TType>
		using RemoveReference = typename Internal::TRemoveReference<TType>::Type;
		//! @}

		/*!
		 * Removes one level pointer modifier.
		 * @tparam TType Type to perform operation on.
		 */
		//! @{
		namespace Internal
		{
			template<typename TType> 
			struct TRemovePointer final : public TNonCreatable
			{
				typedef TType Type;
			};	// struct TRemovePointer
			template<typename TType> 
			struct TRemovePointer<TType*> final : public TNonCreatable
			{
				typedef TType Type;
			};	// struct TRemovePointer
			template<typename TType> 
			struct TRemovePointer<TType const*> final : public TNonCreatable
			{
				typedef TType Type;
			};	// struct TRemovePointer
			template<typename TType> 
			struct TRemovePointer<TType volatile*> final : public TNonCreatable
			{
				typedef TType Type;
			};	// struct TRemovePointer
			template<typename TType> 
			struct TRemovePointer<TType volatile const*> final : public TNonCreatable
			{
				typedef TType Type;
			};	// struct TRemovePointer
		}	// namespace Internal
		template<typename TType>
		using RemovePointer = typename Internal::TRemovePointer<TType>::Type;
		//! @}

		/*!
		 * Removes all one level modifiers.
		 * @tparam TType Type to perform operation on.
		 */
		template<typename TType>
		using Decay = RemoveReference<
				RemoveVolatile<
					RemoveConst<
						TType
					>
				>
			>;

		// ------------------------------------------------------------------------------------------
		// Is something.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Removes all one level modifiers.
		 * @tparam TValue Trait check result value.
		 */
		namespace Internal
		{
			template<bool TValue> 
			struct TypeTraitsBase : public TNonCreatable
			{
				enum { Value = static_cast<int>(TValue) };
			};	// struct TypeTraitsBase
		}	// namespace Internal

		/*!
		 * Checks if specified type is integral.
		 * @tparam TType Type to perform checks on.
		 */
		//! @{
		template<typename TType> 
		struct IsIntegral final : public Internal::TypeTraitsBase<false>
		{};
		template<> 
		struct IsIntegral<Int8> final : public Internal::TypeTraitsBase<true>
		{};
		template<>
		struct IsIntegral<UInt8> final : public Internal::TypeTraitsBase<true>
		{};
		template<> 
		struct IsIntegral<Int16> final : public Internal::TypeTraitsBase<true> 
		{};
		template<>
		struct IsIntegral<UInt16> final : public Internal::TypeTraitsBase<true>
		{};
		template<> 
		struct IsIntegral<Int32> final : public Internal::TypeTraitsBase<true> 
		{};
		template<>
		struct IsIntegral<UInt32> final : public Internal::TypeTraitsBase<true>
		{};
		template<> 
		struct IsIntegral<Int64> final : public Internal::TypeTraitsBase<true> 
		{};
		template<> 
		struct IsIntegral<UInt64> final : public Internal::TypeTraitsBase<true> 
		{};
		//! @}

		/*!
		 * Checks if specified type is floating point.
		 * @tparam TType Type to perform checks on.
		 */
		//! @{
		template<typename TType> 
		struct IsFloatingPoint final : public Internal::TypeTraitsBase<false>
		{};
		template<> 
		struct IsFloatingPoint<Float32> final : public Internal::TypeTraitsBase<true>
		{};
		template<> 
		struct IsFloatingPoint<Float64> final : public Internal::TypeTraitsBase<true> 
		{};
		//! @}

		/*!
		 * Checks if specified type is enum.
		 * @tparam TType Type to perform checks on.
		 */
		template<typename TType>
		struct IsEnum final : public Internal::TypeTraitsBase<__is_enum(TType)>
		{};

		/*!
		 * Checks if specified type is enum.
		 * @tparam TType Type to perform checks on.
		 */
		template<typename TType>
		struct IsClass final : public Internal::TypeTraitsBase<__is_class(TType)>
		{};

		/*!
		 * Checks if specified type is character type.
		 * @tparam TType Type to perform checks on.
		 */
		//! @{
		template<typename TType>
		struct IsCharacter final : public Internal::TypeTraitsBase<false>
		{};
		template<> 
		struct IsCharacter<Char> final : public Internal::TypeTraitsBase<true>
		{};
		template<> 
		struct IsCharacter<WideChar> final : public Internal::TypeTraitsBase<true>
		{};
		//! @}

		/*!
		 * Checks if specified type is reference.
		 * @tparam TType Type to perform checks on.
		 */
		//! @{
		template<typename TType>
		struct IsReference final : public Internal::TypeTraitsBase<false>
		{};
		template<typename TType>
		struct IsReference<TType&> final : public Internal::TypeTraitsBase<true>
		{};
		template<typename TType>
		struct IsReference<TType&&> final : public Internal::TypeTraitsBase<true>
		{};
		//! @}

		/*!
		 * Checks if specified type is L-Value reference.
		 * @tparam TType Type to perform checks on.
		 */
		//! @{
		template<typename TType> 
		struct IsLValueReference final : public Internal::TypeTraitsBase<false>
		{};
		template<typename TType> 
		struct IsLValueReference<TType&> final : public Internal::TypeTraitsBase<true>
		{};
		//! @}

		/*!
		 * Checks if specified type is R-Value reference.
		 * @tparam TType Type to perform checks on.
		 */
		//! @{
		template<typename TType> 
		struct IsRValueReference final : public Internal::TypeTraitsBase<false>
		{};
		template<typename TType> 
		struct IsRValueReference<TType&&> final : public Internal::TypeTraitsBase<true>
		{};
		//! @}

		/*!
		 * Checks if type is pointer.
		 * @tparam TType Type to perform checks on.
		 */
		//! @{
		template<typename TType> 
		struct IsPointer final : public Internal::TypeTraitsBase<false>
		{};
		template<typename TType> 
		struct IsPointer<TType*> final : public Internal::TypeTraitsBase<true>
		{};
		template<typename TType>
		struct IsPointer<TType const*> final : public Internal::TypeTraitsBase<true>
		{};
		template<typename TType> 
		struct IsPointer<TType volatile*> final : public Internal::TypeTraitsBase<true>
		{};
		template<typename TType> 
		struct IsPointer<TType volatile const*> final : public Internal::TypeTraitsBase<true>
		{};
		//! @}

		/*!
		 * Checks if type is const.
		 * @tparam TType Type to perform checks on.
		 */
		//! @{
		template<typename TType>
		struct IsConst final : public Internal::TypeTraitsBase<false>
		{};
		template<typename TType>
		struct IsConst<TType const> final : public Internal::TypeTraitsBase<true>
		{};
		//! @}

		/*!
		 * Checks if type is volatile.
		 * @tparam TType Type to perform checks on.
		 */
		//! @{
		template<typename TType>
		struct IsVolatile final : public Internal::TypeTraitsBase<false>
		{};
		template<typename TType>
		struct IsVolatile<TType volatile> final : public Internal::TypeTraitsBase<true>
		{};
		//! @}

		template<typename TType>
		struct HasUnsignedValues : public Internal::TypeTraitsBase<(static_cast<TType>(-1) > static_cast<TType>(0))>
		{};

		/*!
		 * Checks if type is signed.
		 * @tparam TType Type to perform checks on.
		 */
		template<typename TType>
		struct IsSigned final : public Conditional<IsIntegral<TType>::Value, Internal::TypeTraitsBase<!HasUnsignedValues<TType>::Value>, Internal::TypeTraitsBase<false>>
		{};

		/*!
		 * Checks if type is unsigned.
		 * @tparam TType Type to perform checks on.
		 */
		template<typename TType>
		struct IsUnsigned final : public Conditional<IsIntegral<TType>::Value, HasUnsignedValues<TType>, Internal::TypeTraitsBase<false>>
		{};

		/*!
		 * Checks if type is plain old data.
		 * @tparam TType Type to perform checks on.
		 */
		template<typename TType>
		struct IsPOD final : public Internal::TypeTraitsBase<__is_pod(TType)>
		{};

		/*!
		 * Checks if type is contains pure virtual methods.
		 * @tparam TType Type to perform checks on.
		 */
		template<typename TType>
		struct IsAbstract final : public Internal::TypeTraitsBase<__is_abstract(TType)>
		{};

		/*!
		 * Checks if two types are same.
		 *
		 * @tparam TLHS First comparand type.
		 * @tparam TRHS Second comparand type.
		 */
		//! @{
		template<typename TLHS, typename TRHS> 
		struct IsSame final : Internal::TypeTraitsBase<false>
		{};
		template<typename TLHS> 
		struct IsSame<TLHS, TLHS> final : Internal::TypeTraitsBase<true>
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
			using TBaseTypeUnPtr = Decay<TBaseType>;
			using TDerivedTypeUnPtr = Decay<TDerivedType>;

			GDINT static Internal::TTrue& Test(TBaseTypeUnPtr const* const) { abort(); }
			GDINT static Internal::TFalse& Test(...) { abort(); }

		public:
			enum { Value = sizeof(Test(static_cast<TDerivedTypeUnPtr const*>(nullptr))) == sizeof(Internal::TTrue) };
		};	// struct IsBase
		 
		/*!
		 * Checks is two types are related.
		 *
		 * @tparam TLHS First comparand type.
		 * @tparam TRHS Second comparand type.
		 */
		template<typename TLHS, typename TRHS>
		struct IsRelated final : Internal::TypeTraitsBase<IsBase<TLHS, TRHS>::Value || IsBase<TRHS, TLHS>::Value> {};

	}	// namespace TypeTraits

GD_NAMESPACE_END
