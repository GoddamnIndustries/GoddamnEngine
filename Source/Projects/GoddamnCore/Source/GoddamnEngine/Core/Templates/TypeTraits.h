// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Templates/TypeTraits.h
//! Compile-time types information and modifiers management.
#pragma once

#include <GoddamnEngine/Include.h>
#include <type_traits>

#if GD_DOCUMENTATION || GD_COMPILER_MSVC

// ------------------------------------------------------------------------------------------
//! Compiler intrinsic for traits. Hope are 'cross-platform'.
//! @param Type The type to be checked.
//! @{
#define GD_TYPE_TRAITS_IS_ENUM(Type)			__is_enum(Type)
#define GD_TYPE_TRAITS_IS_CLASS(Type)			__is_class(Type)
#define GD_TYPE_TRAITS_IS_POD(Type)				__is_pod(Type)
#define GD_TYPE_TRAITS_IS_ABSTARCT(Type)		__is_abstract(Type)
#define GD_TYPE_TRAITS_UNDERLYING_TYPE(Type)	__underlying_type(Type)
//! @}

#else	// if GD_DOCUMENTATION || GD_COMPILER_MSVC
#	if GD_COMPILER_CLANG
#		if __has_feature(__is_enum) && __has_feature(__is_pod) && __has_feature(__is_abstract)
#			define GD_TYPE_TRAITS_IS_ENUM(Type)		__is_enum(Type)
#			define GD_TYPE_TRAITS_IS_POD(Type)		__is_pod(Type)
#			define GD_TYPE_TRAITS_IS_ABSTARCT(Type) __is_abstract(Type)
#		else	// if __has_feature(__is_enum) && __has_feature(__is_pod) && __has_feature(__is_abstract)
#			error Type traits features are not supported on compiler level (Clang).
#		endif	// if __has_feature(__is_enum) && __has_feature(__is_pod) && __has_feature(__is_abstract)
#	else	//if GD_COMPILER_CLANG
#		error Unimplemented compiler-level type traits.
#	endif	//if GD_COMPILER_CLANG
#endif	// if GD_DOCUMENTATION || GD_COMPILER_MSVC

// ------------------------------------------------------------------------------------------
//! Defines helper code for determination whether class contains named member function.
//! @param FunctionName Name of the function.
#define GD_HAS_MEMBER_FUNCTION(FunctionName) \
	template<typename ClassTp> struct HasMemberFunction ## FunctionName { \
	private: \
		typedef char NoTp [1]; \
		typedef char YesTp[2]; \
		template<typename Tp, Tp Instance> struct TestTp; \
		template<typename Tp> GDINT static YesTp& Test(TestTp<decltype(&Tp::FunctionName), &Tp::FunctionName>* const Unused); \
		template<           > GDINT static NoTp&  Test(...); \
		\
	public: \
		/** True if function with specified signature and name exist. */ \
		enum ValueTp { Value = (sizeof(Test<ClassTp>(nullptr)) == sizeof(YesTp)) };\
	};	// struct HasMemberFunction ## FunctionName

GD_NAMESPACE_BEGIN

	// ------------------------------------------------------------------------------------------
	//! Template type selector. Similar to trinary operator.
	//! @tparam Condition Compile-time condition for type selector.
	//! @{
	template<  bool Condition,   typename TrueType, typename FalseType> struct Conditional { typedef TrueType Type; };
	template</*bool Condition,*/ typename TrueType, typename FalseType> struct Conditional<false, TrueType, FalseType> { typedef FalseType Type; };
	//! @}

	// ------------------------------------------------------------------------------------------
	//! Typedefs specified type if condition is true.
	//! @tparam Condition Compile-time condition for the check.
	//! @tparam CondType Type that would available if condition is true.
	//! @{
	template<  bool Condition,   typename CondType   = void  > struct EnableIf { };
	template</*bool Condition,*/ typename CondType /*= void*/> struct EnableIf <true, CondType> { typedef CondType Type; };
	//! @}

	// ------------------------------------------------------------------------------------------
	//! Typedefs specified type if condition is false.
	//! @tparam Condition Compile-time condition for the check.
	//! @tparam CondType Type that would available if condition is false.
	//! @{
	template<  bool Condition,   typename CondType   = void  > struct DisableIf { };
	template</*bool Condition,*/ typename CondType /*= void*/> struct DisableIf<false, CondType> { typedef CondType Type; };
	//! @}

	// ------------------------------------------------------------------------------------------
	//! Does some actions on types.
	namespace TypeTraits
	{
		// ------------------------------------------------------------------------------------------
		//! Substitutes the underlying type of the specified.
		//! @tparam InType Type to perform operation on.
		template<typename InType> struct Underlying final
		{
		private:
			enum SomeEnum {};
			typedef typename Conditional<GD_TYPE_TRAITS_IS_ENUM(InType), InType, SomeEnum>::Type SafeType;
		public:
			typedef GD_TYPE_TRAITS_UNDERLYING_TYPE(SafeType) Type;
		};	// struct Underlying

		// ------------------------------------------------------------------------------------------
		//! Removes one level constant modifier.
		//! @tparam InType Type to perform operation on.
		//! @{
		template<typename InType> struct RemoveConst               final { typedef InType Type; };
#if !GD_DOCUMENTATION
		template<typename InType> struct RemoveConst<InType const> final { typedef InType Type; };
#endif	// if !GD_DOCUMENTATION
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Removes one level reference ('&') modifier.
		//! @tparam InType Type to perform operation on.
		//! @{
		template<typename InType> struct RemoveReference           final { typedef InType Type; };
#if !GD_DOCUMENTATION
		template<typename InType> struct RemoveReference<InType &> final { typedef InType Type; };
		template<typename InType> struct RemoveReference<InType&&> final { typedef InType Type; };
#endif	// if !GD_DOCUMENTATION
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Removes one level pointer ('*') modifier.
		//! @tparam InType Type to perform operation on.
		//! @{
		template<typename InType> struct RemovePointer                         final { typedef InType Type; };
#if !GD_DOCUMENTATION
		template<typename InType> struct RemovePointer<InType*>                final { typedef InType Type; };
		template<typename InType> struct RemovePointer<InType const*>          final { typedef InType Type; };
		template<typename InType> struct RemovePointer<InType volatile*>       final { typedef InType Type; };
		template<typename InType> struct RemovePointer<InType volatile const*> final { typedef InType Type; };
#endif	// if !GD_DOCUMENTATION
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Removes all one level modifiers.
		//! @tparam InType Type to perform operation on.
		template<typename InType> struct RemoveCV final { typedef typename RemoveConst<typename RemoveReference<InType>::Type>::Type Type; };

		// ------------------------------------------------------------------------------------------
		//! Base struct for all traits.
		//! @tparam	TraitsValue	Trait check result value.
		template<bool const TraitsValue> 
		struct TypeTraitsBase {	enum ValueType { Value = static_cast<int>(TraitsValue) }; };

		// ------------------------------------------------------------------------------------------
		//! Checks if specified type is integral.
		//! @tparam Type Type to perform checks on.
		//! @{
		template<typename Type> struct IsIntegral final : public TypeTraitsBase<false> { };
#if !GD_DOCUMENTATION
		template<> struct IsIntegral<Int8>   final : public TypeTraitsBase<true>{};
		template<> struct IsIntegral<Int16>  final : public TypeTraitsBase<true> { };
		template<> struct IsIntegral<Int32>  final : public TypeTraitsBase<true> { };
		template<> struct IsIntegral<Int64>  final : public TypeTraitsBase<true> { };
		template<> struct IsIntegral<UInt8>  final : public TypeTraitsBase<true> { };
		template<> struct IsIntegral<UInt16> final : public TypeTraitsBase<true> { };
		template<> struct IsIntegral<UInt32> final : public TypeTraitsBase<true> { };
		template<> struct IsIntegral<UInt64> final : public TypeTraitsBase<true> { };
#endif	// if !GD_DOCUMENTATION
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Checks if specified type is floating point.
		//! @tparam Type Type to perform checks on.
		//! @{
		template<typename Type> struct IsFloatingPoint final : public TypeTraitsBase<false> { };
#if !GD_DOCUMENTATION
		template<> struct IsFloatingPoint<Float32> final : public TypeTraitsBase<true>{};
		template<> struct IsFloatingPoint<Float64> final : public TypeTraitsBase<true> { };
#endif	// if !GD_DOCUMENTATION
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Checks if specified type is enumeration.
		//! @tparam Type Type to perform checks on.
		template<typename Type> struct IsEnum final : public TypeTraitsBase<GD_TYPE_TRAITS_IS_ENUM(Type)> { };

		// ------------------------------------------------------------------------------------------
		//! Checks if specified type is class.
		//! @tparam Type Type to perform checks on.
		template<typename Type> struct IsClass final : public TypeTraitsBase<GD_TYPE_TRAITS_IS_CLASS(Type)> { };

		// ------------------------------------------------------------------------------------------
		//! Checks if specified type is character type.
		//! @tparam Type Type to perform checks on.
		//! @{
		template<typename Type> struct IsCharacter final : public TypeTraitsBase<false> { };
#if !GD_DOCUMENTATION
		template<> struct IsCharacter<char   > final : public TypeTraitsBase<true>{};
		template<> struct IsCharacter<wchar_t> final : public TypeTraitsBase<true> { };
#endif	// if !GD_DOCUMENTATION
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Checks if type is reference.
		//! @tparam Type Type to perform checks on.
		//! @{
		template<typename Type> struct IsReference	       final : public TypeTraitsBase<false> { };
#if !GD_DOCUMENTATION
		template<typename Type> struct IsReference<Type& > final : public TypeTraitsBase<true>{};
		template<typename Type> struct IsReference<Type&&> final : public TypeTraitsBase<true> { };
#endif	// if !GD_DOCUMENTATION
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Checks if type is L-Value reference.
		//! @tparam Type Type to perform checks on.
		//! @{
		template<typename Type> struct IsLValueReference        final : public TypeTraitsBase<false> { };
#if !GD_DOCUMENTATION
		template<typename Type> struct IsLValueReference<Type&> final : public TypeTraitsBase<true>{};
#endif	// if !GD_DOCUMENTATION
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Checks if type is R-Value reference.
		//! @tparam Type Type to perform checks on.
		//! @{
		template<typename Type> struct IsRValueReference         final : public TypeTraitsBase<false> { };
#if !GD_DOCUMENTATION
		template<typename Type> struct IsRValueReference<Type&&> final : public TypeTraitsBase<true>{};
#endif	// if !GD_DOCUMENTATION
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Checks if type is pointer.
		//! @tparam Type Type to perform checks on.
		//! @{
		template<typename Type> struct IsPointer                       final : public TypeTraitsBase<false> { };
#if !GD_DOCUMENTATION
		template<typename Type> struct IsPointer<Type			    *> final : public TypeTraitsBase<true>{};
		template<typename Type> struct IsPointer<Type volatile      *> final : public TypeTraitsBase<true> { };
		template<typename Type> struct IsPointer<Type volatile const*> final : public TypeTraitsBase<true> { };
		template<typename Type> struct IsPointer<Type          const*> final : public TypeTraitsBase<true> { };
#endif	// if !GD_DOCUMENTATION
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Checking if type contains 'const' modifier.
		//! @tparam Type Type to perform checks on.
		//! @{
		template<typename Type> struct IsConst		       final : public TypeTraitsBase<false> { };
#if !GD_DOCUMENTATION
		template<typename Type> struct IsConst<Type const> final : public TypeTraitsBase<true>{};
#endif	// if !GD_DOCUMENTATION
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Checking if type contains 'volatile' modifier.
		//! @tparam Type Type to perform checks on.
		//! @{
		template<typename Type> struct IsVolatile			     final : public TypeTraitsBase<false> {};
#if !GD_DOCUMENTATION
		template<typename Type> struct IsVolatile<Type volatile> final : public TypeTraitsBase<true> {};
#endif	// if !GD_DOCUMENTATION
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Checking if type contains 'unsigned' modifier.
		//! @tparam Type Type to perform checks on.
		//! @{
#if !GD_DOCUMENTATION
		template<typename Type> struct HasUnsignedValues : public TypeTraitsBase<(Type(-1) > Type(0))> {};
#endif	// if !GD_DOCUMENTATION
		template<typename Type> struct IsUnsigned final
			: public Conditional<TypeTraits::IsIntegral<Type>::Value, HasUnsignedValues<Type>, TypeTraitsBase<false>>::Type {};
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Checks if 'First' type is same to 'Second'.
		//! @tparam First First comparand type.
		//! @tparam Second Second comparand type.
		//! @{
		template<typename First, typename Second> struct IsSame               final : TypeTraitsBase<false> {};
#if !GD_DOCUMENTATION
		template<typename First		            > struct IsSame<First, First> final : TypeTraitsBase<true> {};
#endif	// if !GD_DOCUMENTATION
		//! @}
		
		// ------------------------------------------------------------------------------------------
		//! Checks if type is POD (Plain Old Data).
		//! @tparam Type Type to perform checks on.
		template<typename Type> struct IsPOD final : public TypeTraitsBase<GD_TYPE_TRAITS_IS_POD(Type)> { };

		// ------------------------------------------------------------------------------------------
		//! Checks if type contains pure virtual methods.
		//! @tparam Type Type to perform checks on.
		template<typename Type> struct IsAbstract final : public TypeTraitsBase<GD_TYPE_TRAITS_IS_ABSTARCT(Type)> { };

		// ------------------------------------------------------------------------------------------
		//! Checks if 'DerivedType' is derived from 'BaseType'.
		//! @tparam BaseType Type that is supposed to be base.
		//! @tparam DerivedType Type that is supposed to be derived.
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
	}; // namespace TypeTraits

GD_NAMESPACE_END
