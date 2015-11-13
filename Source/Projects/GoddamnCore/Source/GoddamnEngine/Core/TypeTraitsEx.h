// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/TypeTraits.h
//! Compile-time types information and modifiers management - extended edition.
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/UniquePtr.h>
#include <GoddamnEngine/Core/SharedPtr.h>
#include <GoddamnEngine/Core/TypeTraits.h>
#include <GoddamnEngine/Core/Containers/String.h>

#include <typeinfo>
#if GD_COMPILER_GCC_COMPATIBLE
#	include <cxxabi.h>	// for CXX ABI
#	include <memory>	// for std::free
#endif	// if GD_COMPILER_GCC_COMPATIBLE

GD_NAMESPACE_BEGIN

	//! @todo Implement this.
	template<typename Type, SizeTp PtrCapacity = 16>
	class InlinePtr {};
//	using InlinePtr = UniquePtr<Type>;
	template<typename Type>
	class ObjectPtr {};

	// ------------------------------------------------------------------------------------------
	//! Contains a brief list of the modifiers that can be added to the instance of some
	//! type. Note that type modifiers can stay unparsed depending on the build modifiers policy.
	enum TypeInfoModifier : Int32
	{
		TYPEINFO_MODIFIER_NONE				= 0,			//!< Type contains no modifiers.
		TYPEINFO_MODIFIER_CONST				= GD_BIT(0),	//!< Type contains @c const modifier.
		TYPEINFO_MODIFIER_VOLATILE			= GD_BIT(1),	//!< Type contains @c volatile modifier.
		TYPEINFO_MODIFIER_RESTRICT			= GD_BIT(2),	//!< Type contains @c restrict modifier.
		TYPEINFO_MODIFIER_LVALUE_REFERENCE	= GD_BIT(3),	//!< Type is l-value reference (<tt>&</tt>).
		TYPEINFO_MODIFIER_RVALUE_REFERENCE	= GD_BIT(4),	//!< Type is r-value reference (<tt>&&</tt>).
		TYPEINFO_MODIFIER_STATIC			= GD_BIT(5),	//!< Member was defined with @c static keyword.
		TYPEINFO_MODIFIER_VIRTUAL			= GD_BIT(6),	//!< Method was defined with @c virtual keyword.
	};	// enum TypeInfoModifierEnum : TypeInfoModifier

	// ------------------------------------------------------------------------------------------
	//! List of supported type originatives for a reflection type.
	enum TypeInfoOriginative : UInt8
	{
		TYPEINFO_ORIGINATIVE_UNDEFINED,						//!< Type originative in undefined.
		TYPEINFO_ORIGINATIVE_PRIMITIVE,						//!< Type originative is one of primitive types.
		TYPEINFO_ORIGINATIVE_ENUM,							//!< Type is a enumeration.
		TYPEINFO_ORIGINATIVE_POINTER,						//!< Type is a pointer.
		TYPEINFO_ORIGINATIVE_CLASS,							//!< Type is a class/struct.
	};	// enum TypeInfoOriginativeEnum : TypeInfoOriginative

	// ------------------------------------------------------------------------------------------
	//! List of supported type categories for a primitive type.
	enum TypeInfoPrimitiveCategory : UInt8
	{
		TYPEINFO_PRIMITIVE_CATEGORY_UNDEFINED,				//!< Undefined primitive type.
		TYPEINFO_PRIMITIVE_CATEGORY_VOID,					//!< @c void primitive type.
		TYPEINFO_PRIMITIVE_CATEGORY_BOOL,					//!< @c bool primitive type.
		TYPEINFO_PRIMITIVE_CATEGORY_CHAR,					//!< @c Char primitive type.
		TYPEINFO_PRIMITIVE_CATEGORY_WCHAR,					//!< @c WideChar primitive type.
		TYPEINFO_PRIMITIVE_CATEGORY_INT8,					//!< @c Int8 primitive type.
		TYPEINFO_PRIMITIVE_CATEGORY_UINT8,					//!< @c UInt8 primitive type.
/**@{*/	TYPEINFO_PRIMITIVE_CATEGORY_INT16,					//!< @c Int16 primitive type.
		TYPEINFO_PRIMITIVE_CATEGORY_SHORT					= TYPEINFO_PRIMITIVE_CATEGORY_INT16, /**@}*/
/**@{*/	TYPEINFO_PRIMITIVE_CATEGORY_UINT16,					//!< @c UInt16 primitive type.
		TYPEINFO_PRIMITIVE_CATEGORY_INT32,					//!< @c Int32 primitive type.
		TYPEINFO_PRIMITIVE_CATEGORY_INT						= TYPEINFO_PRIMITIVE_CATEGORY_INT32, /**@}*/
/**@{*/	TYPEINFO_PRIMITIVE_CATEGORY_UINT32,					//!< @c UInt32 primitive type.
		TYPEINFO_PRIMITIVE_CATEGORY_INT64,					//!< @c Int32 primitive type.
		TYPEINFO_PRIMITIVE_CATEGORY_LONG					= TYPEINFO_PRIMITIVE_CATEGORY_INT64, /**@}*/
		TYPEINFO_PRIMITIVE_CATEGORY_UINT64,					//!< @c UInt32 primitive type.
/**@{*/	TYPEINFO_PRIMITIVE_CATEGORY_FLOAT32,				//!< @c Float32 primitive type.
		TYPEINFO_PRIMITIVE_CATEGORY_FLOAT					= TYPEINFO_PRIMITIVE_CATEGORY_FLOAT32, /**@}*/
/**@{*/	TYPEINFO_PRIMITIVE_CATEGORY_FLOAT64,				//!< @c Float64 primitive type.
		TYPEINFO_PRIMITIVE_CATEGORY_DOUBLE					= TYPEINFO_PRIMITIVE_CATEGORY_FLOAT64, /**@}*/
	};	// enum class TypeInfoPrimitiveCategory

	// ------------------------------------------------------------------------------------------
	//! List of supported type categories for a pointer type.
	//! @{
	enum TypeInfoPointerCategory : UInt8
	{
		TYPEINFO_PTR_CATEGORY_UNDEFINED,					//!< Undefined pointer originative.
		TYPEINFO_PTR_CATEGORY_RAW,							//!< Raw pointer.
		TYPEINFO_PTR_CATEGORY_UNIQUE,						//!< Unique pointer (@ref UniquePtr class).
		TYPEINFO_PTR_CATEGORY_INLINE,						//!< Unique pointer (@ref InlinePtr class).
		TYPEINFO_PTR_CATEGORY_SHARED,						//!< Unique pointer (@ref SharedPtr class).
		TYPEINFO_PTR_CATEGORY_OBJECT,						//!< Unique pointer (@ref ObjectPtr class).
	};	// enum TypeInfoPointerCategoryEnum : TypeInfoPointerCategory
	//! @}

	// ------------------------------------------------------------------------------------------
	//! Additional type traits.
	namespace TypeTraitsEx
	{
		using TypeTraits::TypeTraitsBase;

		// ------------------------------------------------------------------------------------------
		//! Checks if specified type is a unique pointer.
		//! @{
		template<typename Type> 
		struct IsUniquePointer final : public TypeTraitsBase<false> {};
#if !GD_DOCUMENTATION
		template<typename Type> 
		struct IsUniquePointer<UniquePtr<Type>> final : public TypeTraitsBase<true>{};
#endif	// if !GD_DOCUMENTATION
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Checks if specified type is an inline pointer.
		//! @{
		template<typename Type>
		struct IsInlinePointer final : public TypeTraitsBase<false> {};
#if !GD_DOCUMENTATION
		template<typename Type, SizeTp PtrCapacity>
		struct IsInlinePointer<InlinePtr<Type, PtrCapacity>> final : public TypeTraitsBase<true>{};
#endif	// if !GD_DOCUMENTATION
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Checks if specified type is a shared pointer.
		//! @{
		template<typename Type>
		struct IsSharedPointer final : public TypeTraitsBase<false> {};
#if !GD_DOCUMENTATION
		template<typename Type, SharedPtrType PtrType> 
		struct IsSharedPointer<SharedPtr<Type, PtrType>> final : public TypeTraitsBase<true>{};
#endif	// if !GD_DOCUMENTATION
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Checks if specified type is an object pointer.
		//! @{
		template<typename Type>
		struct IsObjectPointer final : public TypeTraitsBase<false> {};
#if !GD_DOCUMENTATION
		template<typename Type>
		struct IsObjectPointer<ObjectPtr<Type>> final : public TypeTraitsBase<true> {};
#endif	// if !GD_DOCUMENTATION
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Calculates the name of the type in runtime.
		//! @tparam Tp Specified type.
		//! @returns The name of the type.
		template<typename Tp>
		GDINL static String const& GetTypeName()
		{
			typedef typename TypeTraits::RemoveCV<typename TypeTraits::RemoveReference<Tp>::Type>::Type Type;
#if GD_COMPILER_GCC_COMPATIBLE
			static String TypeName;
			if (TypeName.IsEmpty())
			{
				int ManglingStatus = 0;
				char const* const TypeNameMangled = typeid(Type).name();
				char const* const TypeNameDemangled = ::abi::__cxa_demangle(TypeNameMangled, nullptr, nullptr, &ManglingStatus);
				TypeName = (ManglingStatus != 0) && (TypeNameDemangled != nullptr) ? TypeNameDemangled : TypeNameMangled;
				std::free(TypeNameDemangled);
			}
#endif	// if GD_COMPILER_GCC_COMPATIBLE
#if GD_COMPILER_MSVC_COMPATIBLE
			static String const TypeName = typeid(Type).name();
			return TypeName;
#endif	// if GD_COMPILER_MSVC_COMPATIBLE
		}

		// ------------------------------------------------------------------------------------------
		//! Calculates modifiers of the type in compile time.
		//! @tparam Tp Specified type.
		//! @returns The modifiers bitset for specified type.
		template<typename Tp>
		GDINL static constexpr TypeInfoModifier GetTypeModifiers()
		{
			return TYPEINFO_MODIFIER_NONE
				| (TypeTraits::IsConst<Tp>::Value           ? TYPEINFO_MODIFIER_CONST            : 0)
				| (TypeTraits::IsVolatile<Tp>::Value        ? TYPEINFO_MODIFIER_VOLATILE         : 0)
				| (TypeTraits::IsLValueReference<Tp>::Value ? TYPEINFO_MODIFIER_LVALUE_REFERENCE : 0)
				| (TypeTraits::IsRValueReference<Tp>::Value ? TYPEINFO_MODIFIER_RVALUE_REFERENCE : 0);
		}

		// ------------------------------------------------------------------------------------------
		//! Calculates originative of the type in compile time.
		//! @tparam Tp Specified type.
		//! @returns The originative enum for specified type.
		template<typename Tp>
		GDINL static constexpr TypeInfoOriginative GetTypeOriginative()
		{
			typedef typename TypeTraits::RemoveCV<typename TypeTraits::RemoveReference<Tp>::Type>::Type Type;
			return (TypeTraits::IsCharacter<Type>::Value || TypeTraits::IsIntegral<Type>::Value || TypeTraits::IsFloatingPoint<Type>::Value
				|| TypeTraits::IsSame<Type, void>::Value || TypeTraits::IsSame<Type, bool>::Value) ? TYPEINFO_ORIGINATIVE_PRIMITIVE
					: (TypeTraitsEx::IsUniquePointer<Type>::Value || TypeTraitsEx::IsInlinePointer<Type>::Value 
					|| TypeTraitsEx::IsSharedPointer<Type>::Value || TypeTraitsEx::IsObjectPointer<Type>::Value
				|| TypeTraits::IsPointer<Type>::Value ? TYPEINFO_ORIGINATIVE_POINTER
				: (TypeTraits::IsEnum   <Type>::Value ? TYPEINFO_ORIGINATIVE_ENUM
				: (TypeTraits::IsClass  <Type>::Value ? TYPEINFO_ORIGINATIVE_CLASS : TYPEINFO_ORIGINATIVE_UNDEFINED)));
		}
	
		// ------------------------------------------------------------------------------------------
		//! Calculates category of the primitive type in compile time.
		//! @tparam Tp Specified type.
		//! @returns The category enum for specified primitive type.
		//! @{
		template<typename Tp
#if !GD_DOCUMENTATION
			, bool const PerformChecks = true
#endif	// if !GD_DOCUMENTATION
		> GDINL static constexpr TypeInfoPrimitiveCategory GetPrimitiveTypeCategory()
		{
			static_assert(GetTypeOriginative<Tp>() == TYPEINFO_ORIGINATIVE_PRIMITIVE || (!PerformChecks)
				, "'GetPrimitiveTypeCategory' error: template parameter 'Tp' should be one of the primitive types.");
			typedef typename TypeTraits::RemoveCV<typename TypeTraits::RemoveReference<Tp>::Type>::Type Type;
			return TypeTraits::IsSame<Type, void>::Value ? TYPEINFO_PRIMITIVE_CATEGORY_VOID
				: (TypeTraits::IsSame<Type, bool>::Value ? TYPEINFO_PRIMITIVE_CATEGORY_BOOL
				: (TypeTraits::IsCharacter<Type>::Value 
					? (TypeTraits::IsSame<Type, Char>::Value	 ? TYPEINFO_PRIMITIVE_CATEGORY_CHAR
					: (TypeTraits::IsSame<Type, WideChar>::Value ? TYPEINFO_PRIMITIVE_CATEGORY_WCHAR 
					: TYPEINFO_PRIMITIVE_CATEGORY_UNDEFINED))
				: (TypeTraits::IsIntegral<Type>::Value
					? (TypeTraits::IsSame<Type, Int8  >::Value   ? TYPEINFO_PRIMITIVE_CATEGORY_INT8
					: (TypeTraits::IsSame<Type, UInt8 >::Value   ? TYPEINFO_PRIMITIVE_CATEGORY_UINT8
					: (TypeTraits::IsSame<Type, Int16 >::Value   ? TYPEINFO_PRIMITIVE_CATEGORY_INT16
					: (TypeTraits::IsSame<Type, UInt16>::Value   ? TYPEINFO_PRIMITIVE_CATEGORY_UINT16
					: (TypeTraits::IsSame<Type, Int32 >::Value   ? TYPEINFO_PRIMITIVE_CATEGORY_INT32
					: (TypeTraits::IsSame<Type, UInt32>::Value   ? TYPEINFO_PRIMITIVE_CATEGORY_UINT32
					: (TypeTraits::IsSame<Type, Int64 >::Value   ? TYPEINFO_PRIMITIVE_CATEGORY_INT64
					: (TypeTraits::IsSame<Type, UInt64>::Value   ? TYPEINFO_PRIMITIVE_CATEGORY_UINT64
					: TYPEINFO_PRIMITIVE_CATEGORY_UNDEFINED))))))))
				: (TypeTraits::IsFloatingPoint<Type>::Value 
					? (TypeTraits::IsSame<Type, Float32>::Value  ? TYPEINFO_PRIMITIVE_CATEGORY_FLOAT32
					: (TypeTraits::IsSame<Type, Float64>::Value  ? TYPEINFO_PRIMITIVE_CATEGORY_FLOAT64
					: TYPEINFO_PRIMITIVE_CATEGORY_UNDEFINED))
				: TYPEINFO_PRIMITIVE_CATEGORY_UNDEFINED))));
		}
		template<typename Tp>
		GDINL static constexpr TypeInfoPrimitiveCategory GetPrimitiveTypeCategoryUnsafe()
		{
			return GetPrimitiveTypeCategory<Tp, false>();
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Calculates category of the pointer type in compile time.
		//! @tparam Tp Specified type.
		//! @returns The category enum for specified pointer type.
		//! @{
		template<typename Tp
#if !GD_DOCUMENTATION
			, bool const PerformChecks = true
#endif	// if !GD_DOCUMENTATION
		> 
		GDINL static constexpr TypeInfoPointerCategory GetPointerTypeCategory()
		{
			static_assert((GetTypeOriginative<Tp>() == TYPEINFO_ORIGINATIVE_POINTER) || (!PerformChecks)
				, "'GetPointerTypeCategory' error: template parameter 'Tp' should be one of the pointer types.");
			typedef typename TypeTraits::RemoveCV<typename TypeTraits::RemoveReference<Tp>::Type>::Type Type;
			return TypeTraits::IsPointer<Type>::Value ? TYPEINFO_PTR_CATEGORY_RAW
				: (TypeTraitsEx::IsUniquePointer<Type>::Value ? TYPEINFO_PTR_CATEGORY_UNIQUE
				: (TypeTraitsEx::IsInlinePointer<Type>::Value ? TYPEINFO_PTR_CATEGORY_INLINE
				: (TypeTraitsEx::IsSharedPointer<Type>::Value ? TYPEINFO_PTR_CATEGORY_SHARED
				: (TypeTraitsEx::IsObjectPointer<Type>::Value ? TYPEINFO_PTR_CATEGORY_OBJECT
				: TYPEINFO_PTR_CATEGORY_UNDEFINED))));
		}
		template<typename Tp>
		GDINL static constexpr TypeInfoPointerCategory GetPointerTypeCategoryUnsafe()
		{
			return GetPointerTypeCategory<Tp, false>();
		}
		//! @}
	}	// namespace TypeTraitsEx

GD_NAMESPACE_END
