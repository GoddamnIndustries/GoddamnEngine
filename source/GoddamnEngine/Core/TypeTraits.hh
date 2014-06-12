//////////////////////////////////////////////////////////////////////////
/// TypeTraits.h: Compile-time types information and modifiers management.
/// Copyright (C) $(GD_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.--.2013 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_CORE_TYPE_TRAITS
#define GD_CORE_TYPE_TRAITS

#include <GoddamnEngine/Include.hh>
#include <type_traits>

GD_NAMESPACE_BEGIN

	/// Does some actions on types.
	namespace TypeTraits
	{
#if (!defined(GD_DOCUMENTATION))
		template<typename Type> struct RemoveConst             final { typedef Type Type; };
		template<typename Type> struct RemoveConst<Type const> final { typedef Type Type; };
#else	// if (!defined(GD_DOCUMENTATION))
		/// Removes one level constant modifier.
		template<typename Type> struct RemoveConst final { };
#endif	// if (!defined(GD_DOCUMENTATION))

#if (!defined(GD_DOCUMENTATION)) 
		template<typename Type> struct RemoveReference         final { typedef Type Type; };
		template<typename Type> struct RemoveReference<Type &> final { typedef Type Type; };
		template<typename Type> struct RemoveReference<Type&&> final { typedef Type Type; };
#else	// if (!defined(GD_DOCUMENTATION))
		/// Removes one level reference ('&') modifier.
		template<typename Type> struct RemoveReference final { };
#endif	// if (!defined(GD_DOCUMENTATION))

#if (!defined(GD_DOCUMENTATION)) 
		template<typename Type> struct RemovePointer                       final { typedef Type Type; };
		template<typename Type> struct RemovePointer<Type               *> final { typedef Type Type; };
		template<typename Type> struct RemovePointer<Type volatile      *> final { typedef Type Type; };
		template<typename Type> struct RemovePointer<Type volatile const*> final { typedef Type Type; };
		template<typename Type> struct RemovePointer<Type          const*> final { typedef Type Type; };
#else	// if (!defined(GD_DOCUMENTATION))
		/// Removes one level pointer ('*') modifier.
		template<typename Type> struct RemovePointer final { };
#endif	// if (!defined(GD_DOCUMENTATION))

#if (!defined(GD_DOCUMENTATION)) 
		template<typename Type> struct RemoveAllModifiers final { typedef typename RemoveConst<typename RemovePointer<typename RemoveReference<Type>::Type>::Type>::Type Type; };
#else	// if (!defined(GD_DOCUMENTATION))
		/// Removes all one level modifiers.
		template<typename Type> struct RemoveAllModifiers final { };
#endif	// if (!defined(GD_DOCUMENTATION))

#if (!defined(GD_DOCUMENTATION))
		template<bool const TraitsValue> struct TypeTraitsBase { enum : bool { Value = static_cast<bool>(TraitsValue) }; };
#endif	// if (!defined(GD_DOCUMENTATION))

#if (!defined(GD_DOCUMENTATION))
		template<typename Type> struct IsPodType final : public TypeTraitsBase<std::is_pod<Type>::value> { };
#else	// if (!defined(GD_DOCUMENTATION))
		/// Checks if type is POD (Plain Old Data).
		template<typename Type> struct IsPodType final { };
#endif	// if (!defined(GD_DOCUMENTATION))

#if (!defined(GD_DOCUMENTATION))
		template<typename Type> struct IsAbstractType final : public TypeTraitsBase<std::is_abstract<Type>::value> { };
#else	// if (!defined(GD_DOCUMENTATION))
		/// Checks if type is abstract.
		template<typename Type> struct IsAbstractType final { };
#endif	// if (!defined(GD_DOCUMENTATION))

#if (!defined(GD_DOCUMENTATION))
		template<typename Type> struct IsArithmeticType final : public TypeTraitsBase<std::is_arithmetic<Type>::value || std::is_enum<Type>::value> { };
#else	// if (!defined(GD_DOCUMENTATION))
		/// Checks if type is arithmetic.
		///	'enum' and 'enum class' are also arithmetic.
		template<typename Type> struct IsArithmeticType final { };
#endif	// if (!defined(GD_DOCUMENTATION))

#if (!defined(GD_DOCUMENTATION))
		template<typename Type> struct IsIntegral final : public TypeTraitsBase<std::is_integral<Type>::value> { };
#else	// if (!defined(GD_DOCUMENTATION))
		/// Checks if specified type is integral.
		template<typename Type> struct IsIntegral final { };
#endif	// if (!defined(GD_DOCUMENTATION))

#if (!defined(GD_DOCUMENTATION))
		template<typename Type> struct IsFloatingPoint final : public TypeTraitsBase<std::is_floating_point<Type>::value> { };
#else	// if (!defined(GD_DOCUMENTATION))
		/// Checks if specified type is floating point.
		template<typename Type> struct IsFloatingPoint final { };
#endif	// if (!defined(GD_DOCUMENTATION))

#if (!defined(GD_DOCUMENTATION))
		template<typename Type> struct IsReference	       final : public TypeTraitsBase<false> { };
		template<typename Type> struct IsReference<Type &> final : public TypeTraitsBase<true > { };
		template<typename Type> struct IsReference<Type&&> final : public TypeTraitsBase<true > { };
#else	// if (!defined(GD_DOCUMENTATION))
		/// Checks if type is reference.
		template<typename Type> struct IsReference final { };
#endif	// if (!defined(GD_DOCUMENTATION))

#if (!defined(GD_DOCUMENTATION))
		template<typename Type> struct IsLValueReference        final : public TypeTraitsBase<false> { };
		template<typename Type> struct IsLValueReference<Type&> final : public TypeTraitsBase<true > { };
#else	// if (!defined(GD_DOCUMENTATION))
		/// Checks if type is L-Value reference.
		template<typename Type> struct IsLValueReference final { }; 
#endif	// if (!defined(GD_DOCUMENTATION))

#if (!defined(GD_DOCUMENTATION))
		template<typename Type> struct IsRValueReference         final : public TypeTraitsBase<false> { };
		template<typename Type> struct IsRValueReference<Type&&> final : public TypeTraitsBase<true > { };
#else	// if (!defined(GD_DOCUMENTATION))
		/// Checks if type is R-Value reference.
		template<typename Type> struct IsRValueReference final { };
#endif	// if (!defined(GD_DOCUMENTATION))

#if (!defined(GD_DOCUMENTATION))
		template<typename Type> struct IsPointer				       final : public TypeTraitsBase<false> { };
		template<typename Type> struct IsPointer<Type			    *> final : public TypeTraitsBase<true > { };
		template<typename Type> struct IsPointer<Type volatile      *> final : public TypeTraitsBase<true > { };
		template<typename Type> struct IsPointer<Type volatile const*> final : public TypeTraitsBase<true > { };
		template<typename Type> struct IsPointer<Type          const*> final : public TypeTraitsBase<true > { };
#else	// if (!defined(GD_DOCUMENTATION))
		/// Checks if type is pointer.
		template<typename Type>  struct IsPointer { };
#endif	// if (!defined(GD_DOCUMENTATION))

#if (!defined(GD_DOCUMENTATION))
		template<typename Type> struct IsConstType		       final : public TypeTraitsBase<false> { };
		template<typename Type> struct IsConstType<Type const> final : public TypeTraitsBase<true>{};
#else	// if (!defined(GD_DOCUMENTATION))
		/// Checking if type contains 'const' modifier.
		template<typename Type> struct IsConstType final { };
#endif	// if (!defined(GD_DOCUMENTATION))

#if (!defined(GD_DOCUMENTATION))
		template<typename First, typename Second> struct IsSameType               final : TypeTraitsBase<false> { };
		template<typename First		            > struct IsSameType<First, First> final : TypeTraitsBase<true > { };
#else	// if (!defined(GD_DOCUMENTATION))
		/// Checks if 'First' is same to 'Second'.
		template<typename First, typename Second> struct IsSameType { };
#endif	// if (!defined(GD_DOCUMENTATION))

		/// Checks if 'DerivedType' is derived from 'BaseType'.
		template<typename BaseType, typename DerivedType>
		struct IsBaseType final
		{ 
#if (!defined(GD_DOCUMENTATION))
		private:
			typedef typename TypeTraits::RemovePointer<BaseType   >::Type BaseTypePtr;
			typedef typename TypeTraits::RemovePointer<DerivedType>::Type DerivedTypePtr;

			typedef char Yes;
			typedef int	 No;
			static_assert(sizeof(Yes) != sizeof(No), "'IsBaseType' internal error: invalid types used");

			GDINT static Yes Test(BaseTypePtr const* const);
			GDINT static No	 Test(...);
			GDINT static DerivedTypePtr const* GetDummyInstance();

		public:
			enum : bool { Value = (sizeof(Test(GetDummyInstance())) == sizeof(Yes)) };
#endif	// if (!defined(GD_DOCUMENTATION))
		};	// struct IsBaseType
	} // namespace TypeTraits
	
	/// Typedefs specified type if condition is true.
	template<  bool Condition,   typename CondType   = void  > struct EnableIf { };
#if (!defined(GD_DOCUMENTATION))
	template</*bool Condition,*/ typename CondType /*= void*/> struct EnableIf<true, CondType> { typedef CondType Type; };
#endif	// if (!defined(GD_DOCUMENTATION))

	/// Template type selecetor. Similar to trinary operator.
	template<  bool Condition,   typename TrueType, typename FalseType> struct Conditional { typedef TrueType Type; };
#if (!defined(GD_DOCUMENTATION))
	template</*bool Condition,*/ typename TrueType, typename FalseType> struct Conditional<false, TrueType, FalseType> { typedef FalseType Type; };
#endif	// if (!defined(GD_DOCUMENTATION))

GD_NAMESPACE_END

#endif
