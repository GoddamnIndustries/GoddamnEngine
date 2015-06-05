// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Object/Type.h
//! Common interface for runtime type information and reflection.
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/UniquePtr.h>
#include <GoddamnEngine/Core/Templates/SharedPtr.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Containers/Vector.h>
#include <GoddamnEngine/Core/Object/Variant.h>

#include <typeinfo>
#if GD_COMPILER_GCC_COMPATIBLE
#	include <cxxabi.h>
#endif	// if GD_COMPILER_GCC_COMPATIBLE

GD_NAMESPACE_BEGIN

	// ==========================================================================================
	// Book 0 - Prologue. "Predefinitions."
	// ==========================================================================================

	//! @todo Implement this.
	template<typename Type, SizeTp PtrCapacity = 16>
	class InlinePtr {};
//	using InlinePtr = UniquePtr<Type>;
	template<typename Type>
	class ObjectPtr {};
//	using ObjectPtr = SharedPtr<Type>;
#define HashMap Map
#define HashSet Set
	typedef String Symbol;

	class CtorInfo;
	class DtorInfo;
	class FieldInfo;
	class MethodInfo;

	// ------------------------------------------------------------------------------------------
	//! Spawns a RAII object that statically invokes specified code. Example:
	//! @code{.cpp}
	//!		GD_STATIC {
	//!			TypeInfoRegisterer::Register<MyClass>()
	//!             .DefineCtor<Int32>()
	//!             .DefineField(&MyClass::IntValue, "IntValue")
	//!             ...;
	//!		}
	//! @endcode
	//! @{
#define GD_STATIC static StaticCtorExecutor const GD_GLUE(StaticCtor, __LINE__) = [&]() -> void
#if !GD_DOCUMENTATION
	class StaticCtorExecutor
	{
		template<typename Tp>
		GDINL /*explicit*/ StaticCtorExecutor(Tp const& StaticCtor) { StaticCtor(); }
	};	// class StaticCtorExecutor
#endif	// if !GD_DOCUMENTATION
	//! @}

	// ==========================================================================================
	// Book 1. "Main classes."
	// ==========================================================================================

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Introduction. "Additional type traits and etc."
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Contains a brief list of the modifiers that can be added to the instance of some
	//! type. Note that type modifiers can stay unparsed depending on the build modifiers policy.
	//! @{
	typedef Int32 TypeInfoModifier;
	enum TypeInfoModifierEnum : TypeInfoModifier
	{
		GD_TYPEINFO_MODIFIER_NONE				= 0,			//!< Type contains no modifiers.
		GD_TYPEINFO_MODIFIER_CONST				= GD_BIT(0),	//!< Type contains @c const modifier.
		GD_TYPEINFO_MODIFIER_VOLATILE			= GD_BIT(1),	//!< Type contains @c volatile modifier.
		GD_TYPEINFO_MODIFIER_RESTRICT			= GD_BIT(2),	//!< Type contains @c restrict modifier.
		GD_TYPEINFO_MODIFIER_LVALUE_REFERENCE	= GD_BIT(3),	//!< Type is l-value reference (<tt>&</tt>).
		GD_TYPEINFO_MODIFIER_RVALUE_REFERENCE	= GD_BIT(4),	//!< Type is r-value reference (<tt>&&</tt>).
		GD_TYPEINFO_MODIFIER_STATIC				= GD_BIT(5),	//!< Member was defined with @c static keyword.
		GD_TYPEINFO_MODIFIER_VIRTUAL			= GD_BIT(6),	//!< Method was defined with @c virtual keyword.
	};	// enum TypeInfoModifierEnum : TypeInfoModifier
	//! @}

	// ------------------------------------------------------------------------------------------
	//! List of supported type originatives for a reflection type.
	//! @{
	typedef UInt8 TypeInfoOriginative;
	enum TypeInfoOriginativeEnum : TypeInfoOriginative
	{
		GD_TYPEINFO_ORIGINATIVE_UNDEFINED,						//!< Type originative in undefined.
		GD_TYPEINFO_ORIGINATIVE_PRIMITIVE,						//!< Type originative is one of primitive types.
		GD_TYPEINFO_ORIGINATIVE_ENUM,							//!< Type is a enumeration.
		GD_TYPEINFO_ORIGINATIVE_POINTER,						//!< Type is a pointer.
		GD_TYPEINFO_ORIGINATIVE_CLASS,							//!< Type is a class/struct.
	};	// enum TypeInfoOriginativeEnum : TypeInfoOriginative
	//! @}

	// ------------------------------------------------------------------------------------------
	//! List of supported type categories for a primitive type.
	//! @{
	typedef UInt8 TypeInfoPrimitiveCategory;
	enum TypeInfoPrimitiveCategoryEnum : TypeInfoPrimitiveCategory
	{
		GD_TYPEINFO_PRIMITIVE_CATEGORY_UNDEFINED,				//!< Undefined primitive type.
		GD_TYPEINFO_PRIMITIVE_CATEGORY_VOID,					//!< @c void primitive type.
		GD_TYPEINFO_PRIMITIVE_CATEGORY_BOOL,					//!< @c bool primitive type.
		GD_TYPEINFO_PRIMITIVE_CATEGORY_CHAR,					//!< @c Char primitive type.
		GD_TYPEINFO_PRIMITIVE_CATEGORY_WCHAR,					//!< @c WideChar primitive type.
		GD_TYPEINFO_PRIMITIVE_CATEGORY_INT8,					//!< @c Int8 primitive type.
		GD_TYPEINFO_PRIMITIVE_CATEGORY_UINT8,					//!< @c UInt8 primitive type.
/**@{*/	GD_TYPEINFO_PRIMITIVE_CATEGORY_INT16,					//!< @c Int16 primitive type.
		GD_TYPEINFO_PRIMITIVE_CATEGORY_SHORT					= GD_TYPEINFO_PRIMITIVE_CATEGORY_INT16, /**@}*/
/**@{*/	GD_TYPEINFO_PRIMITIVE_CATEGORY_UINT16,					//!< @c UInt16 primitive type.
		GD_TYPEINFO_PRIMITIVE_CATEGORY_INT32,					//!< @c Int32 primitive type.
		GD_TYPEINFO_PRIMITIVE_CATEGORY_INT						= GD_TYPEINFO_PRIMITIVE_CATEGORY_INT32, /**@}*/
/**@{*/	GD_TYPEINFO_PRIMITIVE_CATEGORY_UINT32,					//!< @c UInt32 primitive type.
		GD_TYPEINFO_PRIMITIVE_CATEGORY_INT64,					//!< @c Int32 primitive type.
		GD_TYPEINFO_PRIMITIVE_CATEGORY_LONG						= GD_TYPEINFO_PRIMITIVE_CATEGORY_INT64, /**@}*/
		GD_TYPEINFO_PRIMITIVE_CATEGORY_UINT64,					//!< @c UInt32 primitive type.
/**@{*/	GD_TYPEINFO_PRIMITIVE_CATEGORY_FLOAT32,					//!< @c Float32 primitive type.
		GD_TYPEINFO_PRIMITIVE_CATEGORY_FLOAT					= GD_TYPEINFO_PRIMITIVE_CATEGORY_FLOAT32, /**@}*/
/**@{*/	GD_TYPEINFO_PRIMITIVE_CATEGORY_FLOAT64,					//!< @c Float64 primitive type.
		GD_TYPEINFO_PRIMITIVE_CATEGORY_DOUBLE					= GD_TYPEINFO_PRIMITIVE_CATEGORY_FLOAT64, /**@}*/
	};	// enum class TypeInfoPrimitiveCategory
	//! @}

	// ------------------------------------------------------------------------------------------
	//! List of supported type categories for a pointer type.
	//! @{
	typedef UInt8 TypeInfoPointerCategory;
	enum TypeInfoPointerCategoryEnum : TypeInfoPointerCategory
	{
		GD_TYPEINFO_PTR_CATEGORY_UNDEFINED,						//!< Undefined pointer originative.
		GD_TYPEINFO_PTR_CATEGORY_RAW,							//!< Raw pointer.
		GD_TYPEINFO_PTR_CATEGORY_UNIQUE,						//!< Unique pointer (@ref UniquePtr class).
		GD_TYPEINFO_PTR_CATEGORY_INLINE,						//!< Unique pointer (@ref InlinePtr class).
		GD_TYPEINFO_PTR_CATEGORY_SHARED,						//!< Unique pointer (@ref SharedPtr class).
		GD_TYPEINFO_PTR_CATEGORY_OBJECT,						//!< Unique pointer (@ref ObjectPtr class).
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
			return GD_TYPEINFO_MODIFIER_NONE
				| (TypeTraits::IsConst<Tp>::Value           ? GD_TYPEINFO_MODIFIER_CONST            : 0)
				| (TypeTraits::IsVolatile<Tp>::Value        ? GD_TYPEINFO_MODIFIER_VOLATILE         : 0)
				| (TypeTraits::IsLValueReference<Tp>::Value ? GD_TYPEINFO_MODIFIER_LVALUE_REFERENCE : 0)
				| (TypeTraits::IsRValueReference<Tp>::Value ? GD_TYPEINFO_MODIFIER_RVALUE_REFERENCE : 0);
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
				|| TypeTraits::IsSame<Type, void>::Value || TypeTraits::IsSame<Type, bool>::Value) ? GD_TYPEINFO_ORIGINATIVE_PRIMITIVE
					: (TypeTraitsEx::IsUniquePointer<Type>::Value || TypeTraitsEx::IsInlinePointer<Type>::Value 
					|| TypeTraitsEx::IsSharedPointer<Type>::Value || TypeTraitsEx::IsObjectPointer<Type>::Value
				|| TypeTraits::IsPointer<Type>::Value ? GD_TYPEINFO_ORIGINATIVE_POINTER
				: (TypeTraits::IsEnum   <Type>::Value ? GD_TYPEINFO_ORIGINATIVE_ENUM
				: (TypeTraits::IsClass  <Type>::Value ? GD_TYPEINFO_ORIGINATIVE_CLASS : GD_TYPEINFO_ORIGINATIVE_UNDEFINED)));
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
			static_assert(GetTypeOriginative<Tp>() == GD_TYPEINFO_ORIGINATIVE_PRIMITIVE || (!PerformChecks)
				, "'GetPrimitiveTypeCategory' error: template parameter 'Tp' should be one of the primitive types.");
			typedef typename TypeTraits::RemoveCV<typename TypeTraits::RemoveReference<Tp>::Type>::Type Type;
			return TypeTraits::IsSame<Type, void>::Value ? GD_TYPEINFO_PRIMITIVE_CATEGORY_VOID
				: (TypeTraits::IsSame<Type, bool>::Value ? GD_TYPEINFO_PRIMITIVE_CATEGORY_BOOL
				: (TypeTraits::IsCharacter<Type>::Value 
					? (TypeTraits::IsSame<Type, Char>::Value	 ? GD_TYPEINFO_PRIMITIVE_CATEGORY_CHAR
					: (TypeTraits::IsSame<Type, WideChar>::Value ? GD_TYPEINFO_PRIMITIVE_CATEGORY_WCHAR 
					: GD_TYPEINFO_PRIMITIVE_CATEGORY_UNDEFINED))
				: (TypeTraits::IsIntegral<Type>::Value
					? (TypeTraits::IsSame<Type, Int8  >::Value   ? GD_TYPEINFO_PRIMITIVE_CATEGORY_INT8
					: (TypeTraits::IsSame<Type, UInt8 >::Value   ? GD_TYPEINFO_PRIMITIVE_CATEGORY_UINT8
					: (TypeTraits::IsSame<Type, Int16 >::Value   ? GD_TYPEINFO_PRIMITIVE_CATEGORY_INT16
					: (TypeTraits::IsSame<Type, UInt16>::Value   ? GD_TYPEINFO_PRIMITIVE_CATEGORY_UINT16
					: (TypeTraits::IsSame<Type, Int32 >::Value   ? GD_TYPEINFO_PRIMITIVE_CATEGORY_INT32
					: (TypeTraits::IsSame<Type, UInt32>::Value   ? GD_TYPEINFO_PRIMITIVE_CATEGORY_UINT32
					: (TypeTraits::IsSame<Type, Int64 >::Value   ? GD_TYPEINFO_PRIMITIVE_CATEGORY_INT64
					: (TypeTraits::IsSame<Type, UInt64>::Value   ? GD_TYPEINFO_PRIMITIVE_CATEGORY_UINT64
					: GD_TYPEINFO_PRIMITIVE_CATEGORY_UNDEFINED))))))))
				: (TypeTraits::IsFloatingPoint<Type>::Value 
					? (TypeTraits::IsSame<Type, Float32>::Value  ? GD_TYPEINFO_PRIMITIVE_CATEGORY_FLOAT32
					: (TypeTraits::IsSame<Type, Float64>::Value  ? GD_TYPEINFO_PRIMITIVE_CATEGORY_FLOAT64
					: GD_TYPEINFO_PRIMITIVE_CATEGORY_UNDEFINED))
				: GD_TYPEINFO_PRIMITIVE_CATEGORY_UNDEFINED))));
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
		> GDINL static constexpr TypeInfoPrimitiveCategory GetPointerTypeCategory()
		{
			static_assert((GetTypeOriginative<Tp>() == GD_TYPEINFO_ORIGINATIVE_POINTER) || (!PerformChecks)
				, "'GetPointerTypeCategory' error: template parameter 'Tp' should be one of the pointer types.");
			typedef typename TypeTraits::RemoveCV<typename TypeTraits::RemoveReference<Tp>::Type>::Type Type;
			return TypeTraits::IsPointer<Type>::Value ? GD_TYPEINFO_PTR_CATEGORY_RAW
				: (TypeTraitsEx::IsUniquePointer<Type>::Value ? GD_TYPEINFO_PTR_CATEGORY_UNIQUE
				: (TypeTraitsEx::IsInlinePointer<Type>::Value ? GD_TYPEINFO_PTR_CATEGORY_INLINE
				: (TypeTraitsEx::IsSharedPointer<Type>::Value ? GD_TYPEINFO_PTR_CATEGORY_SHARED
				: (TypeTraitsEx::IsObjectPointer<Type>::Value ? GD_TYPEINFO_PTR_CATEGORY_OBJECT
				: GD_TYPEINFO_PTR_CATEGORY_UNDEFINED))));
		}
		template<typename Tp>
		GDINL static constexpr TypeInfoPrimitiveCategory GetPointerTypeCategoryUnsafe()
		{
			return GetPointerTypeCategory<Tp, false>();
		}
		//! @}
	}	// namespace TypeTraitsEx

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Chapter 1. TypeInfo class.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Contains brief type information for any arbitrary object.
	//!        
	//! Common
	//! ----------------------
	//! Type information can be divided into several categories be the originatives of the types: 
	//! primitive types, enumerations, pointer types, structures and classes.
	//! To operate with the instances of the @ref TypeInfo class use @ref TypeInfoPtr.
	//! It holds pointer to the type information, but also contains wrapped comparison
	//! operations.
	//!        
	//! Meta information
	//! ----------------------
	//! Each type information can also contain user-defined meta information, especially
	//! enumerations, classes and structures. Meta information contains super class, the 
	//! list of available constructors, destructor, fields and methods for classes and
	//! structures and enumeration constants for enumerations.
	//!           
	//! @remark All top-level CV qualifiers and references would be removed.
	//! @sa TypeInfoPtr, CtorInfo, DtorInfo, FieldInfo, MethodInfo.
	//! @{
	typedef class TypeInfo const* TypeInfoPtr;
	typedef Vector<TypeInfoPtr> TypeInfoPtrArray;
	class TypeInfo final : public IUncopiable
	{
	private:
		template<typename>
		friend class UniquePtr;
		friend class Allocator;
		template<typename>
		friend class TypeInfoRegisterer;

	private:
		SizeTp									TypeSize;
		Symbol									TypeName;
		Symbol									TypeSimpleName;
		Symbol									TypeNamespaceName;
		TypeInfoOriginative						TypeOriginative;
		union {
			struct {
				TypeInfoPrimitiveCategory		TypePrimitiveCategory;
			};	// anonymous struct
			struct {
				TypeInfoPtr						TypePointerDereferencedType;
				TypeInfoPointerCategory			TypePointerCategory;
			};	// anonymous struct
			struct {
				TypeInfoPtr						TypeEnumUnderlyingType;
				Vector<UInt64>					TypeEnumConstants;
				Vector<Symbol>					TypeEnumNames;
			};	// anonymous struct
			struct {
				TypeInfoPtr						TypeClassBaseType;
				Vector<UniquePtr<CtorInfo>>		TypeClassCtorsList;
				Vector<UniquePtr<DtorInfo>>		TypeClassDtor;
				Vector<UniquePtr<FieldInfo>>	TypeClassFieldsList;
				Vector<UniquePtr<MethodInfo>>	TypeClassMethodsList;
			};	// anonymous struct
		};	// anonymous union
		
	private:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructors and destructor.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Initializes a new type information for some type and registers it in global table.
		//! @param TypeName Raw name of the type.
		//! @param TypeSize Size of the type in bytes.
		//! @param TypeOriginative The originative of the type.
		GDAPI TypeInfo(String const& TypeName, SizeTp const	TypeSize, TypeInfoOriginative const TypeOriginative);

		// ------------------------------------------------------------------------------------------
		//! Initializes a new type information for some type and registers it in global table.
		//! @param Unused Dummy parameter to specialize template parameter @ref Tp.
		//! @tparam Tp Type for one this type information is going to be generated.
		template<typename Tp>
		GDAPI explicit TypeInfo(Dummy<Tp> const Unused)
			: TypeInfo(TypeTraitsEx::GetTypeName<Tp>(), sizeof(Tp), TypeTraitsEx::GetTypeOriginative<Tp>())
		{
			GD_NOT_USED(Unused);
			typedef typename TypeTraits::RemoveCV<typename TypeTraits::RemoveReference<Tp>::Type>::Type Type;
			static_assert(TypeTraitsEx::GetTypeOriginative<Tp>() != GD_TYPEINFO_ORIGINATIVE_UNDEFINED
				, "'TypeInfo::TypeInfo<T>' error: the undefined originative was selected for specified template parameter Tp.");
			switch (TypeTraitsEx::GetTypeOriginative<Tp>())
			{
				case GD_TYPEINFO_ORIGINATIVE_PRIMITIVE:
					this->TypePrimitiveCategory = TypeTraitsEx::GetPrimitiveTypeCategoryUnsafe<Type>();
					break;
				case GD_TYPEINFO_ORIGINATIVE_ENUM:
					this->TypeEnumUnderlyingType = TypeInfo::ForType<typename TypeTraits::Underlying<Type>::Type>();
					break;
				case GD_TYPEINFO_ORIGINATIVE_POINTER:
					this->TypePointerCategory = TypeTraitsEx::GetPointerTypeCategoryUnsafe<Type>();
					this->TypePointerDereferencedType = TypeInfo::ForType<typename TypeTraits::RemovePointer<Type>::Type>();
					break;
			}
		}

		// ------------------------------------------------------------------------------------------
		//! Deinitializes a type information.
		GDAPI ~TypeInfo();

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Global types database access.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Retrieves information for type with specified name, if such type was registered or
		//! null pointer otherwise.
		//! @param TypeName Name of the desired type. Please, read more about naming convection.
		//! @returns The type information for type with specified name or null pointer.
		//! @warning Each custom type initially resolved though @ref ForType function. 
		GDAPI static TypeInfoPtr ForName(String const& TypeName);

		// ------------------------------------------------------------------------------------------
		//! Retrieves information for specified types. all top-level CV qualifiers and references
		//! are being ignored.
		//! @tparam Tp The type name for one we are resolving type information.
		//! @returns The type information for specified types.
		//! @{
		template<typename Tp>
		GDINL static TypeInfoPtr ForType()
		{
			typedef typename TypeTraits::RemoveCV<typename TypeTraits::RemoveReference<Tp>::Type>::Type Type;
			static TypeInfoPtr RetrievedTypeInfo = TypeInfo::ForName(TypeTraitsEx::GetTypeName<Type>());
			if (RetrievedTypeInfo == nullptr)
				RetrievedTypeInfo = GD_NEW(TypeInfo, Dummy<Type>());
			return RetrievedTypeInfo;
		}
		template<typename... Tp>
		GDINL static TypeInfoPtrArray const& ForTypes()
		{
			static TypeInfoPtrArray const RetrievedTypeInfos({ TypeInfo::ForType<Tp>... });
			return RetrievedTypeInfos;
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Retrieves information strictly for specified type. As far as all top-level CV 
		//! qualifiers or references ("CVR") would be removed when type information is generated,
		//! now type information for types with CV-s or references exist, this function throw
		//! compilation error when such type is specified.
		//! @tparam Tp The type name for one we are resolving type information.
		//! @returns The type information strictly for specified type.
		template<typename Tp>
		GDINL static TypeInfoPtr ForTypeStrictly()
		{
			static_assert((!TypeTraits::IsConst<Tp>::Value) && (!TypeTraits::IsVolatile<Tp>::Value) 
				&& (!TypeTraits::IsLValueReference<Tp>::Value) && (!TypeTraits::IsRValueReference<Tp>::Value), 
				"'ForTypeStrictly' error: specified type contains CVR-qualifiers and has no type information generated.");
			return TypeInfo::ForType<Tp>();
		}
		
		// ------------------------------------------------------------------------------------------
		//! Retrieves information for type of the specified expression. Is a direct equivalent
		//! of a @ref ForType function for all ordinary types.
		//! @param Expression The expression for one we are resolving type information. 
		//! @returns The type information for type of the specified expression.
		template<typename Tp>
		GDINL static TypeInfoPtr ForExpression(Tp const& Expression)
		{
			GD_NOT_USED(Expression);
			typedef typename TypeTraits::RemoveCV<typename TypeTraits::RemoveReference<Tp>::Type>::Type Type;
			return TypeInfo::ForType<Type>();
		}
	};	// class TypeInfo
	//! @}


	

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Chapter 2. CtorInfo class.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Contains brief type information for any arbitrary constructor of a class type.
	class CtorInfo final : public IUncopiable
	{
	private:
		Vector<TypeInfoPtr> /*const&*/					CtorArgumentTypes;


	public:
		GDINL  Vector<TypeInfoPtr> const& GetCtorArgumentTypes() const { return this->CtorArgumentTypes; }
	};	// class CtorInfo

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Chapter 3. DtorInfo class.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	class DtorInfo final : public IUncopiable
	{
	
	};	// class DtorInfo

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Chapter 4. FieldInfo class.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Represents information about single field in class or struct type.
	class FieldInfo final : public IUncopiable
	{
	public:
		template<typename, typename>
		friend struct						TypeInfoTypeInfoResolver;
		friend struct						TypeInfoMetaInfoRegisterer;

	private:
		String								FieldName;
		TypeInfoPtr							FieldType;
		TypeInfoPtr							FieldOwningType;
		TypeInfoModifier					FieldModifiers;

	public:

		GDINL explicit FieldInfo(...) {}
		GDINL ~FieldInfo() {}

	public:
		GDINL String const& GetFieldName() const { return this->FieldName; }
	};	// class FieldInfo

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Chapter 5. MethodInfo class.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	class MethodInfo final : public IUncopiable
	{
		template<typename, typename>
		friend struct TypeInfoTypeInfoResolver;
		friend class TypeInfo;

	private:
		String								MethodName;
		TypeInfoModifier					MethodModifier;
		TypeInfoPtr							MethodReturnType;
		Vector<TypeInfoPtr>	/*const&*/			MethodArgumentTypes;
		bool								MethodHasVariableArguments;

	public:

		GDINL explicit MethodInfo(...) {}
		GDINL ~MethodInfo() {}

	public:
		GDINL String const& GetMethodName() const { return this->MethodName; }
		GDINL  Vector<TypeInfoPtr> const& GetMethodArgumentTypes() const { return this->MethodArgumentTypes; }
	};	// class MethodInfo

	// ==========================================================================================
	// Book 2. "Meta data registration API."
	// ==========================================================================================

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// TypeInfoMetaInfoRegisterer struct.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	template<typename Tp>
	class TypeInfoRegisterer
	{
		TypeInfo& TypeInformation;
	public:

		// ------------------------------------------------------------------------------------------
		//! Registers base type for a class type.
		//! @tparam BaseTp Specified super class for a class type.
		//! @returns Registerer class instance.
		template<typename BaseTp>
		GDINL TypeInfoRegisterer& Extends()
		{
			static_assert(TypeTraits::IsBase<BaseTp, Tp>::Value
				, "'TypeInfoRegisterer::Extends<T>' error: specified template parameter is not a super class.");
			static_assert((reinterpret_cast<int*>(static_cast<Tp*>(nullptr)) - reinterpret_cast<int*>(static_cast<BaseTp*>(nullptr))) == 0
				, "'TypeInfoRegisterer::Extends<T>' error: specified template parameter is not a first super class.");

			this.TypeInformation->TypeClassBaseType = TypeInfo::ForTypeStrictly<BaseTp>();
			return *this;
		}

		// ------------------------------------------------------------------------------------------
		template<typename... ArgumentTypesTp>
		GDINL TypeInfoRegisterer& Ctor()
		{
			return *this;
		}

		// ------------------------------------------------------------------------------------------
		GDINL TypeInfoRegisterer& Dtor()
		{
			return *this;
		}

		// ------------------------------------------------------------------------------------------
		template<typename FieldClassTypeTp, typename FieldTypeTp>
		GDINL TypeInfoRegisterer& Field(FieldTypeTp FieldClassTypeTp::* const FieldPtr, String const& FieldName)
		{
			return *this;
		}

		// ------------------------------------------------------------------------------------------
		//! Registers a method inside class meta information.
		//! @param MethodPtr Pointer to member function.
		//! @param MethodName Name of the member function being registered.
		//! @{
		template<typename MethodReturnTypeTp, typename... MethodArgumentTypesTp>
		GDINL TypeInfoRegisterer& Method(MethodReturnTypeTp(*const MethodPtr)(MethodArgumentTypesTp...), String const& MethodName)
		{
			return *this;
		}	
		template<typename MethodClassTypeTp, typename MethodReturnTypeTp, typename... MethodArgumentTypesTp>
		GDINL TypeInfoRegisterer& Method(MethodReturnTypeTp(MethodClassTypeTp::* const MethodPtr)(MethodArgumentTypesTp...), String const& MethodName)
		{
		//	HashCode const MethodNameHash = MethodName.GetHashCode();
		//	if (!this->TypeMetaInfo->TypeClassMethods.ContainsElementWithKey(MethodNameHash))
		//		this->TypeMetaInfo->TypeClassMethods.InsertKeyValue(MethodNameHash, GD_NEW(MethodInfo
		//		, MethodName, MethodPtr
		//		, TypeInfo::RetrieveForType<MethodReturnTypeTp>()
		//		, TypeInfo::RetrieveForTypesTuple<MethodArgumentTypesTp...>()));
			return *this;
		}
		//! @}
	};	// class TypeInfoMetaInfoRegisterer

GD_NAMESPACE_END

#include <GoddamnEngine/Core/Object/BuiltinTypes.h>
