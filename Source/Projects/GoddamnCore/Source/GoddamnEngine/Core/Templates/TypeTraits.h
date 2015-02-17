// ==========================================================================================
// TypeTraits.h: Compile-time types information and modifiers management.
// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
// ==========================================================================================

#pragma once
#ifndef GD_CORE_TYPE_TRAITS
#define GD_CORE_TYPE_TRAITS

#include <GoddamnEngine/Include.h>

/// Compiler intrinsic for traits. Hope are 'cross-platform'.
/// @param	Type The type to be checked.
/// @{
#define GD_TYPE_TRAITS_IS_ENUM(Type) __is_enum(Type)
#define GD_TYPE_TRAITS_IS_POD(Type) __is_pod(Type)
#define GD_TYPE_TRAITS_IS_ABSTARCT(Type) __is_abstract(Type)
/// @}

GD_NAMESPACE_BEGIN

	/// @brief Template type selector. Similar to trinary operator.
	/// @tparam Condition Compile-time condition for type selector.
	/// @{
	template<  bool Condition,   typename TrueType, typename FalseType> struct Conditional { typedef TrueType Type; };
	template</*bool Condition,*/ typename TrueType, typename FalseType> struct Conditional<false, TrueType, FalseType> { typedef FalseType Type; };
	/// @}

	/// @brief Typedefs specified type if condition is true.
	/// @tparam Condition Compile-time condition for the check.
	/// @tparam CondType Type that would available if condition is true.
	/// @{
	template<  bool Condition,   typename CondType   = void  > struct EnableIf { };
	template</*bool Condition,*/ typename CondType /*= void*/> struct EnableIf <true, CondType> { typedef CondType Type; };
	/// @}

	/// @brief Typedefs specified type if condition is false.
	/// @tparam Condition Compile-time condition for the check.
	/// @tparam CondType Type that would available if condition is false.
	/// @{
	template<  bool Condition,   typename CondType   = void  > struct DisableIf { };
	template</*bool Condition,*/ typename CondType /*= void*/> struct DisableIf<false, CondType> { typedef CondType Type; };
	/// @}

	/// @brief Does some actions on types.
#if GD_COMPILER_MSVC_COMPATIBLE || defined(GD_DOCUMENTATION)
	class TypeTraits final
	{
	public:
#else	// if GD_COMPILER_MSVC_COMPATIBLE || defined(GD_DOCUMENTATION)
	// Avoiding 'explicit specialization in class scope' errors.
	namespace TypeTraits
	{
#endif	// if GD_COMPILER_MSVC_COMPATIBLE || defined(GD_DOCUMENTATION)

		/// @brief Removes one level constant modifier.
		/// @tparam InType Type to perform operation on.
		/// @{
		template<typename InType> struct RemoveConst               final { typedef InType Type; };
		template<typename InType> struct RemoveConst<InType const> final { typedef InType Type; };
		/// @}

		/// @brief Removes one level reference ('&') modifier.
		/// @tparam InType Type to perform operation on.
		/// @{
		template<typename InType> struct RemoveReference           final { typedef InType Type; };
		template<typename InType> struct RemoveReference<InType &> final { typedef InType Type; };
		template<typename InType> struct RemoveReference<InType&&> final { typedef InType Type; };
		/// @}

		/// @brief Removes one level pointer ('*') modifier.
		/// @tparam InType Type to perform operation on.
		/// @{
		template<typename InType> struct RemovePointer                         final { typedef InType Type; };
		template<typename InType> struct RemovePointer<InType*>                final { typedef InType Type; };
		template<typename InType> struct RemovePointer<InType volatile*>       final { typedef InType Type; };
		template<typename InType> struct RemovePointer<InType volatile const*> final { typedef InType Type; };
		template<typename InType> struct RemovePointer<InType const*>          final { typedef InType Type; };
		/// @}

		/// @brief Removes all one level modifiers.
		/// @tparam InType Type to perform operation on.
		template<typename InType> struct RemoveAllModifiers final { typedef typename RemoveConst<typename RemovePointer<typename RemoveReference<InType>::Type>::Type>::Type Type; };

		/// @brief Base struct for all traits.
		/// @tparam	TraitsValue	Trait check result value.
		template<bool const TraitsValue> 
		struct TypeTraitsBase 
		{ 
			enum ValueType { Value = static_cast<int>(TraitsValue) }; 
		};

		/// @brief Checks if specified type is integral.
		/// @tparam Type Type to perform checks on.
		/// @{
		template<typename Type> struct IsIntegral final : public TypeTraitsBase<false> { };
		template<> struct IsIntegral<Int8>   final : public TypeTraitsBase<true> { };
		template<> struct IsIntegral<Int16>  final : public TypeTraitsBase<true> { };
		template<> struct IsIntegral<Int32>  final : public TypeTraitsBase<true> { };
		template<> struct IsIntegral<Int64>  final : public TypeTraitsBase<true> { };
		template<> struct IsIntegral<UInt8>  final : public TypeTraitsBase<true> { };
		template<> struct IsIntegral<UInt16> final : public TypeTraitsBase<true> { };
		template<> struct IsIntegral<UInt32> final : public TypeTraitsBase<true> { };
		template<> struct IsIntegral<UInt64> final : public TypeTraitsBase<true> { };
		/// @}

		/// @brief Checks if specified type is floating point.
		/// @tparam Type Type to perform checks on.
		/// @{
		template<typename Type> struct IsFloatingPoint final : public TypeTraitsBase<false> { };
		template<> struct IsFloatingPoint<Float32> final : public TypeTraitsBase<true> { };
		template<> struct IsFloatingPoint<Float64> final : public TypeTraitsBase<true> { };
		/// @}

		/// @brief Checks if is enumeration.
		/// @tparam Type Type to perform checks on.
		template<typename Type> struct IsEnum final : public TypeTraitsBase<GD_TYPE_TRAITS_IS_ENUM(Type)> { };

		/// @brief Checks if specified type is character type.
		/// @tparam Type Type to perform checks on.
		/// @{
		template<typename Type> struct IsCharacter final : public TypeTraitsBase<false> { };
		template<> struct IsCharacter<char   > final : public TypeTraitsBase<true> { };
		template<> struct IsCharacter<wchar_t> final : public TypeTraitsBase<true> { };
		/// @}

		/// @brief Checks if type is reference.
		/// @tparam Type Type to perform checks on.
		/// @{
		template<typename Type> struct IsReference	       final : public TypeTraitsBase<false> { };
		template<typename Type> struct IsReference<Type& > final : public TypeTraitsBase<true> { };
		template<typename Type> struct IsReference<Type&&> final : public TypeTraitsBase<true> { };
		/// @}

		/// @brief Checks if type is L-Value reference.
		/// @tparam Type Type to perform checks on.
		/// @{
		template<typename Type> struct IsLValueReference        final : public TypeTraitsBase<false> { };
		template<typename Type> struct IsLValueReference<Type&> final : public TypeTraitsBase<true> { };
		/// @}

		/// @brief Checks if type is R-Value reference.
		/// @tparam Type Type to perform checks on.
		/// @{
		template<typename Type> struct IsRValueReference         final : public TypeTraitsBase<false> { };
		template<typename Type> struct IsRValueReference<Type&&> final : public TypeTraitsBase<true> { };
		/// @}

		/// @brief Checks if type is pointer.
		/// @tparam Type Type to perform checks on.
		/// @{
		template<typename Type> struct IsPointer                       final : public TypeTraitsBase<false> { };
		template<typename Type> struct IsPointer<Type			    *> final : public TypeTraitsBase<true> { };
		template<typename Type> struct IsPointer<Type volatile      *> final : public TypeTraitsBase<true> { };
		template<typename Type> struct IsPointer<Type volatile const*> final : public TypeTraitsBase<true> { };
		template<typename Type> struct IsPointer<Type          const*> final : public TypeTraitsBase<true> { };
		/// @}

		/// @brief Checking if type contains 'const' modifier.
		/// @tparam Type Type to perform checks on.
		/// @{
		template<typename Type> struct IsConstType		       final : public TypeTraitsBase<false> { };
		template<typename Type> struct IsConstType<Type const> final : public TypeTraitsBase<true> { };
		/// @}

		/// @brief Checks if 'First' type is same to 'Second'.
		/// @tparam First First comparand type.
		/// @tparam Second Second comparand type.
		/// @{
		template<typename First, typename Second> struct IsSame               final : TypeTraitsBase<false> { };
		template<typename First		            > struct IsSame<First, First> final : TypeTraitsBase<true> { };
		/// @}
		
		/// @brief Checks if type is POD (Plain Old Data).
		/// @tparam Type Type to perform checks on.
		template<typename Type> struct IsPOD final : public TypeTraitsBase<GD_TYPE_TRAITS_IS_POD(Type)> { };

		/// @brief Checks if type contains pure virtual methods.
		/// @tparam Type Type to perform checks on.
		template<typename Type> struct IsAbstract final : public TypeTraitsBase<GD_TYPE_TRAITS_IS_ABSTARCT(Type)> { };

		/// @brief Checks if 'DerivedType' is derived from 'BaseType'.
		/// @tparam BaseType Type that is supposed to be base.
		/// @tparam DerivedType Type that is supposed to be derived.
		template<typename BaseType, typename DerivedType>
		struct IsBase final
		{
		private:
			typedef typename TypeTraits::RemovePointer<BaseType   >::Type BaseTypeUnPtr;
			typedef typename TypeTraits::RemovePointer<DerivedType>::Type DerivedTypeUnPtr;

			typedef char No[1];
			typedef char Yes[2];

			GDINT static Yes& Test(BaseTypeUnPtr const* const);
			GDINT static No	& Test(...);

		public:
			enum ValueType { Value = (sizeof(Test(static_cast<DerivedTypeUnPtr const*>(nullptr))) == sizeof(Yes)) };
		};	// struct IsBase

	}; // class TypeTraits

GD_NAMESPACE_END

#endif
